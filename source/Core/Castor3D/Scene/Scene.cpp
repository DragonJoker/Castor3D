#include "Castor3D/Scene/Scene.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Event/Frame/CpuFunctorEvent.hpp"
#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Render/RenderInfo.hpp"
#include "Castor3D/Render/RenderLoop.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/RenderWindow.hpp"
#include "Castor3D/Render/Culling/DummyCuller.hpp"
#include "Castor3D/Render/EnvironmentMap/EnvironmentMap.hpp"
#include "Castor3D/Render/EnvironmentMap/EnvironmentMapPass.hpp"
#include "Castor3D/Render/Node/BillboardRenderNode.hpp"
#include "Castor3D/Render/Node/SceneRenderNodes.hpp"
#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Animation/AnimatedObjectGroup.hpp"
#include "Castor3D/Scene/Background/BackgroundTextWriter.hpp"
#include "Castor3D/Scene/Background/Background.hpp"
#include "Castor3D/Scene/Background/Colour.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/LightFactory.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleSystem.hpp"
#include "Castor3D/Shader/LightingModelFactory.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Graphics/Font.hpp>
#include <CastorUtils/Graphics/FontCache.hpp>

CU_ImplementSmartPtr( castor3d, Scene )

namespace castor3d
{
	//*************************************************************************************************

	std::string print( castor::Point3f const & obj )
	{
		std::stringstream stream;
		stream << std::setprecision( 4 ) << obj->x
			<< ", " << std::setprecision( 4 ) << obj->y
			<< ", " << std::setprecision( 4 ) << obj->z;
		return stream.str();
	}

	std::string print( castor::BoundingBox const & obj )
	{
		std::stringstream stream;
		stream << "min: " << print( obj.getMin() ) << ", max: " << print( obj.getMax() );
		return stream.str();
	}

	//*************************************************************************************************

	template<>
	inline void CacheViewT< OverlayCache, EventType( CpuEventType::ePreGpuStep ) >::clear()
	{
		for ( auto name : m_createdElements )
		{
			auto resource = m_cache.tryRemove( name );
		}
	}

	//*************************************************************************************************

	template<>
	inline void CacheViewT< castor::FontCache, EventType( CpuEventType::ePreGpuStep ) >::clear()
	{
		for ( auto name : m_createdElements )
		{
			auto resource = m_cache.tryRemove( name );
		}
	}

	//*************************************************************************************************

	castor::String Scene::RootNode = cuT( "C3D.RootNode" );
	castor::String Scene::CameraRootNode = cuT( "C3D.CameraRootNode" );
	castor::String Scene::ObjectRootNode = cuT( "C3D.ObjectRootNode" );

