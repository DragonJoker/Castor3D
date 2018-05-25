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
	{
	}

	SceneBackground::~SceneBackground()
	{
	}

	bool SceneBackground::initialise( renderer::RenderPass const & renderPass
		, HdrConfigUbo const & hdrConfigUbo )
	{
		m_semaphore = getEngine()->getRenderSystem()->getCurrentDevice()->createSemaphore();
		m_initialised = doInitialiseVertexBuffer()
			&& doInitialise( renderPass );
		castor::String const name = cuT( "Skybox" );
		SamplerSPtr sampler;

		if ( getEngine()->getSamplerCache().has( name ) )
		{
			sampler = getEngine()->getSamplerCache().find( name );
		}
		else
		{
			sampler = getEngine()->getSamplerCache().add( name );
			sampler->setMinFilter( renderer::Filter::eLinear );
			sampler->setMagFilter( renderer::Filter::eLinear );
			sampler->setWrapS( renderer::WrapMode::eClampToEdge );
			sampler->setWrapT( renderer::WrapMode::eClampToEdge );
			sampler->setWrapR( renderer::WrapMode::eClampToEdge );
			sampler->setMinLod( 0.0f );
			sampler->setMaxLod( float( m_texture->getMipmapCount() - 1u ) );

			if ( m_texture->getMipmapCount() > 1u )
			{
				sampler->setMipFilter( renderer::MipmapMode::eLinear );
			}
		}

		sampler->initialise();
		m_sampler = sampler;

		if ( m_initialised )
		{
			doInitialisePipeline( doInitialiseShader()
				, renderPass
				, hdrConfigUbo );
		}

		if ( m_initialised
			&& m_scene.getMaterialsType() != MaterialType::eLegacy
			&& m_texture->getLayersCount() == 6u )
		{
			m_ibl = std::make_unique< IblTextures >( m_scene
				, m_texture->getTexture()
				, sampler );
			m_ibl->update();
		}

		if ( m_initialised )
		{
			m_timer = std::make_shared< RenderPassTimer >( *getEngine(), cuT( "Background" ), cuT( "Background" ) );
		}

		return m_initialised;
	}

	void SceneBackground::cleanup()
	{
		m_timer.reset();

		doCleanup();

		m_matrixUbo.cleanup();
		m_modelMatrixUbo.cleanup();
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
		if ( m_initialised )
		{
			static castor::Point3r const Scale{ 1, -1, 1 };
			static castor::Matrix3x3r const Identity{ 1.0f };
			auto node = camera.getParent();
			castor::matrix::setTranslate( m_mtxModel, node->getDerivedPosition() );
			castor::matrix::scale( m_mtxModel, Scale );
			m_modelMatrixUbo.update( m_mtxModel, Identity );
			doUpdate( camera );
		}
	}

	bool SceneBackground::prepareFrame( renderer::CommandBuffer & commandBuffer
		, castor::Size const & size
		, renderer::RenderPass const & renderPass
		, renderer::FrameBuffer const & frameBuffer )
	{
		REQUIRE( m_initialised );
		renderer::ClearColorValue colour;
		renderer::DepthStencilClearValue depth;
		auto result = commandBuffer.begin();

		if ( result )
		{
			m_timer->beginPass( commandBuffer );
			commandBuffer.beginRenderPass( renderPass
				, frameBuffer
				, { depth, colour }
				, renderer::SubpassContents::eInline );
			doPrepareFrame( commandBuffer
				, size
				, renderPass
				, *m_descriptorSet );
			commandBuffer.endRenderPass();
			m_timer->endPass( commandBuffer );
			result = commandBuffer.end();
		}

		return result;
	}

	bool SceneBackground::prepareFrame( renderer::CommandBuffer & commandBuffer
		, castor::Size const & size
		, renderer::RenderPass const & renderPass )
	{
		REQUIRE( m_initialised );
		return prepareFrame( commandBuffer
			, size
			, renderPass
			, *m_descriptorSet );
	}

	bool SceneBackground::prepareFrame( renderer::CommandBuffer & commandBuffer
		, castor::Size const & size
		, renderer::RenderPass const & renderPass
		, renderer::DescriptorSet const & descriptorSet )const
	{
		REQUIRE( m_initialised );
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
			doPrepareFrame( commandBuffer
				, size
				, renderPass
				, descriptorSet );
			result = commandBuffer.end();
		}

		return result;
	}

	void SceneBackground::initialiseDescriptorSet( MatrixUbo const & matrixUbo
		, ModelMatrixUbo const & modelMatrixUbo
		, HdrConfigUbo const & hdrConfigUbo
		, renderer::DescriptorSet & descriptorSet )const
	{
		descriptorSet.createBinding( m_descriptorLayout->getBinding( 0u )
			, matrixUbo.getUbo() );
		descriptorSet.createBinding( m_descriptorLayout->getBinding( 1u )
			, modelMatrixUbo.getUbo() );
		descriptorSet.createBinding( m_descriptorLayout->getBinding( 2u )
			, hdrConfigUbo.getUbo() );
		descriptorSet.createBinding( m_descriptorLayout->getBinding( 3u )
			, m_texture->getDefaultView()
			, m_sampler.lock()->getSampler() );
	}

	void SceneBackground::start()
	{
		m_timer->start();
	}

	void SceneBackground::notifyPassRender()
	{
		m_timer->notifyPassRender();
	}

	void SceneBackground::stop()
	{
		m_timer->stop();
	}

	void SceneBackground::notifyChanged()
	{
		if ( m_initialised )
		{
			m_initialised = false;
			onChanged( *this );
		}
	}

	void SceneBackground::doPrepareFrame( renderer::CommandBuffer & commandBuffer
		, castor::Size const & size
		, renderer::RenderPass const & renderPass
		, renderer::DescriptorSet const & descriptorSet )const
	{
		REQUIRE( m_initialised );
		commandBuffer.bindPipeline( *m_pipeline );
		commandBuffer.setViewport( { size.getWidth(), size.getHeight(), 0, 0 } );
		commandBuffer.setScissor( { 0, 0, size.getWidth(), size.getHeight() } );
		commandBuffer.bindDescriptorSet( descriptorSet, *m_pipelineLayout );
		commandBuffer.bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
		commandBuffer.bindIndexBuffer( m_indexBuffer->getBuffer(), 0u, renderer::IndexType::eUInt16 );
		commandBuffer.drawIndexed( m_indexBuffer->getCount() );
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
			auto out = gl_PerVertex{ writer };

			std::function< void() > main = [&]()
			{
				out.gl_Position() = writer.paren( c3d_projection * c3d_curView * c3d_mtxModel * vec4( position, 1.0 ) ).xyww();
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
		, renderer::RenderPass const & renderPass
		, HdrConfigUbo const & hdrConfigUbo )
	{
		auto & device = *getEngine()->getRenderSystem()->getCurrentDevice();
		m_pipelineLayout.reset();
		doInitialiseDescriptorLayout();
		m_pipelineLayout = device.createPipelineLayout( *m_descriptorLayout );

		m_matrixUbo.initialise();
		m_modelMatrixUbo.initialise();

		m_descriptorSet.reset();
		m_descriptorPool = m_descriptorLayout->createPool( 1u );
		m_descriptorSet = m_descriptorPool->createDescriptorSet( 0u );
		initialiseDescriptorSet( m_matrixUbo
			, m_modelMatrixUbo
			, hdrConfigUbo
			, *m_descriptorSet );
		m_descriptorSet->update();
		renderer::VertexInputState vertexInput;
		vertexInput.vertexBindingDescriptions.push_back( { 0u, sizeof( castor::Point3f ), renderer::VertexInputRate::eVertex } );
		vertexInput.vertexAttributeDescriptions.push_back( { 0u, 0u, renderer::Format::eR32G32B32_SFLOAT, 0u } );

		m_pipeline = m_pipelineLayout->createPipeline(
		{
			std::move( program ),
			renderPass,
			vertexInput,
			renderer::InputAssemblyState{ renderer::PrimitiveTopology::eTriangleList },
			renderer::RasterisationState{ 0u, false, false, renderer::PolygonMode::eFill, renderer::CullModeFlag::eNone },
			renderer::MultisampleState{},
			renderer::ColourBlendState::createDefault(),
			{ renderer::DynamicState::eViewport, renderer::DynamicState::eScissor },
			renderer::DepthStencilState{ 0u, true, false, renderer::CompareOp::eLessEqual }
		} );
		return true;
	}
}
