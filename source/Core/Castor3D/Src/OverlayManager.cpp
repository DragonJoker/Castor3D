#include "OverlayManager.hpp"

#include "BorderPanelOverlay.hpp"
#include "Engine.hpp"
#include "CleanupEvent.hpp"
#include "FunctorEvent.hpp"
#include "InitialiseEvent.hpp"
#include "Overlay.hpp"
#include "OverlayRenderer.hpp"
#include "PanelOverlay.hpp"
#include "Pipeline.hpp"
#include "RenderSystem.hpp"
#include "Scene.hpp"
#include "SceneFileParser.hpp"
#include "TextOverlay.hpp"

#include <Font.hpp>

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

	OverlayManager::OverlayManager( Engine & p_engine )
		: Manager< String, Overlay, Engine >( p_engine )
		, m_overlayCountPerLevel( 1000, 0 )
		, m_viewport( Viewport::Ortho( p_engine, 0, 1, 1, 0, 0, 1000 ) )
	{
	}

	OverlayManager::~OverlayManager()
	{
	}

	void OverlayManager::Clear()
	{
		std::unique_lock< Collection > l_lock( m_elements );
		Manager< String, Overlay, Engine >::Clear();
		m_overlays.clear();
		m_fontTextures.clear();
	}

	void OverlayManager::Cleanup()
	{
		std::unique_lock< Collection > l_lock( m_elements );

		for ( auto && l_overlay : *this )
		{
			GetOwner()->PostEvent( MakeCleanupEvent( *l_overlay ) );
		}

		for ( auto && l_it : m_fontTextures )
		{
			GetOwner()->PostEvent( MakeCleanupEvent( *l_it.second ) );
		}
	}

	PanelOverlaySPtr OverlayManager::CreatePanel( String const & p_name, Point2d const & p_position, Point2d const & p_size, MaterialSPtr p_material, OverlaySPtr p_parent )
	{
		OverlaySPtr l_overlay = Create( eOVERLAY_TYPE_PANEL, p_name, p_parent, nullptr );
		l_overlay->SetPosition( p_position );
		l_overlay->SetSize( p_size );
		l_overlay->SetMaterial( p_material );
		return l_overlay->GetPanelOverlay();
	}

	PanelOverlaySPtr OverlayManager::CreatePanel( String const & p_name, Position const & p_position, Size const & p_size, MaterialSPtr p_material, OverlaySPtr p_parent )
	{
		OverlaySPtr l_overlay = Create( eOVERLAY_TYPE_PANEL, p_name, p_parent, nullptr );
		l_overlay->SetPixelPosition( p_position );
		l_overlay->SetPixelSize( p_size );
		l_overlay->SetMaterial( p_material );
		return l_overlay->GetPanelOverlay();
	}

	BorderPanelOverlaySPtr OverlayManager::CreateBorderPanel( String const & p_name, Point2d const & p_position, Point2d const & p_size, MaterialSPtr p_material, Point4d const & p_bordersSize, MaterialSPtr p_bordersMaterial, OverlaySPtr p_parent )
	{
		OverlaySPtr l_overlay = Create( eOVERLAY_TYPE_BORDER_PANEL, p_name, p_parent, nullptr );
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
		OverlaySPtr l_overlay = Create( eOVERLAY_TYPE_BORDER_PANEL, p_name, p_parent, nullptr );
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
		OverlaySPtr l_overlay = Create( eOVERLAY_TYPE_TEXT, p_name, p_parent, nullptr );
		l_overlay->SetMaterial( p_material );
		TextOverlaySPtr l_return = l_overlay->GetTextOverlay();
		l_return->SetPosition( p_position );
		l_return->SetSize( p_size );
		l_return->SetFont( p_font->GetName() );
		return l_return;
	}

	TextOverlaySPtr OverlayManager::CreateText( String const & p_name, Position const & p_position, Size const & p_size, MaterialSPtr p_material, FontSPtr p_font, OverlaySPtr p_parent )
	{
		OverlaySPtr l_overlay = Create( eOVERLAY_TYPE_TEXT, p_name, p_parent, nullptr );
		l_overlay->SetMaterial( p_material );
		TextOverlaySPtr l_return = l_overlay->GetTextOverlay();
		l_return->SetPixelPosition( p_position );
		l_return->SetPixelSize( p_size );
		l_return->SetFont( p_font->GetName() );
		return l_return;
	}

	OverlaySPtr OverlayManager::Create( eOVERLAY_TYPE p_type, String const & p_name, OverlaySPtr p_parent, SceneSPtr p_scene )
	{
		std::unique_lock< Collection > l_lock( m_elements );
		OverlaySPtr l_return;

		if ( !m_elements.has( p_name ) )
		{
			l_return = std::make_shared< Overlay >( *GetOwner(), p_type, p_scene, p_parent );
			l_return->SetName( p_name );

			if ( p_scene )
			{
				p_scene->AddOverlay( l_return );
			}

			DoAddOverlay( p_name, l_return, p_parent );
			Logger::LogInfo( cuT( "OverlayManager::Create - Created Overlay: " ) + p_name + cuT( "" ) );
		}
		else
		{
			l_return = m_elements.find( p_name );
			Logger::LogWarning( cuT( "OverlayManager::Create - Duplicate Overlay: " ) + p_name );
		}

		return  l_return;
	}

	void OverlayManager::Remove( Castor::String const & p_name )
	{
		std::unique_lock< Collection > l_lock( m_elements );
		OverlaySPtr l_overlay = m_elements.find( p_name );

		if ( l_overlay )
		{
			m_elements.erase( p_name );

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

	void OverlayManager::Update()
	{
		if ( GetOwner()->IsCleaned() )
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
				m_pRenderer = std::make_shared< OverlayRenderer >( *GetOwner()->GetRenderSystem() );
				m_pRenderer->Initialise();
			}
		}
	}

	void OverlayManager::Render( Scene const & p_scene, Castor::Size const & p_size )
	{
		std::unique_lock< Collection > l_lock( m_elements );
		Update();

		for ( auto l_it : m_elements )
		{
			if ( l_it.second->GetCategory()->GetType() == eOVERLAY_TYPE_TEXT )
			{
				l_it.second->GetTextOverlay()->LoadNewGlyphs();
			}
		}

		RenderSystem * l_renderSystem = GetOwner()->GetRenderSystem();
		Context * l_context = l_renderSystem->GetCurrentContext();

		if ( l_context && m_pRenderer )
		{
			l_context->CullFace( eFACE_BACK );
			m_viewport.SetSize( p_size );
			m_viewport.SetRight( real( p_size.width() ) );
			m_viewport.SetBottom( real( p_size.height() ) );
			m_viewport.Render( l_renderSystem->GetPipeline() );
			m_pRenderer->BeginRender( p_size );

			for ( auto l_category : m_overlays )
			{
				SceneSPtr l_scene = l_category->GetOverlay().GetScene();

				if ( l_category->GetOverlay().IsVisible() && ( !l_scene || l_scene->GetName() == p_scene.GetName() ) )
				{
					l_category->Render();
				}
			}

			m_pRenderer->EndRender();
		}
	}

	bool OverlayManager::Write( Castor::TextFile & p_file )const
	{
		std::unique_lock< Collection > l_lock( m_elements );
		bool l_return = true;
		auto && l_it = m_overlays.begin();
		bool l_first = true;

		while ( l_return && l_it != m_overlays.end() )
		{
			Overlay & l_overlay = ( *l_it )->GetOverlay();

			if ( l_first )
			{
				l_first = false;
			}
			else
			{
				p_file.WriteText( cuT( "\n" ) );
			}

			switch ( l_overlay.GetType() )
			{
			case eOVERLAY_TYPE_PANEL:
				l_return = PanelOverlay::TextLoader()( *l_overlay.GetPanelOverlay(), p_file );
				break;

			case eOVERLAY_TYPE_BORDER_PANEL:
				l_return = BorderPanelOverlay::TextLoader()( *l_overlay.GetBorderPanelOverlay(), p_file );
				break;

			case eOVERLAY_TYPE_TEXT:
				l_return = TextOverlay::TextLoader()( *l_overlay.GetTextOverlay(), p_file );
				break;

			default:
				l_return = false;
			}

			++l_it;
		}

		return l_return;
	}

	bool OverlayManager::Read( Castor::TextFile & p_file )
	{
		std::unique_lock< Collection > l_lock( m_elements );
		SceneFileParser l_parser( *GetOwner() );
		return l_parser.ParseFile( p_file );
	}

	bool OverlayManager::Save( Castor::BinaryFile & p_file )const
	{
		std::unique_lock< Collection > l_lock( m_elements );
		bool l_return = p_file.Write( uint32_t( m_overlays.size() ) ) == sizeof( uint32_t );
		auto && l_it = m_overlays.begin();

		while ( l_return && l_it != m_overlays.end() )
		{
			l_return = BinaryLoader< Overlay >()( ( *l_it )->GetOverlay(), p_file );
			++l_it;
		}

		return l_return;
	}

	bool OverlayManager::Load( Castor::BinaryFile & p_file )
	{
		std::unique_lock< Collection > l_lock( m_elements );
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
				l_overlay = Find( l_name ) ;

				if ( !l_overlay )
				{
					l_overlay = std::make_shared< Overlay >( *GetOwner(), l_type );
					l_overlay->SetName( l_name );
					DoAddOverlay( l_name, l_overlay, nullptr );
				}

				l_return = l_overlay != nullptr;
			}

			if ( l_return )
			{
				l_return = BinaryLoader< Overlay >()( *l_overlay, p_file );
			}
		}

		return l_return;
	}

	FontTextureSPtr OverlayManager::GetFontTexture( Castor::String const & p_name )
	{
		auto l_it = m_fontTextures.find( p_name );
		FontTextureSPtr l_return;

		if ( l_it != m_fontTextures.end() )
		{
			l_return = l_it->second;
		}

		return l_return;
	}

	FontTextureSPtr OverlayManager::CreateFontTexture( Castor::FontSPtr p_font )
	{
		auto l_it = m_fontTextures.find( p_font->GetName() );
		FontTextureSPtr l_return;

		if ( l_it == m_fontTextures.end() )
		{
			l_return = std::make_shared< FontTexture >( *GetOwner(), p_font );
			m_fontTextures.insert( std::make_pair( p_font->GetName(), l_return ) );
			GetOwner()->PostEvent( MakeInitialiseEvent( *l_return ) );
		}

		return l_return;
	}

	void OverlayManager::DoAddOverlay( Castor::String const & p_name, OverlaySPtr p_overlay, OverlaySPtr p_parent )
	{
		std::unique_lock< Collection > l_lock( m_elements );
		m_elements.insert( p_name, p_overlay );
		int l_level = 0;

		if ( p_parent )
		{
			l_level = p_parent->GetLevel() + 1;
			p_parent->AddChild( p_overlay );
		}

		if ( l_level > int( m_overlayCountPerLevel.size() ) )
		{
			m_overlayCountPerLevel.resize( m_overlayCountPerLevel.size() * 2 );
		}

		p_overlay->SetOrder( ++m_overlayCountPerLevel[l_level], l_level );
		m_overlays.insert( p_overlay->GetCategory() );
	}
}
