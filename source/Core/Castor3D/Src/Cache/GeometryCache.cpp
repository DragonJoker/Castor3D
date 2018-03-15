#include "GeometryCache.hpp"

#include "Engine.hpp"

#include "Event/Frame/FrameListener.hpp"
#include "Event/Frame/FunctorEvent.hpp"
#include "Scene/Geometry.hpp"
#include "Scene/Scene.hpp"
#include "Mesh/Mesh.hpp"

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
	}

	ObjectCache< Geometry, castor::String >::ObjectCache( Engine & engine
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

	ObjectCache< Geometry, castor::String >::~ObjectCache()
	{
	}

	void ObjectCache< Geometry, castor::String >::fillInfo( RenderInfo & info )const
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
}
