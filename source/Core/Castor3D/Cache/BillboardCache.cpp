#include "Castor3D/Cache/BillboardCache.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

CU_ImplementCUSmartPtr( castor3d, BillboardListCache )

namespace castor3d
{
	const castor::String ObjectCacheTraitsT< BillboardList, castor::String >::Name = cuT( "BillboardList" );

	//*********************************************************************************************

	size_t hash( BillboardBase const & billboard
		, Pass const & pass )
	{
		size_t result = std::hash< BillboardBase const * >{}( &billboard );
		castor::hashCombinePtr( result, pass );
		return result;
	}

	size_t hash( BillboardBase const & billboard
		, Pass const & pass
		, uint32_t instanceMult )
	{
		size_t result = std::hash< uint32_t >{}( instanceMult );
		castor::hashCombinePtr( result, billboard );
		castor::hashCombinePtr( result, pass );
		return result;
	}

	//*********************************************************************************************

	ObjectCacheT< BillboardList, castor::String, BillboardCacheTraits >::ObjectCacheT( Scene & scene
		, SceneNodeSPtr rootNode
		, SceneNodeSPtr rootCameraNode
		, SceneNodeSPtr rootObjectNode )
		: ElementObjectCacheT{ scene
			, rootNode
			, rootCameraNode
			, rootCameraNode
			, [this]( ElementT & element )
			{
				registerElement( element );
				getScene()->getListener().postEvent( makeGpuInitialiseEvent( element ) );
			}
			, [this]( ElementT & element )
			{
				getScene()->getListener().postEvent( makeGpuCleanupEvent( element ) );
				unregisterElement( element );
			}
			, MovableMergerT< BillboardListCache >{ scene.getName() }
			, MovableAttacherT< BillboardListCache >{}
			, MovableDetacherT< BillboardListCache >{} }
		, m_device{ scene.getEngine()->getRenderSystem()->getRenderDevice() }
		, m_nodesData{ makeBuffer< ModelBufferConfiguration >( m_device
			, 250'000ull
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, "BillboardNodesData" ) }
		, m_billboardsData{ makeBuffer< BillboardUboConfiguration >( m_device
			, 250'000ull
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, "BillboardsDimensions" ) }
	{
	}

	void ObjectCacheT< BillboardList, castor::String, BillboardCacheTraits >::initialise( RenderDevice const & device )
	{
		if ( !m_nodesData )
		{
			m_nodesData = makeBuffer< ModelBufferConfiguration >( m_device
				, 250'000ull
				, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, "BillboardNodesData" );
			m_billboardsData = makeBuffer< BillboardUboConfiguration >( m_device
				, 250'000ull
				, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, "BillboardsDimensions" );
		}
	}

	void ObjectCacheT< BillboardList, castor::String, BillboardCacheTraits >::cleanup()
	{
		ObjectCacheBaseT< BillboardList, castor::String, BillboardCacheTraits >::cleanup();
		m_billboardsData.reset();
		m_nodesData.reset();
	}

	void ObjectCacheT< BillboardList, castor::String, BillboardCacheTraits >::registerPass( RenderNodesPass const & renderPass )
	{
		auto instanceMult = renderPass.getInstanceMult();
		auto iresult = m_instances.emplace( instanceMult, RenderPassSet{} );

		if ( iresult.second )
		{
			for ( auto entry : m_baseEntries )
			{
				entry.second.hash = hash( entry.second.billboard
					, entry.second.pass
					, instanceMult );
				auto it = m_entries.emplace( entry.second.hash, entry.second ).first;
				it->second.id = int32_t( m_entries.size() );
			}
		}

		iresult.first->second.insert( &renderPass );
	}

	void ObjectCacheT< BillboardList, castor::String, BillboardCacheTraits >::unregisterPass( RenderNodesPass const * renderPass
		, uint32_t instanceMult )
	{
		auto instIt = m_instances.find( instanceMult );

		if ( instIt != m_instances.end() )
		{
			auto rendIt = instIt->second.find( renderPass );

			if ( rendIt != instIt->second.end() )
			{
				instIt->second.erase( rendIt );
			}

			if ( instIt->second.empty() )
			{
				m_instances.erase( instIt );

				for ( auto & entry : m_baseEntries )
				{
					auto it = m_entries.find( hash( entry.second.billboard
						, entry.second.pass
						, instanceMult ) );

					if ( it != m_entries.end() )
					{
						auto entrySave = it->second;
						m_entries.erase( it );
					}
				}

				uint32_t id = 1u;

				for ( auto & entry : m_entries )
				{
					entry.second.id = int( id++ );
				}
			}
		}
	}

