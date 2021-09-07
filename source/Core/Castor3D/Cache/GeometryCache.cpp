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
#include "Castor3D/Render/RenderPass.hpp"
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

	namespace
	{
		castor::String printhex( uint64_t v )
		{
			auto stream = castor::makeStringStream();
			stream << "0x" << std::hex << std::setw( 16u ) << std::setfill( '0' ) << v;
			return stream.str();
		}
	}

	//*********************************************************************************************

	size_t hash( Geometry const & geometry
		, Submesh const & submesh
		, Pass const & pass )
	{
		size_t result = std::hash< Geometry const * >{}( &geometry );
		castor::hashCombinePtr( result, submesh );
		castor::hashCombinePtr( result, pass );
		return result;
	}

	size_t hash( Geometry const & geometry
		, Submesh const & submesh
		, Pass const & pass
		, uint32_t instanceMult )
	{
		size_t result = std::hash< uint32_t >{}( instanceMult );
		castor::hashCombinePtr( result, geometry );
		castor::hashCombinePtr( result, submesh );
		castor::hashCombinePtr( result, pass );
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
	{
	}

	void ObjectCacheT< Geometry, castor::String, GeometryCacheTraits >::registerPass( SceneRenderPass const & renderPass )
	{
		auto instanceMult = renderPass.getInstanceMult();
		auto iresult = m_instances.emplace( instanceMult, RenderPassSet{} );

		if ( iresult.second )
		{
			auto & device = getScene()->getEngine()->getRenderSystem()->getRenderDevice();
			auto & uboPools = *device.uboPools;

			for ( auto entry : m_baseEntries )
			{
				entry.second.hash = hash( entry.second.geometry
					, entry.second.submesh
					, entry.second.pass
					, instanceMult );
				auto it = m_entries.emplace( entry.second.hash, entry.second ).first;
				it->second.id = int32_t( m_entries.size() );

				if ( instanceMult )
				{
					it->second.modelInstancesUbo = uboPools.getBuffer< ModelInstancesUboConfiguration >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
				}
			}
		}

		iresult.first->second.insert( &renderPass );
	}

	void ObjectCacheT< Geometry, castor::String, GeometryCacheTraits >::unregisterPass( SceneRenderPass const * renderPass
		, uint32_t instanceMult )
	{
		auto instIt = m_instances.find( instanceMult );

		if ( instIt != m_instances.end() )
		{
			auto it = instIt->second.find( renderPass );

			if ( it != instIt->second.end() )
			{
				instIt->second.erase( it );
			}

			if ( instIt->second.empty() )
			{
				m_instances.erase( instIt );
				auto & device = getScene()->getEngine()->getRenderSystem()->getRenderDevice();
				auto & uboPools = *device.uboPools;

				for ( auto & entry : m_baseEntries )
				{
					auto it = m_entries.find( hash( entry.second.geometry
						, entry.second.submesh
						, entry.second.pass
						, instanceMult ) );

					if ( it != m_entries.end() )
					{
						auto entry = it->second;
						m_entries.erase( it );

						if ( entry.modelInstancesUbo )
						{
							uboPools.putBuffer( entry.modelInstancesUbo );
						}
					}
				}

				uint32_t id = 1u;

				for ( auto & entry : m_entries )
				{
					entry.second.id = id++;
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
		for ( auto & pair : m_baseEntries )
		{
			auto & entry = pair.second;

			if ( entry.geometry.getParent()
				&& bool( entry.modelUbo ) )
			{
				auto & modelData = entry.modelUbo.getData();
				modelData.nodeId = entry.id;
				modelData.shadowReceiver = entry.geometry.isShadowReceiver();
				modelData.materialIndex = entry.pass.getId();

				if ( entry.pass.hasEnvironmentMapping() )
				{
					modelData.environmentIndex = getScene()->getEnvironmentMapIndex( *entry.geometry.getParent() ) + 1u;
				}

				modelData.prvModel = modelData.curModel;
				modelData.curModel = entry.geometry.getParent()->getDerivedTransformationMatrix();
				auto normal = castor::Matrix3x3f{ modelData.curModel };
				modelData.normal = castor::Matrix4x4f{ normal.getInverse().getTransposed() };
			}
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
		auto & uboPools = *device.uboPools;

		for ( auto & entry : m_entries )
		{
			if ( entry.second.modelInstancesUbo )
			{
				uboPools.putBuffer( entry.second.modelInstancesUbo );
			}
		}

		for ( auto & entry : m_baseEntries )
		{
			uboPools.putBuffer( entry.second.modelUbo );
		}

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
		, Geometry const & geometry
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
			auto & uboPools = *device.uboPools;
			auto & baseEntry = iresult.first->second;
			baseEntry.id = int32_t( m_baseEntries.size( ));
			baseEntry.modelUbo = uboPools.getBuffer< ModelUboConfiguration >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

			for ( auto instanceMult : m_instances )
			{
				auto entry = baseEntry;

				if ( instanceMult.first > 1 )
				{
					entry.modelInstancesUbo = uboPools.getBuffer< ModelInstancesUboConfiguration >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
				}

				entry.hash = hash( geometry, submesh, pass, instanceMult.first );
				m_entries.emplace( entry.hash, entry );
			}
		}
	}

	void ObjectCacheT< Geometry, castor::String, GeometryCacheTraits >::doRemoveEntry( RenderDevice const & device
		, Geometry const & geometry
		, Submesh const & submesh
		, Pass const & pass )
	{
		auto & uboPools = *device.uboPools;
		auto baseHash = hash( geometry, submesh, pass );

		for ( auto instanceMult : m_instances )
		{
			auto it = m_entries.find( hash( geometry, submesh, pass, instanceMult.first ) );

			if ( it != m_entries.end() )
			{
				auto entry = it->second;
				m_entries.erase( it );

				if ( entry.modelInstancesUbo )
				{
					uboPools.putBuffer( entry.modelInstancesUbo );
				}
			}
		}

		auto it = m_baseEntries.find( baseHash );

		if ( it != m_baseEntries.end() )
		{
			auto entry = it->second;
			m_baseEntries.erase( it );
			uboPools.putBuffer( entry.modelUbo );
		}
	}

	void ObjectCacheT< Geometry, castor::String, GeometryCacheTraits >::doRegister( Geometry & geometry )
	{
		auto & device = getScene()->getEngine()->getRenderSystem()->getRenderDevice();
		m_connections.emplace( &geometry
			, geometry.onMaterialChanged.connect( [this, &device]( Geometry const & geometry
					, Submesh const & submesh
					, MaterialRPtr oldMaterial
					, MaterialRPtr newMaterial )
				{
					if ( oldMaterial )
					{
						for ( auto & pass : *oldMaterial )
						{
							doRemoveEntry( device, geometry, submesh, *pass );
						}
					}

					if ( newMaterial )
					{
						for ( auto & pass : *newMaterial )
						{
							doCreateEntry( device, geometry, submesh, *pass );
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