	Scene::Scene( castor::String const & name, Engine & engine )
		: castor::OwnedBy< Engine >{ engine }
		, castor::Named{ name }
		, m_resources{ engine.getGraphResourceHandler() }
		, m_sceneNodeCache{ makeObjectCache< SceneNode, castor::String, SceneNodeCacheTraits >( *this
			, castor::DummyFunctorT< SceneNodeCache >{}
			, castor::DummyFunctorT< SceneNodeCache >{}
			, SceneNodeMergerT< SceneNodeCache >{ getName() }
			, SceneNodeAttacherT< SceneNodeCache >{}
			, SceneNodeDetacherT< SceneNodeCache >{} ) }
		, m_rootNode{ m_sceneNodeCache->find( RootNode ) }
		, m_rootCameraNode{ m_sceneNodeCache->find( CameraRootNode ) }
		, m_rootObjectNode{ m_sceneNodeCache->find( ObjectRootNode ) }
		, m_background{ castor::makeUniqueDerived< SceneBackground, ColourBackground >( engine, *this ) }
		, m_lightFactory{ castor::makeUnique< LightFactory >() }
		, m_listener{ engine.addNewFrameListener( cuT( "Scene_" ) + name + castor::string::toString( intptr_t( this ) ) ) }
		, m_renderNodes{ castor::makeUnique< SceneRenderNodes >( *this ) }
	{
		m_billboardCache = makeObjectCache< BillboardList, castor::String, BillboardListCacheTraits >( *this
			, m_rootNode
			, m_rootCameraNode
			, m_rootObjectNode );
		m_cameraCache = makeObjectCache< Camera, castor::String, CameraCacheTraits >( *this
			, m_rootNode
			, m_rootCameraNode
			, m_rootObjectNode
			, castor::DummyFunctorT< CameraCache >{}
			, castor::DummyFunctorT< CameraCache >{}
			, MovableMergerT< CameraCache >{ getName() }
			, MovableAttacherT< CameraCache >{}
			, MovableDetacherT< CameraCache >{} );
		m_geometryCache = makeObjectCache< Geometry, castor::String, GeometryCacheTraits >( *this
			, m_rootNode
			, m_rootCameraNode
			, m_rootObjectNode );
		m_lightCache = makeObjectCache< Light, castor::String, LightCacheTraits >( *this
			, m_rootNode
			, m_rootCameraNode
			, m_rootObjectNode );
		m_particleSystemCache = makeObjectCache< ParticleSystem, castor::String, ParticleSystemCacheTraits >( *this
			, m_rootNode
			, m_rootCameraNode
			, m_rootObjectNode
			, [this]( ParticleSystem & element )
			{
				auto & nodes = getRenderNodes();

				getListener().postEvent( makeGpuFunctorEvent( GpuEventType::ePreUpload
					, [&element, &nodes]( RenderDevice const & device
						, QueueData const & queueData )
					{
						element.initialise( device );
						auto material = element.getMaterial();

						for ( auto & pass : *material )
						{
							nodes.createNode( *pass
								, *element.getBillboards() );
						}
					} ) );
			}
			, GpuEventCleanerT< ParticleSystemCache >{ getListener() }
			, MovableMergerT< ParticleSystemCache >{ getName() }
			, MovableAttacherT< ParticleSystemCache >{}
			, MovableDetacherT< ParticleSystemCache >{} );
		m_animatedObjectGroupCache = castor::makeCache< AnimatedObjectGroup, castor::String, AnimatedObjectGroupCacheTraits >( *this );
		m_meshCache = castor::makeCache< Mesh, castor::String, MeshCacheTraits >( getLogger( engine )
			, CpuEventInitialiserT< MeshCache >{ getListener() }
			, CpuEventCleanerT< MeshCache >{ getListener() }
			, castor::ResourceMergerT< MeshCache >{ getName() } );
		m_skeletonCache = castor::makeCache< Skeleton, castor::String, SkeletonCacheTraits >( getLogger( engine )
			, castor::DummyFunctorT< SkeletonCache >{}
			, castor::DummyFunctorT< SkeletonCache >{}
			, castor::ResourceMergerT< SkeletonCache >{ getName() } );

		m_materialCacheView = makeCacheView< EventType( GpuEventType::ePreUpload ) >( getName()
			, getEngine()->getMaterialCache()
			, [this]( MaterialCache::ElementT & element )
			{
				getListener().postEvent( makeCpuInitialiseEvent( element ) );
				m_materialsListeners.emplace( &element
					, element.onChanged.connect( [this]( Material const & material )
						{
							onMaterialChanged( material );
						} ) );
				m_dirtyMaterials = true;
			}
			, [this]( MaterialCache::ElementT & element )
			{
				m_dirtyMaterials = true;
				m_materialsListeners.erase( &element );
				element.cleanup();
			} );
		m_samplerCacheView = makeCacheView< EventType( GpuEventType::ePreUpload ) >( getName()
			, getEngine()->getSamplerCache()
			, [this]( SamplerCache::ElementT & element )
			{
				element.initialise( getOwner()->getRenderSystem()->getRenderDevice() );
			}
			, []( SamplerCache::ElementT & element )
			{
				element.cleanup();
			} );
		m_overlayCache = castor::makeCache< Overlay, castor::String, OverlayCacheTraits >( *getEngine() );
		m_fontCacheView = makeCacheView< EventType( CpuEventType::ePreGpuStep ) >( getName()
			, getEngine()->getFontCache()
			, []( castor::FontCache::ElementT & element )
			{
				element.initialise();
			}
			, []( castor::FontCache::ElementT & element )
			{
				element.cleanup();
			} );

		m_animatedObjectGroupCache->add( cuT( "C3D_Textures" ), *this );
		auto & device = engine.getRenderSystem()->getRenderDevice();
		auto data = device.graphicsData();
		m_reflectionMap = castor::makeUnique< EnvironmentMap >( m_resources
			, device
			, *data
			, *this );
	}

	Scene::~Scene()
	{
		if ( m_cleanBackground )
		{
			m_cleanBackground->skip();
			m_cleanBackground = nullptr;
		}

		m_reflectionMap->cleanup();
		m_onSceneNodeChanged.disconnect();
		m_onGeometryChanged.disconnect();
		m_onBillboardListChanged.disconnect();
		m_onParticleSystemChanged.disconnect();

		m_meshCache->clear();
		m_overlayCache->clear();

		m_reflectionMap.reset();

		m_background.reset();
		m_animatedObjectGroupCache.reset();
		m_billboardCache.reset();
		m_particleSystemCache.reset();
		m_cameraCache.reset();
		m_geometryCache.reset();
		m_lightCache.reset();

		m_meshCache.reset();
		m_skeletonCache.reset();
		m_materialCacheView.reset();
		m_samplerCacheView.reset();
		m_overlayCache.reset();
		m_fontCacheView.reset();

		if ( m_rootCameraNode )
		{
			m_rootCameraNode->detach( true );
			m_rootCameraNode = {};
		}

		if ( m_rootObjectNode )
		{
			m_rootObjectNode->detach( true );
			m_rootObjectNode = {};
		}

		if ( m_rootNode )
		{
			m_rootNode->detach( true );
			m_rootNode = {};
		}

		m_sceneNodeCache.reset();
	}

