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

	RenderTargetSPtr RenderTargetCache::Add( TargetType p_type )
	{
		auto l_lock = make_unique_lock( *this );
		RenderTargetSPtr l_return = std::make_shared< RenderTarget >( *GetEngine(), p_type );
		m_renderTargets[size_t( p_type )].push_back( l_return );
		return l_return;
	}

	void RenderTargetCache::AddShadowProducer( Light & p_light )
	{
		auto l_lock = make_unique_lock( *this );

		for ( auto l_target : m_renderTargets[size_t( TargetType::eWindow )] )
		{
			if ( l_target->GetScene()->GetName() == p_light.GetScene()->GetName() )
			{
				l_target->AddShadowProducer( p_light );
			}
		}
	}

	void RenderTargetCache::Remove( RenderTargetSPtr p_target )
	{
		auto l_lock = make_unique_lock( *this );
		auto l_v = m_renderTargets.begin() + size_t( p_target->GetTargetType() );
		auto l_it = std::find( l_v->begin(), l_v->end(), p_target );

		if ( l_it != l_v->end() )
		{
			l_v->erase( l_it );
		}
	}

	void RenderTargetCache::Render( RenderInfo & p_info )
	{
		auto l_lock = make_unique_lock( *this );

		for ( auto l_target : m_renderTargets[size_t( TargetType::eTexture )] )
		{
			l_target->Render( p_info );
		}

		for ( auto l_target : m_renderTargets[size_t( TargetType::eWindow )] )
		{
			l_target->Render( p_info );
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
