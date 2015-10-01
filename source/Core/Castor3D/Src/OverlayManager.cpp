#include "OverlayManager.hpp"

#include "CleanupEvent.hpp"
#include "FunctorEvent.hpp"
#include "Overlay.hpp"
#include "OverlayRenderer.hpp"
#include "PanelOverlay.hpp"
#include "BorderPanelOverlay.hpp"
#include "TextOverlay.hpp"
#include "Engine.hpp"
#include "SceneFileParser.hpp"
#include "RenderSystem.hpp"
#include "Scene.hpp"
#include "Pipeline.hpp"

using namespace Castor;

#if defined( DrawText )
#	undef DrawText
#endif

namespace Castor3D
{
	struct OverlayInitialiser
	{
		void operator()( OverlaySPtr p_overlay )
		{
			p_overlay->Initialise();
		}
	};

	//*************************************************************************************************

	OverlayManager::OverlayManager( Engine * p_engine )
		: m_engine( p_engine )
		, m_overlayCountPerLevel( 1000, 0 )
	{
	}

	OverlayManager::~OverlayManager()
	{
	}

	void OverlayManager::Clear()
	{
		Castor::Collection< Overlay, Castor::String >::lock();
		Castor::Collection< Overlay, Castor::String >::clear();
		m_overlays.clear();
		Castor::Collection< Overlay, Castor::String >::unlock();
	}

	void OverlayManager::Cleanup()
	{
		Castor::Collection< Overlay, Castor::String >::lock();

		for ( auto && l_overlay : *this )
		{
			m_engine->PostEvent( MakeCleanupEvent( *l_overlay ) );
		}

		Castor::Collection< Overlay, Castor::String >::unlock();
	}

	PanelOverlaySPtr OverlayManager::CreatePanel( String const & p_name, Point2d const & p_position, Point2d const & p_size, MaterialSPtr p_material, OverlaySPtr p_parent )
	{
		OverlaySPtr l_overlay = CreateOverlay( eOVERLAY_TYPE_PANEL, p_name, p_parent, nullptr );
		l_overlay->SetPosition( p_position );
		l_overlay->SetSize( p_size );
		return l_overlay->GetPanelOverlay();
	}

	PanelOverlaySPtr OverlayManager::CreatePanel( String const & p_name, Position const & p_position, Size const & p_size, MaterialSPtr p_material, OverlaySPtr p_parent )
	{
		OverlaySPtr l_overlay = CreateOverlay( eOVERLAY_TYPE_PANEL, p_name, p_parent, nullptr );
		l_overlay->SetPixelPosition( p_position );
		l_overlay->SetPixelSize( p_size );
		return l_overlay->GetPanelOverlay();
	}

	BorderPanelOverlaySPtr OverlayManager::CreateBorderPanel( String const & p_name, Point2d const & p_position, Point2d const & p_size, MaterialSPtr p_material, Point4d const & p_bordersSize, MaterialSPtr p_bordersMaterial, OverlaySPtr p_parent )
	{
		OverlaySPtr l_overlay = CreateOverlay( eOVERLAY_TYPE_BORDER_PANEL, p_name, p_parent, nullptr );
		l_overlay->SetMaterial( p_material );
		l_overlay->SetPosition( p_position );
		l_overlay->SetSize( p_size );
		BorderPanelOverlaySPtr l_return = l_overlay->GetBorderPanelOverlay();
		l_return->SetBorderMaterial( p_bordersMaterial );
		l_return->SetBorderSize( p_bordersSize );
		return l_return;
	}

	BorderPanelOverlaySPtr OverlayManager::CreateBorderPanel( String const & p_name, Position const & p_position, Size const & p_size, MaterialSPtr p_material, Rectangle const & p_bordersSize, MaterialSPtr p_bordersMaterial, OverlaySPtr p_parent )
	{
		OverlaySPtr l_overlay = CreateOverlay( eOVERLAY_TYPE_BORDER_PANEL, p_name, p_parent, nullptr );
		l_overlay->SetPixelPosition( p_position );
		l_overlay->SetPixelSize( p_size );
		l_overlay->SetMaterial( p_material );
		BorderPanelOverlaySPtr l_return = l_overlay->GetBorderPanelOverlay();
		l_return->SetBorderMaterial( p_bordersMaterial );
		l_return->SetBorderPixelSize( p_bordersSize );
		return l_return;
	}

