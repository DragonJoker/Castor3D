#include "CastorUtils/Log/LoggerInstance.hpp"

#include "CastorUtils/Log/LoggerImpl.hpp"

#include "CastorUtils/Config/MultiThreadConfig.hpp"
#include "CastorUtils/Data/File.hpp"

namespace castor
{
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

	void LoggerInstance::pushMessage( LogType logLevel, std::string const & message, bool p_newLine )
	{
		if ( logLevel >= m_logLevel )
		{
#if !defined( NDEBUG )
			m_impl.printMessage( logLevel, message, p_newLine );
#endif
			auto lock( makeUniqueLock( m_mutexQueue ) );
			m_queue.push_back( { logLevel, message, p_newLine } );
		}
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
}