	void Scene::initialise()
	{
		auto & engine = *getEngine();
		auto & device = engine.getRenderSystem()->getRenderDevice();
		m_timerParticlesGpu = castor::makeUnique< crg::FramePassTimer >( device.makeContext(), getName() + "/ParticlesGPU" );
		engine.registerTimer( getName() + "/ParticlesGPU", *m_timerParticlesGpu );
#if C3D_DebugTimers
		m_timerSceneNodes = castor::makeUnique< crg::FramePassTimer >( device.makeContext(), getName() + "/SceneNodes" );
		engine.registerTimer( getName() + "/SceneNodes", *m_timerSceneNodes );
		m_timerBoundingBox = castor::makeUnique< crg::FramePassTimer >( device.makeContext(), getName() + "/BoundingBoxes" );
		engine.registerTimer( getName() + "/BoundingBoxes", *m_timerBoundingBox );
		m_timerMaterials = castor::makeUnique< crg::FramePassTimer >( device.makeContext(), getName() + "/Materials" );
		engine.registerTimer( getName() + "/Materials", *m_timerMaterials );
		m_timerLights = castor::makeUnique< crg::FramePassTimer >( device.makeContext(), getName() + "/Lights" );
		engine.registerTimer( getName() + "/Lights", *m_timerLights );
		m_timerParticlesCpu = castor::makeUnique< crg::FramePassTimer >( device.makeContext(), getName() + "/ParticlesCPU" );
		engine.registerTimer( getName() + "/ParticlesCPU", *m_timerParticlesCpu );
		m_timerGpuUpdate = castor::makeUnique< crg::FramePassTimer >( device.makeContext(), getName() + "/GPUUpdate" );
		engine.registerTimer( getName() + "/GPUUpdate", *m_timerGpuUpdate );
		m_timerMovables = castor::makeUnique< crg::FramePassTimer >( device.makeContext(), getName() + "/Movables" );
		engine.registerTimer( getName() + "/Movables", *m_timerMovables );
#endif

		m_animatedObjectGroupCache->initialise( device );
		m_lightCache->initialise( device );
		m_background->initialise( device );
		doUpdateLightsDependent();
		updateBoundingBox();
		doUpdateMaterials();
		log::info << "Initialised scene [" << getName() << "], AABB: " << print( m_boundingBox ) << std::endl;
		m_initialised = true;
	}

	void Scene::updateBoundingBox()
	{
#if C3D_DebugTimers
		auto block( m_timerBoundingBox ? std::make_unique< crg::FramePassTimerBlock >( m_timerBoundingBox->start() ) : nullptr );
#endif
		auto & cache = *m_geometryCache;
		auto lock( castor::makeUniqueLock( cache ) );

		if ( !cache.isEmptyNoLock() )
		{
			constexpr float fmin = std::numeric_limits< float >::max();
			constexpr float fmax = std::numeric_limits< float >::lowest();
			castor::Point3f min{ fmin, fmin, fmin };
			castor::Point3f max{ fmax, fmax, fmax };

			for ( auto & geomIt : cache )
			{
				auto & geometry = *geomIt.second;
				auto node = geometry.getParent();
				auto mesh = geometry.getMesh();

				if ( node && mesh )
				{
					auto bbox = mesh->getBoundingBox().getAxisAligned( node->getDerivedTransformationMatrix() );

					for ( auto i = 0u; i < 3u; ++i )
					{
						min[i] = std::min( min[i], bbox.getMin()[i] );
						max[i] = std::max( max[i], bbox.getMax()[i] );
					}
				}
			}

			m_boundingBox.load( min, max );
		}
		else
		{
			m_boundingBox = castor::BoundingBox{};
		}
	}

