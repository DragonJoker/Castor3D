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

		for ( auto & pair : m_entries )
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
		, Pass const & pass )const
	{
		auto lock( castor::makeUniqueLock( *this ) );
		auto it = m_entries.find( hash( billboard, pass ) );
		CU_Require( it != m_entries.end() );
		return it->second;
	}

	BillboardListCache::PoolsEntry const & ObjectCacheT< BillboardList, castor::String, BillboardCacheTraits >::getEntry( uint32_t nodeId )const
	{
		++nodeId;
		auto it = std::find_if( m_entries.begin()
			, m_entries.end()
			, [nodeId]( std::map< size_t, PoolsEntry >::value_type const & lookup )
			{
				return nodeId == lookup.second.id;
			} );
		CU_Require( it != m_entries.end() );
		return it->second;
	}

	void ObjectCacheT< BillboardList, castor::String, BillboardCacheTraits >::doCreateEntry( RenderDevice const & device
		, BillboardBase & billboard
		, Pass const & pass )
	{
		auto baseHash = hash( billboard, pass );
		auto iresult = m_entries.emplace( baseHash
			, BillboardListCache::PoolsEntry{ 0u
				, baseHash
				, billboard
				, pass } );

		if ( iresult.second )
		{
			auto & baseEntry = iresult.first->second;
			baseEntry.id = uint32_t( m_entries.size() );
			billboard.setId( baseEntry.id );
		}
	}

	void ObjectCacheT< BillboardList, castor::String, BillboardCacheTraits >::doRemoveEntry( RenderDevice const & device
		, BillboardBase & billboard
		, Pass const & pass )
	{
		auto baseHash = hash( billboard, pass );
		billboard.setId( 0u );
		auto it = m_entries.find( baseHash );

		if ( it != m_entries.end() )
		{
			auto entry = it->second;
			m_entries.erase( it );
		}
	}

	//*********************************************************************************************
}
