#include "SceneManager.hpp"

#include "AnimatedObjectGroupManager.hpp"
#include "CameraManager.hpp"
#include "BillboardManager.hpp"
#include "Engine.hpp"
#include "GeometryManager.hpp"
#include "LightManager.hpp"
#include "MaterialManager.hpp"
#include "MeshManager.hpp"
#include "OverlayManager.hpp"
#include "SamplerManager.hpp"
#include "SceneNodeManager.hpp"
#include "WindowManager.hpp"

#include "Skybox.hpp"

#include "Manager/ManagerView.hpp"
#include "Mesh/Importer.hpp"
#include "Render/RenderLoop.hpp"
#include "State/DepthStencilState.hpp"
#include "Texture/TextureLayout.hpp"

#include <Image.hpp>
#include <Logger.hpp>

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	namespace
	{
		template< typename ResourceType, eEVENT_TYPE EventType, typename ManagerType >
		std::unique_ptr< ManagerView< ResourceType, ManagerType, EventType > > make_manager_view( Castor::String const & p_name, ManagerType & p_manager )
		{
			return std::make_unique< ManagerView< ResourceType, ManagerType, EventType > >( p_name, p_manager );
		}
	}

	//*************************************************************************************************

	template<>
	inline void ManagerView< Overlay, OverlayManager, eEVENT_TYPE_PRE_RENDER >::Clear()
	{
		for ( auto l_name : m_createdElements )
		{
			auto l_resource = m_manager.Find( l_name );

			if ( l_resource )
			{
				m_manager.Remove( l_name );
			}
		}
	}

	//*************************************************************************************************

	template<>
	inline void ManagerView< Font, FontManager, eEVENT_TYPE_PRE_RENDER >::Clear()
	{
		for ( auto l_name : m_createdElements )
		{
			auto l_resource = m_manager.Find( l_name );

			if ( l_resource )
			{
				m_manager.Remove( l_name );
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
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "scene \"" ) + p_scene.GetName() + cuT( "\"\n" ) ) > 0
				&& p_file.WriteText( m_tabs + cuT( "{" ) ) > 0;
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
		}

		if ( l_return && p_scene.GetBackgroundImage() )
		{
			Logger::LogInfo( cuT( "Scene::Write - Background image" ) );
			Path l_relative = Scene::TextWriter::CopyFile( Path{ p_scene.GetBackgroundImage()->GetImage().ToString() }, p_file.GetFilePath(), Path{ cuT( "Textures" ) } );
			l_return = p_file.WriteText( m_tabs + cuT( "\tbackground_image \"" ) + l_relative + cuT( "\"\n" ) ) > 0;
		}

		if ( l_return )
		{
			Logger::LogInfo( cuT( "Scene::Write - Ambient light" ) );
			l_return = p_file.Print( 256, cuT( "%s\tambient_light " ), m_tabs.c_str() ) > 0
				&& Colour::TextWriter( String() )( p_scene.GetAmbientLight(), p_file )
				&& p_file.WriteText( cuT( "\n" ) ) > 0;
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
			auto l_lock = make_unique_lock( p_scene.GetCameraManager() );

			for ( auto const & l_it : p_scene.GetCameraManager() )
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
			auto l_lock = make_unique_lock( p_scene.GetLightManager() );

			for ( auto const & l_it : p_scene.GetLightManager() )
			{
				l_return &= Light::TextWriter( m_tabs + cuT( "\t" ) )( *l_it.second, p_file );
			}
		}

		if ( l_return )
		{
			Logger::LogInfo( cuT( "Scene::Write - Geometries" ) );
			auto l_lock = make_unique_lock( p_scene.GetGeometryManager() );

			for ( auto const & l_it : p_scene.GetGeometryManager() )
			{
				l_return &= Geometry::TextWriter( m_tabs + cuT( "\t" ) )( *l_it.second, p_file );
			}
		}

		if ( l_return )
		{
			Logger::LogInfo( cuT( "Scene::Write - Animated object groups" ) );
			auto l_lock = make_unique_lock( p_scene.GetAnimatedObjectGroupManager() );

			for ( auto const & l_it : p_scene.GetAnimatedObjectGroupManager() )
			{
				l_return &= AnimatedObjectGroup::TextWriter( m_tabs + cuT( "\t" ) )( *l_it.second, p_file );
			}
		}

		if ( l_return )
		{
			Logger::LogInfo( cuT( "Scene::Write - Windows" ) );
			auto l_lock = make_unique_lock( p_scene.GetWindowManager() );

			for ( auto const & l_it : p_scene.GetWindowManager() )
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

	Scene::Scene( String const & p_name, Engine & p_engine )
		: OwnedBy< Engine >{ p_engine }
		, Named( p_name )
		, m_rootCameraNode()
		, m_rootObjectNode()
		, m_changed( false )
	{
		m_rootNode = std::make_shared< SceneNode >( RootNode, *this );
		m_rootCameraNode = std::make_shared< SceneNode >( CameraRootNode, *this );
		m_rootObjectNode = std::make_shared< SceneNode >( ObjectRootNode, *this );
		m_rootCameraNode->AttachTo( m_rootNode );
		m_rootObjectNode->AttachTo( m_rootNode );

		m_animatedObjectGroupManager = std::make_unique< AnimatedObjectGroupManager >( *this, m_rootNode, m_rootCameraNode, m_rootObjectNode );
		m_billboardManager = std::make_unique< BillboardManager >( *this, m_rootNode, m_rootCameraNode, m_rootObjectNode );
		m_cameraManager = std::make_unique< CameraManager >( *this, m_rootNode, m_rootCameraNode, m_rootObjectNode );
		m_geometryManager = std::make_unique< GeometryManager >( *this, m_rootNode, m_rootCameraNode, m_rootObjectNode );
		m_meshManager = std::make_unique< MeshManager >( *this );
		m_lightManager = std::make_unique< LightManager >( *this, m_rootNode, m_rootCameraNode, m_rootObjectNode );
		m_sceneNodeManager = std::make_unique< SceneNodeManager >( *this, m_rootNode, m_rootCameraNode, m_rootObjectNode );
		m_windowManager = std::make_unique< WindowManager >( *this );

		m_materialManagerView = make_manager_view< Material, eEVENT_TYPE_PRE_RENDER >( GetName(), GetEngine()->GetMaterialManager() );
		m_samplerManagerView = make_manager_view< Sampler, eEVENT_TYPE_PRE_RENDER >( GetName(), GetEngine()->GetSamplerManager() );
		m_overlayManagerView = make_manager_view< Overlay, eEVENT_TYPE_PRE_RENDER >( GetName(), GetEngine()->GetOverlayManager() );
		m_fontManagerView = make_manager_view< Font, eEVENT_TYPE_PRE_RENDER >( GetName(), GetEngine()->GetFontManager() );

		m_meshManager->SetRenderSystem( p_engine.GetRenderSystem() );
		m_windowManager->SetRenderSystem( p_engine.GetRenderSystem() );

		auto l_notify = [this]()
		{
			m_changed = true;
		};

		m_sceneNodeManager->Insert( cuT( "ObjectRootNode" ), m_rootObjectNode );
	}

	Scene::~Scene()
	{
		m_meshManager->Clear();
		m_windowManager->Clear();

		m_skybox.reset();
		m_animatedObjectGroupManager.reset();
		m_billboardManager.reset();
		m_cameraManager.reset();
		m_geometryManager.reset();
		m_lightManager.reset();
		m_sceneNodeManager.reset();

		m_meshManager.reset();
		m_materialManagerView.reset();
		m_samplerManagerView.reset();
		m_windowManager.reset();
		m_overlayManagerView.reset();
		m_fontManagerView.reset();

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
	}

	void Scene::Cleanup()
	{
		auto l_lock = Castor::make_unique_lock( m_mutex );
		m_overlays.clear();
		m_animatedObjectGroupManager->Cleanup();
		m_cameraManager->Cleanup();
		m_billboardManager->Cleanup();
		m_geometryManager->Cleanup();
		m_lightManager->Cleanup();
		m_sceneNodeManager->Cleanup();

		m_materialManagerView->Clear();
		m_samplerManagerView->Clear();
		m_overlayManagerView->Clear();
		m_fontManagerView->Clear();

		// Those two ones, being ResourceManager, need to be cleared in destructor only
		m_meshManager->Cleanup();
		m_windowManager->Cleanup();

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
		m_animatedObjectGroupManager->Update();
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
			p_scene->GetAnimatedObjectGroupManager().MergeInto( *m_animatedObjectGroupManager );
			p_scene->GetCameraManager().MergeInto( *m_cameraManager );
			p_scene->GetBillboardManager().MergeInto( *m_billboardManager );
			p_scene->GetGeometryManager().MergeInto( *m_geometryManager );
			p_scene->GetLightManager().MergeInto( *m_lightManager );
			p_scene->GetSceneNodeManager().MergeInto( *m_sceneNodeManager );
			m_ambientLight = p_scene->GetAmbientLight();
			m_changed = true;
		}

		p_scene->Cleanup();
	}

	bool Scene::ImportExternal( Path const & p_fileName, Importer & p_importer )
	{
		auto l_lock = Castor::make_unique_lock( m_mutex );
		bool l_return = true;
		SceneSPtr l_scene = p_importer.ImportScene( p_fileName, Parameters() );

		if ( l_scene )
		{
			Merge( l_scene );
			m_changed = true;
			l_return = true;
		}

		return l_return;
	}

	MeshSPtr Scene::ImportMesh( Castor::Path const & p_fileName, Importer & p_importer, Parameters const & p_parameters )
	{
		auto l_return = p_importer.ImportMesh( *this, p_fileName, p_parameters );

		if ( l_return )
		{
			m_meshManager->Insert( l_return->GetName(), l_return );
		}

		return l_return;
	}

	uint32_t Scene::GetVertexCount()const
	{
		uint32_t l_return = 0;
		auto l_lock = make_unique_lock( *m_geometryManager );

		for ( auto && l_pair : *m_geometryManager )
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
		auto l_lock = make_unique_lock( *m_geometryManager );

		for ( auto && l_pair : *m_geometryManager )
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
