#include "Scene/Background/Background.hpp"

#include "Engine.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"
#include "Texture/Sampler.hpp"

#include <Buffer/StagingBuffer.hpp>
#include <Command/CommandBufferInheritanceInfo.hpp>
#include <Core/Device.hpp>
#include <Shader/GlslToSpv.hpp>

#include <ShaderWriter/Source.hpp>
#include "Shader/Shaders/GlslUtils.hpp"

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

	bool SceneBackground::initialise( ashes::RenderPass const & renderPass
		, HdrConfigUbo const & hdrConfigUbo )
	{
		m_semaphore = getCurrentDevice( *this ).createSemaphore();
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
			sampler->setMinFilter( ashes::Filter::eLinear );
			sampler->setMagFilter( ashes::Filter::eLinear );
			sampler->setWrapS( ashes::WrapMode::eClampToEdge );
			sampler->setWrapT( ashes::WrapMode::eClampToEdge );
			sampler->setWrapR( ashes::WrapMode::eClampToEdge );
			sampler->setMinLod( 0.0f );
			sampler->setMaxLod( float( m_texture->getMipmapCount() - 1u ) );

			if ( m_texture->getMipmapCount() > 1u )
			{
				sampler->setMipFilter( ashes::MipmapMode::eLinear );
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

	bool SceneBackground::prepareFrame( ashes::CommandBuffer & commandBuffer
		, castor::Size const & size
		, ashes::RenderPass const & renderPass
		, ashes::FrameBuffer const & frameBuffer )
	{
		REQUIRE( m_initialised );
		ashes::ClearColorValue colour;
		ashes::DepthStencilClearValue depth{ 1.0, 0 };
		commandBuffer.begin();
		m_timer->beginPass( commandBuffer );
		commandBuffer.beginRenderPass( renderPass
			, frameBuffer
			, { depth, colour }
			, ashes::SubpassContents::eInline );
		doPrepareFrame( commandBuffer
			, size
			, renderPass
			, *m_descriptorSet );
		commandBuffer.endRenderPass();
		m_timer->endPass( commandBuffer );
		commandBuffer.end();

		return true;
	}

	bool SceneBackground::prepareFrame( ashes::CommandBuffer & commandBuffer
		, castor::Size const & size
		, ashes::RenderPass const & renderPass )
	{
		REQUIRE( m_initialised );
		return prepareFrame( commandBuffer
			, size
			, renderPass
			, *m_descriptorSet );
	}

	bool SceneBackground::prepareFrame( ashes::CommandBuffer & commandBuffer
		, castor::Size const & size
		, ashes::RenderPass const & renderPass
		, ashes::DescriptorSet const & descriptorSet )const
	{
		REQUIRE( m_initialised );
		commandBuffer.begin( ashes::CommandBufferUsageFlag::eRenderPassContinue
			, ashes::CommandBufferInheritanceInfo
			{
				&renderPass,
				0u,
				nullptr,
				false,
				0u,
				0u
			} );
		doPrepareFrame( commandBuffer
			, size
			, renderPass
			, descriptorSet );
		commandBuffer.end();

		return true;
	}

	void SceneBackground::initialiseDescriptorSet( MatrixUbo const & matrixUbo
		, ModelMatrixUbo const & modelMatrixUbo
		, HdrConfigUbo const & hdrConfigUbo
		, ashes::DescriptorSet & descriptorSet )const
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

	RenderPassTimerBlock SceneBackground::start()
	{
		return m_timer->start();
	}

	void SceneBackground::notifyPassRender()
	{
		m_timer->notifyPassRender();
	}

	void SceneBackground::notifyChanged()
	{
		if ( m_initialised )
		{
			m_initialised = false;
			onChanged( *this );
		}
	}

	void SceneBackground::doPrepareFrame( ashes::CommandBuffer & commandBuffer
		, castor::Size const & size
		, ashes::RenderPass const & renderPass
		, ashes::DescriptorSet const & descriptorSet )const
	{
		REQUIRE( m_initialised );
		commandBuffer.bindPipeline( *m_pipeline );
		commandBuffer.setViewport( { size.getWidth(), size.getHeight(), 0, 0 } );
		commandBuffer.setScissor( { 0, 0, size.getWidth(), size.getHeight() } );
		commandBuffer.bindDescriptorSet( descriptorSet, *m_pipelineLayout );
		commandBuffer.bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
		commandBuffer.bindIndexBuffer( m_indexBuffer->getBuffer(), 0u, ashes::IndexType::eUInt16 );
		commandBuffer.drawIndexed( m_indexBuffer->getCount() );
	}

	ashes::ShaderStageStateArray SceneBackground::doInitialiseShader()
	{
		using namespace sdw;
		auto & renderSystem = *getEngine()->getRenderSystem();

		ShaderModule vtx{ ashes::ShaderStageFlag::eVertex, "Background" };
		{
			VertexWriter writer;

			// Inputs
			auto position = writer.declInput< Vec3 >( cuT( "position" ), 0u );
			UBO_MATRIX( writer, 0, 0 );
			UBO_MODEL_MATRIX( writer, 1, 0 );

			// Outputs
			auto vtx_texture = writer.declOutput< Vec3 >( cuT( "vtx_texture" ), 0u );
			auto out = writer.getOut();

			std::function< void() > main = [&]()
			{
				out.gl_out.gl_Position = writer.paren( c3d_projection * c3d_curView * c3d_curMtxModel * vec4( position, 1.0_f ) ).xyww();
				vtx_texture = position;
			};

			writer.implementFunction< sdw::Void >( cuT( "main" ), main );
			vtx.shader = std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		ShaderModule pxl{ ashes::ShaderStageFlag::eFragment, "Background" };
		{
			FragmentWriter writer;

			// Inputs
			UBO_HDR_CONFIG( writer, 2, 0 );
			auto vtx_texture = writer.declInput< Vec3 >( cuT( "vtx_texture" ), 0u );
			auto c3d_mapSkybox = writer.declSampledImage< FImgCubeRgba32 >( cuT( "c3d_mapSkybox" ), 3u, 0u );
			shader::Utils utils{ writer };

			if ( !m_hdr )
			{
				utils.declareRemoveGamma();
			}

			// Outputs
			auto pxl_FragColor = writer.declOutput< Vec4 >( cuT( "pxl_FragColor" ), 0u );

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

			writer.implementFunction< sdw::Void >( cuT( "main" ), main );
			pxl.shader = std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		auto & device = getCurrentDevice( renderSystem );
		ashes::ShaderStageStateArray result;
		result.push_back( { device.createShaderModule( ashes::ShaderStageFlag::eVertex ) } );
		result.push_back( { device.createShaderModule( ashes::ShaderStageFlag::eFragment ) } );
		result[0].module->loadShader( renderSystem.compileShader( vtx ) );
		result[1].module->loadShader( renderSystem.compileShader( pxl ) );
		return result;
	}

	void SceneBackground::doInitialiseDescriptorLayout()
	{
		auto & device = getCurrentDevice( *this );
		ashes::DescriptorSetLayoutBindingArray setLayoutBindings
		{
			{ 0u, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eVertex },			// Matrix UBO
			{ 1u, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eVertex },			// Model Matrix UBO
			{ 2u, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eFragment },			// HDR Config UBO
			{ 3u, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment },	// Skybox Map
		};
		m_descriptorLayout = device.createDescriptorSetLayout( std::move( setLayoutBindings ) );
	}

	bool SceneBackground::doInitialiseVertexBuffer()
	{
		using castor::Point3f;
		auto & renderSystem = *getEngine()->getRenderSystem();
		auto & device = getCurrentDevice( renderSystem );
		ashes::StagingBuffer stagingBuffer{ device, 0u, sizeof( Cube ) };
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
		m_vertexBuffer = ashes::makeVertexBuffer< Cube >( device
			, 1u
			, ashes::BufferTarget::eTransferDst
			, ashes::MemoryPropertyFlag::eDeviceLocal );
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
		m_indexBuffer = ashes::makeBuffer< uint16_t >( device
			, uint32_t( indexData.size() )
			, ashes::BufferTarget::eIndexBuffer | ashes::BufferTarget::eTransferDst
			, ashes::MemoryPropertyFlag::eDeviceLocal );
		stagingBuffer.uploadBufferData( *commandBuffer
			, indexData
			, *m_indexBuffer );

		return true;
	}

	bool SceneBackground::doInitialisePipeline( ashes::ShaderStageStateArray program
		, ashes::RenderPass const & renderPass
		, HdrConfigUbo const & hdrConfigUbo )
	{
		auto & device = getCurrentDevice( *this );
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
		ashes::VertexInputState vertexInput;
		vertexInput.vertexBindingDescriptions.push_back( { 0u, sizeof( castor::Point3f ), ashes::VertexInputRate::eVertex } );
		vertexInput.vertexAttributeDescriptions.push_back( { 0u, 0u, ashes::Format::eR32G32B32_SFLOAT, 0u } );

		m_pipeline = m_pipelineLayout->createPipeline(
		{
			std::move( program ),
			renderPass,
			vertexInput,
			ashes::InputAssemblyState{ ashes::PrimitiveTopology::eTriangleList },
			ashes::RasterisationState{ 0u, false, false, ashes::PolygonMode::eFill, ashes::CullModeFlag::eNone },
			ashes::MultisampleState{},
			ashes::ColourBlendState::createDefault(),
			{ ashes::DynamicState::eViewport, ashes::DynamicState::eScissor },
			ashes::DepthStencilState{ 0u, false, false, ashes::CompareOp::eLessEqual }
		} );
		return true;
	}
}
