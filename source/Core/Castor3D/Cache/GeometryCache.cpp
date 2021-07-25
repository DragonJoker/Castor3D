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

using namespace castor;

namespace castor3d
{
	template<> const String ObjectCacheTraits< Geometry, String >::Name = cuT( "Geometry" );

	namespace
	{
		struct GeometryInitialiser
		{
			GeometryInitialiser( uint32_t & faceCount
				, uint32_t & vertexCount
				, FrameListener & listener )
				: m_faceCount{ faceCount }
				, m_vertexCount{ vertexCount }
				, m_listener{ listener }
			{
			}

			inline void operator()( GeometrySPtr element )
			{
				m_listener.postEvent( makeGpuFunctorEvent( EventType::ePreRender
					, [element, this]( RenderDevice const & device )
					{
						element->prepare( m_faceCount, m_vertexCount );
					} ) );
			}

			uint32_t & m_faceCount;
			uint32_t & m_vertexCount;
			FrameListener & m_listener;
		};

		castor::String printhex( uint64_t v )
		{
			auto stream = castor::makeStringStream();
			stream << "0x" << std::hex << std::setw( 16u ) << std::setfill( '0' ) << v;
			return stream.str();
		}
	}

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

	GeometryCache::GeometryCache( Engine & engine
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
			, std::bind( GeometryInitialiser{ m_faceCount, m_vertexCount, scene.getListener() }, std::placeholders::_1 )
			, std::move( clean )
			, std::move( merge )
			, std::move( attach )
			, std::move( detach ) )
	{
	}

	GeometryCache::~GeometryCache()
	{
	}

	void GeometryCache::registerPass( SceneRenderPass const & renderPass )
	{
		auto instanceMult = renderPass.getInstanceMult();
		auto iresult = m_instances.emplace( instanceMult, RenderPassSet{} );

		if ( iresult.second )
		{
			auto & device = *getScene()->getEngine()->getRenderSystem()->getMainRenderDevice();
			auto & uboPools = *device.uboPools;

			for ( auto entry : m_baseEntries )
			{
				entry.second.hash = hash( entry.second.geometry
					, entry.second.submesh
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

	void GeometryCache::unregisterPass( SceneRenderPass const * renderPass
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
				auto & device = *getScene()->getEngine()->getRenderSystem()->getMainRenderDevice();
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

	void GeometryCache::fillInfo( RenderInfo & info )const
	{
		auto lock( castor::makeUniqueLock( m_elements ) );

		for ( auto element : m_elements )
		{
			if ( element.second->getMesh() )
			{
				auto mesh = element.second->getMesh();
				info.m_totalObjectsCount += mesh->getSubmeshCount();
				info.m_totalVertexCount += mesh->getVertexCount();
				info.m_totalFaceCount += mesh->getFaceCount();
			}
		}
	}

	void GeometryCache::update( CpuUpdater & updater )
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

	GeometryCache::PoolsEntry GeometryCache::getUbos( Geometry const & geometry
		, Submesh const & submesh
		, Pass const & pass
		, uint32_t instanceMult )const
	{
		auto it = m_entries.find( hash( geometry, submesh, pass, instanceMult ) );
		CU_Require( it != m_entries.end() );
		return it->second;
	}

	void GeometryCache::clear( RenderDevice const & device )
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

		for ( auto & entry : m_baseEntries )
		{
			uboPools.putBuffer( entry.second.modelUbo );
		}

		m_entries.clear();
		m_baseEntries.clear();
		m_instances.clear();
	}

	void GeometryCache::add( ElementPtr element )
	{
		MyObjectCache::add( element->getName(), element );
		doRegister( *element );
	}

	GeometrySPtr GeometryCache::add( Key const & name
		, SceneNode & parent
		, MeshSPtr mesh )
	{
		CU_Require( mesh );
		auto result = MyObjectCache::add( name, parent, mesh );
		doRegister( *result );
		return result;
	}

	void GeometryCache::remove( Key const & name )
	{
		auto lock( castor::makeUniqueLock( m_elements ) );

		if ( m_elements.has( name ) )
		{
			auto element = m_elements.find( name );
			m_detach( element );
			m_elements.erase( name );
			onChanged();
			doUnregister( *element );
		}
	}

	void GeometryCache::doCreateEntry( RenderDevice const & device
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
			baseEntry.id = m_baseEntries.size();
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

	void GeometryCache::doRemoveEntry( RenderDevice const & device
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

	void GeometryCache::doRegister( Geometry & geometry )
	{
		auto & device = *getScene()->getEngine()->getRenderSystem()->getMainRenderDevice();
		m_connections.emplace( &geometry
			, geometry.onMaterialChanged.connect( [this, &device]( Geometry const & geometry
				, Submesh const & submesh
				, MaterialSPtr oldMaterial
				, MaterialSPtr newMaterial )
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

		if ( geometry.getMesh() )
		{
			for ( auto & submesh : *geometry.getMesh() )
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

	void GeometryCache::doUnregister( Geometry & geometry )
	{
		m_connections.erase( &geometry );

		if ( geometry.getMesh() )
		{
			for ( auto & submesh : *geometry.getMesh() )
			{
				for ( auto & pass : *geometry.getMaterial( *submesh ) )
				{
					doRemoveEntry( *getScene()->getEngine()->getRenderSystem()->getMainRenderDevice()
						, geometry
						, *submesh
						, *pass );
				}
			}
		}
	}
}
