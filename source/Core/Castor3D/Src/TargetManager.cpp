#include "TargetManager.hpp"

#include "GeometryManager.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Scene.hpp"

using namespace Castor;

namespace Castor3D
{
	const String ManagedObjectNamer< RenderTarget >::Name = cuT( "RenderTarget" );

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

	void TargetManager::Remove( RenderTargetSPtr p_target )
	{
		auto l_lock = make_unique_lock( m_elements );
		auto l_v = m_renderTargets.begin() + p_target->GetTargetType();
		auto l_it = std::find( l_v->begin(), l_v->end(), p_target );

		if ( l_it != l_v->end() )
		{
			l_v->erase( l_it );
		}
	}

	void TargetManager::Render( uint32_t & p_time, uint32_t & p_vtxCount, uint32_t & p_fceCount, uint32_t & p_objCount )
	{
		std::unique_lock< Collection > l_lock( m_elements );

		for ( auto l_target : m_renderTargets[eTARGET_TYPE_TEXTURE] )
		{
			auto l_scene = l_target->GetScene();

			if ( l_scene )
			{
				p_objCount += l_scene->GetGeometryManager().GetObjectCount();
				p_fceCount += l_scene->GetGeometryManager().GetFaceCount();
				p_vtxCount += l_scene->GetGeometryManager().GetVertexCount();
				l_target->Render( p_time );
			}
		}

		for ( auto l_target : m_renderTargets[eTARGET_TYPE_WINDOW] )
		{
			auto l_scene = l_target->GetScene();

			if ( l_scene )
			{
				p_objCount += l_scene->GetGeometryManager().GetObjectCount();
				p_fceCount += l_scene->GetGeometryManager().GetFaceCount();
				p_vtxCount += l_scene->GetGeometryManager().GetVertexCount();
				l_target->Render( p_time );
			}
		}
	}
}
