#include "Scene.hpp"

#include "Engine.hpp"
#include "Camera.hpp"
#include "BillboardList.hpp"
#include "Geometry.hpp"
#include "SceneNode.hpp"
#include "Skybox.hpp"

#include "Animation/AnimatedObjectGroup.hpp"
#include "Cache/CacheView.hpp"
#include "Event/Frame/CleanupEvent.hpp"
#include "Light/Light.hpp"
#include "Material/Material.hpp"
#include "Mesh/Importer.hpp"
#include "Mesh/Mesh.hpp"
#include "Overlay/Overlay.hpp"
#include "Render/RenderLoop.hpp"
#include "Render/RenderSystem.hpp"
#include "Render/RenderWindow.hpp"
#include "State/DepthStencilState.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <Graphics/Font.hpp>
#include <Graphics/Image.hpp>
#include <Log/Logger.hpp>

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	namespace
	{
		template< typename ResourceType, eEVENT_TYPE EventType, typename CacheType >
		std::unique_ptr< CacheView< ResourceType, CacheType, EventType > > MakeCacheView( Castor::String const & p_name, CacheType & p_cache )
		{
			return std::make_unique< CacheView< ResourceType, CacheType, EventType > >( p_name, p_cache );
		}
	}

	//*************************************************************************************************

	template<>
	inline void CacheView< Overlay, OverlayCache, eEVENT_TYPE_PRE_RENDER >::Clear()
	{
		for ( auto l_name : m_createdElements )
		{
			auto l_resource = m_cache.Find( l_name );

			if ( l_resource )
			{
				m_cache.Remove( l_name );
			}
		}
	}

	//*************************************************************************************************

	template<>
	inline void CacheView< Font, FontCache, eEVENT_TYPE_PRE_RENDER >::Clear()
	{
		for ( auto l_name : m_createdElements )
		{
			auto l_resource = m_cache.Find( l_name );

			if ( l_resource )
			{
				m_cache.Remove( l_name );
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
		Logger::LogInfo( cuT( "Scene::Write - Scene Name" ) );

		bool l_return = true;

		if ( p_scene.GetEngine()->GetRenderLoop().GetShowDebugOverlays() )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "debug_overlays true\n" ) ) > 0;
			Castor::TextWriter< Scene >::CheckError( l_return, "Scene debug overlays" );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "scene \"" ) + p_scene.GetName() + cuT( "\"\n" ) ) > 0
				&& p_file.WriteText( m_tabs + cuT( "{" ) ) > 0;
			Castor::TextWriter< Scene >::CheckError( l_return, "Scene name" );
		}

		if ( l_return )
		{
			Logger::LogInfo( cuT( "Scene::Write - Fonts" ) );
			for ( auto const & l_name : p_scene.GetFontView() )
			{
				auto l_font = p_scene.GetFontView().Find( l_name );
				l_return &= Font::TextWriter( m_tabs + cuT( "\t" ) )( *l_font, p_file );
			}
		}

		if ( l_return )
		{
			Logger::LogInfo( cuT( "Scene::Write - Samplers" ) );
			for ( auto const & l_name : p_scene.GetSamplerView() )
			{
				auto l_sampler = p_scene.GetSamplerView().Find( l_name );
				l_return &= Sampler::TextWriter( m_tabs + cuT( "\t" ) )( *l_sampler, p_file );
			}
		}

		if ( l_return )
		{
			Logger::LogInfo( cuT( "Scene::Write - Materials" ) );
			for ( auto const & l_name : p_scene.GetMaterialView() )
			{
				auto l_material = p_scene.GetMaterialView().Find( l_name );
				l_return &= Material::TextWriter( m_tabs + cuT( "\t" ) )( *l_material, p_file );
			}
		}

		if ( l_return )
		{
			Logger::LogInfo( cuT( "Scene::Write - Overlays" ) );
			for ( auto const & l_name : p_scene.GetOverlayView() )
			{
				auto l_overlay = p_scene.GetOverlayView().Find( l_name );

				if ( !l_overlay->GetParent() )
				{
					l_return &= Overlay::TextWriter( m_tabs + cuT( "\t" ) )( *l_overlay, p_file );
				}
			}
		}

		if ( l_return )
		{
			Logger::LogInfo( cuT( "Scene::Write - Background colour" ) );
			l_return = p_file.Print( 256, cuT( "\n%s\tbackground_colour " ), m_tabs.c_str() ) > 0
				&& Colour::TextWriter( String() )( p_scene.GetBackgroundColour(), p_file )
				&& p_file.WriteText( cuT( "\n" ) ) > 0;
			Castor::TextWriter< Scene >::CheckError( l_return, "Scene background colour" );
		}

		if ( l_return && p_scene.GetBackgroundImage() )
		{
			Logger::LogInfo( cuT( "Scene::Write - Background image" ) );
			Path l_relative = Scene::TextWriter::CopyFile( Path{ p_scene.GetBackgroundImage()->GetImage().ToString() }, p_file.GetFilePath(), Path{ cuT( "Textures" ) } );
			l_return = p_file.WriteText( m_tabs + cuT( "\tbackground_image \"" ) + l_relative + cuT( "\"\n" ) ) > 0;
			Castor::TextWriter< Scene >::CheckError( l_return, "Scene background image" );
		}

		if ( l_return )
		{
			Logger::LogInfo( cuT( "Scene::Write - Ambient light" ) );
			l_return = p_file.Print( 256, cuT( "%s\tambient_light " ), m_tabs.c_str() ) > 0
				&& Colour::TextWriter( String() )( p_scene.GetAmbientLight(), p_file )
				&& p_file.WriteText( cuT( "\n" ) ) > 0;
			Castor::TextWriter< Scene >::CheckError( l_return, "Scene ambient light" );
		}

		if ( l_return && p_scene.GetSkybox() )
		{
			Logger::LogInfo( cuT( "Scene::Write - Skybox" ) );
			l_return = Skybox::TextWriter( m_tabs + cuT( "\t" ) )( *p_scene.GetSkybox(), p_file );
		}

		if ( l_return )
		{
			Logger::LogInfo( cuT( "Scene::Write - Cameras nodes" ) );
			for ( auto const & l_it : p_scene.GetCameraRootNode()->GetChilds() )
			{
				if ( l_return
					 && l_it.first.find( cuT( "_REye" ) ) == String::npos
					 && l_it.first.find( cuT( "_LEye" ) ) == String::npos )
				{
					l_return = SceneNode::TextWriter( m_tabs + cuT( "\t" ) )( *l_it.second.lock(), p_file );
				}
			}
		}

		if ( l_return )
		{
			Logger::LogInfo( cuT( "Scene::Write - Objects nodes" ) );
			for ( auto const & l_it : p_scene.GetObjectRootNode()->GetChilds() )
			{
				l_return &= SceneNode::TextWriter( m_tabs + cuT( "\t" ) )( *l_it.second.lock(), p_file );
			}
		}

		if ( l_return )
		{
			Logger::LogInfo( cuT( "Scene::Write - Cameras" ) );
			auto l_lock = make_unique_lock( p_scene.GetCameraCache() );

			for ( auto const & l_it : p_scene.GetCameraCache() )
			{
				if ( l_return
					 && l_it.first.find( cuT( "_REye" ) ) == String::npos
					 && l_it.first.find( cuT( "_LEye" ) ) == String::npos )
				{
					l_return = Camera::TextWriter( m_tabs + cuT( "\t" ) )( *l_it.second, p_file );
				}
			}
		}

		if ( l_return )
		{
			Logger::LogInfo( cuT( "Scene::Write - Lights" ) );
			auto l_lock = make_unique_lock( p_scene.GetLightCache() );

			for ( auto const & l_it : p_scene.GetLightCache() )
			{
				l_return &= Light::TextWriter( m_tabs + cuT( "\t" ) )( *l_it.second, p_file );
			}
		}

		if ( l_return )
		{
			Logger::LogInfo( cuT( "Scene::Write - Geometries" ) );
			auto l_lock = make_unique_lock( p_scene.GetGeometryCache() );

			for ( auto const & l_it : p_scene.GetGeometryCache() )
			{
				l_return &= Geometry::TextWriter( m_tabs + cuT( "\t" ) )( *l_it.second, p_file );
			}
		}

		if ( l_return )
		{
			Logger::LogInfo( cuT( "Scene::Write - Animated object groups" ) );
			auto l_lock = make_unique_lock( p_scene.GetAnimatedObjectGroupCache() );

			for ( auto const & l_it : p_scene.GetAnimatedObjectGroupCache() )
			{
				l_return &= AnimatedObjectGroup::TextWriter( m_tabs + cuT( "\t" ) )( *l_it.second, p_file );
			}
		}

		if ( l_return )
		{
			Logger::LogInfo( cuT( "Scene::Write - Windows" ) );
			auto l_lock = make_unique_lock( p_scene.GetRenderWindowCache() );

			for ( auto const & l_it : p_scene.GetRenderWindowCache() )
			{
				l_return &= RenderWindow::TextWriter( m_tabs + cuT( "\t" ) )( *l_it.second, p_file );
			}
		}

		p_file.WriteText( cuT( "}\n" ) );
		return l_return;
	}

	//*************************************************************************************************

	String Scene::RootNode = cuT( "RootNode" );
	String Scene::CameraRootNode = cuT( "CameraRootNode" );
	String Scene::ObjectRootNode = cuT( "ObjectRootNode" );

	namespace
	{
	}

	Scene::Scene( String const & p_name, Engine & p_engine )
		: OwnedBy< Engine >{ p_engine }
		, Named( p_name )
		, m_rootCameraNode()
		, m_rootObjectNode()
		, m_changed( false )
	{
		auto l_mergeObject = [this]( auto const & p_source
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

			String l_name = p_element->GetName();

			while ( p_destination.has( l_name ) )
			{
				l_name = this->GetName() + cuT( "_" ) + l_name;
			}

			p_element->SetName( l_name );
			p_destination.insert( l_name, p_element );
		};
		auto l_mergeResource = [this]( auto const & p_source
								  , auto & p_destination
								  , auto p_element )
		{
			String l_name = p_element->GetName();

			if ( !p_destination.has( l_name ) )
			{
				l_name = this->GetName() + cuT( "_" ) + l_name;
			}

			p_element->SetName( l_name );
			p_destination.insert( l_name, p_element );
		};
		auto l_eventClean = [this]( auto p_element )
		{
			GetEngine()->PostEvent( MakeCleanupEvent( *p_element ) );
		};
		auto l_attachObject = []( auto p_element
								 , SceneNodeSPtr p_parent
								 , SceneNodeSPtr p_rootNode
								 , SceneNodeSPtr p_rootCameraNode
								 , SceneNodeSPtr p_rootObjectNode )
		{
			if ( p_parent )
			{
				p_parent->AttachObject( p_element );
			}
			else
			{
				p_rootCameraNode->AttachObject( p_element );
			}
		};
		auto l_attachCamera = []( auto p_element
								 , SceneNodeSPtr p_parent
								 , SceneNodeSPtr p_rootNode
								 , SceneNodeSPtr p_rootCameraNode
								 , SceneNodeSPtr p_rootObjectNode )
		{
			if ( p_parent )
			{
				p_parent->AttachObject( p_element );
			}
			else
			{
				p_rootCameraNode->AttachObject( p_element );
			}
		};
		auto l_attachNode = []( auto p_element
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
		auto l_dummy = []( auto p_element )
		{
		};

		m_rootNode = std::make_shared< SceneNode >( RootNode, *this );
		m_rootCameraNode = std::make_shared< SceneNode >( CameraRootNode, *this );
		m_rootObjectNode = std::make_shared< SceneNode >( ObjectRootNode, *this );
		m_rootCameraNode->AttachTo( m_rootNode );
		m_rootObjectNode->AttachTo( m_rootNode );

		m_billboardCache = MakeObjectCache< BillboardList, String >( p_engine, *this, m_rootNode, m_rootCameraNode, m_rootObjectNode
																	, [this]( String const & p_name, SceneNodeSPtr p_parent )
																	{
																		return std::make_shared< BillboardList >( p_name, *this, p_parent, *GetEngine()->GetRenderSystem() );
																	}
																	, l_dummy
																	, l_dummy
																	, l_mergeObject
																	, l_attachObject
																	, [this]( BillboardListSPtr p_element )
																	{
																		p_element->Detach();
																	} );
		m_cameraCache = MakeObjectCache< Camera, String >(	p_engine, *this, m_rootNode, m_rootCameraNode, m_rootObjectNode
															, [this]( String const & p_name, SceneNodeSPtr p_parent, Viewport const & p_viewport )
															{
																return std::make_shared< Camera >( p_name, *this, p_parent, p_viewport );
															}
															, l_dummy
															, l_dummy
															, l_mergeObject
															, l_attachCamera
															, [this]( CameraSPtr p_element )
															{
																p_element->Detach();
															} );
		m_geometryCache = MakeObjectCache< Geometry, String >(	p_engine, *this, m_rootNode, m_rootCameraNode, m_rootObjectNode
																, [this]( String const & p_name, SceneNodeSPtr p_parent, MeshSPtr p_mesh )
																{
																	return std::make_shared< Geometry >( p_name, *this, p_parent, p_mesh );
																}
																, l_dummy
																, l_dummy
																, l_mergeObject
																, l_attachObject
																, [this]( GeometrySPtr p_element )
																{
																	p_element->Detach();
																} );
		m_lightCache = MakeObjectCache< Light, String >( p_engine, *this, m_rootNode, m_rootCameraNode, m_rootObjectNode
														, [this]( String const & p_name, SceneNodeSPtr p_node, eLIGHT_TYPE p_lightType )
														{
															return std::make_shared< Light >( p_name, *this, p_node, m_lightFactory, p_lightType );
														}
														, l_dummy
														, l_dummy
														, l_mergeObject
														, l_attachObject
														, [this]( LightSPtr p_element )
														{
															p_element->Detach();
														} );
		m_sceneNodeCache = MakeObjectCache< SceneNode, String >( p_engine, *this, m_rootNode, m_rootCameraNode, m_rootObjectNode
																, [this]( String const & p_name )
																{
																	return std::make_shared< SceneNode >( p_name, *this );
																}
																, l_dummy
																, l_dummy
																, l_mergeObject
																, l_attachNode
																, [this]( SceneNodeSPtr p_element )
																{
																	p_element->Detach();
																} );
		m_animatedObjectGroupCache = MakeCache< AnimatedObjectGroup, String >(	p_engine
																				, [this]( Castor::String const & p_name )
																				{
																					return std::make_shared< AnimatedObjectGroup >( p_name, *this );
																				}
																				, l_dummy
																				, l_dummy
																				, l_mergeResource );
		m_meshCache = MakeCache< Mesh, String >( p_engine
												, [this]( Castor::String const & p_name )
												{
													return std::make_shared< Mesh >( p_name, *this );
												}
												, l_dummy
												, l_eventClean
												, l_mergeResource );
		m_windowCache = MakeCache < RenderWindow, String >(	p_engine
															, [this]( Castor::String const & p_name )
															{
																return std::make_shared< RenderWindow >( p_name, *GetEngine() );
															}
															, l_dummy
															, l_eventClean
															, l_mergeResource );

		m_materialCacheView = MakeCacheView< Material, eEVENT_TYPE_PRE_RENDER >( GetName(), GetEngine()->GetMaterialCache() );
		m_samplerCacheView = MakeCacheView< Sampler, eEVENT_TYPE_PRE_RENDER >( GetName(), GetEngine()->GetSamplerCache() );
		m_overlayCacheView = MakeCacheView< Overlay, eEVENT_TYPE_PRE_RENDER >( GetName(), GetEngine()->GetOverlayCache() );
		m_fontCacheView = MakeCacheView< Font, eEVENT_TYPE_PRE_RENDER >( GetName(), GetEngine()->GetFontCache() );

		auto l_notify = [this]()
		{
			m_changed = true;
		};

		m_sceneNodeCache->Add( cuT( "ObjectRootNode" ), m_rootObjectNode );

		m_onBillboardListChanged = m_billboardCache->m_onChanged.connect( std::bind( &Scene::SetChanged, this ) );
		m_onGeometryChanged = m_geometryCache->m_onChanged.connect( std::bind( &Scene::SetChanged, this ) );
		m_onSceneNodeChanged = m_sceneNodeCache->m_onChanged.connect( std::bind( &Scene::SetChanged, this ) );
	}

	Scene::~Scene()
	{
		m_sceneNodeCache->m_onChanged.disconnect( m_onSceneNodeChanged );
		m_geometryCache->m_onChanged.disconnect( m_onGeometryChanged );
		m_billboardCache->m_onChanged.disconnect( m_onBillboardListChanged );

		m_meshCache->Clear();
		m_windowCache->Clear();

		m_skybox.reset();
		m_animatedObjectGroupCache.reset();
		m_billboardCache.reset();
		m_cameraCache.reset();
		m_geometryCache.reset();
		m_lightCache.reset();
		m_sceneNodeCache.reset();

		m_meshCache.reset();
		m_materialCacheView.reset();
		m_samplerCacheView.reset();
		m_windowCache.reset();
		m_overlayCacheView.reset();
		m_fontCacheView.reset();

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

		if ( m_rootNode )
		{
			m_rootNode->Detach();
		}

		m_rootNode.reset();
	}

	void Scene::Initialise()
	{
		m_lightCache->Initialise();
	}

	void Scene::Cleanup()
	{
		auto l_lock = Castor::make_unique_lock( m_mutex );
		m_overlays.clear();
		m_animatedObjectGroupCache->Cleanup();
		m_cameraCache->Cleanup();
		m_billboardCache->Cleanup();
		m_geometryCache->Cleanup();
		m_lightCache->Cleanup();
		m_sceneNodeCache->Cleanup();

		m_materialCacheView->Clear();
		m_samplerCacheView->Clear();
		m_overlayCacheView->Clear();
		m_fontCacheView->Clear();

		// Those two ones, being ResourceCache, need to be cleared in destructor only
		m_meshCache->Cleanup();
		m_windowCache->Cleanup();

		if ( m_backgroundImage )
		{
			GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [this]()
			{
				m_backgroundImage->Cleanup();
				m_backgroundImage->Destroy();
			} ) );
		}

		if ( m_skybox )
		{
			GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [this]()
			{
				m_skybox->Cleanup();
			} ) );
		}
	}

	void Scene::RenderBackground( Size const & p_size, Pipeline & p_pipeline )
	{
		if ( m_backgroundImage )
		{
			if ( m_backgroundImage->IsInitialised() )
			{
				ContextRPtr l_context = GetEngine()->GetRenderSystem()->GetCurrentContext();
				l_context->GetNoDepthState()->Apply();
				l_context->RenderTexture( p_size, *m_backgroundImage );
			}
		}
	}

	void Scene::RenderForeground( Size const & p_size, Camera const & p_camera, Pipeline & p_pipeline )
	{
		if ( m_skybox )
		{
			m_skybox->Render( p_camera, p_pipeline );
		}
	}

	void Scene::Update()
	{
		auto l_lock = make_unique_lock( *m_animatedObjectGroupCache );

		for ( auto l_pair : *m_animatedObjectGroupCache )
		{
			l_pair.second->Update();
		}

		m_changed = false;
	}

	bool Scene::SetBackground( Path const & p_folder, Path const & p_relative )
	{
		bool l_return = false;

		try
		{
			auto l_texture = GetEngine()->GetRenderSystem()->CreateTexture( TextureType::TwoDimensions, 0, eACCESS_TYPE_READ );
			l_texture->GetImage().SetSource( p_folder, p_relative );
			m_backgroundImage = l_texture;
			GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [this]()
			{
				m_backgroundImage->Create();
				m_backgroundImage->Initialise();
				m_backgroundImage->Bind( 0 );
				m_backgroundImage->GenerateMipmaps();
				m_backgroundImage->Unbind( 0 );
			} ) );
			l_return = true;
		}
		catch ( Castor::Exception & p_exc )
		{
			Logger::LogError( p_exc.what() );
		}

		return l_return;
	}

	bool Scene::SetForeground( SkyboxSPtr p_skybox )
	{
		m_skybox = p_skybox;
		GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_skybox]()
		{
			p_skybox->Initialise();
		} ) );
		return true;
	}

	void Scene::Merge( SceneSPtr p_scene )
	{
		{
			auto l_lock = Castor::make_unique_lock( m_mutex );
			auto l_lockOther = Castor::make_unique_lock( p_scene->m_mutex );
			p_scene->GetAnimatedObjectGroupCache().MergeInto( *m_animatedObjectGroupCache );
			p_scene->GetCameraCache().MergeInto( *m_cameraCache );
			p_scene->GetBillboardListCache().MergeInto( *m_billboardCache );
			p_scene->GetGeometryCache().MergeInto( *m_geometryCache );
			p_scene->GetLightCache().MergeInto( *m_lightCache );
			p_scene->GetSceneNodeCache().MergeInto( *m_sceneNodeCache );
			m_ambientLight = p_scene->GetAmbientLight();
			m_changed = true;
		}

		p_scene->Cleanup();
	}

	bool Scene::ImportExternal( Path const & p_fileName, Importer & p_importer )
	{
		auto l_lock = Castor::make_unique_lock( m_mutex );
		m_changed = true;
		return p_importer.ImportScene( *this, p_fileName, Parameters() );
	}

	uint32_t Scene::GetVertexCount()const
	{
		uint32_t l_return = 0;
		auto l_lock = make_unique_lock( *m_geometryCache );

		for ( auto l_pair : *m_geometryCache )
		{
			auto l_mesh = l_pair.second->GetMesh();

			if ( l_mesh )
			{
				l_return += l_pair.second->GetMesh()->GetVertexCount();
			}
		}

		return l_return;
	}

	uint32_t Scene::GetFaceCount()const
	{
		uint32_t l_return = 0;
		auto l_lock = make_unique_lock( *m_geometryCache );

		for ( auto l_pair : *m_geometryCache )
		{
			auto l_mesh = l_pair.second->GetMesh();

			if ( l_mesh )
			{
				l_return += l_mesh->GetFaceCount();
			}
		}

		return l_return;
	}
}
