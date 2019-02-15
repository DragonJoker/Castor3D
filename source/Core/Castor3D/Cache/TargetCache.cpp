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

	RenderTargetSPtr RenderTargetCache::add( TargetType type )
	{
		auto lock = makeUniqueLock( *this );
		RenderTargetSPtr result = std::make_shared< RenderTarget >( *getEngine(), type );
		m_renderTargets[size_t( type )].push_back( result );
		return result;
	}

	void RenderTargetCache::remove( RenderTargetSPtr target )
	{
		auto lock = makeUniqueLock( *this );
		auto v = m_renderTargets.begin() + size_t( target->getTargetType() );
		auto it = std::find( v->begin(), v->end(), target );

		if ( it != v->end() )
		{
			v->erase( it );
		}
	}

	void RenderTargetCache::render( RenderInfo & info )
	{
		auto lock = makeUniqueLock( *this );

		for ( auto target : m_renderTargets[size_t( TargetType::eTexture )] )
		{
			target->render( info );
		}

		for ( auto target : m_renderTargets[size_t( TargetType::eWindow )] )
		{
			target->render( info );
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
