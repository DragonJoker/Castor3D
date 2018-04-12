#include "Scene/Background/Image.hpp"

#include "Engine.hpp"

#include "Render/RenderPipeline.hpp"
#include "Event/Frame/FrameListener.hpp"
#include "Event/Frame/FunctorEvent.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Background/Visitor.hpp"
#include "EnvironmentMap/EnvironmentMap.hpp"
#include "RenderToTexture/EquirectangularToCube.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <Command/CommandBufferInheritanceInfo.hpp>
#include <Miscellaneous/PushConstantRange.hpp>
#include <RenderPass/FrameBuffer.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>
#include <Shader/ShaderModule.hpp>

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

using namespace castor;
using namespace glsl;

namespace castor3d
{
	ImageBackground::ImageBackground( Engine & engine
		, Scene & scene )
		: SceneBackground{ engine, scene, BackgroundType::eImage }
		, m_matrixUbo{ engine }
		, m_modelMatrixUbo{ engine }
		, m_sizePushConstant{ renderer::ShaderStageFlag::eFragment, { { 0u, 0u, renderer::ConstantFormat::eVec2f } } }
	{
	}

	ImageBackground::~ImageBackground()
	{
	}

	bool ImageBackground::loadImage( Path const & folder, Path const & relative )
	{
		bool result = false;

		try
		{
			renderer::ImageCreateInfo image{};
			image.arrayLayers = 1u;
			image.flags = 0u;
			image.imageType = renderer::TextureType::e2D;
			image.initialLayout = renderer::ImageLayout::eUndefined;
			image.mipLevels = 1u;
			image.samples = renderer::SampleCountFlag::e1;
			image.sharingMode = renderer::SharingMode::eExclusive;
			image.tiling = renderer::ImageTiling::eOptimal;
			image.usage = renderer::ImageUsageFlag::eSampled;
			auto texture = std::make_shared< TextureLayout >( *getEngine()->getRenderSystem()
				, image
				, renderer::MemoryPropertyFlag::eDeviceLocal );
			texture->setSource( folder, relative );
			m_texture = texture;
			m_size.set( m_texture->getWidth(), m_texture->getHeight() );
			m_scene.getListener().postEvent( makeFunctorEvent( EventType::ePreRender, [this]()
			{
				m_texture->initialise();
				m_texture->generateMipmaps();
			} ) );
			result = true;
		}
		catch ( castor::Exception & p_exc )
		{
			Logger::logError( p_exc.what() );
		}

		return result;
	}

	bool ImageBackground::prepareFrame( renderer::CommandBuffer & commandBuffer
		, renderer::RenderPass const & renderPass
		, renderer::FrameBuffer const & frameBuffer )
	{
		renderer::ClearColorValue colour;
		renderer::DepthStencilClearValue depth;
		*m_sizePushConstant.getData() = Point2f{ m_size.getWidth()
			, m_size.getHeight() };

		auto result = commandBuffer.begin();

		if ( result )
		{
			commandBuffer.beginRenderPass( renderPass
				, frameBuffer
				, { depth, colour }
			, renderer::SubpassContents::eInline );
			commandBuffer.bindPipeline( *m_pipeline );
			commandBuffer.setViewport( { m_size.getWidth(), m_size.getHeight(), 0, 0 } );
			commandBuffer.setScissor( { 0, 0, m_size.getWidth(), m_size.getHeight() } );
			commandBuffer.bindDescriptorSet( *m_descriptorSet, *m_pipelineLayout );
			commandBuffer.pushConstants( *m_pipelineLayout, m_sizePushConstant );
			commandBuffer.bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
			commandBuffer.draw( 36u );
			commandBuffer.endRenderPass();
			result = commandBuffer.end();
		}

		return result;
	}

	bool ImageBackground::prepareFrame( renderer::CommandBuffer & commandBuffer
		, renderer::RenderPass const & renderPass )
	{
		*m_sizePushConstant.getData() = Point2f{ m_size.getWidth()
			, m_size.getHeight() };

		auto result = commandBuffer.begin( renderer::CommandBufferUsageFlag::eRenderPassContinue
			, renderer::CommandBufferInheritanceInfo
			{
				&renderPass,
				0u,
				nullptr,
				false,
				0u,
				0u
			} );

		if ( result )
		{
			commandBuffer.bindPipeline( *m_pipeline );
			commandBuffer.setViewport( { m_size.getWidth(), m_size.getHeight(), 0, 0 } );
			commandBuffer.setScissor( { 0, 0, m_size.getWidth(), m_size.getHeight() } );
			commandBuffer.bindDescriptorSet( *m_descriptorSet, *m_pipelineLayout );
			commandBuffer.pushConstants( *m_pipelineLayout, m_sizePushConstant );
			commandBuffer.bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
			commandBuffer.draw( 36u );
			result = commandBuffer.end();
		}

		return result;
	}

