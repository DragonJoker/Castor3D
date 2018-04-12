#include "Scene/Background/Colour.hpp"

#include "Engine.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Background/Visitor.hpp"
#include "EnvironmentMap/EnvironmentMap.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <RenderPass/FrameBuffer.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>
#include <Sync/ImageMemoryBarrier.hpp>

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

using namespace castor;
using namespace glsl;

namespace castor3d
{
	//************************************************************************************************

	namespace
	{
		static uint32_t constexpr Dim = 16u;

		renderer::ImageCreateInfo doGetImageCreate()
		{
			Size size{ Dim, Dim };
			renderer::ImageCreateInfo result;
			result.flags = renderer::ImageCreateFlag::eCubeCompatible;
			result.arrayLayers = 6u;
			result.extent.width = Dim;
			result.extent.height = Dim;
			result.extent.depth = 1u;
			result.format = renderer::Format::eR8G8B8A8_UNORM;
			result.imageType = renderer::TextureType::e2D;
			result.initialLayout = renderer::ImageLayout::eUndefined;
			result.mipLevels = 1u;
			result.samples = renderer::SampleCountFlag::e1;
			result.sharingMode = renderer::SharingMode::eExclusive;
			result.tiling = renderer::ImageTiling::eOptimal;
			result.usage = renderer::ImageUsageFlag::eSampled | renderer::ImageUsageFlag::eTransferDst;
			return result;
		}

		renderer::BufferImageCopyArray doInitialiseCopies()
		{
			renderer::BufferImageCopyArray result( 6u );
			renderer::BufferImageCopy copyRegion{};
			copyRegion.imageExtent.width = 16u;
			copyRegion.imageExtent.height = 16u;
			copyRegion.imageExtent.depth = 1u;
			copyRegion.imageSubresource.aspectMask = renderer::ImageAspectFlag::eColour;
			copyRegion.imageSubresource.layerCount = 1u;
			copyRegion.imageSubresource.mipLevel = 0u;

			for ( uint32_t i = 0; i < 6u; ++i )
			{
				copyRegion.imageSubresource.baseArrayLayer = i;
				result[i] = copyRegion;
			}

			return result;
		}
	}

	//************************************************************************************************

	ColourBackground::ColourBackground( Engine & engine
		, Scene & scene )
		: SceneBackground{ engine, scene, BackgroundType::eColour }
		, m_copyRegions{ doInitialiseCopies() }
		, m_matrixUbo{ engine }
		, m_modelMatrixUbo{ engine }
	{
		m_texture = std::make_shared< TextureLayout >( *engine.getRenderSystem()
			, doGetImageCreate()
			, renderer::MemoryPropertyFlag::eDeviceLocal );
		m_size.set( Dim, Dim );
	}

	ColourBackground::~ColourBackground()
	{
	}

	void ColourBackground::accept( BackgroundVisitor & visitor )const
	{
		visitor.visit( *this );
	}

	bool ColourBackground::doInitialise( renderer::RenderPass const & renderPass )
	{
		auto & value = m_scene.getBackgroundColour();
		m_colour = HdrRgbColour::fromComponents( value.red(), value.green(), value.blue() );
		auto & device = *getEngine()->getRenderSystem()->getCurrentDevice();
		m_stagingBuffer = renderer::makeBuffer< Point4f >( device
			, 256u
			, renderer::BufferTarget::eTransferSrc
			, renderer::MemoryPropertyFlag::eHostVisible | renderer::MemoryPropertyFlag::eHostCoherent );

		m_texture->getImage( 0u ).initialiseSource();
		m_texture->getImage( 1u ).initialiseSource();
		m_texture->getImage( 2u ).initialiseSource();
		m_texture->getImage( 3u ).initialiseSource();
		m_texture->getImage( 4u ).initialiseSource();
		m_texture->getImage( 5u ).initialiseSource();
		auto result = m_texture->initialise();
		m_colour.reset();

		m_cmdCopy = device.getGraphicsCommandPool().createCommandBuffer( true );
		m_cmdCopy->begin();
		m_cmdCopy->memoryBarrier( renderer::PipelineStageFlag::eTopOfPipe
			, renderer::PipelineStageFlag::eTransfer
			, m_texture->getDefaultView().makeTransferDestination( renderer::ImageLayout::eUndefined, 0u ) );
		m_cmdCopy->copyToImage( m_copyRegions
			, m_stagingBuffer->getBuffer()
			, m_texture->getTexture() );
		m_cmdCopy->memoryBarrier( renderer::PipelineStageFlag::eTransfer
			, renderer::PipelineStageFlag::eFragmentShader
			, m_texture->getDefaultView().makeShaderInputResource( renderer::ImageLayout::eTransferDstOptimal, renderer::AccessFlag::eTransferWrite ) );
		m_cmdCopy->end();

		auto & program = doInitialiseShader();

		if ( result )
		{
			result = doInitialiseVertexBuffer()
				&& doInitialisePipeline( program, renderPass );
		}

		doUpdateColour();
		return result;
	}

