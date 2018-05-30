#include "SubsurfaceScatteringPass.hpp"

#include "Engine.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Camera.hpp"
#include "Shader/PassBuffer/PassBuffer.hpp"
#include "Shader/Shaders/GlslMaterial.hpp"
#include "Shader/ShaderProgram.hpp"
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

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

#include <random>

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	namespace
	{
		glsl::Shader doGetVertexProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			auto position = writer.declAttribute< Vec2 >( cuT( "position" ), 0u );
			auto texcoord = writer.declAttribute< Vec2 >( cuT( "texcoord" ), 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto out = gl_PerVertex{ writer };

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					vtx_texture = texcoord;
					out.gl_Position() = vec4( position, 0.0, 1.0 );
				} );
			return writer.finalise();
		}

		glsl::Shader doGetBlurProgram( Engine & engine
			, bool isVertic )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			auto materials = shader::createMaterials( writer
				, ( engine.getMaterialsType() == MaterialType::ePbrMetallicRoughness
					? PassFlag::ePbrMetallicRoughness
					: ( engine.getMaterialsType() == MaterialType::ePbrSpecularGlossiness
						? PassFlag::ePbrSpecularGlossiness
						: PassFlag( 0u ) ) ) );
			materials->declare();
			UBO_SCENE( writer, SceneUbo::BindingPoint, 0u );
			UBO_GPINFO( writer, GpInfoUbo::BindingPoint, 0u );
			Ubo config{ writer, SubsurfaceScatteringPass::Config, 4u, 0u };
			auto c3d_pixelSize = config.declMember< Vec2 >( SubsurfaceScatteringPass::PixelSize );
			auto c3d_correction = config.declMember< Float >( SubsurfaceScatteringPass::Correction );
			config.end();
			auto c3d_mapDepth = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eDepth ), 6u, 0u );
			auto c3d_mapData4 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData4 ), 7u, 0u );
			auto c3d_mapData5 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData5 ), 8u, 0u );
			auto c3d_mapLightDiffuse = writer.declSampler< Sampler2D >( cuT( "c3d_mapLightDiffuse" ), 9u, 0u );

			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );

			glsl::Utils utils{ writer };
			utils.declareCalcVSPosition();

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0 );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				auto data4 = writer.declLocale( cuT( "data4" )
					, texture( c3d_mapData4, vtx_texture ) );
				auto data5 = writer.declLocale( cuT( "data5" )
					, texture( c3d_mapData5, vtx_texture ) );
				auto materialId = writer.declLocale( cuT( "materialId" )
					, writer.cast< Int >( data5.z() ) );
				auto translucency = writer.declLocale( cuT( "translucency" )
					, data4.w() );
				auto material = materials->getBaseMaterial( materialId );

				IF( writer, material->m_subsurfaceScatteringEnabled() == 0_i )
				{
					writer.discard();
				}
				FI;

				// Fetch color and linear depth for current pixel:
				auto colorM = writer.declLocale( cuT( "colorM" )
					, texture( c3d_mapLightDiffuse, vtx_texture ) );
				auto depthM = writer.declLocale( cuT( "depthM" )
					, texture( c3d_mapDepth, vtx_texture ).r() );
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

				auto step = writer.declBuiltin< Vec2 >( cuT( "step" ) );

				// Calculate the step that we will use to fetch the surrounding pixels,
				// where "step" is:
				//     step = sssStrength * gaussianWidth * pixelSize * dir
				// The closer the pixel, the stronger the effect needs to be, hence
				// the factor 1.0 / depthM.
				auto finalStep = writer.declLocale( cuT( "finalStep" )
					, translucency * step * material->m_subsurfaceScatteringStrength() * material->m_gaussianWidth() / depthM );

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
					, std::vector< Float >{ { -1.0, -0.6667, -0.3333, 0.3333, 0.6667, 1.0 } } );

				// Accumulate the other samples:
				for ( int i = 0; i < 6; i++ )
				{
					// Fetch color and depth for current sample:
					offset = glsl::fma( vec2( o[i] ), finalStep, vtx_texture );
					color = texture( c3d_mapLightDiffuse, offset, 0.0_f ).rgb();
					depth = texture( c3d_mapDepth, offset, 0.0_f ).r();
					depth = utils.calcVSPosition( vtx_texture
						, depth
						, c3d_mtxInvProj ).z();

					// If the difference in depth is huge, we lerp color back to "colorM":
					s = min( 0.0125_f * c3d_correction * abs( depthM - depth ), 1.0_f );
					color = mix( color, colorM.rgb(), s );

					// Accumulate:
					pxl_fragColor.rgb() += w[i] * color;
					writer << glsl::endl;
				}
			} );
			return writer.finalise();
		}
		glsl::Shader doGetCombineProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			auto materials = shader::createMaterials( writer
				, ( engine.getMaterialsType() == MaterialType::ePbrMetallicRoughness
					? PassFlag::ePbrMetallicRoughness
					: ( engine.getMaterialsType() == MaterialType::ePbrSpecularGlossiness
						? PassFlag::ePbrSpecularGlossiness
						: PassFlag( 0u ) ) ) );
			materials->declare();

			auto c3d_mapData4 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData4 ), 2u, 0u );
			auto c3d_mapData5 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData5 ), 3u, 0u );
			auto c3d_mapBlur1 = writer.declSampler< Sampler2D >( cuT( "c3d_mapBlur1" ), 4u, 0u );
			auto c3d_mapBlur2 = writer.declSampler< Sampler2D >( cuT( "c3d_mapBlur2" ), 5u, 0u );
			auto c3d_mapBlur3 = writer.declSampler< Sampler2D >( cuT( "c3d_mapBlur3" ), 6u, 0u );
			auto c3d_mapLightDiffuse = writer.declSampler< Sampler2D >( cuT( "c3d_mapLightDiffuse" ), 7u, 0u );

			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0 );

			writer.implementFunction< void >( cuT( "main" ), [&]()
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
					, texture( c3d_mapData4, vtx_texture ) );
				auto data5 = writer.declLocale( cuT( "data5" )
					, texture( c3d_mapData5, vtx_texture ) );
				auto original = writer.declLocale( cuT( "original" )
					, texture( c3d_mapLightDiffuse, vtx_texture ) );
				auto blur1 = writer.declLocale( cuT( "blur1" )
					, texture( c3d_mapBlur1, vtx_texture ) );
				auto blur2 = writer.declLocale( cuT( "blur2" )
					, texture( c3d_mapBlur2, vtx_texture ) );
				auto blur3 = writer.declLocale( cuT( "blur3" )
					, texture( c3d_mapBlur3, vtx_texture ) );
				auto materialId = writer.declLocale( cuT( "materialId" )
					, writer.cast< Int >( data5.z() ) );
				auto translucency = writer.declLocale( cuT( "translucency" )
					, data4.w() );
				auto material = materials->getBaseMaterial( materialId );

