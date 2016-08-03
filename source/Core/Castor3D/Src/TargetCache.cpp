#include "TargetCache.hpp"

#include "Engine.hpp"

#include "GeometryCache.hpp"
#include "Render/RenderTarget.hpp"
#include "Scene/Scene.hpp"

using namespace Castor;

namespace Castor3D
{
	RenderTargetCache::RenderTargetCache( Engine & p_engine )
		: OwnedBy< Engine >{ p_engine }
	{
	}

	RenderTargetCache::~RenderTargetCache()
	{
	}

	RenderTargetSPtr RenderTargetCache::Add( eTARGET_TYPE p_type )
	{
		auto l_lock = make_unique_lock( *this );
		RenderTargetSPtr l_return = std::make_shared< RenderTarget >( *GetEngine(), p_type );
		m_renderTargets[p_type].push_back( l_return );
		return l_return;
	}

	void RenderTargetCache::Remove( RenderTargetSPtr p_target )
	{
		auto l_lock = make_unique_lock( *this );
		auto l_v = m_renderTargets.begin() + p_target->GetTargetType();
		auto l_it = std::find( l_v->begin(), l_v->end(), p_target );

		if ( l_it != l_v->end() )
		{
			l_v->erase( l_it );
		}
	}

	void RenderTargetCache::Render( uint32_t & p_time, uint32_t & p_vtxCount, uint32_t & p_fceCount, uint32_t & p_objCount, uint32_t & p_visible )
	{
		auto l_lock = make_unique_lock( *this );

		for ( auto l_target : m_renderTargets[eTARGET_TYPE_TEXTURE] )
		{
			auto l_scene = l_target->GetScene();

			if ( l_scene )
			{
				p_objCount += l_scene->GetGeometryCache().GetObjectCount();
				p_fceCount += l_scene->GetGeometryCache().GetFaceCount();
				p_vtxCount += l_scene->GetGeometryCache().GetVertexCount();
				l_target->Render( p_time );
				p_visible += l_target->GetVisbleObjectsCount();
			}
		}

		for ( auto l_target : m_renderTargets[eTARGET_TYPE_WINDOW] )
		{
			auto l_scene = l_target->GetScene();

			if ( l_scene )
			{
				p_objCount += l_scene->GetGeometryCache().GetObjectCount();
				p_fceCount += l_scene->GetGeometryCache().GetFaceCount();
				p_vtxCount += l_scene->GetGeometryCache().GetVertexCount();
				l_target->Render( p_time );
				p_visible += l_target->GetVisbleObjectsCount();
			}
		}
	}

	void RenderTargetCache::Clear()
	{
		for ( auto & l_array : m_renderTargets )
		{
			l_array.clear();
		}
	}
}