	void ColourBackground::doCleanup()
	{
		m_stagingBuffer.reset();
		m_cmdCopy.reset();
		m_matrixUbo.cleanup();
		m_modelMatrixUbo.cleanup();
	}

	void ColourBackground::doUpdate( Camera const & camera )
	{
		auto & value = m_scene.getBackgroundColour();
		m_colour = HdrRgbColour::fromComponents( value.red(), value.green(), value.blue() );

		if ( m_colour.isDirty() )
		{
			doUpdateColour();
		}

		auto node = camera.getParent();
		matrix::setTranslate( m_mtxModel, node->getDerivedPosition() );
		m_viewport.setPerspective( 45.0_degrees
			, camera.getViewport().getRatio()
			, 0.1f
			, 2.0f );
		m_viewport.update();
		m_matrixUbo.update( camera.getView()
			//, m_viewport.getProjection() );
			, camera.getViewport().getProjection() );
		m_modelMatrixUbo.update( m_mtxModel );
	}

	void ColourBackground::doUpdateColour()
	{
		if ( auto * buffer = m_stagingBuffer->lock( 0u, 256u, renderer::MemoryMapFlag::eWrite ) )
		{
			Point4f colour{ m_colour->red().value(), m_colour->green().value(), m_colour->blue().value(), 1.0f };

			for ( auto i = 0u; i < 256; ++i )
			{
				*buffer = colour;
				++buffer;
			}

			m_stagingBuffer->flush( 0u, 256u );
			m_stagingBuffer->unlock();
			auto & device = *getEngine()->getRenderSystem()->getCurrentDevice();
			device.getGraphicsQueue().submit( *m_cmdCopy, nullptr );
			device.getGraphicsQueue().waitIdle();
			m_colour.reset();
		}
	}

	renderer::ShaderStageStateArray ColourBackground::doInitialiseShader()
	{
		auto & renderSystem = *getEngine()->getRenderSystem();

		glsl::Shader vtx;
		{
			GlslWriter writer{ renderSystem.createGlslWriter() };

			// Inputs
			auto position = writer.declAttribute< Vec3 >( cuT( "position" ), 0u );
			UBO_MATRIX( writer, 0, 0 );
			UBO_MODEL_MATRIX( writer, 1, 0 );

			// Outputs
			auto vtx_texture = writer.declOutput< Vec3 >( cuT( "vtx_texture" ), 0u );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			std::function< void() > main = [&]()
			{
				gl_Position = writer.paren( c3d_projection * c3d_curView * c3d_mtxModel * vec4( position, 1.0 ) ).SWIZZLE_XYWW;
				vtx_texture = position;
			};

			writer.implementFunction< void >( cuT( "main" ), main );
			vtx = writer.finalise();
		}

		glsl::Shader pxl;
		{
			GlslWriter writer{ renderSystem.createGlslWriter() };

			// Inputs
			UBO_HDR_CONFIG( writer, 2, 0 );
			auto vtx_texture = writer.declInput< Vec3 >( cuT( "vtx_texture" ), 0u );
			auto skybox = writer.declSampler< SamplerCube >( cuT( "skybox" ), 3u, 0u );
			glsl::Utils utils{ writer };

			if ( !m_hdr )
			{
				utils.declareRemoveGamma();
			}

			// Outputs
			auto pxl_FragColor = writer.declFragData< Vec4 >( cuT( "pxl_FragColor" ), 0u );

			std::function< void() > main = [&]()
			{
				auto colour = writer.declLocale( cuT( "colour" )
					, texture( skybox, vec3( vtx_texture.x(), -vtx_texture.y(), vtx_texture.z() ) ) );

				if ( !m_hdr )
				{
					pxl_FragColor = vec4( utils.removeGamma( c3d_gamma, colour.xyz() ), colour.w() );
				}
				else
				{
					pxl_FragColor = vec4( colour.xyz(), colour.w() );
				}
			};

			writer.implementFunction< void >( cuT( "main" ), main );
			pxl = writer.finalise();
		}

		renderer::ShaderStageStateArray result;
		result.push_back( { renderSystem.getCurrentDevice()->createShaderModule( renderer::ShaderStageFlag::eVertex ) } );
		result.push_back( { renderSystem.getCurrentDevice()->createShaderModule( renderer::ShaderStageFlag::eFragment ) } );
		result[0].module->loadShader( vtx.getSource() );
		result[1].module->loadShader( pxl.getSource() );
		return result;
	}

