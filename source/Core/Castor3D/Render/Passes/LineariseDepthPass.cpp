#include "Castor3D/Render/Passes/LineariseDepthPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Viewport.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Render/Ssao/SsaoConfig.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Shader/GlslToSpv.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/SsaoConfigUbo.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Sync/Queue.hpp>
#include <ashespp/Core/Device.hpp>
#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/Miscellaneous/RendererFeatures.hpp>
#include <ashespp/Pipeline/GraphicsPipeline.hpp>
#include <ashespp/Pipeline/GraphicsPipelineCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>
#include <ashespp/Pipeline/PipelineVertexInputStateCreateInfo.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

#include <RenderGraph/FrameGraph.hpp>
#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <random>

CU_ImplementCUSmartPtr( castor3d, LineariseDepthPass )

using namespace castor;

namespace castor3d
{
	namespace
	{
		static uint32_t constexpr DepthImgIdx = 0u;
		static uint32_t constexpr ClipInfoUboIdx = 1u;
		static uint32_t constexpr PrevLvlUboIdx = 1u;

		ShaderPtr getVertexProgram()
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			auto position = writer.declInput< Vec2 >( "position", 0u );

			writer.implementMainT< VoidT, VoidT >( [&]( VertexIn in
				, VertexOut out )
				{
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
		
		ShaderPtr getLinearisePixelProgram()
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			Ubo clipInfo{ writer, "ClipInfo", ClipInfoUboIdx, 0u, ast::type::MemoryLayout::eStd140 };
			auto c3d_clipInfo = clipInfo.declMember< Vec3 >( "c3d_clipInfo" );
			clipInfo.end();
			auto c3d_mapDepth = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapDepth", DepthImgIdx, 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Float >( "pxl_fragColor", 0u );

			auto reconstructCSZ = writer.implementFunction< Float >( "reconstructCSZ"
				, [&]( Float const & depth
					, Vec3 const & clipInfo )
				{
					writer.returnStmt( clipInfo[0] / ( clipInfo[1] * depth + clipInfo[2] ) );
				}
				, InFloat{ writer, "d" }
				, InVec3{ writer, "clipInfo" } );

			writer.implementMainT< VoidT, VoidT >( [&]( FragmentIn in
				, FragmentOut out )
				{
					pxl_fragColor = reconstructCSZ( c3d_mapDepth.fetch( ivec2( in.fragCoord.xy() ), 0_i ).r()
						, c3d_clipInfo );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
		
		ShaderPtr getMinifyPixelProgram()
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			Ubo previousLevel{ writer, "PreviousLevel", PrevLvlUboIdx, 0u, ast::type::MemoryLayout::eStd140 };
			auto c3d_textureSize = previousLevel.declMember< IVec2 >( "c3d_textureSize" );
			previousLevel.end();
			auto c3d_mapDepth = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapDepth", DepthImgIdx, 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Float >( "pxl_fragColor", 0u );

			writer.implementMainT< VoidT, VoidT >( [&]( FragmentIn in
				, FragmentOut out )
				{
					auto ssPosition = writer.declLocale( "ssPosition"
						, ivec2( in.fragCoord.xy() ) );

					// Rotated grid subsampling to avoid XY directional bias or Z precision bias while downsampling.
					pxl_fragColor = c3d_mapDepth.fetch( clamp( ssPosition * 2 + ivec2( ssPosition.y() & 1, ssPosition.x() & 1 )
							, ivec2( 0_i, 0_i )
							, c3d_textureSize - ivec2( 1_i, 1_i ) )
						, 0_i ).r();
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		Texture doCreateTexture( RenderDevice const & device
			, crg::ResourceHandler & handler
			, VkExtent2D const & size
			, castor::String const & prefix )
		{
			return Texture{ device
				, handler
				, prefix + cuT( "LinearisedDepth" )
				, 0u
				, { size.width, size.height, 1u }
				, 1u
				, LineariseDepthPass::MaxMipLevel + 1u
				, VK_FORMAT_R32_SFLOAT
				, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
					| VK_IMAGE_USAGE_SAMPLED_BIT
					| VK_IMAGE_USAGE_TRANSFER_DST_BIT
					| VK_IMAGE_USAGE_TRANSFER_SRC_BIT ) };
		}
	}

	//*********************************************************************************************

	LineariseDepthPass::LineariseDepthPass( crg::FramePassGroup & graph
		, crg::FramePass const & previousPass
		, RenderDevice const & device
		, ProgressBar * progress
		, String const & prefix
		, SsaoConfig const & ssaoConfig
		, VkExtent2D const & size
		, crg::ImageViewId const & depthBuffer )
		: m_device{ device }
		, m_graph{ graph }
		, m_engine{ *m_device.renderSystem.getEngine() }
		, m_ssaoConfig{ ssaoConfig }
		, m_srcDepthBuffer{ depthBuffer }
		, m_prefix{ graph.name + prefix }
		, m_size{ size }
		, m_result{ doCreateTexture( m_device, m_graph.getHandler(), m_size, m_prefix ) }
		, m_clipInfo{ m_device.uboPools->getBuffer< Point3f >( 0u ) }
		, m_lastPass{ &previousPass }
		, m_lineariseVertexShader{ VK_SHADER_STAGE_VERTEX_BIT, m_prefix + "LineariseDepth", getVertexProgram() }
		, m_linearisePixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, m_prefix + "LineariseDepth", getLinearisePixelProgram() }
		, m_lineariseStages{ makeShaderState( m_device, m_lineariseVertexShader )
			, makeShaderState( m_device, m_linearisePixelShader ) }
		, m_linearisePass{ doInitialiseLinearisePass( progress ) }
		, m_minifyVertexShader{ VK_SHADER_STAGE_VERTEX_BIT, m_prefix + "MinifyDepth", getVertexProgram() }
		, m_minifyPixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, m_prefix + "MinifyDepth", getMinifyPixelProgram() }
		, m_minifyStages{ makeShaderState( m_device, m_minifyVertexShader )
			, makeShaderState( m_device, m_minifyPixelShader ) }
	{
		doInitialiseMinifyPass( progress );
	}

	LineariseDepthPass::~LineariseDepthPass()
	{
		for ( auto & level : m_previousLevel )
		{
			m_device.uboPools->putBuffer( level );
		}

		if ( m_clipInfo )
		{
			m_device.uboPools->putBuffer( m_clipInfo );
		}

		m_result.destroy();
	}

	void LineariseDepthPass::update( CpuUpdater & updater )
	{
		auto & viewport = updater.camera->getViewport();
		auto z_f = viewport.getFar();
		auto z_n = viewport.getNear();
		auto clipInfo = std::isinf( z_f )
			? Point3f{ z_n, -1.0f, 1.0f }
			: Point3f{ z_n * z_f, z_n - z_f, z_f };
		// result = clipInfo[0] / ( clipInfo[1] * depth + clipInfo[2] );
		// depth = 0 => result = z_n
		// depth = 1 => result = z_f
		m_clipInfoValue = clipInfo;

		if ( m_clipInfoValue.isDirty() )
		{
			m_clipInfo.getData() = m_clipInfoValue;
			m_clipInfoValue.reset();
		}
	}

	void LineariseDepthPass::accept( PipelineVisitorBase & visitor )
	{
		uint32_t index = 0u;

		for ( auto & layer : getResult().subViewsId )
		{
			visitor.visit( "Linearised Depth " + string::toString( index++ )
				, layer
				, m_graph.getFinalLayoutState( layer ).layout
				, TextureFactors{}.invert( true ) );
		}

		visitor.visit( m_lineariseVertexShader );
		visitor.visit( m_linearisePixelShader );

		visitor.visit( m_minifyVertexShader );
		visitor.visit( m_minifyPixelShader );
	}

	crg::FramePass const & LineariseDepthPass::doInitialiseLinearisePass( ProgressBar * progress )
	{
		stepProgressBar( progress, "Creating depth linearise pass" );
		auto & pass = m_graph.createPass( "LineariseDepth"
			, [this, progress]( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				stepProgressBar( progress, "Initialising depth linearise pass" );
				auto result = crg::RenderQuadBuilder{}
					.program( crg::makeVkArray< VkPipelineShaderStageCreateInfo >( m_lineariseStages ) )
					.renderSize( m_size )
					.enabled( &m_ssaoConfig.enabled )
					.build( pass, context, graph );
				m_device.renderSystem.getEngine()->registerTimer( graph.getName() + "/SSAO"
					, result->getTimer() );
				return result;
			} );
		pass.addDependency( *m_lastPass );
		pass.addSampledView( m_srcDepthBuffer, DepthImgIdx );
		m_clipInfo.createPassBinding( pass, "ClipInfoCfg", ClipInfoUboIdx );
		pass.addOutputColourView( m_result.targetViewId );
		m_result.create();
		m_lastPass = &pass;
		return pass;
	}

	void LineariseDepthPass::doInitialiseMinifyPass( ProgressBar * progress )
	{
		auto size = m_size;

		for ( auto index = 0u; index < MaxMipLevel; ++index )
		{
			stepProgressBar( progress, "Creating depth minify pass " + std::to_string( index ) );
			m_previousLevel.push_back( m_device.uboPools->getBuffer< castor::Point2i >( 0u ) );
			auto & previousLevel = m_previousLevel.back();
			auto & data = previousLevel.getData();
			data = Point2i{ size.width, size.height };
			size.width >>= 1;
			size.height >>= 1;
			auto source = m_graph.createView( crg::ImageViewData{ m_result.imageId.data->name + std::to_string( index )
				, m_result.imageId
				, 0u
				, VK_IMAGE_VIEW_TYPE_2D
				, m_result.getFormat()
				, VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, index, 1u, 0u, 1u } } );
			auto destination = m_graph.createView( crg::ImageViewData{ m_result.imageId.data->name + std::to_string( index + 1u )
				, m_result.imageId
				, 0u
				, VK_IMAGE_VIEW_TYPE_2D
				, m_result.getFormat()
				, VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, index + 1u, 1u, 0u, 1u } } );
			auto & pass = m_graph.createPass( "MinimiseDepth" + std::to_string( index )
				, [this, progress, size]( crg::FramePass const & pass
					, crg::GraphContext & context
					, crg::RunnableGraph & graph )
				{
					stepProgressBar( progress, "Initialising depth minify pass" );
					auto result = crg::RenderQuadBuilder{}
						.program( crg::makeVkArray< VkPipelineShaderStageCreateInfo >( m_minifyStages ) )
						.renderSize( size )
						.enabled( &m_ssaoConfig.enabled )
						.build( pass, context, graph );
					m_device.renderSystem.getEngine()->registerTimer( graph.getName() + "/SSAO"
						, result->getTimer() );
					return result;
				} );
			pass.addDependency( *m_lastPass );
			pass.addSampledView( source, DepthImgIdx );
			previousLevel.createPassBinding( pass, "PreviousLvlCfg", PrevLvlUboIdx );
			pass.addOutputColourView( destination );
			m_lastPass = &pass;
		}
	}
}
