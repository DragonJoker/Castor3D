#include "Castor3D/Render/Technique/Opaque/LightVolumeGI/LayeredLightVolumeGIPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/PoolUniformBuffer.hpp"
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
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightVolumePassResult.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslFog.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslLpvGI.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslMetallicBrdfLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslPhongLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/GpInfoUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelMatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/RsmConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/LayeredLpvGridConfigUbo.hpp"
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

CU_ImplementCUSmartPtr( castor3d, LayeredLightVolumeGIPass )

using namespace castor;

namespace castor3d
{
	namespace
	{
		enum InIds
		{
			GpInfoUboIdx,
			LpvGridUboIdx,
			DepthMapIdx,
			Data1MapIdx,
			RLpvAccum1Idx,
			GLpvAccum1Idx,
			BLpvAccum1Idx,
			RLpvAccum2Idx,
			GLpvAccum2Idx,
			BLpvAccum2Idx,
			RLpvAccum3Idx,
			GLpvAccum3Idx,
			BLpvAccum3Idx,
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
			shader::LpvGI lpvGI{ writer };

			// Shader inputs
			UBO_GPINFO( writer, GpInfoUboIdx, 0u );
			UBO_LAYERED_LPVGRIDCONFIG( writer, LpvGridUboIdx, 0u, true );
			auto c3d_mapDepth = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eDepth ), DepthMapIdx, 0u );
			auto c3d_mapData1 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData1 ), Data1MapIdx, 0u );
			lpvGI.declareLayered( uint32_t( RLpvAccum1Idx ) );
			auto in = writer.getIn();

			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_lpvGI = writer.declOutput< Vec3 >( "pxl_lpvGI", 0 );

			// Utility functions
			shader::Utils utils{ writer };
			utils.declareCalcWSPosition();
			utils.declareCalcVSPosition();

			writer.implementMain( [&]()
				{
					auto texCoord = writer.declLocale( "texCoord"
						, vtx_texture );
					auto depth = writer.declLocale( "depth"
						, c3d_mapDepth.lod( texCoord, 0.0_f ).x() );

					IF( writer, depth == 1.0_f )
					{
						writer.discard();
					}
					FI;

					auto data1 = writer.declLocale( "data1"
						, c3d_mapData1.lod( texCoord, 0.0_f ) );
					auto wsPosition = writer.declLocale( "wsPosition"
						, utils.calcWSPosition( texCoord, depth, c3d_mtxInvViewProj ) );
					auto wsNormal = writer.declLocale( "wsNormal"
						, data1.xyz() );

					pxl_lpvGI = c3d_indirectAttenuations / Float{ castor::Pi< float > }
						* lpvGI.computeLLPVRadiance( wsPosition
							, wsNormal
							, c3d_allMinVolumeCorners
							, c3d_allCellSizes
							, c3d_gridSizes );
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
			, VkFormat format
			, BlendMode blendMode )
		{
			ashes::VkAttachmentDescriptionArray attaches
			{
				{
					0u,
					format,
					VK_SAMPLE_COUNT_1_BIT,
					( blendMode == BlendMode::eNoBlend
						? VK_ATTACHMENT_LOAD_OP_CLEAR
						: VK_ATTACHMENT_LOAD_OP_LOAD ),
					VK_ATTACHMENT_STORE_OP_STORE,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					( blendMode == BlendMode::eNoBlend
						? VK_IMAGE_LAYOUT_UNDEFINED
						: VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ),
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

		rq::BindingDescriptionArray createBindings()
		{
			return
			{
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, std::nullopt },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, std::nullopt },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_IMAGE_VIEW_TYPE_2D },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_IMAGE_VIEW_TYPE_2D },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_IMAGE_VIEW_TYPE_3D },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_IMAGE_VIEW_TYPE_3D },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_IMAGE_VIEW_TYPE_3D },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_IMAGE_VIEW_TYPE_3D },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_IMAGE_VIEW_TYPE_3D },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_IMAGE_VIEW_TYPE_3D },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_IMAGE_VIEW_TYPE_3D },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_IMAGE_VIEW_TYPE_3D },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_IMAGE_VIEW_TYPE_3D },
			};
		}
	}

	//*********************************************************************************************

	LayeredLightVolumeGIPass::LayeredLightVolumeGIPass( Engine & engine
		, RenderDevice const & device
		, castor::String const & prefix
		, GpInfoUbo const & gpInfo
		, LayeredLpvGridConfigUbo const & lpvConfigUbo
		, OpaquePassResult const & gpResult
		, LightVolumePassResultArray const & lpvResult
		, TextureUnit const & dst
		, BlendMode blendMode )
		: RenderQuad{ device
			, prefix + "GIResolve"
			, VK_FILTER_LINEAR
			, { createBindings()
				, ashes::nullopt
				, rq::Texcoord{}
				, blendMode } }
		, m_gpInfo{ gpInfo }
		, m_lpvConfigUbo{ lpvConfigUbo }
		, m_gpResult{ gpResult }
		, m_lpvResult{ lpvResult }
		, m_result{ dst }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, getName(), getVertexProgram() }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, getName(), getPixelProgram() }
		, m_renderPass{ doCreateRenderPass( device
			, m_result.getTexture()->getPixelFormat()
			, blendMode ) }
		, m_frameBuffer{ doCreateFrameBuffer( *m_renderPass
			, m_result.getTexture()->getDefaultView().getTargetView() ) }
		, m_timer{ std::make_shared< RenderPassTimer >( engine, m_device, cuT( "Light Propagation Volumes" ), cuT( "Layered GI Resolve" ) ) }
	{
		ashes::PipelineShaderStageCreateInfoArray shaderStages;
		shaderStages.push_back( makeShaderState( m_device, m_vertexShader ) );
		shaderStages.push_back( makeShaderState( m_device, m_pixelShader ) );
		auto & lpv0 = *m_lpvResult[0];
		auto & lpv1 = *m_lpvResult[1];
		auto & lpv2 = *m_lpvResult[2];

		createPipelineAndPass( { m_result.getTexture()->getDimensions().width, m_result.getTexture()->getDimensions().height }
			, {}
			, shaderStages
			, * m_renderPass
			, {
				makeDescriptorWrite( m_gpInfo.getUbo(), GpInfoUboIdx ),
				makeDescriptorWrite( m_lpvConfigUbo.getUbo(), LpvGridUboIdx ),
				makeDescriptorWrite( m_gpResult[DsTexture::eDepth].getTexture()->getDefaultView().getSampledView()
				, m_gpResult[DsTexture::eDepth].getSampler()->getSampler()
					, DepthMapIdx ),
				makeDescriptorWrite( m_gpResult[DsTexture::eData1].getTexture()->getDefaultView().getSampledView()
					, m_gpResult[DsTexture::eData1].getSampler()->getSampler()
					, Data1MapIdx ),
				makeDescriptorWrite( lpv0[LpvTexture::eR].getTexture()->getDefaultView().getSampledView()
					, lpv0[LpvTexture::eR].getSampler()->getSampler()
					, RLpvAccum1Idx ),
				makeDescriptorWrite( lpv0[LpvTexture::eG].getTexture()->getDefaultView().getSampledView()
					, lpv0[LpvTexture::eG].getSampler()->getSampler()
					, GLpvAccum1Idx ),
				makeDescriptorWrite( lpv0[LpvTexture::eB].getTexture()->getDefaultView().getSampledView()
					, lpv0[LpvTexture::eB].getSampler()->getSampler()
					, BLpvAccum1Idx ),
				makeDescriptorWrite( lpv1[LpvTexture::eR].getTexture()->getDefaultView().getSampledView()
					, lpv1[LpvTexture::eR].getSampler()->getSampler()
					, RLpvAccum2Idx ),
				makeDescriptorWrite( lpv1[LpvTexture::eG].getTexture()->getDefaultView().getSampledView()
					, lpv1[LpvTexture::eG].getSampler()->getSampler()
					, GLpvAccum2Idx ),
				makeDescriptorWrite( lpv1[LpvTexture::eB].getTexture()->getDefaultView().getSampledView()
					, lpv1[LpvTexture::eB].getSampler()->getSampler()
					, BLpvAccum2Idx ),
				makeDescriptorWrite( lpv2[LpvTexture::eR].getTexture()->getDefaultView().getSampledView()
					, lpv2[LpvTexture::eR].getSampler()->getSampler()
					, RLpvAccum3Idx ),
				makeDescriptorWrite( lpv2[LpvTexture::eG].getTexture()->getDefaultView().getSampledView()
					, lpv2[LpvTexture::eG].getSampler()->getSampler()
					, GLpvAccum3Idx ),
				makeDescriptorWrite( lpv2[LpvTexture::eB].getTexture()->getDefaultView().getSampledView()
					, lpv2[LpvTexture::eB].getSampler()->getSampler()
					, BLpvAccum3Idx ),
			} );
		m_commands = getCommands( *m_timer, 0u );
	}

	ashes::Semaphore const & LayeredLightVolumeGIPass::compute( ashes::Semaphore const & toWait )const
	{
		auto & renderSystem = m_renderSystem;
		RenderPassTimerBlock timerBlock{ m_timer->start() };
		timerBlock->notifyPassRender();
		auto * result = &toWait;

		m_device.graphicsQueue->submit( *m_commands.commandBuffer
			, toWait
			, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, *m_commands.semaphore
			, nullptr );
		result = m_commands.semaphore.get();

		return *result;
	}

	CommandsSemaphore LayeredLightVolumeGIPass::getCommands( RenderPassTimer const & timer
		, uint32_t index )const
	{
		castor3d::CommandsSemaphore commands
		{
			m_device.graphicsCommandPool->createCommandBuffer( getName() ),
			m_device->createSemaphore( getName() )
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
		registerPass( cmd );
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
}
