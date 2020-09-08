#include "Castor3D/Render/Technique/Opaque/LightVolumeGI/LayeredLightVolumeGIPass.hpp"

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
#include "Castor3D/Shader/Ubos/LayeredLpvConfigUbo.hpp"
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
		static constexpr uint32_t RLpvAccumIdx0 = 4u;
		static constexpr uint32_t GLpvAccumIdx0 = 5u;
		static constexpr uint32_t BLpvAccumIdx0 = 6u;
		static constexpr uint32_t RLpvAccumIdx1 = 7u;
		static constexpr uint32_t GLpvAccumIdx1 = 8u;
		static constexpr uint32_t BLpvAccumIdx1 = 9u;
		static constexpr uint32_t RLpvAccumIdx2 = 10u;
		static constexpr uint32_t GLpvAccumIdx2 = 11u;
		static constexpr uint32_t BLpvAccumIdx2 = 12u;
		static constexpr uint32_t RLpvAccumIdx3 = 13u;
		static constexpr uint32_t GLpvAccumIdx3 = 14u;
		static constexpr uint32_t BLpvAccumIdx3 = 15u;

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
			UBO_LAYERED_LPVCONFIG( writer, LIUboIdx, 0u );
			auto c3d_mapDepth = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eDepth ), DepthMapIdx, 0u );
			auto c3d_mapData1 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData1 ), Data1MapIdx, 0u );
			auto c3d_lpvAccumulatorR0 = writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eR, "Accumulator" ) + "0", RLpvAccumIdx0, 0u );
			auto c3d_lpvAccumulatorG0 = writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eG, "Accumulator" ) + "0", GLpvAccumIdx0, 0u );
			auto c3d_lpvAccumulatorB0 = writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eB, "Accumulator" ) + "0", BLpvAccumIdx0, 0u );
			auto c3d_lpvAccumulatorR1 = writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eR, "Accumulator" ) + "1", RLpvAccumIdx1, 0u );
			auto c3d_lpvAccumulatorG1 = writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eG, "Accumulator" ) + "1", GLpvAccumIdx1, 0u );
			auto c3d_lpvAccumulatorB1 = writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eB, "Accumulator" ) + "1", BLpvAccumIdx1, 0u );
			auto c3d_lpvAccumulatorR2 = writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eR, "Accumulator" ) + "2", RLpvAccumIdx2, 0u );
			auto c3d_lpvAccumulatorG2 = writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eG, "Accumulator" ) + "2", GLpvAccumIdx2, 0u );
			auto c3d_lpvAccumulatorB2 = writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eB, "Accumulator" ) + "2", BLpvAccumIdx2, 0u );
			auto c3d_lpvAccumulatorR3 = writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eR, "Accumulator" ) + "3", RLpvAccumIdx3, 0u );
			auto c3d_lpvAccumulatorG3 = writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eG, "Accumulator" ) + "3", GLpvAccumIdx3, 0u );
			auto c3d_lpvAccumulatorB3 = writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eB, "Accumulator" ) + "3", BLpvAccumIdx3, 0u );
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

					// l3 is the finest
					auto lpvCellCoords3 = writer.declLocale( "lpvCellCoords3"
						, ( wsPosition - c3d_allMinVolumeCorners[3].xyz() ) / c3d_allCellSizes.w() );
					auto lpvCellCoords2 = writer.declLocale( "lpvCellCoords2"
						, ( wsPosition - c3d_allMinVolumeCorners[2].xyz() ) / c3d_allCellSizes.z() );
					auto lpvCellCoords1 = writer.declLocale( "lpvCellCoords1"
						, ( wsPosition - c3d_allMinVolumeCorners[1].xyz() ) / c3d_allCellSizes.y() );
					auto lpvCellCoords0 = writer.declLocale( "lpvCellCoords0"
						, ( wsPosition - c3d_allMinVolumeCorners[0].xyz() ) / c3d_allCellSizes.x() );

					auto gridSize = writer.declLocale( "gridSize"
						, vec3( c3d_gridSize.xyz() ) );

					lpvCellCoords3 /= gridSize;
					auto lpvIntensity3 = writer.declLocale( "lpvIntensity3"
						, vec3(
							dot( SHintensity, texture( c3d_lpvAccumulatorR3, lpvCellCoords3 ) ),
							dot( SHintensity, texture( c3d_lpvAccumulatorG3, lpvCellCoords3 ) ),
							dot( SHintensity, texture( c3d_lpvAccumulatorB3, lpvCellCoords3 ) ) ) );

					lpvCellCoords2 /= gridSize;
					auto lpvIntensity2 = writer.declLocale( "lpvIntensity2"
						, vec3(
							dot( SHintensity, texture( c3d_lpvAccumulatorR2, lpvCellCoords2 ) ),
							dot( SHintensity, texture( c3d_lpvAccumulatorG2, lpvCellCoords2 ) ),
							dot( SHintensity, texture( c3d_lpvAccumulatorB2, lpvCellCoords2 ) ) ) );

					lpvCellCoords1 /= gridSize;
					auto lpvIntensity1 = writer.declLocale( "lpvIntensity1"
						, vec3(
							dot( SHintensity, texture( c3d_lpvAccumulatorR1, lpvCellCoords1 ) ),
							dot( SHintensity, texture( c3d_lpvAccumulatorG1, lpvCellCoords1 ) ),
							dot( SHintensity, texture( c3d_lpvAccumulatorB1, lpvCellCoords1 ) ) ) );

					lpvCellCoords0 /= gridSize;
					auto lpvIntensity0 = writer.declLocale( "lpvIntensity0"
						, vec3(
							dot( SHintensity, texture( c3d_lpvAccumulatorR0, lpvCellCoords0 ) ),
							dot( SHintensity, texture( c3d_lpvAccumulatorG0, lpvCellCoords0 ) ),
							dot( SHintensity, texture( c3d_lpvAccumulatorB0, lpvCellCoords0 ) ) ) );

					auto lpvIntensity = writer.declLocale( "lpvIntensity"
						, lpvIntensity0 + lpvIntensity1 + lpvIntensity2 + lpvIntensity3 );
					auto finalLPVRadiance = writer.declLocale( "finalLPVRadiance"
						, ( c3d_config.x() / Float{ castor::Pi< float > } ) * max( lpvIntensity, vec3( 0.0_f ) ) );
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

	LayeredLightVolumeGIPass::LayeredLightVolumeGIPass( Engine & engine
		, GpInfoUbo const & gpInfo
		, LayeredLpvConfigUbo const & lpvConfigUbo
		, OpaquePassResult const & gpResult
		, LightVolumePassResult const & lpResult0
		, LightVolumePassResult const & lpResult1
		, LightVolumePassResult const & lpResult2
		, LightVolumePassResult const & lpResult3
		, TextureUnit const & dst )
		: RenderQuad{ *engine.getRenderSystem()
			, "DirectionalLayeredVplGI"
			, VK_FILTER_LINEAR
			, { ashes::nullopt, RenderQuadConfig::Texcoord{}, BlendMode::eAdditive } }
		, m_gpInfo{ gpInfo }
		, m_lpvConfigUbo{ lpvConfigUbo }
		, m_gpResult{ gpResult }
		, m_lpResult0{ lpResult0 }
		, m_lpResult1{ lpResult1 }
		, m_lpResult2{ lpResult2 }
		, m_lpResult3{ lpResult3 }
		, m_result{ dst }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, getName(), getVertexProgram() }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, getName(), getPixelProgram() }
		, m_renderPass{ doCreateRenderPass( getCurrentRenderDevice( m_renderSystem )
			, m_result.getTexture()->getPixelFormat() ) }
		, m_frameBuffer{ doCreateFrameBuffer( *m_renderPass
			, m_result.getTexture()->getDefaultView().getTargetView() ) }
		, m_timer{ std::make_shared< RenderPassTimer >( engine, cuT( "Light Propagation Volumes" ), cuT( "Layered GI Resolve" ) ) }
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
			makeDescriptorSetLayoutBinding( RLpvAccumIdx0
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
			makeDescriptorSetLayoutBinding( GLpvAccumIdx0
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
			makeDescriptorSetLayoutBinding( BLpvAccumIdx0
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
			makeDescriptorSetLayoutBinding( RLpvAccumIdx1
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
			makeDescriptorSetLayoutBinding( GLpvAccumIdx1
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
			makeDescriptorSetLayoutBinding( BLpvAccumIdx1
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
			makeDescriptorSetLayoutBinding( RLpvAccumIdx2
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
			makeDescriptorSetLayoutBinding( GLpvAccumIdx2
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
			makeDescriptorSetLayoutBinding( BLpvAccumIdx2
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
			makeDescriptorSetLayoutBinding( RLpvAccumIdx3
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
			makeDescriptorSetLayoutBinding( GLpvAccumIdx3
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
		};
		this->createPipeline( { m_result.getTexture()->getDimensions().width, m_result.getTexture()->getDimensions().height }
			, {}
			, shaderStages
			, m_lpResult3[LpvTexture::eB].getTexture()->getDefaultView().getSampledView()
			, *m_renderPass
			, std::move( bindings )
			, {} );
		auto commands = getCommands( *m_timer, 0u );
		m_commandBuffer = std::move( commands.commandBuffer );
		m_finished = std::move( commands.semaphore );
	}

	ashes::Semaphore const & LayeredLightVolumeGIPass::compute( ashes::Semaphore const & toWait )const
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

	CommandsSemaphore LayeredLightVolumeGIPass::getCommands( RenderPassTimer const & timer
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

	void LayeredLightVolumeGIPass::accept( PipelineVisitorBase & visitor )
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );
	}

	void LayeredLightVolumeGIPass::doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
		, ashes::DescriptorSet & descriptorSet )
	{
		m_gpInfo.getUbo().createSizedBinding( descriptorSet
			, descriptorSetLayout.getBinding( GpInfoUboIdx ) );
		descriptorSet.createSizedBinding( descriptorSetLayout.getBinding( LIUboIdx )
			, m_lpvConfigUbo.getUbo() );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( DepthMapIdx )
			, m_gpResult[DsTexture::eDepth].getTexture()->getDefaultView().getSampledView()
			, m_gpResult[DsTexture::eDepth].getSampler()->getSampler() );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( Data1MapIdx )
			, m_gpResult[DsTexture::eData1].getTexture()->getDefaultView().getSampledView()
			, m_gpResult[DsTexture::eData1].getSampler()->getSampler() );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( RLpvAccumIdx0 )
			, m_lpResult0[LpvTexture::eR].getTexture()->getDefaultView().getSampledView()
			, m_lpResult0[LpvTexture::eR].getSampler()->getSampler() );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( GLpvAccumIdx0 )
			, m_lpResult0[LpvTexture::eG].getTexture()->getDefaultView().getSampledView()
			, m_lpResult0[LpvTexture::eG].getSampler()->getSampler() );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( BLpvAccumIdx0 )
			, m_lpResult0[LpvTexture::eB].getTexture()->getDefaultView().getSampledView()
			, m_lpResult0[LpvTexture::eB].getSampler()->getSampler() );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( RLpvAccumIdx1 )
			, m_lpResult1[LpvTexture::eR].getTexture()->getDefaultView().getSampledView()
			, m_lpResult1[LpvTexture::eR].getSampler()->getSampler() );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( GLpvAccumIdx1 )
			, m_lpResult1[LpvTexture::eG].getTexture()->getDefaultView().getSampledView()
			, m_lpResult1[LpvTexture::eG].getSampler()->getSampler() );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( BLpvAccumIdx1 )
			, m_lpResult1[LpvTexture::eB].getTexture()->getDefaultView().getSampledView()
			, m_lpResult1[LpvTexture::eB].getSampler()->getSampler() );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( RLpvAccumIdx2 )
			, m_lpResult2[LpvTexture::eR].getTexture()->getDefaultView().getSampledView()
			, m_lpResult2[LpvTexture::eR].getSampler()->getSampler() );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( GLpvAccumIdx2 )
			, m_lpResult2[LpvTexture::eG].getTexture()->getDefaultView().getSampledView()
			, m_lpResult2[LpvTexture::eG].getSampler()->getSampler() );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( BLpvAccumIdx2 )
			, m_lpResult2[LpvTexture::eB].getTexture()->getDefaultView().getSampledView()
			, m_lpResult2[LpvTexture::eB].getSampler()->getSampler() );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( RLpvAccumIdx3 )
			, m_lpResult3[LpvTexture::eR].getTexture()->getDefaultView().getSampledView()
			, m_lpResult3[LpvTexture::eR].getSampler()->getSampler() );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( GLpvAccumIdx3 )
			, m_lpResult3[LpvTexture::eG].getTexture()->getDefaultView().getSampledView()
			, m_lpResult3[LpvTexture::eG].getSampler()->getSampler() );
	}

	void LayeredLightVolumeGIPass::doRegisterFrame( ashes::CommandBuffer & commandBuffer )const
	{
	}
}
