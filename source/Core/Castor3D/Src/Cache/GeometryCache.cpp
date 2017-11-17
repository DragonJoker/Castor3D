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
			GeometryInitialiser( uint32_t & p_faceCount
				, uint32_t & p_vertexCount
				, FrameListener & p_listener )
				: m_faceCount{ p_faceCount }
				, m_vertexCount{ p_vertexCount }
				, m_listener{ p_listener }
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
		, Scene & p_scene
		, SceneNodeSPtr p_rootNode
		, SceneNodeSPtr p_rootCameraNode
		, SceneNodeSPtr p_rootObjectNode
		, Producer && p_produce
		, Initialiser && p_initialise
		, Cleaner && p_clean
		, Merger && p_merge
		, Attacher && p_attach
		, Detacher && p_detach )
		: MyObjectCache( engine
			, p_scene
			, p_rootNode
			, p_rootCameraNode
			, p_rootCameraNode
			, std::move( p_produce )
			, std::bind( GeometryInitialiser{ m_faceCount, m_vertexCount, p_scene.getListener() }, std::placeholders::_1 )
			, std::move( p_clean )
			, std::move( p_merge )
			, std::move( p_attach )
			, std::move( p_detach ) )
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