	void ImageBackground::accept( BackgroundVisitor & visitor )const
	{
		visitor.visit( *this );
	}

	bool ImageBackground::doInitialise( renderer::RenderPass const & renderPass )
	{
		auto & program = doInitialiseShader();
		doInitialiseVertexBuffer();
		*m_sizePushConstant.getData() = Point2f{ m_size.getWidth()
			, m_size.getHeight() };
		doInitialisePipeline( program, renderPass );
		return true;
	}

	void ImageBackground::doCleanup()
	{
		m_matrixUbo.cleanup();
		m_modelMatrixUbo.cleanup();
	}

	void ImageBackground::doUpdate( Camera const & camera )
	{
		static Matrix3x3r const Identity{ 1.0f };
		auto const & viewport = camera.getViewport();
		auto node = camera.getParent();
		matrix::setTranslate( m_mtxModel, node->getDerivedPosition() );
		m_matrixUbo.update( camera.getView()
			, viewport.getProjection() );
		m_modelMatrixUbo.update( m_mtxModel, Identity );
	}

	renderer::ShaderStageStateArray ImageBackground::doInitialiseShader()
	{
		auto & renderSystem = *getEngine()->getRenderSystem();
		glsl::Shader vtx;
		{
			using namespace glsl;
			GlslWriter writer{ renderSystem.createGlslWriter() };

			// Inputs
			auto position = writer.declAttribute< Vec3 >( cuT( "position" ), 0u );
			UBO_MATRIX( writer, 0, 0 );
			UBO_MODEL_MATRIX( writer, 1, 0 );

			// Outputs
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				gl_Position = writer.paren( c3d_projection * c3d_curView * c3d_mtxModel * vec4( position, 1.0 ) ).SWIZZLE_XYWW;
			} );

