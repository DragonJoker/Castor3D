#include "RadianceComputer.hpp"

#include "Engine.hpp"
#include "Shader/Ubos/MatrixUbo.hpp"

#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <Buffer/UniformBuffer.hpp>
#include <Buffer/VertexBuffer.hpp>
#include <Descriptor/DescriptorSet.hpp>
#include <Descriptor/DescriptorSetLayout.hpp>
#include <Descriptor/DescriptorSetPool.hpp>
#include <Image/Texture.hpp>
#include <Image/TextureView.hpp>
#include <Pipeline/DepthStencilState.hpp>
#include <Pipeline/InputAssemblyState.hpp>
#include <Pipeline/Pipeline.hpp>
#include <Pipeline/PipelineLayout.hpp>
#include <Pipeline/Scissor.hpp>
#include <Pipeline/VertexLayout.hpp>
#include <Pipeline/Viewport.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>
#include <RenderPass/RenderSubpass.hpp>
#include <RenderPass/RenderSubpassState.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>
#include <Shader/ShaderProgram.hpp>

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		TextureUnit doCreateRadianceTexture( Engine & engine
			, Size const & size )
		{
			renderer::ImageCreateInfo image{};
			image.flags = renderer::ImageCreateFlag::eCubeCompatible;
			image.arrayLayers = 6u;
			image.extent.width = 32u;
			image.extent.height = 32u;
			image.extent.depth = 1u;
			image.format = renderer::Format::eR32G32B32_SFLOAT;
			image.imageType = renderer::TextureType::e2D;
			image.initialLayout = renderer::ImageLayout::eUndefined;
			image.mipLevels = 1u;
			image.samples = renderer::SampleCountFlag::e1;
			image.sharingMode = renderer::SharingMode::eExclusive;
			image.tiling = renderer::ImageTiling::eOptimal;
			image.usage = renderer::ImageUsageFlag::eColourAttachment | renderer::ImageUsageFlag::eSampled;
			auto texture = std::make_shared< TextureLayout >( *engine.getRenderSystem()
				, image
				, renderer::MemoryPropertyFlag::eDeviceLocal );
			SamplerSPtr sampler;
			auto name = cuT( "IblTexturesRadiance" );

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().create( name );
				sampler->setMinFilter( renderer::Filter::eLinear );
				sampler->setMagFilter( renderer::Filter::eLinear );
				sampler->setWrapS( renderer::WrapMode::eClampToEdge );
				sampler->setWrapT( renderer::WrapMode::eClampToEdge );
				sampler->setWrapR( renderer::WrapMode::eClampToEdge );
				engine.getSamplerCache().add( name, sampler );
			}

			sampler->initialise();
			TextureUnit result{ engine };
			result.setTexture( texture );
			result.setSampler( sampler );
			result.initialise();
			return result;
		}

	}

	RadianceComputer::RadianceComputer( Engine & engine
		, Size const & size
		, renderer::Texture const & srcTexture )
		: m_renderSystem{ *engine.getRenderSystem() }
		, m_result{ doCreateRadianceTexture( engine, size ) }
		, m_vertexData
		{
			{
				Point3f{ -1, +1, -1 }, Point3f{ +1, -1, -1 }, Point3f{ -1, -1, -1 }, Point3f{ +1, -1, -1 }, Point3f{ -1, +1, -1 }, Point3f{ +1, +1, -1 },
				Point3f{ -1, -1, +1 }, Point3f{ -1, +1, -1 }, Point3f{ -1, -1, -1 }, Point3f{ -1, +1, -1 }, Point3f{ -1, -1, +1 }, Point3f{ -1, +1, +1 },
				Point3f{ +1, -1, -1 }, Point3f{ +1, +1, +1 }, Point3f{ +1, -1, +1 }, Point3f{ +1, +1, +1 }, Point3f{ +1, -1, -1 }, Point3f{ +1, +1, -1 },
				Point3f{ -1, -1, +1 }, Point3f{ +1, +1, +1 }, Point3f{ -1, +1, +1 }, Point3f{ +1, +1, +1 }, Point3f{ -1, -1, +1 }, Point3f{ +1, -1, +1 },
				Point3f{ -1, +1, -1 }, Point3f{ +1, +1, +1 }, Point3f{ +1, +1, -1 }, Point3f{ +1, +1, +1 }, Point3f{ -1, +1, -1 }, Point3f{ -1, +1, +1 },
				Point3f{ -1, -1, -1 }, Point3f{ +1, -1, -1 }, Point3f{ -1, -1, +1 }, Point3f{ +1, -1, -1 }, Point3f{ +1, -1, +1 }, Point3f{ -1, -1, +1 },
			}
		}
	{
		auto & dstTexture = m_result.getTexture()->getTexture();

		if ( !engine.getSamplerCache().has( cuT( "RadianceComputer" ) ) )
		{
			m_sampler = engine.getSamplerCache().add( cuT( "RadianceComputer" ) );
			m_sampler->setMinFilter( renderer::Filter::eLinear );
			m_sampler->setMagFilter( renderer::Filter::eLinear );
			m_sampler->setWrapS( renderer::WrapMode::eClampToEdge );
			m_sampler->setWrapT( renderer::WrapMode::eClampToEdge );
			m_sampler->setWrapR( renderer::WrapMode::eClampToEdge );
			m_sampler->initialise();
		}
		else
		{
			m_sampler = engine.getSamplerCache().find( cuT( "RadianceComputer" ) );
		}
		
		auto & device = *m_renderSystem.getCurrentDevice();
		// Initialise the vertex buffer.
		m_vertexBuffer = renderer::makeVertexBuffer< NonTexturedCube >( device
			, 1u
			, 0u
			, renderer::MemoryPropertyFlag::eHostVisible );

		// Initialise the vertex layout.
		m_vertexLayout = renderer::makeLayout< NonTexturedCube >( 0u );
		m_vertexLayout->createAttribute( 0u, renderer::Format::eR32G32B32_SFLOAT, offsetof( NonTexturedCube::Quad::Vertex, position ) );

		// Initialise the UBO.
		m_configUbo = renderer::makeUniformBuffer< castor::Matrix4x4f >( device
			, 6u
			, renderer::BufferTarget::eTransferDst
			, renderer::MemoryPropertyFlag::eHostVisible );
		m_configUbo->getData( 0u ) = matrix::lookAt( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ +1.0f, +0.0f, +0.0f }, Point3r{ 0.0f, -1.0f, +0.0f } );
		m_configUbo->getData( 1u ) = matrix::lookAt( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ -1.0f, +0.0f, +0.0f }, Point3r{ 0.0f, -1.0f, +0.0f } );
		m_configUbo->getData( 2u ) = matrix::lookAt( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ +0.0f, +1.0f, +0.0f }, Point3r{ 0.0f, +0.0f, +1.0f } );
		m_configUbo->getData( 3u ) = matrix::lookAt( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ +0.0f, -1.0f, +0.0f }, Point3r{ 0.0f, +0.0f, -1.0f } );
		m_configUbo->getData( 4u ) = matrix::lookAt( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ +0.0f, +0.0f, +1.0f }, Point3r{ 0.0f, -1.0f, +0.0f } );
		m_configUbo->getData( 5u ) = matrix::lookAt( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ +0.0f, +0.0f, -1.0f }, Point3r{ 0.0f, -1.0f, +0.0f } );
		m_configUbo->upload();

		// Prepare descriptor and pipeline layouts.
		renderer::DescriptorSetLayoutBindingArray bindings;
		bindings.emplace_back( 0u
			, renderer::DescriptorType::eCombinedImageSampler
			, renderer::ShaderStageFlag::eFragment );
		bindings.emplace_back( 1u
			, renderer::DescriptorType::eUniformBuffer
			, renderer::ShaderStageFlag::eVertex );
		m_descriptorLayout = device.createDescriptorSetLayout( std::move( bindings ) );
		m_descriptorPool = m_descriptorLayout->createPool( 6u * ( glsl::Utils::MaxIblReflectionLod + 1 ) );
		m_pipelineLayout = device.createPipelineLayout( *m_descriptorLayout );

		// Create the render passes.
		renderer::RenderPassCreateInfo createInfo{};
		createInfo.flags = 0u;

		createInfo.attachments.resize( 1u );
		createInfo.attachments[0].index = 0u;
		createInfo.attachments[0].format = dstTexture.getFormat();
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

		auto program = doCreateProgram();
		
		for ( auto face = 0u; face < 6u; ++face )
		{
			auto & facePass = m_renderPasses[face];

			// Create the views.
			facePass.srcView = srcTexture.createView( renderer::TextureViewType::e2D
				, srcTexture.getFormat()
				, 0u
				, 1u
				, face
				, 1u );
			facePass.dstView = dstTexture.createView( renderer::TextureViewType::e2D
				, dstTexture.getFormat()
				, 0u
				, 1u
				, face
				, 1u );

			// Initialise the descriptor set.
			facePass.descriptorSet = m_descriptorPool->createDescriptorSet();
			facePass.descriptorSet->createBinding( m_descriptorLayout->getBinding( 0u )
				, *facePass.srcView
				, m_sampler->getSampler() );
			facePass.descriptorSet->createBinding( m_descriptorLayout->getBinding( 1u )
				, *m_configUbo
				, face
				, 1u );
			facePass.descriptorSet->update();

			// Create the render pass.
			facePass.renderPass = device.createRenderPass( createInfo );

			// Initialise the frame buffer.
			renderer::FrameBufferAttachmentArray attaches;
			attaches.emplace_back( *( facePass.renderPass->getAttachments().begin() ), *facePass.dstView );
			facePass.frameBuffer = facePass.renderPass->createFrameBuffer( renderer::Extent2D{ size.getWidth(), size.getHeight() }
				, std::move( attaches ) );

			// Initialise the pipeline.
			facePass.pipeline = m_pipelineLayout->createPipeline( {
				program,
				*facePass.renderPass,
				renderer::VertexInputState::create( *m_vertexLayout ),
				renderer::InputAssemblyState{ renderer::PrimitiveTopology::eTriangleStrip },
				renderer::RasterisationState{},
				renderer::MultisampleState{},
				renderer::ColourBlendState::createDefault(),
				{},
				renderer::DepthStencilState{ 0u, false, false },
				std::nullopt,
				renderer::Viewport{ size[0], size[0], 0, 0, },
				renderer::Scissor{ 0, 0, size[0], size[0] },
			} );
		}

		m_commandBuffer = device.getGraphicsCommandPool().createCommandBuffer();

		for ( auto face = 0u; face < 6u; ++face )
		{
			auto & facePass = m_renderPasses[face];
			m_commandBuffer->beginRenderPass( *facePass.renderPass
				, *facePass.frameBuffer
				, { renderer::ClearColorValue{ 0, 0, 0, 0 } }
			, renderer::SubpassContents::eInline );
			m_commandBuffer->bindPipeline( *facePass.pipeline );
			m_commandBuffer->bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
			m_commandBuffer->draw( 36u, 1u, 0u, 0u );
			m_commandBuffer->endRenderPass();
		}
	}

	void RadianceComputer::render()
	{
		auto & device = *m_renderSystem.getCurrentDevice();
		device.getGraphicsQueue().submit( *m_commandBuffer, nullptr );
	}

	renderer::ShaderStageStateArray RadianceComputer::doCreateProgram()
	{
		glsl::Shader vtx;
		{
			using namespace glsl;
			GlslWriter writer{ m_renderSystem.createGlslWriter() };

			// Inputs
			auto position = writer.declAttribute< Vec3 >( cuT( "position" ), 0u );
			UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0 );

			// Outputs
			auto vtx_worldPosition = writer.declOutput< Vec3 >( cuT( "vtx_worldPosition" ), 0u );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			std::function< void() > main = [&]()
			{
				vtx_worldPosition = position;
				auto view = writer.declLocale( cuT( "normal" )
					, mat4( mat3( c3d_curView ) ) );
				gl_Position = writer.paren( c3d_projection * view * vec4( position, 1.0 ) ).SWIZZLE_XYWW;
			};

			writer.implementFunction< void >( cuT( "main" ), main );
			vtx = writer.finalise();
		}

		glsl::Shader pxl;
		{
			using namespace glsl;
			GlslWriter writer{ m_renderSystem.createGlslWriter() };

			// Inputs
			auto vtx_worldPosition = writer.declInput< Vec3 >( cuT( "vtx_worldPosition" ), 0u );
			auto c3d_mapDiffuse = writer.declSampler< SamplerCube >( cuT( "c3d_mapDiffuse" ), MinBufferIndex, 0u );

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

		auto & cache = m_renderSystem.getEngine()->getShaderProgramCache();
		auto program = cache.getNewProgram( false );
		program.push_back( { m_renderSystem.getCurrentDevice()->createShaderModule( renderer::ShaderStageFlag::eVertex ) } );
		program.push_back( { m_renderSystem.getCurrentDevice()->createShaderModule( renderer::ShaderStageFlag::eFragment ) } );
		program[0].module->loadShader( vtx.getSource() );
		program[1].module->loadShader( pxl.getSource() );
		return program;
	}
}