	void Scene::cleanup()
	{
		m_initialised = false;
		m_dirtyNodes.clear();
		m_dirtyBillboards.clear();
		m_dirtyObjects.clear();

		m_animatedObjectGroupCache->cleanup();
		m_geometryCache->cleanup();
		m_cameraCache->cleanup();
		m_lightCache->cleanup();
		m_billboardCache->cleanup();
		m_particleSystemCache->cleanup();
		m_sceneNodeCache->cleanup();
		m_skeletonCache->cleanup();
		m_overlayCache->cleanup();

		m_materialCacheView->clear();
		m_samplerCacheView->clear();
		m_fontCacheView->clear();

		m_renderNodes->clear();

		m_meshCache->cleanup();

		if ( m_cleanBackground )
		{
			m_cleanBackground->skip();
			m_cleanBackground = nullptr;
		}

		m_cleanBackground = getListener().postEvent( makeCpuFunctorEvent( CpuEventType::ePreGpuStep
			, [this]()
			{
				m_background->cleanup();
				m_cleanBackground = nullptr;
			} ) );

		auto & engine = *getEngine();
#if C3D_DebugTimers
		engine.unregisterTimer( getName() + "/SceneNodes", *m_timerSceneNodes );
		m_timerSceneNodes.reset();
		engine.unregisterTimer( getName() + "/BoundingBoxes", *m_timerBoundingBox );
		m_timerBoundingBox.reset();
		engine.unregisterTimer( getName() + "/Materials", *m_timerMaterials );
		m_timerMaterials.reset();
		engine.unregisterTimer( getName() + "/Lights", *m_timerLights );
		m_timerLights.reset();
		engine.unregisterTimer( getName() + "/ParticlesCPU", *m_timerParticlesCpu );
		m_timerParticlesCpu.reset();
		engine.unregisterTimer( getName() + "/GPUUpdate", *m_timerGpuUpdate );
		m_timerGpuUpdate.reset();
		engine.unregisterTimer( getName() + "/Movables", *m_timerMovables );
		m_timerMovables.reset();
#endif
		engine.unregisterTimer( getName() + "/ParticlesGPU", *m_timerParticlesGpu );
		m_timerParticlesGpu.reset();

		{
			auto lock( castor::makeUniqueLock( getEngine()->getRenderTargetCache() ) );
			for ( auto & target : getEngine()->getRenderTargetCache().getRenderTargets( TargetType::eTexture ) )
			{
				if ( target->getScene() == this )
				{
					target->cleanup( *getEngine()->getRenderDevice() );
				}
			}

			for ( auto & target : getEngine()->getRenderTargetCache().getRenderTargets( TargetType::eWindow ) )
			{
				if ( target->getScene() == this )
				{
					target->cleanup( *getEngine()->getRenderDevice() );
				}
			}
		}
	}

	void Scene::update( CpuUpdater & updater )
	{
		if ( m_initialised )
		{
			onUpdate( *this );
			updater.scene = this;
			auto & sceneObjs = updater.dirtyScenes.emplace( this, CpuUpdater::DirtyObjects{} ).first->second;
			doGatherDirty( sceneObjs );
			doUpdateSceneNodes( updater, sceneObjs );
			m_animatedObjectGroupCache->update( updater );
			doUpdateMovables( updater, sceneObjs );
			updateBoundingBox();
			doUpdateMaterials();
			doUpdateLights( updater, sceneObjs );
			m_renderNodes->update( updater );
			doUpdateParticles( updater, sceneObjs );
			doUpdateLightsDependent();
			m_changed = false;
		}
	}

	void Scene::upload( UploadData & uploader )
	{
		getLightCache().upload( uploader );
		m_meshCache->forEach( [&uploader]( Mesh & mesh )
			{
				for ( auto & submesh : mesh )
				{
					submesh->upload( uploader );
				}
			} );
		m_background->upload( uploader );
	}

	void Scene::update( GpuUpdater & updater )
	{
#if C3D_DebugTimers
		auto block( m_timerGpuUpdate->start() );
#endif
		updater.scene = this;
		updater.info.total.lightsCount += getLightCache().getLightsBufferCount( LightType::eDirectional );
		updater.info.total.lightsCount += getLightCache().getLightsBufferCount( LightType::eSpot );
		updater.info.total.lightsCount += getLightCache().getLightsBufferCount( LightType::ePoint );
		doUpdateParticles( updater );
		m_renderNodes->update( updater );
	}

	void Scene::setBackground( SceneBackgroundUPtr value )
	{
		m_background = std::move( value );
		m_background->initialise( getEngine()->getRenderSystem()->getRenderDevice() );
		onSetBackground( *m_background );
	}

	void Scene::merge( Scene & scene )
	{
		scene.getAnimatedObjectGroupCache().mergeInto( *m_animatedObjectGroupCache );
		scene.getCameraCache().mergeInto( *m_cameraCache );
		scene.getBillboardListCache().mergeInto( *m_billboardCache );
		scene.getParticleSystemCache().mergeInto( *m_particleSystemCache );
		scene.getGeometryCache().mergeInto( *m_geometryCache );
		scene.getLightCache().mergeInto( *m_lightCache );
		scene.getSceneNodeCache().mergeInto( *m_sceneNodeCache );
		m_ambientLight = scene.getAmbientLight();
		scene.cleanup();
	}

