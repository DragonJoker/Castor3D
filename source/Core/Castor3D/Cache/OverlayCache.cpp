#include "Castor3D/Cache/OverlayCache.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Event/Frame/CleanupEvent.hpp"
#include "Castor3D/Event/Frame/InitialiseEvent.hpp"
#include "Castor3D/Overlay/OverlayRenderer.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Graphics/Font.hpp>

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

	void OverlayCache::OverlayInitialiser::operator()( OverlaySPtr element )
	{
		int level = 0;

		if ( element->getParent() )
		{
			level = element->getParent()->getLevel() + 1;
			element->getParent()->addChild( element );
		}

		while ( level >= int( m_overlayCountPerLevel.size() ) )
		{
			m_overlayCountPerLevel.resize( m_overlayCountPerLevel.size() * 2 );
		}

		element->setOrder( ++m_overlayCountPerLevel[level], level );
		m_overlays.insert( element->getCategory() );
	}

	//*************************************************************************************************

	OverlayCache::OverlayCleaner::OverlayCleaner( Cache< Overlay, castor::String > & cache )
		: m_overlays{ cache.m_overlays }
		, m_overlayCountPerLevel{ cache.m_overlayCountPerLevel }
	{
	}

	void OverlayCache::OverlayCleaner::operator()( OverlaySPtr element )
	{
		if ( element->getChildrenCount() )
		{
			for ( auto child : *element )
			{
				child->setPosition( child->getAbsolutePosition() );
				child->setSize( child->getAbsoluteSize() );
			}
		}

		m_overlays.erase( element->getCategory() );
	}

	//*************************************************************************************************

	Cache< Overlay, castor::String >::Cache( Engine & engine
		 , Producer && produce
		 , Initialiser && initialise
		 , Cleaner && clean
		 , Merger && merge )
		: MyCacheType( engine
		   , std::move( produce )
		   , std::bind( OverlayInitialiser{ *this }, std::placeholders::_1 )
		   , std::bind( OverlayCleaner{ *this }, std::placeholders::_1 )
		   , std::move( merge ) )
		, m_overlayCountPerLevel{ 1000, 0 }
		, m_viewport{ engine }
	{
		m_viewport.setOrtho( 0, 1, 1, 0, 0, 1000 );
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
		auto lock = makeUniqueLock( *this );

		for ( auto it : m_fontTextures )
		{
			getEngine()->postEvent( makeCleanupEvent( *it.second ) );
		}
	}

	FontTextureSPtr Cache< Overlay, castor::String >::getFontTexture( castor::String const & name )
	{
		auto it = m_fontTextures.find( name );
		FontTextureSPtr result;

		if ( it != m_fontTextures.end() )
		{
			result = it->second;
		}

		return result;
	}

	FontTextureSPtr Cache< Overlay, castor::String >::createFontTexture( castor::FontSPtr font )
	{
		auto it = m_fontTextures.find( font->getName() );
		FontTextureSPtr result;

		if ( it == m_fontTextures.end() )
		{
			result = std::make_shared< FontTexture >( *getEngine(), font );
			m_fontTextures.emplace( font->getName(), result );
			getEngine()->postEvent( makeInitialiseEvent( *result ) );
		}

		return result;
	}
}
