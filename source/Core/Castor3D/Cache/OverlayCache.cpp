#include "Castor3D/Cache/OverlayCache.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Overlay/FontTexture.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Graphics/Font.hpp>

namespace castor
{
	using namespace castor3d;

	ResourceCacheT< Overlay, String >::ResourceCacheT( Engine & engine )
		: ElementCacheT{ engine.getLogger()
			, [&engine]( String const & name
				, OverlayType type
				, SceneSPtr scene
				, OverlaySPtr parent )
			{
				auto result = std::make_shared< Overlay >( engine
					, type
					, scene
					, parent );
				result->setName( name );
				return result;
			}
			, [this]( ElementPtrT resource )
			{
				int level = 0;

				if ( resource->getParent() )
				{
					level = resource->getParent()->getLevel() + 1;
					resource->getParent()->addChild( resource );
				}

				while ( level >= int( m_overlayCountPerLevel.size() ) )
				{
					m_overlayCountPerLevel.resize( m_overlayCountPerLevel.size() * 2 );
				}

				resource->setOrder( ++m_overlayCountPerLevel[level], level );
				m_overlays.insert( resource->getCategory() );
			}
			, [this]( ElementPtrT resource )
			{
				if ( resource->getChildrenCount() )
				{
					for ( auto child : *resource )
					{
						child->setPosition( child->getAbsolutePosition() );
						child->setSize( child->getAbsoluteSize() );
					}
				}

				m_overlays.erase( resource->getCategory() );
			} }
		, m_engine{ engine }
		, m_overlayCountPerLevel{ 1000, 0 }
		, m_viewport{ engine }
	{
		m_viewport.setOrtho( 0, 1, 1, 0, 0, 1000 );
	}

	void ResourceCacheT< Overlay, String >::clear()
	{
		auto lock( makeUniqueLock( *this ) );
		doClearNoLock();
		m_overlays.clear();
		m_fontTextures.clear();
	}

	void ResourceCacheT< Overlay, String >::cleanup()
	{
		auto lock( makeUniqueLock( *this ) );
		doCleanupNoLock();

		for ( auto it : m_fontTextures )
		{
			m_engine.postEvent( makeGpuCleanupEvent( *it.second ) );
		}
	}

	FontTextureSPtr ResourceCacheT< Overlay, String >::getFontTexture( String const & name )
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

	FontTextureSPtr ResourceCacheT< Overlay, String >::createFontTexture( FontSPtr font )
	{
		auto lock( makeUniqueLock( *this ) );
		auto it = m_fontTextures.find( font->getName() );
		FontTextureSPtr result;

		if ( it == m_fontTextures.end() )
		{
			result = std::make_shared< FontTexture >( m_engine, font );
			m_fontTextures.emplace( font->getName(), result );
			m_engine.postEvent( makeGpuFunctorEvent( EventType::ePreRender
				, [result]( RenderDevice const & device
					, QueueData const & queueData )
				{
					result->initialise( device, queueData );
				} ) );
		}

		return result;
	}
}
