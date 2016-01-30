#include "TargetManager.hpp"

#include "GeometryManager.hpp"
#include "RenderSystem.hpp"
#include "Scene.hpp"

using namespace Castor;

namespace Castor3D
{
	TargetManager::TargetManager( Engine & p_engine )
		: ResourceManager< String, RenderTarget >( p_engine )
	{
	}

	TargetManager::~TargetManager()
	{
	}

	RenderTargetSPtr TargetManager::Create( eTARGET_TYPE p_type )
	{
		std::unique_lock< Collection > l_lock( m_elements );
		RenderTargetSPtr l_return = std::make_shared< RenderTarget >( *GetEngine(), p_type );
		m_renderTargets[p_type].push_back( l_return );
		return l_return;
	}

	void TargetManager::Remove( RenderTargetSPtr && p_pRenderTarget )
	{
		std::unique_lock< Collection > l_lock( m_elements );
		eTARGET_TYPE l_type = p_pRenderTarget->GetTargetType();
		auto l_v = ( m_renderTargets.begin() + l_type );
		auto l_it = l_v->begin();

		while ( l_it != l_v->end() )
		{
			if ( *l_it == p_pRenderTarget )
			{
				l_v->erase( l_it );
				l_it = l_v->end();
			}
			else
			{
				++l_it;
			}
		}
	}

	void TargetManager::Render( double & p_time, uint32_t & p_vtxCount, uint32_t & p_fceCount, uint32_t & p_objCount )
	{
		std::unique_lock< Collection > l_lock( m_elements );

		for ( auto l_target : m_renderTargets[eTARGET_TYPE_TEXTURE] )
		{
			p_objCount += l_target->GetScene()->GetGeometryManager().GetObjectCount();
			p_fceCount += l_target->GetScene()->GetGeometryManager().GetFaceCount();
			p_vtxCount += l_target->GetScene()->GetGeometryManager().GetVertexCount();
			l_target->Render( p_time );
		}

		for ( auto l_target : m_renderTargets[eTARGET_TYPE_WINDOW] )
		{
			p_objCount += l_target->GetScene()->GetGeometryManager().GetObjectCount();
			p_fceCount += l_target->GetScene()->GetGeometryManager().GetFaceCount();
			p_vtxCount += l_target->GetScene()->GetGeometryManager().GetVertexCount();
			l_target->Render( p_time );
		}
	}
}