	uint32_t Scene::getVertexCount()const
	{
		uint32_t result = 0;
		using LockType = std::unique_lock< GeometryCache >;
		LockType lock{ castor::makeUniqueLock( *m_geometryCache ) };

		for ( auto & pair : *m_geometryCache )
		{
			if ( auto mesh = pair.second->getMesh() )
			{
				result += mesh->getVertexCount();
			}
		}

		return result;
	}

	uint32_t Scene::getFaceCount()const
	{
		uint32_t result = 0;
		using LockType = std::unique_lock< GeometryCache >;
		LockType lock{ castor::makeUniqueLock( *m_geometryCache ) };

		for ( auto & pair : *m_geometryCache )
		{
			if ( auto mesh = pair.second->getMesh() )
			{
				result += mesh->getFaceCount();
			}
		}

		return result;
	}

	SceneFlags Scene::getFlags()const
	{
		SceneFlags result{};

		switch ( m_fog.getType() )
		{
		case FogType::eLinear:
			result |= SceneFlag::eFogLinear;
			break;

		case FogType::eExponential:
			result |= SceneFlag::eFogExponential;
			break;

		case FogType::eSquaredExponential:
			result |= SceneFlag::eFogSquaredExponential;
			break;

		default:
			break;
		}

		if ( m_hasShadows[size_t( LightType::eDirectional )] )
		{
			result |= SceneFlag::eShadowDirectional;
		}

		if ( m_hasShadows[size_t( LightType::ePoint )] )
		{
			result |= SceneFlag::eShadowPoint;
		}

		if ( m_hasShadows[size_t( LightType::eSpot )] )
		{
			result |= SceneFlag::eShadowSpot;
		}

		if ( m_voxelConfig.enabled )
		{
			result |= SceneFlag::eVoxelConeTracing;
		}
		else
		{
			if ( needsGlobalIllumination( LightType::eDirectional, GlobalIlluminationType::eLpv )
				|| needsGlobalIllumination( LightType::eDirectional, GlobalIlluminationType::eLpvG )
				|| needsGlobalIllumination( LightType::ePoint, GlobalIlluminationType::eLpv )
				|| needsGlobalIllumination( LightType::ePoint, GlobalIlluminationType::eLpvG )
				|| needsGlobalIllumination( LightType::eSpot, GlobalIlluminationType::eLpv )
				|| needsGlobalIllumination( LightType::eSpot, GlobalIlluminationType::eLpvG ) )
			{
				result |= SceneFlag::eLpvGI;
			}

			if ( needsGlobalIllumination( LightType::eDirectional, GlobalIlluminationType::eLayeredLpv )
				|| needsGlobalIllumination( LightType::eDirectional, GlobalIlluminationType::eLayeredLpvG )
				|| needsGlobalIllumination( LightType::ePoint, GlobalIlluminationType::eLayeredLpv )
				|| needsGlobalIllumination( LightType::ePoint, GlobalIlluminationType::eLayeredLpvG )
				|| needsGlobalIllumination( LightType::eSpot, GlobalIlluminationType::eLayeredLpv )
				|| needsGlobalIllumination( LightType::eSpot, GlobalIlluminationType::eLayeredLpvG ) )
			{
				if ( getEngine()->getRenderSystem()->hasLLPV() )
				{
					result |= SceneFlag::eLayeredLpvGI;
				}
				else
				{
					result |= SceneFlag::eLpvGI;
				}
			}
		}

		return result;
	}

	bool Scene::hasShadows()const
	{
		return m_hasAnyShadows;
	}

	bool Scene::hasShadows( LightType lightType )const
	{
		return m_hasShadows[size_t( lightType )];
	}

	void Scene::addEnvironmentMap( SceneNode & node )
	{
		m_reflectionMap->addNode( node );
		markDirty( node );
	}

	void Scene::removeEnvironmentMap( SceneNode & node )
	{
		m_reflectionMap->removeNode( node );
		markDirty( node );
	}

	bool Scene::hasEnvironmentMap( SceneNode & node )const
	{
		return m_reflectionMap->hasNode( node );
	}

	EnvironmentMap & Scene::getEnvironmentMap()const
	{
		return *m_reflectionMap;
	}

	uint32_t Scene::getEnvironmentMapIndex( SceneNode const & node )const
	{
		return m_reflectionMap->getIndex( node );
	}

	castor::String const & Scene::getBackgroundModel()const
	{
		return m_background->getModelName();
	}

	std::vector< LightingModelID > Scene::getLightingModelsID()const
	{
		return getEngine()->getLightingModelFactory().getLightingModelsID( m_background->getModelID() );
	}

	AnimatedObjectRPtr Scene::addAnimatedTexture( TextureSourceInfo const & sourceInfo
		, TextureConfiguration const & config
		, Pass & pass )
	{
		auto & cache = getAnimatedObjectGroupCache();
		auto group = cache.find( cuT( "C3D_Textures" ) );
		return group->addObject( sourceInfo, config, pass );
	}