			vtx = writer.finalise();
		}

		glsl::Shader pxl;
		{
			using namespace glsl;
			GlslWriter writer{ renderSystem.createGlslWriter() };

			// Inputs
			auto c3d_mapDiffuse = writer.declSampler< Sampler2D >( cuT( "c3d_mapDiffuse" ), 2u, 0u );
			auto c3d_size = writer.declUniform< Vec2 >( cuT( "c3d_size" ), 0u );
			auto gl_FragCoord = writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );

			// Outputs
			auto pxl_FragColor = writer.declFragData< Vec4 >( cuT( "pxl_FragColor" ), 0u );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				pxl_FragColor = texture( c3d_mapDiffuse, gl_FragCoord.xy() / c3d_size );
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

	bool ImageBackground::doInitialiseVertexBuffer()
	{
		auto & renderSystem = *getEngine()->getRenderSystem();
		auto & device = *renderSystem.getCurrentDevice();
		m_vertexBuffer = renderer::makeVertexBuffer< NonTexturedCube >( device
			, 1u
			, renderer::BufferTarget::eTransferDst
			, renderer::MemoryPropertyFlag::eHostVisible );

		if ( auto buffer = m_vertexBuffer->lock( 0u
			, 1u
			, renderer::MemoryMapFlag::eWrite | renderer::MemoryMapFlag::eInvalidateRange ) )
		{
			*buffer = NonTexturedCube
			{
				{
					{ Point3f{ -1.0, +1.0, -1.0 }, Point3f{ +1.0, -1.0, -1.0 }, Point3f{ -1.0, -1.0, -1.0 }, Point3f{ +1.0, -1.0, -1.0 }, Point3f{ -1.0, +1.0, -1.0 }, Point3f{ +1.0, +1.0, -1.0 } },
					{ Point3f{ -1.0, -1.0, +1.0 }, Point3f{ -1.0, +1.0, -1.0 }, Point3f{ -1.0, -1.0, -1.0 }, Point3f{ -1.0, +1.0, -1.0 }, Point3f{ -1.0, -1.0, +1.0 }, Point3f{ -1.0, +1.0, +1.0 } },
					{ Point3f{ +1.0, -1.0, -1.0 }, Point3f{ +1.0, +1.0, +1.0 }, Point3f{ +1.0, -1.0, +1.0 }, Point3f{ +1.0, +1.0, +1.0 }, Point3f{ +1.0, -1.0, -1.0 }, Point3f{ +1.0, +1.0, -1.0 } },
					{ Point3f{ -1.0, -1.0, +1.0 }, Point3f{ +1.0, +1.0, +1.0 }, Point3f{ -1.0, +1.0, +1.0 }, Point3f{ +1.0, +1.0, +1.0 }, Point3f{ -1.0, -1.0, +1.0 }, Point3f{ +1.0, -1.0, +1.0 } },
					{ Point3f{ -1.0, +1.0, -1.0 }, Point3f{ +1.0, +1.0, +1.0 }, Point3f{ +1.0, +1.0, -1.0 }, Point3f{ +1.0, +1.0, +1.0 }, Point3f{ -1.0, +1.0, -1.0 }, Point3f{ -1.0, +1.0, +1.0 } },
					{ Point3f{ -1.0, -1.0, -1.0 }, Point3f{ +1.0, -1.0, -1.0 }, Point3f{ -1.0, -1.0, +1.0 }, Point3f{ +1.0, -1.0, -1.0 }, Point3f{ +1.0, -1.0, +1.0 }, Point3f{ -1.0, -1.0, +1.0 } },
				}
			};
			m_vertexBuffer->flush( 0u, 1u );
			m_vertexBuffer->unlock();
		}

		return true;
	}

	bool ImageBackground::doInitialisePipeline( renderer::ShaderStageStateArray & program
		, renderer::RenderPass const & renderPass )
	{
		renderer::DepthStencilState dsState
		{
			0u,
			true,
			false,
			renderer::CompareOp::eLessEqual
		};

		renderer::RasterisationState rsState
		{
			0u,
			false,
			false,
			renderer::PolygonMode::eFill,
			renderer::CullModeFlag::eFront
		};

		auto vertexLayout = renderer::makeLayout< NonTexturedCube >( 0u );
		vertexLayout->createAttribute( 0u
			, renderer::Format::eR32G32B32_SFLOAT
			, offsetof( NonTexturedCube::Quad::Vertex, position ) );

		auto blState = renderer::ColourBlendState::createDefault();
		auto & renderSystem = *getEngine()->getRenderSystem();
		auto & device = *renderSystem.getCurrentDevice();

		renderer::DescriptorSetLayoutBindingArray bindings
		{
			{ 0u, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eVertex },
			{ 1u, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eVertex },
			{ 2u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment },
		};
		m_descriptorLayout = device.createDescriptorSetLayout( std::move( bindings ) );
		renderer::PushConstantRange pushRange{ renderer::ShaderStageFlag::eFragment, m_sizePushConstant.getOffset(), m_sizePushConstant.getSize() };
		m_pipelineLayout = device.createPipelineLayout( *m_descriptorLayout, pushRange );

		m_pipeline = m_pipelineLayout->createPipeline(
		{
			program,
			renderPass,
			renderer::VertexInputState::create( *vertexLayout ),
			renderer::InputAssemblyState{ renderer::PrimitiveTopology::eTriangleList },
			rsState,
			renderer::MultisampleState{},
			blState,
			{ renderer::DynamicState::eViewport, renderer::DynamicState::eScissor },
			std::move( dsState )
		} );

		m_descriptorPool = m_descriptorLayout->createPool( 1u );
		m_descriptorSet = m_descriptorPool->createDescriptorSet( 0u );
		m_descriptorSet->createBinding( m_descriptorLayout->getBinding( 0u )
			, m_matrixUbo.getUbo()
			, 0u
			, 1u );
		m_descriptorSet->createBinding( m_descriptorLayout->getBinding( 1u )
			, m_modelMatrixUbo.getUbo()
			, 0u
			, 1u );
		m_descriptorSet->createBinding( m_descriptorLayout->getBinding( 2u )
			, m_texture->getDefaultView()
			, m_sampler.lock()->getSampler() );
		m_descriptorSet->update();
		return true;
	}
}
