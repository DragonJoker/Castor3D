#include "RadianceComputer.hpp"

#include "Engine.hpp"
#include "Shader/Ubos/MatrixUbo.hpp"

#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <Ashes/Image/Texture.hpp>
#include <Ashes/RenderPass/RenderPassCreateInfo.hpp>
#include <Ashes/RenderPass/FrameBufferAttachment.hpp>
#include <Shader/GlslToSpv.hpp>
#include <Ashes/Sync/Fence.hpp>

#include <ShaderWriter/Source.hpp>
#include "Shader/Shaders/GlslUtils.hpp"

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		ashes::TexturePtr doCreateRadianceTexture( RenderSystem const & renderSystem
			, Size const & size )
		{
			auto & device = getCurrentDevice( renderSystem );
			ashes::ImageCreateInfo image{};
			image.flags = ashes::ImageCreateFlag::eCubeCompatible;
			image.arrayLayers = 6u;
			image.extent.width = size.getWidth();
			image.extent.height = size.getHeight();
			image.extent.depth = 1u;
			image.format = ashes::Format::eR32G32B32A32_SFLOAT;
			image.imageType = ashes::TextureType::e2D;
			image.initialLayout = ashes::ImageLayout::eUndefined;
			image.mipLevels = 1u;
			image.samples = ashes::SampleCountFlag::e1;
			image.sharingMode = ashes::SharingMode::eExclusive;
			image.tiling = ashes::ImageTiling::eOptimal;
			image.usage = ashes::ImageUsageFlag::eColourAttachment | ashes::ImageUsageFlag::eSampled;
			return device.createTexture( image
				, ashes::MemoryPropertyFlag::eDeviceLocal );
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
				result->setMinFilter( ashes::Filter::eLinear );
				result->setMagFilter( ashes::Filter::eLinear );
				result->setWrapS( ashes::WrapMode::eClampToEdge );
				result->setWrapT( ashes::WrapMode::eClampToEdge );
				result->setWrapR( ashes::WrapMode::eClampToEdge );
				engine.getSamplerCache().add( name, result );
			}

			result->initialise();
			return result;
		}

		ashes::TextureViewPtr doCreateSrcView( ashes::Texture const & texture )
		{
			return texture.createView( ashes::TextureViewType::eCube
				, texture.getFormat()
				, 0u
				, 1u
				, 0u
				, 6u );
		}

		ashes::ShaderStageStateArray doCreateProgram( RenderSystem & renderSystem )
		{
			ShaderModule vtx{ ashes::ShaderStageFlag::eVertex, "RadianceCompute" };
			{
				using namespace sdw;
				VertexWriter writer;

				// Inputs
				auto position = writer.declInput< Vec3 >( "position", 0u );
				Ubo matrix{ writer, "Matrix", 0u, 0u };
				auto c3d_viewProjection = matrix.declMember< Mat4 >( "c3d_viewProjection" );
				matrix.end();

				// Outputs
				auto vtx_worldPosition = writer.declOutput< Vec3 >( "vtx_worldPosition", 0u );
				auto out = writer.getOut();

				writer.implementFunction< Void >( "main"
					, [&]()
					{
						vtx_worldPosition = position;
						out.gl_out.gl_Position = writer.paren( c3d_viewProjection * vec4( position, 1.0 ) ).xyww();
					} );
				vtx.shader = std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
			}

			ShaderModule pxl{ ashes::ShaderStageFlag::eFragment, "RadianceCompute" };
			{
				using namespace sdw;
				FragmentWriter writer;

				// Inputs
				auto vtx_worldPosition = writer.declInput< Vec3 >( "vtx_worldPosition", 0u );
				auto c3d_mapDiffuse = writer.declSampledImage< FImgCubeRgba32 >( "c3d_mapDiffuse", 1u, 0u );

				// Outputs
				auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_FragColor", 0u );

				writer.implementFunction< Void >( "main"
					, [&]()
					{
						// From https://learnopengl.com/#!PBR/Lighting
						// the sample direction equals the hemisphere's orientation 
						auto normal = writer.declLocale( "normal"
							, normalize( vtx_worldPosition ) );

						auto irradiance = writer.declLocale( "irradiance"
							, vec3( 0.0_f ) );

						auto up = writer.declLocale( "up"
							, vec3( 0.0_f, 1.0_f, 0.0_f ) );
						auto right = writer.declLocale( "right"
							, cross( up, normal ) );
						up = cross( normal, right );

						auto sampleDelta = writer.declLocale( "sampleDelta"
							, 0.025_f );
						auto nrSamples = writer.declLocale( "nrSamples"
							, 0_i );

						FOR( writer, Float, phi, 0.0_f, phi < Float{ PiMult2< float > }, phi += sampleDelta )
						{
							FOR( writer, Float, theta, 0.0_f, theta < Float{ PiDiv2< float > }, theta += sampleDelta )
							{
								// spherical to cartesian (in tangent space)
								auto tangentSample = writer.declLocale( "tangentSample"
									, vec3( sin( theta ) * cos( phi ), sin( theta ) * sin( phi ), cos( theta ) ) );
								// tangent space to world
								auto sampleVec = writer.declLocale( "sampleVec"
									, right * tangentSample.x() + up * tangentSample.y() + normal * tangentSample.z() );

								irradiance += texture( c3d_mapDiffuse, sampleVec ).rgb() * cos( theta ) * sin( theta );
								nrSamples = nrSamples + 1;
							}
							ROF;
						}
						ROF;

						irradiance = irradiance * Float{ Pi< float > } * writer.paren( 1.0_f / writer.cast< Float >( nrSamples ) );
						pxl_fragColor = vec4( irradiance, 1.0_f );
					} );

				pxl.shader = std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
			}

			ashes::ShaderStageStateArray program
			{
				{ getCurrentDevice( renderSystem ).createShaderModule( ashes::ShaderStageFlag::eVertex ) },
				{ getCurrentDevice( renderSystem ).createShaderModule( ashes::ShaderStageFlag::eFragment ) }
			};
			program[0].module->loadShader( renderSystem.compileShader( vtx ) );
			program[1].module->loadShader( renderSystem.compileShader( pxl ) );
			return program;
		}

		ashes::RenderPassPtr doCreateRenderPass( RenderSystem const & renderSystem
			, ashes::Format format )
		{
			auto & device = getCurrentDevice( renderSystem );
			ashes::RenderPassCreateInfo createInfo{};
			createInfo.flags = 0u;

			createInfo.attachments.resize( 1u );
			createInfo.attachments[0].format = format;
			createInfo.attachments[0].samples = ashes::SampleCountFlag::e1;
			createInfo.attachments[0].loadOp = ashes::AttachmentLoadOp::eDontCare;
			createInfo.attachments[0].storeOp = ashes::AttachmentStoreOp::eStore;
			createInfo.attachments[0].stencilLoadOp = ashes::AttachmentLoadOp::eDontCare;
			createInfo.attachments[0].stencilStoreOp = ashes::AttachmentStoreOp::eDontCare;
			createInfo.attachments[0].initialLayout = ashes::ImageLayout::eUndefined;
			createInfo.attachments[0].finalLayout = ashes::ImageLayout::eShaderReadOnlyOptimal;

			ashes::AttachmentReference colourReference;
			colourReference.attachment = 0u;
			colourReference.layout = ashes::ImageLayout::eColourAttachmentOptimal;

			createInfo.subpasses.resize( 1u );
			createInfo.subpasses[0].flags = 0u;
			createInfo.subpasses[0].colorAttachments = { colourReference };

			createInfo.dependencies.resize( 1u );
			createInfo.dependencies[0].srcSubpass = 0u;
			createInfo.dependencies[0].dstSubpass = ashes::ExternalSubpass;
			createInfo.dependencies[0].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite;
			createInfo.dependencies[0].dstAccessMask = ashes::AccessFlag::eShaderRead;
			createInfo.dependencies[0].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
			createInfo.dependencies[0].dstStageMask = ashes::PipelineStageFlag::eFragmentShader;
			createInfo.dependencies[0].dependencyFlags = ashes::DependencyFlag::eByRegion;

			return device.createRenderPass( createInfo );
		}
	}

	//*********************************************************************************************

	RadianceComputer::RadianceComputer( Engine & engine
		, Size const & size
		, ashes::Texture const & srcTexture )
		: RenderCube{ *engine.getRenderSystem(), false }
		, m_renderSystem{ *engine.getRenderSystem() }
		, m_result{ doCreateRadianceTexture( m_renderSystem, size ) }
		, m_resultView{ m_result->createView( ashes::TextureViewType::eCube, m_result->getFormat(), 0u, m_result->getMipmapLevels(), 0u, 6u ) }
		, m_sampler{ doCreateSampler( engine ) }
		, m_srcView{ doCreateSrcView( srcTexture ) }
		, m_renderPass{ doCreateRenderPass( m_renderSystem, m_result->getFormat() ) }
	{
		auto & dstTexture = *m_result;
		auto & device = getCurrentDevice( m_renderSystem );
		
		for ( auto face = 0u; face < 6u; ++face )
		{
			auto & facePass = m_renderPasses[face];

			// Create the views.
			facePass.dstView = dstTexture.createView( ashes::TextureViewType::e2D
				, dstTexture.getFormat()
				, 0u
				, 1u
				, face
				, 1u );
			// Initialise the frame buffer.
			ashes::FrameBufferAttachmentArray attaches;
			attaches.emplace_back( *( m_renderPass->getAttachments().begin() ), *facePass.dstView );
			facePass.frameBuffer = m_renderPass->createFrameBuffer( ashes::Extent2D{ size.getWidth(), size.getHeight() }
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
				, { ashes::ClearColorValue{ 0, 0, 0, 0 } }
				, ashes::SubpassContents::eInline );
			registerFrame( *m_commandBuffer, face );
			m_commandBuffer->endRenderPass();
		}

		m_commandBuffer->end();
	}

	void RadianceComputer::render()
	{
		auto & device = getCurrentDevice( m_renderSystem );
		device.getGraphicsQueue().submit( *m_commandBuffer, nullptr );
		device.getGraphicsQueue().waitIdle();
	}

	//*********************************************************************************************
}
