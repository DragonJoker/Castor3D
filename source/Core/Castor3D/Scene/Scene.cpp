#include "Castor3D/Scene/Scene.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Event/Frame/CpuFunctorEvent.hpp"
#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Render/RenderLoop.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/RenderWindow.hpp"
#include "Castor3D/Render/EnvironmentMap/EnvironmentMap.hpp"
#include "Castor3D/Render/EnvironmentMap/EnvironmentMapPass.hpp"
#include "Castor3D/Render/Node/SceneRenderNodes.hpp"
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
#include "Castor3D/Shader/Shaders/SdwModule.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Graphics/Font.hpp>
#include <CastorUtils/Graphics/FontCache.hpp>

namespace castor3d
{
	//*************************************************************************************************

	namespace
	{
		template< typename ObjType, typename ViewType >
		bool writeView( ViewType const & view
			, castor::String const & elemsName
			, castor::String const & tabs
			, castor::TextFile & file )
		{
			bool result = true;

			if ( !view.isEmpty() )
			{
				result = file.writeText( cuT( "\n" ) + tabs + cuT( "\t// " ) + elemsName + cuT( "\n" ) ) > 0;
				log::info << cuT( "Scene::write - " ) << elemsName << std::endl;

				for ( auto const & name : view )
				{
					if ( result )
					{
						auto elem = view.find( name );
						result = typename ObjType::TextWriter{ tabs + cuT( "\t" ) }( *elem, file );
					}
				}
			}

			return result;
		}

		std::ostream & operator<<( std::ostream & stream, castor::Point3f const & obj )
		{
			stream << std::setprecision( 4 ) << obj->x
				<< ", " << std::setprecision( 4 ) << obj->y
				<< ", " << std::setprecision( 4 ) << obj->z;
			return stream;
		}

		std::ostream & operator<<( std::ostream & stream, castor::BoundingBox const & obj )
		{
			stream << "min: " << obj.getMin() << ", max: " << obj.getMax();
			return stream;
		}
	}

	//*************************************************************************************************

	template<>
	inline void CacheViewT< OverlayCache, EventType::ePreRender >::clear()
	{
		for ( auto name : m_createdElements )
		{
			auto resource = m_cache.tryRemove( name );
		}
	}

	//*************************************************************************************************

	template<>
	inline void CacheViewT< castor::FontCache, EventType::ePreRender >::clear()
	{
		for ( auto name : m_createdElements )
		{
			auto resource = m_cache.tryRemove( name );
		}
	}

	//*************************************************************************************************

	castor::String Scene::RootNode = cuT( "RootNode" );
	castor::String Scene::CameraRootNode = cuT( "CameraRootNode" );
	castor::String Scene::ObjectRootNode = cuT( "ObjectRootNode" );

	Scene::Scene( castor::String const & name, Engine & engine )
		: castor::OwnedBy< Engine >{ engine }
		, castor::Named{ name }
		, m_rootNode{ std::make_shared< SceneNode >( RootNode, *this ) }
		, m_rootCameraNode{ std::make_shared< SceneNode >( CameraRootNode, *this ) }
		, m_rootObjectNode{ std::make_shared< SceneNode >( ObjectRootNode, *this ) }
		, m_background{ std::make_shared< ColourBackground >( engine, *this ) }
		, m_lightFactory{ std::make_shared< LightFactory >() }
		, m_listener{ engine.getFrameListenerCache().add( cuT( "Scene_" ) + name + castor::string::toString( intptr_t( this ) ) ) }
		, m_animationUpdater{ std::min( 4u, std::max( 2u, engine.getCpuInformations().getCoreCount() / 2u ) ) }
		, m_renderNodes{ castor::makeUnique< SceneRenderNodes >( *this ) }
	{
		m_rootCameraNode->attachTo( *m_rootNode );
		m_rootObjectNode->attachTo( *m_rootNode );

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
			, GpuEventInitialiserT< ParticleSystemCache >{ getListener() }
			, GpuEventCleanerT< ParticleSystemCache >{ getListener() }
			, MovableMergerT< ParticleSystemCache >{ getName() }
			, MovableAttacherT< ParticleSystemCache >{}
			, MovableDetacherT< ParticleSystemCache >{} );
		m_sceneNodeCache = makeObjectCache< SceneNode, castor::String, SceneNodeCacheTraits >( *this
			, m_rootNode
			, m_rootCameraNode
			, m_rootObjectNode
			, castor::DummyFunctorT< SceneNodeCache >{}
			, castor::DummyFunctorT< SceneNodeCache >{}
			, MovableMergerT< SceneNodeCache >{ getName() }
			, SceneNodeAttacherT< SceneNodeCache >{}
			, SceneNodeDetacherT< SceneNodeCache >{} );
		m_animatedObjectGroupCache = castor::makeCache< AnimatedObjectGroup, castor::String, AnimatedObjectGroupCacheTraits >( *this );
		m_meshCache = castor::makeCache< Mesh, castor::String, MeshCacheTraits >( getLogger( engine )
			, CpuEventInitialiserT< MeshCache >{ getListener() }
			, CpuEventCleanerT< MeshCache >{ getListener() }
			, castor::ResourceMergerT< MeshCache >{ getName() } );

