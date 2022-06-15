#include "Castor3D/Scene/Background/Background.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/HdrConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelDataUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/Buffer/StagingBuffer.hpp>
#include <ashespp/Core/Device.hpp>

#include <RenderGraph/FramePassGroup.hpp>

#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	//*********************************************************************************************

	class BackgroundPass
		: public BackgroundPassBase
	{
	public:
		BackgroundPass( crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, SceneBackground & background
			, VkExtent2D const & size
			, bool usesDepth )
			: BackgroundPassBase{ pass
				, context
				, graph
				, device
				, background
				, size
				, usesDepth }
		{
		}

	private:
		void doInitialiseVertexBuffer()override
		{
			if ( m_vertexBuffer )
			{
				return;
			}

			using castor::Point3f;
			auto data = m_device.graphicsData();
			ashes::StagingBuffer stagingBuffer{ *m_device.device, 0u, sizeof( Vertex ) * 24u };

			// Vertex Buffer
			std::vector< Vertex > vertexData
			{
				// Front
				{ Point3f{ -1.0, -1.0, +1.0 } }, { Point3f{ -1.0, +1.0, +1.0 } }, { Point3f{ +1.0, -1.0, +1.0 } }, { Point3f{ +1.0, +1.0, +1.0 } },
				// Top
				{ Point3f{ -1.0, +1.0, +1.0 } }, { Point3f{ -1.0, +1.0, -1.0 } }, { Point3f{ +1.0, +1.0, +1.0 } }, { Point3f{ +1.0, +1.0, -1.0 } },
				// Back
				{ Point3f{ -1.0, +1.0, -1.0 } }, { Point3f{ -1.0, -1.0, -1.0 } }, { Point3f{ +1.0, +1.0, -1.0 } }, { Point3f{ +1.0, -1.0, -1.0 } },
				// Bottom
				{ Point3f{ -1.0, -1.0, -1.0 } }, { Point3f{ -1.0, -1.0, +1.0 } }, { Point3f{ +1.0, -1.0, -1.0 } }, { Point3f{ +1.0, -1.0, +1.0 } },
				// Right
				{ Point3f{ +1.0, -1.0, +1.0 } }, { Point3f{ +1.0, +1.0, +1.0 } }, { Point3f{ +1.0, -1.0, -1.0 } }, { Point3f{ +1.0, +1.0, -1.0 } },
				// Left
				{ Point3f{ -1.0, -1.0, -1.0 } }, { Point3f{ -1.0, +1.0, -1.0 } }, { Point3f{ -1.0, -1.0, +1.0 } }, { Point3f{ -1.0, +1.0, +1.0 } },
			};
			m_vertexBuffer = makeVertexBuffer< Vertex >( m_device
				, 24u
				, VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, "Background");
			stagingBuffer.uploadVertexData( *data->queue
				, *data->commandPool
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
			m_indexBuffer = makeBuffer< uint16_t >( m_device
				, uint32_t( indexData.size() )
				, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, "BackgroundIndexBuffer" );
			stagingBuffer.uploadBufferData( *data->queue
				, *data->commandPool
				, indexData
				, *m_indexBuffer );
		}

		ashes::PipelineShaderStageCreateInfoArray doInitialiseShader()override
		{
			using namespace sdw;

			ShaderModule vtx{ VK_SHADER_STAGE_VERTEX_BIT, "Background" };
			{
				VertexWriter writer;

				// Inputs
				auto position = writer.declInput< Vec3 >( "position", 0u );
				C3D_Matrix( writer, SceneBackground::MtxUboIdx, 0u );
				C3D_ModelData( writer, SceneBackground::MdlMtxUboIdx, 0u );

				// Outputs
				auto vtx_texture = writer.declOutput< Vec3 >( "vtx_texture", 0u );

				writer.implementMainT< VoidT, VoidT >( [&]( VertexIn in
					, VertexOut out )
					{
						out.vtx.position = c3d_matrixData.worldToCurProj( c3d_modelData.modelToWorld( vec4( position, 1.0_f ) ) ).xyww();
						vtx_texture = position;
					} );

				vtx.shader = std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
			}

			ShaderModule pxl{ VK_SHADER_STAGE_FRAGMENT_BIT, "Background" };
			{
				FragmentWriter writer;

				// Inputs
				C3D_Scene( writer, SceneBackground::SceneUboIdx, 0u );
				C3D_HdrConfig( writer, SceneBackground::HdrCfgUboIdx, 0u );
				auto vtx_texture = writer.declInput< Vec3 >( "vtx_texture", 0u );
				auto c3d_mapSkybox = writer.declCombinedImg< FImgCubeRgba32 >( "c3d_mapSkybox", SceneBackground::SkyBoxImgIdx, 0u );
				shader::Utils utils{ writer, *m_device.renderSystem.getEngine() };

				// Outputs
				auto pxl_FragColor = writer.declOutput< Vec4 >( "pxl_FragColor", 0u );

				writer.implementMainT< VoidT, VoidT >( [&]( FragmentIn in
					, FragmentOut out )
					{
						IF( writer, c3d_sceneData.fogType == UInt( uint32_t( FogType::eDisabled ) ) )
						{
							auto colour = writer.declLocale( "colour"
								, c3d_mapSkybox.sample( vtx_texture ) );

							if ( !m_background->isHdr() && !m_background->isSRGB() )
							{
								pxl_FragColor = vec4( c3d_hdrConfigData.removeGamma( colour.xyz() ), colour.w() );
							}
							else
							{
								pxl_FragColor = vec4( colour.xyz(), colour.w() );
							}
						}
						ELSE
						{
							pxl_FragColor = vec4( c3d_sceneData.getBackgroundColour( c3d_hdrConfigData ).xyz(), 1.0_f );
						}
						FI;
					} );
				pxl.shader = std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
			}

			return ashes::PipelineShaderStageCreateInfoArray
			{
				makeShaderState( m_device, vtx ),
				makeShaderState( m_device, pxl ),
			};
		}

		void doFillDescriptorBindings()override
		{
			VkShaderStageFlags shaderStage = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
			m_descriptorBindings.clear();
			m_descriptorWrites.clear();

			for ( auto & uniform : m_pass.buffers )
			{
				m_descriptorBindings.push_back( { uniform.binding
					, uniform.getDescriptorType()
					, 1u
					, shaderStage
					, nullptr } );
				m_descriptorWrites.push_back( uniform.getBufferWrite() );
			}

			m_descriptorBindings.push_back( { SceneBackground::SkyBoxImgIdx
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, 1u
				, shaderStage
				, nullptr } );
			m_descriptorWrites.push_back( crg::WriteDescriptorSet{ SceneBackground::SkyBoxImgIdx
				, 0u
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VkDescriptorImageInfo{ m_background->getSampler().getSampler()
				, m_background->getTexture().getDefaultView().getSampledView()
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } } );
		}

		void doCreatePipeline()override
		{
			m_pipeline = m_device->createPipeline( ashes::GraphicsPipelineCreateInfo{ 0u
				, doInitialiseShader()
				, ashes::PipelineVertexInputStateCreateInfo{ 0u
					, { 1u, { 0u, sizeof( castor::Point3f ), VK_VERTEX_INPUT_RATE_VERTEX } }
					, { 1u, { 0u, 0u, VK_FORMAT_R32G32B32_SFLOAT, 0u } } }
				, ashes::PipelineInputAssemblyStateCreateInfo{ 0u, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE }
				, std::nullopt
				, doCreateViewportState()
				, ashes::PipelineRasterizationStateCreateInfo{ 0u, VK_FALSE, VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE }
				, ashes::PipelineMultisampleStateCreateInfo{}
				, ( m_usesDepth
					? std::make_optional( ashes::PipelineDepthStencilStateCreateInfo{ 0u, VK_TRUE, VK_FALSE, VK_COMPARE_OP_LESS_OR_EQUAL } )
					: std::nullopt )
				, ashes::PipelineColorBlendStateCreateInfo{ 0u, VK_FALSE, VK_LOGIC_OP_COPY, doGetBlendAttachs() }
				, std::nullopt
				, *m_pipelineLayout
				, getRenderPass() } );
		}
	};

	//*********************************************************************************************

	SceneBackground::SceneBackground( Engine & engine
		, Scene & scene
		, castor::String const & name
		, castor::String type )
		: castor::OwnedBy< Engine >{ engine }
		, castor::Named{ scene.getName() + name }
		, m_scene{ scene }
		, m_type{ std::move( type ) }
	{
	}

	SceneBackground::~SceneBackground()
	{
		m_textureId.destroy();
	}

	bool SceneBackground::initialise( RenderDevice const & device )
	{
		if ( !m_initialised )
		{
			m_initialised = doInitialise( device );
			castor::String const name = cuT( "Skybox" );
			auto sampler = getEngine()->tryFindSampler( name );

			if ( !sampler.lock() )
			{
				auto created = getEngine()->createSampler( name, *getEngine() );
				created->setMinFilter( VK_FILTER_LINEAR );
				created->setMagFilter( VK_FILTER_LINEAR );
				created->setWrapS( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				created->setWrapT( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				created->setWrapR( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				created->setMinLod( 0.0f );
				created->setMaxLod( float( m_texture->getMipmapCount() - 1u ) );
				sampler = getEngine()->addSampler( name, created, false );

				if ( m_texture->getMipmapCount() > 1u )
				{
					sampler.lock()->setMipFilter( VK_SAMPLER_MIPMAP_MODE_LINEAR );
				}
			}

			sampler.lock()->initialise( device );
			m_sampler = sampler;

			if ( m_initialised
				&& m_scene.getEngine()->getPassFactory().hasIBL( m_scene.getPassesType() )
				&& m_texture->getLayersCount() == 6u )
			{
				m_ibl = std::make_unique< IblTextures >( m_scene
					, device
					, m_textureId
					, device.renderSystem.getPrefilteredBrdfTexture()
					, sampler );
				m_ibl->update( *device.graphicsData() );
			}

			onChanged( *this );
		}

		return m_initialised;
	}

	void SceneBackground::cleanup()
	{
		doCleanup();

		if ( m_texture )
		{
			m_texture->cleanup();
			m_texture.reset();
		}

		m_ibl.reset();
	}

	void SceneBackground::update( CpuUpdater & updater )const
	{
		if ( m_initialised )
		{
			static castor::Point3f const Scale{ 1, -1, 1 };
			static castor::Matrix3x3f const Identity{ 1.0f };

			auto & camera = *updater.camera;
			auto node = camera.getParent();

			castor::matrix::setTranslate( updater.bgMtxModl, node->getDerivedPosition() );
			castor::matrix::scale( updater.bgMtxModl, Scale );
			doCpuUpdate( updater );
		}
	}

	void SceneBackground::update( GpuUpdater & updater )const
	{
		if ( m_initialised )
		{
			doGpuUpdate( updater );
		}
	}

	void SceneBackground::notifyChanged()
	{
		if ( m_initialised )
		{
			m_initialised = false;
			onChanged( *this );
		}
	}

	crg::FramePass & SceneBackground::createBackgroundPass( crg::FramePassGroup & graph
		, RenderDevice const & device
		, ProgressBar * progress
		, VkExtent2D const & size
		, bool usesDepth
		, MatrixUbo const & matrixUbo
		, SceneUbo const & sceneUbo
		, BackgroundPassBase *& backgroundPass )
	{
		auto & result = graph.createPass( "Background"
			, [this, &backgroundPass, &device, progress, size, usesDepth]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBar( progress, "Initialising background pass" );
				auto res = std::make_unique< BackgroundPass >( framePass
					, context
					, runnableGraph
					, device
					, *this
					, size
					, usesDepth );
				backgroundPass = res.get();
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				return res;
			} );
		result.addSampledView( m_textureId.sampledViewId
			, SceneBackground::SkyBoxImgIdx
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		return result;
	}

	//*********************************************************************************************
}
