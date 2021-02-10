#include "Castor3D/Scene/Scene.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/AnimatedObjectGroupCache.hpp"
#include "Castor3D/Cache/BillboardCache.hpp"
#include "Castor3D/Cache/CacheView.hpp"
#include "Castor3D/Cache/CameraCache.hpp"
#include "Castor3D/Cache/GeometryCache.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Cache/ListenerCache.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/MeshCache.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Cache/ParticleSystemCache.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Cache/SceneNodeCache.hpp"
#include "Castor3D/Cache/TargetCache.hpp"
#include "Castor3D/Cache/WindowCache.hpp"
#include "Castor3D/Event/Frame/CleanupEvent.hpp"
#include "Castor3D/Event/Frame/InitialiseEvent.hpp"
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

#include <CastorUtils/Graphics/Font.hpp>
#include <CastorUtils/Graphics/FontCache.hpp>

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	namespace
	{
		template< typename ResourceType
			, EventType EventType
			, typename CacheType
			, typename Initialiser
			, typename Cleaner >
		std::unique_ptr< CacheView< ResourceType, CacheType, EventType > > makeCacheView( castor::String const & name
			, Initialiser && initialise
			, Cleaner && clean
			, CacheType & cache )
		{
			return std::make_unique< CacheView< ResourceType, CacheType, EventType > >( name
				, std::move( initialise )
				, std::move( clean )
				, cache );
		}

		template< typename ObjType, typename ViewType >
		bool writeView( ViewType const & view
			, String const & elemsName
			, String const & tabs
			, TextFile & file )
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
	}

	//*************************************************************************************************

	template<>
	inline void CacheView< Overlay, OverlayCache, EventType::ePreRender >::clear()
	{
		for ( auto name : m_createdElements )
		{
			auto resource = m_cache.find( name );

			if ( resource )
			{
				m_cache.remove( name );
			}
		}
	}

	//*************************************************************************************************

	template<>
	inline void CacheView< castor::Font, FontCache, EventType::ePreRender >::clear()
	{
		for ( auto name : m_createdElements )
		{
			auto resource = m_cache.find( name );

			if ( resource )
			{
				m_cache.remove( name );
			}
		}
	}

	//*************************************************************************************************

	String Scene::RootNode = cuT( "RootNode" );
	String Scene::CameraRootNode = cuT( "CameraRootNode" );
	String Scene::ObjectRootNode = cuT( "ObjectRootNode" );

	Scene::Scene( String const & name, Engine & engine )
		: OwnedBy< Engine >{ engine }
		, Named{ name }
		, m_listener{ engine.getFrameListenerCache().add( cuT( "Scene_" ) + name + string::toString( (size_t)this ) ) }
		, m_animationUpdater{ std::max( 2u, engine.getCpuInformations().getCoreCount() - ( engine.isThreaded() ? 2u : 1u ) ) }
		, m_background{ std::make_shared< ColourBackground >( engine, *this ) }
		, m_colourBackground{ std::make_shared< ColourBackground >( engine, *this, cuT( "Default" ) ) }
		, m_lightFactory{ std::make_shared< LightFactory >() }
	{
		auto mergeObject = [this]( auto const & source
			, auto & destination
			, auto element
			, SceneNodeSPtr rootCameraNode
			, SceneNodeSPtr rootObjectNode )
		{
			if ( element->getParent()->getName() == rootCameraNode->getName() )
			{
				element->detach();
				element->attachTo( *rootCameraNode );
			}
			else if ( element->getParent()->getName() == rootObjectNode->getName() )
			{
				element->detach();
				element->attachTo( *rootObjectNode );
			}

			String name = element->getName();

			while ( destination.has( name ) )
			{
				name = this->getName() + cuT( "_" ) + name;
			}

			element->setName( name );
			destination.insert( name, element );
		};
		auto mergeResource = [this]( auto const & source
			, auto & destination
			, auto element )
		{
			String name = element->getName();

			if ( !destination.has( name ) )
			{
				name = this->getName() + cuT( "_" ) + name;
			}

			element->setName( name );
			destination.insert( name, element );
		};
		auto gpuEvtInitialise = [this]( auto element )
		{
			this->getListener().postEvent( makeGpuInitialiseEvent( *element ) );
		};
		auto gpuEvtClean = [this]( auto element )
		{
			this->getListener().postEvent( makeGpuCleanupEvent( *element ) );
		};
		auto cpuEvtInitialise = [this]( auto element )
		{
			this->getListener().postEvent( makeCpuInitialiseEvent( *element ) );
		};
		auto cpuEvtClean = [this]( auto element )
		{
			this->getListener().postEvent( makeCpuCleanupEvent( *element ) );
		};
		auto attachObject = []( auto element
			, SceneNode & parent
			, SceneNodeSPtr rootNode
			, SceneNodeSPtr rootCameraNode
			, SceneNodeSPtr rootObjectNode )
		{
			parent.attachObject( *element );
		};
		auto attachCamera = []( auto element
			, SceneNode & parent
			, SceneNodeSPtr rootNode
			, SceneNodeSPtr rootCameraNode
			, SceneNodeSPtr rootObjectNode )
		{
			parent.attachObject( *element );
		};
		auto attachNode = []( auto element
			, SceneNode & parent
			, SceneNodeSPtr rootNode
			, SceneNodeSPtr rootCameraNode
			, SceneNodeSPtr rootObjectNode )
		{
			element->attachTo( parent );
		};
		auto dummy = []( auto element )
		{
		};

		m_rootNode = std::make_shared< SceneNode >( RootNode, *this );
		m_rootCameraNode = std::make_shared< SceneNode >( CameraRootNode, *this );
		m_rootObjectNode = std::make_shared< SceneNode >( ObjectRootNode, *this );
		m_rootCameraNode->attachTo( *m_rootNode );
		m_rootObjectNode->attachTo( *m_rootNode );

		m_billboardCache = makeObjectCache< BillboardList, String >( engine
			, *this
			, m_rootNode
			, m_rootCameraNode
			, m_rootObjectNode
			, [this]( String const & name, SceneNode & parent )
				{
					return std::make_shared< BillboardList >( name
						, *this
						, parent );
				}
			, gpuEvtInitialise
			, gpuEvtClean
			, mergeObject
			, attachObject
			, [this]( BillboardListSPtr element )
				{
					element->detach();
				} );
		m_cameraCache = makeObjectCache< Camera, String >( engine
			, *this
			, m_rootNode
			, m_rootCameraNode
			, m_rootObjectNode
			, [this]( String const & name
				, SceneNode & parent
				, Viewport && viewport )
				{
					return std::make_shared< Camera >( name
						, *this
						, parent
						, std::move( viewport ) );
				}
			, dummy
			, dummy
			, mergeObject
			, attachCamera
			, [this]( CameraSPtr element )
				{
					element->detach();
				} );
		m_geometryCache = makeObjectCache< Geometry, String >( engine
			, *this
			, m_rootNode
			, m_rootCameraNode
			, m_rootObjectNode
			, [this]( String const & name
				, SceneNode & parent
				, MeshSPtr mesh )
				{
					return std::make_shared< Geometry >( name
						, *this
						, parent
						, mesh );
				}
			, dummy
			, dummy
			, mergeObject
			, attachObject
			, [this]( GeometrySPtr element )
				{
					element->detach();
				} );
		m_lightCache = makeObjectCache< Light, String >( engine
			, *this
			, m_rootNode
			, m_rootCameraNode
			, m_rootObjectNode
			, [this]( String const & name
				, SceneNode & parent
				, LightType lightType )
				{
					auto result = std::make_shared< Light >( name
						, *this
						, parent
						, *m_lightFactory
						, lightType );
					m_lightConnections.emplace( name
						, result->onChanged.connect( [this]( Light const & light )
						{
							doUpdateLightDependent( light.getLightType()
								, light.isShadowProducer()
								, light.getExpectedGlobalIlluminationType() );
						} ) );
					return result;
				}
			, dummy
			, dummy
			, mergeObject
			, attachObject
			, [this]( LightSPtr element )
				{
					element->detach();
				} );
		m_particleSystemCache = makeObjectCache< ParticleSystem, String >( engine
			, *this
			, m_rootNode
			, m_rootCameraNode
			, m_rootObjectNode
			, [this]( String const & name
				, SceneNode & parent
				, uint32_t count )
				{
					return std::make_shared< ParticleSystem >( name
						, *this
						, parent
						, count );
				}
			, gpuEvtInitialise
			, gpuEvtClean
			, mergeObject
			, attachObject
			, [this]( ParticleSystemSPtr element )
				{
					element->detach();
				} );
		m_sceneNodeCache = makeObjectCache< SceneNode, String >( engine
			, *this
			, m_rootNode
			, m_rootCameraNode
			, m_rootObjectNode
			, [this]( String const & name
				, SceneNode & parent )
				{
					return std::make_shared< SceneNode >( name, *this );
				}
			, dummy
			, dummy
			, mergeObject
			, attachNode
			, [this]( SceneNodeSPtr element )
				{
					element->detach();
				} );
		m_animatedObjectGroupCache = makeCache< AnimatedObjectGroup, String >( engine
			, [this]( castor::String const & name )
				{
					return std::make_shared< AnimatedObjectGroup >( name, *this );
				}
			, dummy
			, dummy
			, mergeResource );
		m_meshCache = makeCache< Mesh, String >( engine
			, [this]( castor::String const & name )
				{
					return std::make_shared< Mesh >( name
						, *this );
				}
			, dummy
			, cpuEvtClean
			, mergeResource );

		m_materialCacheView = makeCacheView< Material, EventType::ePreRender >( getName()
			, [this]( MaterialSPtr element )
				{
					this->getListener().postEvent( makeGpuInitialiseEvent( *element ) );
					this->m_materialsListeners.emplace( element
						, element->onChanged.connect( [this]( Material const & material )
							{
								onMaterialChanged( material );
							} ) );
					m_dirtyMaterials = true;
				}
			, [this]( MaterialSPtr element )
				{
					m_dirtyMaterials = true;
					this->m_materialsListeners.erase( element );
					this->getListener().postEvent( makeCpuCleanupEvent( *element ) );
				}
			, getEngine()->getMaterialCache() );
		m_samplerCacheView = makeCacheView< Sampler, EventType::ePreRender >( getName()
			, gpuEvtInitialise
			, cpuEvtClean
			, getEngine()->getSamplerCache() );
		m_overlayCacheView = makeCacheView< Overlay, EventType::ePreRender >( getName()
			, std::bind( OverlayCache::OverlayInitialiser{ getEngine()->getOverlayCache() }, std::placeholders::_1 )
			, std::bind( OverlayCache::OverlayCleaner{ getEngine()->getOverlayCache() }, std::placeholders::_1 )
			, getEngine()->getOverlayCache() );
		m_fontCacheView = makeCacheView< castor::Font, EventType::ePreRender >( getName()
			, dummy
			, dummy
			, getEngine()->getFontCache() );

		auto notify = [this]()
		{
			setChanged();
		};

		m_sceneNodeCache->add( cuT( "ObjectRootNode" ), m_rootObjectNode );
		m_sceneNodeCache->add( cuT( "CameraRootNode" ), m_rootCameraNode );
		auto setThisChanged = [this]()
		{
			setChanged();
		};
		m_onParticleSystemChanged = m_particleSystemCache->onChanged.connect( setThisChanged );
		m_onBillboardListChanged = m_billboardCache->onChanged.connect( setThisChanged );
		m_onGeometryChanged = m_geometryCache->onChanged.connect( setThisChanged );
		m_onSceneNodeChanged = m_sceneNodeCache->onChanged.connect( setThisChanged );
		m_animatedObjectGroupCache->add( cuT( "C3D_Textures" ) );
	}

	Scene::~Scene()
	{
		m_onSceneNodeChanged.disconnect();
		m_onGeometryChanged.disconnect();
		m_onBillboardListChanged.disconnect();
		m_onParticleSystemChanged.disconnect();

		m_meshCache->clear();

		m_reflectionMapsArray.clear();
		m_reflectionMaps.clear();

		m_background.reset();
		m_colourBackground.reset();
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
		m_lightCache->initialise();
		doUpdateLightsDependent();
		m_initialised = true;
	}

	void Scene::cleanup()
	{
		m_initialised = false;
		m_animatedObjectGroupCache->cleanup();
		m_cameraCache->cleanup();
		m_particleSystemCache->cleanup();

		getListener().postEvent( makeGpuFunctorEvent( EventType::ePreRender
			, [this]( RenderDevice const & device )
			{
				m_billboardCache->cleanup( device );
			} ) );

		m_geometryCache->cleanup();
		m_lightCache->cleanup();
		m_sceneNodeCache->cleanup();

		for ( auto & pass : m_reflectionMapsArray )
		{
			getListener().postEvent( makeGpuFunctorEvent( EventType::ePreRender
				, [&pass]( RenderDevice const & device )
				{
					pass.get().cleanup( device );
				} ) );
		}

		m_materialCacheView->clear();
		m_samplerCacheView->clear();
		m_overlayCacheView->clear();
		m_fontCacheView->clear();

		// These ones, being ResourceCache, need to be cleared in destructor only
		m_meshCache->cleanup();

		getListener().postEvent( makeGpuFunctorEvent( EventType::ePreRender
			, [this]( RenderDevice const & device )
			{
				m_background->cleanup( device );
				m_colourBackground->cleanup( device );
			} ) );
	}

	void Scene::update( CpuUpdater & updater )
	{
		if ( m_initialised )
		{
			m_rootNode->update();
			doUpdateBoundingBox();
			doUpdateAnimations( updater );
			doUpdateMaterials();
			getLightCache().update( updater );
			getGeometryCache().update( updater );
			getBillboardListCache().update( updater );
			getAnimatedObjectGroupCache().update( updater );
			onUpdate( *this );
			m_changed = false;
		}
	}

	void Scene::update( GpuUpdater & updater )
	{
		getLightCache().update( updater );
		getMeshCache().forEach( []( Mesh & mesh )
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

	bool Scene::importExternal( Path const & fileName, SceneImporter & importer )
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
			auto mesh = pair.second->getMesh();

			if ( mesh )
			{
				result += pair.second->getMesh()->getVertexCount();
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
			auto mesh = pair.second->getMesh();

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
				result |= SceneFlag::eLayeredLpvGI;
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

	void Scene::createEnvironmentMap( SceneNode & node )
	{
		if ( !hasEnvironmentMap( node ) )
		{
			auto it = m_reflectionMaps.emplace( &node, std::make_unique< EnvironmentMap >( *getEngine(), node ) ).first;
			auto & pass = *it->second;
			m_reflectionMapsArray.emplace_back( pass );

			getListener().postEvent( makeGpuFunctorEvent( EventType::ePreRender
				, [&pass]( RenderDevice const & device )
				{
					pass.initialise( device );
				} ) );
		}
	}

	bool Scene::hasEnvironmentMap( SceneNode const & node )const
	{
		return m_reflectionMaps.end() != m_reflectionMaps.find( &node );
	}

	EnvironmentMap & Scene::getEnvironmentMap( SceneNode const & node )
	{
		CU_Require( hasEnvironmentMap( node ) );
		return *m_reflectionMaps.find( &node )->second;
	}

	EnvironmentMap const & Scene::getEnvironmentMap( SceneNode const & node )const
	{
		CU_Require( hasEnvironmentMap( node ) );
		return *m_reflectionMaps.find( &node )->second;
	}

	AnimatedObjectSPtr Scene::addAnimatedTexture( TextureUnit & texture
		, Pass & pass )
	{
		auto & cache = getAnimatedObjectGroupCache();
		auto group = cache.find( cuT( "C3D_Textures" ) );
		return group->addObject( texture, pass );
	}

	MaterialType Scene::getMaterialsType()const
	{
		return getEngine()->getMaterialsType();
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

	void Scene::setMaterialsType( MaterialType value )
	{
		getEngine()->setMaterialsType( value );
	}

	ashes::SemaphoreCRefArray Scene::getRenderTargetsSemaphores()const
	{
		ashes::SemaphoreCRefArray result;

		for ( auto & target : getEngine()->getRenderTargetCache().getRenderTargets( TargetType::eTexture ) )
		{
			if ( target->getScene().get() != this )
			{
				result.emplace_back( target->getSemaphore() );
			}
		}

		return result;
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
		Point3f min{ fmin, fmin, fmin };
		Point3f max{ fmax, fmax, fmax };
		m_geometryCache->forEach( [&min, &max]( Geometry const & geometry )
			{
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

			m_animationUpdater.waitAll( Milliseconds::max() );
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
				if ( cache.has( matName ) )
				{
					auto material = cache.find( matName );

					m_needsSubsurfaceScattering |= material->hasSubsurfaceScattering();

					for ( auto & pass : *material )
					{
						m_hasTransparentObjects |= pass->hasAlphaBlending();
						m_hasOpaqueObjects |= !pass->hasAlphaBlending();
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
			m_giTypes[size_t( lightType )].insert( globalIllumination );
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