	void Scene::markDirty( SceneNode & node )
	{
		if ( !node.isDisplayable() )
		{
			return;
		}

		std::vector< SceneNode * > work;
		work.push_back( &node );

		while ( !work.empty() )
		{
			auto & curNode = *work.back();
			work.pop_back();
			auto it = std::find( m_dirtyNodes.begin()
				, m_dirtyNodes.end()
				, &curNode );

			if ( it != m_dirtyNodes.end() )
			{
				m_dirtyNodes.erase( it );
			}

			auto parent = curNode.getParent();

			if ( !parent )
			{
				it = m_dirtyNodes.begin();
			}
			else
			{
				do
				{
					it = std::find( m_dirtyNodes.begin()
						, m_dirtyNodes.end()
						, parent );
					parent = parent->getParent();

				}
				while ( parent && it == m_dirtyNodes.end() );

				if ( it == m_dirtyNodes.end() )
				{
					it = m_dirtyNodes.begin();
				}
				else
				{
					it = std::next( it );
				}
			}

			m_dirtyNodes.insert( it, &curNode );

			for ( auto & object : curNode.getObjects() )
			{
				markDirty( object.get() );
			}

			for ( auto child : curNode.getChildren() )
			{
				if ( auto nd = child.second )
				{
					work.push_back( nd );
				}
			}
		}
	}

	void Scene::markDirty( BillboardBase & object )
	{
		if ( !object.getNode()->isDisplayable() )
		{
			return;
		}

		auto it = std::find( m_dirtyBillboards.begin()
			, m_dirtyBillboards.end()
			, &object );

		if ( it == m_dirtyBillboards.end() )
		{
			m_dirtyBillboards.emplace_back( &object );
		}
	}

	void Scene::markDirty( MovableObject & object )
	{
		if ( !object.getParent()->isDisplayable() )
		{
			return;
		}

		auto it = std::find( m_dirtyObjects.begin()
			, m_dirtyObjects.end()
			, &object );

		if ( it == m_dirtyObjects.end() )
		{
			m_dirtyObjects.emplace_back( &object );
		}
	}

	BackgroundModelID Scene::getBackgroundModelId()const
	{
		return m_background
			? m_background->getModelID()
			: BackgroundModelID( 0u );
	}

	LightingModelID Scene::getDefaultLightingModel()const
	{
		return getEngine()->getDefaultLightingModel();
	}

	castor::String Scene::getDefaultLightingModelName()const
	{
		return getEngine()->getDefaultLightingModelName();
	}

	bool Scene::needsGlobalIllumination()const
	{
		return m_needsGlobalIllumination;
	}

	bool Scene::needsGlobalIllumination( LightType ltType
		, GlobalIlluminationType giType )const
	{
		return m_giTypes[size_t( ltType )].end() != m_giTypes[size_t( ltType )].find( giType );
	}

	void Scene::setDefaultLightingModel( LightingModelID value )
	{
		getEngine()->setDefaultLightingModel( value );
	}

	crg::SemaphoreWaitArray Scene::getRenderTargetsSemaphores()const
	{
		crg::SemaphoreWaitArray result;
		auto lock( castor::makeUniqueLock( getEngine()->getRenderTargetCache() ) );

		for ( auto & target : getEngine()->getRenderTargetCache().getRenderTargets( TargetType::eTexture ) )
		{
			if ( target->getScene() != this )
			{
				auto toWait = target->getSemaphore();
				result.insert( result.end()
					, toWait.begin()
					, toWait.end() );
			}
		}

		return result;
	}

	uint32_t Scene::getLpvGridSize()const
	{
		return getEngine()->getLpvGridSize();
	}

	bool Scene::hasBindless()const
	{
		auto device = getEngine()->getRenderDevice();
		CU_Require( getEngine()->getRenderDevice() );
		return device->hasBindless();
	}

	ashes::DescriptorSetLayout * Scene::getBindlessTexDescriptorLayout()const
	{
		return getEngine()->getTextureUnitCache().getDescriptorLayout();
	}

	ashes::DescriptorPool * Scene::getBindlessTexDescriptorPool()const
	{
		return getEngine()->getTextureUnitCache().getDescriptorPool();
	}

	ashes::DescriptorSet * Scene::getBindlessTexDescriptorSet()const
	{
		return getEngine()->getTextureUnitCache().getDescriptorSet();
	}

	ashes::Buffer< ModelBufferConfiguration > const & Scene::getModelBuffer()const
	{
		CU_Require( m_renderNodes );
		return m_renderNodes->getModelBuffer();
	}

