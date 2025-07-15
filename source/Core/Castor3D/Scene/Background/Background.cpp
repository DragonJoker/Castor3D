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

CU_ImplementSmartPtr( c3d, SceneBackground )

namespace c3d
{
	//*********************************************************************************************

	namespace back
	{
		enum class Bindings : uint32_t
		{
			eMatrix = 0u,
			eModel = 1u,
			eHdrConfig = 2u,
			eScene = 3u,
			eSkybox = 4u,
			eIrradiance = 5u,
		};

		struct Shaders
		{
			ProgramModule shader;
			ashes::PipelineShaderStageCreateInfoArray stages;
		};

		using Programs = Array< Shaders, SceneBackground::PassCount >;

		class BackgroundPass
			: public DataHolderT< ashes::VertexBufferPtr< Point3f > >
			, public DataHolderT< ashes::BufferPtr< uint16_t > >
			, public DataHolderT< Programs >
			, public BackgroundPassBase
			, public crg::RenderMesh
		{
			using VertexBufferHolder = DataHolderT< ashes::VertexBufferPtr< Point3f > >;
			using IndexBufferHolder = DataHolderT< ashes::BufferPtr< u16 > >;

			crg::rm::Config buildConfig( RenderDevice const & device
				, Extent2D const & size
				, crg::ImageViewIdArray const & depth
				, bool forceVisible )
			{
				crg::rm::Config result;
				result.vertexBuffer( doCreateVertexBuffer( device ) )
					.indexBuffer( doCreateIndexBuffer( device ) )
					.depthStencilState( ( depth.empty()
						? ashes::PipelineDepthStencilStateCreateInfo{ 0u, VK_FALSE, VK_FALSE, VK_COMPARE_OP_GREATER_OR_EQUAL }
						: ashes::PipelineDepthStencilStateCreateInfo{ 0u, VK_TRUE, VK_FALSE, VK_COMPARE_OP_GREATER_OR_EQUAL } ) )
					.getIndexType( crg::GetIndexTypeCallback( [](){ return VK_INDEX_TYPE_UINT16; } ) )
					.getPrimitiveCount( crg::GetPrimitiveCountCallback( [](){ return 36u; } ) )
					.isEnabled( IsEnabledCallback( [this](){ return doIsEnabled(); } ) )
					.getPassIndex( GetPassIndexCallback( [this, forceVisible]() { return m_background->getPassIndex( forceVisible ); } ) )
					.renderSize( size )
					.programCreator( { SceneBackground::PassCount
						, [this, &device]( uint32_t programIndex )
						{
							return crg::makeVkArray< VkPipelineShaderStageCreateInfo >( doInitialiseShader( device, programIndex ) );
						} } );
				return result;
			}

		public:
			BackgroundPass( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph
				, RenderDevice const & device
				, SceneBackground & background
				, Extent2D const & size
				, crg::ImageViewIdArray const & depth
				, bool forceVisible )
				: BackgroundPassBase{ pass
					, device
					, background
					, forceVisible }
				, crg::RenderMesh{ pass
					, context
					, graph
					, crg::ru::Config{ SceneBackground::PassCount, true }
					, buildConfig( device, size, depth, forceVisible ) }
			{
			}

		private:
			void doResetPipeline( uint32_t index )override
			{
				resetPipeline( {}, index );
			}

			crg::IndexBuffer doCreateIndexBuffer( RenderDevice const & device )
			{
				if ( !DataHolderT< ashes::BufferPtr< uint16_t > >::getData() )
				{
					Vector< uint16_t > indexData
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
					DataHolderT< ashes::BufferPtr< uint16_t > >::setData( makeBuffer< uint16_t >( device
						, uint32_t( indexData.size() )
						, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
						, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
						, cuT( "BackgroundIndexBuffer" ) ) );
					auto & indexBuffer = *DataHolderT< ashes::BufferPtr< uint16_t > >::getData();
					{
						auto data = m_device.graphicsData();
						InstantDirectUploadData uploader{ *data->queue
							, device
							, cuT( "BackgroundIndexBuffer" )
							, *data->commandPool };
						uploader->pushUpload( indexData.data()
							, VkDeviceSize( sizeof( uint16_t ) * indexData.size() )
							, indexBuffer.getBuffer(), 0u
							, VertexIndexInputState );
					}
				}

				auto & indexBuffer = *DataHolderT< ashes::BufferPtr< uint16_t > >::getData();
				return crg::IndexBuffer{ crg::Buffer{ indexBuffer.getBuffer(), "Index" }
					, indexBuffer.getBuffer().getStorage() };
			}

			crg::VertexBuffer doCreateVertexBuffer( RenderDevice const & device )
			{
				if ( !VertexBufferHolder::getData() )
				{
					// Vertex Buffer
					static constexpr Array< Point3f, 24u > vertexData
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
						, cuT( "Background" )) );
					auto & vertexBuffer = *VertexBufferHolder::getData();
					{
						auto data = m_device.graphicsData();
						InstantDirectUploadData uploader{ *data->queue
							, device
							, cuT( "BackgroundVertexBuffer" )
							, *data->commandPool };
						uploader->pushUpload( vertexData.data()
							, vertexData.size() * sizeof( Point3f )
							, vertexBuffer.getBuffer(), 0u
							, VertexAttributeInputState );
					}
				}