	TextOverlaySPtr OverlayManager::CreateText( String const & p_name, Point2d const & p_position, Point2d const & p_size, MaterialSPtr p_material, FontSPtr p_font, OverlaySPtr p_parent )
	{
		OverlaySPtr l_overlay = CreateOverlay( eOVERLAY_TYPE_TEXT, p_name, p_parent, nullptr );
		l_overlay->SetMaterial( p_material );
		TextOverlaySPtr l_return = l_overlay->GetTextOverlay();
		l_return->SetPosition( p_position );
		l_return->SetSize( p_size );
		l_return->SetFont( p_font->GetName() );
		return l_return;
	}

	TextOverlaySPtr OverlayManager::CreateText( String const & p_name, Position const & p_position, Size const & p_size, MaterialSPtr p_material, FontSPtr p_font, OverlaySPtr p_parent )
	{
		OverlaySPtr l_overlay = CreateOverlay( eOVERLAY_TYPE_TEXT, p_name, p_parent, nullptr );
		l_overlay->SetMaterial( p_material );
		TextOverlaySPtr l_return = l_overlay->GetTextOverlay();
		l_return->SetPixelPosition( p_position );
		l_return->SetPixelSize( p_size );
		l_return->SetFont( p_font->GetName() );
		return l_return;
	}

	OverlaySPtr OverlayManager::CreateOverlay( eOVERLAY_TYPE p_type, String const & p_name, OverlaySPtr p_parent, SceneSPtr p_scene )
	{
		OverlaySPtr l_pReturn = GetOverlay( p_name );

		if ( !l_pReturn )
		{
			l_pReturn = std::make_shared< Overlay >( m_engine, p_type, p_scene, p_parent );
			l_pReturn->SetName( p_name );

			if ( p_scene )
			{
				p_scene->AddOverlay( l_pReturn );
			}

			AddOverlay( p_name, l_pReturn, p_parent );
			Logger::LogInfo( cuT( "Scene::CreateOverlay - Overlay [" ) + p_name + cuT( "] - Created" ) );
		}
		else
		{
			Logger::LogWarning( cuT( "Scene::CreateOverlay - Can't create Overlay [" ) + p_name + cuT( "] - Another overlay with the same name already exists" ) );
		}

		return  l_pReturn;
	}

	void OverlayManager::AddOverlay( Castor::String const & p_name, OverlaySPtr p_overlay, OverlaySPtr p_parent )
	{
		Castor::Collection< Overlay, Castor::String >::insert( p_name, p_overlay );
		int l_level = 0;

		if ( !p_parent )
		{
			m_overlays.push_back( p_overlay );
		}
		else
		{
			l_level = p_parent->GetLevel() + 1;
			p_parent->AddChild( p_overlay );
		}

		if ( l_level > int( m_overlayCountPerLevel.size() ) )
		{
			m_overlayCountPerLevel.resize( m_overlayCountPerLevel.size() * 2 );
		}

		p_overlay->SetOrder( m_overlayCountPerLevel[l_level], l_level );
		m_overlayCountPerLevel[l_level]++;
	}

	void OverlayManager::RemoveOverlay( Castor::String const & p_name )
	{
		OverlaySPtr l_overlay = Castor::Collection< Overlay, Castor::String >::find( p_name );

		if ( l_overlay )
		{
			Castor::Collection< Overlay, Castor::String >::erase( p_name );

			if ( l_overlay->GetChildsCount() )
			{
				Size l_size = m_pRenderer->GetSize();

				for ( auto && l_child : *l_overlay )
				{
					l_child->SetPosition( l_child->GetAbsolutePosition() );
					l_child->SetSize( l_child->GetAbsoluteSize() );
				}
			}
		}
	}

	bool OverlayManager::HasOverlay( Castor::String const & p_name )
	{
		return Castor::Collection< Overlay, Castor::String >::has( p_name );
	}

	OverlaySPtr OverlayManager::GetOverlay( Castor::String const & p_name )
	{
		return Castor::Collection< Overlay, Castor::String >::find( p_name );
	}

