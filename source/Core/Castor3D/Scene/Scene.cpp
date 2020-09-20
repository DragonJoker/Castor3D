#include "Castor3D/Scene/Scene.hpp"

#include "Castor3D/Cache/AnimatedObjectGroupCache.hpp"
#include "Castor3D/Cache/BillboardCache.hpp"
#include "Castor3D/Cache/CacheView.hpp"
#include "Castor3D/Cache/CameraCache.hpp"
#include "Castor3D/Cache/GeometryCache.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Cache/ListenerCache.hpp"
#include "Castor3D/Cache/MeshCache.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Cache/ParticleSystemCache.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Cache/SceneNodeCache.hpp"
#include "Castor3D/Cache/TargetCache.hpp"
#include "Castor3D/Cache/WindowCache.hpp"
#include "Castor3D/Event/Frame/CleanupEvent.hpp"
#include "Castor3D/Event/Frame/FunctorEvent.hpp"
#include "Castor3D/Event/Frame/InitialiseEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
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

	Scene::TextWriter::TextWriter( castor::String const & tabs
		, castor::Path const & materialsFile )
		: castor::TextWriter< Scene >{ tabs }
		, m_materialsFile{ materialsFile }
	{
	}

	bool Scene::TextWriter::operator()( Scene const & scene, TextFile & file )
	{
		log::info << cuT( "Scene::write - Scene Name" ) << std::endl;
		bool result = file.writeText( m_tabs + cuT( "// Global configuration\n" ) ) > 0;

		if ( scene.getEngine()->getRenderLoop().hasDebugOverlays() )
		{
			result = file.writeText( m_tabs + cuT( "debug_overlays true\n" ) ) > 0;
			castor::TextWriter< Scene >::checkError( result, "Debug overlays" );
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "materials " ) + castor3d::getName( scene.getMaterialsType() ) + cuT( "\n" ) ) > 0;
			castor::TextWriter< Scene >::checkError( result, "Materials type" );
		}

		if ( result )
		{
			result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "scene \"" ) + scene.getName() + cuT( "\"\n" ) ) > 0
				&& file.writeText( m_tabs + cuT( "{" ) ) > 0;
			castor::TextWriter< Scene >::checkError( result, "Scene name" );
		}

		if ( result )
		{
			result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "\t// Scene configuration\n" ) ) > 0;
		}

		if ( result )
		{
			log::info << cuT( "Scene::write - Ambient light" ) << std::endl;
			result = file.print( 256, cuT( "%s\tambient_light " ), m_tabs.c_str() ) > 0
				&& RgbColour::TextWriter( String() )( scene.getAmbientLight(), file )
				&& file.writeText( cuT( "\n" ) ) > 0;
			castor::TextWriter< Scene >::checkError( result, "Scene ambient light" );
		}

		if ( result )
		{
			log::info << cuT( "Scene::write - Background colour" ) << std::endl;
			result = file.print( 256, cuT( "%s\tbackground_colour " ), m_tabs.c_str() ) > 0
				&& RgbColour::TextWriter( String() )( scene.getBackgroundColour(), file )
				&& file.writeText( cuT( "\n" ) ) > 0;
			castor::TextWriter< Scene >::checkError( result, "Scene background colour" );
		}

		if ( result && !m_materialsFile.empty() )
		{
			log::info << cuT( "Scene::write - Materials file" ) << std::endl;
			String path = m_materialsFile;
			string::replace( path, cuT( "\\" ), cuT( "/" ) );
			result = file.writeText( m_tabs + cuT( "\tinclude \"" ) + path +cuT( "\"\n" ) ) > 0;
			castor::TextWriter< Scene >::checkError( result, "Scene material file" );
		}

		if ( result )
		{
			log::info << cuT( "Scene::write - Background" ) << std::endl;
			BackgroundTextWriter writer{ file, m_tabs + cuT( "\t" ) };
			scene.getBackground()->accept( writer );
		}

		if ( result && scene.getFog().getType() != FogType::eDisabled )
		{
			log::info << cuT( "Scene::write - Fog type" ) << std::endl;
			result = file.writeText( m_tabs + cuT( "\tfog_type " ) + castor3d::getName( scene.getFog().getType() ) + cuT( "\n" ) ) > 0;
			castor::TextWriter< Scene >::checkError( result, "Scene fog type" );

			if ( result )
			{
				log::info << cuT( "Scene::write - Fog density" ) << std::endl;
				result = file.writeText( m_tabs + cuT( "\tfog_density " ) + string::toString( scene.getFog().getDensity(), std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< Scene >::checkError( result, "Scene fog density" );
			}
		}

		if ( result )
		{
			result = writeView< castor::Font >( scene.getFontView()
				, cuT( "Fonts" )
				, m_tabs
				, file );
		}

		if ( m_materialsFile.empty() )
		{
			if ( result )
			{
				result = writeView< Sampler >( scene.getSamplerView()
					, cuT( "Samplers" )
					, m_tabs
					, file );
			}

			if ( result )
			{
				result = writeView< Material >( scene.getMaterialView()
					, cuT( "Materials" )
					, m_tabs
					, file );
			}
		}

		if ( result && !scene.getOverlayView().isEmpty() )
		{
			result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "\t// Overlays\n" ) ) > 0;

			if ( result )
			{
				log::info << cuT( "Scene::write - Overlays" ) << std::endl;

				for ( auto const & name : scene.getOverlayView() )
				{
					auto overlay = scene.getOverlayView().find( name );

					if ( !overlay->getParent() )
					{
						result = result && Overlay::TextWriter( m_tabs + cuT( "\t" ) )( *overlay, file );
					}
				}
			}
		}

		if ( result && !scene.getMeshCache().isEmpty() )
		{
			result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "\t// Meshes\n" ) ) > 0;

			if ( result )
			{
				log::info << cuT( "Scene::write - Meshes" ) << std::endl;
				using LockType = std::unique_lock< MeshCache const >;
				LockType lock{ castor::makeUniqueLock( scene.getMeshCache() ) };

				for ( auto const & it : scene.getMeshCache() )
				{
					if ( result && it.second->isSerialisable() )
					{
						result = Mesh::TextWriter( m_tabs + cuT( "\t" ) )( *it.second, file );
					}
				}
			}
		}

		if ( result && !scene.getCameraRootNode()->getChildren().empty() )
		{
			result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "\t// Cameras nodes\n" ) ) > 0;

			if ( result )
			{
				log::info << cuT( "Scene::write - Cameras nodes" ) << std::endl;

				for ( auto const & it : scene.getCameraRootNode()->getChildren() )
				{
					if ( result
						&& it.first.find( cuT( "_REye" ) ) == String::npos
						&& it.first.find( cuT( "_LEye" ) ) == String::npos )
					{
						result = SceneNode::TextWriter( m_tabs + cuT( "\t" ) )( *it.second.lock(), file );
					}
				}
			}
		}

		if ( result && !scene.getCameraCache().isEmpty() )
		{
			result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "\t// Cameras\n" ) ) > 0;

			if ( result )
			{
				log::info << cuT( "Scene::write - Cameras" ) << std::endl;
				using LockType = std::unique_lock< CameraCache const >;
				LockType lock{ castor::makeUniqueLock( scene.getCameraCache() ) };

				for ( auto const & it : scene.getCameraCache() )
				{
					if ( result
						&& it.first.find( cuT( "_REye" ) ) == String::npos
						&& it.first.find( cuT( "_LEye" ) ) == String::npos )
					{
						result = Camera::TextWriter( m_tabs + cuT( "\t" ) )( *it.second, file );
					}
				}
			}
		}

		if ( result && !scene.getObjectRootNode()->getChildren().empty() )
		{
			result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "\t// Objects nodes\n" ) ) > 0;

			if ( result )
			{
				log::info << cuT( "Scene::write - Objects nodes" ) << std::endl;

				for ( auto const & it : scene.getObjectRootNode()->getChildren() )
				{
					result = result && SceneNode::TextWriter( m_tabs + cuT( "\t" ) )( *it.second.lock(), file );
				}
			}
		}

		if ( result && !scene.getLightCache() .isEmpty() )
		{
			result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "\t// Lights\n" ) ) > 0;

			if ( result )
			{
				log::info << cuT( "Scene::write - Lights" ) << std::endl;
				using LockType = std::unique_lock< LightCache const >;
				LockType lock{ castor::makeUniqueLock( scene.getLightCache() ) };

				for ( auto const & it : scene.getLightCache() )
				{
					result = result && Light::TextWriter( m_tabs + cuT( "\t" ) )( *it.second, file );
				}
			}
		}

		if ( result && !scene.getGeometryCache().isEmpty() )
		{
			result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "\t// Geometries\n" ) ) > 0;

			if ( result )
			{
				log::info << cuT( "Scene::write - Geometries" ) << std::endl;
				using LockType = std::unique_lock< GeometryCache const >;
				LockType lock{ castor::makeUniqueLock( scene.getGeometryCache() ) };

				for ( auto const & it : scene.getGeometryCache() )
				{
					if ( result
						&& it.second->getMesh()
						&& it.second->getMesh()->isSerialisable() )
					{
						result = Geometry::TextWriter( m_tabs + cuT( "\t" ) )( *it.second, file );
					}
				}
			}
		}

		if ( result && !scene.getParticleSystemCache().isEmpty() )
		{
			result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "\t// Particle systems\n" ) ) > 0;

			if ( result )
			{
				log::info << cuT( "Scene::write - Particle systems" ) << std::endl;
				using LockType = std::unique_lock< ObjectCache< ParticleSystem, castor::String > const >;
				LockType lock{ castor::makeUniqueLock( scene.getParticleSystemCache() ) };

				for ( auto const & it : scene.getParticleSystemCache() )
				{
					result = result && ParticleSystem::TextWriter( m_tabs + cuT( "\t" ) )( *it.second, file );
				}
			}
		}

		if ( result && !scene.getAnimatedObjectGroupCache().isEmpty() )
		{
			result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "\t// Animated object groups\n" ) ) > 0;

			if ( result )
			{
				log::info << cuT( "Scene::write - Animated object groups" ) << std::endl;
				using LockType = std::unique_lock< AnimatedObjectGroupCache const >;
				LockType lock{ castor::makeUniqueLock( scene.getAnimatedObjectGroupCache() ) };

				for ( auto const & it : scene.getAnimatedObjectGroupCache() )
				{
					result = result && AnimatedObjectGroup::TextWriter( m_tabs + cuT( "\t" ) )( *it.second, file );
				}
			}
		}

		file.writeText( cuT( "}\n" ) );

		if ( result )
		{
			result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "// Windows\n" ) ) > 0;
		}

		if ( result )
		{
			log::info << cuT( "Scene::write - Windows" ) << std::endl;
			using LockType = std::unique_lock< RenderWindowCache >;
			LockType lock{ castor::makeUniqueLock( scene.getEngine()->getRenderWindowCache() ) };

			for ( auto const & it : scene.getEngine()->getRenderWindowCache() )
			{
				if ( it.second->getRenderTarget()->getScene()->getName() == scene.getName() )
				{
					result = result && RenderWindow::TextWriter( m_tabs )( *it.second, file );
				}
			}
		}

		return result;
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
		, m_colourBackground{ std::make_shared< ColourBackground >( engine, *this ) }
		, m_billboardPools{ std::make_shared< BillboardUboPools >( *engine.getRenderSystem() ) }
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
		auto eventInitialise = [this]( auto element )
		{
			this->getListener().postEvent( makeInitialiseEvent( *element ) );
		};
		auto eventClean = [this]( auto element )
		{
			this->getListener().postEvent( makeCleanupEvent( *element ) );
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
			, eventInitialise
			, eventClean
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
					return std::make_shared< Light >( name
						, *this
						, parent
						, *m_lightFactory
						, lightType );
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
			, eventInitialise
			, eventClean
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
			, eventClean
			, mergeResource );

		m_materialCacheView = makeCacheView< Material, EventType::ePreRender >( getName()
			, [this]( MaterialSPtr element )
				{
					this->getListener().postEvent( makeInitialiseEvent( *element ) );
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
					this->getListener().postEvent( makeCleanupEvent( *element ) );
				}
			, getEngine()->getMaterialCache() );
		m_samplerCacheView = makeCacheView< Sampler, EventType::ePreRender >( getName()
			, eventInitialise
			, eventClean
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
		m_initialised = true;
	}

	void Scene::cleanup()
	{
		m_initialised = false;
		m_animatedObjectGroupCache->cleanup();
		m_cameraCache->cleanup();
		m_billboardCache->cleanup();
		m_particleSystemCache->cleanup();
		m_geometryCache->cleanup();
		m_lightCache->cleanup();
		m_sceneNodeCache->cleanup();

		for ( auto & pass : m_reflectionMapsArray )
		{
			getListener().postEvent( makeFunctorEvent( EventType::ePreRender
				, [&pass]()
				{
					pass.get().cleanup();
				} ) );
		}

		m_materialCacheView->clear();
		m_samplerCacheView->clear();
		m_overlayCacheView->clear();
		m_fontCacheView->clear();

		// These ones, being ResourceCache, need to be cleared in destructor only
		m_meshCache->cleanup();

		getListener().postEvent( makeFunctorEvent( EventType::ePreRender
			, [this]()
			{
				m_background->cleanup();
				m_colourBackground->cleanup();
				m_billboardPools->clear();
			} ) );
	}

	void Scene::update( CpuUpdater & updater )
	{
		if ( m_initialised )
		{
			m_rootNode->update();
			doUpdateBoundingBox();
			doUpdateAnimations();
			doUpdateMaterials();
			getLightCache().update( updater );
			getGeometryCache().cpuUpdate();
			m_billboardPools->update();
			getAnimatedObjectGroupCache().update();
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

		return result;
	}

	bool Scene::hasShadows()const
	{
		using LockType = std::unique_lock< LightCache const >;
		LockType lock{ castor::makeUniqueLock( getLightCache() ) };

		return getLightCache().end() != std::find_if( getLightCache().begin(), getLightCache().end(), []( std::pair< String, LightSPtr > const & p_it )
		{
			return p_it.second->isShadowProducer();
		} );
	}

	void Scene::createEnvironmentMap( SceneNode & node )
	{
		if ( !hasEnvironmentMap( node ) )
		{
			auto it = m_reflectionMaps.emplace( &node, std::make_unique< EnvironmentMap >( *getEngine(), node ) ).first;
			auto & pass = *it->second;
			m_reflectionMapsArray.emplace_back( pass );

			getListener().postEvent( makeFunctorEvent( EventType::ePreRender
				, [&pass]()
				{
					pass.initialise();
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

	MaterialType Scene::getMaterialsType()const
	{
		return getEngine()->getMaterialsType();
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

	void Scene::doUpdateBoundingBox()
	{
		float fmin = std::numeric_limits< float >::max();
		float fmax = std::numeric_limits< float >::lowest();
		Point3f min{ fmin, fmin, fmin };
		Point3f max{ fmax, fmax, fmax };
		m_geometryCache->forEach( [&min, &max]( Geometry const & geometry )
			{
				auto bbox = geometry.getMesh()->getBoundingBox().getAxisAligned( geometry.getParent()->getDerivedTransformationMatrix() );

				for ( auto i = 0u; i < 3u; ++i )
				{
					min[i] = std::min( min[i], bbox.getMin()[i] );
					max[i] = std::max( max[i], bbox.getMax()[i] );
				}
			} );
		m_boundingBox.load( min, max );
	}

	void Scene::doUpdateAnimations()
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
				m_animationUpdater.pushJob( [&group]()
				{
					group.get().update();
				} );
			}

			m_animationUpdater.waitAll( Milliseconds::max() );
		}
		else
		{
			for ( auto & group : groups )
			{
				group.get().update();
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

	void Scene::onMaterialChanged( Material const & material )
	{
		m_dirtyMaterials = true;
	}
}
