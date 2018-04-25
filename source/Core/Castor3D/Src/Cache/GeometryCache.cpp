#include "GeometryCache.hpp"

#include "Engine.hpp"

#include "Event/Frame/FrameListener.hpp"
#include "Event/Frame/FunctorEvent.hpp"
#include "Material/Material.hpp"
#include "Material/Pass.hpp"
#include "Mesh/Mesh.hpp"
#include "Mesh/Skeleton/Skeleton.hpp"
#include "Render/RenderInfo.hpp"
#include "Scene/Geometry.hpp"
#include "Scene/Scene.hpp"

#include <Miscellaneous/Hash.hpp>

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
				m_listener.postEvent( makeFunctorEvent( EventType::ePreRender
					, [element, this]()
					{
						element->prepare( m_faceCount, m_vertexCount );
					} ) );
			}

			uint32_t & m_faceCount;
			uint32_t & m_vertexCount;
			FrameListener & m_listener;
		};

		size_t hash( Submesh const & submesh
			, Pass const & pass )
		{
			size_t result = std::hash< Submesh const * >{}( &submesh );
			castor::hashCombine( result, pass );
			return result;
		}
	}

	GeometryCache::ObjectCache( Engine & engine
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
		, m_modelUboPool{ *engine.getRenderSystem() }
		, m_modelMatrixUboPool{ *engine.getRenderSystem() }
	{
	}

	GeometryCache::~ObjectCache()
	{
	}

	void GeometryCache::fillInfo( RenderInfo & info )const
	{
		auto lock = castor::makeUniqueLock( m_elements );

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

	void GeometryCache::update()
	{
		for ( auto & pair : m_entries )
		{
			auto & entry = pair.second;
			auto & modelData = entry.modelUbo.getData();
			modelData.shadowReceiver = entry.geometry.isShadowReceiver();
			modelData.materialIndex = entry.pass.getId() - 1u;
			auto & modelMatrixData = entry.modelMatrixUbo.getData();
			modelMatrixData.model = convert( entry.geometry.getParent()->getDerivedTransformationMatrix() );
		}
	}

	void GeometryCache::uploadUbos()
	{
		m_modelUboPool.upload();
		m_modelMatrixUboPool.upload();
	}

	void GeometryCache::cleanupUbos()
	{
		m_modelUboPool.cleanup();
		m_modelMatrixUboPool.cleanup();
	}

	GeometryCache::PoolsEntry GeometryCache::getUbos( Submesh const & submesh, Pass const & pass )const
	{
		return m_entries.at( hash( submesh, pass ) );
	}

	void GeometryCache::clear()
	{
		MyObjectCache::clear();

		for ( auto & entry : m_entries )
		{
			m_modelUboPool.putBuffer( entry.second.modelUbo );
			m_modelMatrixUboPool.putBuffer( entry.second.modelMatrixUbo );
		}

		m_entries.clear();
	}

	void GeometryCache::add( ElementPtr element )
	{
		MyObjectCache::add( element->getName(), element );
		doRegister( *element );
	}

	GeometrySPtr GeometryCache::add( Key const & name
		, SceneNodeSPtr parent
		, MeshSPtr mesh )
	{
		REQUIRE( mesh );
		auto result = MyObjectCache::add( name, parent, mesh );
		doRegister( *result );
		return result;
	}

	void GeometryCache::remove( Key const & name )
	{
		auto lock = castor::makeUniqueLock( m_elements );

		if ( m_elements.has( name ) )
		{
			auto element = m_elements.find( name );
			m_detach( element );
			m_elements.erase( name );
			onChanged();
			doUnregister( *element );
		}
	}

	GeometryCache::PoolsEntry GeometryCache::doCreateEntry( Geometry const & geometry
		, Submesh const & submesh
		, Pass const & pass )
	{
		return
		{
			geometry,
			submesh,
			pass,
			m_modelUboPool.getBuffer( renderer::MemoryPropertyFlag::eHostVisible ),
			m_modelMatrixUboPool.getBuffer( renderer::MemoryPropertyFlag::eHostVisible ),
		};
	}

	void GeometryCache::doRemoveEntry( Submesh const & submesh
		, Pass const & pass )
	{
		auto entry = getUbos( submesh, pass );
		m_entries.erase( hash( submesh, pass ) );
		m_modelUboPool.putBuffer( entry.modelUbo );
		m_modelMatrixUboPool.putBuffer( entry.modelMatrixUbo );
	}

	void GeometryCache::doRegister( Geometry & geometry )
	{
		m_connections.emplace( &geometry, geometry.onMaterialChanged.connect( [this]( Geometry const & geometry
			, Submesh const & submesh
			, MaterialSPtr oldMaterial
			, MaterialSPtr newMaterial )
		{
			if ( oldMaterial )
			{
				for ( auto & pass : *oldMaterial )
				{
					doRemoveEntry( submesh, *pass );
				}
			}

			if ( newMaterial )
			{
				for ( auto & pass : *newMaterial )
				{
					m_entries.emplace( hash( submesh, *pass )
						, doCreateEntry( geometry, submesh, *pass ) );
				}
			}
		} ) );

		for ( auto & submesh : *geometry.getMesh() )
		{
			for ( auto & pass : *geometry.getMaterial( *submesh ) )
			{
				m_entries.emplace( hash( *submesh, *pass )
					, doCreateEntry( geometry, *submesh, *pass ) );
			}
		}
	}

	void GeometryCache::doUnregister( Geometry & geometry )
	{
		m_connections.erase( &geometry );

		for ( auto & submesh : *geometry.getMesh() )
		{
			for ( auto & pass : *geometry.getMaterial( *submesh ) )
			{
				doRemoveEntry( *submesh, *pass );
			}
		}
	}
}