	bool ColourBackground::doInitialiseVertexBuffer()
	{
		m_vertexBuffer = renderer::makeVertexBuffer< NonTexturedCube >( *getEngine()->getRenderSystem()->getCurrentDevice()
			, 1u
			, 0u
			, renderer::MemoryPropertyFlag::eHostVisible );

		if ( auto * buffer = m_vertexBuffer->lock( 0u, 1u, renderer::MemoryMapFlag::eWrite ) )
		{
			NonTexturedCube bufferVertex
			{
				{
					{ Point3f{ -1, +1, -1 }, Point3f{ +1, -1, -1 }, Point3f{ -1, -1, -1 }, Point3f{ +1, -1, -1 }, Point3f{ -1, +1, -1 }, Point3f{ +1, +1, -1 } },
					{ Point3f{ -1, -1, +1 }, Point3f{ -1, +1, -1 }, Point3f{ -1, -1, -1 }, Point3f{ -1, +1, -1 }, Point3f{ -1, -1, +1 }, Point3f{ -1, +1, +1 } },
					{ Point3f{ +1, -1, -1 }, Point3f{ +1, +1, +1 }, Point3f{ +1, -1, +1 }, Point3f{ +1, +1, +1 }, Point3f{ +1, -1, -1 }, Point3f{ +1, +1, -1 } },
					{ Point3f{ -1, -1, +1 }, Point3f{ +1, +1, +1 }, Point3f{ -1, +1, +1 }, Point3f{ +1, +1, +1 }, Point3f{ -1, -1, +1 }, Point3f{ +1, -1, +1 } },
					{ Point3f{ -1, +1, -1 }, Point3f{ +1, +1, +1 }, Point3f{ +1, +1, -1 }, Point3f{ +1, +1, +1 }, Point3f{ -1, +1, -1 }, Point3f{ -1, +1, +1 } },
					{ Point3f{ -1, -1, -1 }, Point3f{ +1, -1, -1 }, Point3f{ -1, -1, +1 }, Point3f{ +1, -1, -1 }, Point3f{ +1, -1, +1 }, Point3f{ -1, -1, +1 } },
				}
			};
			std::memcpy( buffer, bufferVertex.faces, m_vertexBuffer->getSize() );
			m_vertexBuffer->flush( 0u, 1u );
			m_vertexBuffer->unlock();
		}

		return true;
	}

	bool ColourBackground::doInitialisePipeline( renderer::ShaderStageStateArray & program
		, renderer::RenderPass const & renderPass )
	{
		auto & device = *getEngine()->getRenderSystem()->getCurrentDevice();
		renderer::DescriptorSetLayoutBindingArray setLayoutBindings
		{
			{ 0u, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eVertex },
			{ 1u, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eVertex },
			{ 2u, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eFragment },
			{ 3u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment },
		};
		m_descriptorLayout = device.createDescriptorSetLayout( std::move( setLayoutBindings ) );
		m_pipelineLayout = device.createPipelineLayout( *m_descriptorLayout );

		m_matrixUbo.initialise();
		m_modelMatrixUbo.initialise();

		m_descriptorPool = m_descriptorLayout->createPool( 1u );
		m_descriptorSet = m_descriptorPool->createDescriptorSet( 0u );
		m_descriptorSet->createBinding( m_descriptorLayout->getBinding( 0u )
			, m_matrixUbo.getUbo() );
		m_descriptorSet->createBinding( m_descriptorLayout->getBinding( 1u )
			, m_modelMatrixUbo.getUbo() );
		m_descriptorSet->createBinding( m_descriptorLayout->getBinding( 2u )
			, m_configUbo.getUbo() );
		m_descriptorSet->createBinding( m_descriptorLayout->getBinding( 3u )
			, m_texture->getDefaultView()
			, m_sampler.lock()->getSampler() );
		m_descriptorSet->update();
		renderer::VertexInputState vertexInput;
		vertexInput.vertexBindingDescriptions.push_back( { 0u, sizeof( Point3f ), renderer::VertexInputRate::eVertex } );
		vertexInput.vertexAttributeDescriptions.push_back( { 0u, 0u, renderer::Format::eR32G32B32_SFLOAT, 0u } );

		m_pipeline = m_pipelineLayout->createPipeline( renderer::GraphicsPipelineCreateInfo
		{
			program,
			renderPass,
			vertexInput,
			renderer::InputAssemblyState{ renderer::PrimitiveTopology::eTriangleList },
			renderer::RasterisationState{ 0u, false, false, renderer::PolygonMode::eFill, renderer::CullModeFlag::eFront },
			renderer::MultisampleState{},
			renderer::ColourBlendState::createDefault(),
			{ renderer::DynamicState::eViewport, renderer::DynamicState::eScissor },
			renderer::DepthStencilState{ 0u, true, false, renderer::CompareOp::eLessEqual }
		} );
		return true;
	}

	//************************************************************************************************
}
