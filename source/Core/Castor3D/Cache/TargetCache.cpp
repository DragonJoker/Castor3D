#include "Castor3D/Cache/TargetCache.hpp"

#include "Castor3D/Render/RenderTarget.hpp"

using namespace castor;

namespace castor3d
{
	namespace
	{
		using LockType = std::unique_lock< RenderTargetCache >;
	}

	RenderTargetCache::RenderTargetCache( Engine & engine )
		: OwnedBy< Engine >{ engine }
	{
	}

	RenderTargetCache::~RenderTargetCache()
	{
	}

	RenderTargetSPtr RenderTargetCache::add( TargetType type )
	{
		LockType lock{ makeUniqueLock( *this ) };
		RenderTargetSPtr result = std::make_shared< RenderTarget >( *getEngine(), type );
		m_renderTargets[size_t( type )].push_back( result );
		return result;
	}

	void RenderTargetCache::remove( RenderTargetSPtr target )
	{
		LockType lock{ makeUniqueLock( *this ) };
		auto v = m_renderTargets.begin() + size_t( target->getTargetType() );
		auto it = std::find( v->begin(), v->end(), target );

		if ( it != v->end() )
		{
			v->erase( it );
		}
	}

	void RenderTargetCache::render( RenderInfo & info )
	{
		LockType lock{ makeUniqueLock( *this ) };

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