		m_materialCacheView = makeCacheView< EventType::ePreRender >( getName()
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
		m_samplerCacheView = makeCacheView< EventType::ePreRender >( getName()
			, getEngine()->getSamplerCache()
			, [this]( SamplerCache::ElementT & element )
			{
				element.initialise( getOwner()->getRenderSystem()->getRenderDevice() );
			}
			, []( SamplerCache::ElementT & element )
			{
				element.cleanup();
			} );
		m_overlayCacheView = makeCacheView< EventType::ePreRender >( getName()
			, getEngine()->getOverlayCache()
			, [this]( OverlayCache::ElementT & element )
			{
				getEngine()->getOverlayCache().initialise( element );
			}
			, [this]( OverlayCache::ElementT & element )
			{
				getEngine()->getOverlayCache().cleanup( element );
			} );
		m_fontCacheView = makeCacheView< EventType::ePreRender >( getName()
			, getEngine()->getFontCache()
			, []( castor::FontCache::ElementT & element )
			{
				element.initialise();
			}
			, []( castor::FontCache::ElementT & element )
			{
				element.cleanup();
			} );

		auto node = m_rootNode;
		m_sceneNodeCache->add( cuT( "RootNode" ), node );
		node = m_rootCameraNode;
		m_sceneNodeCache->add( cuT( "ObjectRootNode" ), node );
		node = m_rootCameraNode;
		m_sceneNodeCache->add( cuT( "CameraRootNode" ), node );
		auto setThisChanged = [this]()
		{
			setChanged();
		};
		m_onParticleSystemChanged = m_particleSystemCache->onChanged.connect( setThisChanged );
		m_onBillboardListChanged = m_billboardCache->onChanged.connect( setThisChanged );
		m_onGeometryChanged = m_geometryCache->onChanged.connect( setThisChanged );
		m_onSceneNodeChanged = m_sceneNodeCache->onChanged.connect( setThisChanged );
		m_animatedObjectGroupCache->add( cuT( "C3D_Textures" ), *this );
		auto & device = engine.getRenderSystem()->getRenderDevice();
		auto data = device.graphicsData();
		m_reflectionMap = std::make_unique< EnvironmentMap >( engine.getGraphResourceHandler()
			, device
			, *data
			, *this );
	}

	Scene::~Scene()
	{
		m_reflectionMap.reset();
		m_onSceneNodeChanged.disconnect();
		m_onGeometryChanged.disconnect();
		m_onBillboardListChanged.disconnect();
		m_onParticleSystemChanged.disconnect();

		m_meshCache->clear();

		m_reflectionMap.reset();

		m_background.reset();
		m_animatedObjectGroupCache.reset();
		m_billboardCache.reset();
		m_particleSystemCache.reset();
		m_cameraCache.reset();
		m_geometryCache.reset();
		m_lightCache.reset();

		if ( m_rootCameraNode )
		{
			m_rootCameraNode->detach();
			m_rootCameraNode.reset();
		}

		if ( m_rootObjectNode )
		{
			m_rootObjectNode->detach();
			m_rootObjectNode.reset();
		}

		m_sceneNodeCache.reset();

		m_meshCache.reset();
		m_materialCacheView.reset();
		m_samplerCacheView.reset();
		m_overlayCacheView.reset();
		m_fontCacheView.reset();

		if ( m_rootNode )
		{
			m_rootNode->detach();
		}

		m_rootNode.reset();
		getEngine()->getFrameListenerCache().remove( m_listener.lock()->getName() );
	}

