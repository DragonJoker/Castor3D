#include "Castor3D/Render/Technique/Opaque/LightVolumeGI/LightVolumeGIPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/PoolUniformBufferBase.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/Parameter.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Model/Vertex.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/Passes/LineariseDepthPass.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapResult.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaquePassResult.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightPassResult.hpp"
#include "Castor3D/Render/Technique/Opaque/ReflectiveShadowMapGI/ReflectiveShadowMapping.hpp"
#include "Castor3D/Render/Technique/Opaque/ReflectiveShadowMapGI/RsmConfig.hpp"
#include "Castor3D/Render/Technique/Opaque/LightVolumeGI/LightVolumePassResult.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslFog.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslMetallicBrdfLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslPhongLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/GpInfoUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelMatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/RsmConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/LpvConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <CastorUtils/Design/ArrayView.hpp>
#include <CastorUtils/Graphics/Image.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>
#include <ashespp/RenderPass/SubpassDescription.hpp>

#include <ShaderWriter/Source.hpp>

#include <numeric>
#include <random>

using namespace castor;

namespace castor3d
{
	namespace
	{
		static constexpr uint32_t GpInfoUboIdx = 0u;
		static constexpr uint32_t LIUboIdx = 1u;
		static constexpr uint32_t DepthMapIdx = 2u;
		static constexpr uint32_t Data1MapIdx = 3u;
		static constexpr uint32_t RLpvAccumIdx = 4u;
		static constexpr uint32_t GLpvAccumIdx = 5u;
		static constexpr uint32_t BLpvAccumIdx = 6u;

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

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					vtx_texture = uv;
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		ShaderPtr getPixelProgram()
		{
			using namespace sdw;
			FragmentWriter writer;

			/*Spherical harmonics coefficients - precomputed*/
			auto SH_C0 = writer.declConstant( "SH_C0"
				, 0.282094792_f );// 1 / (2 * sqrt(pi))
			auto SH_C1 = writer.declConstant( "SH_C1"
				, 0.488602512_f ); // sqrt(3 / pi) / 2

			// Shader inputs
			UBO_GPINFO( writer, GpInfoUboIdx, 0u );
			UBO_LPVCONFIG( writer, LIUboIdx, 0u );
			auto c3d_mapDepth = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eDepth ), DepthMapIdx, 0u );
			auto c3d_mapData1 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData1 ), Data1MapIdx, 0u );
			auto c3d_lpvAccumulatorR = writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eR, "Accumulator" ), RLpvAccumIdx, 0u );
			auto c3d_lpvAccumulatorG = writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eG, "Accumulator" ), GLpvAccumIdx, 0u );
			auto c3d_lpvAccumulatorB = writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eB, "Accumulator" ), BLpvAccumIdx, 0u );
			auto in = writer.getIn();

			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_lpvGI = writer.declOutput< Vec3 >( "pxl_lpvGI", 0 );

			// Utility functions
			shader::Utils utils{ writer };
			utils.declareCalcWSPosition();
			utils.declareCalcVSPosition();

			// no normalization
			auto evalSH_direct = writer.implementFunction< Vec4 >( "evalSH_direct"
				, [&]( Vec3 direction )
				{
					writer.returnStmt( vec4( SH_C0
						, -SH_C1 * direction.y()
						, SH_C1 * direction.z()
						, -SH_C1 * direction.x() ) );
				}
				, InVec3{ writer, "direction" } );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					auto texCoord = writer.declLocale( "texCoord"
						, vtx_texture );
					auto depth = writer.declLocale( "depth"
						, textureLod( c3d_mapDepth, texCoord, 0.0_f ).x() );

					IF( writer, depth == 1.0_f )
					{
						writer.discard();
					}
					FI;

					auto data1 = writer.declLocale( "data1"
						, textureLod( c3d_mapData1, texCoord, 0.0_f ) );
					auto wsPosition = writer.declLocale( "wsPosition"
						, utils.calcWSPosition( texCoord, depth, c3d_mtxInvViewProj ) );
					auto wsNormal = writer.declLocale( "wsNormal"
						, data1.xyz() );

					auto SHintensity = writer.declLocale( "SHintensity"
						, evalSH_direct( -wsNormal ) );
					auto lpvIntensity = writer.declLocale( "lpvIntensity"
						, vec3( 0.0_f ) );
					auto lpvCellCoords = writer.declLocale( "lpvCellCoords"
						, ( wsPosition - c3d_minVolumeCorner.xyz() ) / c3d_minVolumeCorner.w() / c3d_gridSizes.xyz() );

					lpvIntensity = vec3(
						dot( SHintensity, texture( c3d_lpvAccumulatorR, lpvCellCoords ) ),
						dot( SHintensity, texture( c3d_lpvAccumulatorG, lpvCellCoords ) ),
						dot( SHintensity, texture( c3d_lpvAccumulatorB, lpvCellCoords ) )
					);

					auto finalLPVRadiance = writer.declLocale( "finalLPVRadiance"
						, ( c3d_lpvConfig.x() / Float{ castor::Pi< float > } ) * max( lpvIntensity, vec3( 0.0_f ) ) );
					pxl_lpvGI = finalLPVRadiance;
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		SamplerSPtr doCreateSampler( Engine & engine
			, String const & name
			, VkSamplerAddressMode mode )
		{
			SamplerSPtr sampler;

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().add( name );
				sampler->setMinFilter( VK_FILTER_LINEAR );
				sampler->setMagFilter( VK_FILTER_LINEAR );
				sampler->setWrapS( mode );
				sampler->setWrapT( mode );
			}

			return sampler;
		}

		ashes::RenderPassPtr doCreateRenderPass( RenderDevice const & device
			, VkFormat format )
		{
			ashes::VkAttachmentDescriptionArray attaches
			{
				{
					0u,
					format,
					VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_LOAD,
					VK_ATTACHMENT_STORE_OP_STORE,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				},
			};
			ashes::SubpassDescriptionArray subpasses;
			subpasses.emplace_back( ashes::SubpassDescription
				{
					0u,
					VK_PIPELINE_BIND_POINT_GRAPHICS,
					{},
					{
						{ 0u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
					},
					{},
					ashes::nullopt,
					{},
				} );
			ashes::VkSubpassDependencyArray dependencies
			{
				{
					VK_SUBPASS_EXTERNAL,
					0u,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
					VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
					VK_ACCESS_SHADER_READ_BIT,
					VK_DEPENDENCY_BY_REGION_BIT,
				},
				{
					0u,
					VK_SUBPASS_EXTERNAL,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
					VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
					VK_ACCESS_SHADER_READ_BIT,
					VK_DEPENDENCY_BY_REGION_BIT,
				},
			};
			ashes::RenderPassCreateInfo createInfo
			{
				0u,
				std::move( attaches ),
				std::move( subpasses ),
				std::move( dependencies ),
			};
			auto result = device->createRenderPass( "VplGI"
				, std::move( createInfo ) );
			return result;
		}

		ashes::FrameBufferPtr doCreateFrameBuffer( ashes::RenderPass const & renderPass
			, ashes::ImageView const & view )
		{
			ashes::ImageViewCRefArray attaches;
			attaches.emplace_back( view );
			auto size = view.image->getDimensions();
			return renderPass.createFrameBuffer( "VplGI"
				, VkExtent2D{ size.width, size.height }
				, std::move( attaches ) );
		}
	}

	//*********************************************************************************************

	LightVolumeGIPass::LightVolumeGIPass( Engine & engine
		, LightType lightType
		, GpInfoUbo const & gpInfo
		, LpvConfigUbo const & lpvConfigUbo
		, OpaquePassResult const & gpResult
		, LightVolumePassResult const & lpResult
		, TextureUnit const & dst )
		: RenderQuad{ *engine.getRenderSystem()
			, castor3d::getName( lightType ) + "VplGI"
			, VK_FILTER_LINEAR
			, { ashes::nullopt, RenderQuadConfig::Texcoord{}, BlendMode::eAdditive } }
		, m_gpInfo{ gpInfo }
		, m_lpvConfigUbo{ lpvConfigUbo }
		, m_gpResult{ gpResult }
		, m_lpResult{ lpResult }
		, m_result{ dst }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, getName(), getVertexProgram() }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, getName(), getPixelProgram() }
		, m_renderPass{ doCreateRenderPass( getCurrentRenderDevice( m_renderSystem )
			, m_result.getTexture()->getPixelFormat() ) }
		, m_frameBuffer{ doCreateFrameBuffer( *m_renderPass
			, m_result.getTexture()->getDefaultView().getTargetView() ) }
		, m_timer{ std::make_shared< RenderPassTimer >( engine, cuT( "Light Propagation Volumes" ), cuT( "GI Resolve" ) ) }
	{
		auto & device = getCurrentRenderDevice( m_renderSystem );
		ashes::PipelineShaderStageCreateInfoArray shaderStages;
		shaderStages.push_back( makeShaderState( device, m_vertexShader ) );
		shaderStages.push_back( makeShaderState( device, m_pixelShader ) );

		ashes::VkDescriptorSetLayoutBindingArray bindings
		{
			makeDescriptorSetLayoutBinding( GpInfoUboIdx
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
			makeDescriptorSetLayoutBinding( LIUboIdx
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
			makeDescriptorSetLayoutBinding( DepthMapIdx
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
			makeDescriptorSetLayoutBinding( Data1MapIdx
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
			makeDescriptorSetLayoutBinding( RLpvAccumIdx
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
			makeDescriptorSetLayoutBinding( GLpvAccumIdx
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
		};
		this->createPipeline( { m_result.getTexture()->getDimensions().width, m_result.getTexture()->getDimensions().height }
			, {}
			, shaderStages
			, m_lpResult[LpvTexture::eB].getTexture()->getDefaultView().getSampledView()
			, *m_renderPass
			, std::move( bindings )
			, {} );
		auto commands = getCommands( *m_timer, 0u );
		m_commandBuffer = std::move( commands.commandBuffer );
		m_finished = std::move( commands.semaphore );
	}

	ashes::Semaphore const & LightVolumeGIPass::compute( ashes::Semaphore const & toWait )const
	{
		auto & renderSystem = m_renderSystem;
		auto & device = getCurrentRenderDevice( renderSystem );
		RenderPassTimerBlock timerBlock{ m_timer->start() };
		timerBlock->notifyPassRender();
		auto * result = &toWait;

		device.graphicsQueue->submit( *m_commandBuffer
			, toWait
			, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, *m_finished
			, nullptr );
		result = m_finished.get();

		return *result;
	}

	CommandsSemaphore LightVolumeGIPass::getCommands( RenderPassTimer const & timer
		, uint32_t index )const
	{
		auto & device = getCurrentRenderDevice( m_renderSystem );
		castor3d::CommandsSemaphore commands
		{
			device.graphicsCommandPool->createCommandBuffer( getName() ),
			device->createSemaphore( getName() )
		};
		auto & cmd = *commands.commandBuffer;

		cmd.begin( VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT );
		cmd.memoryBarrier( VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT
			, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
			, m_gpResult[DsTexture::eDepth].getTexture()->getDefaultView().getTargetView().makeShaderInputResource( VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL ) );
		timer.beginPass( cmd, index );
		cmd.beginDebugBlock(
			{
				"Lighting - LPV GI",
				castor3d::makeFloatArray( m_renderSystem.getEngine()->getNextRainbowColour() ),
			} );
		cmd.beginRenderPass( *m_renderPass
			, *m_frameBuffer
			, { castor3d::transparentBlackClearColor }
			, VK_SUBPASS_CONTENTS_INLINE );
		registerFrame( cmd );
		cmd.endRenderPass();
		cmd.memoryBarrier( VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
			, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT
			, m_gpResult[DsTexture::eDepth].getTexture()->getDefaultView().getTargetView().makeDepthStencilReadOnly( VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) );
		cmd.endDebugBlock();
		timer.endPass( cmd, index );
		cmd.end();

		return commands;
	}

	void LightVolumeGIPass::accept( PipelineVisitorBase & visitor )
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );
	}

	void LightVolumeGIPass::doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
		, ashes::DescriptorSet & descriptorSet )
	{
		m_gpInfo.createSizedBinding( descriptorSet
			, descriptorSetLayout.getBinding( GpInfoUboIdx ) );
		m_lpvConfigUbo.createSizedBinding( descriptorSet
			, descriptorSetLayout.getBinding( LIUboIdx ) );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( DepthMapIdx )
			, m_gpResult[DsTexture::eDepth].getTexture()->getDefaultView().getSampledView()
			, m_gpResult[DsTexture::eDepth].getSampler()->getSampler() );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( Data1MapIdx )
			, m_gpResult[DsTexture::eData1].getTexture()->getDefaultView().getSampledView()
			, m_gpResult[DsTexture::eData1].getSampler()->getSampler() );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( RLpvAccumIdx )
			, m_lpResult[LpvTexture::eR].getTexture()->getDefaultView().getSampledView()
			, m_lpResult[LpvTexture::eR].getSampler()->getSampler() );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( GLpvAccumIdx )
			, m_lpResult[LpvTexture::eG].getTexture()->getDefaultView().getSampledView()
			, m_lpResult[LpvTexture::eG].getSampler()->getSampler() );
	}

	void LightVolumeGIPass::doRegisterFrame( ashes::CommandBuffer & commandBuffer )const
	{
	}
}
