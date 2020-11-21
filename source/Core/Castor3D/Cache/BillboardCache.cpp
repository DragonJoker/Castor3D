#include "Castor3D/Cache/BillboardCache.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Scene.hpp"

using namespace castor;

namespace castor3d
{
	template<> const String ObjectCacheTraits< BillboardList, String >::Name = cuT( "BillboardList" );

	BillboardListCache::ObjectCache( Engine & engine
		, Scene & scene
		, SceneNodeSPtr rootNode
		, SceneNodeSPtr rootCameraNode
		, SceneNodeSPtr rootObjectNode
		, Producer && produce
		, Initialiser && initialise
		, Cleaner && clean
		, Merger && merge
		, Attacher && attach
		, Detacher && detach )
		: MyObjectCache( engine
			, scene
			, rootNode
			, rootCameraNode
			, rootCameraNode
			, std::move( produce )
			, std::move( initialise )
			, std::move( clean )
			, std::move( merge )
			, std::move( attach )
			, std::move( detach ) )
	{
		scene.getListener().postEvent( makeGpuFunctorEvent( EventType::ePreRender
			, [this]( RenderDevice const & device )
			{
				m_pools = std::make_shared< BillboardUboPools >( device );

				for ( auto pass : m_pendingPasses )
				{
					m_pools->registerPass( *pass );
				}
			} ) );
	}

	BillboardListCache::~ObjectCache()
	{
	}

	void BillboardListCache::registerPass( RenderPass const & renderPass )
	{
		if ( m_pools )
		{
			m_pools->registerPass( renderPass );
		}
		else
		{
			m_pendingPasses.insert( &renderPass );
		}
	}

	void BillboardListCache::unregisterPass( RenderPass const * renderPass
		, uint32_t instanceMult )
	{
		if ( m_pools )
		{
			m_pools->unregisterPass( renderPass, instanceMult );
		}
		else
		{
			auto it = m_pendingPasses.find( renderPass );

			if ( it != m_pendingPasses.end() )
			{
				m_pendingPasses.erase( it );
			}
		}
	}

	void BillboardListCache::cleanup( RenderDevice const & device )
	{
		if ( m_pools )
		{
			m_pools->clear( device );
		}
	}

	void BillboardListCache::clear()
	{
		for ( auto & element : m_elements )
		{
			m_pools->unregisterElement( *element.second );
		}

		MyObjectCache::clear();
	}

	void BillboardListCache::update( CpuUpdater & updater )
	{
		m_pools->update();
	}

	BillboardListSPtr BillboardListCache::add( Key const & name
		, SceneNode & parent )
	{
		auto result = MyObjectCache::add( name, parent );
		getEngine()->sendEvent( makeGpuFunctorEvent( EventType::ePreRender
			, [this, result]( RenderDevice const & device )
			{
				m_pools->registerElement( *result );
			} ) );
		return result;
	}

	void BillboardListCache::add( ElementPtr element )
	{
		m_initialise( element );
		MyObjectCache::add( element->getName(), element );
		getEngine()->sendEvent( makeGpuFunctorEvent( EventType::ePreRender
			, [this, element]( RenderDevice const & device )
			{
				m_pools->registerElement( *element );
			} ) );
	}

	void BillboardListCache::remove( Key const & name )
	{
		auto lock( castor::makeUniqueLock( m_elements ) );

		if ( m_elements.has( name ) )
		{
			auto element = m_elements.find( name );
			m_detach( element );
			m_elements.erase( name );
			onChanged();
			m_pools->unregisterElement( *element );
		}
	}
}
