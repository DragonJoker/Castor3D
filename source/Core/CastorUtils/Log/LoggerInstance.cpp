#include "CastorUtils/Log/LoggerInstance.hpp"

#include "CastorUtils/Log/LoggerImpl.hpp"

#include "CastorUtils/Config/MultiThreadConfig.hpp"
#include "CastorUtils/Data/File.hpp"

namespace castor
{
	LoggerInstance::LoggerInstance( LoggerInstance && rhs )noexcept
		: m_logLevel{ castor::move( rhs.m_logLevel ) }
		, m_impl{ castor::move( rhs.m_impl ) }
		, m_headers{ castor::move( rhs.m_headers ) }
		, m_queue{ castor::move( rhs.m_queue ) }
		, m_logThread{ castor::move( rhs.m_logThread ) }
		, m_initialised{ rhs.m_initialised.load() }
		, m_stopped{ rhs.m_stopped.load() }
		, m_threadEnded{ rhs.m_threadEnded.load() }
	{
		rhs.m_initialised = false;
		rhs.m_stopped = false;
		rhs.m_threadEnded = false;
	}

	LoggerInstance & LoggerInstance::operator=( LoggerInstance && rhs )noexcept
	{
		m_logLevel = castor::move( rhs.m_logLevel );
		m_impl = castor::move( rhs.m_impl );
		m_headers = castor::move( rhs.m_headers );
		m_queue = castor::move( rhs.m_queue );
		m_logThread = castor::move( rhs.m_logThread );
		m_initialised = rhs.m_initialised.load();
		m_stopped = rhs.m_stopped.load();
		m_threadEnded = rhs.m_threadEnded.load();
		rhs.m_initialised = false;
		rhs.m_stopped = false;
		rhs.m_threadEnded = false;
		return *this;
	}

	LoggerInstance::~LoggerInstance()noexcept
	{
		doCleanupThread();
	}

	LoggerInstance::LoggerInstance( ProgramConsole & console
		, LogType logType )
		: m_logLevel{ logType }
		, m_impl{ console, logType, *this }
	{
		doInitialiseThread();
	}

	void LoggerInstance::registerCallback( LogCallback const & pfnCallback, void * pCaller )
	{
		m_impl.registerCallback( pfnCallback, pCaller );
	}

	void LoggerInstance::unregisterCallback( void * pCaller )
	{
		m_impl.unregisterCallback( pCaller );
	}

	void LoggerInstance::setFileName( Path const & logFilePath
		, LogType logType )
	{
		m_impl.setFileName( logFilePath, logType );
		m_initialised = true;
	}

	LogType LoggerInstance::getLevel()const
	{
		return m_logLevel;
	}

	void LoggerInstance::flushQueue()
	{
		MessageQueue queue;
		[&queue, this]()
		{
			auto lock( makeUniqueLock( m_mutexQueue ) );
			castor::swap( queue, m_queue );
		}();

		if ( !queue.empty() )
		{
			m_impl.logMessageQueue( queue );
		}
	}

	void LoggerInstance::doInitialiseThread()
	{
		m_logThread = std::thread( [this]()
			{
				while ( !m_initialised && !m_stopped )
				{
					std::this_thread::sleep_for( Milliseconds( 10 ) );
				}

				while ( !m_stopped )
				{
					flushQueue();
					std::this_thread::sleep_for( Milliseconds( 100 ) );
				}

				if ( m_initialised )
				{
					flushQueue();
				}

				m_threadEnded = true;
			} );
	}

	void LoggerInstance::doCleanupThread()
	{
		if ( !m_stopped )
		{
			m_stopped = true;

			while ( !m_threadEnded )
			{
				std::this_thread::sleep_for( 1_ms );
			}

			m_logThread.join();
		}
	}

	void LoggerInstance::doLockedPushMessage( LogType logLevel, MbString const & message, bool newLine )
	{
		if ( logLevel >= m_logLevel )
		{
#if !defined( NDEBUG )
			m_impl.printMessage( logLevel, message, newLine );
#endif
			m_queue.emplace_back( logLevel, message, newLine );
		}
	}
}