	ashes::Buffer< BillboardUboConfiguration > const & Scene::getBillboardsBuffer()const
	{
		CU_Require( m_renderNodes );
		return m_renderNodes->getBillboardsBuffer();
	}

	bool Scene::hasObjects( LightingModelID lightingModelId )const
	{
		return m_renderNodes->hasNodes( lightingModelId );
	}

	bool Scene::hasIBLSupport( LightingModelID lightingModelId )const
	{
		return getEngine()->getPassFactory().hasIBLSupport( lightingModelId );
	}

	void Scene::setDirectionalShadowCascades( uint32_t value )
	{
		CU_Require( value <= MaxDirectionalCascadesCount );
		m_directionalShadowCascades = value;
	}

	void Scene::setLpvIndirectAttenuation( float value )
	{
		m_lpvIndirectAttenuation = value;
	}

	void Scene::doGatherDirty( CpuUpdater::DirtyObjects & sceneObjs )
	{
		sceneObjs.dirtyNodes.insert( sceneObjs.dirtyNodes.end()
			, m_dirtyNodes.begin()
			, m_dirtyNodes.end() );

		for ( auto movable : m_dirtyObjects )
		{
			switch ( movable->getMovableType() )
			{
			case MovableType::eGeometry:
				sceneObjs.dirtyGeometries.emplace_back( static_cast< Geometry * >( movable ) );
				break;
			case MovableType::eBillboard:
				markDirty( *static_cast< BillboardBase * >( static_cast< BillboardList * >( movable ) ) );
				break;
			case MovableType::eParticleEmitter:
				if ( auto billboards = static_cast< ParticleSystem & >( *movable ).getBillboards() )
				{
					markDirty( *billboards );
				}
				break;
			case MovableType::eLight:
				sceneObjs.dirtyLights.emplace_back( static_cast< Light * >( movable ) );
				break;
			case MovableType::eCamera:
				sceneObjs.dirtyCameras.emplace_back( static_cast< Camera * >( movable ) );
				break;
			default:
				break;
			}
		}

		if ( !sceneObjs.dirtyCameras.empty() )
		{
			for ( auto & light : getLightCache() )
			{
				if ( light.second->getLightType() == LightType::eDirectional )
				{
					sceneObjs.dirtyLights.push_back( light.second.get() );
				}
			}
		}

		sceneObjs.dirtyBillboards.insert( sceneObjs.dirtyBillboards.end()
			, m_dirtyBillboards.begin()
			, m_dirtyBillboards.end() );
		m_dirtyBillboards.clear();
		m_dirtyObjects.clear();
		m_dirtyNodes.clear();
	}

	void Scene::doUpdateSceneNodes( CpuUpdater & updater
		, CpuUpdater::DirtyObjects & sceneObjs )
	{
#if C3D_DebugTimers
		auto block( m_timerSceneNodes->start() );
#endif

		for ( auto & node : sceneObjs.dirtyNodes )
		{
			node->update();
		}
	}

	void Scene::doUpdateMovables( CpuUpdater & updater
		, CpuUpdater::DirtyObjects & sceneObjs )
	{
#if C3D_DebugTimers
		auto block( m_timerMovables->start() );
#endif

		for ( auto camera : sceneObjs.dirtyCameras )
		{
			camera->update();
		}

		for ( auto object : sceneObjs.dirtyGeometries )
		{
			bool dirty = false;
			auto & geometry = *object;

			for ( auto & passIt : geometry.getIds() )
			{
				for ( auto & submeshIt : passIt.second )
				{
					auto & submesh = submeshIt.second.second->data;

					if ( submesh.isInitialised() )
					{
						geometry.fillEntry( submeshIt.second.first
							, *passIt.first
							, *geometry.getParent()
							, submesh.getMeshletsCount()
							, submeshIt.second.second->modelData );
						geometry.fillEntryOffsets( submeshIt.second.first
							, submesh.getVertexOffset( geometry )
							, submesh.getIndexOffset()
							, submesh.getMeshletOffset() );
					}
					else
					{
						markDirty( geometry );
					}
				}

				dirty = dirty || passIt.first->getId() == 0;
			}

			if ( dirty )
			{
				markDirty( geometry );
			}
		}

		for ( auto object : sceneObjs.dirtyBillboards )
		{
			bool dirty = false;

			for ( auto & billboardIt : object->getIds() )
			{
				object->fillEntry( billboardIt.second.first
					, *billboardIt.first
					, *object->getNode()
					, 0u
					, billboardIt.second.second->modelData );
				object->fillEntryOffsets( billboardIt.second.first
					, 0u
					, 0u
					, 0u );
				object->fillData( billboardIt.second.second->billboardData );
				dirty = dirty || billboardIt.first->getId() == 0;
			}

			if ( dirty )
			{
				markDirty( *object );
			}
		}
	}

