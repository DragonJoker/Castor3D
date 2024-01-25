#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LayeredLightPropagationVolumes.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/GeometryInjectionPass.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightInjectionPass.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationPass.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LpvConfig.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapResult.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"
#include "Castor3D/Shader/Ubos/LayeredLpvGridConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/LpvGridConfigUbo.hpp"

#include <RenderGraph/GraphContext.hpp>
#include <RenderGraph/RunnablePass.hpp>
#include <RenderGraph/RunnablePasses/ImageBlit.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace llpvpropvol
	{
		class LpvClear
			: public crg::RunnablePass
		{
		public:
			LpvClear( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
				: crg::RunnablePass{ pass
					, context
					, graph
					, { crg::defaultV< InitialiseCallback >
						, GetPipelineStateCallback( [](){ return crg::getPipelineState( VK_PIPELINE_STAGE_TRANSFER_BIT ); } )
						, RecordCallback( [this]( crg::RecordContext &, VkCommandBuffer cb, uint32_t i ){ doRecordInto( cb, i ); } ) } }
			{
			}

		private:
			void doRecordInto( VkCommandBuffer commandBuffer
				, uint32_t index )
			{
				auto clearValue = transparentBlackClearColor.color;

				for ( auto & attach : m_pass.images )
				{
					auto view = attach.view( index );
					auto image = m_graph.createImage( view.data->image );
					assert( attach.isTransferOutputView() );
					m_context.vkCmdClearColorImage( commandBuffer
						, image
						, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
						, &clearValue
						, 1u
						, &view.data->info.subresourceRange );
				}
			}
		};

		static castor::Vector< LightVolumePassResult > createInjection( crg::ResourcesCache & resources
			, RenderDevice const & device
			, castor::String const & name
			, uint32_t lpvGridSize
			, uint32_t cascadeCount )
		{
			castor::Vector< LightVolumePassResult > result;

			for ( uint32_t cascade = 0u; cascade < cascadeCount; ++cascade )
			{
				result.emplace_back( resources
					, device
					, name + cuT( "Injection" ) + castor::string::toString( cascade )
					, lpvGridSize );
			}

			return result;
		}

		static TextureArray createGeometry( crg::ResourcesCache & resources
			, RenderDevice const & device
			, castor::String const & name
			, uint32_t lpvGridSize
			, uint32_t cascadeCount
			, bool enabled )
		{
			TextureArray result;

			for ( uint32_t cascade = 0u; cascade < cascadeCount; ++cascade )
			{
				result.push_back( enabled
					? GeometryInjectionPass::createResult( resources
						, device
						, name
						, cascade
						, lpvGridSize )
					: Texture{} );
			}

			return result;
		}

		static castor::Vector< castor::Array< LightVolumePassResult, 2u > > createPropagation( crg::ResourcesCache & resources
			, RenderDevice const & device
			, castor::String const & name
			, uint32_t lpvGridSize
			, uint32_t cascadeCount )
		{
			castor::Vector< castor::Array< LightVolumePassResult, 2u > > result;

			for ( uint32_t cascade = 0u; cascade < cascadeCount; ++cascade )
			{
				result.push_back( { LightVolumePassResult{ resources, device, name + cuT( "Propagate" ) + castor::string::toString( cascade ) + cuT( "0" ), lpvGridSize }
					, LightVolumePassResult{ resources, device, name + cuT( "Propagate" ) + castor::string::toString( cascade ) + cuT( "1" ), lpvGridSize } } );
			}

			return result;
		}
	}

	//*********************************************************************************************

	LayeredLightPropagationVolumesBase::LightLpv::LightLpv( crg::FramePassGroup & graph
		, crg::FramePass const & previousPass
		, RenderDevice const & device
		, castor::String const & name
		, LightCache const & plightCache
		, LightType lightType
		, ShadowMapResult const & smResult
		, LpvGridConfigUboArray const & lpvGridConfigUbos
		, castor::Vector< LightVolumePassResult > const & injection
		, TextureArray const * geometry )
		: lightCache{ plightCache }
		, lastLightPass{ &previousPass }
		, lastGeomPass{ &previousPass }
	{
		for ( uint32_t cascade = 0u; cascade < LpvMaxCascadesCount; ++cascade )
		{
			lpvLightConfigUbos.emplace_back( device );
			lightInjectionPassesDesc.emplace_back( &doCreateInjectionPass( graph
				, device
				, name
				, lightType
				, smResult
				, lpvGridConfigUbos
				, injection
				, cascade ) );

			if ( geometry )
			{
				geometryInjectionPassesDesc.emplace_back( &doCreateGeometryPass( graph
					, device
					, name
					, lightType
					, smResult
					, lpvGridConfigUbos
					, *geometry
					, cascade ) );
			}
		}
	}

	bool LayeredLightPropagationVolumesBase::LightLpv::update( CpuUpdater & updater
		, castor::Vector< float > const & lpvCellSizes )
	{
		auto & sceneObjs = updater.dirtyScenes[lightCache.getScene()];
		auto & light = *updater.light;
		auto changed = sceneObjs.dirtyLights.end() != std::find( sceneObjs.dirtyLights.begin(), sceneObjs.dirtyLights.end(), &light )
			|| light.getLpvConfig().indirectAttenuation.isDirty()
			|| light.getLpvConfig().texelAreaModifier.isDirty();

		if ( changed )
		{
			uint32_t index = 0;

			for ( auto & lpvLightConfigUbo : lpvLightConfigUbos )
			{
				lpvLightConfigUbo.cpuUpdate( light, lpvCellSizes[index], 0u );
				++index;
			}
		}

		return changed;
	}

	crg::FramePass & LayeredLightPropagationVolumesBase::LightLpv::doCreateInjectionPass( crg::FramePassGroup & graph
		, RenderDevice const & device
		, castor::String const & name
		, LightType lightType
		, ShadowMapResult const & smResult
		, LpvGridConfigUboArray const & lpvGridConfigUbos
		, castor::Vector< LightVolumePassResult > const & injection
		, uint32_t cascade )
	{
		auto rsmSize = smResult[SmTexture::eDepth].getExtent().width;
		auto & result = graph.createPass( castor::toUtf8( name ) + "LightInjection" + castor::string::toMbString( cascade )
			, [this, &device, lightType, rsmSize]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto res = castor::make_unique< LightInjectionPass >( framePass
					, context
					, runnableGraph
					, device
					, lightType
					, device.renderSystem.getEngine()->getLpvGridSize()
					, rsmSize );
				lightInjectionPasses.emplace_back( res.get() );
				device.renderSystem.getEngine()->registerTimer( castor::makeString( framePass.getFullName() )
					, res->getTimer() );
				return res;
			} );
		result.addDependency( *lastLightPass );
		lightCache.createPassBinding( result
			, LightInjectionPass::LightsIdx );
		result.addSampledView( smResult[SmTexture::eNormal].sampledViewId
			, LightInjectionPass::RsmNormalsIdx );
		result.addSampledView( smResult[SmTexture::ePosition].sampledViewId
			, LightInjectionPass::RsmPositionIdx );
		result.addSampledView( smResult[SmTexture::eFlux].sampledViewId
			, LightInjectionPass::RsmFluxIdx );
		lpvGridConfigUbos[cascade].createPassBinding( result
			, LightInjectionPass::LpvGridUboIdx );
		lpvLightConfigUbos[cascade].createPassBinding( result
			, LightInjectionPass::LpvLightUboIdx );

		result.addInOutColourView( injection[cascade][LpvTexture::eR].targetViewId );
		result.addInOutColourView( injection[cascade][LpvTexture::eG].targetViewId );
		result.addInOutColourView( injection[cascade][LpvTexture::eB].targetViewId );
		lastLightPass = &result;
		return result;
	}

	crg::FramePass & LayeredLightPropagationVolumesBase::LightLpv::doCreateGeometryPass( crg::FramePassGroup & graph
		, RenderDevice const & device
		, castor::String const & name
		, LightType lightType
		, ShadowMapResult const & smResult
		, LpvGridConfigUboArray const & lpvGridConfigUbos
		, TextureArray const & geometry
		, uint32_t cascade )
	{
		auto rsmSize = smResult[SmTexture::eDepth].getExtent().width;
		auto & result = graph.createPass( castor::toUtf8( name ) + "GeomInjection" + castor::string::toMbString( cascade )
			, [this, &device, lightType, rsmSize]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto res = castor::make_unique< GeometryInjectionPass >( framePass
					, context
					, runnableGraph
					, device
					, lightType
					, device.renderSystem.getEngine()->getLpvGridSize()
					, rsmSize );
				geometryInjectionPasses.emplace_back( res.get() );
				device.renderSystem.getEngine()->registerTimer( castor::makeString( framePass.getFullName() )
					, res->getTimer() );
				return res;
			} );
		result.addDependency( *lastGeomPass );
		lightCache.createPassBinding( result
			, GeometryInjectionPass::LightsIdx );
		result.addSampledView( smResult[SmTexture::eNormal].sampledViewId
			, GeometryInjectionPass::RsmNormalsIdx );
		result.addSampledView( smResult[SmTexture::ePosition].sampledViewId
			, GeometryInjectionPass::RsmPositionIdx );
		lpvGridConfigUbos[cascade].createPassBinding( result
			, GeometryInjectionPass::LpvGridUboIdx );
		lpvLightConfigUbos[cascade].createPassBinding( result
			, GeometryInjectionPass::LpvLightUboIdx );

		result.addInOutColourView( geometry[cascade].targetViewId );
		lastGeomPass = &result;
		return result;
	}

	//*********************************************************************************************

	LayeredLightPropagationVolumesBase::LayeredLightPropagationVolumesBase( crg::ResourcesCache & resources
		, Scene const & scene
		, LightType lightType
		, RenderDevice const & device
		, ShadowMapResult const & smResult
		, LightVolumePassResultArray const & lpvResult
		, LayeredLpvGridConfigUbo & lpvGridConfigUbo
		, bool geometryVolumes )
		: castor::Named{ cuT( "LLPV" ) + ( geometryVolumes ? castor::String( cuT( "G" ) ) : castor::String( cuT( "" ) ) ) }
		, m_scene{ scene }
		, m_device{ device }
		, m_sourceSmResult{ smResult }
		, m_downsampledSmResult{ resources
			, device
			, cuT( "LPV" )
			, ( ( lightType == LightType::ePoint ) ? VkImageCreateFlags( VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT ) : VkImageCreateFlags{} )
			, castor::Size{ 512u, 512u }
			, smResult[SmTexture::eDepth].imageId.data->info.arrayLayers }
		, m_lpvResult{ lpvResult }
		, m_lpvGridConfigUbo{ lpvGridConfigUbo }
		, m_geometryVolumes{ geometryVolumes }
		, m_graph{ resources.getHandler(), castor::toUtf8( getName() ) }
		, m_lightType{ lightType }
		, m_injection{ llpvpropvol::createInjection( resources, m_device, getName(), m_scene.getLpvGridSize(), LpvMaxCascadesCount ) }
		, m_geometry{ llpvpropvol::createGeometry( resources, m_device, getName(), m_scene.getLpvGridSize(), LpvMaxCascadesCount, m_geometryVolumes ) }
		, m_propagate{ llpvpropvol::createPropagation( resources, m_device, getName(), m_scene.getLpvGridSize(), LpvMaxCascadesCount ) }
		, m_clearInjectionPass{ doCreateClearInjectionPass() }
		, m_downsamplePass{ doCreateDownsamplePass() }
	{
		m_graph.addInput( m_sourceSmResult[SmTexture::eNormal].targetViewId
			, crg::makeLayoutState( VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) );
		m_graph.addInput( m_sourceSmResult[SmTexture::ePosition].targetViewId
			, crg::makeLayoutState( VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) );
		m_graph.addInput( m_sourceSmResult[SmTexture::eFlux].targetViewId
			, crg::makeLayoutState( VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) );

		for ( uint32_t cascade = 0u; cascade < LpvMaxCascadesCount; ++cascade )
		{
			m_lpvGridConfigUbos.emplace_back( m_device );
			auto const & result = *lpvResult[cascade];
			m_graph.addOutput( result[LpvTexture::eR].targetViewId
				, crg::makeLayoutState( VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) );
			m_graph.addOutput( result[LpvTexture::eG].targetViewId
				, crg::makeLayoutState( VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) );
			m_graph.addOutput( result[LpvTexture::eB].targetViewId
				, crg::makeLayoutState( VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) );
		}
	}

	void LayeredLightPropagationVolumesBase::initialise()
	{
		if ( !m_initialised
			&& m_scene.needsGlobalIllumination( m_lightType
				, ( m_geometryVolumes
					? GlobalIlluminationType::eLayeredLpvG
					: GlobalIlluminationType::eLayeredLpv ) ) )
		{
			m_aabb = m_scene.getBoundingBox();
			m_lightPropagationPassesDesc = doCreatePropagationPasses();
			m_runnable = m_graph.compile( m_device.makeContext() );
			m_scene.getEngine()->registerTimer( castor::makeString( m_runnable->getName() ) + cuT( "/Graph" )
				, m_runnable->getTimer() );
			printGraph( *m_runnable );
			m_recordEvent = m_device.renderSystem.getEngine()->postEvent( makeGpuFunctorEvent( GpuEventType::ePreUpload
				, [this]( RenderDevice const &
					, QueueData const & )
				{
					m_runnable->record();
					m_recordEvent = nullptr;
				} ) );
			m_initialised = true;
		}
	}

	void LayeredLightPropagationVolumesBase::cleanup()noexcept
	{
		m_initialised = false;
		m_lightPropagationPasses = {};
		m_lightLpvs.clear();
	}

	void LayeredLightPropagationVolumesBase::registerLight( Light * light )
	{
		auto it = m_lightLpvs.find( light );

		if ( it == m_lightLpvs.end() )
		{
			auto & group = m_graph.createPassGroup( castor::toUtf8( light->getName() ) );
			it = m_lightLpvs.try_emplace( light
				, group
				, m_downsamplePass
				, m_device
				, this->getName() + light->getName()
				, light->getScene()->getLightCache()
				, m_lightType
				, m_downsampledSmResult
				, m_lpvGridConfigUbos
				, m_injection
				, ( m_geometryVolumes ? &m_geometry : nullptr ) ).first;

			for ( uint32_t cascade = 0u; cascade < LpvMaxCascadesCount; ++cascade )
			{
				auto index = cascade * LpvMaxPropagationSteps;
				m_lightPropagationPassesDesc[index]->addDependency( *it->second.lightInjectionPassesDesc[cascade] );

				if ( m_geometryVolumes )
				{
					m_lightPropagationPassesDesc[index + 1u]->addDependency( *it->second.geometryInjectionPassesDesc[cascade] );
				}
			}

			if ( m_runnable )
			{
				m_scene.getEngine()->unregisterTimer( castor::makeString( m_runnable->getName() ) + cuT( "/Graph" )
					, m_runnable->getTimer() );
				m_runnable.reset();
				m_runnable = m_graph.compile( m_device.makeContext() );
				m_scene.getEngine()->registerTimer( castor::makeString( m_runnable->getName() ) + cuT( "/Graph" )
					, m_runnable->getTimer() );
				printGraph( *m_runnable );

				if ( m_recordEvent )
				{
					m_recordEvent->skip();
				}

				m_recordEvent = m_device.renderSystem.getEngine()->postEvent( makeGpuFunctorEvent( GpuEventType::ePreUpload
					, [this]( RenderDevice const &
						, QueueData const & )
					{
						m_runnable->record();
						m_recordEvent = nullptr;
					} ) );
			}
		}
	}

	void LayeredLightPropagationVolumesBase::update( CpuUpdater & updater )
	{
		if ( !m_initialised
			|| !m_scene.needsGlobalIllumination( m_lightType
				, ( m_geometryVolumes
					? GlobalIlluminationType::eLayeredLpvG
					: GlobalIlluminationType::eLayeredLpv ) ) )
		{
			return;
		}

		auto const & camera = *updater.camera;
		auto aabb = m_scene.getBoundingBox();
		auto camPos = camera.getParent()->getDerivedPosition();
		castor::Point3f camDir{ 0, 0, 1 };
		camera.getParent()->getDerivedOrientation().transform( camDir, camDir );
		auto changed = m_aabb != aabb
			|| m_cameraPos != camPos
			|| m_cameraDir != camDir;

		for ( auto & [light, lpv] : m_lightLpvs )
		{
			updater.light = light;
			changed = lpv.update( updater
				, { m_lpvGridConfigUbo.getUbo().getData().allMinVolumeCorners[0]->w
					, m_lpvGridConfigUbo.getUbo().getData().allMinVolumeCorners[1]->w
					, m_lpvGridConfigUbo.getUbo().getData().allMinVolumeCorners[2]->w } )
				|| changed;
		}

		if ( changed )
		{
			m_aabb = aabb;
			m_cameraPos = camPos;
			m_cameraDir = camDir;
			auto cellSize = std::max( std::max( m_aabb.getDimensions()->x
				, m_aabb.getDimensions()->y )
				, m_aabb.getDimensions()->z ) / float( m_scene.getLpvGridSize() );
			castor::Grid grid{ m_scene.getLpvGridSize(), cellSize, m_aabb.getMax(), m_aabb.getMin(), 1.0f };
			castor::Array< float, LpvMaxCascadesCount > const scales{ 1.0f, 0.65f, 0.4f };

			for ( auto i = 0u; i < LpvMaxCascadesCount; ++i )
			{
				m_grids[i] = &m_lpvGridConfigUbos[i].cpuUpdate( scales[i]
					, grid
					, m_cameraPos
					, m_cameraDir
					, m_scene.getLpvIndirectAttenuation() );
				m_gridsSizes[i] = castor::Point4f{ m_grids[i]->getCenter()->x
					, m_grids[i]->getCenter()->y
					, m_grids[i]->getCenter()->z
					, m_grids[i]->getCellSize() };
			}

			m_lpvGridConfigUbo.cpuUpdate( m_grids
				, m_scene.getLpvIndirectAttenuation() );
		}
	}

	crg::SemaphoreWaitArray LayeredLightPropagationVolumesBase::render( crg::SemaphoreWaitArray const & toWait
		, ashes::Queue const & queue )
	{
		if ( !m_initialised
			|| !m_scene.needsGlobalIllumination( m_lightType
				, ( m_geometryVolumes
					? GlobalIlluminationType::eLayeredLpvG
					: GlobalIlluminationType::eLayeredLpv ) )
			|| m_lightLpvs.empty() )
		{
			return toWait;
		}

		return m_runnable->run( toWait, queue );
	}

	void LayeredLightPropagationVolumesBase::accept( ConfigurationVisitorBase & visitor )
	{
		if ( m_initialised )
		{
			for ( auto const & [light, lpv] : m_lightLpvs )
			{
				for ( auto & lightInjectionPass : lpv.lightInjectionPasses )
				{
					if ( lightInjectionPass )
					{
						lightInjectionPass->accept( visitor );
					}
				}

				if ( m_geometryVolumes )
				{
					for ( auto & geometryInjectionPass : lpv.geometryInjectionPasses )
					{
						if ( geometryInjectionPass )
						{
							geometryInjectionPass->accept( visitor );
						}
					}
				}
			}

			for ( auto & pass : m_lightPropagationPasses )
			{
				if ( pass )
				{
					pass->accept( visitor );
				}
			}

			uint32_t layer = 0u;

			for ( auto const & injection : m_injection )
			{
				for ( auto i = 0u; i < uint32_t( LpvTexture::eCount ); ++i )
				{
					auto tex = LpvTexture( i );
					visitor.visit( cuT( "Layered LPV Injection" ) + castor::string::toString( layer ) + cuT( " " ) + getTexName( tex )
						, injection[tex]
						, m_graph.getFinalLayoutState( injection[tex].wholeViewId ).layout
						, TextureFactors::tex3D( &m_gridsSizes[i] ) );
				}

				++layer;
			}

			if ( m_geometryVolumes )
			{
				layer = 0u;

				for ( auto const & geometry : m_geometry )
				{
					visitor.visit( cuT( "Layered LPV Geometry" ) + castor::string::toString( layer )
						, geometry
						, m_graph.getFinalLayoutState( geometry.wholeViewId ).layout
						, TextureFactors::tex3D( &m_gridsSizes[layer] ) );
					++layer;
				}
			}

			uint32_t level = 0u;

			for ( auto const & propagates : m_propagate )
			{
				layer = 0u;

				for ( auto const & propagate : propagates )
				{
					for ( auto i = 0u; i < uint32_t( LpvTexture::eCount ); ++i )
					{
						auto tex = LpvTexture( i );
						visitor.visit( cuT( "Layered LPV Propagation" ) + castor::string::toString( level ) + cuT( "_" ) + castor::string::toString( layer ) + cuT( " " ) + getTexName( tex )
							, propagate[tex]
							, m_graph.getFinalLayoutState( propagate[tex].wholeViewId ).layout
							, TextureFactors::tex3D( &m_gridsSizes[i] ) );
					}

					++layer;
				}

				++level;
			}
		}
	}

	crg::FramePass & LayeredLightPropagationVolumesBase::doCreateClearInjectionPass()
	{
		auto & result = m_graph.createPass( "LLpvClearInjection"
			, []( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				return castor::make_unique< llpvpropvol::LpvClear >( pass
					, context
					, graph );
			} );

		for ( auto const & injection : m_injection )
		{
			for ( auto const & texture : injection )
			{
				result.addTransferOutputView( texture->wholeViewId );
			}
		}

		if ( m_geometryVolumes )
		{
			for ( auto const & texture : m_geometry )
			{
				result.addTransferOutputView( texture.wholeViewId );
			}
		}

		return result;
	}

	crg::FramePass & LayeredLightPropagationVolumesBase::doCreateDownsamplePass()
	{
		crg::FramePass * lastPass{ &m_clearInjectionPass };
		auto extent = m_sourceSmResult[SmTexture::eNormal].getExtent();

		for ( auto i = uint32_t( SmTexture::eNormal ); i < uint32_t( SmTexture::eCount ); ++i )
		{
			auto smTexture = SmTexture( i );
			auto & pass = m_graph.createPass( "LLpvDownsampleShadowMap/" + castor::toUtf8( getTexName( smTexture ) )
				, [extent]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & graph )
				{
					return castor::make_unique< crg::ImageBlit >( framePass
						, context
						, graph
						, VkOffset3D{}
						, extent
						, VkOffset3D{}
						, VkExtent3D{ 512u, 512u, 1u }
						, VK_FILTER_LINEAR );
				} );
			pass.addDependency( *lastPass );
			pass.addTransferInputView( m_sourceSmResult[smTexture].wholeViewId );
			pass.addTransferOutputView( m_downsampledSmResult[smTexture].wholeViewId );
			lastPass = &pass;
		}

		return *lastPass;
	}

	crg::FramePass & LayeredLightPropagationVolumesBase::doCreatePropagationPass( crg::FramePassArray const & previousPasses
		, castor::String const & name
		, LightVolumePassResult const & injection
		, LightVolumePassResult const & lpvResult
		, LightVolumePassResult const & propagation
		, uint32_t cascade
		, uint32_t index )
	{
		auto & result = m_graph.createPass( castor::toUtf8( name ) + castor::string::toMbString( cascade )
			, [this, index]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto res = castor::make_unique< LightPropagationPass >( framePass
					, context
					, runnableGraph
					, m_device
					, m_geometryVolumes && index > 0u
					, m_scene.getLpvGridSize()
					, ( index == 0u ? BlendMode::eNoBlend : BlendMode::eAdditive ) );
				m_lightPropagationPasses.emplace_back( res.get() );
				m_device.renderSystem.getEngine()->registerTimer( castor::makeString( framePass.getFullName() )
					, res->getTimer() );
				return res;
			} );

		for ( auto & previousPass : previousPasses )
		{
			result.addDependency( *previousPass );
		}

		m_lpvGridConfigUbos[cascade].createPassBinding( result
			, LightPropagationPass::LpvGridUboIdx );
		result.addSampledView( injection[LpvTexture::eR].sampledViewId
			, LightPropagationPass::RLpvGridIdx
			, crg::SamplerDesc{ VK_FILTER_LINEAR
				, VK_FILTER_LINEAR
				, VK_SAMPLER_MIPMAP_MODE_LINEAR } );
		result.addSampledView( injection[LpvTexture::eG].sampledViewId
			, LightPropagationPass::GLpvGridIdx
			, crg::SamplerDesc{ VK_FILTER_LINEAR
				, VK_FILTER_LINEAR
				, VK_SAMPLER_MIPMAP_MODE_LINEAR } );
		result.addSampledView( injection[LpvTexture::eB].sampledViewId
			, LightPropagationPass::BLpvGridIdx
			, crg::SamplerDesc{ VK_FILTER_LINEAR
				, VK_FILTER_LINEAR
				, VK_SAMPLER_MIPMAP_MODE_LINEAR } );

		if ( index > 0u && m_geometryVolumes )
		{
			result.addSampledView( m_geometry[cascade].sampledViewId
				, LightPropagationPass::GpGridIdx
				, crg::SamplerDesc{ VK_FILTER_LINEAR
					, VK_FILTER_LINEAR
					, VK_SAMPLER_MIPMAP_MODE_LINEAR } );
		}

		if ( index == 0u )
		{
			result.addOutputColourView( lpvResult[LpvTexture::eR].targetViewId );
			result.addOutputColourView( lpvResult[LpvTexture::eG].targetViewId );
			result.addOutputColourView( lpvResult[LpvTexture::eB].targetViewId );
		}
		else
		{
			result.addInOutColourView( lpvResult[LpvTexture::eR].targetViewId );
			result.addInOutColourView( lpvResult[LpvTexture::eG].targetViewId );
			result.addInOutColourView( lpvResult[LpvTexture::eB].targetViewId );
		}

		if ( index <= 1u )
		{
			result.addOutputColourView( propagation[LpvTexture::eR].targetViewId );
			result.addOutputColourView( propagation[LpvTexture::eG].targetViewId );
			result.addOutputColourView( propagation[LpvTexture::eB].targetViewId );
		}
		else
		{
			result.addInOutColourView( propagation[LpvTexture::eR].targetViewId );
			result.addInOutColourView( propagation[LpvTexture::eG].targetViewId );
			result.addInOutColourView( propagation[LpvTexture::eB].targetViewId );
		}

		return result;
	}

	castor::Vector< crg::FramePass * > LayeredLightPropagationVolumesBase::doCreatePropagationPasses()
	{
		castor::Vector< crg::FramePass * > result;
		crg::FramePassArray previousPasses;

		for ( uint32_t cascade = 0u; cascade < LpvMaxCascadesCount; ++cascade )
		{
			auto & propagate = m_propagate[cascade];
			auto const & lpvResult = *m_lpvResult[cascade];
			auto const & injection = m_injection[cascade];
			uint32_t propIndex = 0u;
			result.emplace_back( &doCreatePropagationPass( previousPasses
				, cuT( "Propagation" ) + castor::string::toString( cascade ) + cuT( "NoOccNoBlend" )
				, injection
				, lpvResult
				, propagate[propIndex]
				, cascade
				, 0u ) );
			auto previous = result.back();
			previousPasses.clear();

			if ( m_geometryVolumes )
			{
				for ( auto const & [light, lpv] : m_lightLpvs )
				{
					previousPasses.emplace_back( lpv.geometryInjectionPassesDesc[cascade] );
				}
			}

			for ( uint32_t i = 1u; i < LpvMaxPropagationSteps; ++i )
			{
				auto const & input = propagate[propIndex];
				propIndex = 1u - propIndex;
				auto const & output = propagate[propIndex];
				castor::String name = ( m_geometryVolumes
					? castor::String{ cuT( "OccBlend" ) }
					: castor::String{ cuT( "NoOccBlend" ) } );
				previousPasses.emplace_back( previous );
				result.emplace_back( &doCreatePropagationPass( previousPasses
					, cuT( "Propagation" ) + castor::string::toString( cascade ) + name + castor::string::toString( i )
					, input
					, lpvResult
					, output
					, cascade
					, i ) );
				previousPasses.clear();
				previous = result.back();
			}

			previousPasses.emplace_back( result.back() );
		}

		return result;
	}
}
