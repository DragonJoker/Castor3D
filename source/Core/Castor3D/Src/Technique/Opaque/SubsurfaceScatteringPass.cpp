#include "SubsurfaceScatteringPass.hpp"

#include "Engine.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Camera.hpp"
#include "Shader/PassBuffer/PassBuffer.hpp"
#include "Shader/Shaders/GlslMaterial.hpp"
#include "Shader/Program.hpp"
#include "Castor3DPrerequisites.hpp"
#include "Shader/Shaders/GlslLight.hpp"
#include "Shader/Shaders/GlslShadow.hpp"
#include "Shader/Shaders/GlslSssTransmittance.hpp"
#include "Shader/Ubos/GpInfoUbo.hpp"
#include "Shader/Ubos/SceneUbo.hpp"
#include "Technique/Opaque/GeometryPassResult.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"

#include <Buffer/VertexBuffer.hpp>
#include <Pipeline/VertexLayout.hpp>
#include <RenderPass/FrameBuffer.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>
#include <Shader/GlslToSpv.hpp>

#include <ShaderWriter/Source.hpp>
#include "Shader/Shaders/GlslUtils.hpp"

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
			auto position = writer.declInput< Vec2 >( cuT( "position" ), 0u );
			auto uv = writer.declInput< Vec2 >( cuT( "uv" ), 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto out = writer.getOut();

			writer.implementFunction< sdw::Void >( cuT( "main" )
				, [&]()
				{
					vtx_texture = uv;
					out.gl_out.gl_Position = vec4( position, 0.0, 1.0 );
				} );
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		ShaderPtr doGetBlurProgram( Engine & engine
			, bool isVertic )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			auto materials = shader::createMaterials( writer
				, ( engine.getMaterialsType() == MaterialType::ePbrMetallicRoughness
					? PassFlag::ePbrMetallicRoughness
					: ( engine.getMaterialsType() == MaterialType::ePbrSpecularGlossiness
						? PassFlag::ePbrSpecularGlossiness
						: PassFlag( 0u ) ) ) );
			materials->declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
			UBO_SCENE( writer, SceneUbo::BindingPoint, 0u );
			UBO_GPINFO( writer, GpInfoUbo::BindingPoint, 0u );
			Ubo config{ writer, SubsurfaceScatteringPass::Config, 4u, 0u };
			auto c3d_pixelSize = config.declMember< Vec2 >( SubsurfaceScatteringPass::PixelSize );
			auto c3d_correction = config.declMember< Float >( SubsurfaceScatteringPass::Correction );
			config.end();
			auto c3d_mapDepth = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eDepth ), 6u, 0u );
			auto c3d_mapData4 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData4 ), 7u, 0u );
			auto c3d_mapData5 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData5 ), 8u, 0u );
			auto c3d_mapLightDiffuse = writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapLightDiffuse" ), 9u, 0u );

			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );

			shader::Utils utils{ writer, renderSystem.isTopDown(), renderSystem.isZeroToOneDepth() };
			utils.declareCalcVSPosition();

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( cuT( "pxl_fragColor" ), 0 );

			writer.implementFunction< sdw::Void >( cuT( "main" ), [&]()
			{
				auto data4 = writer.declLocale( cuT( "data4" )
					, textureLod( c3d_mapData4, vtx_texture, 0.0_f ) );
				auto data5 = writer.declLocale( cuT( "data5" )
					, textureLod( c3d_mapData5, vtx_texture, 0.0_f ) );
				auto materialId = writer.declLocale( cuT( "materialId" )
					, writer.cast< Int >( data5.z() ) );
				auto translucency = writer.declLocale( cuT( "translucency" )
					, data4.w() );
				auto material = materials->getBaseMaterial( materialId );

				IF( writer, material->m_subsurfaceScatteringEnabled == 0_i )
				{
					writer.discard();
				}
				FI;

				// Fetch color and linear depth for current pixel:
				auto colorM = writer.declLocale( cuT( "colorM" )
					, textureLod( c3d_mapLightDiffuse, vtx_texture, 0.0_f ) );
				auto depthM = writer.declLocale( cuT( "depthM" )
					, textureLod( c3d_mapDepth, vtx_texture, 0.0_f ).r() );
				depthM = utils.calcVSPosition( vtx_texture
					, depthM
					, c3d_mtxInvProj ).z();

				// Accumulate center sample, multiplying it with its gaussian weight:
				pxl_fragColor = colorM;
				pxl_fragColor.rgb() *= 0.382_f;

				if ( isVertic )
				{
					auto step = writer.declLocale( cuT( "step" )
						, c3d_pixelSize * vec2( 0.0_f, 1.0_f ) );
				}
				else
				{
					auto step = writer.declLocale( cuT( "step" )
						, c3d_pixelSize * vec2( 1.0_f, 0.0_f ) );
				}

				auto step = writer.getVariable< Vec2 >( cuT( "step" ) );

				// Calculate the step that we will use to fetch the surrounding pixels,
				// where "step" is:
				//     step = sssStrength * gaussianWidth * pixelSize * dir
				// The closer the pixel, the stronger the effect needs to be, hence
				// the factor 1.0 / depthM.
				auto finalStep = writer.declLocale( cuT( "finalStep" )
					, translucency * step * material->m_subsurfaceScatteringStrength * material->m_gaussianWidth / depthM );

				auto offset = writer.declLocale< Vec2 >( cuT( "offset" ) );
				auto color = writer.declLocale< Vec3 >( cuT( "color" ) );
				auto depth = writer.declLocale< Float >( cuT( "depth" ) );
				auto s = writer.declLocale< Float >( cuT( "s" ) );

				// Gaussian weights for the six samples around the current pixel:
				//   -3 -2 -1 +1 +2 +3
				auto w = writer.declLocaleArray( cuT( "w" )
					, 6u
					, std::vector< Float >{ { 0.006_f, 0.061_f, 0.242_f, 0.242_f, 0.061_f, 0.006_f } } );
				auto o = writer.declLocaleArray( cuT( "o" )
					, 6u
					, std::vector< Float >{ { -1.0_f, -0.6667_f, -0.3333_f, 0.3333_f, 0.6667_f, 1.0_f } } );

				// Accumulate the other samples:
				for ( int i = 0; i < 6; i++ )
				{
					// Fetch color and depth for current sample:
					offset = sdw::fma( vec2( o[i] ), finalStep, vtx_texture );
					color = textureLod( c3d_mapLightDiffuse, offset, 0.0_f ).rgb();
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
			} );
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		ShaderPtr doGetCombineProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			auto materials = shader::createMaterials( writer
				, ( engine.getMaterialsType() == MaterialType::ePbrMetallicRoughness
					? PassFlag::ePbrMetallicRoughness
					: ( engine.getMaterialsType() == MaterialType::ePbrSpecularGlossiness
						? PassFlag::ePbrSpecularGlossiness
						: PassFlag( 0u ) ) ) );
			materials->declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );

			auto c3d_mapData4 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData4 ), 2u, 0u );
			auto c3d_mapData5 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData5 ), 3u, 0u );
			auto c3d_mapBlur1 = writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapBlur1" ), 4u, 0u );
			auto c3d_mapBlur2 = writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapBlur2" ), 5u, 0u );
			auto c3d_mapBlur3 = writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapBlur3" ), 6u, 0u );
			auto c3d_mapLightDiffuse = writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapLightDiffuse" ), 7u, 0u );

			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( cuT( "pxl_fragColor" ), 0 );

			writer.implementFunction< sdw::Void >( cuT( "main" ), [&]()
			{
				auto originalWeight = writer.declLocale< Vec4 >( cuT( "originalWeight" )
					, vec4( 0.2406_f, 0.4475_f, 0.6159_f, 0.25_f ) );
				auto blurWeights = writer.declLocaleArray< Vec4 >( cuT( "blurWeights" )
					, 3u
					, {
						vec4( 0.1158_f, 0.3661_f, 0.3439_f, 0.25_f ),
						vec4( 0.1836_f, 0.1864_f, 0.0_f, 0.25_f ),
						vec4( 0.46_f, 0.0_f, 0.0402_f, 0.25_f )
					} );
				auto blurVariances = writer.declLocaleArray< Float >( cuT( "blurVariances" )
					, 3u
					, {
						0.0516_f,
						0.2719_f,
						2.0062_f
					} );
				auto data4 = writer.declLocale( cuT( "data4" )
					, textureLod( c3d_mapData4, vtx_texture, 0.0_f ) );
				auto data5 = writer.declLocale( cuT( "data5" )
					, textureLod( c3d_mapData5, vtx_texture, 0.0_f ) );
				auto original = writer.declLocale( cuT( "original" )
					, textureLod( c3d_mapLightDiffuse, vtx_texture, 0.0_f ) );
				auto blur1 = writer.declLocale( cuT( "blur1" )
					, textureLod( c3d_mapBlur1, vtx_texture, 0.0_f ) );
				auto blur2 = writer.declLocale( cuT( "blur2" )
					, textureLod( c3d_mapBlur2, vtx_texture, 0.0_f ) );
				auto blur3 = writer.declLocale( cuT( "blur3" )
					, textureLod( c3d_mapBlur3, vtx_texture, 0.0_f ) );
				auto materialId = writer.declLocale( cuT( "materialId" )
					, writer.cast< Int >( data5.z() ) );
				auto translucency = writer.declLocale( cuT( "translucency" )
					, data4.w() );
				auto material = materials->getBaseMaterial( materialId );

#if !C3D_DebugSSSTransmittance
				IF( writer, material->m_subsurfaceScatteringEnabled == 0_i )
				{
					pxl_fragColor = original;
				}
				ELSE
				{
					pxl_fragColor = original * originalWeight
						+ blur1 * blurWeights[0]
						+ blur2 * blurWeights[1]
						+ blur3 * blurWeights[2];
				}
				FI;
#else
				pxl_fragColor = original;
#endif
			} );
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		ashes::ShaderStageStateArray doCreateBlurProgram( Engine & engine
			, bool isVertic
			, ShaderModule & vertexShader
			, ShaderModule & pixelShader )
		{
			auto & device = getCurrentDevice( engine );
			vertexShader.shader = doGetVertexProgram( engine );
			pixelShader.shader = doGetBlurProgram( engine, isVertic );

			ashes::ShaderStageStateArray program
			{
				{ device.createShaderModule( ashes::ShaderStageFlag::eVertex ) },
				{ device.createShaderModule( ashes::ShaderStageFlag::eFragment ) },
			};
			program[0].module->loadShader( engine.getRenderSystem()->compileShader( vertexShader ) );
			program[1].module->loadShader( engine.getRenderSystem()->compileShader( pixelShader ) );
			return program;
		}

		ashes::ShaderStageStateArray doCreateCombineProgram( Engine & engine
			, ShaderModule & vertexShader
			, ShaderModule & pixelShader )
		{
			auto & device = getCurrentDevice( engine );
			vertexShader.shader = doGetVertexProgram( engine );
			pixelShader.shader = doGetCombineProgram( engine );

			ashes::ShaderStageStateArray program
			{
				{ device.createShaderModule( ashes::ShaderStageFlag::eVertex ) },
				{ device.createShaderModule( ashes::ShaderStageFlag::eFragment ) },
			};
			program[0].module->loadShader( engine.getRenderSystem()->compileShader( vertexShader ) );
			program[1].module->loadShader( engine.getRenderSystem()->compileShader( pixelShader ) );
			return program;
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
				sampler->setMinFilter( ashes::Filter::eNearest );
				sampler->setMagFilter( ashes::Filter::eNearest );
				sampler->setWrapS( ashes::WrapMode::eClampToEdge );
				sampler->setWrapT( ashes::WrapMode::eClampToEdge );
			}

			return sampler;
		}

		TextureUnit doCreateTexture( Engine & engine
			, Size const & size )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto sampler = doCreateSampler( engine );

			ashes::ImageCreateInfo image{};
			image.arrayLayers = 1u;
			image.extent.width = size.getWidth();
			image.extent.height = size.getHeight();
			image.extent.depth = 1u;
			image.format = ashes::Format::eR32G32B32A32_SFLOAT;
			image.imageType = ashes::TextureType::e2D;
			image.mipLevels = 1u;
			image.samples = ashes::SampleCountFlag::e1;
			image.usage = ashes::ImageUsageFlag::eColourAttachment | ashes::ImageUsageFlag::eSampled;

			auto texture = std::make_shared< TextureLayout >( renderSystem
				, image
				, ashes::MemoryPropertyFlag::eDeviceLocal );
			texture->getDefaultImage().initialiseSource();
			TextureUnit unit{ engine };
			unit.setTexture( texture );
			unit.setSampler( sampler );
			unit.initialise();
			return unit;
		}
	}

	//*********************************************************************************************

	SubsurfaceScatteringPass::Blur::Blur( RenderSystem & renderSystem
		, castor::Size const & size
		, GpInfoUbo & gpInfoUbo
		, SceneUbo & sceneUbo
		, ashes::UniformBuffer< BlurConfiguration > const & blurUbo
		, GeometryPassResult const & gp
		, TextureUnit const & source
		, TextureUnit const & destination
		, bool isVertic
		, ashes::ShaderStageStateArray const & shaderStages )
		: RenderQuad{ renderSystem, false, false }
		, m_renderSystem{ renderSystem }
		, m_geometryBufferResult{ gp }
		, m_gpInfoUbo{ gpInfoUbo }
		, m_sceneUbo{ sceneUbo }
		, m_blurUbo{ blurUbo }
	{
		auto & device = getCurrentDevice( renderSystem );

		// Create the render pass.
		ashes::RenderPassCreateInfo renderPass;
		renderPass.flags = 0u;

		renderPass.attachments.resize( 1u );
		renderPass.attachments[0].format = destination.getTexture()->getPixelFormat();
		renderPass.attachments[0].loadOp = ashes::AttachmentLoadOp::eClear;
		renderPass.attachments[0].storeOp = ashes::AttachmentStoreOp::eStore;
		renderPass.attachments[0].stencilLoadOp = ashes::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].stencilStoreOp = ashes::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].samples = ashes::SampleCountFlag::e1;
		renderPass.attachments[0].initialLayout = ashes::ImageLayout::eUndefined;
		renderPass.attachments[0].finalLayout = ashes::ImageLayout::eShaderReadOnlyOptimal;

		renderPass.subpasses.resize( 1u );
		renderPass.subpasses[0].flags = 0u;
		renderPass.subpasses[0].pipelineBindPoint = ashes::PipelineBindPoint::eGraphics;
		renderPass.subpasses[0].colorAttachments.push_back( { 0u, ashes::ImageLayout::eColourAttachmentOptimal } );
		
		renderPass.dependencies.resize( 2u );
		renderPass.dependencies[0].srcSubpass = ashes::ExternalSubpass;
		renderPass.dependencies[0].dstSubpass = 0u;
		renderPass.dependencies[0].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[0].dstAccessMask = ashes::AccessFlag::eShaderRead;
		renderPass.dependencies[0].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].dstStageMask = ashes::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[0].dependencyFlags = ashes::DependencyFlag::eByRegion;

		renderPass.dependencies[1].srcSubpass = 0u;
		renderPass.dependencies[1].dstSubpass = ashes::ExternalSubpass;
		renderPass.dependencies[1].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[1].dstAccessMask = ashes::AccessFlag::eShaderRead;
		renderPass.dependencies[1].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[1].dstStageMask = ashes::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[1].dependencyFlags = ashes::DependencyFlag::eByRegion;

		m_renderPass = device.createRenderPass( renderPass );

		// Initialise the frame buffer.
		ashes::Extent2D extent{ size.getWidth(), size.getHeight() };
		ashes::FrameBufferAttachmentArray attaches;
		attaches.emplace_back( *m_renderPass->getAttachments().begin(), destination.getTexture()->getDefaultView() );
		m_frameBuffer = m_renderPass->createFrameBuffer( extent
			, std::move( attaches ) );

		ashes::DescriptorSetLayoutBindingArray bindings
		{
			renderSystem.getEngine()->getMaterialCache().getPassBuffer().createLayoutBinding(),
			{ SceneUbo::BindingPoint, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eFragment }, // Scene UBO
			{ GpInfoUbo::BindingPoint, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eFragment }, // GpInfo UBO
			{ 4u, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eFragment }, // Blur UBO
			{ 6u, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment }, // Depth map
			{ 7u, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment }, // Translucency map
			{ 8u, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment }, // MaterialIndex map
		};

		createPipeline( extent
			, Position{}
			, shaderStages
			, source.getTexture()->getDefaultView()
			, *m_renderPass
			, bindings
			, {} );
	}

	SubsurfaceScatteringPass::Blur::Blur( Blur && rhs )
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
		commandBuffer.beginRenderPass( *m_renderPass
			, *m_frameBuffer
			, { ashes::ClearColorValue{ 0, 0, 0, 1 } }
			, ashes::SubpassContents::eInline );
		registerFrame( commandBuffer );
		commandBuffer.endRenderPass();
	}

	void SubsurfaceScatteringPass::Blur::doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
		, ashes::DescriptorSet & descriptorSet )
	{
		m_renderSystem.getEngine()->getMaterialCache().getPassBuffer().createBinding( descriptorSet
			, descriptorSetLayout.getBinding( PassBufferIndex ) );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( SceneUbo::BindingPoint )
			, m_sceneUbo.getUbo()
			, 0u
			, 1u );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( GpInfoUbo::BindingPoint )
			, m_gpInfoUbo.getUbo()
			, 0u
			, 1u );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 4u )
			, m_blurUbo
			, 0u
			, 1u );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 6u )
			, *m_geometryBufferResult.getViews()[size_t( DsTexture::eDepth )]
			, m_sampler->getSampler() );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 7u )
			, *m_geometryBufferResult.getViews()[size_t( DsTexture::eData4 )]
			, m_sampler->getSampler() );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 8u )
			, *m_geometryBufferResult.getViews()[size_t( DsTexture::eData5 )]
			, m_sampler->getSampler() );
	}

	//*********************************************************************************************

	SubsurfaceScatteringPass::Combine::Combine( RenderSystem & renderSystem
		, Size const & size
		, ashes::UniformBuffer< BlurWeights > const & blurUbo
		, GeometryPassResult const & gp
		, TextureUnit const & source
		, std::array< TextureUnit, 3u > const & blurResults
		, TextureUnit const & destination
		, ashes::ShaderStageStateArray const & shaderStages )
		: RenderQuad{ renderSystem, false, false }
		, m_renderSystem{ renderSystem }
		, m_blurUbo{ blurUbo }
		, m_geometryBufferResult{ gp }
		, m_source{ source }
		, m_blurResults{ blurResults }
	{
		auto & device = getCurrentDevice( renderSystem );

		// Create the render pass.
		ashes::RenderPassCreateInfo renderPass;
		renderPass.flags = 0u;

		renderPass.attachments.resize( 1u );
		renderPass.attachments[0].format = destination.getTexture()->getPixelFormat();
		renderPass.attachments[0].loadOp = ashes::AttachmentLoadOp::eClear;
		renderPass.attachments[0].storeOp = ashes::AttachmentStoreOp::eStore;
		renderPass.attachments[0].stencilLoadOp = ashes::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].stencilStoreOp = ashes::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].samples = ashes::SampleCountFlag::e1;
		renderPass.attachments[0].initialLayout = ashes::ImageLayout::eUndefined;
		renderPass.attachments[0].finalLayout = ashes::ImageLayout::eShaderReadOnlyOptimal;

		renderPass.subpasses.resize( 1u );
		renderPass.subpasses[0].flags = 0u;
		renderPass.subpasses[0].pipelineBindPoint = ashes::PipelineBindPoint::eGraphics;
		renderPass.subpasses[0].colorAttachments.push_back( { 0u, ashes::ImageLayout::eColourAttachmentOptimal } );

		renderPass.dependencies.resize( 2u );
		renderPass.dependencies[0].srcSubpass = ashes::ExternalSubpass;
		renderPass.dependencies[0].dstSubpass = 0u;
		renderPass.dependencies[0].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[0].dstAccessMask = ashes::AccessFlag::eShaderRead;
		renderPass.dependencies[0].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].dstStageMask = ashes::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[0].dependencyFlags = ashes::DependencyFlag::eByRegion;

		renderPass.dependencies[1].srcSubpass = 0u;
		renderPass.dependencies[1].dstSubpass = ashes::ExternalSubpass;
		renderPass.dependencies[1].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[1].dstAccessMask = ashes::AccessFlag::eShaderRead;
		renderPass.dependencies[1].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[1].dstStageMask = ashes::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[1].dependencyFlags = ashes::DependencyFlag::eByRegion;

		m_renderPass = device.createRenderPass( renderPass );

		// Initialise the frame buffer.
		ashes::Extent2D extent{ size.getWidth(), size.getHeight() };
		ashes::FrameBufferAttachmentArray attaches;
		attaches.emplace_back( *m_renderPass->getAttachments().begin(), destination.getTexture()->getDefaultView() );
		m_frameBuffer = m_renderPass->createFrameBuffer( extent
			, std::move( attaches ) );

		ashes::DescriptorSetLayoutBindingArray bindings
		{
			renderSystem.getEngine()->getMaterialCache().getPassBuffer().createLayoutBinding(),
			{ 1u, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eFragment },			// Blur weights
			{ 2u, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment },	// Translucency map
			{ 3u, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment },	// MaterialIndex map
			{ 4u, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment },	// Blur result 0
			{ 5u, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment },	// Blur result 1
			{ 6u, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment },	// Blur result 2
		};

		createPipeline( extent
			, Position{}
			, shaderStages
			, source.getTexture()->getDefaultView()
			, *m_renderPass
			, bindings
			, {} );
	}

	SubsurfaceScatteringPass::Combine::Combine( Combine && rhs )
		: RenderQuad{ std::forward< RenderQuad >( rhs ) }
		, m_renderSystem{ rhs.m_renderSystem }
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
		commandBuffer.beginRenderPass( *m_renderPass
			, *m_frameBuffer
			, { ashes::ClearColorValue{ 0, 1, 0, 1 } }
		, ashes::SubpassContents::eInline );
		registerFrame( commandBuffer );
		commandBuffer.endRenderPass();
	}

	void SubsurfaceScatteringPass::Combine::doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
		, ashes::DescriptorSet & descriptorSet )
	{
		m_renderSystem.getEngine()->getMaterialCache().getPassBuffer().createBinding( descriptorSet
			, descriptorSetLayout.getBinding( PassBufferIndex ) );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 1u )
			, m_blurUbo );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 2u )
			, *m_geometryBufferResult.getViews()[size_t( DsTexture::eData4 )]
			, m_sampler->getSampler() );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 3u )
			, *m_geometryBufferResult.getViews()[size_t( DsTexture::eData5 )]
			, m_sampler->getSampler() );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 4u )
			, m_blurResults[0].getTexture()->getDefaultView()
			, m_blurResults[0].getSampler()->getSampler() );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 5u )
			, m_blurResults[1].getTexture()->getDefaultView()
			, m_blurResults[1].getSampler()->getSampler() );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 6u )
			, m_blurResults[2].getTexture()->getDefaultView()
			, m_blurResults[2].getSampler()->getSampler() );
	}

	//*********************************************************************************************

	String const SubsurfaceScatteringPass::Config = cuT( "Config" );
	String const SubsurfaceScatteringPass::Step = cuT( "c3d_step" );
	String const SubsurfaceScatteringPass::Correction = cuT( "c3d_correction" );
	String const SubsurfaceScatteringPass::PixelSize = cuT( "c3d_pixelSize" );
	String const SubsurfaceScatteringPass::Weights = cuT( "c3d_weights" );
	String const SubsurfaceScatteringPass::Offsets = cuT( "c3d_offsets" );

	SubsurfaceScatteringPass::SubsurfaceScatteringPass( Engine & engine
		, GpInfoUbo & gpInfoUbo
		, SceneUbo & sceneUbo
		, Size const & textureSize
		, GeometryPassResult const & gp
		, TextureUnit const & lightDiffuse )
		: OwnedBy< Engine >{ engine }
		, m_size{ textureSize }
		, m_blurConfigUbo{ ashes::makeUniformBuffer< BlurConfiguration >( getCurrentDevice( engine )
			, 1u
			, ashes::BufferTarget::eTransferDst
			, ashes::MemoryPropertyFlag::eHostVisible ) }
		, m_blurWeightsUbo{ ashes::makeUniformBuffer< BlurWeights >( getCurrentDevice( engine )
			, 1u
			, ashes::BufferTarget::eTransferDst
			, ashes::MemoryPropertyFlag::eHostVisible ) }
		, m_intermediate{ doCreateTexture( engine, textureSize ) }
		, m_blurHorizVertexShader{ ashes::ShaderStageFlag::eVertex, "SssBlurX" }
		, m_blurHorizPixelShader{ ashes::ShaderStageFlag::eFragment, "SssBlurX" }
		, m_blurHorizProgram{ doCreateBlurProgram( engine, false, m_blurHorizVertexShader, m_blurHorizPixelShader ) }
		, m_blurVerticVertexShader{ ashes::ShaderStageFlag::eVertex, "SssBlurY" }
		, m_blurVerticPixelShader{ ashes::ShaderStageFlag::eFragment, "SssBlurY" }
		, m_blurVerticProgram{ doCreateBlurProgram( engine, true, m_blurVerticVertexShader, m_blurVerticPixelShader ) }
		, m_combineVertexShader{ ashes::ShaderStageFlag::eVertex, "SssCombine" }
		, m_combinePixelShader{ ashes::ShaderStageFlag::eFragment, "SssCombine" }
		, m_combineProgram{ doCreateCombineProgram( engine, m_combineVertexShader, m_combinePixelShader ) }
		, m_blurResults
		{
			{
				doCreateTexture( engine, textureSize ),
				doCreateTexture( engine, textureSize ),
				doCreateTexture( engine, textureSize ),
			}
		}
		, m_result{ doCreateTexture( engine, textureSize ) }
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
		, m_combine{ *engine.getRenderSystem(), textureSize, *m_blurWeightsUbo, gp, lightDiffuse, m_blurResults, m_result, m_combineProgram }
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

		auto & device = getCurrentDevice( engine );
		m_finished = device.createSemaphore();
		m_fence = device.createFence( ashes::FenceCreateFlag::eSignaled );
		m_commandBuffer = device.getGraphicsCommandPool().createCommandBuffer();
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
		m_timer->beginPass( *m_commandBuffer );

		for ( size_t i{ 0u }; i < m_blurResults.size(); ++i )
		{
			m_blurX[i].prepareFrame( *m_commandBuffer );
			m_blurY[i].prepareFrame( *m_commandBuffer );
		}

		m_combine.prepareFrame( *m_commandBuffer );
		m_timer->endPass( *m_commandBuffer );
		m_commandBuffer->end();
	}

	ashes::Semaphore const & SubsurfaceScatteringPass::render( ashes::Semaphore const & toWait )const
	{
		auto & device = getCurrentDevice( *this );
		auto timerBlock = m_timer->start();
		m_timer->notifyPassRender();
		auto * result = &toWait;

		device.getGraphicsQueue().submit( *m_commandBuffer
			, *result
			, ashes::PipelineStageFlag::eColourAttachmentOutput
			, *m_finished
			, nullptr );
		result = m_finished.get();
		return *result;
	}

	void SubsurfaceScatteringPass::debugDisplay( castor::Size const & size )const
	{
		//auto count = 9u;
		//int width = int( m_size.getWidth() ) / count;
		//int height = int( m_size.getHeight() ) / count;
		//int top = int( m_size.getHeight() ) - height;
		//auto renderSize = Size( width, height );
		//auto & context = *getEngine()->getRenderSystem()->getCurrentContext();
		//auto index = 0;
		//context.renderTexture( Position{ width * index++, top }
		//	, renderSize
		//	, *m_blurResults[0].getTexture() );
		//context.renderTexture( Position{ width * index++, top }
		//	, renderSize
		//	, *m_blurResults[1].getTexture() );
		//context.renderTexture( Position{ width * index++, top }
		//	, renderSize
		//	, *m_blurResults[2].getTexture() );
		//context.renderTexture( Position{ width * index++, top }
		//	, renderSize
		//	, *m_result.getTexture() );
	}

	void SubsurfaceScatteringPass::accept( RenderTechniqueVisitor & visitor )
	{
		visitor.visit( cuT( "SSSSS - Blur X" )
			, ashes::ShaderStageFlag::eVertex
			, *m_blurHorizVertexShader.shader );
		visitor.visit( cuT( "SSSSS - Blur X" )
			, ashes::ShaderStageFlag::eFragment
			, *m_blurHorizPixelShader.shader );

		visitor.visit( cuT( "SSSSS - Blur Y" )
			, ashes::ShaderStageFlag::eVertex
			, *m_blurVerticVertexShader.shader );
		visitor.visit( cuT( "SSSSS - Blur Y" )
			, ashes::ShaderStageFlag::eFragment
			, *m_blurVerticPixelShader.shader );

		visitor.visit( cuT( "SSSSS - Combine" )
			, ashes::ShaderStageFlag::eVertex
			, *m_combineVertexShader.shader );
		visitor.visit( cuT( "SSSSS - Combine" )
			, ashes::ShaderStageFlag::eFragment
			, *m_combinePixelShader.shader );
	}
}
