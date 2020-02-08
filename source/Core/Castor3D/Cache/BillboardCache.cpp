#include "Castor3D/Cache/BillboardCache.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Event/Frame/FunctorEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass.hpp"
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
		, m_pools{ scene.getBillboardPools() }
	{
	}

	BillboardListCache::~ObjectCache()
	{
	}

	void BillboardListCache::clear()
	{
		for ( auto & element : m_elements )
		{
			m_pools.unregisterElement( *element.second );
		}

		MyObjectCache::clear();
	}

	BillboardListSPtr BillboardListCache::add( Key const & name
		, SceneNodeSPtr parent )
	{
		auto result = MyObjectCache::add( name, parent );
		m_pools.registerElement( *result );
		return result;
	}

	void BillboardListCache::add( ElementPtr element )
	{
		m_initialise( element );
		MyObjectCache::add( element->getName(), element );
		m_pools.registerElement( *element );
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
			m_pools.unregisterElement( *element );
		}
	}
}
