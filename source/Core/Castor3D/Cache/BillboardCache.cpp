#include "Castor3D/Cache/BillboardCache.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/RenderPass.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

namespace castor3d
{
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

	ObjectCacheT< BillboardList, castor::String >::ObjectCacheT( Scene & scene
		, SceneNodeSPtr rootNode
		, SceneNodeSPtr rootCameraNode
		, SceneNodeSPtr rootObjectNode )
		: ElementObjectCacheT{ scene
			, rootNode
			, rootCameraNode
			, rootCameraNode
			, [this]( castor::String const & name
				, SceneNode & parent )
			{
				return std::make_shared< BillboardList >( name
					, *getScene()
					, parent );
			}
			, [this]( ElementPtrT element )
			{
				registerElement( *element );
				getScene()->getListener().postEvent( makeGpuInitialiseEvent( *element ) );
			}
			, [this]( ElementPtrT element )
			{
				getScene()->getListener().postEvent( makeGpuCleanupEvent( *element ) );
				unregisterElement( *element );
			}
			, [this]( ElementObjectCacheT const & source
				, ElementContT & destination
				, ElementPtrT element
				, SceneNodeSPtr rootCameraNode
				, SceneNodeSPtr rootObjectNode )
			{
				if ( element->getParent()->getName() == rootCameraNode->getName() )
				{
					element->detach();
					element->attachTo( *rootCameraNode );
				}
				else if ( element->getParent()->getName() == rootObjectNode->getName() )
				{
					element->detach();
					element->attachTo( *rootObjectNode );
				}

				auto name = element->getName();
				auto ires = destination.emplace( name, element );

				while ( !ires.second )
				{
					name = getScene()->getName() + cuT( "_" ) + name;
					ires = destination.emplace( name, element );
				}

				element->setName( name );
			}
			, []( ElementPtrT element
				, SceneNode & parent
				, SceneNodeSPtr rootNode
				, SceneNodeSPtr rootCameraNode
				, SceneNodeSPtr rootObjectNode )
			{
				parent.attachObject( *element );
			}
			, [this]( ElementPtrT element )
			{
				element->detach();
			} }
	{
	}

	void ObjectCacheT< BillboardList, castor::String >::registerPass( SceneRenderPass const & renderPass )
	{
		auto instanceMult = renderPass.getInstanceMult();
		auto iresult = m_instances.emplace( instanceMult, RenderPassSet{} );

		if ( iresult.second )
		{
			auto & device = getScene()->getEngine()->getRenderSystem()->getRenderDevice();
			auto & uboPools = *device.uboPools;

			for ( auto entry : m_baseEntries )
			{
				entry.second.hash = hash( entry.second.billboard
					, entry.second.pass
					, instanceMult );
				auto it = m_entries.emplace( entry.second.hash, entry.second ).first;
				it->second.id = m_entries.size();

				if ( instanceMult )
				{
					it->second.modelInstancesUbo = uboPools.getBuffer< ModelInstancesUboConfiguration >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
				}
			}
		}

		iresult.first->second.insert( &renderPass );
	}

	void ObjectCacheT< BillboardList, castor::String >::unregisterPass( SceneRenderPass const * renderPass
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
					auto it = m_entries.find( hash( entry.second.billboard
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

	void ObjectCacheT< BillboardList, castor::String >::registerElement( BillboardBase & billboard )
	{
		getEngine()->sendEvent( makeGpuFunctorEvent( EventType::ePreRender
			, [this, &billboard]( RenderDevice const & device
				, QueueData const & queueData )
			{
				m_connections.emplace( &billboard
					, billboard.onMaterialChanged.connect( [this, &device]( BillboardBase const & billboard
						, MaterialSPtr oldMaterial
						, MaterialSPtr newMaterial )
						{
							if ( oldMaterial )
							{
								for ( auto & pass : *oldMaterial )
								{
									doRemoveEntry( device, billboard, *pass );
								}
							}

							if ( newMaterial )
							{
								for ( auto & pass : *newMaterial )
								{
									doCreateEntry( device, billboard, *pass );
								}
							}
						} ) );

				for ( auto & pass : *billboard.getMaterial() )
				{
					doCreateEntry( device, billboard, *pass );
				}
			} ) );
	}

	void ObjectCacheT< BillboardList, castor::String >::unregisterElement( BillboardBase & billboard )
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

	void ObjectCacheT< BillboardList, castor::String >::clear( RenderDevice const & device )
	{
		auto lock( castor::makeUniqueLock( *this ) );
		ElementObjectCacheT::doClearNoLock();
		auto & uboPools = *device.uboPools;

		for ( auto & entry : m_entries )
		{
			if ( entry.second.modelInstancesUbo )
			{
				uboPools.putBuffer( entry.second.modelInstancesUbo );
			}
		}

		for ( auto & entry : m_entries )
		{
			uboPools.putBuffer( entry.second.modelUbo );
			uboPools.putBuffer( entry.second.billboardUbo );
		}
	}

	void ObjectCacheT< BillboardList, castor::String >::update( CpuUpdater & updater )
	{
		auto lock( castor::makeUniqueLock( *this ) );

		for ( auto & pair : m_baseEntries )
		{
			auto & entry = pair.second;
			auto & modelData = entry.modelUbo.getData();
			modelData.nodeId = entry.id;
			modelData.shadowReceiver = entry.billboard.isShadowReceiver();
			modelData.materialIndex = entry.pass.getId();
			modelData.prvModel = modelData.curModel;
			modelData.curModel = entry.billboard.getNode()->getDerivedTransformationMatrix();
			auto normal = castor::Matrix3x3f{ modelData.curModel };
			modelData.normal = castor::Matrix4x4f{ normal.getInverse().getTransposed() };
			auto & billboardData = entry.billboardUbo.getData();
			billboardData.dimensions = entry.billboard.getDimensions();
		}
	}

	BillboardListCache::PoolsEntry ObjectCacheT< BillboardList, castor::String >::getUbos( BillboardBase const & billboard
		, Pass const & pass
		, uint32_t instanceMult )const
	{
		auto lock( castor::makeUniqueLock( *this ) );
		auto it = m_entries.find( hash( billboard, pass, instanceMult ) );
		CU_Require( it != m_entries.end() );
		return it->second;
	}

	void ObjectCacheT< BillboardList, castor::String >::doCreateEntry( RenderDevice const & device
		, BillboardBase const & billboard
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
			auto & uboPools = *device.uboPools;
			auto & baseEntry = iresult.first->second;
			baseEntry.id = m_baseEntries.size();
			baseEntry.modelUbo = uboPools.getBuffer< ModelUboConfiguration >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
			baseEntry.billboardUbo = uboPools.getBuffer< BillboardUboConfiguration >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

			for ( auto instanceMult : m_instances )
			{
				auto entry = baseEntry;

				if ( instanceMult.first > 1 )
				{
					entry.modelInstancesUbo = uboPools.getBuffer< ModelInstancesUboConfiguration >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
				}

				entry.hash = hash( billboard, pass, instanceMult.first );
				m_entries.emplace( entry.hash, entry );
			}
		}
	}

	void ObjectCacheT< BillboardList, castor::String >::doRemoveEntry( RenderDevice const & device
		, BillboardBase const & billboard
		, Pass const & pass )
	{
		auto & uboPools = *device.uboPools;
		auto baseHash = hash( billboard, pass );

		for ( auto instanceMult : m_instances )
		{
			auto it = m_entries.find( hash( billboard, pass, instanceMult.first ) );

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
			uboPools.putBuffer( entry.billboardUbo );
		}
	}

	//*********************************************************************************************
}
