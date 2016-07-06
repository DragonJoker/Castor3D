#include "GeometryCache.hpp"

#include "Engine.hpp"

#include "Event/Frame/FunctorEvent.hpp"
#include "Scene/Geometry.hpp"

using namespace Castor;

namespace Castor3D
{
	const String ObjectCacheTraits< Geometry, String >::Name = cuT( "Geometry" );

	namespace
	{
		struct GeometryInitialiser
		{
			GeometryInitialiser( uint32_t & p_faceCount, uint32_t & p_vertexCount, Engine & p_engine )
				: m_faceCount{ p_faceCount }
				, m_vertexCount{ p_vertexCount }
				, m_engine{ p_engine }
			{
			}

			inline void operator()( GeometrySPtr p_element )
			{
				m_engine.PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_element, this]()
				{
					p_element->CreateBuffers( m_faceCount, m_vertexCount );
				} ) );
			}

			uint32_t & m_faceCount;
			uint32_t & m_vertexCount;
			Engine & m_engine;
		};
	}

	GeometryCache::ObjectCache( Engine & p_engine
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
		: MyObjectCacheType( p_engine
							 , p_scene
							 , p_rootNode
							 , p_rootCameraNode
							 , p_rootCameraNode
							 , std::move( p_produce )
							 , std::bind( GeometryInitialiser{ m_faceCount, m_vertexCount, p_engine }, std::placeholders::_1 )
							 , std::move( p_clean )
							 , std::move( p_merge )
							 , std::move( p_attach )
							 , std::move( p_detach ) )
	{
	}

	GeometryCache::~ObjectCache()
	{
	}
}
