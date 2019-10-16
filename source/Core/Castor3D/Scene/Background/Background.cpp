#include "Castor3D/Scene/Background/Background.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Texture/Sampler.hpp"

#include <ashespp/Buffer/StagingBuffer.hpp>
#include <ashespp/Command/CommandBufferInheritanceInfo.hpp>
#include <ashespp/Core/Device.hpp>
#include "Castor3D/Shader/GlslToSpv.hpp"

#include <ShaderWriter/Source.hpp>
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

namespace castor3d
{
	namespace
	{
		static uint32_t constexpr MtxUboIdx = 0u;
		static uint32_t constexpr MdlMtxUboIdx = 1u;
		static uint32_t constexpr HdrCfgUboIdx = 2u;
		static uint32_t constexpr SkyBoxImgIdx = 3u;
	}

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
		m_semaphore = getCurrentRenderDevice( *this )->createSemaphore();
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
			sampler->setMinFilter( VK_FILTER_LINEAR );
			sampler->setMagFilter( VK_FILTER_LINEAR );
			sampler->setWrapS( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
			sampler->setWrapT( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
			sampler->setWrapR( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
			sampler->setMinLod( 0.0f );
			sampler->setMaxLod( float( m_texture->getMipmapCount() - 1u ) );

			if ( m_texture->getMipmapCount() > 1u )
			{
				sampler->setMipFilter( VK_SAMPLER_MIPMAP_MODE_LINEAR );
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
			&& m_scene.getMaterialsType() != MaterialType::ePhong
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
		CU_Require( m_initialised );
		VkClearColorValue colour{};
		VkClearDepthStencilValue depth{ 1.0, 0 };
		commandBuffer.begin();
		m_timer->beginPass( commandBuffer );
		commandBuffer.beginRenderPass( renderPass
			, frameBuffer
			, { ashes::makeClearValue( depth ), ashes::makeClearValue( colour ) }
			, VK_SUBPASS_CONTENTS_INLINE );
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
		CU_Require( m_initialised );
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
		CU_Require( m_initialised );
		commandBuffer.begin( VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT
			, makeVkType< VkCommandBufferInheritanceInfo >( renderPass,
				0u,
				VkFramebuffer( VK_NULL_HANDLE ),
				VkBool32( VK_FALSE ),
				0u,
				0u ) );
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
		descriptorSet.createSizedBinding( m_descriptorLayout->getBinding( 0u )
			, matrixUbo.getUbo() );
		descriptorSet.createSizedBinding( m_descriptorLayout->getBinding( 1u )
			, modelMatrixUbo.getUbo() );
		descriptorSet.createSizedBinding( m_descriptorLayout->getBinding( 2u )
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
		CU_Require( m_initialised );
		commandBuffer.bindPipeline( *m_pipeline );
		commandBuffer.setViewport( makeViewport( size ) );
		commandBuffer.setScissor( makeScissor( size ) );
		commandBuffer.bindDescriptorSet( descriptorSet, *m_pipelineLayout );
		commandBuffer.bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
		commandBuffer.bindIndexBuffer( m_indexBuffer->getBuffer(), 0u, VK_INDEX_TYPE_UINT16 );
		commandBuffer.drawIndexed( uint32_t( m_indexBuffer->getCount() ) );
	}

	ashes::PipelineShaderStageCreateInfoArray SceneBackground::doInitialiseShader()
	{
		using namespace sdw;
		auto & renderSystem = *getEngine()->getRenderSystem();

		ShaderModule vtx{ VK_SHADER_STAGE_VERTEX_BIT, "Background" };
		{
			VertexWriter writer;

			// Inputs
			auto position = writer.declInput< Vec3 >( cuT( "position" ), 0u );
			UBO_MATRIX( writer, MtxUboIdx, 0 );
			UBO_MODEL_MATRIX( writer, MdlMtxUboIdx, 0 );

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

		ShaderModule pxl{ VK_SHADER_STAGE_FRAGMENT_BIT, "Background" };
		{
			FragmentWriter writer;

			// Inputs
			UBO_HDR_CONFIG( writer, HdrCfgUboIdx, 0 );
			auto vtx_texture = writer.declInput< Vec3 >( cuT( "vtx_texture" ), 0u );
			auto c3d_mapSkybox = writer.declSampledImage< FImgCubeRgba32 >( cuT( "c3d_mapSkybox" ), SkyBoxImgIdx, 0u );
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

		auto & device = getCurrentRenderDevice( renderSystem );
		return ashes::PipelineShaderStageCreateInfoArray
		{
			makeShaderState( device, vtx ),
			makeShaderState( device, pxl ),
		};
	}

	void SceneBackground::doInitialiseDescriptorLayout()
	{
		auto & device = getCurrentRenderDevice( *this );
		ashes::VkDescriptorSetLayoutBindingArray setLayoutBindings
		{
			makeDescriptorSetLayoutBinding( MtxUboIdx
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_VERTEX_BIT ),
			makeDescriptorSetLayoutBinding( MdlMtxUboIdx
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_VERTEX_BIT ),
			makeDescriptorSetLayoutBinding( HdrCfgUboIdx
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
			makeDescriptorSetLayoutBinding( SkyBoxImgIdx
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
		};
		m_descriptorLayout = device->createDescriptorSetLayout( std::move( setLayoutBindings ) );
	}

	bool SceneBackground::doInitialiseVertexBuffer()
	{
		using castor::Point3f;
		auto & renderSystem = *getEngine()->getRenderSystem();
		auto & device = getCurrentRenderDevice( renderSystem );
		ashes::StagingBuffer stagingBuffer{ *device.device, 0u, sizeof( Cube ) };

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
		m_vertexBuffer = makeVertexBuffer< Cube >( device
			, 1u
			, VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, "Background");
		stagingBuffer.uploadVertexData( *device.graphicsQueue
			, *device.graphicsCommandPool
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
		m_indexBuffer = makeBuffer< uint16_t >( device
			, uint32_t( indexData.size() )
			, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, "BackgroundIndexBuffer" );
		stagingBuffer.uploadBufferData( *device.graphicsQueue
			, *device.graphicsCommandPool
			, indexData
			, *m_indexBuffer );

		return true;
	}

	bool SceneBackground::doInitialisePipeline( ashes::PipelineShaderStageCreateInfoArray program
		, ashes::RenderPass const & renderPass
		, HdrConfigUbo const & hdrConfigUbo )
	{
		auto & device = getCurrentRenderDevice( *this );
		m_pipelineLayout.reset();
		doInitialiseDescriptorLayout();
		m_pipelineLayout = device->createPipelineLayout( *m_descriptorLayout );

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
		ashes::PipelineVertexInputStateCreateInfo vertexInput
		{
			0u,
			{ 1u, { 0u, sizeof( castor::Point3f ), VK_VERTEX_INPUT_RATE_VERTEX } },
			{ 1u, { 0u, 0u, VK_FORMAT_R32G32B32_SFLOAT, 0u } },
		};

		m_pipeline = device->createPipeline( ashes::GraphicsPipelineCreateInfo
			{
				0u,
				std::move( program ),
				std::move( vertexInput ),
				ashes::PipelineInputAssemblyStateCreateInfo{ 0u, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST },
				std::nullopt,
				ashes::PipelineViewportStateCreateInfo{},
				ashes::PipelineRasterizationStateCreateInfo{ 0u, false, false, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE },
				ashes::PipelineMultisampleStateCreateInfo{},
				ashes::PipelineDepthStencilStateCreateInfo{ 0u, false, false, VK_COMPARE_OP_LESS_OR_EQUAL },
				ashes::PipelineColorBlendStateCreateInfo{},
				ashes::PipelineDynamicStateCreateInfo{ 0u, { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR } },
				*m_pipelineLayout,
				renderPass,
			} );
		return true;
	}
}