	void Scene::initialise()
	{
		m_lightCache->initialise( getEngine()->getRenderSystem()->getRenderDevice() );
		m_background->initialise( getEngine()->getRenderSystem()->getRenderDevice() );
		doUpdateLightsDependent();
		doUpdateBoundingBox();
		log::info << "Initialised scene [" << getName() << "], AABB: " << m_boundingBox << std::endl;
		m_initialised = true;
	}

	void Scene::cleanup()
	{
		m_initialised = false;
		m_animatedObjectGroupCache->cleanup();
		m_cameraCache->cleanup();
		m_particleSystemCache->cleanup();
		m_billboardCache->cleanup();
		m_geometryCache->cleanup();
		m_lightCache->cleanup();
		m_reflectionMap->cleanup();
		m_sceneNodeCache->cleanup();

		m_materialCacheView->clear();
		m_samplerCacheView->clear();
		m_overlayCacheView->clear();
		m_fontCacheView->clear();

		// These ones, being ResourceCache, need to be cleared in destructor only
		m_meshCache->cleanup();

		getListener().postEvent( makeGpuFunctorEvent( EventType::ePreRender
			, [this]( RenderDevice const & device
				, QueueData const & queueData )
			{
				m_background->cleanup( device );
			} ) );
	}

	void Scene::update( CpuUpdater & updater )
	{
		if ( m_initialised )
		{
			updater.scene = this;
			m_rootNode->update();
			doUpdateBoundingBox();
			doUpdateAnimations( updater );
			doUpdateMaterials();
			m_lightCache->update( updater );
			m_geometryCache->update( updater );
			m_billboardCache->update( updater );
			m_animatedObjectGroupCache->update( updater );
			onUpdate( *this );
			m_changed = false;
		}
	}

	void Scene::update( GpuUpdater & updater )
	{
		updater.scene = this;
		m_lightCache->update( updater );
		m_meshCache->forEach( []( Mesh & mesh )
			{
				for ( auto & submesh : mesh )
				{
					submesh->update();
				}
			} );

		if ( doUpdateLightsDependent() )
		{
			onChanged( *this );
		}
	}

	void Scene::setBackground( SceneBackgroundSPtr value )
	{
		m_background = std::move( value );
		m_background->initialise( getEngine()->getRenderSystem()->getRenderDevice() );
		onSetBackground( *m_background );
	}

	void Scene::merge( SceneSPtr scene )
	{
		scene->getAnimatedObjectGroupCache().mergeInto( *m_animatedObjectGroupCache );
		scene->getCameraCache().mergeInto( *m_cameraCache );
		scene->getBillboardListCache().mergeInto( *m_billboardCache );
		scene->getParticleSystemCache().mergeInto( *m_particleSystemCache );
		scene->getGeometryCache().mergeInto( *m_geometryCache );
		scene->getLightCache().mergeInto( *m_lightCache );
		scene->getSceneNodeCache().mergeInto( *m_sceneNodeCache );
		m_ambientLight = scene->getAmbientLight();
		setChanged();

		scene->cleanup();
	}

	bool Scene::importExternal( castor::Path const & fileName, SceneImporter & importer )
	{
		setChanged();
		//return importer.importScene( *this, fileName, Parameters() );
		return false;
	}

