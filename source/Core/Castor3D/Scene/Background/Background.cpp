#include "Castor3D/Scene/Background/Background.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/DirectUploadData.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/InstantUploadData.hpp"
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
#include "Castor3D/Shader/Shaders/GlslBaseIO.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/HdrConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelDataUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <CastorUtils/Design/DataHolder.hpp>
#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/Core/Device.hpp>

#include <RenderGraph/FramePassGroup.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

CU_ImplementSmartPtr( castor3d, SceneBackground )

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
			castor3d::ProgramModule shader;
			ashes::PipelineShaderStageCreateInfoArray stages;
		};

		using Programs = std::array< Shaders, 2u >;

		class BackgroundPass
			: public castor::DataHolderT< ashes::VertexBufferPtr< castor::Point3f > >
			, public castor::DataHolderT< ashes::BufferPtr< uint16_t > >
			, public castor::DataHolderT< Programs >
			, public BackgroundPassBase
			, public crg::RenderMesh
		{
			using VertexBufferHolder = castor::DataHolderT< ashes::VertexBufferPtr< castor::Point3f > >;
			using IndexBufferHolder = castor::DataHolderT< ashes::BufferPtr< u16 > >;

		public:
			BackgroundPass( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph
				, RenderDevice const & device
				, SceneBackground & background
				, VkExtent2D const & size
				, crg::ImageViewIdArray const & colour
				, crg::ImageViewIdArray const & depth
				, bool forceVisible )
				: BackgroundPassBase{ pass
					, context
					, graph
					, device
					, background
					, forceVisible }
				, crg::RenderMesh{ pass
					, context
					, graph
					, crg::ru::Config{ 2u, true }
					, crg::rm::Config{}
						.vertexBuffer( doCreateVertexBuffer( device ) )
						.indexBuffer( doCreateIndexBuffer( device ) )
						.depthStencilState( ( depth.empty()
							? ashes::PipelineDepthStencilStateCreateInfo{ 0u, VK_FALSE, VK_FALSE, VK_COMPARE_OP_GREATER_OR_EQUAL }
							: ashes::PipelineDepthStencilStateCreateInfo{ 0u, VK_TRUE, VK_FALSE, VK_COMPARE_OP_GREATER_OR_EQUAL } ) )
						.getIndexType( crg::GetIndexTypeCallback( [](){ return VK_INDEX_TYPE_UINT16; } ) )
						.getPrimitiveCount( crg::GetPrimitiveCountCallback( [](){ return 36u; } ) )
						.isEnabled( IsEnabledCallback( [this](){ return doIsEnabled(); } ) )
						.getPassIndex( GetPassIndexCallback( [this, forceVisible]() { return m_background->getPassIndex( forceVisible ); } ) )
						.renderSize( size )
						.programCreator( { 2u
							, [this, &device]( uint32_t programIndex )
							{
								return crg::makeVkArray< VkPipelineShaderStageCreateInfo >( doInitialiseShader( device, programIndex ) );
							} } ) }
			{
			}

		private:
			void doResetPipeline( uint32_t index )override
			{
				resetPipeline( {}, index );
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
						, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
						, "BackgroundIndexBuffer" ) );
					auto & indexBuffer = *castor::DataHolderT< ashes::BufferPtr< uint16_t > >::getData();
					{
						auto data = m_device.graphicsData();
						InstantDirectUploadData uploader{ *data->queue
							, device
							, "BackgroundIndexBuffer"
							, *data->commandPool };
						uploader->pushUpload( indexData.data()
							, VkDeviceSize( sizeof( uint16_t ) * indexData.size() )
							, indexBuffer.getBuffer()
							, 0u
							, VK_ACCESS_INDEX_READ_BIT
							, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT );
					}
				}

				auto & indexBuffer = *castor::DataHolderT< ashes::BufferPtr< uint16_t > >::getData();
				return crg::IndexBuffer{ crg::Buffer{ indexBuffer.getBuffer(), "Index" }
					, indexBuffer.getBuffer().getStorage() };
			}

			crg::VertexBuffer doCreateVertexBuffer( RenderDevice const & device )
			{
				if ( !VertexBufferHolder::getData() )
				{
					using castor::Point3f;

					// Vertex Buffer
					static constexpr std::array< Point3f, 24u > vertexData
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
					VertexBufferHolder::setData( makeVertexBuffer< Point3f >( m_device
						, 24u
						, VK_BUFFER_USAGE_TRANSFER_DST_BIT
						, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
						, "Background" ) );
					auto & vertexBuffer = *VertexBufferHolder::getData();
					{
						auto data = m_device.graphicsData();
						InstantDirectUploadData uploader{ *data->queue
							, device
							, "BackgroundVertexBuffer"
							, *data->commandPool };
						uploader->pushUpload( vertexData.data()
							, vertexData.size() * sizeof( Point3f )
							, vertexBuffer.getBuffer()
							, 0u
							, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT
							, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT );
					}
				}

				auto & vertexBuffer = *VertexBufferHolder::getData();
				return crg::VertexBuffer{ crg::Buffer{ vertexBuffer.getBuffer(), "Vertex" }
					, vertexBuffer.getBuffer().getStorage()
					, { 1u, VkVertexInputAttributeDescription{ 0u, 0u, VK_FORMAT_R32G32B32_SFLOAT, 0u } }
					, { 1u, VkVertexInputBindingDescription{ 0u, sizeof( castor::Point3f ), VK_VERTEX_INPUT_RATE_VERTEX } } };
			}

			crg::VkPipelineShaderStageCreateInfoArray doInitialiseShader( RenderDevice const & device
				, uint32_t programIndex )
			{
				auto & program = castor::DataHolderT< Programs >::getData()[programIndex];

				if ( program.stages.empty() )
				{
					auto & engine = *device.renderSystem.getEngine();
					program.shader = ProgramModule{ "Background" };
					{
						sdw::TraditionalGraphicsWriter writer{ &engine.getShaderAllocator() };
						shader::Utils utils{ writer };

						C3D_Camera( writer, Bindings::eMatrix, 0u );
						C3D_ModelData( writer, Bindings::eModel, 0u );
						C3D_HdrConfig( writer, Bindings::eHdrConfig, 0u );
						C3D_Scene( writer, Bindings::eScene, 0u );
						auto c3d_mapSkybox = writer.declCombinedImg< FImgCubeRgba32 >( "c3d_mapSkybox", Bindings::eSkybox, 0u, programIndex == 0u );

						writer.implementEntryPointT< shader::Position3FT, shader::Uv3FT >( [&]( sdw::VertexInT< shader::Position3FT > in
							, sdw::VertexOutT< shader::Uv3FT > out )
							{
								out.vtx.position = c3d_cameraData.worldToCurProj( c3d_modelData.modelToWorld( vec4( in.position(), 1.0_f ) ) ).xyww();
								out.uv() = in.position();
							} );

						writer.implementEntryPointT< shader::Uv3FT, shader::Colour4FT >( [&]( sdw::FragmentInT< shader::Uv3FT >  in
							, sdw::FragmentOutT< shader::Colour4FT > out )
							{
								if ( programIndex == SceneBackground::VisiblePassIndex )
								{
									IF( writer, sdw::UInt{ programIndex } == 0_u
										&& c3d_sceneData.fogType() == sdw::UInt( uint32_t( FogType::eDisabled ) ) )
									{
										auto colour = writer.declLocale( "colour"
											, c3d_mapSkybox.sample( in.uv() ) );

										if ( !m_background->isHdr() && !m_background->isSRGB() )
										{
											out.colour() = vec4( c3d_hdrConfigData.removeGamma( colour.xyz() ), colour.w() );
										}
										else
										{
											out.colour() = vec4( colour.xyz(), colour.w() );
										}
									}
									ELSE
									{
										out.colour() = vec4( c3d_sceneData.getBackgroundColour( c3d_hdrConfigData ).xyz(), 1.0_f );
									}
									FI;
								}
								else
								{
									out.colour() = vec4( c3d_sceneData.getBackgroundColour( c3d_hdrConfigData ).xyz(), 1.0_f );
								}
							} );
						program.shader.shader = writer.getBuilder().releaseShader();
					}

					program.stages = makeProgramStates( device, program.shader );
				}

				return ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( program.stages );
			}
		};
	}

	//*********************************************************************************************

	SceneBackground::SceneBackground( Engine & engine
		, Scene & scene
		, castor::String const & name
		, castor::String type
		, bool hasIBLSupport )
		: castor::OwnedBy< Engine >{ engine }
		, castor::Named{ scene.getName() + name }
		, m_scene{ scene }
		, m_type{ std::move( type ) }
		, m_hasIBLSupport{ hasIBLSupport }
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
			castor::String const name = cuT( "Skybox_" ) + castor::string::toString( m_texture->getMipLevels() );
			{
				auto queueData = device.graphicsData();
				InstantDirectUploadData uploader{ *queueData->queue
					, device
					, name
					, *queueData->commandPool };
				upload( uploader );
			}
			auto sampler = getEngine()->tryFindSampler( name );

			if ( !sampler )
			{
				auto created = getEngine()->createSampler( name, *getEngine() );
				created->setMinFilter( VK_FILTER_LINEAR );
				created->setMagFilter( VK_FILTER_LINEAR );
				created->setWrapS( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				created->setWrapT( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				created->setWrapR( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				created->setMinLod( 0.0f );
				created->setMaxLod( float( m_texture->getMipLevels() - 1u ) );
				sampler = getEngine()->addSampler( name, created, false );

				if ( m_texture->getMipLevels() > 1u )
				{
					sampler->setMipFilter( VK_SAMPLER_MIPMAP_MODE_LINEAR );
				}
			}

			sampler->initialise( device );
			m_sampler = sampler;

			if ( m_initialised
				&& m_hasIBLSupport
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

	void SceneBackground::upload( UploadData & uploader )
	{
		if ( m_initialised && m_needsUpload )
		{
			doUpload( uploader );
			m_needsUpload = false;
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
		, crg::ImageViewIdArray const & colour
		, crg::ImageViewIdArray const & depth
		, crg::ImageViewId const * depthObj
		, UniformBufferOffsetT< ModelBufferConfiguration > const & modelUbo
		, CameraUbo const & cameraUbo
		, HdrConfigUbo const & hdrConfigUbo
		, SceneUbo const & sceneUbo
		, bool clearColour
		, bool clearDepth
		, bool forceVisible
		, BackgroundPassBase *& backgroundPass )
	{
		if ( hasIbl() )
		{
			auto & ibl = getIbl();
			graph.addInput( ibl.getIrradianceTexture().sampledViewId
				, crg::makeLayoutState( VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) );
			graph.addInput( ibl.getPrefilteredEnvironmentTexture().sampledViewId
				, crg::makeLayoutState( VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) );
			graph.addInput( ibl.getPrefilteredEnvironmentSheenTexture().sampledViewId
				, crg::makeLayoutState( VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) );
		}

		graph.addInput( m_textureId.sampledViewId
			, crg::makeLayoutState( VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) );

		auto & result = graph.createPass( "Background"
			, [this, &backgroundPass, &device, progress, size, colour, depth, forceVisible]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBarLocal( progress, "Initialising background pass" );
				auto res = std::make_unique< back::BackgroundPass >( framePass
					, context
					, runnableGraph
					, device
					, *this
					, size
					, colour
					, depth
					, forceVisible );
				backgroundPass = res.get();
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				return res;
			} );
		cameraUbo.createPassBinding( result
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
			, crg::SamplerDesc{ VK_FILTER_LINEAR
				, VK_FILTER_LINEAR } );

		if ( !depth.empty() )
		{
			if ( clearDepth )
			{
				result.addOutputDepthStencilView( depth
					, defaultClearDepthStencil );
			}
			else
			{
				result.addInOutDepthStencilView( depth );
			}
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
		, crg::ImageViewIdArray const & targetImage
		, uint32_t & index )const
	{
		doAddPassBindings( pass, targetImage, index );

		if ( hasIbl() )
		{
			auto & ibl = getIbl();
			pass.addSampledView( ibl.getIrradianceTexture().sampledViewId
				, index++
				, crg::SamplerDesc{ VK_FILTER_LINEAR
					, VK_FILTER_LINEAR
					, VK_SAMPLER_MIPMAP_MODE_LINEAR } );
			pass.addSampledView( ibl.getPrefilteredEnvironmentTexture().sampledViewId
				, index++
				, crg::SamplerDesc{ VK_FILTER_LINEAR
					, VK_FILTER_LINEAR
					, VK_SAMPLER_MIPMAP_MODE_LINEAR } );
			pass.addSampledView( ibl.getPrefilteredEnvironmentSheenTexture().sampledViewId
				, index++
				, crg::SamplerDesc{ VK_FILTER_LINEAR
					, VK_FILTER_LINEAR
					, VK_SAMPLER_MIPMAP_MODE_LINEAR } );
		}
	}

	void SceneBackground::addBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
		, VkShaderStageFlags shaderStages
		, uint32_t & index )const
	{
		doAddBindings( bindings, shaderStages, index );

		if ( hasIbl() )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, shaderStages ) );	// c3d_mapIrradiance
			bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, shaderStages ) );	// c3d_mapPrefiltered
			bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, shaderStages ) );	// c3d_mapPrefilteredSheen
		}
	}

	void SceneBackground::addDescriptors( ashes::WriteDescriptorSetArray & descriptorWrites
		, crg::ImageViewIdArray const & targetImage
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

	BackgroundModelID SceneBackground::getModelID()const
	{
		return getEngine()->getBackgroundModelFactory().getTypeId( getModelName() );
	}

	castor::PxBufferBaseUPtr SceneBackground::adaptBuffer( Engine & engine
		, castor::PxBufferBase & buffer
		, castor::String const & name
		, bool generateMips )
	{
		auto result = buffer.clone();
		auto dstFormat = result->getFormat();

		switch ( dstFormat )
		{
		case castor::PixelFormat::eR8G8B8_UNORM:
			dstFormat = castor::PixelFormat::eR8G8B8A8_UNORM;
			break;
		case castor::PixelFormat::eB8G8R8_UNORM:
			dstFormat = castor::PixelFormat::eA8B8G8R8_UNORM;
			break;
		case castor::PixelFormat::eR8G8_SRGB:
		case castor::PixelFormat::eR8G8B8_SRGB:
			dstFormat = castor::PixelFormat::eR8G8B8A8_SRGB;
			break;
		case castor::PixelFormat::eB8G8R8_SRGB:
			dstFormat = castor::PixelFormat::eA8B8G8R8_SRGB;
			break;
		case castor::PixelFormat::eR16G16B16_SFLOAT:
			dstFormat = castor::PixelFormat::eR16G16B16A16_SFLOAT;
			break;
		case castor::PixelFormat::eR32G32B32_SFLOAT:
			dstFormat = castor::PixelFormat::eR32G32B32A32_SFLOAT;
			break;
		default:
			// No conversion
			break;
		}

		if ( result->getFormat() != dstFormat )
		{
			log::debug << name << cuT( " - Converting RGB to RGBA.\n" );
			result = castor::PxBufferBase::create( result->getDimensions()
				, result->getLayers()
				, result->getLevels()
				, dstFormat
				, result->getConstPtr()
				, result->getFormat()
				, result->getAlign() );
		}

		if ( generateMips
			&& !castor::isCompressed( result->getFormat() ) )
		{
			log::debug << ( name + cuT( " - Generating result mipmaps.\n" ) );
			result->generateMips();
		}

		return result;
	}

	castor::ImageUPtr SceneBackground::loadImage( Engine & engine
		, castor::String const & name
		, castor::Path const & folder
		, castor::Path const & relative
		, bool generateMips )
	{
		auto & image = getFileImage( engine
			, name
			, folder
			, relative );
		auto buffer = adaptBuffer( engine
			, image.getPxBuffer()
			, name
			, generateMips );
		castor::ImageLayout layout{ image.getLayout().type, * buffer };
		return castor::makeUnique< castor::Image >( name
			, folder / relative
			, layout
			, std::move( buffer ) );
	}

	//*********************************************************************************************
}