				auto & vertexBuffer = *VertexBufferHolder::getData();
				return crg::VertexBuffer{ crg::Buffer{ vertexBuffer.getBuffer(), "Vertex" }
					, vertexBuffer.getBuffer().getStorage()
					, { 1u, VkVertexInputAttributeDescription{ 0u, 0u, VK_FORMAT_R32G32B32_SFLOAT, 0u } }
					, { 1u, VkVertexInputBindingDescription{ 0u, sizeof( Point3f ), VK_VERTEX_INPUT_RATE_VERTEX } } };
			}

			crg::VkPipelineShaderStageCreateInfoArray doInitialiseShader( RenderDevice const & device
				, uint32_t programIndex )
			{
				auto & program = DataHolderT< Programs >::getData()[programIndex];

				if ( program.stages.empty() )
				{
					auto & engine = *device.renderSystem.getEngine();
					program.shader = ProgramModule{ cuT( "Background" ) };
					{
						sdw::TraditionalGraphicsWriter writer{ &engine.getShaderAllocator() };
						shader::Utils utils{ writer };

						C3D_Camera( writer, Bindings::eMatrix, 0u );
						C3D_ModelData( writer, Bindings::eModel, 0u );
						C3D_HdrConfig( writer, Bindings::eHdrConfig, 0u );
						C3D_Scene( writer, Bindings::eScene, 0u );
						auto c3d_mapSkybox = writer.declCombinedImg< FImgCubeRgba32 >( "c3d_mapSkybox", uint32_t( Bindings::eSkybox ), 0u, programIndex == SceneBackground::VisiblePassIndex );
						auto c3d_mapIrradiance = writer.declCombinedImg< FImgCubeRgba32 >( "c3d_mapIrradiance", uint32_t( Bindings::eIrradiance ), 0u, programIndex == SceneBackground::IrradiancePassIndex );

						writer.implementEntryPointT< shader::Position3FT, shader::Uv3FT >( [&c3d_cameraData, &c3d_modelData]( sdw::VertexInT< shader::Position3FT > const & in
							, sdw::VertexOutT< shader::Uv3FT > out )
							{
								out.vtx.position = c3d_cameraData.worldToCurProj( c3d_modelData.modelToWorld( vec4( in.position(), 1.0_f ) ) ).xyww();
								out.uv() = in.position();
							} );

						writer.implementEntryPointT< shader::Uv3FT, shader::Colour4FT >( [this, &writer, &c3d_sceneData, &c3d_hdrConfigData, &c3d_mapSkybox, &c3d_mapIrradiance, programIndex]( sdw::FragmentInT< shader::Uv3FT > const & in
							, sdw::FragmentOutT< shader::Colour4FT > const & out )
							{
								if ( programIndex != SceneBackground::HiddenPassIndex )
								{
									sdwIF( writer, sdw::UInt{ programIndex } != sdw::UInt{ SceneBackground::HiddenPassIndex }
										&& c3d_sceneData.fogType() == sdw::UInt( uint32_t( FogType::eDisabled ) ) )
									{
										auto colour = writer.declLocale( "colour"
											, ( programIndex == SceneBackground::IrradiancePassIndex
												? c3d_mapIrradiance.sample( in.uv() )
												: c3d_mapSkybox.sample( in.uv() ) ) );

										if ( !m_background->isHdr() && !m_background->isSRGB() )
										{
											out.colour() = vec4( c3d_hdrConfigData.removeGamma( colour.xyz() ), colour.w() );
										}
										else
										{
											out.colour() = vec4( colour.xyz(), colour.w() );
										}
									}
									sdwELSE
									{
										out.colour() = vec4( c3d_sceneData.getBackgroundColour( c3d_hdrConfigData ).xyz(), 1.0_f );
									}
									sdwFI
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
		, String const & name
		, String type
		, bool hasIBLSupport )
		: OwnedBy< Engine >{ engine }
		, Named{ scene.getName() + name }
		, m_scene{ scene }
		, m_type{ c3d::move( type ) }
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
			m_passIndex = ( m_visible
				? VisiblePassIndex
				: ( isIrradianceShown()
					? IrradiancePassIndex
					: HiddenPassIndex ) );
			m_initialised = doInitialise( device );
			String const name = cuT( "Skybox_" ) + string::toString( m_texture->getMipLevels() );
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
				created->setMinFilter( FilterMode::eLinear );
				created->setMagFilter( FilterMode::eLinear );
				created->setWrapS( WrapMode::eClampToEdge );
				created->setWrapT( WrapMode::eClampToEdge );
				created->setWrapR( WrapMode::eClampToEdge );
				created->setMinLod( 0.0f );
				created->setMaxLod( float( m_texture->getMipLevels() - 1u ) );
				created->setSerialisable( false );
				sampler = getEngine()->addSampler( name, created, false );

				if ( m_texture->getMipLevels() > 1u )
				{
					sampler->setMipFilter( MipmapMode::eLinear );
				}
			}

			sampler->initialise( device );
			m_sampler = sampler;

			if ( m_initialised
				&& m_hasIBLSupport
				&& m_texture->getLayersCount() == 6u )
			{
				m_ibl = makeRawUnique< IblTextures >( m_scene
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
			static Point3f const Scale{ 1, -1, 1 };
			static Matrix3x3f const Identity{ 1.0f };

			auto const & camera = *updater.camera;
			auto node = camera.getParent();

			matrix::setTranslate( updater.bgMtxModl, node->getDerivedPosition() );
			matrix::scale( updater.bgMtxModl, Scale );
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
		, Extent2D const & size
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
				, makeLayoutState( ImageLayout::eShaderReadOnly ) );
			graph.addInput( ibl.getPrefilteredEnvironmentTexture().sampledViewId
				, makeLayoutState( ImageLayout::eShaderReadOnly ) );
			graph.addInput( ibl.getPrefilteredEnvironmentSheenTexture().sampledViewId
				, makeLayoutState( ImageLayout::eShaderReadOnly ) );
		}

		graph.addInput( m_textureId.sampledViewId
			, crg::makeLayoutState( ImageLayout::eShaderReadOnly ) );

		auto & result = graph.createPass( "Background"
			, [this, &backgroundPass, &device, progress, size, depth, forceVisible]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBarLocal( progress, cuT( "Initialising background pass" ) );
				auto res = makeRawUnique< back::BackgroundPass >( framePass
					, context
					, runnableGraph
					, device
					, *this
					, size
					, depth
					, forceVisible );
				backgroundPass = res.get();
				device.renderSystem.getEngine()->registerTimer( makeString( framePass.getFullName() )
					, res->getTimer() );
				return res;
			} );
		cameraUbo.createPassBinding( result
			, uint32_t( back::Bindings::eMatrix ) );
		modelUbo.createPassBinding( result
			, "Model"
			, uint32_t( back::Bindings::eModel ) );
		hdrConfigUbo.createPassBinding( result
			, uint32_t( back::Bindings::eHdrConfig ) );
		sceneUbo.createPassBinding( result
			, uint32_t( back::Bindings::eScene ) );
		result.addSampledView( m_textureId.sampledViewId
			, uint32_t( back::Bindings::eSkybox )
			, crg::SamplerDesc{ FilterMode::eLinear
				, FilterMode::eLinear } );

		if ( hasIbl() )
		{
			result.addSampledView( getIbl().getIrradianceTexture().sampledViewId
				, uint32_t( back::Bindings::eIrradiance )
				, crg::SamplerDesc{ FilterMode::eLinear
					, FilterMode::eLinear } );
		}

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
				, index
				, crg::SamplerDesc{ FilterMode::eLinear
					, FilterMode::eLinear
					, MipmapMode::eLinear } );
			++index;
			pass.addSampledView( ibl.getPrefilteredEnvironmentTexture().sampledViewId
				, index
				, crg::SamplerDesc{ FilterMode::eLinear
					, FilterMode::eLinear
					, MipmapMode::eLinear } );
			++index;
			pass.addSampledView( ibl.getPrefilteredEnvironmentSheenTexture().sampledViewId
				, index
				, crg::SamplerDesc{ FilterMode::eLinear
					, FilterMode::eLinear
					, MipmapMode::eLinear } );
			++index;
		}
	}

