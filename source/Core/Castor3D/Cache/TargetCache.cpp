#include "Castor3D/Cache/TargetCache.hpp"

#include "Castor3D/Render/RenderTarget.hpp"

namespace c3d
{
	namespace cachetgt
	{
		using LockType = UniqueLock< RenderTargetCache >;
	}

	RenderTargetCache::RenderTargetCache( Engine & engine )
		: OwnedBy< Engine >{ engine }
	{
	}

	RenderTargetRPtr RenderTargetCache::addNew( TargetType type
		, Size const & size
		, PixelFormat pixelFormat )
	{
		cachetgt::LockType lock{ makeUniqueLock( *this ) };
		auto result = makeUnique< RenderTarget >( *getEngine()
			, type
			, size
			, pixelFormat );
		auto ret = result.get();
		m_renderTargets[size_t( type )].push_back( c3d::move( result ) );
		return ret;
	}

	void RenderTargetCache::remove( RenderTarget const * target )noexcept
	{
		cachetgt::LockType lock{ makeUniqueLock( *this ) };
		auto v = std::next( m_renderTargets.begin()
			, ptrdiff_t( target->getTargetType() ) );
		auto it = std::find_if( v->begin()
			, v->end()
			, [&target]( RenderTargetUPtr const & lookup )
			{
				return target == lookup.get();
			} );

		if ( it != v->end() )
		{
			v->erase( it );
		}
	}

	void RenderTargetCache::update( CpuUpdater & updater )
	{
		cachetgt::LockType lock{ makeUniqueLock( *this ) };

		for ( auto const & target : m_renderTargets[size_t( TargetType::eTexture )] )
		{
			target->update( updater );
		}
	}

	void RenderTargetCache::update( GpuUpdater & updater )
	{
		cachetgt::LockType lock{ makeUniqueLock( *this ) };

		for ( auto const & target : m_renderTargets[size_t( TargetType::eTexture )] )
		{
			target->update( updater );
		}
	}

	void RenderTargetCache::upload( UploadData & uploader )
	{
		cachetgt::LockType lock{ makeUniqueLock( *this ) };

		for ( auto const & target : m_renderTargets[size_t( TargetType::eTexture )] )
		{
			target->upload( uploader );
		}
	}

	SemaphoreWaitArray RenderTargetCache::render( RenderDevice const & device
		, RenderInfo & info
		, ashes::Queue const & queue
		, SemaphoreWaitArray signalsToWait )
	{
		cachetgt::LockType lock{ makeUniqueLock( *this ) };
		SemaphoreWaitArray result;

		for ( auto const & target : m_renderTargets[size_t( TargetType::eTexture )] )
		{
			auto toWait = target->render( queue, signalsToWait );
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
		cachetgt::LockType lock{ makeUniqueLock( *this ) };

		for ( auto const & array : m_renderTargets )
		{
			for ( auto const & target : array )
			{
				target->cleanup( device );
			}
		}
	}

	void RenderTargetCache::clear()
	{
		cachetgt::LockType lock{ makeUniqueLock( *this ) };

		for ( auto & array : m_renderTargets )
		{
			array.clear();
		}
	}

	void RenderTargetCache::registerToneMappingName( String const & name
		, String fullName )
	{
		m_toneMappings[name] = c3d::move( fullName );
	}
}