	void ObjectCacheT< BillboardList, castor::String, BillboardCacheTraits >::registerElement( BillboardBase & billboard )
	{
		getEngine()->sendEvent( makeGpuFunctorEvent( EventType::ePreRender
			, [this, &billboard]( RenderDevice const & device
				, QueueData const & queueData )
			{
				m_connections.emplace( &billboard
					, billboard.onMaterialChanged.connect( [this, &device]( BillboardBase & pbillboard
						, MaterialRPtr oldMaterial
						, MaterialRPtr newMaterial )
						{
							if ( oldMaterial )
							{
								for ( auto & pass : *oldMaterial )
								{
									doRemoveEntry( device, pbillboard, *pass );
								}
							}

							if ( newMaterial )
							{
								for ( auto & pass : *newMaterial )
								{
									doCreateEntry( device, pbillboard, *pass );
								}
							}
						} ) );

				for ( auto & pass : *billboard.getMaterial() )
				{
					doCreateEntry( device, billboard, *pass );
				}
			} ) );
	}

	void ObjectCacheT< BillboardList, castor::String, BillboardCacheTraits >::unregisterElement( BillboardBase & billboard )
	{
		getEngine()->sendEvent( makeGpuFunctorEvent( EventType::ePreRender
			, [this, &billboard]( RenderDevice const & device
				, QueueData const & queueData )
			{
				m_connections.erase( &billboard );

				for ( auto & pass : *billboard.getMaterial() )
				{
					doRemoveEntry( device, billboard, *pass );
				}
			} ) );
	}

	void ObjectCacheT< BillboardList, castor::String, BillboardCacheTraits >::clear( RenderDevice const & device )
	{
		auto lock( castor::makeUniqueLock( *this ) );
		ElementObjectCacheT::doClearNoLock();
	}


	void ObjectCacheT< BillboardList, castor::String, BillboardCacheTraits >::update( CpuUpdater & updater )
	{
		auto lock( castor::makeUniqueLock( *this ) );
		auto nodesBuffer = m_nodesData->lock( 0u, ashes::WholeSize, 0u );
		auto billboardsBuffer = m_billboardsData->lock( 0u, ashes::WholeSize, 0u );

		for ( auto & pair : m_baseEntries )
		{
			auto & entry = pair.second;
			fillEntry( entry.pass
				, *entry.billboard.getNode()
				, entry.billboard
				, nodesBuffer[entry.id - 1u] );
			auto & billboardData = billboardsBuffer[entry.id - 1u];
			billboardData.dimensions = entry.billboard.getDimensions();
		}

		m_nodesData->flush( 0u, ashes::WholeSize );
		m_nodesData->unlock();
		m_billboardsData->flush( 0u, ashes::WholeSize );
		m_billboardsData->unlock();
	}

	BillboardListCache::PoolsEntry ObjectCacheT< BillboardList, castor::String, BillboardCacheTraits >::getUbos( BillboardBase const & billboard
		, Pass const & pass
		, uint32_t instanceMult )const
	{
		auto lock( castor::makeUniqueLock( *this ) );
		auto it = m_entries.find( hash( billboard, pass, instanceMult ) );
		CU_Require( it != m_entries.end() );
		return it->second;
	}

	void ObjectCacheT< BillboardList, castor::String, BillboardCacheTraits >::doCreateEntry( RenderDevice const & device
		, BillboardBase & billboard
		, Pass const & pass )
	{
		auto baseHash = hash( billboard, pass );
		auto iresult = m_baseEntries.emplace( baseHash
			, BillboardListCache::PoolsEntry{ 0u
				, baseHash
				, billboard
				, pass } );

		if ( iresult.second )
		{
			auto & baseEntry = iresult.first->second;
			baseEntry.id = int32_t( m_baseEntries.size() );
			billboard.setId( uint32_t( baseEntry.id ) );

			for ( auto instanceMult : m_instances )
			{
				auto entry = baseEntry;
				entry.hash = hash( billboard, pass, instanceMult.first );
				m_entries.emplace( entry.hash, entry );
			}
		}
	}

	void ObjectCacheT< BillboardList, castor::String, BillboardCacheTraits >::doRemoveEntry( RenderDevice const & device
		, BillboardBase & billboard
		, Pass const & pass )
	{
		auto baseHash = hash( billboard, pass );

		for ( auto instanceMult : m_instances )
		{
			auto it = m_entries.find( hash( billboard, pass, instanceMult.first ) );

			if ( it != m_entries.end() )
			{
				auto entry = it->second;
				m_entries.erase( it );
			}
		}

		billboard.setId( 0u );
		auto it = m_baseEntries.find( baseHash );

		if ( it != m_baseEntries.end() )
		{
			auto entry = it->second;
			m_baseEntries.erase( it );
		}
	}

	//*********************************************************************************************
}
