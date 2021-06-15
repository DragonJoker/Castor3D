#include "GrayScalePostEffect/GrayScalePostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBufferPools.hpp>
#include <Castor3D/Buffer/GpuBuffer.hpp>
#include <Castor3D/Cache/SamplerCache.hpp>
#include <Castor3D/Cache/ShaderCache.hpp>
#include <Castor3D/Material/Texture/Sampler.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>
#include <Castor3D/Model/Vertex.hpp>
#include <Castor3D/Scene/ParticleSystem/ParticleDeclaration.hpp>
#include <Castor3D/Scene/ParticleSystem/ParticleElementDeclaration.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Render/RenderPassTimer.hpp>
#include <Castor3D/Render/RenderPipeline.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/RenderWindow.hpp>
#include <Castor3D/Render/Viewport.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>

#include <ashespp/Pipeline/GraphicsPipeline.hpp>
#include <ashespp/Pipeline/GraphicsPipelineCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <numeric>

namespace grayscale
{
	namespace
	{
		enum Idx : uint32_t
		{
			GrayCfgUboIdx,
			ColorTexIdx,
		};

		std::unique_ptr< ast::Shader > getVertexProgram()
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			Vec2 position = writer.declInput< Vec2 >( "position", 0u );
			Vec2 uv = writer.declInput< Vec2 >( "uv", 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( "vtx_texture", 0u );
			auto out = writer.getOut();

			castor3d::shader::Utils utils{ writer };
			utils.declareInvertVec2Y();

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					vtx_texture = uv;
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		std::unique_ptr< ast::Shader > getFragmentProgram()
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			auto configUbo = Ubo{ writer, "Configuration", GrayCfgUboIdx, 0u };
			auto c3d_factors = configUbo.declMember< Vec3 >( "c3d_factors" );
			configUbo.end();
			auto c3d_mapColor = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapColor", ColorTexIdx, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0u );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					auto colour = writer.declLocale( "colour"
						, c3d_mapColor.sample( vtx_texture ).xyz() );
					pxl_fragColor = vec4( vec3( dot( c3d_factors, colour ) ), 1.0_f );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//*********************************************************************************************

	castor::String PostEffect::Type = cuT( "grayscale" );
	castor::String PostEffect::Name = cuT( "GrayScale PostEffect" );

