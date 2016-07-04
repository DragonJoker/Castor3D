#include "OverlayManager.hpp"

#include "Engine.hpp"
#include "Overlay/BorderPanelOverlay.hpp"
#include "Overlay/Overlay.hpp"
#include "Overlay/OverlayRenderer.hpp"
#include "Overlay/PanelOverlay.hpp"
#include "Overlay/TextOverlay.hpp"
#include "Render/Pipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Scene.hpp"
#include "Scene/SceneFileParser.hpp"

#include <Font.hpp>

using namespace Castor;

#if defined( DrawText )
#	undef DrawText
#endif

namespace Castor3D
{
	const String CachedObjectNamer< Overlay >::Name = cuT( "Overlay" );

	//*************************************************************************************************

	OverlayCache::OverlayCache( Engine & p_engine )
		: OwnedBy< Engine >{ p_engine }
		, m_overlayCountPerLevel{ 1000, 0 }
		, m_viewport{ p_engine }
	{
		m_viewport.SetOrtho( 0, 1, 1, 0, 0, 1000 );
		p_engine.PostEvent( MakeInitialiseEvent( m_viewport ) );
	}

	OverlayCache::~OverlayCache()
	{
	}

	void OverlayCache::Clear()
	{
		auto l_lock = make_unique_lock( *this );
		m_overlays.clear();
		m_fontTextures.clear();
	}

	void OverlayCache::Cleanup()
	{
		m_viewport.Cleanup();
		auto l_lock = make_unique_lock( *this );

		for ( auto l_it : m_fontTextures )
		{
			GetEngine()->PostEvent( MakeCleanupEvent( *l_it.second ) );
		}
	}

	OverlaySPtr OverlayCache::Add( String const & p_name, eOVERLAY_TYPE p_type, OverlaySPtr p_parent, SceneSPtr p_scene )
	{
		auto l_lock = make_unique_lock( *this );
		OverlaySPtr l_return;

		if ( !m_elements.has( p_name ) )
		{
			l_return = std::make_shared< Overlay >( *GetEngine(), p_type, p_scene, p_parent );
			l_return->SetName( p_name );
			DoAddOverlay( p_name, l_return, p_parent );
			Castor::Logger::LogInfo( Castor::StringStream() << INFO_CACHE_CREATED_OBJECT << CachedObjectNamer< Overlay >::Name << cuT( ": " ) << p_name );
		}
		else
		{
			l_return = m_elements.find( p_name );
			Castor::Logger::LogWarning( Castor::StringStream() << WARNING_CACHE_DUPLICATE_OBJECT << CachedObjectNamer< Overlay >::Name << cuT( ": " ) << p_name );
		}

		return  l_return;
	}

	void OverlayCache::Remove( Castor::String const & p_name )
	{
		auto l_lock = make_unique_lock( *this );
		OverlaySPtr l_overlay = m_elements.find( p_name );

		if ( l_overlay )
		{
			m_elements.erase( p_name );

			if ( l_overlay->GetChildrenCount() )
			{
				Size l_size = m_pRenderer->GetSize();

				for ( auto l_child : *l_overlay )
				{
					l_child->SetPosition( l_child->GetAbsolutePosition() );
					l_child->SetSize( l_child->GetAbsoluteSize() );
				}
			}
		}
	}

	void OverlayCache::UpdateRenderer()
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

	void OverlayCache::Update()
	{
		auto l_lock = make_unique_lock( *this );

		for ( auto l_category : m_overlays )
		{
			l_category->Update();
		}
	}

	void OverlayCache::Render( Scene const & p_scene, Castor::Size const & p_size )
	{
		auto l_lock = make_unique_lock( *this );
		RenderSystem * l_renderSystem = GetEngine()->GetRenderSystem();
		Context * l_context = l_renderSystem->GetCurrentContext();

		if ( l_context && m_pRenderer )
		{
			m_viewport.Resize( p_size );
			m_viewport.UpdateRight( real( p_size.width() ) );
			m_viewport.UpdateBottom( real( p_size.height() ) );
			m_viewport.Render( l_context->GetPipeline() );
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

	bool OverlayCache::Write( Castor::TextFile & p_file )const
	{
		auto l_lock = make_unique_lock( *this );
		bool l_return = true;
		auto l_it = m_overlays.begin();
		bool l_first = true;

		while ( l_return && l_it != m_overlays.end() )
		{
			Overlay const & l_overlay = ( *l_it )->GetOverlay();

			if ( l_first )
			{
				l_first = false;
			}
			else
			{
				p_file.WriteText( cuT( "\n" ) );
			}

			l_return = Overlay::TextWriter( String{} )( l_overlay, p_file );
			++l_it;
		}

		return l_return;
	}

	bool OverlayCache::Read( Castor::TextFile & p_file )
	{
		auto l_lock = make_unique_lock( *this );
		SceneFileParser l_parser( *GetEngine() );
		return l_parser.ParseFile( p_file );
	}

	FontTextureSPtr OverlayCache::GetFontTexture( Castor::String const & p_name )
	{
		auto l_it = m_fontTextures.find( p_name );
		FontTextureSPtr l_return;

		if ( l_it != m_fontTextures.end() )
		{
			l_return = l_it->second;
		}

		return l_return;
	}

	FontTextureSPtr OverlayCache::CreateFontTexture( Castor::FontSPtr p_font )
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

	void OverlayCache::DoAddOverlay( Castor::String const & p_name, OverlaySPtr p_overlay, OverlaySPtr p_parent )
	{
		auto l_lock = make_unique_lock( *this );
		m_elements.insert( p_name, p_overlay );
		int l_level = 0;

		if ( p_parent )
		{
			l_level = p_parent->GetLevel() + 1;
			p_parent->AddChild( p_overlay );
		}

		while ( l_level >= int( m_overlayCountPerLevel.size() ) )
		{
			m_overlayCountPerLevel.resize( m_overlayCountPerLevel.size() * 2 );
		}

		p_overlay->SetOrder( ++m_overlayCountPerLevel[l_level], l_level );
		m_overlays.insert( p_overlay->GetCategory() );
	}
}
