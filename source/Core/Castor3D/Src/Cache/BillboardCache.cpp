#include "BillboardCache.hpp"

#include "Engine.hpp"

#include "Event/Frame/FrameListener.hpp"
#include "Event/Frame/FunctorEvent.hpp"
#include "Material/Material.hpp"
#include "Material/Pass.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Scene.hpp"

#include <Miscellaneous/Hash.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		size_t hash( BillboardBase const & billboard
			, Pass const & pass )
		{
			size_t result = std::hash< BillboardBase const * >{}( &billboard );
			castor::hashCombine( result, &pass );
			return result;
		}
	}

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
		, m_modelUboPool{ *engine.getRenderSystem() }
		, m_modelMatrixUboPool{ *engine.getRenderSystem() }
		, m_billboardUboPool{ *engine.getRenderSystem() }
	{
	}

	BillboardListCache::~ObjectCache()
	{
	}

	void BillboardListCache::update()
	{
		for ( auto & [hash, entry] : m_entries )
		{
			auto & modelData = entry.modelUbo.getData();
			modelData.shadowReceiver = entry.billboard.isShadowReceiver();
			modelData.materialIndex = entry.pass.getId() - 1u;
			auto & modelMatrixData = entry.modelMatrixUbo.getData();
			modelMatrixData.model = convert( entry.billboard.getNode()->getDerivedTransformationMatrix() );
			auto & billboardData = entry.billboardUbo.getData();
			billboardData.dimensions = entry.billboard.getDimensions();
		}
	}

	void BillboardListCache::uploadUbos()
	{
		m_modelUboPool.upload();
		m_modelMatrixUboPool.upload();
		m_billboardUboPool.upload();
	}

	void BillboardListCache::cleanupUbos()
	{
		m_modelUboPool.cleanup();
		m_modelMatrixUboPool.cleanup();
		m_billboardUboPool.cleanup();
	}

	BillboardListCache::PoolsEntry BillboardListCache::getUbos( BillboardBase const & billboard, Pass const & pass )const
	{
		return m_entries.at( hash( billboard, pass ) );
	}

	void BillboardListCache::clear()
	{
		MyObjectCache::clear();

		for ( auto & entry : m_entries )
		{
			m_modelUboPool.putBuffer( entry.second.modelUbo );
			m_modelMatrixUboPool.putBuffer( entry.second.modelMatrixUbo );
			m_billboardUboPool.putBuffer( entry.second.billboardUbo );
		}
	}

	BillboardListSPtr BillboardListCache::add( Key const & name
		, SceneNodeSPtr parent )
	{
		auto result = MyObjectCache::add( name, parent );
		doRegister( *result );
		return result;
	}

	void BillboardListCache::remove( Key const & name )
	{
		auto lock = castor::makeUniqueLock( m_elements );

		if ( m_elements.has( name ) )
		{
			auto element = m_elements.find( name );
			m_detach( element );
			m_elements.erase( name );
			onChanged();
			doUnregister( *element );
		}
	}
	
	BillboardListCache::PoolsEntry BillboardListCache::doCreateEntry( BillboardBase const & billboard
		, Pass const & pass )
	{
		return
		{
			billboard,
			pass,
			m_modelUboPool.getBuffer( renderer::MemoryPropertyFlag::eHostVisible ),
			m_modelMatrixUboPool.getBuffer( renderer::MemoryPropertyFlag::eHostVisible ),
			m_billboardUboPool.getBuffer( renderer::MemoryPropertyFlag::eHostVisible ),
		};
	}

	void BillboardListCache::doRemoveEntry( BillboardBase const & billboard
		, Pass const & pass )
	{
		auto entry = getUbos( billboard, pass );
		m_entries.erase( hash( billboard, pass ) );
		m_modelUboPool.putBuffer( entry.modelUbo );
		m_modelMatrixUboPool.putBuffer( entry.modelMatrixUbo );
		m_billboardUboPool.putBuffer( entry.billboardUbo );
	}

	void BillboardListCache::doRegister( BillboardBase & billboard )
	{
		m_connections.emplace( &billboard, billboard.onMaterialChanged.connect( [this]( BillboardBase const & billboard
			, MaterialSPtr oldMaterial
			, MaterialSPtr newMaterial )
		{
			if ( oldMaterial )
			{
				for ( auto & pass : *oldMaterial )
				{
					doRemoveEntry( billboard, *pass );
				}
			}

			if ( newMaterial )
			{
				for ( auto & pass : *newMaterial )
				{
					m_entries.emplace( hash( billboard, *pass )
						, doCreateEntry( billboard, *pass ) );
				}
			}
		} ) );

		for ( auto & pass : *billboard.getMaterial() )
		{
			m_entries.emplace( hash( billboard, *pass )
				, doCreateEntry( billboard, *pass ) );
		}
	}

	void BillboardListCache::doUnregister( BillboardBase & billboard )
	{
		m_connections.erase( &billboard );

		for ( auto & pass : *billboard.getMaterial() )
		{
			doRemoveEntry( billboard, *pass );
		}
	}
}
