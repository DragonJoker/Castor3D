#include "OverlayCache.hpp"

#include "Engine.hpp"
#include "Event/Frame/CleanupEvent.hpp"
#include "Event/Frame/InitialiseEvent.hpp"
#include "Overlay/OverlayRenderer.hpp"
#include "Scene/Scene.hpp"
#include "Scene/SceneFileParser.hpp"

#include <Graphics/Font.hpp>

using namespace castor;

#if defined( drawText )
#	undef drawText
#endif

namespace castor3d
{
	//*************************************************************************************************

	template<> const String CacheTraits< Overlay, String >::Name = cuT( "Overlay" );

	//*************************************************************************************************

	OverlayCache::OverlayInitialiser::OverlayInitialiser( Cache< Overlay, castor::String > & cache )
		: m_overlays{ cache.m_overlays }
		, m_overlayCountPerLevel{ cache.m_overlayCountPerLevel }
	{
	}

	void OverlayCache::OverlayInitialiser::operator()( OverlaySPtr p_element )
	{
		int level = 0;

		if ( p_element->getParent() )
		{
			level = p_element->getParent()->getLevel() + 1;
			p_element->getParent()->addChild( p_element );
		}

		while ( level >= int( m_overlayCountPerLevel.size() ) )
		{
			m_overlayCountPerLevel.resize( m_overlayCountPerLevel.size() * 2 );
		}

		p_element->setOrder( ++m_overlayCountPerLevel[level], level );
		m_overlays.insert( p_element->getCategory() );
	}

	//*************************************************************************************************

	OverlayCache::OverlayCleaner::OverlayCleaner( Cache< Overlay, castor::String > & cache )
		: m_overlays{ cache.m_overlays }
		, m_overlayCountPerLevel{ cache.m_overlayCountPerLevel }
	{
	}

	void OverlayCache::OverlayCleaner::operator()( OverlaySPtr p_element )
	{
		if ( p_element->getChildrenCount() )
		{
			for ( auto child : *p_element )
			{
				child->setPosition( child->getAbsolutePosition() );
				child->setSize( child->getAbsoluteSize() );
			}
		}

		m_overlays.erase( p_element->getCategory() );
	}

	//*************************************************************************************************

	Cache< Overlay, castor::String >::Cache( Engine & engine
		 , Producer && p_produce
		 , Initialiser && p_initialise
		 , Cleaner && p_clean
		 , Merger && p_merge )
		: MyCacheType( engine
		   , std::move( p_produce )
		   , std::bind( OverlayInitialiser{ *this }, std::placeholders::_1 )
		   , std::bind( OverlayCleaner{ *this }, std::placeholders::_1 )
		   , std::move( p_merge ) )
		, m_overlayCountPerLevel{ 1000, 0 }
		, m_viewport{ *getEngine() }
	{
		m_viewport.setOrtho( 0, 1, 1, 0, 0, 1000 );
		getEngine()->postEvent( makeInitialiseEvent( m_viewport ) );
	}

	Cache< Overlay, castor::String >::~Cache()
	{
	}

	void Cache< Overlay, castor::String >::clear()
	{
		auto lock = makeUniqueLock( *this );
		m_overlays.clear();
		m_fontTextures.clear();
	}

	void Cache< Overlay, castor::String >::cleanup()
	{
		m_viewport.cleanup();
		auto lock = makeUniqueLock( *this );

		for ( auto it : m_fontTextures )
		{
			getEngine()->postEvent( makeCleanupEvent( *it.second ) );
		}
	}

	void Cache< Overlay, castor::String >::updateRenderer()
	{
		if ( getEngine()->isCleaned() )
		{
			if ( m_pRenderer )
			{
				m_pRenderer->cleanup();
				m_pRenderer.reset();
			}
		}
		else
		{
			if ( !m_pRenderer )
			{
				m_pRenderer = std::make_shared< OverlayRenderer >( *getEngine()->getRenderSystem() );
				m_pRenderer->initialise();
			}
		}
	}

	void Cache< Overlay, castor::String >::update()
	{
		auto lock = makeUniqueLock( *this );

		for ( auto category : m_overlays )
		{
			category->update();
		}
	}

	void Cache< Overlay, castor::String >::render( Scene const & p_scene, castor::Size const & p_size )
	{
		auto lock = makeUniqueLock( *this );

		if ( m_pRenderer )
		{
			m_viewport.resize( p_size );
			m_viewport.updateRight( real( p_size.getWidth() ) );
			m_viewport.updateBottom( real( p_size.getHeight() ) );
			m_viewport.update();
			m_pRenderer->beginRender( m_viewport );

			for ( auto category : m_overlays )
			{
				SceneSPtr scene = category->getOverlay().getScene();

				if ( category->getOverlay().isVisible() && ( !scene || scene->getName() == p_scene.getName() ) )
				{
					category->render();
				}
			}

			m_pRenderer->endRender();
		}
	}

	bool Cache< Overlay, castor::String >::write( castor::TextFile & p_file )const
	{
		auto lock = makeUniqueLock( *this );
		bool result = true;
		auto it = m_overlays.begin();
		bool first = true;

		while ( result && it != m_overlays.end() )
		{
			Overlay const & overlay = ( *it )->getOverlay();

			if ( first )
			{
				first = false;
			}
			else
			{
				p_file.writeText( cuT( "\n" ) );
			}

			result = Overlay::TextWriter( String{} )( overlay, p_file );
			++it;
		}

		return result;
	}

	bool Cache< Overlay, castor::String >::read( castor::TextFile & p_file )
	{
		auto lock = makeUniqueLock( *this );
		SceneFileParser parser( *getEngine() );
		return parser.parseFile( p_file );
	}

	FontTextureSPtr Cache< Overlay, castor::String >::getFontTexture( castor::String const & p_name )
	{
		auto it = m_fontTextures.find( p_name );
		FontTextureSPtr result;

		if ( it != m_fontTextures.end() )
		{
			result = it->second;
		}

		return result;
	}

	FontTextureSPtr Cache< Overlay, castor::String >::createFontTexture( castor::FontSPtr p_font )
	{
		auto it = m_fontTextures.find( p_font->getName() );
		FontTextureSPtr result;

		if ( it == m_fontTextures.end() )
		{
			result = std::make_shared< FontTexture >( *getEngine(), p_font );
			m_fontTextures.emplace( p_font->getName(), result );
			getEngine()->postEvent( makeInitialiseEvent( *result ) );
		}

		return result;
	}
}
