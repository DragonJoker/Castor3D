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

	template<> const castor::String ObjectCacheTraits< BillboardList, castor::String >::Name = cuT( "BillboardList" );

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
	{
	}

	BillboardListCache::~ObjectCache()
	{
	}

	void BillboardListCache::registerPass( SceneRenderPass const & renderPass )
	{
		auto instanceMult = renderPass.getInstanceMult();
		auto iresult = m_instances.emplace( instanceMult, RenderPassSet{} );

		if ( iresult.second )
		{
			m_engine.sendEvent( makeGpuFunctorEvent( EventType::ePreRender
				, [this, instanceMult]( RenderDevice const & device )
				{
					auto & uboPools = *device.uboPools;

					for ( auto entry : m_baseEntries )
					{
						entry.second.hash = hash( entry.second.billboard
							, entry.second.pass
							, instanceMult );
						auto it = m_entries.emplace( entry.second.hash, entry.second ).first;

						if ( instanceMult )
						{
							it->second.modelInstancesUbo = uboPools.getBuffer< ModelInstancesUboConfiguration >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
						}
					}
				} ) );
		}

		iresult.first->second.insert( &renderPass );
	}

	void BillboardListCache::unregisterPass( SceneRenderPass const * renderPass
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
				m_engine.sendEvent( makeGpuFunctorEvent( EventType::ePreRender
					, [this, instanceMult]( RenderDevice const & device )
					{
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
					} ) );
			}
		}
	}

	void BillboardListCache::registerElement( BillboardBase & billboard )
	{
		getEngine()->sendEvent( makeGpuFunctorEvent( EventType::ePreRender
			, [this, &billboard]( RenderDevice const & device )
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

	void BillboardListCache::unregisterElement( BillboardBase & billboard )
	{
		getEngine()->sendEvent( makeGpuFunctorEvent( EventType::ePreRender
			, [this, &billboard]( RenderDevice const & device )
			{
				m_connections.erase( &billboard );

				for ( auto & pass : *billboard.getMaterial() )
				{
					doRemoveEntry( device, billboard, *pass );
				}
			} ) );
	}

	void BillboardListCache::clear( RenderDevice const & device )
	{
		MyObjectCache::clear();
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

	void BillboardListCache::update( CpuUpdater & updater )
	{
		for ( auto & pair : m_baseEntries )
		{
			auto & entry = pair.second;
			auto & modelData = entry.modelUbo.getData();
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

	BillboardListCache::PoolsEntry BillboardListCache::getUbos( BillboardBase const & billboard
		, Pass const & pass
		, uint32_t instanceMult )const
	{
		return m_entries.at( hash( billboard, pass, instanceMult ) );
	}

	BillboardListSPtr BillboardListCache::add( Key const & name
		, SceneNode & parent )
	{
		auto result = MyObjectCache::add( name, parent );
		registerElement( *result );
		return result;
	}

	void BillboardListCache::add( ElementPtr element )
	{
		m_initialise( element );
		MyObjectCache::add( element->getName(), element );
		registerElement( *element );
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
			unregisterElement( *element );
		}
	}

	void BillboardListCache::doCreateEntry( RenderDevice const & device
		, BillboardBase const & billboard
		, Pass const & pass )
	{
		auto baseHash = hash( billboard, pass );
		auto iresult = m_baseEntries.emplace( baseHash
			, BillboardListCache::PoolsEntry{ baseHash
			, billboard
			, pass } );

		if ( iresult.second )
		{
			auto & uboPools = *device.uboPools;
			auto & baseEntry = iresult.first->second;
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

	void BillboardListCache::doRemoveEntry( RenderDevice const & device
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
}