	void Scene::doUpdateLights( CpuUpdater & updater
		, CpuUpdater::DirtyObjects & sceneObjs )
	{
#if C3D_DebugTimers
		auto block( m_timerLights->start() );
#endif

		for ( auto light : sceneObjs.dirtyLights )
		{
			doUpdateLightDependent( light->getLightType()
				, light->isShadowProducer()
				, light->getExpectedGlobalIlluminationType() );
		}

		m_lightCache->update( updater );
	}

	void Scene::doUpdateParticles( CpuUpdater & updater
		, CpuUpdater::DirtyObjects & sceneObjs )
	{
#if C3D_DebugTimers
		auto block( m_timerParticlesCpu->start() );
#endif
		auto & cache = getParticleSystemCache();
		auto lock( castor::makeUniqueLock( cache ) );
		updater.index = 0u;

		for ( auto & particleSystem : cache )
		{
			particleSystem.second->update( updater );
		}
	}

	void Scene::doUpdateParticles( GpuUpdater & updater )
	{
		auto & cache = getParticleSystemCache();
		auto lock( castor::makeUniqueLock( cache ) );

		if ( !m_timerParticlesGpu )
		{
			return;
		}

		auto timerBlock( m_timerParticlesGpu->start() );
		updater.index = 0u;
		updater.timer = m_timerParticlesGpu.get();

		for ( auto & particleSystem : cache )
		{
			particleSystem.second->update( updater );
		}
	}

	void Scene::doUpdateMaterials()
	{
#if C3D_DebugTimers
		auto block( m_timerMaterials->start() );
#endif

		if ( m_dirtyMaterials )
		{
			auto & cache = getEngine()->getMaterialCache();
			cache.lock();
			m_needsSubsurfaceScattering = false;
			m_hasTransparentObjects = false;
			m_hasOpaqueObjects = false;

			for ( auto & matName : *m_materialCacheView )
			{
				if ( cache.hasNoLock( matName ) )
				{
					if ( auto material = cache.findNoLock( matName ) )
					{
						m_needsSubsurfaceScattering |= material->hasSubsurfaceScattering();

						for ( auto & pass : *material )
						{
							m_hasTransparentObjects |= pass->hasAlphaBlending();
							m_hasOpaqueObjects |= !pass->hasOnlyAlphaBlending();
						}
					}
				}
			}

			cache.unlock();
			m_dirtyMaterials = false;
		}
	}

	bool Scene::doUpdateLightDependent( LightType lightType
		, bool shadowProducer
		, GlobalIlluminationType globalIllumination )
	{
		auto changed = m_hasShadows[size_t( lightType )] != shadowProducer
			|| ( globalIllumination != GlobalIlluminationType::eNone
				&& m_giTypes[size_t( lightType )].find( globalIllumination ) == m_giTypes[size_t( lightType )].end() );

		if ( changed )
		{
			m_hasShadows[size_t( lightType )] = shadowProducer;

			if ( globalIllumination != GlobalIlluminationType::eNone )
			{
				m_giTypes[size_t( lightType )].insert( globalIllumination );
			}
		}

		return changed;
	}

	bool Scene::doUpdateLightsDependent()
	{
		bool needsGI = false;
		bool hasAnyShadows = false;
		std::array< bool, size_t( LightType::eCount ) > hasShadows{};
		std::array< std::set< GlobalIlluminationType >, size_t( LightType::eCount ) > giTypes{};

		m_lightCache->forEach( [&giTypes, &needsGI, &hasAnyShadows, &hasShadows]( Light const & light )
			{
				if ( light.getExpectedGlobalIlluminationType() != GlobalIlluminationType::eNone )
				{
					giTypes[uint32_t( light.getLightType() )].insert( light.getExpectedGlobalIlluminationType() );
					needsGI = true;
				}

				if ( light.isExpectedShadowProducer() )
				{
					hasAnyShadows = true;
					hasShadows[size_t( light.getLightType() )] = true;
				}
			} );

		size_t i = 0u;
		bool changed = false;
		for ( auto & shadows : m_hasShadows )
		{
			changed = shadows.exchange( hasShadows[i] ) != hasShadows[i]
				|| changed;
			++i;
		}

		i = 0u;
		for ( auto & types : m_giTypes )
		{
			changed = types != giTypes[i]
				|| changed;
			types = giTypes[i];
			++i;
		}

		if ( !needsGI )
		{
			needsGI = getVoxelConeTracingConfig().enabled;
		}

		changed = m_hasAnyShadows.exchange( hasAnyShadows ) != hasAnyShadows
			|| changed;
		changed = m_needsGlobalIllumination.exchange( needsGI ) != needsGI
			|| changed;
		return changed;
	}

	void Scene::onMaterialChanged( Material const & material )
	{
		m_dirtyMaterials = true;
	}
}