	uint32_t Scene::getVertexCount()const
	{
		uint32_t result = 0;
		using LockType = std::unique_lock< GeometryCache >;
		LockType lock{ castor::makeUniqueLock( *m_geometryCache ) };

		for ( auto pair : *m_geometryCache )
		{
			auto mesh = pair.second->getMesh().lock();

			if ( mesh )
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

		for ( auto pair : *m_geometryCache )
		{
			auto mesh = pair.second->getMesh().lock();

			if ( mesh )
			{
				result += mesh->getFaceCount();
			}
		}

		return result;
	}

	SceneFlags Scene::getFlags()const
	{
		SceneFlags result;

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
	}

	void Scene::removeEnvironmentMap( SceneNode & node )
	{
		m_reflectionMap->removeNode( node );
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

	AnimatedObjectSPtr Scene::addAnimatedTexture( TextureSourceInfo const & sourceInfo
		, TextureConfiguration const & config
		, Pass & pass )
	{
		auto & cache = getAnimatedObjectGroupCache();
		auto group = cache.find( cuT( "C3D_Textures" ) ).lock();
		return group->addObject( sourceInfo, config, pass );
	}

	void Scene::registerLight( Light & light )
	{
		m_lightConnections.emplace( light.getName()
			, light.onChanged.connect( [this]( Light const & lgt )
				{
					doUpdateLightDependent( lgt.getLightType()
						, lgt.isShadowProducer()
						, lgt.getExpectedGlobalIlluminationType() );
				} ) );
	}

	void Scene::unregisterLight( Light & light )
	{
		m_lightConnections.erase( light.getName() );
	}

	PassTypeID Scene::getPassesType()const
	{
		return getEngine()->getPassesType();
	}

	castor::String Scene::getPassesName()const
	{
		return getEngine()->getPassesName();
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

	void Scene::setPassesType( PassTypeID value )
	{
		getEngine()->setPassesType( value );
	}

	crg::SemaphoreWaitArray Scene::getRenderTargetsSemaphores()const
	{
		crg::SemaphoreWaitArray result;
		auto lock( castor::makeUniqueLock( getEngine()->getRenderTargetCache() ) );

		for ( auto & target : getEngine()->getRenderTargetCache().getRenderTargets( TargetType::eTexture ) )
		{
			if ( target->getScene().get() != this )
			{
				result.emplace_back( target->getSemaphore() );
			}
		}

		return result;
	}

	uint32_t Scene::getLpvGridSize()const
	{
		return getEngine()->getLpvGridSize();
	}

	void Scene::setDirectionalShadowCascades( uint32_t value )
	{
		CU_Require( value <= shader::DirectionalMaxCascadesCount );
		m_directionalShadowCascades = value;
	}

	void Scene::setLpvIndirectAttenuation( float value )
	{
		m_lpvIndirectAttenuation = value;
	}

	void Scene::doUpdateBoundingBox()
	{
		float fmin = std::numeric_limits< float >::max();
		float fmax = std::numeric_limits< float >::lowest();
		castor::Point3f min{ fmin, fmin, fmin };
		castor::Point3f max{ fmax, fmax, fmax };
		m_geometryCache->forEach( [&min, &max]( Geometry const & geometry )
			{
				auto node = geometry.getParent();
				auto mesh = geometry.getMesh().lock();

				if ( node && mesh )
				{
					auto bbox = mesh->getBoundingBox().getAxisAligned( node->getDerivedTransformationMatrix() );

					for ( auto i = 0u; i < 3u; ++i )
					{
						min[i] = std::min( min[i], bbox.getMin()[i] );
						max[i] = std::max( max[i], bbox.getMax()[i] );
					}
				}
			} );
		m_boundingBox.load( min, max );
	}

	void Scene::doUpdateAnimations( CpuUpdater & updater )
	{
		std::vector< std::reference_wrapper< AnimatedObjectGroup > > groups;

		m_animatedObjectGroupCache->forEach( [&groups]( AnimatedObjectGroup & group )
		{
			groups.emplace_back( group );
		} );

		if ( groups.size() > m_animationUpdater.getCount() )
		{
			for ( auto & group : groups )
			{
				m_animationUpdater.pushJob( [&group, &updater]()
				{
					group.get().update( updater );
				} );
			}

			m_animationUpdater.waitAll( castor::Milliseconds::max() );
		}
		else
		{
			for ( auto & group : groups )
			{
				group.get().update( updater );
			}
		}
	}

	void Scene::doUpdateMaterials()
	{
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
					auto material = cache.findNoLock( matName ).lock();

					m_needsSubsurfaceScattering |= material->hasSubsurfaceScattering();

					for ( auto & pass : *material )
					{
						m_hasTransparentObjects |= pass->hasAlphaBlending();
						m_hasOpaqueObjects |= !pass->hasOnlyAlphaBlending();
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
			|| m_giTypes[size_t( lightType )].find( globalIllumination ) == m_giTypes[size_t( lightType )].end();

		if ( changed )
		{
			m_hasShadows[size_t( lightType )] = shadowProducer;

			if ( globalIllumination != GlobalIlluminationType::eNone )
			{
				m_giTypes[size_t( lightType )].insert( globalIllumination );
			}

			onChanged( *this );
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
