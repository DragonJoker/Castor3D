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
#include "Castor3D/Shader/ShaderBuffers/ModelDataBuffer.hpp"

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
		, m_modelDataBuffer{ castor::makeUnique< ModelDataBuffer >( *m_device.renderSystem.getEngine()
			, m_device
			, ( m_device.hasBindless()
				? m_device.getMaxBindlessSampled()
				: shader::MaxModelDataCount ) ) }
	{
	}

	void ObjectCacheT< BillboardList, castor::String, BillboardCacheTraits >::initialise( RenderDevice const & device )
	{
		if ( !m_modelDataBuffer )
		{
			m_modelDataBuffer = castor::makeUnique< ModelDataBuffer >( *device.renderSystem.getEngine()
				, device
				, ( device.hasBindless()
					? device.getMaxBindlessSampled()
					: shader::MaxModelDataCount ) );
		}
	}

	void ObjectCacheT< BillboardList, castor::String, BillboardCacheTraits >::cleanup()
	{
		ObjectCacheBaseT< BillboardList, castor::String, BillboardCacheTraits >::cleanup();
		m_modelDataBuffer.reset();
	}

	void ObjectCacheT< BillboardList, castor::String, BillboardCacheTraits >::registerPass( RenderNodesPass const & renderPass )
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
				it->second.id = int32_t( m_entries.size() );

				if ( instanceMult )
				{
					it->second.modelInstancesUbo = uboPools.getBuffer< ModelInstancesUboConfiguration >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
				}
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
				auto & device = getScene()->getEngine()->getRenderSystem()->getRenderDevice();
				auto & uboPools = *device.uboPools;

				for ( auto & entry : m_baseEntries )
				{
					auto it = m_entries.find( hash( entry.second.billboard
						, entry.second.pass
						, instanceMult ) );

					if ( it != m_entries.end() )
					{
						auto entrySave = it->second;
						m_entries.erase( it );

						if ( entrySave.modelInstancesUbo )
						{
							uboPools.putBuffer( entrySave.modelInstancesUbo );
						}
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
					, billboard.onMaterialChanged.connect( [this, &device]( BillboardBase const & pbillboard
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
			uboPools.putBuffer( entry.second.modelIndexUbo );
			m_modelDataBuffer->putBuffer( *entry.second.modelDataUbo );
			uboPools.putBuffer( entry.second.billboardUbo );
		}

		m_modelDataBuffer.reset();
	}

	void ObjectCacheT< BillboardList, castor::String, BillboardCacheTraits >::update( CpuUpdater & updater )
	{
		auto lock( castor::makeUniqueLock( *this ) );

		for ( auto & pair : m_baseEntries )
		{
			auto & entry = pair.second;
			auto & modelData = entry.modelDataUbo->getData();
			modelData.shadowEnvMapIndex->x = entry.billboard.isShadowReceiver();
			modelData.prvModel = modelData.curModel;
			modelData.curModel = entry.billboard.getNode()->getDerivedTransformationMatrix();
			auto normal = castor::Matrix3x3f{ modelData.curModel };
			modelData.normal = castor::Matrix4x4f{ normal.getInverse().getTransposed() };

			auto & modelIndexData = entry.modelIndexUbo.getData();
			modelIndexData.nodeId = entry.id;
			modelIndexData.materialId = int( entry.pass.getId() );
			uint32_t index = 0u;

			for ( auto & unit : entry.pass )
			{
				if ( index < 4 )
				{
					modelIndexData.textures0[index] = unit->getId();
				}
				else if ( index < 8 )
				{
					modelIndexData.textures1[index - 4] = unit->getId();
				}

				++index;
			}

			while ( index < 8u )
			{
				if ( index < 4 )
				{
					modelIndexData.textures0[index] = 0u;
				}
				else
				{
					modelIndexData.textures1[index - 4] = 0u;
				}

				++index;
			}

			modelIndexData.textures = int32_t( std::min( 8u, entry.pass.getTextureUnitsCount() ) );
			auto & billboardData = entry.billboardUbo.getData();
			billboardData.dimensions = entry.billboard.getDimensions();
		}
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
			baseEntry.id = int32_t( m_baseEntries.size() );
			baseEntry.modelIndexUbo = uboPools.getBuffer< ModelIndexUboConfiguration >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
			baseEntry.billboardUbo = uboPools.getBuffer< BillboardUboConfiguration >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
			baseEntry.modelDataUbo = &m_modelDataBuffer->getBuffer();

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

	void ObjectCacheT< BillboardList, castor::String, BillboardCacheTraits >::doRemoveEntry( RenderDevice const & device
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
			uboPools.putBuffer( entry.modelIndexUbo );
			m_modelDataBuffer->putBuffer( *entry.modelDataUbo );
			uboPools.putBuffer( entry.billboardUbo );
		}
	}

	//*********************************************************************************************
}
