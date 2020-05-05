#include "Castor3D/Render/Technique/Opaque/SubsurfaceScatteringPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Buffer/UniformBuffer.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Technique/Opaque/GeometryPassResult.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"
#include "Castor3D/Shader/Shaders/GlslSssTransmittance.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/DebugUbo.hpp"
#include "Castor3D/Shader/Ubos/GpInfoUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/Pipeline/PipelineVertexInputStateCreateInfo.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

#include <random>

using namespace castor;
using namespace castor3d;

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

		uint32_t constexpr BlurSceneUboId = 2u;
		uint32_t constexpr BlurGpInfoUboId = 3u;
		uint32_t constexpr BlurSssUboId = 4u;
		uint32_t constexpr BlurDepthImgId = 5u;
		uint32_t constexpr BlurData4ImgId = 6u;
		uint32_t constexpr BlurData5ImgId = 7u;
		uint32_t constexpr BlurLgtDiffImgId = 8u;

		ShaderPtr doGetBlurProgram( Engine & engine
			, bool isVertic )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			auto materials = shader::createMaterials( writer
				, ( engine.getMaterialsType() == MaterialType::eMetallicRoughness
					? PassFlag::eMetallicRoughness
					: ( engine.getMaterialsType() == MaterialType::eSpecularGlossiness
						? PassFlag::eSpecularGlossiness
						: PassFlag( 0u ) ) ) );
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
						, textureLod( c3d_mapData4, vtx_texture, 0.0_f ) );
					auto data5 = writer.declLocale( "data5"
						, textureLod( c3d_mapData5, vtx_texture, 0.0_f ) );
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
						, textureLod( c3d_mapLightDiffuse, vtx_texture, 0.0_f ) );
					auto depthM = writer.declLocale( "depthM"
						, textureLod( c3d_mapDepth, vtx_texture, 0.0_f ).r() );
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
						color = textureLod( c3d_mapLightDiffuse, offset, 0.0_f ).rgb();
						offset = sdw::fma( vec2( o[i] ), finalStep, vtx_texture );
						depth = textureLod( c3d_mapDepth, offset, 0.0_f ).r();
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

		uint32_t constexpr CombData4ImgId = 2u;
		uint32_t constexpr CombData5ImgId = 3u;
		uint32_t constexpr CombBlur1ImgId = 4u;
		uint32_t constexpr CombBlur2ImgId = 5u;
		uint32_t constexpr CombBlur3ImgId = 6u;
		uint32_t constexpr CombLgtDiffImgId = 7u;

		ShaderPtr doGetCombineProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			auto materials = shader::createMaterials( writer
				, ( engine.getMaterialsType() == MaterialType::eMetallicRoughness
					? PassFlag::eMetallicRoughness
					: ( engine.getMaterialsType() == MaterialType::eSpecularGlossiness
						? PassFlag::eSpecularGlossiness
						: PassFlag( 0u ) ) ) );
			materials->declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );

			UBO_DEBUG( writer, 1u, 0u );
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
						, textureLod( c3d_mapData5, vtx_texture, 0.0_f ) );
					auto original = writer.declLocale( "original"
						, textureLod( c3d_mapLightDiffuse, vtx_texture, 0.0_f ) );
					auto materialId = writer.declLocale( "materialId"
						, writer.cast< UInt >( data5.z() ) );
					auto material = materials->getBaseMaterial( materialId );

					IF( writer, c3d_debugDeferredSSSTransmittance == 0_i )
					{
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
								, textureLod( c3d_mapBlur1, vtx_texture, 0.0_f ) );
							auto blur2 = writer.declLocale( "blur2"
								, textureLod( c3d_mapBlur2, vtx_texture, 0.0_f ) );
							auto blur3 = writer.declLocale( "blur3"
								, textureLod( c3d_mapBlur3, vtx_texture, 0.0_f ) );
							auto data4 = writer.declLocale( "data4"
								, textureLod( c3d_mapData4, vtx_texture, 0.0_f ) );
							auto translucency = writer.declLocale( "translucency"
								, data4.w() );
							pxl_fragColor = original * originalWeight
								+ blur1 * blurWeights[0]
								+ blur2 * blurWeights[1]
								+ blur3 * blurWeights[2];
						}
						FI;
					}
					ELSE
					{
						pxl_fragColor = original;
					}
					FI;
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		ashes::PipelineShaderStageCreateInfoArray doCreateBlurProgram( Engine & engine
			, bool isVertic
			, ShaderModule & vertexShader
			, ShaderModule & pixelShader )
		{
			auto & device = getCurrentRenderDevice( engine );
			vertexShader.shader = doGetVertexProgram( engine );
			pixelShader.shader = doGetBlurProgram( engine, isVertic );

			return ashes::PipelineShaderStageCreateInfoArray
			{
				makeShaderState( device, vertexShader ),
				makeShaderState( device, pixelShader ),
			};
		}

		ashes::PipelineShaderStageCreateInfoArray doCreateCombineProgram( Engine & engine
			, ShaderModule & vertexShader
			, ShaderModule & pixelShader )
		{
			auto & device = getCurrentRenderDevice( engine );
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
			String const name{ cuT( "SubsurfaceScatteringPass" ) };

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
			, Size const & size
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
			texture->getDefaultImage().initialiseSource();
			TextureUnit unit{ engine };
			unit.setTexture( texture );
			unit.setSampler( sampler );
			unit.initialise();
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

		ashes::FrameBufferPtr doCreateFrameBuffer( RenderDevice const & device
			, ashes::RenderPass const & renderPass
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
	}

	//*********************************************************************************************

	SubsurfaceScatteringPass::Blur::Blur( RenderSystem & renderSystem
		, castor::Size const & size
		, GpInfoUbo & gpInfoUbo
		, SceneUbo & sceneUbo
		, UniformBuffer< BlurConfiguration > const & blurUbo
		, GeometryPassResult const & gp
		, TextureUnit const & source
		, TextureUnit const & destination
		, bool isVertic
		, ashes::PipelineShaderStageCreateInfoArray const & shaderStages )
		: RenderQuad{ renderSystem, VK_FILTER_LINEAR, TexcoordConfig{} }
		, m_renderSystem{ renderSystem }
		, m_geometryBufferResult{ gp }
		, m_gpInfoUbo{ gpInfoUbo }
		, m_sceneUbo{ sceneUbo }
		, m_blurUbo{ blurUbo }
		, m_renderPass{ doCreateRenderPass( getCurrentRenderDevice( renderSystem ), destination.getTexture()->getPixelFormat(), "SubscatteringBlur" ) }
		, m_frameBuffer{ doCreateFrameBuffer( getCurrentRenderDevice( renderSystem ), *m_renderPass, size, destination.getTexture()->getDefaultView(), "SubscatteringBlur" ) }
	{
		ashes::VkDescriptorSetLayoutBindingArray bindings
		{
			renderSystem.getEngine()->getMaterialCache().getPassBuffer().createLayoutBinding(),
			makeDescriptorSetLayoutBinding( BlurSceneUboId
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_FRAGMENT_BIT ), // Scene UBO
			makeDescriptorSetLayoutBinding( BlurGpInfoUboId
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_FRAGMENT_BIT ), // GpInfo UBO
			makeDescriptorSetLayoutBinding( BlurSssUboId
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_FRAGMENT_BIT ), // Blur UBO
			makeDescriptorSetLayoutBinding( BlurDepthImgId
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ), // Depth map
			makeDescriptorSetLayoutBinding( BlurData4ImgId
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ), // Translucency map
			makeDescriptorSetLayoutBinding( BlurData5ImgId
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ), // MaterialIndex map
		};

		VkExtent2D extent{ size.getWidth(), size.getHeight() };
		createPipeline( extent
			, Position{}
			, shaderStages
			, source.getTexture()->getDefaultView()
			, *m_renderPass
			, std::move( bindings )
			, {} );
	}

	SubsurfaceScatteringPass::Blur::Blur( Blur && rhs )noexcept
		: RenderQuad{ std::forward< RenderQuad >( rhs ) }
		, m_renderSystem{ rhs.m_renderSystem }
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
		registerFrame( commandBuffer );
		commandBuffer.endRenderPass();
		commandBuffer.endDebugBlock();
	}

	void SubsurfaceScatteringPass::Blur::doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
		, ashes::DescriptorSet & descriptorSet )
	{
		m_renderSystem.getEngine()->getMaterialCache().getPassBuffer().createBinding( descriptorSet
			, descriptorSetLayout.getBinding( getPassBufferIndex() ) );
		descriptorSet.createSizedBinding( descriptorSetLayout.getBinding( BlurSceneUboId )
			, m_sceneUbo.getUbo().getBuffer()
			, 0u
			, 1u );
		descriptorSet.createSizedBinding( descriptorSetLayout.getBinding( BlurGpInfoUboId )
			, m_gpInfoUbo.getUbo().getBuffer()
			, 0u
			, 1u );
		descriptorSet.createSizedBinding( descriptorSetLayout.getBinding( BlurSssUboId )
			, m_blurUbo.getBuffer()
			, 0u
			, 1u );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( BlurDepthImgId )
			, m_geometryBufferResult.getViews()[size_t( DsTexture::eDepth )]
			, m_sampler->getSampler() );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( BlurData4ImgId )
			, m_geometryBufferResult.getViews()[size_t( DsTexture::eData4 )]
			, m_sampler->getSampler() );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( BlurData5ImgId )
			, m_geometryBufferResult.getViews()[size_t( DsTexture::eData5 )]
			, m_sampler->getSampler() );
	}

	//*********************************************************************************************

	SubsurfaceScatteringPass::Combine::Combine( RenderSystem & renderSystem
		, Size const & size
		, UniformBuffer< BlurWeights > const & blurUbo
		, DebugUbo const & debugUbo
		, GeometryPassResult const & gp
		, TextureUnit const & source
		, std::array< TextureUnit, 3u > const & blurResults
		, TextureUnit const & destination
		, ashes::PipelineShaderStageCreateInfoArray const & shaderStages )
		: RenderQuad{ renderSystem, VK_FILTER_LINEAR, TexcoordConfig{} }
		, m_renderSystem{ renderSystem }
		, m_blurUbo{ blurUbo }
		, m_debugUbo{ debugUbo }
		, m_geometryBufferResult{ gp }
		, m_source{ source }
		, m_blurResults{ blurResults }
		, m_renderPass{ doCreateRenderPass( getCurrentRenderDevice( renderSystem ), destination.getTexture()->getPixelFormat(), "SubscatteringCombine" ) }
		, m_frameBuffer{ doCreateFrameBuffer( getCurrentRenderDevice( renderSystem ), *m_renderPass, size, destination.getTexture()->getDefaultView(), "SubscatteringCombine" ) }
	{
		ashes::VkDescriptorSetLayoutBindingArray bindings
		{
			renderSystem.getEngine()->getMaterialCache().getPassBuffer().createLayoutBinding(),
			makeDescriptorSetLayoutBinding( 1u
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),	// Debug
			makeDescriptorSetLayoutBinding( CombData4ImgId
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),	// Translucency map
			makeDescriptorSetLayoutBinding( CombData5ImgId
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),	// MaterialIndex map
			makeDescriptorSetLayoutBinding( CombBlur1ImgId
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),	// Blur result 0
			makeDescriptorSetLayoutBinding( CombBlur2ImgId
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),	// Blur result 1
			makeDescriptorSetLayoutBinding( CombBlur3ImgId
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),	// Blur result 2
		};
		VkExtent2D extent{ size.getWidth(), size.getHeight() };
		createPipeline( extent
			, Position{}
			, shaderStages
			, source.getTexture()->getDefaultView()
			, *m_renderPass
			, std::move( bindings )
			, {} );
	}

	SubsurfaceScatteringPass::Combine::Combine( Combine && rhs )noexcept
		: RenderQuad{ std::forward< RenderQuad >( rhs ) }
		, m_renderSystem{ rhs.m_renderSystem }
		, m_blurUbo{ rhs.m_blurUbo }
		, m_debugUbo{ rhs.m_debugUbo }
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
		registerFrame( commandBuffer );
		commandBuffer.endRenderPass();
	}

	void SubsurfaceScatteringPass::Combine::doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
		, ashes::DescriptorSet & descriptorSet )
	{
		m_renderSystem.getEngine()->getMaterialCache().getPassBuffer().createBinding( descriptorSet
			, descriptorSetLayout.getBinding( getPassBufferIndex() ) );
		descriptorSet.createSizedBinding( descriptorSetLayout.getBinding( 1u )
			, m_debugUbo.getUbo() );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( CombData4ImgId )
			, m_geometryBufferResult.getViews()[size_t( DsTexture::eData4 )]
			, m_sampler->getSampler() );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( CombData5ImgId )
			, m_geometryBufferResult.getViews()[size_t( DsTexture::eData5 )]
			, m_sampler->getSampler() );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( CombBlur1ImgId )
			, m_blurResults[0].getTexture()->getDefaultView()
			, m_blurResults[0].getSampler()->getSampler() );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( CombBlur2ImgId )
			, m_blurResults[1].getTexture()->getDefaultView()
			, m_blurResults[1].getSampler()->getSampler() );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( CombBlur3ImgId )
			, m_blurResults[2].getTexture()->getDefaultView()
			, m_blurResults[2].getSampler()->getSampler() );
	}

	//*********************************************************************************************

	String const SubsurfaceScatteringPass::Config = "Config";
	String const SubsurfaceScatteringPass::Step = "c3d_step";
	String const SubsurfaceScatteringPass::Correction = "c3d_correction";
	String const SubsurfaceScatteringPass::PixelSize = "c3d_pixelSize";
	String const SubsurfaceScatteringPass::Weights = "c3d_weights";
	String const SubsurfaceScatteringPass::Offsets = "c3d_offsets";

	SubsurfaceScatteringPass::SubsurfaceScatteringPass( Engine & engine
		, GpInfoUbo & gpInfoUbo
		, SceneUbo & sceneUbo
		, DebugUbo const & debugUbo
		, Size const & textureSize
		, GeometryPassResult const & gp
		, TextureUnit const & lightDiffuse )
		: OwnedBy< Engine >{ engine }
		, m_size{ textureSize }
		, m_blurConfigUbo{ makeUniformBuffer< BlurConfiguration >( *engine.getRenderSystem()
			, 1u
			, VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, "SubsurfaceScatteringBlurConfig" ) }
		, m_blurWeightsUbo{ makeUniformBuffer< BlurWeights >( *engine.getRenderSystem()
			, 1u
			, VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, "SubsurfaceScatteringBlurWeights" ) }
		, m_intermediate{ doCreateTexture( engine, textureSize, "SubsurfaceScattering intermediate" ) }
		, m_blurHorizVertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "SssBlurX" }
		, m_blurHorizPixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "SssBlurX" }
		, m_blurHorizProgram{ doCreateBlurProgram( engine, false, m_blurHorizVertexShader, m_blurHorizPixelShader ) }
		, m_blurVerticVertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "SssBlurY" }
		, m_blurVerticPixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "SssBlurY" }
		, m_blurVerticProgram{ doCreateBlurProgram( engine, true, m_blurVerticVertexShader, m_blurVerticPixelShader ) }
		, m_combineVertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "SssCombine" }
		, m_combinePixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "SssCombine" }
		, m_combineProgram{ doCreateCombineProgram( engine, m_combineVertexShader, m_combinePixelShader ) }
		, m_blurResults
		{
			{
				doCreateTexture( engine, textureSize, "SubsurfaceScattering Blur 0" ),
				doCreateTexture( engine, textureSize, "SubsurfaceScattering Blur 1" ),
				doCreateTexture( engine, textureSize, "SubsurfaceScattering Blur 2" ),
			}
		}
		, m_result{ doCreateTexture( engine, textureSize, "SubsurfaceScattering Result" ) }
		, m_blurX
		{
			{ *engine.getRenderSystem(), m_size, gpInfoUbo, sceneUbo, *m_blurConfigUbo, gp, lightDiffuse, m_intermediate, false, m_blurHorizProgram },
			{ *engine.getRenderSystem(), m_size, gpInfoUbo, sceneUbo, *m_blurConfigUbo, gp, m_blurResults[0], m_intermediate, false, m_blurHorizProgram },
			{ *engine.getRenderSystem(), m_size, gpInfoUbo, sceneUbo, *m_blurConfigUbo, gp, m_blurResults[1], m_intermediate, false, m_blurHorizProgram },
		}
		, m_blurY
		{
			{ *engine.getRenderSystem(), m_size, gpInfoUbo, sceneUbo, *m_blurConfigUbo, gp, m_intermediate, m_blurResults[0], true, m_blurVerticProgram },
			{ *engine.getRenderSystem(), m_size, gpInfoUbo, sceneUbo, *m_blurConfigUbo, gp, m_intermediate, m_blurResults[1], true, m_blurVerticProgram },
			{ *engine.getRenderSystem(), m_size, gpInfoUbo, sceneUbo, *m_blurConfigUbo, gp, m_intermediate, m_blurResults[2], true, m_blurVerticProgram },
		}
		, m_combine{ *engine.getRenderSystem(), textureSize, *m_blurWeightsUbo, debugUbo, gp, lightDiffuse, m_blurResults, m_result, m_combineProgram }
		, m_timer{ std::make_shared< RenderPassTimer >( engine, cuT( "Opaque" ), cuT( "SSSSS pass" ) ) }
	{
		auto & configuration = m_blurConfigUbo->getData( 0u );
		configuration.blurCorrection = 1.0f;
		configuration.blurPixelSize = Point2f{ 1.0f / m_size.getWidth(), 1.0f / m_size.getHeight() };
		m_blurConfigUbo->upload();

		auto & weights = m_blurWeightsUbo->getData();
		weights.originalWeight = Point4f{ 0.2406f, 0.4475f, 0.6159f, 0.25f };
		weights.blurWeights[0] = Point4f{ 0.1158, 0.3661, 0.3439, 0.25 };
		weights.blurWeights[1] = Point4f{ 0.1836, 0.1864, 0.0, 0.25 };
		weights.blurWeights[2] = Point4f{ 0.46, 0.0, 0.0402, 0.25 };
		weights.blurVariance = Point4f{ 0.0516, 0.2719, 2.0062 };
		m_blurWeightsUbo->upload();

		auto & device = getCurrentRenderDevice( engine );
		m_finished = device->createSemaphore( "SSSSS pass" );
		m_commandBuffer = device.graphicsCommandPool->createCommandBuffer( "SSSSS pass" );
		prepare();
	}

	SubsurfaceScatteringPass::~SubsurfaceScatteringPass()
	{
		m_result.cleanup();
		m_intermediate.cleanup();

		for ( auto & result : m_blurResults )
		{
			result.cleanup();
		}
	}

	void SubsurfaceScatteringPass::prepare()
	{
		m_commandBuffer->begin();
		m_commandBuffer->beginDebugBlock(
			{
				"Deferred - Subsurface Subscattering",
				makeFloatArray( getEngine()->getNextRainbowColour() ),
			} );
		m_timer->beginPass( *m_commandBuffer );

		for ( size_t i{ 0u }; i < m_blurResults.size(); ++i )
		{
			m_blurX[i].prepareFrame( *m_commandBuffer );
			m_blurY[i].prepareFrame( *m_commandBuffer );
		}

		m_combine.prepareFrame( *m_commandBuffer );
		m_timer->endPass( *m_commandBuffer );
		m_commandBuffer->endDebugBlock();
		m_commandBuffer->end();
	}

	ashes::Semaphore const & SubsurfaceScatteringPass::render( ashes::Semaphore const & toWait )const
	{
		auto & device = getCurrentRenderDevice( *this );
		auto timerBlock = m_timer->start();
		m_timer->notifyPassRender();
		auto * result = &toWait;

		device.graphicsQueue->submit( *m_commandBuffer
			, *result
			, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, *m_finished
			, nullptr );
		result = m_finished.get();
		return *result;
	}

	void SubsurfaceScatteringPass::accept( RenderTechniqueVisitor & visitor )
	{
		for ( size_t i{ 0u }; i < m_blurResults.size(); ++i )
		{
			visitor.visit( "SSSSS Blur " + string::toString( i )
				, m_blurResults[i].getTexture()->getDefaultView() );
		}

		visitor.visit( "SSSSS Result"
			, m_result.getTexture()->getDefaultView() );

		visitor.visit( m_blurHorizVertexShader );
		visitor.visit( m_blurHorizPixelShader );

		visitor.visit( m_blurVerticVertexShader );
		visitor.visit( m_blurVerticPixelShader );

		visitor.visit( m_combineVertexShader );
		visitor.visit( m_combinePixelShader );
	}
}
