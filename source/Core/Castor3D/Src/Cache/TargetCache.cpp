#include "TargetCache.hpp"

#include "Engine.hpp"

#include "GeometryCache.hpp"
#include "Render/RenderTarget.hpp"
#include "Scene/Scene.hpp"

using namespace Castor;

namespace Castor3D
{
	RenderTargetCache::RenderTargetCache( Engine & engine )
		: OwnedBy< Engine >{ engine }
	{
	}

	RenderTargetCache::~RenderTargetCache()
	{
	}

	RenderTargetSPtr RenderTargetCache::Add( TargetType p_type )
	{
		auto lock = make_unique_lock( *this );
		RenderTargetSPtr result = std::make_shared< RenderTarget >( *GetEngine(), p_type );
		m_renderTargets[size_t( p_type )].push_back( result );
		return result;
	}

	void RenderTargetCache::AddShadowProducer( Light & p_light )
	{
		auto lock = make_unique_lock( *this );

		for ( auto target : m_renderTargets[size_t( TargetType::eWindow )] )
		{
			if ( target->GetScene()->GetName() == p_light.GetScene()->GetName() )
			{
				target->AddShadowProducer( p_light );
			}
		}
	}

	void RenderTargetCache::Remove( RenderTargetSPtr p_target )
	{
		auto lock = make_unique_lock( *this );
		auto v = m_renderTargets.begin() + size_t( p_target->GetTargetType() );
		auto it = std::find( v->begin(), v->end(), p_target );

		if ( it != v->end() )
		{
			v->erase( it );
		}
	}

	void RenderTargetCache::Render( RenderInfo & p_info )
	{
		auto lock = make_unique_lock( *this );

		for ( auto target : m_renderTargets[size_t( TargetType::eTexture )] )
		{
			target->Render( p_info );
		}

		for ( auto target : m_renderTargets[size_t( TargetType::eWindow )] )
		{
			target->Render( p_info );
		}
	}

	void RenderTargetCache::Clear()
	{
		for ( auto & array : m_renderTargets )
		{
			array.clear();
		}
	}
}
