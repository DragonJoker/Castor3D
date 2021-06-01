#include "Castor3D/Render/Technique/Opaque/ReflectiveShadowMapGI/RsmGIPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
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
#include "Castor3D/Shader/Ubos/ModelUbo.hpp"
#include "Castor3D/Shader/Ubos/RsmConfigUbo.hpp"
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
		enum Idx : uint32_t
		{
			RsmCfgUboIdx,
			RsmSamplesIdx,
			GpInfoUboIdx,
			LightsMapIdx,
			DepthMapIdx,
			Data1MapIdx,
			RsmNormalsIdx,
			RsmPositionIdx,
			RsmFluxIdx,
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

		ShaderPtr getDirectionalPixelShaderSource( uint32_t width
			, uint32_t height )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			UBO_RSM_CONFIG( writer, RsmCfgUboIdx, 0u );
			ArraySsboT< Vec2 > c3d_rsmSamples{ writer
				, "c3d_rsmSamples"
				, writer.getTypesCache().getVec2F()
				, type::MemoryLayout::eStd430
				, RsmSamplesIdx
				, 0u
				, true };
			UBO_GPINFO( writer, GpInfoUboIdx, 0u );
			auto c3d_sLights = writer.declSampledImage< FImgBufferRgba32 >( "c3d_sLights", LightsMapIdx, 0u );
			auto c3d_mapDepth = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eDepth ), DepthMapIdx, 0u );
			auto c3d_mapData1 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData1 ), Data1MapIdx, 0u );
			auto c3d_rsmNormalMap = writer.declSampledImage< FImg2DArrayRgba32 >( getTextureName( LightType::eDirectional, SmTexture::eNormalLinear ), RsmNormalsIdx, 0u );
			auto c3d_rsmPositionMap = writer.declSampledImage< FImg2DArrayRgba32 >( getTextureName( LightType::eDirectional, SmTexture::ePosition ), RsmPositionIdx, 0u );
			auto c3d_rsmFluxMap = writer.declSampledImage< FImg2DArrayRgba32 >( getTextureName( LightType::eDirectional, SmTexture::eFlux ), RsmFluxIdx, 0u );
			auto in = writer.getIn();

			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_rsmGI = writer.declOutput< Vec3 >( "pxl_rsmGI", 0 );
			auto pxl_rsmNormal = writer.declOutput< Vec3 >( "pxl_rsmNormal", 1 );

			// Utility functions
			shader::Utils utils{ writer };
			utils.declareCalcWSPosition();
			utils.declareCalcVSPosition();

			uint32_t index = 0;
			auto lightingModel = shader::PhongLightingModel::createModel( writer
				, utils
				, LightType::eDirectional
				, shader::ShadowOptions{ true }
				, index
				, 1u );
			ReflectiveShadowMapping rsm{ writer, c3d_rsmSamples, LightType::eDirectional };

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
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
					auto vsPosition = writer.declLocale( "vsPosition"
						, c3d_gpInfoData.projToView( utils, texCoord, depth ) );
					auto wsPosition = writer.declLocale( "wsPosition"
						, c3d_gpInfoData.projToWorld( utils, texCoord, depth ) );
					auto wsNormal = writer.declLocale( "wsNormal"
						, data1.xyz() );
					auto light = writer.declLocale( "light"
						, lightingModel->getDirectionalLight( 0_i ) );
					pxl_rsmGI = rsm.directional( light
						, vsPosition
						, wsPosition
						, wsNormal
						, c3d_rsmConfigData );
					pxl_rsmNormal = wsNormal;
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		ShaderPtr getSpotPixelShaderSource( uint32_t width
			, uint32_t height )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			UBO_RSM_CONFIG( writer, RsmCfgUboIdx, 0u );
			ArraySsboT< Vec2 > c3d_rsmSamples{ writer
				, "c3d_rsmSamples"
				, writer.getTypesCache().getVec2F()
				, type::MemoryLayout::eStd430
				, RsmSamplesIdx
				, 0u
				, true };
			UBO_GPINFO( writer, GpInfoUboIdx, 0u );
			auto c3d_sLights = writer.declSampledImage< FImgBufferRgba32 >( "c3d_sLights", LightsMapIdx, 0u );
			auto c3d_mapDepth = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eDepth ), DepthMapIdx, 0u );
			auto c3d_mapData1 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData1 ), Data1MapIdx, 0u );
			auto c3d_rsmNormalMap = writer.declSampledImage< FImg2DArrayRgba32 >( getTextureName( LightType::eSpot, SmTexture::eNormalLinear ), RsmNormalsIdx, 0u );
			auto c3d_rsmPositionMap = writer.declSampledImage< FImg2DArrayRgba32 >( getTextureName( LightType::eSpot, SmTexture::ePosition ), RsmPositionIdx, 0u );
			auto c3d_rsmFluxMap = writer.declSampledImage< FImg2DArrayRgba32 >( getTextureName( LightType::eSpot, SmTexture::eFlux ), RsmFluxIdx, 0u );
			auto in = writer.getIn();

			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_rsmGI = writer.declOutput< Vec3 >( "pxl_rsmGI", 0 );
			auto pxl_rsmNormal = writer.declOutput< Vec3 >( "pxl_rsmNormal", 1 );

			// Utility functions
			shader::Utils utils{ writer };
			utils.declareCalcWSPosition();

			uint32_t index = 0;
			auto lightingModel = shader::PhongLightingModel::createModel( writer
				, utils
				, LightType::eSpot
				, shader::ShadowOptions{ true }
				, index
				, 1u );
			ReflectiveShadowMapping rsm{ writer, c3d_rsmSamples, LightType::eSpot };

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
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
						, c3d_gpInfoData.projToWorld( utils, texCoord, depth ) );
					auto wsNormal = writer.declLocale( "wsNormal"
						, data1.xyz() );

					IF( writer, dot( wsNormal, wsNormal ) == 0 )
					{
						writer.discard();
					}
					FI;

					auto light = writer.declLocale( "light"
						, lightingModel->getSpotLight( 0_i ) );
					pxl_rsmGI = rsm.spot( light
						, wsPosition
						, wsNormal
						, c3d_rsmConfigData );
					pxl_rsmNormal = wsNormal;
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		ShaderPtr getPointPixelShaderSource( uint32_t width
			, uint32_t height )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			UBO_RSM_CONFIG( writer, RsmCfgUboIdx, 0u );
			ArraySsboT< Vec2 > c3d_rsmSamples{ writer
				, "c3d_rsmSamples"
				, writer.getTypesCache().getVec2F()
				, type::MemoryLayout::eStd430
				, RsmSamplesIdx
				, 0u
				, true };
			UBO_GPINFO( writer, GpInfoUboIdx, 0u );
			auto c3d_sLights = writer.declSampledImage< FImgBufferRgba32 >( "c3d_sLights", LightsMapIdx, 0u );
			auto c3d_mapDepth = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eDepth ), DepthMapIdx, 0u );
			auto c3d_mapData1 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData1 ), Data1MapIdx, 0u );
			auto c3d_rsmNormalMap = writer.declSampledImage< FImgCubeArrayRgba32 >( getTextureName( LightType::ePoint, SmTexture::eNormalLinear ), RsmNormalsIdx, 0u );
			auto c3d_rsmPositionMap = writer.declSampledImage< FImgCubeArrayRgba32 >( getTextureName( LightType::ePoint, SmTexture::ePosition ), RsmPositionIdx, 0u );
			auto c3d_rsmFluxMap = writer.declSampledImage< FImgCubeArrayRgba32 >( getTextureName( LightType::ePoint, SmTexture::eFlux ), RsmFluxIdx, 0u );

			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_rsmGI = writer.declOutput< Vec3 >( "pxl_rsmGI", 0 );
			auto pxl_rsmNormal = writer.declOutput< Vec3 >( "pxl_rsmNormal", 1 );

			// Utility functions
			shader::Utils utils{ writer };
			utils.declareCalcWSPosition();

			uint32_t index = 0;
			auto lightingModel = shader::PhongLightingModel::createModel( writer
				, utils
				, LightType::ePoint
				, shader::ShadowOptions{ true }
				, index
				, 1u );
			ReflectiveShadowMapping rsm{ writer, c3d_rsmSamples, LightType::ePoint };

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					auto texCoord = writer.declLocale( "texCoord"
						, vtx_texture );
					auto data1 = writer.declLocale( "data1"
						, c3d_mapData1.lod( texCoord, 0.0_f ) );
					auto depth = writer.declLocale( "depth"
						, c3d_mapDepth.lod( texCoord, 0.0_f ).x() );
					auto wsPosition = writer.declLocale( "wsPosition"
						, c3d_gpInfoData.projToWorld( utils, texCoord, depth ) );
					auto wsNormal = writer.declLocale( "wsNormal"
						, data1.xyz() );
					auto light = writer.declLocale( "light"
						, lightingModel->getPointLight( 0_i ) );
					pxl_rsmGI = rsm.point( light
						, wsPosition
						, wsNormal
						, c3d_rsmConfigData );
					pxl_rsmNormal = wsNormal;
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		std::unique_ptr< ast::Shader > getPixelProgram( LightType lightType
			, uint32_t width
			, uint32_t height )
		{
			switch ( lightType )
			{
			case LightType::eDirectional:
				return getDirectionalPixelShaderSource( width, height );
			case LightType::eSpot:
				return getSpotPixelShaderSource( width, height );
			case LightType::ePoint:
				return getPointPixelShaderSource( width, height );
			default:
				CU_Failure( "Unexpected MaterialType" );
				return nullptr;
			}
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
				sampler->setBorderColour( VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK );
			}

			return sampler;
		}

		crg::ImageId doCreateImage( crg::FrameGraph & graph
			, castor::String const & name
			, VkFormat format
			, VkExtent2D const & size )
		{
			return graph.createImage( crg::ImageData{ name
				, 0u
				, VK_IMAGE_TYPE_2D
				, format
				, { size.width, size.height, 1u }
				, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
					| VK_IMAGE_USAGE_SAMPLED_BIT ) } );
		}

		crg::ImageIdArray doCreateImages( crg::FrameGraph & graph
			, castor::String const & name
			, VkFormat format1
			, VkFormat format2
			, VkExtent2D const & size )
		{
			crg::ImageIdArray result;
			result.emplace_back( doCreateImage( graph, name, format1, size ) );
			result.emplace_back( doCreateImage( graph, name, format2, size ) );
			return result;
		}

		rq::BindingDescriptionArray createBindings()
		{
			return
			{
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, std::nullopt },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, std::nullopt },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, std::nullopt },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, std::nullopt },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_IMAGE_VIEW_TYPE_2D },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_IMAGE_VIEW_TYPE_2D },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_IMAGE_VIEW_TYPE_2D },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_IMAGE_VIEW_TYPE_2D },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_IMAGE_VIEW_TYPE_2D },
			};
		}
	}

	//*********************************************************************************************

	RsmGIPass::RsmGIPass( crg::FrameGraph & graph
		, crg::FramePass const *& previousPass
		, RenderDevice const & device
		, LightCache const & lightCache
		, LightType lightType
		, VkExtent2D const & size
		, GpInfoUbo const & gpInfo
		, OpaquePassResult const & gpResult
		, ShadowMapResult const & smResult
		, crg::ImageViewIdArray const & downscaleResult )
		: RenderQuad{ device
			, castor3d::getName( lightType ) + "RsmGI"
			, VK_FILTER_LINEAR
			, { createBindings()
				, ashes::nullopt
				, rq::Texcoord{}
				, BlendMode::eNoBlend } }
		, m_lightCache{ lightCache }
		, m_gpResult{ gpResult }
		, m_smResult{ smResult }
		, m_gpInfo{ gpInfo }
		, m_result{ doCreateImages( graph
			, getName() + "Result"
			, getFormat( downscaleResult[0u] )
			, getFormat( downscaleResult[1u] )
			, size ) }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, getName(), getVertexProgram() }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, getName(), getPixelProgram( lightType, size.width, size.height ) }
		, m_rsmConfigUbo{ device }
		, m_rsmSamplesSsbo{ makeBuffer< castor::Point2f >( m_device
			, RsmConfig::MaxRange
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, "RsmSamples" ) }
	{
		if ( auto buffer = m_rsmSamplesSsbo->lock( 0u, RsmConfig::MaxRange, 0u ) )
		{
			std::random_device rd;
			std::mt19937 rng( rd() );
			std::uniform_real_distribution< float > dist( 0.0f, 1.0f );

			for ( auto & point : makeArrayView( buffer, buffer + RsmConfig::MaxRange ) )
			{
				auto rnd = dist( rng );
				auto twoPIy = PiMult2< float > * rnd;
				rnd = dist( rng );
				point[0] = rnd * sin( twoPIy );
				point[1] = rnd * cos( twoPIy );
			}

			m_rsmSamplesSsbo->flush( 0u, RsmConfig::MaxRange );
			m_rsmSamplesSsbo->unlock();
		}

		ashes::PipelineShaderStageCreateInfoArray shaderStages;
		shaderStages.push_back( makeShaderState( m_device, m_vertexShader ) );
		shaderStages.push_back( makeShaderState( m_device, m_pixelShader ) );
		createPipelineAndPass( size
			, {}
			, shaderStages
			, *m_renderPass
			, {
				makeDescriptorWrite( m_rsmConfigUbo.getUbo()
					, RsmCfgUboIdx ),
				makeDescriptorWrite( m_rsmSamplesSsbo->getBuffer()
					, RsmSamplesIdx
					, 0u
					, uint32_t( m_rsmSamplesSsbo->getMemoryRequirements().size ) ),
				makeDescriptorWrite( m_gpInfo.getUbo()
					, GpInfoUboIdx ),
				makeDescriptorWrite( m_lightCache.getBuffer()
					, m_lightCache.getView()
					, LightsMapIdx ),
				// TODO CRG
				//makeDescriptorWrite( m_gpResult[DsTexture::eDepth].getTexture()->getDefaultView().getSampledView()
				//	, m_gpResult[DsTexture::eDepth].getSampler()->getSampler()
				//	, DepthMapIdx ),
				//makeDescriptorWrite( m_gpResult[DsTexture::eData1].getTexture()->getDefaultView().getSampledView()
				//	, m_gpResult[DsTexture::eData1].getSampler()->getSampler()
				//	, Data1MapIdx ),
				//makeDescriptorWrite( m_smResult[SmTexture::eNormalLinear].getTexture()->getDefaultView().getSampledView()
				//	, m_smResult[SmTexture::eNormalLinear].getSampler()->getSampler()
				//	, RsmNormalsIdx ),
				//makeDescriptorWrite( m_smResult[SmTexture::ePosition].getTexture()->getDefaultView().getSampledView()
				//	, m_smResult[SmTexture::ePosition].getSampler()->getSampler()
				//	, RsmPositionIdx ),
				//makeDescriptorWrite( m_smResult[SmTexture::eFlux].getTexture()->getDefaultView().getSampledView()
				//	, m_smResult[SmTexture::eFlux].getSampler()->getSampler()
				//	, RsmFluxIdx ),
			}
			, {} );
		m_commands = getCommands( *m_timer, 0u );
	}

	ashes::Semaphore const & RsmGIPass::compute( ashes::Semaphore const & toWait )const
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

	CommandsSemaphore RsmGIPass::getCommands( RenderPassTimer const & timer
		, uint32_t index )const
	{
		castor3d::CommandsSemaphore commands
		{
			m_device.graphicsCommandPool->createCommandBuffer( getName() ),
			m_device->createSemaphore( getName() )
		};
		auto & cmd = *commands.commandBuffer;

		// TODO CRG
		//cmd.begin();
		//timer.beginPass( cmd, index );
		//cmd.beginDebugBlock(
		//	{
		//		"Lighting - RSM GI",
		//		castor3d::makeFloatArray( m_renderSystem.getEngine()->getNextRainbowColour() ),
		//	} );
		//cmd.memoryBarrier( VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT
		//	, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
		//	, m_gpResult[DsTexture::eDepth].getTexture()->getDefaultView().getTargetView().makeShaderInputResource( VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL ) );
		//cmd.beginRenderPass( *m_renderPass
		//	, *m_frameBuffer
		//	, { castor3d::transparentBlackClearColor, castor3d::transparentBlackClearColor }
		//	, VK_SUBPASS_CONTENTS_INLINE );
		//registerPass( cmd );
		//cmd.endRenderPass();
		//cmd.memoryBarrier( VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
		//	, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT
		//	, m_gpResult[DsTexture::eDepth].getTexture()->getDefaultView().getTargetView().makeDepthStencilReadOnly( VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) );
		//cmd.endDebugBlock();
		//timer.endPass( cmd, index );
		//cmd.end();

		return commands;
	}

	void RsmGIPass::accept( PipelineVisitorBase & visitor )
	{
		visitor.visit( getName() + " GI"
			, m_resultViews[0]
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, TextureFactors{}.invert( true ) );
		visitor.visit( getName() + " Normal"
			, m_resultViews[1]
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, TextureFactors{}.invert( true ) );

		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );
	}

	void RsmGIPass::update( Light const & light )
	{
		m_rsmConfigUbo.update( light );
	}
}
