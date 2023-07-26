#include "Castor3D/Cache/OverlayCache.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Overlay/FontTexture.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Graphics/Font.hpp>

CU_ImplementSmartPtr( castor3d, OverlayCache )

namespace castor3d
{
	const castor::String PtrCacheTraitsT< castor3d::Overlay, castor::String >::Name = cuT( "Overlay" );
}

namespace castor
{
	using namespace castor3d;

	void ResourceCacheT< Overlay, String, OverlayCacheTraits >::initialise( Overlay & overlay )
	{
		auto it = std::find( m_overlays.begin(), m_overlays.end(), &overlay.getCategory() );

		if ( it != m_overlays.end() )
		{
			return;
		}

		auto level = overlay.computeLevel();
		overlay.getCategory().setOrder( level
			, ++m_overlayCountPerLevel[level][uint32_t( overlay.getType() )] );

		if ( auto parent = overlay.getParent() )
		{
			parent->addChild( &overlay );
			it = std::find( m_overlays.begin(), m_overlays.end(), &parent->getCategory() );

			if ( it != m_overlays.end() )
			{
				m_overlays.insert( std::next( it ), &overlay.getCategory() );
			}
			else
			{
				initialise( *parent );
				it = std::find( m_overlays.begin(), m_overlays.end(), &parent->getCategory() );
				m_overlays.insert( std::next( it ), &overlay.getCategory() );
			}
		}
		else
		{
			m_overlays.insert( m_overlays.begin(), &overlay.getCategory() );
		}
	}

	void ResourceCacheT< Overlay, String, OverlayCacheTraits >::cleanup( Overlay & overlay )
	{
		if ( !overlay.empty() )
		{
			for ( auto child : overlay )
			{
				child->reparent( overlay.getParent() );
			}
		}

		if ( auto parent = overlay.getParent() )
		{
			parent->removeChild( &overlay );
		}

		auto it = std::find( m_overlays.begin(), m_overlays.end(), &overlay.getCategory() );

		if ( it != m_overlays.end() )
		{
			m_overlays.erase( it );
		}
	}

	void ResourceCacheT< Overlay, String, OverlayCacheTraits >::upload( castor3d::UploadData & uploader )
	{
		auto lock( makeUniqueLock( *this ) );

		for ( auto & texture : m_fontTextures )
		{
			if ( texture.second )
			{
				texture.second->upload( uploader );
			}
		}
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

		// Flatten hierarchy
		for ( auto overlay : m_overlays )
		{
			overlay->getOverlay().reparent( nullptr );
			overlay->getOverlay().clear();
		}

		m_overlays.clear();

		for ( auto & it : m_fontTextures )
		{
			m_engine.postEvent( makeGpuCleanupEvent( *it.second ) );
		}
	}

	FontTextureRPtr ResourceCacheT< Overlay, String, OverlayCacheTraits >::getFontTexture( String const & name )
	{
		auto lock( makeUniqueLock( *this ) );
		auto it = m_fontTextures.find( name );
		FontTextureRPtr result{};

		if ( it != m_fontTextures.end() )
		{
			result = it->second.get();
		}

		return result;
	}

	FontTextureRPtr ResourceCacheT< Overlay, String, OverlayCacheTraits >::createFontTexture( castor::FontResPtr font )
	{
		auto lock( makeUniqueLock( *this ) );
		auto fontName = font->getName();
		auto ires = m_fontTextures.emplace( fontName, nullptr );

		if ( ires.second )
		{
			auto result = castor::makeUnique< FontTexture >( m_engine, font );
			auto tmp = result.get();
			m_engine.postEvent( makeGpuFunctorEvent( GpuEventType::ePreUpload
				, [tmp]( RenderDevice const & device
					, QueueData const & queueData )
				{
					tmp->initialise( device, queueData );
				} ) );
			ires.first->second = std::move( result );
		}

		return ires.first->second.get();
	}
}
