#include "Scene/Background/Background.hpp"

#include "Engine.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"
#include "Texture/Sampler.hpp"

#include <Buffer/StagingBuffer.hpp>
#include <Command/CommandBufferInheritanceInfo.hpp>
#include <Core/Device.hpp>

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

namespace castor3d
{
	SceneBackground::SceneBackground( Engine & engine
		, Scene & scene
		, BackgroundType type )
		: OwnedBy< Engine >{ engine }
		, m_scene{ scene }
		, m_type{ type }
		, m_matrixUbo{ engine }
		, m_modelMatrixUbo{ engine }
		, m_configUbo{ engine }
	{
	}

	SceneBackground::~SceneBackground()
	{
	}

	bool SceneBackground::initialise( renderer::RenderPass const & renderPass )
	{
		m_semaphore = getEngine()->getRenderSystem()->getCurrentDevice()->createSemaphore();
		m_configUbo.initialise();

		castor::String const name = cuT( "Skybox" );

		if ( getEngine()->getSamplerCache().has( name ) )
		{
			m_sampler = getEngine()->getSamplerCache().find( name );
		}
		else
		{
			auto sampler = getEngine()->getSamplerCache().add( name );
			sampler->setMinFilter( renderer::Filter::eLinear );
			sampler->setMagFilter( renderer::Filter::eLinear );
			sampler->setWrapS( renderer::WrapMode::eClampToEdge );
			sampler->setWrapT( renderer::WrapMode::eClampToEdge );
			sampler->setWrapR( renderer::WrapMode::eClampToEdge );
			m_sampler = sampler;
		}

		m_sampler.lock()->initialise();
		auto result = doInitialiseVertexBuffer()
			&& doInitialise( renderPass );

		if ( result )
		{
			doInitialisePipeline( doInitialiseShader(), renderPass );
		}

		if ( result
			&& m_scene.getMaterialsType() != MaterialType::eLegacy
			&& m_texture->getLayersCount() == 6u )
		{
			m_ibl = std::make_unique< IblTextures >( m_scene, m_texture->getTexture() );
			m_ibl->update();
		}

		if ( result )
		{
			m_timer = std::make_shared< RenderPassTimer >( *getEngine(), cuT( "Background" ), cuT( "Background" ) );
		}

		return result;
	}

	void SceneBackground::cleanup()
	{
		m_timer.reset();

		doCleanup();

		m_matrixUbo.cleanup();
		m_modelMatrixUbo.cleanup();
		m_configUbo.cleanup();
		m_pipeline.reset();
		m_indexBuffer.reset();
		m_vertexBuffer.reset();
		m_descriptorSet.reset();
		m_pipelineLayout.reset();
		m_descriptorPool.reset();
		m_descriptorLayout.reset();
		m_semaphore.reset();

		if ( m_texture )
		{
			m_texture->cleanup();
			m_texture.reset();
		}

		m_ibl.reset();
	}

	void SceneBackground::update( Camera const & camera )
	{
		if ( !m_hdr )
		{
			m_configUbo.update( m_scene.getHdrConfig() );
		}

		static castor::Point3r const Scale{ 1, -1, 1 };
		static castor::Matrix3x3r const Identity{ 1.0f };
		auto node = camera.getParent();
		castor::matrix::setTranslate( m_mtxModel, node->getDerivedPosition() );
		castor::matrix::scale( m_mtxModel, Scale );
		m_modelMatrixUbo.update( m_mtxModel, Identity );
		doUpdate( camera );
	}

