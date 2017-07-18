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

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	namespace
	{
		template< typename ResourceType
			, EventType EventType
			, typename CacheType
			, typename Initialiser
			, typename Cleaner >
		std::unique_ptr< CacheView< ResourceType, CacheType, EventType > > MakeCacheView( Castor::String const & p_name
			, Initialiser && p_initialise
			, Cleaner && p_clean
			, CacheType & p_cache )
		{
			return std::make_unique< CacheView< ResourceType, CacheType, EventType > >( p_name
				, std::move( p_initialise )
				, std::move( p_clean )
				, p_cache );
		}
	}

	//*************************************************************************************************

	template<>
	inline void CacheView< Overlay, OverlayCache, EventType::ePreRender >::Clear()
	{
		for ( auto name : m_createdElements )
		{
			auto resource = m_cache.Find( name );

			if ( resource )
			{
				m_cache.Remove( name );
			}
		}
	}

	//*************************************************************************************************

	template<>
	inline void CacheView< Font, FontCache, EventType::ePreRender >::Clear()
	{
		for ( auto name : m_createdElements )
		{
			auto resource = m_cache.Find( name );

			if ( resource )
			{
				m_cache.Remove( name );
			}
		}
	}

	//*************************************************************************************************

	Scene::TextWriter::TextWriter( String const & p_tabs )
		: Castor::TextWriter< Scene >{ p_tabs }
	{
	}

	bool Scene::TextWriter::operator()( Scene const & p_scene, TextFile & p_file )
	{
		static std::map< GLSL::FogType, Castor::String > const FogTypes
		{
			{ GLSL::FogType::eLinear, cuT( "linear" ) },
			{ GLSL::FogType::eExponential, cuT( "exponential" ) },
			{ GLSL::FogType::eSquaredExponential, cuT( "squared_exponential" ) },
		};

		Logger::LogInfo( cuT( "Scene::Write - Scene Name" ) );

		bool result = true;

		if ( p_scene.GetEngine()->GetRenderLoop().HasDebugOverlays() )
		{
			result = p_file.WriteText( m_tabs + cuT( "debug_overlays true\n" ) ) > 0;
			Castor::TextWriter< Scene >::CheckError( result, "Scene debug overlays" );
		}

		if ( result )
		{
			result = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "scene \"" ) + p_scene.GetName() + cuT( "\"\n" ) ) > 0
					   && p_file.WriteText( m_tabs + cuT( "{" ) ) > 0;
			Castor::TextWriter< Scene >::CheckError( result, "Scene name" );
		}

		if ( result )
		{
			Logger::LogInfo( cuT( "Scene::Write - Fonts" ) );

			for ( auto const & name : p_scene.GetFontView() )
			{
				auto font = p_scene.GetFontView().Find( name );
				result &= Font::TextWriter( m_tabs + cuT( "\t" ) )( *font, p_file );
			}
		}

		if ( result )
		{
			Logger::LogInfo( cuT( "Scene::Write - Samplers" ) );

			for ( auto const & name : p_scene.GetSamplerView() )
			{
				auto sampler = p_scene.GetSamplerView().Find( name );
				result &= Sampler::TextWriter( m_tabs + cuT( "\t" ) )( *sampler, p_file );
			}
		}

		if ( result )
		{
			Logger::LogInfo( cuT( "Scene::Write - Materials" ) );

			for ( auto const & name : p_scene.GetMaterialView() )
			{
				auto material = p_scene.GetMaterialView().Find( name );
				result &= Material::TextWriter( m_tabs + cuT( "\t" ) )( *material, p_file );
			}
		}

		if ( result )
		{
			Logger::LogInfo( cuT( "Scene::Write - Overlays" ) );

			for ( auto const & name : p_scene.GetOverlayView() )
			{
				auto overlay = p_scene.GetOverlayView().Find( name );

				if ( !overlay->GetParent() )
				{
					result &= Overlay::TextWriter( m_tabs + cuT( "\t" ) )( *overlay, p_file );
				}
			}
		}

		if ( result )
		{
			Logger::LogInfo( cuT( "Scene::Write - Background colour" ) );
			result = p_file.Print( 256, cuT( "\n%s\tbackground_colour " ), m_tabs.c_str() ) > 0
					   && Colour::TextWriter( String() )( p_scene.GetBackgroundColour(), p_file )
					   && p_file.WriteText( cuT( "\n" ) ) > 0;
			Castor::TextWriter< Scene >::CheckError( result, "Scene background colour" );
		}

		if ( result && p_scene.GetBackgroundImage() )
		{
			Logger::LogInfo( cuT( "Scene::Write - Background image" ) );
			Path relative = Scene::TextWriter::CopyFile( Path{ p_scene.GetBackgroundImage()->GetImage().ToString() }, p_file.GetFilePath(), Path{ cuT( "Textures" ) } );
			result = p_file.WriteText( m_tabs + cuT( "\tbackground_image \"" ) + relative + cuT( "\"\n" ) ) > 0;
			Castor::TextWriter< Scene >::CheckError( result, "Scene background image" );
		}

		if ( result )
		{
			Logger::LogInfo( cuT( "Scene::Write - Ambient light" ) );
			result = p_file.Print( 256, cuT( "%s\tambient_light " ), m_tabs.c_str() ) > 0
					   && Colour::TextWriter( String() )( p_scene.GetAmbientLight(), p_file )
					   && p_file.WriteText( cuT( "\n" ) ) > 0;
			Castor::TextWriter< Scene >::CheckError( result, "Scene ambient light" );
		}

		if ( result && p_scene.GetFog().GetType() != GLSL::FogType::eDisabled )
		{
			Logger::LogInfo( cuT( "Scene::Write - Fog type" ) );
			result = p_file.WriteText( m_tabs + cuT( "\tfog_type " ) + FogTypes.find( p_scene.GetFog().GetType() )->second + cuT( "\n" ) ) > 0;
			Castor::TextWriter< Scene >::CheckError( result, "Scene fog type" );

			if ( result )
			{
				Logger::LogInfo( cuT( "Scene::Write - Fog density" ) );
				result = p_file.WriteText( m_tabs + cuT( "\tfog_density " ) + string::to_string( p_scene.GetFog().GetDensity() ) + cuT( "\n" ) ) > 0;
				Castor::TextWriter< Scene >::CheckError( result, "Scene fog density" );
			}
		}

		if ( result && p_scene.HasSkybox() )
		{
			Logger::LogInfo( cuT( "Scene::Write - Skybox" ) );
			result = Skybox::TextWriter( m_tabs + cuT( "\t" ) )( p_scene.GetSkybox(), p_file );
		}

		if ( result )
		{
			Logger::LogInfo( cuT( "Scene::Write - Cameras nodes" ) );

			for ( auto const & it : p_scene.GetCameraRootNode()->GetChilds() )
			{
				if ( result
					 && it.first.find( cuT( "_REye" ) ) == String::npos
					 && it.first.find( cuT( "_LEye" ) ) == String::npos )
				{
					result = SceneNode::TextWriter( m_tabs + cuT( "\t" ) )( *it.second.lock(), p_file );
				}
			}
		}

		if ( result )
		{
			Logger::LogInfo( cuT( "Scene::Write - Objects nodes" ) );

			for ( auto const & it : p_scene.GetObjectRootNode()->GetChilds() )
			{
				result &= SceneNode::TextWriter( m_tabs + cuT( "\t" ) )( *it.second.lock(), p_file );
			}
		}

		if ( result )
		{
			Logger::LogInfo( cuT( "Scene::Write - Cameras" ) );
			auto lock = make_unique_lock( p_scene.GetCameraCache() );

			for ( auto const & it : p_scene.GetCameraCache() )
			{
				if ( result
					 && it.first.find( cuT( "_REye" ) ) == String::npos
					 && it.first.find( cuT( "_LEye" ) ) == String::npos )
				{
					result = Camera::TextWriter( m_tabs + cuT( "\t" ) )( *it.second, p_file );
				}
			}
		}

		if ( result )
		{
			Logger::LogInfo( cuT( "Scene::Write - Lights" ) );
			auto lock = make_unique_lock( p_scene.GetLightCache() );

			for ( auto const & it : p_scene.GetLightCache() )
			{
				result &= Light::TextWriter( m_tabs + cuT( "\t" ) )( *it.second, p_file );
			}
		}

		if ( result )
		{
			Logger::LogInfo( cuT( "Scene::Write - Geometries" ) );
			auto lock = make_unique_lock( p_scene.GetGeometryCache() );

			for ( auto const & it : p_scene.GetGeometryCache() )
			{
				result &= Geometry::TextWriter( m_tabs + cuT( "\t" ) )( *it.second, p_file );
			}
		}

		if ( result )
		{
			Logger::LogInfo( cuT( "Scene::Write - Particle systems" ) );
			auto lock = make_unique_lock( p_scene.GetParticleSystemCache() );

			for ( auto const & it : p_scene.GetParticleSystemCache() )
			{
				result &= ParticleSystem::TextWriter( m_tabs + cuT( "\t" ) )( *it.second, p_file );
			}
		}

		if ( result )
		{
			Logger::LogInfo( cuT( "Scene::Write - Animated object groups" ) );
			auto lock = make_unique_lock( p_scene.GetAnimatedObjectGroupCache() );

			for ( auto const & it : p_scene.GetAnimatedObjectGroupCache() )
			{
				result &= AnimatedObjectGroup::TextWriter( m_tabs + cuT( "\t" ) )( *it.second, p_file );
			}
		}

		p_file.WriteText( cuT( "}\n" ) );

		if ( result )
		{
			Logger::LogInfo( cuT( "Scene::Write - Windows" ) );
			auto lock = make_unique_lock( p_scene.GetEngine()->GetRenderWindowCache() );

			for ( auto const & it : p_scene.GetEngine()->GetRenderWindowCache() )
			{
				if ( it.second->GetRenderTarget()->GetScene()->GetName() == p_scene.GetName() )
				{
					result &= RenderWindow::TextWriter( m_tabs )( *it.second, p_file );
				}
			}
		}

		return result;
	}

	//*************************************************************************************************

	String Scene::RootNode = cuT( "RootNode" );
	String Scene::CameraRootNode = cuT( "CameraRootNode" );
	String Scene::ObjectRootNode = cuT( "ObjectRootNode" );

	Scene::Scene( String const & p_name, Engine & p_engine )
		: OwnedBy< Engine >{ p_engine }
		, Named{ p_name }
		, m_listener{ p_engine.GetFrameListenerCache().Add( cuT( "Scene_" ) + p_name + string::to_string( (size_t)this ) ) }
		, m_animationUpdater{ std::max( 2u, p_engine.GetCpuInformations().CoreCount() - ( p_engine.IsThreaded() ? 2u : 1u ) ) }
		, m_materialType{ MaterialType::eLegacy }
	{
		auto mergeObject = [this]( auto const & p_source
			, auto & p_destination
			, auto p_element
			, SceneNodeSPtr p_rootCameraNode
			, SceneNodeSPtr p_rootObjectNode )
		{
			if ( p_element->GetParent()->GetName() == p_rootCameraNode->GetName() )
			{
				p_element->Detach();
				p_element->AttachTo( p_rootCameraNode );
			}
			else if ( p_element->GetParent()->GetName() == p_rootObjectNode->GetName() )
			{
				p_element->Detach();
				p_element->AttachTo( p_rootObjectNode );
			}

			String name = p_element->GetName();

			while ( p_destination.has( name ) )
			{
				name = this->GetName() + cuT( "_" ) + name;
			}

			p_element->SetName( name );
			p_destination.insert( name, p_element );
		};
		auto mergeResource = [this]( auto const & p_source
			, auto & p_destination
			, auto p_element )
		{
			String name = p_element->GetName();

			if ( !p_destination.has( name ) )
			{
				name = this->GetName() + cuT( "_" ) + name;
			}

			p_element->SetName( name );
			p_destination.insert( name, p_element );
		};
		auto eventInitialise = [this]( auto p_element )
		{
			this->GetListener().PostEvent( MakeInitialiseEvent( *p_element ) );
		};
		auto eventClean = [this]( auto p_element )
		{
			this->GetListener().PostEvent( MakeCleanupEvent( *p_element ) );
		};
		auto attachObject = []( auto p_element
			, SceneNodeSPtr p_parent
			, SceneNodeSPtr p_rootNode
			, SceneNodeSPtr p_rootCameraNode
			, SceneNodeSPtr p_rootObjectNode )
		{
			if ( p_parent )
			{
				p_parent->AttachObject( *p_element );
			}
			else
			{
				p_rootObjectNode->AttachObject( *p_element );
			}
		};
		auto attachCamera = []( auto p_element
			, SceneNodeSPtr p_parent
			, SceneNodeSPtr p_rootNode
			, SceneNodeSPtr p_rootCameraNode
			, SceneNodeSPtr p_rootObjectNode )
		{
			if ( p_parent )
			{
				p_parent->AttachObject( *p_element );
			}
			else
			{
				p_rootCameraNode->AttachObject( *p_element );
			}
		};
		auto attachNode = []( auto p_element
			, SceneNodeSPtr p_parent
			, SceneNodeSPtr p_rootNode
			, SceneNodeSPtr p_rootCameraNode
			, SceneNodeSPtr p_rootObjectNode )
		{
			if ( p_parent )
			{
				p_element->AttachTo( p_parent );
			}
			else
			{
				p_element->AttachTo( p_rootNode );
			}
		};
		auto dummy = []( auto p_element )
		{
		};

		m_rootNode = std::make_shared< SceneNode >( RootNode, *this );
		m_rootCameraNode = std::make_shared< SceneNode >( CameraRootNode, *this );
		m_rootObjectNode = std::make_shared< SceneNode >( ObjectRootNode, *this );
		m_rootCameraNode->AttachTo( m_rootNode );
		m_rootObjectNode->AttachTo( m_rootNode );

		m_billboardCache = MakeObjectCache< BillboardList, String >( p_engine
			, *this
			, m_rootNode
			, m_rootCameraNode
			, m_rootObjectNode
			, [this]( String const & p_name, SceneNodeSPtr p_parent )
			{
				return std::make_shared< BillboardList >( p_name
					, *this
					, p_parent );
			}
			, eventInitialise
			, eventClean
			, mergeObject
			, attachObject
			, [this]( BillboardListSPtr p_element )
			{
				p_element->Detach();
			} );
		m_cameraCache = MakeObjectCache< Camera, String >( p_engine
			, *this
			, m_rootNode
			, m_rootCameraNode
			, m_rootObjectNode
			, [this]( String const & p_name
				, SceneNodeSPtr p_parent
				, Viewport && p_viewport )
			{
				return std::make_shared< Camera >( p_name
					, *this
					, p_parent
					, std::move( p_viewport ) );
			}
			, dummy
			, dummy
			, mergeObject
			, attachCamera
			, [this]( CameraSPtr p_element )
			{
				p_element->Detach();
			} );
		m_geometryCache = MakeObjectCache< Geometry, String >( p_engine
			, *this
			, m_rootNode
			, m_rootCameraNode
			, m_rootObjectNode
			, [this]( String const & p_name
				, SceneNodeSPtr p_parent
				, MeshSPtr p_mesh )
			{
				return std::make_shared< Geometry >( p_name
					, *this
					, p_parent
					, p_mesh );
			}
			, dummy
			, dummy
			, mergeObject
			, attachObject
			, [this]( GeometrySPtr p_element )
			{
				p_element->Detach();
			} );
		m_lightCache = MakeObjectCache< Light, String >( p_engine
			, *this
			, m_rootNode
			, m_rootCameraNode
			, m_rootObjectNode
			, [this]( String const & p_name
				, SceneNodeSPtr p_node
				, LightType p_lightType )
			{
				return std::make_shared< Light >( p_name
					, *this
					, p_node
					, m_lightFactory
					, p_lightType );
			}
			, dummy
			, dummy
			, mergeObject
			, attachObject
			, [this]( LightSPtr p_element )
			{
				p_element->Detach();
			} );
		m_particleSystemCache = MakeObjectCache< ParticleSystem, String >( p_engine
			, *this
			, m_rootNode
			, m_rootCameraNode
			, m_rootObjectNode
			, [this]( String const & p_name
				, SceneNodeSPtr p_parent
				, uint32_t p_count )
			{
				return std::make_shared< ParticleSystem >( p_name
					, *this
					, p_parent
					, p_count );
			}
			, eventInitialise
			, eventClean
			, mergeObject
			, attachObject
			, [this]( ParticleSystemSPtr p_element )
			{
				p_element->Detach();
			} );
		m_sceneNodeCache = MakeObjectCache< SceneNode, String >( p_engine
			, *this
			, m_rootNode
			, m_rootCameraNode
			, m_rootObjectNode
			, [this]( String const & p_name )
			{
				return std::make_shared< SceneNode >( p_name, *this );
			}
			, dummy
			, dummy
			, mergeObject
			, attachNode
			, [this]( SceneNodeSPtr p_element )
			{
				p_element->Detach();
			} );
		m_animatedObjectGroupCache = MakeCache< AnimatedObjectGroup, String >( p_engine
			, [this]( Castor::String const & p_name )
			{
				return std::make_shared< AnimatedObjectGroup >( p_name, *this );
			}
			, dummy
			, dummy
			, mergeResource );
		m_meshCache = MakeCache< Mesh, String >( p_engine
			, [this]( Castor::String const & p_name )
			{
				return std::make_shared< Mesh >( p_name
					, *this );
			}
			, dummy
			, eventClean
			, mergeResource );

		m_materialCacheView = MakeCacheView< Material, EventType::ePreRender >( GetName()
			, eventInitialise
			, eventClean
			, GetEngine()->GetMaterialCache() );
		m_samplerCacheView = MakeCacheView< Sampler, EventType::ePreRender >( GetName()
			, eventInitialise
			, eventClean
			, GetEngine()->GetSamplerCache() );
		m_overlayCacheView = MakeCacheView< Overlay, EventType::ePreRender >( GetName()
			, std::bind( OverlayCache::OverlayInitialiser{ GetEngine()->GetOverlayCache() }, std::placeholders::_1 )
			, std::bind( OverlayCache::OverlayCleaner{ GetEngine()->GetOverlayCache() }, std::placeholders::_1 )
			, GetEngine()->GetOverlayCache() );
		m_fontCacheView = MakeCacheView< Font, EventType::ePreRender >( GetName()
			, dummy
			, dummy
			, GetEngine()->GetFontCache() );

		auto notify = [this]()
		{
			SetChanged();
		};

		m_sceneNodeCache->Add( cuT( "ObjectRootNode" ), m_rootObjectNode );
		m_sceneNodeCache->Add( cuT( "CameraRootNode" ), m_rootCameraNode );

		m_onParticleSystemChanged = m_particleSystemCache->onChanged.connect( std::bind( &Scene::SetChanged, this ) );
		m_onBillboardListChanged = m_billboardCache->onChanged.connect( std::bind( &Scene::SetChanged, this ) );
		m_onGeometryChanged = m_geometryCache->onChanged.connect( std::bind( &Scene::SetChanged, this ) );
		m_onSceneNodeChanged = m_sceneNodeCache->onChanged.connect( std::bind( &Scene::SetChanged, this ) );
	}

	Scene::~Scene()
	{
		m_onSceneNodeChanged.disconnect();
		m_onGeometryChanged.disconnect();
		m_onBillboardListChanged.disconnect();
		m_onParticleSystemChanged.disconnect();

		m_meshCache->Clear();

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
			m_rootCameraNode->Detach();
			m_rootCameraNode.reset();
		}

		if ( m_rootObjectNode )
		{
			m_rootObjectNode->Detach();
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
			m_rootNode->Detach();
		}

		m_rootNode.reset();
		GetEngine()->GetFrameListenerCache().Remove( m_listener.lock()->GetName() );
	}

	void Scene::Initialise()
	{
		m_lightCache->Initialise();

		GetListener().PostEvent( MakeFunctorEvent( EventType::ePreRender, [this]()
		{
			m_colour = std::make_unique< TextureProjection >( *GetEngine()->GetRenderSystem()->GetCurrentContext() );
			m_colour->Initialise();
		} ) );

		m_initialised = true;
	}

	void Scene::Cleanup()
	{
		m_initialised = false;
		m_animatedObjectGroupCache->Cleanup();
		m_cameraCache->Cleanup();
		m_billboardCache->Cleanup();
		m_particleSystemCache->Cleanup();
		m_geometryCache->Cleanup();
		m_lightCache->Cleanup();
		m_sceneNodeCache->Cleanup();

		for ( auto & pass : m_reflectionMapsArray )
		{
			GetListener().PostEvent( MakeFunctorEvent( EventType::ePreRender
				, [&pass]()
				{
					pass.get().Cleanup();
				} ) );
		}

		m_materialCacheView->Clear();
		m_samplerCacheView->Clear();
		m_overlayCacheView->Clear();
		m_fontCacheView->Clear();

		// These ones, being ResourceCache, need to be cleared in destructor only
		m_meshCache->Cleanup();

		GetListener().PostEvent( MakeFunctorEvent( EventType::ePreRender, [this]()
		{
			if ( m_colour )
			{
				m_colour->Cleanup();
				m_colour.reset();
			}
		} ) );

		if ( m_backgroundImage )
		{
			GetListener().PostEvent( MakeFunctorEvent( EventType::ePreRender, [this]()
			{
				m_backgroundImage->Cleanup();
			} ) );
		}

		if ( m_skybox )
		{
			GetListener().PostEvent( MakeFunctorEvent( EventType::ePreRender, [this]()
			{
				m_skybox->Cleanup();
			} ) );
		}
	}

	void Scene::RenderBackground( Size const & p_size, Camera const & p_camera )
	{
		if ( m_fog.GetType() == GLSL::FogType::eDisabled )
		{
			if ( m_backgroundImage && m_backgroundImage->IsInitialised())
			{
				m_colour->Render( *m_backgroundImage
					, p_camera );
			}
			else if ( m_skybox )
			{
				m_skybox->Render( p_camera );
			}
		}
	}

	void Scene::Update()
	{
		m_rootNode->Update();
		std::vector< std::reference_wrapper< AnimatedObjectGroup > > groups;

		m_animatedObjectGroupCache->ForEach( [&groups]( AnimatedObjectGroup & p_group )
		{
			groups.push_back( p_group );
		} );

		if ( groups.size() > m_animationUpdater.GetCount() )
		{
			for ( auto & group : groups )
			{
				m_animationUpdater.PushJob( [&group]()
				{
					group.get().Update();
				} );
			}

			m_animationUpdater.WaitAll( std::chrono::milliseconds::max() );
		}
		else
		{
			for ( auto & group : groups )
			{
				group.get().Update();
			}
		}

		if ( !m_skybox && !m_backgroundImage )
		{
			m_skybox = std::make_unique< Skybox >( *GetEngine() );
			m_skybox->SetScene( *this );
			Size size{ 16, 16 };
			constexpr PixelFormat format{ PixelFormat::eR8G8B8 };
			UbPixel pixel{ true };
			uint8_t c;
			pixel.set< format >( { { m_backgroundColour.red().convert_to( c )
				, m_backgroundColour.green().convert_to( c )
				, m_backgroundColour.blue().convert_to( c ) } } );
			auto buffer = PxBufferBase::create( size, format );
			auto data = buffer->ptr();

			for ( uint32_t i = 0u; i < 256; ++i )
			{
				std::memcpy( data, pixel.const_ptr(), 3 );
				data += 3;
			}

			m_skybox->GetTexture().GetImage( 0u ).InitialiseSource( buffer );
			m_skybox->GetTexture().GetImage( 1u ).InitialiseSource( buffer );
			m_skybox->GetTexture().GetImage( 2u ).InitialiseSource( buffer );
			m_skybox->GetTexture().GetImage( 3u ).InitialiseSource( buffer );
			m_skybox->GetTexture().GetImage( 4u ).InitialiseSource( buffer );
			m_skybox->GetTexture().GetImage( 5u ).InitialiseSource( buffer );
			GetListener().PostEvent( MakeInitialiseEvent( *m_skybox ) );
		}

		m_changed = false;
	}

	bool Scene::SetBackground( Path const & p_folder, Path const & p_relative )
	{
		bool result = false;

		try
		{
			auto texture = GetEngine()->GetRenderSystem()->CreateTexture( TextureType::eTwoDimensions, AccessType::eNone, AccessType::eRead );
			texture->SetSource( p_folder, p_relative );
			m_backgroundImage = texture;
			GetListener().PostEvent( MakeFunctorEvent( EventType::ePreRender, [this]()
			{
				m_backgroundImage->Initialise();
				m_backgroundImage->Bind( 0 );
				m_backgroundImage->GenerateMipmaps();
				m_backgroundImage->Unbind( 0 );
			} ) );
			result = true;
		}
		catch ( Castor::Exception & p_exc )
		{
			Logger::LogError( p_exc.what() );
		}

		return result;
	}

	bool Scene::SetForeground( SkyboxUPtr && p_skybox )
	{
		m_skybox = std::move( p_skybox );
		m_skybox->SetScene( *this );
		GetListener().PostEvent( MakeFunctorEvent( EventType::ePreRender, [this]()
		{
			m_skybox->Initialise();
		} ) );
		return true;
	}

	void Scene::Merge( SceneSPtr p_scene )
	{
		{
			p_scene->GetAnimatedObjectGroupCache().MergeInto( *m_animatedObjectGroupCache );
			p_scene->GetCameraCache().MergeInto( *m_cameraCache );
			p_scene->GetBillboardListCache().MergeInto( *m_billboardCache );
			p_scene->GetParticleSystemCache().MergeInto( *m_particleSystemCache );
			p_scene->GetGeometryCache().MergeInto( *m_geometryCache );
			p_scene->GetLightCache().MergeInto( *m_lightCache );
			p_scene->GetSceneNodeCache().MergeInto( *m_sceneNodeCache );
			m_ambientLight = p_scene->GetAmbientLight();
			SetChanged();
		}

		p_scene->Cleanup();
	}

	bool Scene::ImportExternal( Path const & p_fileName, Importer & p_importer )
	{
		SetChanged();
		return p_importer.ImportScene( *this, p_fileName, Parameters() );
	}

	uint32_t Scene::GetVertexCount()const
	{
		uint32_t result = 0;
		auto lock = make_unique_lock( *m_geometryCache );

		for ( auto pair : *m_geometryCache )
		{
			auto mesh = pair.second->GetMesh();

			if ( mesh )
			{
				result += pair.second->GetMesh()->GetVertexCount();
			}
		}

		return result;
	}

	uint32_t Scene::GetFaceCount()const
	{
		uint32_t result = 0;
		auto lock = make_unique_lock( *m_geometryCache );

		for ( auto pair : *m_geometryCache )
		{
			auto mesh = pair.second->GetMesh();

			if ( mesh )
			{
				result += mesh->GetFaceCount();
			}
		}

		return result;
	}

	SceneFlags Scene::GetFlags()const
	{
		SceneFlags result;

		switch ( m_fog.GetType() )
		{
		case GLSL::FogType::eLinear:
			result |= SceneFlag::eFogLinear;
			break;

		case GLSL::FogType::eExponential:
			result |= SceneFlag::eFogExponential;
			break;

		case GLSL::FogType::eSquaredExponential:
			result |= SceneFlag::eFogSquaredExponential;
			break;
		}

		if ( HasShadows() )
		{
			switch ( m_shadow.GetFilterType() )
			{
			case GLSL::ShadowType::eRaw:
				result |= SceneFlag::eShadowFilterRaw;
				break;

			case GLSL::ShadowType::ePoisson:
				result |= SceneFlag::eShadowFilterPoisson;
				break;

			case GLSL::ShadowType::eStratifiedPoisson:
				result |= SceneFlag::eShadowFilterStratifiedPoisson;
				break;
			}
		}

		switch ( m_materialType )
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

	bool Scene::HasShadows()const
	{
		auto lock = make_unique_lock( GetLightCache() );

		return GetLightCache().end() != std::find_if( GetLightCache().begin(), GetLightCache().end(), []( std::pair< String, LightSPtr > const & p_it )
		{
			return p_it.second->IsShadowProducer();
		} );
	}

	void Scene::CreateEnvironmentMap( SceneNode & p_node )
	{
		if ( !HasEnvironmentMap( p_node ) )
		{
			auto it = m_reflectionMaps.emplace( &p_node, std::make_unique< EnvironmentMap >( *GetEngine(), p_node ) ).first;
			auto & pass = *it->second;
			m_reflectionMapsArray.emplace_back( pass );

			GetListener().PostEvent( MakeFunctorEvent( EventType::ePreRender
				, [&pass]()
				{
					pass.Initialise();
				} ) );
		}
	}

	bool Scene::HasEnvironmentMap( SceneNode const & p_node )const
	{
		return m_reflectionMaps.end() != m_reflectionMaps.find( &p_node );
	}

	EnvironmentMap & Scene::GetEnvironmentMap( SceneNode const & p_node )
	{
		REQUIRE( HasEnvironmentMap( p_node ) );
		return *m_reflectionMaps.find( &p_node )->second;
	}

	IblTextures const & Scene::GetIbl( SceneNode const & p_node )const
	{
		REQUIRE( m_skybox );

		if ( !HasEnvironmentMap( p_node ) )
		{
			return m_skybox->GetIbl();
		}

		return m_skybox->GetIbl();
	}
}
