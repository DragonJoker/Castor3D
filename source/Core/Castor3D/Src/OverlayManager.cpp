#include "OverlayManager.hpp"

#include "BorderPanelOverlay.hpp"
#include "Engine.hpp"
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
	const String ManagedObjectNamer< Overlay >::Name = cuT( "Overlay" );

	//*************************************************************************************************

	OverlayManager::OverlayManager( Engine & p_engine )
		: ResourceManager< String, Overlay >( p_engine )
		, m_overlayCountPerLevel( 1000, 0 )
		, m_viewport( Viewport::Ortho( p_engine, 0, 1, 1, 0, 0, 1000 ) )
	{
	}

	OverlayManager::~OverlayManager()
	{
	}

	void OverlayManager::Clear()
	{
		auto l_lock = make_unique_lock( *this );
		ResourceManager< String, Overlay >::Clear();
		m_overlays.clear();
		m_fontTextures.clear();
	}

	void OverlayManager::Cleanup()
	{
		auto l_lock = make_unique_lock( *this );

		for ( auto && l_it : m_fontTextures )
		{
			GetEngine()->PostEvent( MakeCleanupEvent( *l_it.second ) );
		}
	}

	PanelOverlaySPtr OverlayManager::CreatePanel( String const & p_name, Point2d const & p_position, Point2d const & p_size, MaterialSPtr p_material, OverlaySPtr p_parent )
	{
		OverlaySPtr l_overlay = Create( p_name, eOVERLAY_TYPE_PANEL, p_parent, nullptr );
		l_overlay->SetPosition( p_position );
		l_overlay->SetSize( p_size );
		l_overlay->SetMaterial( p_material );
		return l_overlay->GetPanelOverlay();
	}

	PanelOverlaySPtr OverlayManager::CreatePanel( String const & p_name, Position const & p_position, Size const & p_size, MaterialSPtr p_material, OverlaySPtr p_parent )
	{
		OverlaySPtr l_overlay = Create( p_name, eOVERLAY_TYPE_PANEL, p_parent, nullptr );
		l_overlay->SetPixelPosition( p_position );
		l_overlay->SetPixelSize( p_size );
		l_overlay->SetMaterial( p_material );
		return l_overlay->GetPanelOverlay();
	}

	BorderPanelOverlaySPtr OverlayManager::CreateBorderPanel( String const & p_name, Point2d const & p_position, Point2d const & p_size, MaterialSPtr p_material, Point4d const & p_bordersSize, MaterialSPtr p_bordersMaterial, OverlaySPtr p_parent )
	{
		OverlaySPtr l_overlay = Create( p_name, eOVERLAY_TYPE_BORDER_PANEL, p_parent, nullptr );
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
		OverlaySPtr l_overlay = Create( p_name, eOVERLAY_TYPE_BORDER_PANEL, p_parent, nullptr );
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
		OverlaySPtr l_overlay = Create( p_name, eOVERLAY_TYPE_TEXT, p_parent, nullptr );
		l_overlay->SetMaterial( p_material );
		TextOverlaySPtr l_return = l_overlay->GetTextOverlay();
		l_return->SetPosition( p_position );
		l_return->SetSize( p_size );
		l_return->SetFont( p_font->GetName() );
		return l_return;
	}

	TextOverlaySPtr OverlayManager::CreateText( String const & p_name, Position const & p_position, Size const & p_size, MaterialSPtr p_material, FontSPtr p_font, OverlaySPtr p_parent )
	{
		OverlaySPtr l_overlay = Create( p_name, eOVERLAY_TYPE_TEXT, p_parent, nullptr );
		l_overlay->SetMaterial( p_material );
		TextOverlaySPtr l_return = l_overlay->GetTextOverlay();
		l_return->SetPixelPosition( p_position );
		l_return->SetPixelSize( p_size );
		l_return->SetFont( p_font->GetName() );
		return l_return;
	}

	OverlaySPtr OverlayManager::Create( String const & p_name, eOVERLAY_TYPE p_type, OverlaySPtr p_parent, SceneSPtr p_scene )
	{
		auto l_lock = make_unique_lock( *this );
		OverlaySPtr l_return;

		if ( !m_elements.has( p_name ) )
		{
			l_return = std::make_shared< Overlay >( *GetEngine(), p_type, p_scene, p_parent );
			l_return->SetName( p_name );

			if ( p_scene )
			{
				p_scene->AddOverlay( l_return );
			}

			DoAddOverlay( p_name, l_return, p_parent );
			Castor::Logger::LogInfo( Castor::StringStream() << INFO_MANAGER_CREATED_OBJECT << this->GetObjectTypeName() << cuT( ": " ) << p_name );
		}
		else
		{
			l_return = m_elements.find( p_name );
			Castor::Logger::LogWarning( Castor::StringStream() << WARNING_MANAGER_DUPLICATE_OBJECT << this->GetObjectTypeName() << cuT( ": " ) << p_name );
		}

		return  l_return;
	}

	void OverlayManager::Remove( Castor::String const & p_name )
	{
		auto l_lock = make_unique_lock( *this );
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

	void OverlayManager::UpdateRenderer()
	{
		if ( GetEngine()->IsCleaned() )
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
				m_pRenderer = std::make_shared< OverlayRenderer >( *GetEngine()->GetRenderSystem() );
				m_pRenderer->Initialise();
			}
		}
	}

	void OverlayManager::Update()
	{
		auto l_lock = make_unique_lock( *this );

		for ( auto l_category : m_overlays )
		{
			l_category->Update();
		}
	}

	void OverlayManager::Render( Scene const & p_scene, Castor::Size const & p_size )
	{
		auto l_lock = make_unique_lock( *this );
		RenderSystem * l_renderSystem = GetEngine()->GetRenderSystem();
		Context * l_context = l_renderSystem->GetCurrentContext();

		if ( l_context && m_pRenderer )
		{
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
		auto l_lock = make_unique_lock( *this );
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
		auto l_lock = make_unique_lock( *this );
		SceneFileParser l_parser( *GetEngine() );
		return l_parser.ParseFile( p_file );
	}

	bool OverlayManager::Save( Castor::BinaryFile & p_file )const
	{
		auto l_lock = make_unique_lock( *this );
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
		auto l_lock = make_unique_lock( *this );
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
					l_overlay = std::make_shared< Overlay >( *GetEngine(), l_type );
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
			l_return = std::make_shared< FontTexture >( *GetEngine(), p_font );
			m_fontTextures.insert( std::make_pair( p_font->GetName(), l_return ) );
			GetEngine()->PostEvent( MakeInitialiseEvent( *l_return ) );
		}

		return l_return;
	}

	void OverlayManager::DoAddOverlay( Castor::String const & p_name, OverlaySPtr p_overlay, OverlaySPtr p_parent )
	{
		auto l_lock = make_unique_lock( *this );
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