	PostEffect::PostEffect( castor3d::RenderTarget & renderTarget
		, castor3d::RenderSystem & renderSystem
		, castor3d::Parameters const & params )
		: castor3d::PostEffect{ PostEffect::Type
			, PostEffect::Name
			, renderTarget
			, renderSystem
			, params }
		, m_configUbo{ renderSystem.getMainRenderDevice()->uboPools->getBuffer< castor::Point3f >( 0u ) }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "GrayScale", getVertexProgram() }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "GrayScale", getFragmentProgram() }
		, m_stages{ makeShaderState( *renderSystem.getMainRenderDevice(), m_vertexShader )
			, makeShaderState( *renderSystem.getMainRenderDevice(), m_pixelShader ) }
	{
	}

	PostEffect::~PostEffect()
	{
		getRenderSystem()->getMainRenderDevice()->uboPools->putBuffer( m_configUbo );
	}

	castor3d::PostEffectSPtr PostEffect::create( castor3d::RenderTarget & renderTarget
		, castor3d::RenderSystem & renderSystem
		, castor3d::Parameters const & params )
	{
		return std::make_shared< PostEffect >( renderTarget
			, renderSystem
			, params );
	}

	void PostEffect::update( castor3d::CpuUpdater & updater )
	{
		if ( m_factors.isDirty() )
		{
			m_configUbo.getData() = m_factors.value();
			m_factors.reset();
		}
	}

	void PostEffect::accept( castor3d::PipelineVisitorBase & visitor )
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );
		visitor.visit( cuT( "GrayScale" )
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "Configuration" )
			, cuT( "Factors" )
			, m_factors );
	}

	crg::ImageViewId const * PostEffect::doInitialise( castor3d::RenderDevice const & device
		, castor3d::RenderPassTimer const & timer
		, crg::FramePass const & previousPass )
	{
		m_resultImg = m_renderTarget.getGraph().createImage( crg::ImageData{ "GSRes"
			, 0u
			, VK_IMAGE_TYPE_2D
			, m_target->data->info.format
			, castor3d::makeExtent3D( m_renderTarget.getSize() )
			, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT ) } );
		m_resultView = m_renderTarget.getGraph().createView( crg::ImageViewData{ "GSRes"
			, m_resultImg
			, 0u
			, VK_IMAGE_VIEW_TYPE_2D
			, m_resultImg.data->info.format
			, { VK_IMAGE_ASPECT_COLOR_BIT, 0u, 1u, 0u, 1u } } );
		m_pass = &m_renderTarget.getGraph().createPass( "GrayScalePass"
			, [this]( crg::FramePass const & pass
				, crg::GraphContext const & context
				, crg::RunnableGraph & graph )
			{
				return crg::RenderQuadBuilder{}
					.renderPosition( {} )
					.renderSize( castor3d::makeExtent2D( m_renderTarget.getSize() ) )
					.texcoordConfig( {} )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
					.enabled( &isEnabled() )
					.recordDisabledInto( [this, &context, &graph]( crg::RunnablePass const & runnable
						, VkCommandBuffer commandBuffer
						, uint32_t index )
						{
							auto & sceneView = *m_target;
							auto size = castor3d::makeExtent2D( m_renderTarget.getSize() );
							auto & srcSubresource = sceneView.data->info.subresourceRange;
							auto & dstSubresource = m_resultView.data->info.subresourceRange;
							VkImageCopy region{ VkImageSubresourceLayers{ srcSubresource.aspectMask, srcSubresource.baseMipLevel, srcSubresource.baseArrayLayer, 1u }
								, VkOffset3D{ 0u, 0u, 0u }
								, VkImageSubresourceLayers{ dstSubresource.aspectMask, dstSubresource.baseMipLevel, dstSubresource.baseArrayLayer, 1u }
								, VkOffset3D{ 0u, 0u, 0u }
							, { size.width, size.height, 1u } };
							auto srcTransition = runnable.getTransition( index, sceneView );
							auto dstTransition = runnable.getTransition( index, m_resultView );
							graph.memoryBarrier( commandBuffer
								, sceneView
								, srcTransition.to
								, { VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
								, crg::getAccessMask( VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL )
								, crg::getStageMask( VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL ) } );
							graph.memoryBarrier( commandBuffer
								, m_resultView
								, dstTransition.to
								, { VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
								, crg::getAccessMask( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL )
								, crg::getStageMask( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) } );
							context.vkCmdCopyImage( commandBuffer
								, graph.createImage( sceneView.data->image )
								, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
								, graph.createImage( m_resultView.data->image )
								, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
								, 1u
								, &region );
							graph.memoryBarrier( commandBuffer
								, m_resultView
								, { VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
								, crg::getAccessMask( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL )
								, crg::getStageMask( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) }
							, dstTransition.to );
							graph.memoryBarrier( commandBuffer
								, sceneView
								, { VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
								, crg::getAccessMask( VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL )
								, crg::getStageMask( VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL ) }
							, srcTransition.to );
						} )
					.build( pass, context, graph );
			} );
		m_pass->addDependency( previousPass );
		m_configUbo.createPassBinding( *m_pass
			, "GrayCfg"
			, GrayCfgUboIdx );
		m_pass->addSampledView( *m_target
			, ColorTexIdx
			, VK_IMAGE_LAYOUT_UNDEFINED );
		m_pass->addOutputColourView( m_resultView );
		return &m_resultView;
	}

	void PostEffect::doCleanup( castor3d::RenderDevice const & device )
	{
		device.uboPools->putBuffer( m_configUbo );
	}

	bool PostEffect::doWriteInto( castor::StringStream & file, castor::String const & tabs )
	{
		file << ( tabs + cuT( "postfx \"" ) + Type + cuT( "\"" ) );
		return true;
	}

	//*********************************************************************************************
}
