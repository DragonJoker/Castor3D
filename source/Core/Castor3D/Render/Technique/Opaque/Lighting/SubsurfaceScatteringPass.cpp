#include "Castor3D/Render/Technique/Opaque/Lighting/SubsurfaceScatteringPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Buffer/UniformBuffer.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaquePassResult.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightPassResult.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslSssTransmittance.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/GpInfoUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/Pipeline/PipelineVertexInputStateCreateInfo.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

#include <random>

CU_ImplementCUSmartPtr( castor3d, SubsurfaceScatteringPass );

namespace castor3d
{
	namespace
	{
		ShaderPtr doGetVertexProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			auto position = writer.declInput< Vec2 >( "position", 0u );
			auto uv = writer.declInput< Vec2 >( "uv", 1u );

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

		enum BlurId : size_t
		{
			BlurSceneUboId = 1u,
			BlurGpInfoUboId,
			BlurSssUboId,
			BlurDepthImgId,
			BlurData4ImgId,
			BlurData5ImgId,
			BlurLgtDiffImgId,
		};

		ShaderPtr doGetBlurProgram( Engine & engine
			, bool isVertic )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			auto materials = shader::createMaterials( writer, PassFlag( 0u ) );
			materials->declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
			UBO_SCENE( writer, BlurSceneUboId, 0u );
			UBO_GPINFO( writer, BlurGpInfoUboId, 0u );
			Ubo config{ writer, SubsurfaceScatteringPass::Config, BlurSssUboId, 0u };
			auto c3d_pixelSize = config.declMember< Vec2 >( SubsurfaceScatteringPass::PixelSize );
			auto c3d_correction = config.declMember< Float >( SubsurfaceScatteringPass::Correction );
			config.end();
			auto c3d_mapDepth = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eDepth ), BlurDepthImgId, 0u );
			auto c3d_mapData4 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData4 ), BlurData4ImgId, 0u );
			auto c3d_mapData5 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData5 ), BlurData5ImgId, 0u );
			auto c3d_mapLightDiffuse = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapLightDiffuse", BlurLgtDiffImgId, 0u );

			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			shader::Utils utils{ writer };
			utils.declareCalcVSPosition();
			utils.declareInvertVec2Y();

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0 );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					auto data4 = writer.declLocale( "data4"
						, c3d_mapData4.lod( vtx_texture, 0.0_f ) );
					auto data5 = writer.declLocale( "data5"
						, c3d_mapData5.lod( vtx_texture, 0.0_f ) );
					auto materialId = writer.declLocale( "materialId"
						, writer.cast< UInt >( data5.z() ) );
					auto translucency = writer.declLocale( "translucency"
						, data4.w() );
					auto material = materials->getBaseMaterial( materialId );

					IF( writer, material->m_subsurfaceScatteringEnabled == 0_i )
					{
						writer.discard();
					}
					FI;

					// Fetch color and linear depth for current pixel:
					auto colorM = writer.declLocale( "colorM"
						, c3d_mapLightDiffuse.lod( vtx_texture, 0.0_f ) );
					auto depthM = writer.declLocale( "depthM"
						, c3d_mapDepth.lod( vtx_texture, 0.0_f ).r() );
					depthM = utils.calcVSPosition( vtx_texture
						, depthM
						, c3d_mtxInvProj ).z();

					// Accumulate center sample, multiplying it with its gaussian weight:
					pxl_fragColor = colorM;
					pxl_fragColor.rgb() *= 0.382_f;

					if ( isVertic )
					{
						auto step = writer.declLocale( "step"
							, c3d_pixelSize * vec2( 0.0_f, 1.0_f ) );
					}
					else
					{
						auto step = writer.declLocale( "step"
							, c3d_pixelSize * vec2( 1.0_f, 0.0_f ) );
					}

					auto step = writer.getVariable< Vec2 >( "step" );

					// Calculate the step that we will use to fetch the surrounding pixels,
					// where "step" is:
					//     step = sssStrength * gaussianWidth * pixelSize * dir
					// The closer the pixel, the stronger the effect needs to be, hence
					// the factor 1.0 / depthM.
					auto finalStep = writer.declLocale( "finalStep"
						, translucency * step * material->m_subsurfaceScatteringStrength * material->m_gaussianWidth / depthM );

					auto offset = writer.declLocale< Vec2 >( "offset" );
					auto color = writer.declLocale< Vec3 >( "color" );
					auto depth = writer.declLocale< Float >( "depth" );
					auto s = writer.declLocale< Float >( "s" );

					// Gaussian weights for the six samples around the current pixel:
					//   -3 -2 -1 +1 +2 +3
					auto w = writer.declLocaleArray( "w"
						, 6u
						, std::vector< Float >{ { 0.006_f, 0.061_f, 0.242_f, 0.242_f, 0.061_f, 0.006_f } } );
					auto o = writer.declLocaleArray( "o"
						, 6u
						, std::vector< Float >{ { -1.0_f, -0.666666667_f, -0.333333333_f, 0.333333333_f, 0.666666667_f, 1.0_f } } );

					// Accumulate the other samples:
					FOR( writer, Int, i, 0_i, i < 6_i, ++i )
					{
						// Fetch color and depth for current sample:
						offset = sdw::fma( vec2( o[i] ), finalStep, vtx_texture );
						color = c3d_mapLightDiffuse.lod( offset, 0.0_f ).rgb();
						offset = sdw::fma( vec2( o[i] ), finalStep, vtx_texture );
						depth = c3d_mapDepth.lod( offset, 0.0_f ).r();
						depth = utils.calcVSPosition( vtx_texture
							, depth
							, c3d_mtxInvProj ).z();

						// If the difference in depth is huge, we lerp color back to "colorM":
						s = min( 0.0125_f * c3d_correction * abs( depthM - depth ), 1.0_f );
						color = mix( color, colorM.rgb(), vec3( s ) );

						// Accumulate:
						pxl_fragColor.rgb() += w[i] * color;
					}
					ROF;
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		enum CombId : size_t
		{
			CombData4ImgId = 1u,
			CombData5ImgId,
			CombBlur1ImgId,
			CombBlur2ImgId,
			CombBlur3ImgId,
			CombLgtDiffImgId,
		};

		ShaderPtr doGetCombineProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			auto materials = shader::createMaterials( writer, PassFlag( 0u ) );
			materials->declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );

			auto c3d_mapData4 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData4 ), CombData4ImgId, 0u );
			auto c3d_mapData5 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData5 ), CombData5ImgId, 0u );
			auto c3d_mapBlur1 = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapBlur1", CombBlur1ImgId, 0u );
			auto c3d_mapBlur2 = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapBlur2", CombBlur2ImgId, 0u );
			auto c3d_mapBlur3 = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapBlur3", CombBlur3ImgId, 0u );
			auto c3d_mapLightDiffuse = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapLightDiffuse", CombLgtDiffImgId, 0u );

			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0 );

			shader::Utils utils{ writer };
			utils.declareInvertVec2Y();

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					auto data5 = writer.declLocale( "data5"
						, c3d_mapData5.lod( vtx_texture, 0.0_f ) );
					auto original = writer.declLocale( "original"
						, c3d_mapLightDiffuse.lod( vtx_texture, 0.0_f ) );
					auto materialId = writer.declLocale( "materialId"
						, writer.cast< UInt >( data5.z() ) );
					auto material = materials->getBaseMaterial( materialId );

					IF( writer, material->m_subsurfaceScatteringEnabled == 0_i )
					{
						pxl_fragColor = original;
					}
					ELSE
					{
						auto originalWeight = writer.declLocale< Vec4 >( "originalWeight"
							, vec4( 0.2406_f, 0.4475_f, 0.6159_f, 0.25_f ) );
						auto blurWeights = writer.declLocaleArray< Vec4 >( "blurWeights"
							, 3u
							, {
								vec4( 0.1158_f, 0.3661_f, 0.3439_f, 0.25_f ),
								vec4( 0.1836_f, 0.1864_f, 0.0_f, 0.25_f ),
								vec4( 0.46_f, 0.0_f, 0.0402_f, 0.25_f )
							} );
						auto blurVariances = writer.declLocaleArray< Float >( "blurVariances"
							, 3u
							, {
								0.0516_f,
								0.2719_f,
								2.0062_f
							} );
						auto blur1 = writer.declLocale( "blur1"
							, c3d_mapBlur1.lod( vtx_texture, 0.0_f ) );
						auto blur2 = writer.declLocale( "blur2"
							, c3d_mapBlur2.lod( vtx_texture, 0.0_f ) );
						auto blur3 = writer.declLocale( "blur3"
							, c3d_mapBlur3.lod( vtx_texture, 0.0_f ) );
						auto data4 = writer.declLocale( "data4"
							, c3d_mapData4.lod( vtx_texture, 0.0_f ) );
						auto translucency = writer.declLocale( "translucency"
							, data4.w() );
						pxl_fragColor = original * originalWeight
							+ blur1 * blurWeights[0]
							+ blur2 * blurWeights[1]
							+ blur3 * blurWeights[2];
					}
					FI;
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		ashes::PipelineShaderStageCreateInfoArray doCreateBlurProgram( Engine & engine
			, RenderDevice const & device
			, bool isVertic
			, ShaderModule & vertexShader
			, ShaderModule & pixelShader )
		{
			vertexShader.shader = doGetVertexProgram( engine );
			pixelShader.shader = doGetBlurProgram( engine, isVertic );

			return ashes::PipelineShaderStageCreateInfoArray
			{
				makeShaderState( device, vertexShader ),
				makeShaderState( device, pixelShader ),
			};
		}

		ashes::PipelineShaderStageCreateInfoArray doCreateCombineProgram( Engine & engine
			, RenderDevice const & device
			, ShaderModule & vertexShader
			, ShaderModule & pixelShader )
		{
			vertexShader.shader = doGetVertexProgram( engine );
			pixelShader.shader = doGetCombineProgram( engine );

			return ashes::PipelineShaderStageCreateInfoArray
			{
				makeShaderState( device, vertexShader ),
				makeShaderState( device, pixelShader ),
			};
		}

		SamplerSPtr doCreateSampler( Engine & engine )
		{
			SamplerSPtr sampler;
			castor::String const name{ cuT( "SubsurfaceScatteringPass" ) };

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().add( name );
				sampler->setMinFilter( VK_FILTER_NEAREST );
				sampler->setMagFilter( VK_FILTER_NEAREST );
				sampler->setWrapS( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				sampler->setWrapT( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
			}

			return sampler;
		}

		TextureUnit doCreateTexture( Engine & engine
			, RenderDevice const & device
			, castor::Size const & size
			, std::string name )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto sampler = doCreateSampler( engine );
			
			ashes::ImageCreateInfo image
			{
				0u,
				VK_IMAGE_TYPE_2D,
				VK_FORMAT_R32G32B32A32_SFLOAT,
				{ size.getWidth(), size.getHeight(), 1u },
				1u,
				1u,
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_TILING_OPTIMAL,
				( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
					| VK_IMAGE_USAGE_SAMPLED_BIT ),
			};
			auto texture = std::make_shared< TextureLayout >( renderSystem
				, std::move( image )
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, name );
			TextureUnit unit{ engine };
			unit.setTexture( texture );
			unit.setSampler( sampler );
			unit.initialise( device );
			return unit;
		}

		ashes::RenderPassPtr doCreateRenderPass( RenderDevice const & device
			, VkFormat format
			, std::string name )
		{
			ashes::VkAttachmentDescriptionArray attachments
			{
				{
					0u,
					format,
					VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_CLEAR,
					VK_ATTACHMENT_STORE_OP_STORE,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_IMAGE_LAYOUT_UNDEFINED,
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				}
			};
			ashes::SubpassDescriptionArray subpasses;
			subpasses.emplace_back( ashes::SubpassDescription
				{
					0u,
					VK_PIPELINE_BIND_POINT_GRAPHICS,
					{},
					{ { 0u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } },
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
				std::move( attachments ),
				std::move( subpasses ),
				std::move( dependencies ),
			};
			auto result = device->createRenderPass( std::move( name )
				, std::move( createInfo ) );
			return result;
		}

		ashes::FrameBufferPtr doCreateFrameBuffer( ashes::RenderPass const & renderPass
			, castor::Size const & size
			, ashes::ImageView const & view
			, std::string name )
		{
			ashes::ImageViewCRefArray attaches;
			attaches.emplace_back( view );
			auto result = renderPass.createFrameBuffer( std::move( name )
				, { size.getWidth(), size.getHeight() }
				, std::move( attaches ) );
			return result;
		}

		rq::BindingDescriptionArray createBlurBindings( PassBuffer const & buffer )
		{
			return rq::BindingDescriptionArray
			{
				{ buffer.getType(), ashes::nullopt },	// Pass Buffer
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, ashes::nullopt },	// Scene UBO
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, ashes::nullopt },	// GpInfo UBO
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, ashes::nullopt },	// Blur UBO
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_IMAGE_VIEW_TYPE_2D },	// Depth
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_IMAGE_VIEW_TYPE_2D },	// Translucency
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_IMAGE_VIEW_TYPE_2D },	// MaterialIndex
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_IMAGE_VIEW_TYPE_2D },	// LightDiffuse
			};
		}

		rq::BindingDescriptionArray createCombineBindings( PassBuffer const & buffer )
		{
			return rq::BindingDescriptionArray
			{
				{ buffer.getType(), ashes::nullopt },	// Pass Buffer
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_IMAGE_VIEW_TYPE_2D },	// Translucency
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_IMAGE_VIEW_TYPE_2D },	// MaterialIndex
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_IMAGE_VIEW_TYPE_2D },	// BlurResult 0
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_IMAGE_VIEW_TYPE_2D },	// BlurResult 1
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_IMAGE_VIEW_TYPE_2D },	// BlurResult 2
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_IMAGE_VIEW_TYPE_2D },	// LightDiffuse
			};
		}
	}

	//*********************************************************************************************

	SubsurfaceScatteringPass::Blur::Blur( RenderSystem & renderSystem
		, RenderDevice const & device
		, castor::Size const & size
		, GpInfoUbo const & gpInfoUbo
		, SceneUbo & sceneUbo
		, OpaquePassResult const & gpResult
		, TextureUnit const & source
		, TextureUnit const & destination
		, bool isVertic
		, ashes::PipelineShaderStageCreateInfoArray const & shaderStages )
		: RenderQuad{ device
			, "SubscatteringBlur"
			, VK_FILTER_LINEAR
			, { createBlurBindings( renderSystem.getEngine()->getMaterialCache().getPassBuffer() )
				, ashes::nullopt
				, rq::Texcoord{} } }
		, m_geometryBufferResult{ gpResult }
		, m_gpInfoUbo{ gpInfoUbo }
		, m_sceneUbo{ sceneUbo }
		, m_blurUbo{ m_device.uboPools->getBuffer< BlurConfiguration >( 0u ) }
		, m_renderPass{ doCreateRenderPass( m_device, destination.getTexture()->getPixelFormat(), "SubscatteringBlur" ) }
		, m_frameBuffer{ doCreateFrameBuffer( *m_renderPass, size, destination.getTexture()->getDefaultView().getTargetView(), "SubscatteringBlur" ) }
	{
		auto & configuration = m_blurUbo.getData();
		configuration.blurCorrection = 1.0f;
		configuration.blurPixelSize = castor::Point2f{ 1.0f / size.getWidth(), 1.0f / size.getHeight() };
		VkExtent2D extent{ size.getWidth(), size.getHeight() };
		createPipelineAndPass( extent
			, castor::Position{}
			, shaderStages
			, *m_renderPass
			, {
				m_renderSystem.getEngine()->getMaterialCache().getPassBuffer().getBinding(),
				makeDescriptorWrite( m_sceneUbo.getUbo(), BlurSceneUboId ),
				makeDescriptorWrite( m_gpInfoUbo.getUbo(), BlurGpInfoUboId ),
				makeDescriptorWrite( m_blurUbo, BlurSssUboId ),
				makeDescriptorWrite( m_geometryBufferResult[DsTexture::eDepth].getTexture()->getDefaultView().getSampledView()
					, m_sampler->getSampler()
						, BlurDepthImgId ),
				makeDescriptorWrite( m_geometryBufferResult[DsTexture::eData4].getTexture()->getDefaultView().getSampledView()
					, m_sampler->getSampler()
					, BlurData4ImgId ),
				makeDescriptorWrite( m_geometryBufferResult[DsTexture::eData5].getTexture()->getDefaultView().getSampledView()
					, m_sampler->getSampler()
					, BlurData5ImgId ),
				makeDescriptorWrite( source.getTexture()->getDefaultView().getSampledView()
					, m_sampler->getSampler()
					, BlurLgtDiffImgId ),
			}
			, {} );
	}

	SubsurfaceScatteringPass::Blur::Blur( Blur && rhs )noexcept
		: RenderQuad{ std::forward< RenderQuad >( rhs ) }
		, m_geometryBufferResult{ rhs.m_geometryBufferResult }
		, m_gpInfoUbo{ rhs.m_gpInfoUbo }
		, m_sceneUbo{ rhs.m_sceneUbo }
		, m_blurUbo{ rhs.m_blurUbo }
		, m_renderPass{ std::move( rhs.m_renderPass ) }
		, m_frameBuffer{ std::move( rhs.m_frameBuffer ) }
	{
	}

	void SubsurfaceScatteringPass::Blur::prepareFrame( ashes::CommandBuffer & commandBuffer )const
	{
		commandBuffer.beginDebugBlock(
			{
				"Deferred - Subsurface Subscattering - Blur",
				makeFloatArray( m_renderSystem.getEngine()->getNextRainbowColour() ),
			} );
		commandBuffer.beginRenderPass( *m_renderPass
			, *m_frameBuffer
			, { opaqueBlackClearColor }
			, VK_SUBPASS_CONTENTS_INLINE );
		registerPass( commandBuffer );
		commandBuffer.endRenderPass();
		commandBuffer.endDebugBlock();
	}

	//*********************************************************************************************

	SubsurfaceScatteringPass::Combine::Combine( RenderSystem & renderSystem
		, RenderDevice const & device
		, castor::Size const & size
		, OpaquePassResult const & gpResult
		, TextureUnit const & source
		, SubsurfaceScatteringPass::BlurResult const & blurResults
		, TextureUnit const & destination
		, ashes::PipelineShaderStageCreateInfoArray const & shaderStages )
		: RenderQuad{ device
			, cuT( "SubscatteringCombine" )
			, VK_FILTER_LINEAR
			, { createCombineBindings( renderSystem.getEngine()->getMaterialCache().getPassBuffer() )
				, ashes::nullopt
				, rq::Texcoord{} } }
		, m_blurUbo{ m_device.uboPools->getBuffer< BlurWeights >( 0u ) }
		, m_geometryBufferResult{ gpResult }
		, m_source{ source }
		, m_blurResults{ blurResults }
		, m_renderPass{ doCreateRenderPass( m_device, destination.getTexture()->getPixelFormat(), getName() ) }
		, m_frameBuffer{ doCreateFrameBuffer( *m_renderPass, size, destination.getTexture()->getDefaultView().getTargetView(), getName() ) }
	{
		auto & weights = m_blurUbo.getData();
		weights.originalWeight = castor::Point4f{ 0.2406f, 0.4475f, 0.6159f, 0.25f };
		weights.blurWeights[0] = castor::Point4f{ 0.1158, 0.3661, 0.3439, 0.25 };
		weights.blurWeights[1] = castor::Point4f{ 0.1836, 0.1864, 0.0, 0.25 };
		weights.blurWeights[2] = castor::Point4f{ 0.46, 0.0, 0.0402, 0.25 };
		weights.blurVariance = castor::Point4f{ 0.0516, 0.2719, 2.0062 };
		VkExtent2D extent{ size.getWidth(), size.getHeight() };
		createPipelineAndPass( extent
			, castor::Position{}
			, shaderStages
			, *m_renderPass
			, {
				m_renderSystem.getEngine()->getMaterialCache().getPassBuffer().getBinding(),
				makeDescriptorWrite( m_geometryBufferResult[DsTexture::eData4].getTexture()->getDefaultView().getSampledView()
					, m_sampler->getSampler()
					, CombData4ImgId ),
				makeDescriptorWrite( m_geometryBufferResult[DsTexture::eData5].getTexture()->getDefaultView().getSampledView()
					, m_sampler->getSampler()
					, CombData5ImgId ),
				makeDescriptorWrite( m_blurResults[0].getTexture()->getDefaultView().getSampledView()
					, m_blurResults[0].getSampler()->getSampler()
					, CombBlur1ImgId ),
				makeDescriptorWrite( m_blurResults[1].getTexture()->getDefaultView().getSampledView()
					, m_blurResults[1].getSampler()->getSampler()
					, CombBlur2ImgId ),
				makeDescriptorWrite( m_blurResults[2].getTexture()->getDefaultView().getSampledView()
					, m_blurResults[2].getSampler()->getSampler()
					, CombBlur3ImgId ),
				makeDescriptorWrite( source.getTexture()->getDefaultView().getSampledView()
					, m_sampler->getSampler()
					, CombLgtDiffImgId ),
			}
			, {} );
	}

	SubsurfaceScatteringPass::Combine::Combine( Combine && rhs )noexcept
		: RenderQuad{ std::forward< RenderQuad >( rhs ) }
		, m_blurUbo{ rhs.m_blurUbo }
		, m_geometryBufferResult{ rhs.m_geometryBufferResult }
		, m_source{ rhs.m_source }
		, m_blurResults{ rhs.m_blurResults }
		, m_renderPass{ std::move( rhs.m_renderPass ) }
		, m_frameBuffer{ std::move( rhs.m_frameBuffer ) }
	{
	}

	void SubsurfaceScatteringPass::Combine::prepareFrame( ashes::CommandBuffer & commandBuffer )const
	{
		static auto const red{ makeClearValue( 0, 1, 0, 1 ) };
		commandBuffer.beginDebugBlock(
			{
				"Deferred - Subsurface Subscattering - Combine",
				makeFloatArray( m_renderSystem.getEngine()->getNextRainbowColour() ),
			} );
		commandBuffer.beginRenderPass( *m_renderPass
			, *m_frameBuffer
			, { red }
			, VK_SUBPASS_CONTENTS_INLINE );
		registerPass( commandBuffer );
		commandBuffer.endRenderPass();
	}

	//*********************************************************************************************

	castor::String const SubsurfaceScatteringPass::Config = "Config";
	castor::String const SubsurfaceScatteringPass::Step = "c3d_step";
	castor::String const SubsurfaceScatteringPass::Correction = "c3d_correction";
	castor::String const SubsurfaceScatteringPass::PixelSize = "c3d_pixelSize";
	castor::String const SubsurfaceScatteringPass::Weights = "c3d_weights";
	castor::String const SubsurfaceScatteringPass::Offsets = "c3d_offsets";

	SubsurfaceScatteringPass::SubsurfaceScatteringPass( Engine & engine
		, RenderDevice const & device
		, GpInfoUbo const & gpInfoUbo
		, SceneUbo & sceneUbo
		, castor::Size const & textureSize
		, OpaquePassResult const & gpResult
		, LightPassResult const & lpResult )
		: OwnedBy< Engine >{ engine }
		, m_gpInfoUbo{ gpInfoUbo }
		, m_sceneUbo{ sceneUbo }
		, m_gpResult{ gpResult }
		, m_lpResult{ lpResult }
		, m_size{ textureSize }
		, m_intermediate{ doCreateTexture( engine, device, textureSize, "SubsurfaceScattering intermediate" ) }
		, m_blurHorizVertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "SssBlurX" }
		, m_blurHorizPixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "SssBlurX" }
		, m_blurVerticVertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "SssBlurY" }
		, m_blurVerticPixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "SssBlurY" }
		, m_combineVertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "SssCombine" }
		, m_combinePixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "SssCombine" }
		, m_blurResults
		{
			{
				doCreateTexture( engine, device, textureSize, "SubsurfaceScattering Blur 0" ),
				doCreateTexture( engine, device, textureSize, "SubsurfaceScattering Blur 1" ),
				doCreateTexture( engine, device, textureSize, "SubsurfaceScattering Blur 2" ),
			}
		}
		, m_result{ doCreateTexture( engine, device, textureSize, "SubsurfaceScattering Result" ) }
	{
	}

	void SubsurfaceScatteringPass::initialise( RenderDevice const & device )
	{
		auto & engine = *getEngine();

		m_intermediate.initialise( device );
		m_result.initialise( device );

		for ( auto & result : m_blurResults )
		{
			result.initialise( device );
		}

		// Combine pass
		auto combineProgram = doCreateCombineProgram( engine, device, m_combineVertexShader, m_combinePixelShader );
		m_combine = std::make_unique< Combine >( *engine.getRenderSystem()
			, device
			, m_size
			, m_gpResult
			, m_lpResult[LpTexture::eDiffuse]
			, m_blurResults
			, m_result
			, combineProgram );

		// Horizontal blur pass
		auto blurHorizProgram = doCreateBlurProgram( engine, device, false, m_blurHorizVertexShader, m_blurHorizPixelShader );
		m_blurX =
		{
			std::make_unique< Blur >( *engine.getRenderSystem()
				, device
				, m_size
				, m_gpInfoUbo
				, m_sceneUbo
				, m_gpResult
				, m_lpResult[LpTexture::eDiffuse]
				, m_intermediate
				, false
				, blurHorizProgram ),
			std::make_unique< Blur >( *engine.getRenderSystem()
				, device
				, m_size
				, m_gpInfoUbo
				, m_sceneUbo
				, m_gpResult
				, m_blurResults[0]
				, m_intermediate
				, false
				, blurHorizProgram ),
			std::make_unique< Blur >( *engine.getRenderSystem()
				, device
				, m_size
				, m_gpInfoUbo
				, m_sceneUbo
				, m_gpResult
				, m_blurResults[1]
				, m_intermediate
				, false
				, blurHorizProgram ),
		};

		// Vertical blur pass
		auto blurVerticProgram = doCreateBlurProgram( engine, device, true, m_blurVerticVertexShader, m_blurVerticPixelShader );
		m_blurY =
		{
			std::make_unique< Blur >( *engine.getRenderSystem()
				, device
				, m_size
				, m_gpInfoUbo
				, m_sceneUbo
				, m_gpResult
				, m_intermediate
				, m_blurResults[0]
				, true
				, blurVerticProgram ),
			std::make_unique< Blur >( *engine.getRenderSystem()
				, device
				, m_size
				, m_gpInfoUbo
				, m_sceneUbo
				, m_gpResult
				, m_intermediate
				, m_blurResults[1]
				, true
				, blurVerticProgram ),
			std::make_unique< Blur >( *engine.getRenderSystem()
				, device
				, m_size
				, m_gpInfoUbo
				, m_sceneUbo
				, m_gpResult
				, m_intermediate
				, m_blurResults[2]
				, true
				, blurVerticProgram ),
		};

		// Commands.
		m_timer = std::make_shared< RenderPassTimer >( engine, device, cuT( "Opaque" ), cuT( "SSSSS pass" ) );
		m_finished = device->createSemaphore( "SSSSS pass" );
		m_commandBuffer = device.graphicsCommandPool->createCommandBuffer( "SSSSS pass" );
		m_commandBuffer->begin();
		m_commandBuffer->beginDebugBlock(
			{
				"Deferred - Subsurface Subscattering",
				makeFloatArray( getEngine()->getNextRainbowColour() ),
			} );
		m_timer->beginPass( *m_commandBuffer );

		for ( size_t i{ 0u }; i < m_blurResults.size(); ++i )
		{
			m_blurX[i]->prepareFrame( *m_commandBuffer );
			m_blurY[i]->prepareFrame( *m_commandBuffer );
		}

		m_combine->prepareFrame( *m_commandBuffer );
		m_timer->endPass( *m_commandBuffer );
		m_commandBuffer->endDebugBlock();
		m_commandBuffer->end();
	}

	void SubsurfaceScatteringPass::cleanup( RenderDevice const & device )
	{
		m_commandBuffer.reset();
		m_finished.reset();
		m_timer.reset();
		m_blurY = { nullptr, nullptr, nullptr };
		m_blurX = { nullptr, nullptr, nullptr };
		m_combine.reset();
		m_result.cleanup();
		m_intermediate.cleanup();

		for ( auto & result : m_blurResults )
		{
			result.cleanup();
		}
	}

	ashes::Semaphore const & SubsurfaceScatteringPass::render( RenderDevice const & device
		, ashes::Semaphore const & toWait )const
	{
		RenderPassTimerBlock timerBlock{ m_timer->start() };
		timerBlock->notifyPassRender();
		auto * result = &toWait;

		device.graphicsQueue->submit( *m_commandBuffer
			, *result
			, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, *m_finished
			, nullptr );
		result = m_finished.get();
		return *result;
	}

	void SubsurfaceScatteringPass::accept( PipelineVisitorBase & visitor )
	{
		for ( size_t i{ 0u }; i < m_blurResults.size(); ++i )
		{
			visitor.visit( "SSSSS Blur " + castor::string::toString( i )
				, m_blurResults[i].getTexture()->getDefaultView().getSampledView()
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		}

		visitor.visit( "SSSSS Result"
			, m_result.getTexture()->getDefaultView().getSampledView()
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );

		visitor.visit( m_blurHorizVertexShader );
		visitor.visit( m_blurHorizPixelShader );

		visitor.visit( m_blurVerticVertexShader );
		visitor.visit( m_blurVerticPixelShader );

		visitor.visit( m_combineVertexShader );
		visitor.visit( m_combinePixelShader );
	}
}
