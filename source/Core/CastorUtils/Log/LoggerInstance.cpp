#include "CastorUtils/Log/LoggerInstance.hpp"

#include "CastorUtils/Log/LoggerImpl.hpp"

#include "CastorUtils/Config/MultiThreadConfig.hpp"
#include "CastorUtils/Data/File.hpp"

namespace castor
{
	LoggerInstance::LoggerInstance( LoggerInstance && rhs )
		: m_logLevel{ std::move( rhs.m_logLevel ) }
		, m_impl{ std::move( rhs.m_impl ) }
		, m_headers{ std::move( rhs.m_headers ) }
		, m_queue{ std::move( rhs.m_queue ) }
		, m_logThread{ std::move( rhs.m_logThread ) }
		, m_initialised{ rhs.m_initialised.load() }
		, m_stopped{ rhs.m_stopped.load() }
		, m_threadEnded{ rhs.m_threadEnded.load() }
	{
		rhs.m_initialised = false;
		rhs.m_stopped = false;
		rhs.m_threadEnded = false;
	}

	LoggerInstance & LoggerInstance::operator=( LoggerInstance && rhs )
	{
		m_logLevel = std::move( rhs.m_logLevel );
		m_impl = std::move( rhs.m_impl );
		m_headers = std::move( rhs.m_headers );
		m_queue = std::move( rhs.m_queue );
		m_logThread = std::move( rhs.m_logThread );
		m_initialised = rhs.m_initialised.load();
		m_stopped = rhs.m_stopped.load();
		m_threadEnded = rhs.m_threadEnded.load();
		rhs.m_initialised = false;
		rhs.m_stopped = false;
		rhs.m_threadEnded = false;
		return *this;
	}

	LoggerInstance::~LoggerInstance()
	{
		doCleanupThread();
	}

	LoggerInstance::LoggerInstance( ProgramConsole & console
		, LogType logType )
		: m_logLevel{ logType }
		, m_impl{ console, logType, *this }
		, m_headers
		{
			cuT( "****TRACE**** " ),
			cuT( "****DEBUG**** " ),
			cuT( "              " ),
			cuT( "***WARNING*** " ),
			cuT( "****ERROR**** " ),
		}
	{
		doInitialiseThread();
	}

	void LoggerInstance::registerCallback( LogCallback p_pfnCallback, void * p_pCaller )
	{
		m_impl.registerCallback( p_pfnCallback, p_pCaller );
	}

	void LoggerInstance::unregisterCallback( void * p_pCaller )
	{
		m_impl.unregisterCallback( p_pCaller );
	}

	void LoggerInstance::setFileName( Path const & logFilePath
		, LogType logType )
	{
		m_impl.setFileName( logFilePath, logType );
		m_initialised = true;
	}

	LogType LoggerInstance::getLevel()
	{
		return m_logLevel;
	}

	void LoggerInstance::lockedLogTrace( LoggerInstance::my_string const & p_msg )
	{
		doLockedPushMessage( LogType::eTrace, p_msg, true );
	}

	void LoggerInstance::logTrace( LoggerInstance::my_string const & p_msg )
	{
		pushMessage( LogType::eTrace, p_msg, true );
	}

	void LoggerInstance::logTrace( LoggerInstance::my_ostream const & p_msg )
	{
		auto sbuf = p_msg.rdbuf();
		std::stringstream ss;
		ss << sbuf;
		logTrace( ss.str() );
	}

	void LoggerInstance::lockedLogTraceNoLF( LoggerInstance::my_string const & p_msg )
	{
		doLockedPushMessage( LogType::eTrace, p_msg, false );
	}

	void LoggerInstance::logTraceNoLF( LoggerInstance::my_string const & p_msg )
	{
		pushMessage( LogType::eTrace, p_msg, false );
	}

	void LoggerInstance::logTraceNoLF( LoggerInstance::my_ostream const & p_msg )
	{
		auto sbuf = p_msg.rdbuf();
		std::stringstream ss;
		ss << sbuf;
		logTraceNoLF( ss.str() );
	}
	
	void LoggerInstance::lockedLogDebugNoLF( LoggerInstance::my_string const & p_msg )
	{
		doLockedPushMessage( LogType::eDebug, p_msg, false );
	}
	
	void LoggerInstance::logDebugNoLF( LoggerInstance::my_string const & p_msg )
	{
		pushMessage( LogType::eDebug, p_msg, false );
	}

	void LoggerInstance::logDebugNoLF( LoggerInstance::my_ostream const & p_msg )
	{
		auto sbuf = p_msg.rdbuf();
		std::stringstream ss;
		ss << sbuf;
		logDebugNoLF( ss.str() );
	}

