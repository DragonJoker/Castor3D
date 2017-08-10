#include "TargetCache.hpp"

#include "Engine.hpp"

#include "GeometryCache.hpp"
#include "Render/RenderTarget.hpp"
#include "Scene/Scene.hpp"

using namespace castor;

namespace castor3d
{
	RenderTargetCache::RenderTargetCache( Engine & engine )
		: OwnedBy< Engine >{ engine }
	{
	}

	RenderTargetCache::~RenderTargetCache()
	{
	}

	RenderTargetSPtr RenderTargetCache::add( TargetType p_type )
	{
		auto lock = makeUniqueLock( *this );
		RenderTargetSPtr result = std::make_shared< RenderTarget >( *getEngine(), p_type );
		m_renderTargets[size_t( p_type )].push_back( result );
		return result;
	}

	void RenderTargetCache::addShadowProducer( Light & p_light )
	{
		auto lock = makeUniqueLock( *this );

		for ( auto target : m_renderTargets[size_t( TargetType::eWindow )] )
		{
			if ( target->getScene()->getName() == p_light.getScene()->getName() )
			{
				target->addShadowProducer( p_light );
			}
		}
	}

	void RenderTargetCache::remove( RenderTargetSPtr p_target )
	{
		auto lock = makeUniqueLock( *this );
		auto v = m_renderTargets.begin() + size_t( p_target->getTargetType() );
		auto it = std::find( v->begin(), v->end(), p_target );

		if ( it != v->end() )
		{
			v->erase( it );
		}
	}

	void RenderTargetCache::render( RenderInfo & p_info )
	{
		auto lock = makeUniqueLock( *this );

		for ( auto target : m_renderTargets[size_t( TargetType::eTexture )] )
		{
			target->render( p_info );
		}

		for ( auto target : m_renderTargets[size_t( TargetType::eWindow )] )
		{
			target->render( p_info );
		}
	}

	void RenderTargetCache::clear()
	{
		for ( auto & array : m_renderTargets )
		{
			array.clear();
		}
	}
}
