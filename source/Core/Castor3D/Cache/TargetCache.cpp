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

	RenderTargetSPtr RenderTargetCache::add( TargetType type
		, castor::Size const & size
		, castor::PixelFormat pixelFormat )
	{
		LockType lock{ castor::makeUniqueLock( *this ) };
		RenderTargetSPtr result = std::make_shared< RenderTarget >( *getEngine()
			, type
			, size
			, pixelFormat );
		m_renderTargets[size_t( type )].push_back( result );
		return result;
	}

	void RenderTargetCache::remove( RenderTargetSPtr target )
	{
		LockType lock{ castor::makeUniqueLock( *this ) };
		auto v = std::next( m_renderTargets.begin()
			, ptrdiff_t( target->getTargetType() ) );
		auto it = std::find( v->begin(), v->end(), target );

		if ( it != v->end() )
		{
			v->erase( it );
		}
	}

	void RenderTargetCache::update( CpuUpdater & updater )
	{
		LockType lock{ castor::makeUniqueLock( *this ) };

		for ( auto target : m_renderTargets[size_t( TargetType::eTexture )] )
		{
			target->update( updater );
		}
	}

	void RenderTargetCache::update( GpuUpdater & updater )
	{
		LockType lock{ castor::makeUniqueLock( *this ) };

		for ( auto target : m_renderTargets[size_t( TargetType::eTexture )] )
		{
			target->update( updater );
		}
	}

	void RenderTargetCache::upload( ashes::CommandBuffer const & cb )
	{
		LockType lock{ castor::makeUniqueLock( *this ) };

		for ( auto target : m_renderTargets[size_t( TargetType::eTexture )] )
		{
			target->upload( cb );
		}
	}

	crg::SemaphoreWaitArray RenderTargetCache::render( RenderDevice const & device
		, RenderInfo & info
		, ashes::Queue const & queue
		, crg::SemaphoreWaitArray signalsToWait )
	{
		LockType lock{ castor::makeUniqueLock( *this ) };
		crg::SemaphoreWaitArray result;

		for ( auto target : m_renderTargets[size_t( TargetType::eTexture )] )
		{
			auto toWait = target->render( device, info, queue, signalsToWait );
			result.insert( result.end()
				, toWait.begin()
				, toWait.end() );
			signalsToWait.clear();
		}

		result.insert( result.end()
			, signalsToWait.begin()
			, signalsToWait.end() );
		return result;
	}

	void RenderTargetCache::cleanup( RenderDevice const & device )
	{
		LockType lock{ castor::makeUniqueLock( *this ) };

		for ( auto & array : m_renderTargets )
		{
			for ( auto & target : array )
			{
				target->cleanup( device );
			}
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

	void RenderTargetCache::registerToneMappingName( castor::String const & name
		, castor::String const & fullName )
	{
		m_toneMappings[name] = fullName;
	}
}
