#include "Castor3D/Scene/Background/Background.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <ashespp/Buffer/StagingBuffer.hpp>
#include <ashespp/Command/CommandBufferInheritanceInfo.hpp>
#include <ashespp/Core/Device.hpp>

#include <ShaderWriter/Source.hpp>

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
		m_uboDescriptorSet.reset();
		m_texDescriptorSet.reset();
		m_pipelineLayout.reset();
		m_uboDescriptorPool.reset();
		m_texDescriptorPool.reset();
		m_uboDescriptorLayout.reset();
		m_texDescriptorLayout.reset();
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
			static castor::Point3f const Scale{ 1, -1, 1 };
			static castor::Matrix3x3f const Identity{ 1.0f };
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
		commandBuffer.begin();
		commandBuffer.beginDebugBlock(
			{
				"Background Render",
				{
					opaqueBlackClearColor.color.float32[0],
					opaqueBlackClearColor.color.float32[1],
					opaqueBlackClearColor.color.float32[2],
					opaqueBlackClearColor.color.float32[3],
				},
			} );
		m_timer->beginPass( commandBuffer );
		commandBuffer.beginRenderPass( renderPass
			, frameBuffer
			, { defaultClearDepthStencil, opaqueBlackClearColor }
			, VK_SUBPASS_CONTENTS_INLINE );
		doPrepareFrame( commandBuffer
			, size
			, renderPass
			, *m_uboDescriptorSet
			, *m_texDescriptorSet );
		commandBuffer.endRenderPass();
		m_timer->endPass( commandBuffer );
		commandBuffer.endDebugBlock();
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
			, *m_uboDescriptorSet
			, *m_texDescriptorSet );
	}

	bool SceneBackground::prepareFrame( ashes::CommandBuffer & commandBuffer
		, castor::Size const & size
		, ashes::RenderPass const & renderPass
		, ashes::DescriptorSet const & uboDescriptorSet
		, ashes::DescriptorSet const & texDescriptorSet )const
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
			, uboDescriptorSet
			, texDescriptorSet );
		commandBuffer.end();

		return true;
	}

	void SceneBackground::initialiseDescriptorSets( MatrixUbo & matrixUbo
		, ModelMatrixUbo const & modelMatrixUbo
		, HdrConfigUbo const & hdrConfigUbo
		, ashes::DescriptorSet & uboDescriptorSet
		, ashes::DescriptorSet & texDescriptorSet )const
	{
		uboDescriptorSet.createSizedBinding( m_uboDescriptorLayout->getBinding( MtxUboIdx )
			, *matrixUbo.getUbo().buffer
			, matrixUbo.getUbo().offset );
		uboDescriptorSet.createSizedBinding( m_uboDescriptorLayout->getBinding( MdlMtxUboIdx )
			, *modelMatrixUbo.getUbo().buffer
			, modelMatrixUbo.getUbo().offset );
		uboDescriptorSet.createSizedBinding( m_uboDescriptorLayout->getBinding( HdrCfgUboIdx )
			, *hdrConfigUbo.getUbo().buffer
			, hdrConfigUbo.getUbo().offset );
		texDescriptorSet.createBinding( m_texDescriptorLayout->getBinding( SkyBoxImgIdx )
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
		, ashes::DescriptorSet const & uboDescriptorSet
		, ashes::DescriptorSet const & texDescriptorSet )const
	{
		CU_Require( m_initialised );
		commandBuffer.bindPipeline( *m_pipeline );
		commandBuffer.setViewport( makeViewport( size ) );
		commandBuffer.setScissor( makeScissor( size ) );
		commandBuffer.bindDescriptorSets( { uboDescriptorSet, texDescriptorSet }, *m_pipelineLayout );
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
			UBO_MATRIX( writer, MtxUboIdx, UboSetIdx );
			UBO_MODEL_MATRIX( writer, MdlMtxUboIdx, UboSetIdx );

			// Outputs
			auto vtx_texture = writer.declOutput< Vec3 >( cuT( "vtx_texture" ), 0u );
			auto out = writer.getOut();

			std::function< void() > main = [&]()
			{
				out.vtx.position = ( c3d_projection * c3d_curView * c3d_curMtxModel * vec4( position, 1.0_f ) ).xyww();
				vtx_texture = position;
			};

			writer.implementFunction< sdw::Void >( cuT( "main" ), main );
			vtx.shader = std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		ShaderModule pxl{ VK_SHADER_STAGE_FRAGMENT_BIT, "Background" };
		{
			FragmentWriter writer;

			// Inputs
			UBO_HDR_CONFIG( writer, HdrCfgUboIdx, 0 );
			auto vtx_texture = writer.declInput< Vec3 >( cuT( "vtx_texture" ), 0u );
			auto c3d_mapSkybox = writer.declSampledImage< FImgCubeRgba32 >( cuT( "c3d_mapSkybox" ), SkyBoxImgIdx, TexSetIdx );
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
			pxl.shader = std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		auto & device = getCurrentRenderDevice( renderSystem );
		return ashes::PipelineShaderStageCreateInfoArray
		{
			makeShaderState( device, vtx ),
			makeShaderState( device, pxl ),
		};
	}

	void SceneBackground::doInitialiseDescriptorLayouts()
	{
		auto & device = getCurrentRenderDevice( *this );
		ashes::VkDescriptorSetLayoutBindingArray uboBindings
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
		};
		m_uboDescriptorLayout = device->createDescriptorSetLayout( std::move( uboBindings ) );
		ashes::VkDescriptorSetLayoutBindingArray texBindings
		{
			makeDescriptorSetLayoutBinding( SkyBoxImgIdx
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
		};
		m_texDescriptorLayout = device->createDescriptorSetLayout( std::move( texBindings ) );
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
		doInitialiseDescriptorLayouts();
		m_pipelineLayout = device->createPipelineLayout( { *m_uboDescriptorLayout, *m_texDescriptorLayout } );

		m_matrixUbo.initialise();
		m_modelMatrixUbo.initialise();

		m_uboDescriptorSet.reset();
		m_uboDescriptorPool = m_uboDescriptorLayout->createPool( 1u );
		m_uboDescriptorSet = m_uboDescriptorPool->createDescriptorSet( 0u );
		m_texDescriptorSet.reset();
		m_texDescriptorPool = m_texDescriptorLayout->createPool( 1u );
		m_texDescriptorSet = m_texDescriptorPool->createDescriptorSet( 0u );
		initialiseDescriptorSets( m_matrixUbo
			, m_modelMatrixUbo
			, hdrConfigUbo
			, *m_uboDescriptorSet
			, *m_texDescriptorSet );
		m_uboDescriptorSet->update();
		m_texDescriptorSet->update();
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
				ashes::nullopt,
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
