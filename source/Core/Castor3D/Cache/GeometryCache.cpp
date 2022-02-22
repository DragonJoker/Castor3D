#include "Castor3D/Cache/GeometryCache.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderInfo.hpp"
#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Render/EnvironmentMap/EnvironmentMap.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

CU_ImplementCUSmartPtr( castor3d, GeometryCache )

namespace castor3d
{
	const castor::String ObjectCacheTraitsT< Geometry, castor::String >::Name = cuT( "Geometry" );

	//*********************************************************************************************

	size_t hash( Geometry const & geometry
		, Submesh const & submesh
		, Pass const & pass )
	{
		size_t result = std::hash< castor::String >{}( geometry.getName() );
		castor::hashCombine( result, submesh.getOwner()->getName() );
		castor::hashCombine( result, submesh.getId() );
		castor::hashCombine( result, pass.getOwner()->getName() );
		castor::hashCombine( result, pass.getIndex() );
		return result;
	}

	size_t hash( Geometry const & geometry
		, Submesh const & submesh
		, Pass const & pass
		, uint32_t instanceMult )
	{
		auto result = hash( geometry, submesh, pass );
		castor::hashCombine( result, instanceMult );
		return result;
	}

	//*********************************************************************************************

	ObjectCacheT< Geometry, castor::String, GeometryCacheTraits >::ObjectCacheT( Scene & scene
		, SceneNodeSPtr rootNode
		, SceneNodeSPtr rootCameraNode
		, SceneNodeSPtr rootObjectNode )
		: ElementObjectCacheT{ scene
			, rootNode
			, rootCameraNode
			, rootCameraNode
			, [this]( ElementT & element )
			{
				doRegister( element );
				getScene()->getListener().postEvent( makeGpuFunctorEvent( EventType::ePreRender
					, [&element, this]( RenderDevice const & device
						, QueueData const & queueData )
					{
						element.prepare( m_faceCount, m_vertexCount );
					} ) );
			}
			, [this]( ElementT & element )
			{
				doUnregister( element );
			}
			, MovableMergerT< GeometryCache >{ scene.getName() }
			, MovableAttacherT< GeometryCache >{}
			, MovableDetacherT< GeometryCache >{} }
		, m_device{ scene.getEngine()->getRenderSystem()->getRenderDevice() }
		, m_nodesData{ makeBuffer< ModelBufferConfiguration >( m_device
			, 250'000ull
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, "GeometryNodesData" ) }
	{
	}

	void ObjectCacheT< Geometry, castor::String, GeometryCacheTraits >::initialise( RenderDevice const & device )
	{
		if ( !m_nodesData )
		{
			m_nodesData = makeBuffer< ModelBufferConfiguration >( m_device
				, 250'000ull
				, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, "GeometryNodesData" );
		}
	}

	void ObjectCacheT< Geometry, castor::String, GeometryCacheTraits >::cleanup()
	{
		ObjectCacheBaseT< Geometry, castor::String, GeometryCacheTraits >::cleanup();
		m_nodesData.reset();
	}

	void ObjectCacheT< Geometry, castor::String, GeometryCacheTraits >::registerPass( RenderNodesPass const & renderPass )
	{
		auto instanceMult = renderPass.getInstanceMult();
		auto iresult = m_instances.emplace( instanceMult, RenderPassSet{} );

		if ( iresult.second )
		{
			for ( auto entry : m_baseEntries )
			{
				entry.second.hash = hash( entry.second.geometry
					, entry.second.submesh
					, entry.second.pass
					, instanceMult );
				auto it = m_entries.emplace( entry.second.hash, entry.second ).first;
				it->second.id = int32_t( m_entries.size() );
			}
		}

		iresult.first->second.insert( &renderPass );
	}