	bool SceneBackground::prepareFrame( renderer::CommandBuffer & commandBuffer
		, castor::Size const & size
		, renderer::RenderPass const & renderPass
		, renderer::FrameBuffer const & frameBuffer )
	{
		renderer::ClearColorValue colour;
		renderer::DepthStencilClearValue depth;
		auto result = commandBuffer.begin();

		if ( result )
		{
			commandBuffer.resetQueryPool( m_timer->getQuery()
				, 0u
				, 2u );
			commandBuffer.beginRenderPass( renderPass
				, frameBuffer
				, { depth, colour }
				, renderer::SubpassContents::eInline );
			commandBuffer.writeTimestamp( renderer::PipelineStageFlag::eTopOfPipe
				, m_timer->getQuery()
				, 0u );
			commandBuffer.bindPipeline( *m_pipeline );
			commandBuffer.setViewport( { size.getWidth(), size.getHeight(), 0, 0 } );
			commandBuffer.setScissor( { 0, 0, size.getWidth(), size.getHeight() } );
			commandBuffer.bindDescriptorSet( *m_descriptorSet, *m_pipelineLayout );
			commandBuffer.bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
			commandBuffer.bindIndexBuffer( m_indexBuffer->getBuffer(), 0u, renderer::IndexType::eUInt16 );
			commandBuffer.drawIndexed( m_indexBuffer->getCount() );
			commandBuffer.writeTimestamp( renderer::PipelineStageFlag::eBottomOfPipe
				, m_timer->getQuery()
				, 1u );
			commandBuffer.endRenderPass();
			result = commandBuffer.end();
		}

		return result;
	}

	bool SceneBackground::prepareFrame( renderer::CommandBuffer & commandBuffer
		, castor::Size const & size
		, renderer::RenderPass const & renderPass )
	{
		auto result = commandBuffer.begin(  renderer::CommandBufferUsageFlag::eRenderPassContinue
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
			commandBuffer.setViewport( { size.getWidth(), size.getHeight(), 0, 0 } );
			commandBuffer.setScissor( { 0, 0, size.getWidth(), size.getHeight() } );
			commandBuffer.bindDescriptorSet( *m_descriptorSet, *m_pipelineLayout );
			commandBuffer.bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
			commandBuffer.bindIndexBuffer( m_indexBuffer->getBuffer(), 0u, renderer::IndexType::eUInt16 );
			commandBuffer.drawIndexed( m_indexBuffer->getCount() );
			result = commandBuffer.end();
		}

		return result;
	}

	renderer::ShaderStageStateArray SceneBackground::doInitialiseShader()
	{
		using namespace glsl;
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
			auto c3d_mapSkybox = writer.declSampler< SamplerCube >( cuT( "c3d_mapSkybox" ), 3u, 0u );
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
					, texture( c3d_mapSkybox, vtx_texture ) );

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

	void SceneBackground::doInitialiseDescriptorLayout()
	{
		auto & device = *getEngine()->getRenderSystem()->getCurrentDevice();
		renderer::DescriptorSetLayoutBindingArray setLayoutBindings
		{
			{ 0u, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eVertex },			// Matrix UBO
			{ 1u, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eVertex },			// Model Matrix UBO
			{ 2u, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eFragment },			// HDR Config UBO
			{ 3u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment },	// Skybox Map
		};
		m_descriptorLayout = device.createDescriptorSetLayout( std::move( setLayoutBindings ) );
	}

	void SceneBackground::doInitialiseDescriptorSet()
	{
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
	}

