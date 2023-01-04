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
#include "Castor3D/Scene/Background/Shaders/GlslIblBackground.hpp"
#include "Castor3D/Scene/Background/Shaders/GlslNoIblBackground.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/HdrConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelDataUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <CastorUtils/Design/DataHolder.hpp>
#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/Buffer/StagingBuffer.hpp>
#include <ashespp/Core/Device.hpp>

#include <RenderGraph/FramePassGroup.hpp>

#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace back
	{
		enum Bindings : uint32_t
		{
			eMatrix = 0u,
			eModel = 1u,
			eHdrConfig = 2u,
			eScene = 3u,
			eSkybox = 4u,
		};

		struct Shaders
		{
			castor3d::ShaderModule vertexShader;
			castor3d::ShaderModule pixelShader;
			ashes::PipelineShaderStageCreateInfoArray stages;
		};

		class BackgroundPass
			: public castor::DataHolderT< ashes::VertexBufferPtr< castor::Point3f > >
			, public castor::DataHolderT< ashes::BufferPtr< uint16_t > >
			, public castor::DataHolderT< Shaders >
			, public BackgroundPassBase
			, public crg::RenderMesh
		{
		public:
			BackgroundPass( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph
				, RenderDevice const & device
				, SceneBackground & background
				, VkExtent2D const & size
				, crg::ImageViewId const * depth )
				: BackgroundPassBase{ pass
					, context
					, graph
					, device
					, background }
				, crg::RenderMesh{ pass
					, context
					, graph
					, crg::ru::Config{ 1u, true }
					, crg::rm::Config{}
						.vertexBuffer( doCreateVertexBuffer( device ) )
						.indexBuffer( doCreateIndexBuffer( device ) )
						.depthStencilState( ( depth
							? ashes::PipelineDepthStencilStateCreateInfo{ 0u, VK_TRUE, VK_FALSE, VK_COMPARE_OP_LESS_OR_EQUAL }
							: ashes::PipelineDepthStencilStateCreateInfo{ 0u, VK_FALSE, VK_FALSE, VK_COMPARE_OP_LESS_OR_EQUAL } ) )
						.getIndexType( crg::GetIndexTypeCallback( [](){ return VK_INDEX_TYPE_UINT16; } ) )
						.getPrimitiveCount( crg::GetPrimitiveCountCallback( [](){ return 36u; } ) )
						.isEnabled( IsEnabledCallback( [this](){ return doIsEnabled(); } ) )
						.renderSize( size )
						.program( doInitialiseShader( device ) ) }
			{
			}

		private:
			void doResetPipeline()override
			{
				resetCommandBuffer();
				resetPipeline( {} );
				reRecordCurrent();
			}

			crg::IndexBuffer doCreateIndexBuffer( RenderDevice const & device )
			{
				if ( !castor::DataHolderT< ashes::BufferPtr< uint16_t > >::getData() )
				{
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
					castor::DataHolderT< ashes::BufferPtr< uint16_t > >::setData( makeBuffer< uint16_t >( device
						, uint32_t( indexData.size() )
						, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
						, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
						, "BackgroundIndexBuffer" ) );
					auto & indexBuffer = *castor::DataHolderT< ashes::BufferPtr< uint16_t > >::getData();
					auto data = m_device.graphicsData();
					ashes::StagingBuffer stagingBuffer{ *m_device.device, 0u, VkDeviceSize( sizeof( uint16_t ) * indexData.size() ) };
					stagingBuffer.uploadBufferData( *data->queue
						, *data->commandPool
						, indexData
						, indexBuffer );
				}

				auto & indexBuffer = *castor::DataHolderT< ashes::BufferPtr< uint16_t > >::getData();
				return { crg::Buffer{ indexBuffer.getBuffer(), "Index" }
					, indexBuffer.getBuffer().getStorage() };
			}

			crg::VertexBuffer doCreateVertexBuffer( RenderDevice const & device )
			{
				if ( !castor::DataHolderT< ashes::VertexBufferPtr< castor::Point3f > >::getData() )
				{
					using castor::Point3f;

					// Vertex Buffer
					std::vector< Point3f > vertexData
					{
						// Front
						Point3f{ -1.0, -1.0, +1.0 }, Point3f{ -1.0, +1.0, +1.0 }, Point3f{ +1.0, -1.0, +1.0 }, Point3f{ +1.0, +1.0, +1.0 },
						// Top
						Point3f{ -1.0, +1.0, +1.0 }, Point3f{ -1.0, +1.0, -1.0 }, Point3f{ +1.0, +1.0, +1.0 }, Point3f{ +1.0, +1.0, -1.0 },
						// Back
						Point3f{ -1.0, +1.0, -1.0 }, Point3f{ -1.0, -1.0, -1.0 }, Point3f{ +1.0, +1.0, -1.0 }, Point3f{ +1.0, -1.0, -1.0 },
						// Bottom
						Point3f{ -1.0, -1.0, -1.0 }, Point3f{ -1.0, -1.0, +1.0 }, Point3f{ +1.0, -1.0, -1.0 }, Point3f{ +1.0, -1.0, +1.0 },
						// Right
						Point3f{ +1.0, -1.0, +1.0 }, Point3f{ +1.0, +1.0, +1.0 }, Point3f{ +1.0, -1.0, -1.0 }, Point3f{ +1.0, +1.0, -1.0 },
						// Left
						Point3f{ -1.0, -1.0, -1.0 }, Point3f{ -1.0, +1.0, -1.0 }, Point3f{ -1.0, -1.0, +1.0 }, Point3f{ -1.0, +1.0, +1.0 },
					};
					castor::DataHolderT< ashes::VertexBufferPtr< castor::Point3f > >::setData( makeVertexBuffer< Point3f >( m_device
						, 24u
						, VK_BUFFER_USAGE_TRANSFER_DST_BIT
						, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
						, "Background" ) );
					auto & vertexBuffer = *castor::DataHolderT< ashes::VertexBufferPtr< castor::Point3f > >::getData();
					auto data = m_device.graphicsData();
					ashes::StagingBuffer stagingBuffer{ *m_device.device, 0u, VkDeviceSize( sizeof( Point3f ) * 24u ) };
					stagingBuffer.uploadVertexData( *data->queue
						, *data->commandPool
						, vertexData
						, vertexBuffer );
				}

				auto & vertexBuffer = *castor::DataHolderT< ashes::VertexBufferPtr< castor::Point3f > >::getData();
				return { crg::Buffer{ vertexBuffer.getBuffer(), "Vertex" }
					, vertexBuffer.getBuffer().getStorage()
					, { 1u, VkVertexInputAttributeDescription{ 0u, 0u, VK_FORMAT_R32G32B32_SFLOAT, 0u } }
					, { 1u, VkVertexInputBindingDescription{ 0u, sizeof( castor::Point3f ), VK_VERTEX_INPUT_RATE_VERTEX } } };
			}

			crg::VkPipelineShaderStageCreateInfoArray doInitialiseShader( RenderDevice const & device )
			{
				if ( castor::DataHolderT< Shaders >::getData().stages.empty() )
				{
					using namespace sdw;

					castor::DataHolderT< Shaders >::getData().vertexShader = { VK_SHADER_STAGE_VERTEX_BIT, "Background" };
					{
						VertexWriter writer;

						// Inputs
						auto position = writer.declInput< Vec3 >( "position", 0u );
						C3D_Matrix( writer, Bindings::eMatrix, 0u );
						C3D_ModelData( writer, Bindings::eModel, 0u );

						// Outputs
						auto vtx_texture = writer.declOutput< Vec3 >( "vtx_texture", 0u );

						writer.implementMainT< VoidT, VoidT >( [&]( VertexIn in
							, VertexOut out )
							{
								out.vtx.position = c3d_matrixData.worldToCurProj( c3d_modelData.modelToWorld( vec4( position, 1.0_f ) ) ).xyww();
								vtx_texture = position;
							} );

						castor::DataHolderT< Shaders >::getData().vertexShader.shader = std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
					}

					castor::DataHolderT< Shaders >::getData().pixelShader = { VK_SHADER_STAGE_FRAGMENT_BIT, "Background" };
					{
						FragmentWriter writer;

						// Inputs
						C3D_Scene( writer, Bindings::eScene, 0u );
						C3D_HdrConfig( writer, Bindings::eHdrConfig, 0u );
						auto vtx_texture = writer.declInput< Vec3 >( "vtx_texture", 0u );
						auto c3d_mapSkybox = writer.declCombinedImg< FImgCubeRgba32 >( "c3d_mapSkybox", Bindings::eSkybox, 0u );
						shader::Utils utils{ writer };

						// Outputs
						auto outColour = writer.declOutput< Vec4 >( "outColour", 0u );

						writer.implementMainT< VoidT, VoidT >( [&]( FragmentIn in
							, FragmentOut out )
							{
								IF( writer, c3d_sceneData.fogType == UInt( uint32_t( FogType::eDisabled ) ) )
								{
									auto colour = writer.declLocale( "colour"
										, c3d_mapSkybox.sample( vtx_texture ) );

									if ( !m_background->isHdr() && !m_background->isSRGB() )
									{
										outColour = vec4( c3d_hdrConfigData.removeGamma( colour.xyz() ), colour.w() );
									}
									else
									{
										outColour = vec4( colour.xyz(), colour.w() );
									}
								}
								ELSE
								{
									outColour = vec4( c3d_sceneData.getBackgroundColour( c3d_hdrConfigData ).xyz(), 1.0_f );
								}
								FI;
							} );
						castor::DataHolderT< Shaders >::getData().pixelShader.shader = std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
					}

					castor::DataHolderT< Shaders >::getData().stages =
					{
						makeShaderState( device, castor::DataHolderT< Shaders >::getData().vertexShader ),
						makeShaderState( device, castor::DataHolderT< Shaders >::getData().pixelShader ),
					};
				}

				return ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( castor::DataHolderT< Shaders >::getData().stages );
			}
		};
	}

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
		, crg::ImageViewId const & colour
		, crg::ImageViewId const * depth
		, crg::ImageViewId const * depthObj
		, UniformBufferOffsetT< ModelBufferConfiguration > const & modelUbo
		, MatrixUbo const & matrixUbo
		, HdrConfigUbo const & hdrConfigUbo
		, SceneUbo const & sceneUbo
		, bool clearColour
		, BackgroundPassBase *& backgroundPass )
	{
		auto & result = graph.createPass( "Background"
			, [this, &backgroundPass, &device, progress, size, depth]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBar( progress, "Initialising background pass" );
				auto res = std::make_unique< back::BackgroundPass >( framePass
					, context
					, runnableGraph
					, device
					, *this
					, size
					, depth );
				backgroundPass = res.get();
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				return res;
			} );
		matrixUbo.createPassBinding( result
			, back::Bindings::eMatrix );
		modelUbo.createPassBinding( result
			, "Model"
			, back::Bindings::eModel );
		hdrConfigUbo.createPassBinding( result
			, back::Bindings::eHdrConfig );
		sceneUbo.createPassBinding( result
			, back::Bindings::eScene );
		result.addSampledView( m_textureId.sampledViewId
			, back::Bindings::eSkybox
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, crg::SamplerDesc{ VK_FILTER_LINEAR
				, VK_FILTER_LINEAR } );

		if ( depth )
		{
			result.addInOutDepthStencilView( *depth );
		}

		if ( clearColour )
		{
			result.addOutputColourView( colour
				, transparentBlackClearColor );
		}
		else
		{
			result.addInOutColourView( colour );
		}

		return result;
	}

	void SceneBackground::addPassBindings( crg::FramePass & pass
		, crg::ImageData const & targetImage
		, uint32_t & index )const
	{
		doAddPassBindings( pass, targetImage, index );

		if ( hasIbl() )
		{
			auto & ibl = getIbl();
			pass.addSampledView( ibl.getIrradianceTexture().sampledViewId
				, index++
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, crg::SamplerDesc{ VK_FILTER_LINEAR
					, VK_FILTER_LINEAR
					, VK_SAMPLER_MIPMAP_MODE_LINEAR } );
			pass.addSampledView( ibl.getPrefilteredEnvironmentTexture().sampledViewId
				, index++
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, crg::SamplerDesc{ VK_FILTER_LINEAR
					, VK_FILTER_LINEAR
					, VK_SAMPLER_MIPMAP_MODE_LINEAR } );
			pass.addSampledView( ibl.getPrefilteredEnvironmentSheenTexture().sampledViewId
				, index++
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, crg::SamplerDesc{ VK_FILTER_LINEAR
					, VK_FILTER_LINEAR
					, VK_SAMPLER_MIPMAP_MODE_LINEAR } );
		}
	}

	void SceneBackground::addBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
		, uint32_t & index )const
	{
		doAddBindings( bindings, index );

		if ( hasIbl() )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapIrradiance
			bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapPrefiltered
			bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapPrefilteredSheen
		}
	}

	void SceneBackground::addDescriptors( ashes::WriteDescriptorSetArray & descriptorWrites
		, crg::ImageData const & targetImage
		, uint32_t & index )const
	{
		doAddDescriptors( descriptorWrites, targetImage, index );

		if ( hasIbl() )
		{
			auto & ibl = getIbl();
			bindTexture( ibl.getIrradianceTexture().wholeView
				, ibl.getIrradianceSampler()
				, descriptorWrites
				, index );
			bindTexture( ibl.getPrefilteredEnvironmentTexture().wholeView
				, ibl.getPrefilteredEnvironmentSampler()
				, descriptorWrites
				, index );
			bindTexture( ibl.getPrefilteredEnvironmentSheenTexture().wholeView
				, ibl.getPrefilteredEnvironmentSheenSampler()
				, descriptorWrites
				, index );
		}
	}

	castor::String const & SceneBackground::getModelName()const
	{
		if ( hasIbl() )
		{
			return shader::IblBackgroundModel::Name;
		}

		return shader::NoIblBackgroundModel::Name;
	}

	//*********************************************************************************************
}