	void ObjectCacheT< Geometry, castor::String, GeometryCacheTraits >::unregisterPass( RenderNodesPass const * renderPass
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
					auto it = m_entries.find( hash( entry.second.geometry
						, entry.second.submesh
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

	void ObjectCacheT< Geometry, castor::String, GeometryCacheTraits >::fillInfo( RenderInfo & info )const
	{
		auto lock( castor::makeUniqueLock( *this ) );

		for ( auto element : *this )
		{
			if ( auto mesh = element.second->getMesh().lock() )
			{
				info.m_totalObjectsCount += mesh->getSubmeshCount();
				info.m_totalVertexCount += mesh->getVertexCount();
				info.m_totalFaceCount += mesh->getFaceCount();
			}
		}
	}

	void ObjectCacheT< Geometry, castor::String, GeometryCacheTraits >::update( CpuUpdater & updater )
	{
		auto lock( castor::makeUniqueLock( *this ) );

		if ( auto nodesBuffer = m_nodesData->lock( 0u, ashes::WholeSize, 0u ) )
		{
			for ( auto & pair : m_baseEntries )
			{
				auto & entry = pair.second;

				if ( entry.geometry.getParent() )
				{
					fillEntry( entry.pass
						, *entry.geometry.getParent()
						, entry.geometry
						, nodesBuffer[entry.id - 1u] );
				}
			}

			m_nodesData->flush( 0u, ashes::WholeSize );
			m_nodesData->unlock();
		}
	}

	GeometryCache::PoolsEntry ObjectCacheT< Geometry, castor::String, GeometryCacheTraits >::getUbos( Geometry const & geometry
		, Submesh const & submesh
		, Pass const & pass
		, uint32_t instanceMult )const
	{
		auto it = m_entries.find( hash( geometry, submesh, pass, instanceMult ) );
		CU_Require( it != m_entries.end() );
		return it->second;
	}

	void ObjectCacheT< Geometry, castor::String, GeometryCacheTraits >::clear( RenderDevice const & device )
	{
		ElementObjectCacheT::clear();
		m_entries.clear();
		m_baseEntries.clear();
		m_instances.clear();
	}

	void ObjectCacheT< Geometry, castor::String, GeometryCacheTraits >::add( ElementPtrT element )
	{
		auto lock( castor::makeUniqueLock( *this ) );
		ElementObjectCacheT::doAddNoLock( element->getName(), element );
	}

	void ObjectCacheT< Geometry, castor::String, GeometryCacheTraits >::doCreateEntry( RenderDevice const & device
		, Geometry & geometry
		, Submesh const & submesh
		, Pass const & pass )
	{
		auto baseHash = hash( geometry, submesh, pass );
		auto iresult = m_baseEntries.emplace( baseHash
			, GeometryCache::PoolsEntry{ 0u
				, baseHash
				, geometry
				, submesh
				, pass } );

		if ( iresult.second )
		{
			auto & baseEntry = iresult.first->second;
			baseEntry.id = int32_t( m_baseEntries.size() );
			geometry.setId( submesh, uint32_t( baseEntry.id ) );

			for ( auto instanceMult : m_instances )
			{
				auto entry = baseEntry;
				entry.hash = hash( geometry, submesh, pass, instanceMult.first );
				m_entries.emplace( entry.hash, entry );
			}
		}
	}

	void ObjectCacheT< Geometry, castor::String, GeometryCacheTraits >::doRemoveEntry( RenderDevice const & device
		, Geometry & geometry
		, Submesh const & submesh
		, Pass const & pass )
	{
		auto baseHash = hash( geometry, submesh, pass );

		for ( auto instanceMult : m_instances )
		{
			auto it = m_entries.find( hash( geometry, submesh, pass, instanceMult.first ) );

			if ( it != m_entries.end() )
			{
				auto entry = it->second;
				m_entries.erase( it );
			}
		}

		geometry.setId( submesh, 0u );
		auto it = m_baseEntries.find( baseHash );

		if ( it != m_baseEntries.end() )
		{
			auto entry = it->second;
			m_baseEntries.erase( it );
		}
	}

	void ObjectCacheT< Geometry, castor::String, GeometryCacheTraits >::doRegister( Geometry & geometry )
	{
		auto & device = getScene()->getEngine()->getRenderSystem()->getRenderDevice();
		m_connections.emplace( &geometry
			, geometry.onMaterialChanged.connect( [this, &device]( Geometry & pgeometry
					, Submesh const & submesh
					, MaterialRPtr oldMaterial
					, MaterialRPtr newMaterial )
				{
					if ( oldMaterial )
					{
						for ( auto & pass : *oldMaterial )
						{
							doRemoveEntry( device, pgeometry, submesh, *pass );
						}
					}

					if ( newMaterial )
					{
						for ( auto & pass : *newMaterial )
						{
							doCreateEntry( device, pgeometry, submesh, *pass );
						}
					}
				} ) );

		if ( auto mesh = geometry.getMesh().lock() )
		{
			for ( auto & submesh : *mesh )
			{
				auto material = geometry.getMaterial( *submesh );

				if ( material )
				{
					for ( auto & pass : *material )
					{
						doCreateEntry( device, geometry, *submesh, *pass );
					}
				}
			}
		}
	}

	void ObjectCacheT< Geometry, castor::String, GeometryCacheTraits >::doUnregister( Geometry & geometry )
	{
		m_connections.erase( &geometry );

		if ( auto mesh = geometry.getMesh().lock() )
		{
			for ( auto & submesh : *mesh )
			{
				for ( auto & pass : *geometry.getMaterial( *submesh ) )
				{
					doRemoveEntry( getScene()->getEngine()->getRenderSystem()->getRenderDevice()
						, geometry
						, *submesh
						, *pass );
				}
			}
		}
	}

	//*********************************************************************************************
}
