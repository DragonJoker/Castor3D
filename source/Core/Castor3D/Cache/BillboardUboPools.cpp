#include "Castor3D/Cache/BillboardUboPools.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/RenderPass.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/SceneNode.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

using namespace castor;

namespace castor3d
{
	size_t hash( BillboardBase const & billboard
		, Pass const & pass )
	{
		size_t result = std::hash< BillboardBase const * >{}( &billboard );
		castor::hashCombine( result, pass );
		return result;
	}

	size_t hash( BillboardBase const & billboard
		, Pass const & pass
		, uint32_t instanceMult )
	{
		size_t result = std::hash< uint32_t >{}( instanceMult );
		castor::hashCombine( result, billboard );
		castor::hashCombine( result, pass );
		return result;
	}

	BillboardUboPools::BillboardUboPools( RenderDevice const & device )
		: m_device{ device }
	{
	}

	void BillboardUboPools::registerPass( RenderPass const & renderPass )
	{
		auto instanceMult = renderPass.getInstanceMult();
		auto iresult = m_instances.emplace( instanceMult, RenderPassSet{} );

		if ( iresult.second )
		{
			auto & uboPools = *m_device.uboPools;

			for ( auto entry : m_baseEntries )
			{
				entry.second.hash = hash( entry.second.billboard, entry.second.pass, instanceMult );
				auto it = m_entries.emplace( entry.second.hash, entry.second ).first;

				if ( instanceMult )
				{
					it->second.modelInstancesUbo = uboPools.getBuffer< ModelInstancesUboConfiguration >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
				}
			}
		}

		iresult.first->second.insert( &renderPass );
	}

	void BillboardUboPools::unregisterPass( RenderPass const * renderPass
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
				auto & uboPools = *m_device.uboPools;

				for ( auto & entry : m_baseEntries )
				{
					auto it = m_entries.find( hash( entry.second.billboard, entry.second.pass, instanceMult ) );

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
			}
		}
	}

	void BillboardUboPools::update()
	{
		for ( auto & pair : m_baseEntries )
		{
			auto & entry = pair.second;
			auto & modelData = entry.modelUbo.getData();
			modelData.shadowReceiver = entry.billboard.isShadowReceiver();
			modelData.materialIndex = entry.pass.getId();
			auto & modelMatrixData = entry.modelMatrixUbo.getData();
			modelMatrixData.prvModel = modelMatrixData.curModel;
			modelMatrixData.curModel = entry.billboard.getNode()->getDerivedTransformationMatrix();
			auto & billboardData = entry.billboardUbo.getData();
			billboardData.dimensions = entry.billboard.getDimensions();
			auto & texturesData = entry.texturesUbo.getData();
			uint32_t index = 0u;

			for ( auto & unit : entry.pass )
			{
				texturesData.indices[index / 4u][index % 4] = unit->getId();
				++index;
			}
		}
	}

	void BillboardUboPools::registerElement( BillboardBase & billboard )
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
					doCreateEntry( billboard, *pass );
				}
			}
		} ) );

		for ( auto & pass : *billboard.getMaterial() )
		{
			doCreateEntry( billboard, *pass );
		}
	}

	void BillboardUboPools::unregisterElement( BillboardBase & billboard )
	{
		auto it = m_connections.find( &billboard );

		if ( it != m_connections.end() )
		{
			m_connections.erase( it );

			for ( auto & pass : *billboard.getMaterial() )
			{
				doRemoveEntry( billboard, *pass );
			}
		}
	}

	BillboardUboPools::PoolsEntry BillboardUboPools::getUbos( BillboardBase const & billboard
		, Pass const & pass
		, uint32_t instanceMult )const
	{
		return m_entries.at( hash( billboard, pass, instanceMult ) );
	}

	void BillboardUboPools::clear( RenderDevice const & device )
	{
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
			uboPools.putBuffer( entry.second.modelMatrixUbo );
			uboPools.putBuffer( entry.second.billboardUbo );
			uboPools.putBuffer( entry.second.pickingUbo );
			uboPools.putBuffer( entry.second.texturesUbo );
		}
	}
	
	void BillboardUboPools::doCreateEntry( BillboardBase const & billboard
		, Pass const & pass )
	{
		auto baseHash = hash( billboard, pass );
		auto iresult = m_baseEntries.emplace( baseHash
			, BillboardUboPools::PoolsEntry{ baseHash
				, billboard
				, pass } );

		if ( iresult.second )
		{
			auto & uboPools = *m_device.uboPools;
			auto & baseEntry = iresult.first->second;
			baseEntry.modelUbo = uboPools.getBuffer< ModelUboConfiguration >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
			baseEntry.modelMatrixUbo = uboPools.getBuffer< ModelMatrixUboConfiguration >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
			baseEntry.billboardUbo = uboPools.getBuffer< BillboardUboConfiguration >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
			baseEntry.pickingUbo = uboPools.getBuffer< PickingUboConfiguration >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
			baseEntry.texturesUbo = uboPools.getBuffer< TexturesUboConfiguration >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

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

	void BillboardUboPools::doRemoveEntry( BillboardBase const & billboard
		, Pass const & pass )
	{
		auto & uboPools = *m_device.uboPools;
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
			uboPools.putBuffer( entry.modelMatrixUbo );
			uboPools.putBuffer( entry.billboardUbo );
			uboPools.putBuffer( entry.pickingUbo );
			uboPools.putBuffer( entry.texturesUbo );
		}
	}
}
