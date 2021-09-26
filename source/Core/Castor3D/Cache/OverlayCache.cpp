#include "Castor3D/Cache/OverlayCache.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Overlay/FontTexture.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Graphics/Font.hpp>

CU_ImplementCUSmartPtr( castor3d, OverlayCache )

namespace castor3d
{
	const castor::String PtrCacheTraitsT< castor3d::Overlay, castor::String >::Name = cuT( "Overlay" );
}

namespace castor
{
	using namespace castor3d;

	void ResourceCacheT< Overlay, String, OverlayCacheTraits >::initialise( Overlay & overlay )
	{
		auto level = overlay.computeLevel();

		if ( overlay.getParent() )
		{
			overlay.getParent()->addChild( &overlay );
		}

		while ( level >= m_overlayCountPerLevel.size() )
		{
			m_overlayCountPerLevel.resize( m_overlayCountPerLevel.size() * 2 );
		}

		overlay.setOrder( ++m_overlayCountPerLevel[level], int( level ) );
		m_overlays.insert( overlay.getCategory() );
	}

	void ResourceCacheT< Overlay, String, OverlayCacheTraits >::cleanup( Overlay & overlay )
	{
		if ( overlay.getChildrenCount() )
		{
			for ( auto child : overlay )
			{
				child->setPosition( child->getAbsolutePosition() );
				child->setSize( child->getAbsoluteSize() );
			}
		}

		m_overlays.erase( overlay.getCategory() );
	}

	ResourceCacheT< Overlay, String, OverlayCacheTraits >::ResourceCacheT( Engine & engine )
		: ElementCacheT{ engine.getLogger()
			, [this]( ElementT & resource )
			{
				initialise( resource );
			}
			, [this]( ElementT & resource )
			{
				cleanup( resource );
			}
			, castor::ResourceMergerT< OverlayCache >{ "_" } }
		, m_engine{ engine }
		, m_viewport{ engine }
		, m_overlayCountPerLevel{ 1000, 0 }
	{
		m_viewport.setOrtho( 0, 1, 1, 0, 0, 1000 );
	}

	void ResourceCacheT< Overlay, String, OverlayCacheTraits >::clear()
	{
		auto lock( makeUniqueLock( *this ) );
		doClearNoLock();
		m_overlays.clear();
		m_fontTextures.clear();
	}

	void ResourceCacheT< Overlay, String, OverlayCacheTraits >::cleanup()
	{
		auto lock( makeUniqueLock( *this ) );
		doCleanupNoLock();

		for ( auto it : m_fontTextures )
		{
			m_engine.postEvent( makeGpuCleanupEvent( *it.second ) );
		}
	}

	FontTextureSPtr ResourceCacheT< Overlay, String, OverlayCacheTraits >::getFontTexture( String const & name )
	{
		auto lock( makeUniqueLock( *this ) );
		auto it = m_fontTextures.find( name );
		FontTextureSPtr result;

		if ( it != m_fontTextures.end() )
		{
			result = it->second;
		}

		return result;
	}

	FontTextureSPtr ResourceCacheT< Overlay, String, OverlayCacheTraits >::createFontTexture( castor::FontResPtr font )
	{
		auto lock( makeUniqueLock( *this ) );
		auto fontName = font.lock()->getName();
		auto ires = m_fontTextures.emplace( fontName, nullptr );

		if ( ires.second )
		{
			auto result = std::make_shared< FontTexture >( m_engine, font );
			m_engine.postEvent( makeGpuFunctorEvent( EventType::ePreRender
				, [result]( RenderDevice const & device
					, QueueData const & queueData )
				{
					result->initialise( device, queueData );
				} ) );
			ires.first->second = result;
		}

		return ires.first->second;
	}
}