	bool SceneBackground::doInitialiseVertexBuffer()
	{
		using castor::Point3f;
		auto & renderSystem = *getEngine()->getRenderSystem();
		auto & device = *renderSystem.getCurrentDevice();
		renderer::StagingBuffer stagingBuffer{ device, 0u, sizeof( Cube ) };
		auto commandBuffer = device.getGraphicsCommandPool().createCommandBuffer();

		// Vertex Buffer
		std::vector< Cube > vertexData
		{
			Cube
			{
				{
					// Front
					{ Point3f{ -1.0, -1.0, +1.0 }, Point3f{ -1.0, +1.0, +1.0 }, Point3f{ +1.0, -1.0, +1.0 }, Point3f{ +1.0, +1.0, +1.0 } },
					// Top
					{ Point3f{ -1.0, +1.0, +1.0 }, Point3f{ -1.0, +1.0, -1.0 }, Point3f{ +1.0, +1.0, +1.0 }, Point3f{ +1.0, +1.0, -1.0 } },
					// Back
					{ Point3f{ -1.0, +1.0, -1.0 }, Point3f{ -1.0, -1.0, -1.0 }, Point3f{ +1.0, +1.0, -1.0 }, Point3f{ +1.0, -1.0, -1.0 } },
					// Bottom
					{ Point3f{ -1.0, -1.0, -1.0 }, Point3f{ -1.0, -1.0, +1.0 }, Point3f{ +1.0, -1.0, -1.0 }, Point3f{ +1.0, -1.0, +1.0 } },
					// Right
					{ Point3f{ +1.0, -1.0, +1.0 }, Point3f{ +1.0, +1.0, +1.0 }, Point3f{ +1.0, -1.0, -1.0 }, Point3f{ +1.0, +1.0, -1.0 } },
					// Left
					{ Point3f{ -1.0, -1.0, -1.0 }, Point3f{ -1.0, +1.0, -1.0 }, Point3f{ -1.0, -1.0, +1.0 }, Point3f{ -1.0, +1.0, +1.0 } },
				}
			}
		};
		m_vertexBuffer = renderer::makeVertexBuffer< Cube >( device
			, 1u
			, renderer::BufferTarget::eTransferDst
			, renderer::MemoryPropertyFlag::eDeviceLocal );
		stagingBuffer.uploadVertexData( *commandBuffer
			, vertexData
			, *m_vertexBuffer );

		// Index Buffer
		std::vector< uint16_t > indexData
		{
			// Front
			0, 1, 2, 2, 1, 3,
			// Top
			4, 5, 6, 6, 5, 7,
			// Back
			8, 9, 10, 10, 9, 11,
			// Bottom
			12, 13, 14, 14, 13, 15,
			// Right
			16, 17, 18, 18, 17, 19,
			// Left
			20, 21, 22, 22, 21, 23,
		};
		m_indexBuffer = renderer::makeBuffer< uint16_t >( device
			, uint32_t( indexData.size() )
			, renderer::BufferTarget::eIndexBuffer | renderer::BufferTarget::eTransferDst
			, renderer::MemoryPropertyFlag::eDeviceLocal );
		stagingBuffer.uploadBufferData( *commandBuffer
			, indexData
			, *m_indexBuffer );

		return true;
	}

	bool SceneBackground::doInitialisePipeline( renderer::ShaderStageStateArray program
		, renderer::RenderPass const & renderPass )
	{
		auto & device = *getEngine()->getRenderSystem()->getCurrentDevice();
		doInitialiseDescriptorLayout();
		m_pipelineLayout = device.createPipelineLayout( *m_descriptorLayout );

		m_matrixUbo.initialise();
		m_modelMatrixUbo.initialise();

		m_descriptorPool = m_descriptorLayout->createPool( 1u );
		doInitialiseDescriptorSet();
		renderer::VertexInputState vertexInput;
		vertexInput.vertexBindingDescriptions.push_back( { 0u, sizeof( castor::Point3f ), renderer::VertexInputRate::eVertex } );
		vertexInput.vertexAttributeDescriptions.push_back( { 0u, 0u, renderer::Format::eR32G32B32_SFLOAT, 0u } );

		m_pipeline = m_pipelineLayout->createPipeline(
		{
			std::move( program ),
			renderPass,
			vertexInput,
			renderer::InputAssemblyState{ renderer::PrimitiveTopology::eTriangleList },
			renderer::RasterisationState{ 0u, false, false, renderer::PolygonMode::eFill, renderer::CullModeFlag::eBack },
			renderer::MultisampleState{},
			renderer::ColourBlendState::createDefault(),
			{ renderer::DynamicState::eViewport, renderer::DynamicState::eScissor },
			renderer::DepthStencilState{ 0u, true, false, renderer::CompareOp::eLessEqual }
		} );
		return true;
	}
}
