#include "OverlayCache.hpp"

#include "Engine.hpp"
#include "Event/Frame/CleanupEvent.hpp"
#include "Event/Frame/InitialiseEvent.hpp"
#include "Overlay/OverlayRenderer.hpp"
#include "Scene/Scene.hpp"
#include "Scene/SceneFileParser.hpp"

#include <Graphics/Font.hpp>

using namespace Castor;

#if defined( DrawText )
#	undef DrawText
#endif

namespace Castor3D
{
	//*************************************************************************************************

	template<> const String CacheTraits< Overlay, String >::Name = cuT( "Overlay" );

	//*************************************************************************************************

	OverlayCache::OverlayInitialiser::OverlayInitialiser( Cache< Overlay, Castor::String > & cache )
		: m_overlays{ cache.m_overlays }
		, m_overlayCountPerLevel{ cache.m_overlayCountPerLevel }
	{
	}

	void OverlayCache::OverlayInitialiser::operator()( OverlaySPtr p_element )
	{
		int level = 0;

		if ( p_element->GetParent() )
		{
			level = p_element->GetParent()->GetLevel() + 1;
			p_element->GetParent()->AddChild( p_element );
		}

		while ( level >= int( m_overlayCountPerLevel.size() ) )
		{
			m_overlayCountPerLevel.resize( m_overlayCountPerLevel.size() * 2 );
		}

		p_element->SetOrder( ++m_overlayCountPerLevel[level], level );
		m_overlays.insert( p_element->GetCategory() );
	}

	//*************************************************************************************************

	OverlayCache::OverlayCleaner::OverlayCleaner( Cache< Overlay, Castor::String > & cache )
		: m_overlays{ cache.m_overlays }
		, m_overlayCountPerLevel{ cache.m_overlayCountPerLevel }
	{
	}

	void OverlayCache::OverlayCleaner::operator()( OverlaySPtr p_element )
	{
		if ( p_element->GetChildrenCount() )
		{
			for ( auto child : *p_element )
			{
				child->SetPosition( child->GetAbsolutePosition() );
				child->SetSize( child->GetAbsoluteSize() );
			}
		}
	}

	//*************************************************************************************************

	Cache< Overlay, Castor::String >::Cache( Engine & p_engine
		 , Producer && p_produce
		 , Initialiser && p_initialise
		 , Cleaner && p_clean
		 , Merger && p_merge )
		: MyCacheType( p_engine
		   , std::move( p_produce )
		   , std::bind( OverlayInitialiser{ *this }, std::placeholders::_1 )
		   , std::bind( OverlayCleaner{ *this }, std::placeholders::_1 )
		   , std::move( p_merge ) )
		, m_overlayCountPerLevel{ 1000, 0 }
		, m_viewport{ *GetEngine() }
	{
		m_viewport.SetOrtho( 0, 1, 1, 0, 0, 1000 );
		GetEngine()->PostEvent( MakeInitialiseEvent( m_viewport ) );
	}

	Cache< Overlay, Castor::String >::~Cache()
	{
	}

	void Cache< Overlay, Castor::String >::Clear()
	{
		auto lock = make_unique_lock( *this );
		m_overlays.clear();
		m_fontTextures.clear();
	}

	void Cache< Overlay, Castor::String >::Cleanup()
	{
		m_viewport.Cleanup();
		auto lock = make_unique_lock( *this );

		for ( auto it : m_fontTextures )
		{
			GetEngine()->PostEvent( MakeCleanupEvent( *it.second ) );
		}
	}

	void Cache< Overlay, Castor::String >::UpdateRenderer()
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

	void Cache< Overlay, Castor::String >::Update()
	{
		auto lock = make_unique_lock( *this );

		for ( auto category : m_overlays )
		{
			category->Update();
		}
	}

	void Cache< Overlay, Castor::String >::Render( Scene const & p_scene, Castor::Size const & p_size )
	{
		auto lock = make_unique_lock( *this );
		RenderSystem * renderSystem = GetEngine()->GetRenderSystem();

		if ( m_pRenderer )
		{
			m_viewport.Resize( p_size );
			m_viewport.UpdateRight( real( p_size.width() ) );
			m_viewport.UpdateBottom( real( p_size.height() ) );
			m_viewport.Update();
			m_pRenderer->BeginRender( m_viewport );

			for ( auto category : m_overlays )
			{
				SceneSPtr scene = category->GetOverlay().GetScene();

				if ( category->GetOverlay().IsVisible() && ( !scene || scene->GetName() == p_scene.GetName() ) )
				{
					category->Render();
				}
			}

			m_pRenderer->EndRender();
		}
	}

	bool Cache< Overlay, Castor::String >::Write( Castor::TextFile & p_file )const
	{
		auto lock = make_unique_lock( *this );
		bool result = true;
		auto it = m_overlays.begin();
		bool first = true;

		while ( result && it != m_overlays.end() )
		{
			Overlay const & overlay = ( *it )->GetOverlay();

			if ( first )
			{
				first = false;
			}
			else
			{
				p_file.WriteText( cuT( "\n" ) );
			}

			result = Overlay::TextWriter( String{} )( overlay, p_file );
			++it;
		}

		return result;
	}

	bool Cache< Overlay, Castor::String >::Read( Castor::TextFile & p_file )
	{
		auto lock = make_unique_lock( *this );
		SceneFileParser parser( *GetEngine() );
		return parser.ParseFile( p_file );
	}

	FontTextureSPtr Cache< Overlay, Castor::String >::GetFontTexture( Castor::String const & p_name )
	{
		auto it = m_fontTextures.find( p_name );
		FontTextureSPtr result;

		if ( it != m_fontTextures.end() )
		{
			result = it->second;
		}

		return result;
	}

	FontTextureSPtr Cache< Overlay, Castor::String >::CreateFontTexture( Castor::FontSPtr p_font )
	{
		auto it = m_fontTextures.find( p_font->GetName() );
		FontTextureSPtr result;

		if ( it == m_fontTextures.end() )
		{
			result = std::make_shared< FontTexture >( *GetEngine(), p_font );
			m_fontTextures.emplace( p_font->GetName(), result );
			GetEngine()->PostEvent( MakeInitialiseEvent( *result ) );
		}

		return result;
	}
}
