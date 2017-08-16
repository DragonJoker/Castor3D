#include "Scene.hpp"

#include "Camera.hpp"
#include "BillboardList.hpp"
#include "Geometry.hpp"
#include "Skybox.hpp"

#include "Animation/AnimatedObjectGroup.hpp"
#include "Event/Frame/CleanupEvent.hpp"
#include "Event/Frame/InitialiseEvent.hpp"
#include "Material/Material.hpp"
#include "Mesh/Mesh.hpp"
#include "ParticleSystem/ParticleSystem.hpp"
#include "EnvironmentMap/EnvironmentMap.hpp"
#include "Render/RenderLoop.hpp"
#include "Render/RenderTarget.hpp"
#include "Render/RenderWindow.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <Graphics/Font.hpp>
#include <Graphics/Image.hpp>

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
	inline void CacheView< Font, FontCache, EventType::ePreRender >::clear()
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

	Scene::TextWriter::TextWriter( String const & tabs )
		: castor::TextWriter< Scene >{ tabs }
	{
	}

	bool Scene::TextWriter::operator()( Scene const & scene, TextFile & file )
	{
		static std::map< FogType, castor::String > const FogTypes
		{
			{ FogType::eLinear, cuT( "linear" ) },
			{ FogType::eExponential, cuT( "exponential" ) },
			{ FogType::eSquaredExponential, cuT( "squared_exponential" ) },
		};

		static std::map< MaterialType, castor::String > const MaterialTypes
		{
			{ MaterialType::eLegacy, cuT( "legacy" ) },
			{ MaterialType::ePbrMetallicRoughness, cuT( "pbr_metallic_roughness" ) },
			{ MaterialType::ePbrSpecularGlossiness, cuT( "pbr_specular_glossiness" ) },
		};

		Logger::logInfo( cuT( "Scene::write - Scene Name" ) );

		bool result = true;

		if ( scene.getEngine()->getRenderLoop().hasDebugOverlays() )
		{
			result = file.writeText( m_tabs + cuT( "debug_overlays true\n" ) ) > 0;
			castor::TextWriter< Scene >::checkError( result, "Scene debug overlays" );
		}

		if ( result )
		{
			result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "scene \"" ) + scene.getName() + cuT( "\"\n" ) ) > 0
				&& file.writeText( m_tabs + cuT( "{" ) ) > 0;
			castor::TextWriter< Scene >::checkError( result, "Scene name" );
		}

		if ( result )
		{
			Logger::logInfo( cuT( "Scene::write - Background colour" ) );
			result = file.print( 256, cuT( "\n%s\tbackground_colour " ), m_tabs.c_str() ) > 0
				&& Colour::TextWriter( String() )( scene.getBackgroundColour(), file )
				&& file.writeText( cuT( "\n" ) ) > 0;
			castor::TextWriter< Scene >::checkError( result, "Scene background colour" );
		}

		if ( result )
		{
			Logger::logInfo( cuT( "Scene::write - Materials type" ) );
			result = file.writeText( m_tabs + cuT( "\tmaterials " ) + MaterialTypes.find( scene.getMaterialsType() )->second + cuT( "\n" ) ) > 0;
			castor::TextWriter< Scene >::checkError( result, "Scene materials type" );
		}

		if ( result && scene.getBackgroundImage() )
		{
			Logger::logInfo( cuT( "Scene::write - Background image" ) );
			Path relative = Scene::TextWriter::copyFile( Path{ scene.getBackgroundImage()->getImage().toString() }, file.getFilePath(), Path{ cuT( "Textures" ) } );
			result = file.writeText( m_tabs + cuT( "\tbackground_image \"" ) + relative + cuT( "\"\n" ) ) > 0;
			castor::TextWriter< Scene >::checkError( result, "Scene background image" );
		}

		if ( result )
		{
			Logger::logInfo( cuT( "Scene::write - Ambient light" ) );
			result = file.print( 256, cuT( "%s\tambient_light " ), m_tabs.c_str() ) > 0
				&& Colour::TextWriter( String() )( scene.getAmbientLight(), file )
				&& file.writeText( cuT( "\n" ) ) > 0;
			castor::TextWriter< Scene >::checkError( result, "Scene ambient light" );
		}

		if ( result && scene.getFog().getType() != FogType::eDisabled )
		{
			Logger::logInfo( cuT( "Scene::write - Fog type" ) );
			result = file.writeText( m_tabs + cuT( "\tfog_type " ) + FogTypes.find( scene.getFog().getType() )->second + cuT( "\n" ) ) > 0;
			castor::TextWriter< Scene >::checkError( result, "Scene fog type" );

			if ( result )
			{
				Logger::logInfo( cuT( "Scene::write - Fog density" ) );
				result = file.writeText( m_tabs + cuT( "\tfog_density " ) + string::toString( scene.getFog().getDensity() ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< Scene >::checkError( result, "Scene fog density" );
			}
		}

		if ( result && scene.hasSkybox() )
		{
			Logger::logInfo( cuT( "Scene::write - Skybox" ) );
			result = Skybox::TextWriter( m_tabs + cuT( "\t" ) )( scene.getSkybox(), file );
		}

		if ( result )
		{
			Logger::logInfo( cuT( "Scene::write - Fonts" ) );

			for ( auto const & name : scene.getFontView() )
			{
				auto font = scene.getFontView().find( name );
				result &= Font::TextWriter( m_tabs + cuT( "\t" ) )( *font, file );
			}
		}

		if ( result )
		{
			Logger::logInfo( cuT( "Scene::write - Samplers" ) );

			for ( auto const & name : scene.getSamplerView() )
			{
				auto sampler = scene.getSamplerView().find( name );
				result &= Sampler::TextWriter( m_tabs + cuT( "\t" ) )( *sampler, file );
			}
		}

		if ( result )
		{
			Logger::logInfo( cuT( "Scene::write - Materials" ) );

			for ( auto const & name : scene.getMaterialView() )
			{
				auto material = scene.getMaterialView().find( name );
				result &= Material::TextWriter( m_tabs + cuT( "\t" ) )( *material, file );
			}
		}

		if ( result )
		{
			Logger::logInfo( cuT( "Scene::write - Overlays" ) );

			for ( auto const & name : scene.getOverlayView() )
			{
				auto overlay = scene.getOverlayView().find( name );

				if ( !overlay->getParent() )
				{
					result &= Overlay::TextWriter( m_tabs + cuT( "\t" ) )( *overlay, file );
				}
			}
		}

		if ( result )
		{
			Logger::logInfo( cuT( "Scene::write - Cameras nodes" ) );

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

		if ( result )
		{
			Logger::logInfo( cuT( "Scene::write - Objects nodes" ) );

			for ( auto const & it : scene.getObjectRootNode()->getChildren() )
			{
				result &= SceneNode::TextWriter( m_tabs + cuT( "\t" ) )( *it.second.lock(), file );
			}
		}

		if ( result )
		{
			Logger::logInfo( cuT( "Scene::write - Cameras" ) );
			auto lock = makeUniqueLock( scene.getCameraCache() );

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

		if ( result )
		{
			Logger::logInfo( cuT( "Scene::write - Lights" ) );
			auto lock = makeUniqueLock( scene.getLightCache() );

			for ( auto const & it : scene.getLightCache() )
			{
				result &= Light::TextWriter( m_tabs + cuT( "\t" ) )( *it.second, file );
			}
		}

		if ( result )
		{
			Logger::logInfo( cuT( "Scene::write - Geometries" ) );
			auto lock = makeUniqueLock( scene.getGeometryCache() );

			for ( auto const & it : scene.getGeometryCache() )
			{
				result &= Geometry::TextWriter( m_tabs + cuT( "\t" ) )( *it.second, file );
			}
		}

		if ( result )
		{
			Logger::logInfo( cuT( "Scene::write - Particle systems" ) );
			auto lock = makeUniqueLock( scene.getParticleSystemCache() );

			for ( auto const & it : scene.getParticleSystemCache() )
			{
				result &= ParticleSystem::TextWriter( m_tabs + cuT( "\t" ) )( *it.second, file );
			}
		}

		if ( result )
		{
			Logger::logInfo( cuT( "Scene::write - Animated object groups" ) );
			auto lock = makeUniqueLock( scene.getAnimatedObjectGroupCache() );

			for ( auto const & it : scene.getAnimatedObjectGroupCache() )
			{
				result &= AnimatedObjectGroup::TextWriter( m_tabs + cuT( "\t" ) )( *it.second, file );
			}
		}

		file.writeText( cuT( "}\n" ) );

		if ( result )
		{
			Logger::logInfo( cuT( "Scene::write - Windows" ) );
			auto lock = makeUniqueLock( scene.getEngine()->getRenderWindowCache() );

			for ( auto const & it : scene.getEngine()->getRenderWindowCache() )
			{
				if ( it.second->getRenderTarget()->getScene()->getName() == scene.getName() )
				{
					result &= RenderWindow::TextWriter( m_tabs )( *it.second, file );
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
				element->attachTo( rootCameraNode );
			}
			else if ( element->getParent()->getName() == rootObjectNode->getName() )
			{
				element->detach();
				element->attachTo( rootObjectNode );
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
			this->getListener().postEvent( MakeInitialiseEvent( *element ) );
		};
		auto eventClean = [this]( auto element )
		{
			this->getListener().postEvent( MakeCleanupEvent( *element ) );
		};
		auto attachObject = []( auto element
			, SceneNodeSPtr parent
			, SceneNodeSPtr rootNode
			, SceneNodeSPtr rootCameraNode
			, SceneNodeSPtr rootObjectNode )
		{
			if ( parent )
			{
				parent->attachObject( *element );
			}
			else
			{
				rootObjectNode->attachObject( *element );
			}
		};
		auto attachCamera = []( auto element
			, SceneNodeSPtr parent
			, SceneNodeSPtr rootNode
			, SceneNodeSPtr rootCameraNode
			, SceneNodeSPtr rootObjectNode )
		{
			if ( parent )
			{
				parent->attachObject( *element );
			}
			else
			{
				rootCameraNode->attachObject( *element );
			}
		};
		auto attachNode = []( auto element
			, SceneNodeSPtr parent
			, SceneNodeSPtr rootNode
			, SceneNodeSPtr rootCameraNode
			, SceneNodeSPtr rootObjectNode )
		{
			if ( parent )
			{
				element->attachTo( parent );
			}
			else
			{
				element->attachTo( rootNode );
			}
		};
		auto dummy = []( auto element )
		{
		};

		m_rootNode = std::make_shared< SceneNode >( RootNode, *this );
		m_rootCameraNode = std::make_shared< SceneNode >( CameraRootNode, *this );
		m_rootObjectNode = std::make_shared< SceneNode >( ObjectRootNode, *this );
		m_rootCameraNode->attachTo( m_rootNode );
		m_rootObjectNode->attachTo( m_rootNode );

		m_billboardCache = MakeObjectCache< BillboardList, String >( engine
			, *this
			, m_rootNode
			, m_rootCameraNode
			, m_rootObjectNode
			, [this]( String const & name, SceneNodeSPtr parent )
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
		m_cameraCache = MakeObjectCache< Camera, String >( engine
			, *this
			, m_rootNode
			, m_rootCameraNode
			, m_rootObjectNode
			, [this]( String const & name
				, SceneNodeSPtr parent
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
		m_geometryCache = MakeObjectCache< Geometry, String >( engine
			, *this
			, m_rootNode
			, m_rootCameraNode
			, m_rootObjectNode
			, [this]( String const & name
				, SceneNodeSPtr parent
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
		m_lightCache = MakeObjectCache< Light, String >( engine
			, *this
			, m_rootNode
			, m_rootCameraNode
			, m_rootObjectNode
			, [this]( String const & name
				, SceneNodeSPtr node
				, LightType lightType )
			{
				return std::make_shared< Light >( name
					, *this
					, node
					, m_lightFactory
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
		m_particleSystemCache = MakeObjectCache< ParticleSystem, String >( engine
			, *this
			, m_rootNode
			, m_rootCameraNode
			, m_rootObjectNode
			, [this]( String const & name
				, SceneNodeSPtr parent
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
		m_sceneNodeCache = MakeObjectCache< SceneNode, String >( engine
			, *this
			, m_rootNode
			, m_rootCameraNode
			, m_rootObjectNode
			, [this]( String const & name )
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
			, eventInitialise
			, eventClean
			, getEngine()->getMaterialCache() );
		m_samplerCacheView = makeCacheView< Sampler, EventType::ePreRender >( getName()
			, eventInitialise
			, eventClean
			, getEngine()->getSamplerCache() );
		m_overlayCacheView = makeCacheView< Overlay, EventType::ePreRender >( getName()
			, std::bind( OverlayCache::OverlayInitialiser{ getEngine()->getOverlayCache() }, std::placeholders::_1 )
			, std::bind( OverlayCache::OverlayCleaner{ getEngine()->getOverlayCache() }, std::placeholders::_1 )
			, getEngine()->getOverlayCache() );
		m_fontCacheView = makeCacheView< Font, EventType::ePreRender >( getName()
			, dummy
			, dummy
			, getEngine()->getFontCache() );

		auto notify = [this]()
		{
			setChanged();
		};

		m_sceneNodeCache->add( cuT( "ObjectRootNode" ), m_rootObjectNode );
		m_sceneNodeCache->add( cuT( "CameraRootNode" ), m_rootCameraNode );

		m_onParticleSystemChanged = m_particleSystemCache->onChanged.connect( std::bind( &Scene::setChanged, this ) );
		m_onBillboardListChanged = m_billboardCache->onChanged.connect( std::bind( &Scene::setChanged, this ) );
		m_onGeometryChanged = m_geometryCache->onChanged.connect( std::bind( &Scene::setChanged, this ) );
		m_onSceneNodeChanged = m_sceneNodeCache->onChanged.connect( std::bind( &Scene::setChanged, this ) );
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

		m_skybox.reset();
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

		getListener().postEvent( MakeFunctorEvent( EventType::ePreRender, [this]()
		{
			m_colour = std::make_unique< TextureProjection >( *getEngine()->getRenderSystem()->getCurrentContext() );
			m_colour->initialise();
		} ) );

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
			getListener().postEvent( MakeFunctorEvent( EventType::ePreRender
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

		getListener().postEvent( MakeFunctorEvent( EventType::ePreRender, [this]()
		{
			if ( m_colour )
			{
				m_colour->cleanup();
				m_colour.reset();
			}
		} ) );

		if ( m_backgroundImage )
		{
			getListener().postEvent( MakeFunctorEvent( EventType::ePreRender, [this]()
			{
				m_backgroundImage->cleanup();
			} ) );
		}

		if ( m_skybox )
		{
			getListener().postEvent( MakeFunctorEvent( EventType::ePreRender, [this]()
			{
				m_skybox->cleanup();
			} ) );
		}
	}

	void Scene::renderBackground( Size const & size, Camera const & camera )
	{
		if ( m_fog.getType() == FogType::eDisabled )
		{
			if ( m_backgroundImage && m_backgroundImage->isInitialised())
			{
				m_colour->render( *m_backgroundImage
					, camera );
			}
			else if ( m_skybox )
			{
				m_skybox->render( camera );
			}
		}
	}

	void Scene::update()
	{
		m_rootNode->update();
		std::vector< std::reference_wrapper< AnimatedObjectGroup > > groups;

		m_animatedObjectGroupCache->forEach( [&groups]( AnimatedObjectGroup & group )
		{
			groups.push_back( group );
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

		if ( !m_skybox && !m_backgroundImage )
		{
			m_skybox = std::make_unique< Skybox >( *getEngine() );
			m_skybox->setScene( *this );
			Size size{ 16, 16 };
			constexpr PixelFormat format{ PixelFormat::eR8G8B8 };
			UbPixel pixel{ true };
			uint8_t c;
			pixel.set< format >( { { m_backgroundColour.red().convertTo( c )
				, m_backgroundColour.green().convertTo( c )
				, m_backgroundColour.blue().convertTo( c ) } } );
			auto buffer = PxBufferBase::create( size, format );
			auto data = buffer->ptr();

			for ( uint32_t i = 0u; i < 256; ++i )
			{
				std::memcpy( data, pixel.constPtr(), 3 );
				data += 3;
			}

			m_skybox->getTexture().getImage( 0u ).initialiseSource( buffer );
			m_skybox->getTexture().getImage( 1u ).initialiseSource( buffer );
			m_skybox->getTexture().getImage( 2u ).initialiseSource( buffer );
			m_skybox->getTexture().getImage( 3u ).initialiseSource( buffer );
			m_skybox->getTexture().getImage( 4u ).initialiseSource( buffer );
			m_skybox->getTexture().getImage( 5u ).initialiseSource( buffer );
			getListener().postEvent( MakeInitialiseEvent( *m_skybox ) );
		}

		m_changed = false;
	}

	bool Scene::setBackground( Path const & folder, Path const & relative )
	{
		bool result = false;

		try
		{
			auto texture = getEngine()->getRenderSystem()->createTexture( TextureType::eTwoDimensions, AccessType::eNone, AccessType::eRead );
			texture->setSource( folder, relative );
			m_backgroundImage = texture;
			getListener().postEvent( MakeFunctorEvent( EventType::ePreRender, [this]()
			{
				m_backgroundImage->initialise();
				m_backgroundImage->bind( 0 );
				m_backgroundImage->generateMipmaps();
				m_backgroundImage->unbind( 0 );
			} ) );
			result = true;
		}
		catch ( castor::Exception & p_exc )
		{
			Logger::logError( p_exc.what() );
		}

		return result;
	}

	bool Scene::setForeground( SkyboxUPtr && skybox )
	{
		m_skybox = std::move( skybox );
		m_skybox->setScene( *this );
		getListener().postEvent( MakeFunctorEvent( EventType::ePreRender, [this]()
		{
			m_skybox->initialise();
		} ) );
		return true;
	}

	void Scene::merge( SceneSPtr scene )
	{
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
		}

		scene->cleanup();
	}

	bool Scene::importExternal( Path const & fileName, Importer & importer )
	{
		setChanged();
		return importer.importScene( *this, fileName, Parameters() );
	}

	uint32_t Scene::getVertexCount()const
	{
		uint32_t result = 0;
		auto lock = makeUniqueLock( *m_geometryCache );

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
		auto lock = makeUniqueLock( *m_geometryCache );

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
		}

		if ( hasShadows() )
		{
			switch ( m_shadow.getFilterType() )
			{
			case ShadowType::eRaw:
				result |= SceneFlag::eShadowFilterRaw;
				break;

			case ShadowType::ePoisson:
				result |= SceneFlag::eShadowFilterPoisson;
				break;

			case ShadowType::eStratifiedPoisson:
				result |= SceneFlag::eShadowFilterStratifiedPoisson;
				break;
			}
		}

		switch ( getMaterialsType() )
		{
		case MaterialType::ePbrMetallicRoughness:
			result |= SceneFlag::ePbrMetallicRoughness;
			break;

		case MaterialType::ePbrSpecularGlossiness:
			result |= SceneFlag::ePbrSpecularGlossiness;
			break;
		}

		return result;
	}

	bool Scene::hasShadows()const
	{
		auto lock = makeUniqueLock( getLightCache() );

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

			getListener().postEvent( MakeFunctorEvent( EventType::ePreRender
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
		REQUIRE( hasEnvironmentMap( node ) );
		return *m_reflectionMaps.find( &node )->second;
	}

	IblTextures const & Scene::getIbl( SceneNode const & node )const
	{
		REQUIRE( m_skybox );

		if ( !hasEnvironmentMap( node ) )
		{
			return m_skybox->getIbl();
		}

		return m_skybox->getIbl();
	}
}