#if !C3D_DEBUG_SSS_TRANSMITTANCE
				IF( writer, material->m_subsurfaceScatteringEnabled() == 0_i )
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
			return writer.finalise();
		}

		renderer::ShaderStageStateArray doCreateBlurProgram( Engine & engine
			, bool isVertic
			, glsl::Shader & vertexShader
			, glsl::Shader & pixelShader )
		{
			auto & device = getCurrentDevice( engine );
			vertexShader = doGetVertexProgram( engine );
			pixelShader = doGetBlurProgram( engine, isVertic );

			renderer::ShaderStageStateArray program
			{
				{ device.createShaderModule( renderer::ShaderStageFlag::eVertex ) },
				{ device.createShaderModule( renderer::ShaderStageFlag::eFragment ) },
			};
			program[0].module->loadShader( vertexShader.getSource() );
			program[1].module->loadShader( pixelShader.getSource() );
			return program;
		}

		renderer::ShaderStageStateArray doCreateCombineProgram( Engine & engine
			, glsl::Shader & vertexShader
			, glsl::Shader & pixelShader )
		{
			auto & device = getCurrentDevice( engine );
			vertexShader = doGetVertexProgram( engine );
			pixelShader = doGetCombineProgram( engine );

			renderer::ShaderStageStateArray program
			{
				{ device.createShaderModule( renderer::ShaderStageFlag::eVertex ) },
				{ device.createShaderModule( renderer::ShaderStageFlag::eFragment ) },
			};
			program[0].module->loadShader( vertexShader.getSource() );
			program[1].module->loadShader( pixelShader.getSource() );
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
				sampler->setMinFilter( renderer::Filter::eNearest );
				sampler->setMagFilter( renderer::Filter::eNearest );
				sampler->setWrapS( renderer::WrapMode::eClampToEdge );
				sampler->setWrapT( renderer::WrapMode::eClampToEdge );
			}

			return sampler;
		}

		TextureUnit doCreateTexture( Engine & engine
			, Size const & size )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto sampler = doCreateSampler( engine );

			renderer::ImageCreateInfo image{};
			image.arrayLayers = 1u;
			image.extent.width = size.getWidth();
			image.extent.height = size.getHeight();
			image.extent.depth = 1u;
			image.format = renderer::Format::eR32G32B32A32_SFLOAT;
			image.imageType = renderer::TextureType::e2D;
			image.mipLevels = 1u;
			image.samples = renderer::SampleCountFlag::e1;
			image.usage = renderer::ImageUsageFlag::eColourAttachment | renderer::ImageUsageFlag::eSampled;

			auto texture = std::make_shared< TextureLayout >( renderSystem
				, image
				, renderer::MemoryPropertyFlag::eDeviceLocal );
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
		, renderer::UniformBuffer< BlurConfiguration > const & blurUbo
		, GeometryPassResult const & gp
		, TextureUnit const & source
		, TextureUnit const & destination
		, bool isVertic
		, renderer::ShaderStageStateArray const & shaderStages )
		: RenderQuad{ renderSystem, false, false }
		, m_renderSystem{ renderSystem }
		, m_geometryBufferResult{ gp }
		, m_gpInfoUbo{ gpInfoUbo }
		, m_sceneUbo{ sceneUbo }
		, m_blurUbo{ blurUbo }
	{
		auto & device = getCurrentDevice( renderSystem );

		// Create the render pass.
		renderer::RenderPassCreateInfo renderPass;
		renderPass.flags = 0u;

		renderPass.attachments.resize( 1u );
		renderPass.attachments[0].format = destination.getTexture()->getPixelFormat();
		renderPass.attachments[0].loadOp = renderer::AttachmentLoadOp::eClear;
		renderPass.attachments[0].storeOp = renderer::AttachmentStoreOp::eStore;
		renderPass.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].samples = renderer::SampleCountFlag::e1;
		renderPass.attachments[0].initialLayout = renderer::ImageLayout::eUndefined;
		renderPass.attachments[0].finalLayout = renderer::ImageLayout::eShaderReadOnlyOptimal;

		renderPass.subpasses.resize( 1u );
		renderPass.subpasses[0].flags = 0u;
		renderPass.subpasses[0].pipelineBindPoint = renderer::PipelineBindPoint::eGraphics;
		renderPass.subpasses[0].colorAttachments.push_back( { 0u, renderer::ImageLayout::eColourAttachmentOptimal } );
		
		renderPass.dependencies.resize( 2u );
		renderPass.dependencies[0].srcSubpass = renderer::ExternalSubpass;
		renderPass.dependencies[0].dstSubpass = 0u;
		renderPass.dependencies[0].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[0].dstAccessMask = renderer::AccessFlag::eShaderRead;
		renderPass.dependencies[0].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].dstStageMask = renderer::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[0].dependencyFlags = renderer::DependencyFlag::eByRegion;

		renderPass.dependencies[1].srcSubpass = 0u;
		renderPass.dependencies[1].dstSubpass = renderer::ExternalSubpass;
		renderPass.dependencies[1].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[1].dstAccessMask = renderer::AccessFlag::eShaderRead;
		renderPass.dependencies[1].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[1].dstStageMask = renderer::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[1].dependencyFlags = renderer::DependencyFlag::eByRegion;

		m_renderPass = device.createRenderPass( renderPass );

		// Initialise the frame buffer.
		renderer::Extent2D extent{ size.getWidth(), size.getHeight() };
		renderer::FrameBufferAttachmentArray attaches;
		attaches.emplace_back( *m_renderPass->getAttachments().begin(), destination.getTexture()->getDefaultView() );
		m_frameBuffer = m_renderPass->createFrameBuffer( extent
			, std::move( attaches ) );

		renderer::DescriptorSetLayoutBindingArray bindings
		{
			renderSystem.getEngine()->getMaterialCache().getPassBuffer().createLayoutBinding(),
			{ SceneUbo::BindingPoint, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eFragment }, // Scene UBO
			{ GpInfoUbo::BindingPoint, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eFragment }, // GpInfo UBO
			{ 4u, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eFragment }, // Blur UBO
			{ 6u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment }, // Depth map
			{ 7u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment }, // Translucency map
			{ 8u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment }, // MaterialIndex map
		};

		createPipeline( extent
			, Position{}
			, shaderStages
			, source.getTexture()->getDefaultView()
			, *m_renderPass
			, bindings
			, {} );
	}

	void SubsurfaceScatteringPass::Blur::prepareFrame( renderer::CommandBuffer & commandBuffer )const
	{
		commandBuffer.beginRenderPass( *m_renderPass
			, *m_frameBuffer
			, { renderer::ClearColorValue{ 0, 0, 0, 1 } }
			, renderer::SubpassContents::eInline );
		registerFrame( commandBuffer );
		commandBuffer.endRenderPass();
	}

	void SubsurfaceScatteringPass::Blur::doFillDescriptorSet( renderer::DescriptorSetLayout & descriptorSetLayout
		, renderer::DescriptorSet & descriptorSet )
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
		, renderer::UniformBuffer< BlurWeights > const & blurUbo
		, GeometryPassResult const & gp
		, TextureUnit const & source
		, std::array< TextureUnit, 3u > const & blurResults
		, TextureUnit const & destination
		, renderer::ShaderStageStateArray const & shaderStages )
		: RenderQuad{ renderSystem, false, false }
		, m_renderSystem{ renderSystem }
		, m_blurUbo{ blurUbo }
		, m_geometryBufferResult{ gp }
		, m_source{ source }
		, m_blurResults{ blurResults }
	{
		auto & device = getCurrentDevice( renderSystem );

		// Create the render pass.
		renderer::RenderPassCreateInfo renderPass;
		renderPass.flags = 0u;

		renderPass.attachments.resize( 1u );
		renderPass.attachments[0].format = destination.getTexture()->getPixelFormat();
		renderPass.attachments[0].loadOp = renderer::AttachmentLoadOp::eClear;
		renderPass.attachments[0].storeOp = renderer::AttachmentStoreOp::eStore;
		renderPass.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].samples = renderer::SampleCountFlag::e1;
		renderPass.attachments[0].initialLayout = renderer::ImageLayout::eUndefined;
		renderPass.attachments[0].finalLayout = renderer::ImageLayout::eShaderReadOnlyOptimal;

		renderPass.subpasses.resize( 1u );
		renderPass.subpasses[0].flags = 0u;
		renderPass.subpasses[0].pipelineBindPoint = renderer::PipelineBindPoint::eGraphics;
		renderPass.subpasses[0].colorAttachments.push_back( { 0u, renderer::ImageLayout::eColourAttachmentOptimal } );

		renderPass.dependencies.resize( 2u );
		renderPass.dependencies[0].srcSubpass = renderer::ExternalSubpass;
		renderPass.dependencies[0].dstSubpass = 0u;
		renderPass.dependencies[0].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[0].dstAccessMask = renderer::AccessFlag::eShaderRead;
		renderPass.dependencies[0].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].dstStageMask = renderer::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[0].dependencyFlags = renderer::DependencyFlag::eByRegion;

		renderPass.dependencies[1].srcSubpass = 0u;
		renderPass.dependencies[1].dstSubpass = renderer::ExternalSubpass;
		renderPass.dependencies[1].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[1].dstAccessMask = renderer::AccessFlag::eShaderRead;
		renderPass.dependencies[1].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[1].dstStageMask = renderer::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[1].dependencyFlags = renderer::DependencyFlag::eByRegion;

		m_renderPass = device.createRenderPass( renderPass );

		// Initialise the frame buffer.
		renderer::Extent2D extent{ size.getWidth(), size.getHeight() };
		renderer::FrameBufferAttachmentArray attaches;
		attaches.emplace_back( *m_renderPass->getAttachments().begin(), destination.getTexture()->getDefaultView() );
		m_frameBuffer = m_renderPass->createFrameBuffer( extent
			, std::move( attaches ) );

		renderer::DescriptorSetLayoutBindingArray bindings
		{
			renderSystem.getEngine()->getMaterialCache().getPassBuffer().createLayoutBinding(),
			{ 1u, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eFragment },			// Blur weights
			{ 2u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment },	// Translucency map
			{ 3u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment },	// MaterialIndex map
			{ 4u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment },	// Blur result 0
			{ 5u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment },	// Blur result 1
			{ 6u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment },	// Blur result 2
		};

		createPipeline( extent
			, Position{}
			, shaderStages
			, source.getTexture()->getDefaultView()
			, *m_renderPass
			, bindings
			, {} );
	}

	void SubsurfaceScatteringPass::Combine::prepareFrame( renderer::CommandBuffer & commandBuffer )const
	{
		commandBuffer.beginRenderPass( *m_renderPass
			, *m_frameBuffer
			, { renderer::ClearColorValue{ 0, 1, 0, 1 } }
		, renderer::SubpassContents::eInline );
		registerFrame( commandBuffer );
		commandBuffer.endRenderPass();
	}

	void SubsurfaceScatteringPass::Combine::doFillDescriptorSet( renderer::DescriptorSetLayout & descriptorSetLayout
		, renderer::DescriptorSet & descriptorSet )
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
		, m_blurConfigUbo{ renderer::makeUniformBuffer< BlurConfiguration >( getCurrentDevice( engine )
			, 1u
			, renderer::BufferTarget::eTransferDst
			, renderer::MemoryPropertyFlag::eHostVisible ) }
		, m_blurWeightsUbo{ renderer::makeUniformBuffer< BlurWeights >( getCurrentDevice( engine )
			, 1u
			, renderer::BufferTarget::eTransferDst
			, renderer::MemoryPropertyFlag::eHostVisible ) }
		, m_intermediate{ doCreateTexture( engine, textureSize ) }
		, m_blurHorizProgram{ doCreateBlurProgram( engine, false, m_blurHorizVertexShader, m_blurHorizPixelShader ) }
		, m_blurVerticProgram{ doCreateBlurProgram( engine, true, m_blurVerticVertexShader, m_blurVerticPixelShader ) }
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
		m_fence = device.createFence( renderer::FenceCreateFlag::eSignaled );
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
		if ( m_commandBuffer->begin() )
		{
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
	}

	renderer::Semaphore const & SubsurfaceScatteringPass::render( renderer::Semaphore const & toWait )const
	{
		auto & device = getCurrentDevice( *this );
		m_timer->notifyPassRender();
		device.getGraphicsQueue().submit( *m_commandBuffer
			, toWait
			, renderer::PipelineStageFlag::eColourAttachmentOutput
			, *m_finished
			, nullptr );
		return *m_finished;
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
			, renderer::ShaderStageFlag::eVertex
			, m_blurHorizVertexShader );
		visitor.visit( cuT( "SSSSS - Blur X" )
			, renderer::ShaderStageFlag::eFragment
			, m_blurHorizPixelShader );

		visitor.visit( cuT( "SSSSS - Blur Y" )
			, renderer::ShaderStageFlag::eVertex
			, m_blurVerticVertexShader );
		visitor.visit( cuT( "SSSSS - Blur Y" )
			, renderer::ShaderStageFlag::eFragment
			, m_blurVerticPixelShader );

		visitor.visit( cuT( "SSSSS - Combine" )
			, renderer::ShaderStageFlag::eVertex
			, m_combineVertexShader );
		visitor.visit( cuT( "SSSSS - Combine" )
			, renderer::ShaderStageFlag::eFragment
			, m_combinePixelShader );
	}
}
