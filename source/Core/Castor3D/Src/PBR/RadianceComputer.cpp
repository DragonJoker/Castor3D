#include "RadianceComputer.hpp"

#include "Engine.hpp"
#include "Shader/Ubos/MatrixUbo.hpp"

#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <Image/Texture.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>
#include <Shader/ShaderProgram.hpp>

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		renderer::TexturePtr doCreateRadianceTexture( RenderSystem const & renderSystem
			, Size const & size )
		{
			auto & device = *renderSystem.getCurrentDevice();
			renderer::ImageCreateInfo image{};
			image.flags = renderer::ImageCreateFlag::eCubeCompatible;
			image.arrayLayers = 6u;
			image.extent.width = size.getWidth();
			image.extent.height = size.getHeight();
			image.extent.depth = 1u;
			image.format = renderer::Format::eR32G32B32A32_SFLOAT;
			image.imageType = renderer::TextureType::e2D;
			image.initialLayout = renderer::ImageLayout::eUndefined;
			image.mipLevels = 1u;
			image.samples = renderer::SampleCountFlag::e1;
			image.sharingMode = renderer::SharingMode::eExclusive;
			image.tiling = renderer::ImageTiling::eOptimal;
			image.usage = renderer::ImageUsageFlag::eColourAttachment | renderer::ImageUsageFlag::eSampled;
			return device.createTexture( image
				, renderer::MemoryPropertyFlag::eDeviceLocal );
		}

		SamplerSPtr doCreateSampler( Engine & engine )
		{
			SamplerSPtr result;
			auto name = cuT( "IblTexturesRadiance" );

			if ( engine.getSamplerCache().has( name ) )
			{
				result = engine.getSamplerCache().find( name );
			}
			else
			{
				result = engine.getSamplerCache().create( name );
				result->setMinFilter( renderer::Filter::eLinear );
				result->setMagFilter( renderer::Filter::eLinear );
				result->setWrapS( renderer::WrapMode::eClampToEdge );
				result->setWrapT( renderer::WrapMode::eClampToEdge );
				result->setWrapR( renderer::WrapMode::eClampToEdge );
				engine.getSamplerCache().add( name, result );
			}

			result->initialise();
			return result;
		}

		renderer::TextureViewPtr doCreateSrcView( renderer::Texture const & texture )
		{
			return texture.createView( renderer::TextureViewType::eCube
				, texture.getFormat()
				, 0u
				, 1u
				, 0u
				, 6u );
		}

		renderer::ShaderStageStateArray doCreateProgram( RenderSystem & renderSystem )
		{
			glsl::Shader vtx;
			{
				using namespace glsl;
				GlslWriter writer{ renderSystem.createGlslWriter() };

				// Inputs
				auto position = writer.declAttribute< Vec3 >( cuT( "position" ), 0u );
				Ubo matrix{ writer, cuT( "Matrix" ), 0u, 0u };
				auto c3d_viewProjection = matrix.declMember< Mat4 >( cuT( "c3d_viewProjection" ) );
				matrix.end();

				// Outputs
				auto vtx_worldPosition = writer.declOutput< Vec3 >( cuT( "vtx_worldPosition" ), 0u );
				auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

				std::function< void() > main = [&]()
				{
					vtx_worldPosition = position;
					gl_Position = writer.paren( c3d_viewProjection * vec4( position, 1.0 ) ).SWIZZLE_XYWW;
				};

				writer.implementFunction< void >( cuT( "main" ), main );
				vtx = writer.finalise();
			}

			glsl::Shader pxl;
			{
				using namespace glsl;
				GlslWriter writer{ renderSystem.createGlslWriter() };

				// Inputs
				auto vtx_worldPosition = writer.declInput< Vec3 >( cuT( "vtx_worldPosition" ), 0u );
				auto c3d_mapDiffuse = writer.declSampler< SamplerCube >( cuT( "c3d_mapDiffuse" ), 1u, 0u );

				// Outputs
				auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_FragColor" ), 0u );

				writer.implementFunction< void >( cuT( "main" ), [&]()
				{
					// From https://learnopengl.com/#!PBR/Lighting
					// the sample direction equals the hemisphere's orientation 
					auto normal = writer.declLocale( cuT( "normal" )
						, normalize( vtx_worldPosition ) );

					auto irradiance = writer.declLocale( cuT( "irradiance" )
						, vec3( 0.0_f ) );

					auto up = writer.declLocale( cuT( "up" )
						, vec3( 0.0_f, 1.0, 0.0 ) );
					auto right = writer.declLocale( cuT( "right" )
						, cross( up, normal ) );
					up = cross( normal, right );

					auto sampleDelta = writer.declLocale( cuT( "sampleDelta" )
						, 0.025_f );
					auto nrSamples = writer.declLocale( cuT( "nrSamples" )
						, 0_i );
					auto PI = writer.declLocale( cuT( "PI" )
						, 3.14159265359_f );

					FOR( writer, Float, phi, 0.0, "phi < 2.0 * PI", "phi += sampleDelta" )
					{
						FOR( writer, Float, theta, 0.0, "theta < 0.5 * PI", "theta += sampleDelta" )
						{
							// spherical to cartesian (in tangent space)
							auto tangentSample = writer.declLocale( cuT( "tangentSample" )
								, vec3( sin( theta ) * cos( phi ), sin( theta ) * sin( phi ), cos( theta ) ) );
							// tangent space to world
							auto sampleVec = writer.declLocale( cuT( "sampleVec" )
								, right * tangentSample.x() + up * tangentSample.y() + normal * tangentSample.z() );

							irradiance += texture( c3d_mapDiffuse, sampleVec ).rgb() * cos( theta ) * sin( theta );
							nrSamples = nrSamples + 1;
						}
						ROF;
					}
					ROF;

					irradiance = irradiance * PI * writer.paren( 1.0_f / writer.cast< Float >( nrSamples ) );
					pxl_fragColor = vec4( irradiance, 1.0 );
				} );

				pxl = writer.finalise();
			}

			renderer::ShaderStageStateArray program
			{
				{ renderSystem.getCurrentDevice()->createShaderModule( renderer::ShaderStageFlag::eVertex ) },
				{ renderSystem.getCurrentDevice()->createShaderModule( renderer::ShaderStageFlag::eFragment ) }
			};
			program[0].module->loadShader( vtx.getSource() );
			program[1].module->loadShader( pxl.getSource() );
			return program;
		}

		renderer::RenderPassPtr doCreateRenderPass( RenderSystem const & renderSystem
			, renderer::Format format )
		{
			auto & device = *renderSystem.getCurrentDevice();
			renderer::RenderPassCreateInfo createInfo{};
			createInfo.flags = 0u;

			createInfo.attachments.resize( 1u );
			createInfo.attachments[0].format = format;
			createInfo.attachments[0].samples = renderer::SampleCountFlag::e1;
			createInfo.attachments[0].loadOp = renderer::AttachmentLoadOp::eDontCare;
			createInfo.attachments[0].storeOp = renderer::AttachmentStoreOp::eStore;
			createInfo.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
			createInfo.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
			createInfo.attachments[0].initialLayout = renderer::ImageLayout::eUndefined;
			createInfo.attachments[0].finalLayout = renderer::ImageLayout::eColourAttachmentOptimal;

			renderer::AttachmentReference colourReference;
			colourReference.attachment = 0u;
			colourReference.layout = renderer::ImageLayout::eColourAttachmentOptimal;

			createInfo.subpasses.resize( 1u );
			createInfo.subpasses[0].flags = 0u;
			createInfo.subpasses[0].colorAttachments = { colourReference };

			createInfo.dependencies.resize( 2u );
			createInfo.dependencies[0].srcSubpass = renderer::ExternalSubpass;
			createInfo.dependencies[0].dstSubpass = 0u;
			createInfo.dependencies[0].srcStageMask = renderer::PipelineStageFlag::eBottomOfPipe;
			createInfo.dependencies[0].dstStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
			createInfo.dependencies[0].srcAccessMask = renderer::AccessFlag::eMemoryRead;
			createInfo.dependencies[0].dstAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
			createInfo.dependencies[0].dependencyFlags = renderer::DependencyFlag::eByRegion;

			createInfo.dependencies[1].srcSubpass = 0u;
			createInfo.dependencies[1].dstSubpass = renderer::ExternalSubpass;
			createInfo.dependencies[1].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
			createInfo.dependencies[1].dstStageMask = renderer::PipelineStageFlag::eBottomOfPipe;
			createInfo.dependencies[1].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
			createInfo.dependencies[1].dstAccessMask = renderer::AccessFlag::eMemoryRead;
			createInfo.dependencies[1].dependencyFlags = renderer::DependencyFlag::eByRegion;

			return device.createRenderPass( createInfo );
		}
	}

	//*********************************************************************************************

	RadianceComputer::RadianceComputer( Engine & engine
		, Size const & size
		, renderer::Texture const & srcTexture )
		: RenderCube{ *engine.getRenderSystem(), false }
		, m_renderSystem{ *engine.getRenderSystem() }
		, m_result{ doCreateRadianceTexture( m_renderSystem, size ) }
		, m_resultView{ m_result->createView( renderer::TextureViewType::eCube, m_result->getFormat(), 0u, m_result->getMipmapLevels(), 0u, 6u ) }
		, m_sampler{ doCreateSampler( engine ) }
		, m_srcView{ doCreateSrcView( srcTexture ) }
		, m_renderPass{ doCreateRenderPass( m_renderSystem, m_result->getFormat() ) }
	{
		auto & dstTexture = *m_result;
		auto & device = *m_renderSystem.getCurrentDevice();
		
		for ( auto face = 0u; face < 6u; ++face )
		{
			auto & facePass = m_renderPasses[face];

			// Create the views.
			facePass.dstView = dstTexture.createView( renderer::TextureViewType::e2D
				, dstTexture.getFormat()
				, 0u
				, 1u
				, face
				, 1u );
			// Initialise the frame buffer.
			renderer::FrameBufferAttachmentArray attaches;
			attaches.emplace_back( *( m_renderPass->getAttachments().begin() ), *facePass.dstView );
			facePass.frameBuffer = m_renderPass->createFrameBuffer( renderer::Extent2D{ size.getWidth(), size.getHeight() }
				, std::move( attaches ) );
		}

		auto program = doCreateProgram( m_renderSystem );
		createPipelines( { size.getWidth(), size.getHeight() }
			, Position{}
			, program
			, *m_srcView
			, *m_renderPass
			, {} );

		m_commandBuffer = device.getGraphicsCommandPool().createCommandBuffer();
		m_commandBuffer->begin();

		for ( auto face = 0u; face < 6u; ++face )
		{
			auto & facePass = m_renderPasses[face];
			m_commandBuffer->beginRenderPass( *m_renderPass
				, *facePass.frameBuffer
				, { renderer::ClearColorValue{ 0, 0, 0, 0 } }
				, renderer::SubpassContents::eInline );
			registerFrame( *m_commandBuffer, face );
			m_commandBuffer->endRenderPass();
		}

		m_commandBuffer->end();
	}

	void RadianceComputer::render()
	{
		auto & device = *m_renderSystem.getCurrentDevice();
		device.getGraphicsQueue().submit( *m_commandBuffer, nullptr );
		device.getGraphicsQueue().waitIdle();
	}

	//*********************************************************************************************
}
