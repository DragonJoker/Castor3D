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
		LockType lock{ castor::makeUniqueLock( *this ) };
		RenderTargetSPtr result = std::make_shared< RenderTarget >( *getEngine(), type );
		m_renderTargets[size_t( type )].push_back( result );
		return result;
	}

	void RenderTargetCache::remove( RenderTargetSPtr target )
	{
		LockType lock{ castor::makeUniqueLock( *this ) };
		auto v = m_renderTargets.begin() + size_t( target->getTargetType() );
		auto it = std::find( v->begin(), v->end(), target );

		if ( it != v->end() )
		{
			v->erase( it );
		}
	}

	void RenderTargetCache::cpuUpdate()
	{
		LockType lock{ castor::makeUniqueLock( *this ) };

		for ( auto target : m_renderTargets[size_t( TargetType::eTexture )] )
		{
			target->cpuUpdate();
		}

		for ( auto target : m_renderTargets[size_t( TargetType::eWindow )] )
		{
			target->cpuUpdate();
		}
	}

	void RenderTargetCache::gpuUpdate( RenderInfo & info )
	{
		LockType lock{ castor::makeUniqueLock( *this ) };

		for ( auto target : m_renderTargets[size_t( TargetType::eTexture )] )
		{
			target->gpuUpdate( info );
		}

		for ( auto target : m_renderTargets[size_t( TargetType::eWindow )] )
		{
			target->gpuUpdate( info );
		}
	}

	void RenderTargetCache::render( RenderInfo & info
		, ashes::Semaphore const & toWait )
	{
		auto result = &toWait;
		LockType lock{ castor::makeUniqueLock( *this ) };

		for ( auto target : m_renderTargets[size_t( TargetType::eTexture )] )
		{
			result = &target->render( info, *result );
		}

		for ( auto target : m_renderTargets[size_t( TargetType::eWindow )] )
		{
			result = &target->render( info, *result );
		}
	}

	void RenderTargetCache::clear()
	{
		LockType lock{ castor::makeUniqueLock( *this ) };

		for ( auto & array : m_renderTargets )
		{
			array.clear();
		}
	}
}
