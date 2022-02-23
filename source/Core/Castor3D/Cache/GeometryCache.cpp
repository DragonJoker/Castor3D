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
			for ( auto & pair : m_entries )
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
		, Pass const & pass )const
	{
		auto it = m_entries.find( hash( geometry, submesh, pass ) );
		CU_Require( it != m_entries.end() );
		return it->second;
	}

	GeometryCache::PoolsEntry const & ObjectCacheT< Geometry, castor::String, GeometryCacheTraits >::getEntry( uint32_t nodeId )const
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

	void ObjectCacheT< Geometry, castor::String, GeometryCacheTraits >::clear( RenderDevice const & device )
	{
		ElementObjectCacheT::clear();
		m_entries.clear();
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
		auto iresult = m_entries.emplace( baseHash
			, GeometryCache::PoolsEntry{ 0u
				, baseHash
				, geometry
				, submesh
				, pass } );

		if ( iresult.second )
		{
			auto & baseEntry = iresult.first->second;
			baseEntry.id = uint32_t( m_entries.size() );
			geometry.setId( submesh, baseEntry.id );
		}
	}

	void ObjectCacheT< Geometry, castor::String, GeometryCacheTraits >::doRemoveEntry( RenderDevice const & device
		, Geometry & geometry
		, Submesh const & submesh
		, Pass const & pass )
	{
		auto baseHash = hash( geometry, submesh, pass );
		geometry.setId( submesh, 0u );
		auto it = m_entries.find( baseHash );

		if ( it != m_entries.end() )
		{
			m_entries.erase( it );
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