	void LoggerInstance::lockedLogDebug( LoggerInstance::my_string const & p_msg )
	{
		doLockedPushMessage( LogType::eDebug, p_msg, true );
	}

	void LoggerInstance::logDebug( LoggerInstance::my_string const & p_msg )
	{
		pushMessage( LogType::eDebug, p_msg, true );
	}

	void LoggerInstance::logDebug( LoggerInstance::my_ostream const & p_msg )
	{
		auto sbuf = p_msg.rdbuf();
		std::stringstream ss;
		ss << sbuf;
		logDebug( ss.str() );
	}

	void LoggerInstance::lockedLogInfoNoLF( LoggerInstance::my_string const & p_msg )
	{
		doLockedPushMessage( LogType::eInfo, p_msg, false );
	}

	void LoggerInstance::logInfoNoLF( LoggerInstance::my_string const & p_msg )
	{
		pushMessage( LogType::eInfo, p_msg, false );
	}

	void LoggerInstance::logInfoNoLF( LoggerInstance::my_ostream const & p_msg )
	{
		std::stringstream ss;
		ss << p_msg.rdbuf();
		logInfoNoLF( ss.str() );
	}

	void LoggerInstance::lockedLogInfo( LoggerInstance::my_string const & p_msg )
	{
		doLockedPushMessage( LogType::eInfo, p_msg, true );
	}

	void LoggerInstance::logInfo( LoggerInstance::my_string const & p_msg )
	{
		pushMessage( LogType::eInfo, p_msg, true );
	}

	void LoggerInstance::logInfo( LoggerInstance::my_ostream const & p_msg )
	{
		std::stringstream ss;
		ss << p_msg.rdbuf();
		logInfo( ss.str() );
	}

	void LoggerInstance::lockedLogWarningNoLF( LoggerInstance::my_string const & p_msg )
	{
		doLockedPushMessage( LogType::eWarning, p_msg, false );
	}

	void LoggerInstance::logWarningNoLF( LoggerInstance::my_string const & p_msg )
	{
		pushMessage( LogType::eWarning, p_msg, false );
	}

	void LoggerInstance::logWarningNoLF( LoggerInstance::my_ostream const & p_msg )
	{
		std::stringstream ss;
		ss << p_msg.rdbuf();
		logWarningNoLF( ss.str() );
	}

	void LoggerInstance::lockedLogWarning( LoggerInstance::my_string const & p_msg )
	{
		doLockedPushMessage( LogType::eWarning, p_msg, true );
	}

	void LoggerInstance::logWarning( LoggerInstance::my_string const & p_msg )
	{
		pushMessage( LogType::eWarning, p_msg, true );
	}

	void LoggerInstance::logWarning( LoggerInstance::my_ostream const & p_msg )
	{
		std::stringstream ss;
		ss << p_msg.rdbuf();
		logWarning( ss.str() );
	}

	void LoggerInstance::lockedLogErrorNoLF( LoggerInstance::my_string const & p_msg )
	{
		doLockedPushMessage( LogType::eError, p_msg, false );
	}

	void LoggerInstance::logErrorNoLF( LoggerInstance::my_string const & p_msg )
	{
		pushMessage( LogType::eError, p_msg, false );
	}

	void LoggerInstance::logErrorNoLF( LoggerInstance::my_ostream const & p_msg )
	{
		std::stringstream ss;
		ss << p_msg.rdbuf();
		logErrorNoLF( ss.str() );
	}

	void LoggerInstance::lockedLogError( LoggerInstance::my_string const & p_msg )
	{
		doLockedPushMessage( LogType::eError, p_msg );
	}

	void LoggerInstance::logError( LoggerInstance::my_string const & p_msg )
	{
		pushMessage( LogType::eError, p_msg );
	}

	void LoggerInstance::logError( LoggerInstance::my_ostream const & p_msg )
	{
		std::stringstream ss;
		ss << p_msg.rdbuf();
		logError( ss.str() );
	}

	void LoggerInstance::pushMessage( LogType logLevel, std::string const & message, bool newLine )
	{
		auto lock( makeUniqueLock( m_mutexQueue ) );
		doLockedPushMessage( logLevel, message, newLine );
	}

	void LoggerInstance::flushQueue()
	{
		MessageQueue queue;

		{
			auto lock( makeUniqueLock( m_mutexQueue ) );
			std::swap( queue, m_queue );
		}

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

	void LoggerInstance::doLockedPushMessage( LogType logLevel, std::string const & message, bool newLine )
	{
		if ( logLevel >= m_logLevel )
		{
#if !defined( NDEBUG )
			m_impl.printMessage( logLevel, message, newLine );
#endif
			m_queue.push_back( { logLevel, message, newLine } );
		}
	}
}