	void SceneBackground::addBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
		, VkShaderStageFlags shaderStages
		, uint32_t & index )const
	{
		doAddBindings( bindings, shaderStages, index );

		if ( hasIbl() )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( index
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, shaderStages ) );	// c3d_mapIrradiance
			++index;
			bindings.emplace_back( makeDescriptorSetLayoutBinding( index
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, shaderStages ) );	// c3d_mapPrefiltered
			++index;
			bindings.emplace_back( makeDescriptorSetLayoutBinding( index
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, shaderStages ) );	// c3d_mapPrefilteredSheen
			++index;
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

	String const & SceneBackground::getModelName()const
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

	PxBufferBaseUPtr SceneBackground::adaptBuffer( PxBufferBase const & buffer
		, String const & name
		, bool generateMips )
	{
		auto result = buffer.clone();
		auto dstFormat = result->getFormat();

		switch ( dstFormat )
		{
		case PixelFormat::eR8G8B8_UNORM:
			dstFormat = PixelFormat::eR8G8B8A8_UNORM;
			break;
		case PixelFormat::eB8G8R8_UNORM:
			dstFormat = PixelFormat::eA8B8G8R8_UNORM;
			break;
		case PixelFormat::eR8G8_SRGB:
		case PixelFormat::eR8G8B8_SRGB:
			dstFormat = PixelFormat::eR8G8B8A8_SRGB;
			break;
		case PixelFormat::eB8G8R8_SRGB:
			dstFormat = PixelFormat::eA8B8G8R8_SRGB;
			break;
		case PixelFormat::eR16G16B16_SFLOAT:
			dstFormat = PixelFormat::eR16G16B16A16_SFLOAT;
			break;
		case PixelFormat::eR32G32B32_SFLOAT:
			dstFormat = PixelFormat::eR32G32B32A32_SFLOAT;
			break;
		default:
			// No conversion
			break;
		}

		if ( result->getFormat() != dstFormat )
		{
			log::debug << name << cuT( " - Converting RGB to RGBA.\n" );
			result = PxBufferBase::create( result->getDimensions()
				, result->getLayers()
				, result->getLevels()
				, dstFormat
				, result->getConstPtr()
				, result->getFormat()
				, result->getAlign() );
		}

		if ( generateMips
			&& !isCompressed( result->getFormat() ) )
		{
			log::debug << ( name + cuT( " - Generating result mipmaps.\n" ) );
			result->generateMips();
		}

		return result;
	}

	ImageUPtr SceneBackground::loadImage( Engine & engine
		, String const & name
		, Path const & folder
		, Path const & relative
		, bool generateMips )
	{
		auto & image = getFileImage( engine
			, name
			, folder
			, relative );
		auto buffer = adaptBuffer( image.getPxBuffer()
			, name
			, generateMips );
		ImageMemoryLayout layout{ image.getLayout().type, * buffer };
		return makeUnique< Image >( name
			, folder / relative
			, layout
			, c3d::move( buffer ) );
	}

	//*********************************************************************************************
}
