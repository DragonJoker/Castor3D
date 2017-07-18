#include "GeometryCache.hpp"

#include "Engine.hpp"

#include "Event/Frame/FrameListener.hpp"
#include "Event/Frame/FunctorEvent.hpp"
#include "Scene/Geometry.hpp"
#include "Scene/Scene.hpp"
#include "Mesh/Mesh.hpp"

using namespace Castor;

namespace Castor3D
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

			inline void operator()( GeometrySPtr p_element )
			{
				m_listener.PostEvent( MakeFunctorEvent( EventType::ePreRender, [p_element, this]()
				{
					p_element->CreateBuffers( m_faceCount, m_vertexCount );
				} ) );
			}

			uint32_t & m_faceCount;
			uint32_t & m_vertexCount;
			FrameListener & m_listener;
		};
	}

	ObjectCache< Geometry, Castor::String >::ObjectCache( Engine & p_engine
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
		: MyObjectCache( p_engine
			, p_scene
			, p_rootNode
			, p_rootCameraNode
			, p_rootCameraNode
			, std::move( p_produce )
			, std::bind( GeometryInitialiser{ m_faceCount, m_vertexCount, p_scene.GetListener() }, std::placeholders::_1 )
			, std::move( p_clean )
			, std::move( p_merge )
			, std::move( p_attach )
			, std::move( p_detach ) )
	{
	}

	ObjectCache< Geometry, Castor::String >::~ObjectCache()
	{
	}

	void ObjectCache< Geometry, Castor::String >::FillInfo( RenderInfo & p_info )const
	{
		p_info.m_totalVertexCount += m_vertexCount;
		p_info.m_totalFaceCount += m_faceCount;
		auto lock = Castor::make_unique_lock( m_elements );
		p_info.m_totalObjectsCount += std::accumulate( m_elements.begin()
			, m_elements.end()
			, 0u
			, []( uint32_t p_value, std::pair< String, GeometrySPtr > const & p_pair )
			{
				return p_value + p_pair.second->GetMesh()->GetSubmeshCount();
			} );
	}
}