	bool OverlayManager::WriteOverlays( Castor::TextFile & p_file )const
	{
		Castor::Collection< Overlay, Castor::String >::lock();
		bool l_return = true;
		auto && l_it = m_overlays.begin();
		bool l_first = true;

		while ( l_return && l_it != m_overlays.end() )
		{
			OverlaySPtr l_overlay = *l_it;

			if ( l_first )
			{
				l_first = false;
			}
			else
			{
				p_file.WriteText( cuT( "\n" ) );
			}

			switch ( l_overlay->GetType() )
			{
			case eOVERLAY_TYPE_PANEL:
				l_return = PanelOverlay::TextLoader()( *l_overlay->GetPanelOverlay(), p_file );
				break;

			case eOVERLAY_TYPE_BORDER_PANEL:
				l_return = BorderPanelOverlay::TextLoader()( *l_overlay->GetBorderPanelOverlay(), p_file );
				break;

			case eOVERLAY_TYPE_TEXT:
				l_return = TextOverlay::TextLoader()( *l_overlay->GetTextOverlay(), p_file );
				break;

			default:
				l_return = false;
			}

			++l_it;
		}

		Castor::Collection< Overlay, Castor::String >::unlock();
		return l_return;
	}

	bool OverlayManager::ReadOverlays( Castor::TextFile & p_file )
	{
		SceneFileParser l_parser( m_engine );
		return l_parser.ParseFile( p_file );
	}

	bool OverlayManager::SaveOverlays( Castor::BinaryFile & p_file )const
	{
		Castor::Collection< Overlay, Castor::String >::lock();
		bool l_return = p_file.Write( uint32_t( m_overlays.size() ) ) == sizeof( uint32_t );
		auto && l_it = m_overlays.begin();

		while ( l_return && l_it != m_overlays.end() )
		{
			OverlaySPtr l_overlay = *l_it;
			l_return = BinaryLoader< Overlay >()( *l_overlay, p_file );
			++l_it;
		}

		Castor::Collection< Overlay, Castor::String >::unlock();
		return l_return;
	}

	bool OverlayManager::LoadOverlays( Castor::BinaryFile & p_file )
	{
		Castor::Collection< Overlay, Castor::String >::lock();
		uint32_t l_size;
		bool l_return = p_file.Write( l_size ) == sizeof( uint32_t );
		String l_name;
		eOVERLAY_TYPE l_type;
		OverlaySPtr l_overlay;

		for ( uint32_t i = 0; i < l_size && l_return; i++ )
		{
			l_return = p_file.Read( l_name );

			if ( l_return )
			{
				l_return = p_file.Read( l_type ) == sizeof( eOVERLAY_TYPE );
			}

			if ( l_return )
			{
				l_overlay = Castor::Collection< Overlay, Castor::String >::find( l_name ) ;

				if ( !l_overlay )
				{
					l_overlay = std::make_shared< Overlay >( m_engine, l_type );
					l_overlay->SetName( l_name );
					AddOverlay( l_name, l_overlay, nullptr );
				}

				l_return = l_overlay != nullptr;
			}

			if ( l_return )
			{
				l_return = BinaryLoader< Overlay >()( *l_overlay, p_file );
			}
		}

		Castor::Collection< Overlay, Castor::String >::unlock();
		return l_return;
	}

	void OverlayManager::Update()
	{
		if ( m_engine->IsCleaned() )
		{
			if ( m_pRenderer )
			{
				m_pRenderer->Cleanup();
				m_pRenderer.reset();
			}
		}
		else
		{
			if ( !m_pRenderer )
			{
				m_pRenderer = m_engine->GetRenderSystem()->CreateOverlayRenderer();
				m_pRenderer->Initialise();
			}
		}
	}

	void OverlayManager::RenderOverlays( Scene const & p_scene, Castor::Size const & p_size )
	{
		RenderSystem * l_renderSystem = m_engine->GetRenderSystem();
		Pipeline & l_pipeline = l_renderSystem->GetPipeline();

		if ( m_size != p_size )
		{
			m_size = p_size;
			l_pipeline.Ortho( real( 0.0 ), real( m_size.width() ), real( m_size.height() ), real( 0.0 ), real( 0.0 ), real( 1000.0 ) );
			m_projection = l_pipeline.GetProjectionMatrix();
		}
		else
		{
			l_pipeline.SetProjectionMatrix( m_projection );
		}

		lock();
		Update();
		Context * l_context = l_renderSystem->GetCurrentContext();

		if ( l_context && m_pRenderer )
		{
			l_context->CullFace( eFACE_BACK );
			l_pipeline.ApplyViewport( m_size.width(), m_size.height() );
			m_pRenderer->BeginRender( m_size );

			for ( auto l_overlay : m_overlays )
			{
				SceneSPtr l_scene = l_overlay->GetScene();

				if ( !l_scene || l_scene->GetName() == p_scene.GetName() )
				{
					l_overlay->Render( m_size );
				}
			}
		}

		unlock();
	}
}
