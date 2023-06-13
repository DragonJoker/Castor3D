#include "Castor3D/Cache/TargetCache.hpp"

#include "Castor3D/Render/RenderTarget.hpp"

namespace castor3d
{
	namespace cachetgt
	{
		using LockType = std::unique_lock< RenderTargetCache >;
	}

	RenderTargetCache::RenderTargetCache( Engine & engine )
		: castor::OwnedBy< Engine >{ engine }
	{
	}

	RenderTargetCache::~RenderTargetCache()
	{
	}

	RenderTargetRPtr RenderTargetCache::add( TargetType type
		, castor::Size const & size
		, castor::PixelFormat pixelFormat )
	{
		cachetgt::LockType lock{ castor::makeUniqueLock( *this ) };
		auto result = castor::makeUnique< RenderTarget >( *getEngine()
			, type
			, size
			, pixelFormat );
		auto ret = result.get();
		m_renderTargets[size_t( type )].push_back( std::move( result ) );
		return ret;
	}

	void RenderTargetCache::remove( RenderTargetRPtr target )
	{
		cachetgt::LockType lock{ castor::makeUniqueLock( *this ) };
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
		cachetgt::LockType lock{ castor::makeUniqueLock( *this ) };

		for ( auto & target : m_renderTargets[size_t( TargetType::eTexture )] )
		{
			target->update( updater );
		}
	}

	void RenderTargetCache::update( GpuUpdater & updater )
	{
		cachetgt::LockType lock{ castor::makeUniqueLock( *this ) };

		for ( auto & target : m_renderTargets[size_t( TargetType::eTexture )] )
		{
			target->update( updater );
		}
	}

	void RenderTargetCache::upload( UploadData & uploader )
	{
		cachetgt::LockType lock{ castor::makeUniqueLock( *this ) };

		for ( auto & target : m_renderTargets[size_t( TargetType::eTexture )] )
		{
			target->upload( uploader );
		}
	}

	crg::SemaphoreWaitArray RenderTargetCache::render( RenderDevice const & device
		, RenderInfo & info
		, ashes::Queue const & queue
		, crg::SemaphoreWaitArray signalsToWait )
	{
		cachetgt::LockType lock{ castor::makeUniqueLock( *this ) };
		crg::SemaphoreWaitArray result;

		for ( auto & target : m_renderTargets[size_t( TargetType::eTexture )] )
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
		cachetgt::LockType lock{ castor::makeUniqueLock( *this ) };

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
		cachetgt::LockType lock{ castor::makeUniqueLock( *this ) };

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
