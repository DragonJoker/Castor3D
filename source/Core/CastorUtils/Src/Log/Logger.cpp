#include "Logger.hpp"
#include "LoggerImpl.hpp"

#include "Config/MultiThreadConfig.hpp"
#include "Data/File.hpp"

namespace castor
{
	static const std::string ERROR_LOGGER_ALREADY_INITIALISED = "Logger instance already initialised";

	template< typename CharType, typename LogStreambufTraits >
	class LogStreambuf
		: public std::basic_streambuf< CharType >
	{
	public:
		using string_type = std::basic_string< CharType >;
		using ostream_type = std::basic_ostream< CharType >;
		using streambuf_type = std::basic_streambuf< CharType >;
		using int_type = typename std::basic_streambuf< CharType >::int_type;
		using traits_type = typename std::basic_streambuf< CharType >::traits_type;

		explicit LogStreambuf( std::basic_ostream< CharType > & p_stream )
			: m_stream( p_stream )
		{
			m_old = m_stream.rdbuf( this );
		}

		virtual ~LogStreambuf()
		{
			m_stream.rdbuf( m_old );
		}

		virtual int_type overflow( int_type c = traits_type::eof() )
		{
			if ( traits_type::eq_int_type( c, traits_type::eof() ) )
			{
				do_sync();
			}
			else if ( c == '\n' )
			{
				do_sync();
			}
			else if ( c == '\r' )
			{
				m_buffer += '\r';
				do_sync_no_nl();
			}
			else
			{
				m_buffer += traits_type::to_char_type( c );
			}

			return c;
		}

		virtual int do_sync()
		{
			if ( !m_buffer.empty() )
			{
				LogStreambufTraits::Log( m_buffer );
				m_buffer.clear();
			}

			return 0;
		}

		virtual int do_sync_no_nl()
		{
			if ( !m_buffer.empty() )
			{
				LogStreambufTraits::LogNoNL( m_buffer );
				m_buffer.clear();
			}

			return 0;
		}

	private:
		string_type m_buffer;
		ostream_type & m_stream;
		streambuf_type * m_old;
	};

	template< typename CharType >
	struct DebugLogStreambufTraits
	{
		static void Log( std::basic_string< CharType > const & p_text )
		{
			Logger::logDebug( p_text );
		}
		static void LogNoNL( std::basic_string< CharType > const & p_text )
		{
			Logger::logDebugNoNL( p_text );
		}
	};

	template< typename CharType >
	struct InfoLogStreambufTraits
	{
		static void Log( std::basic_string< CharType > const & p_text )
		{
			Logger::logInfo( p_text );
		}
		static void LogNoNL( std::basic_string< CharType > const & p_text )
		{
			Logger::logInfoNoNL( p_text );
		}
	};

	template< typename CharType >
	struct WarningLogStreambufTraits
	{
		static void Log( std::basic_string< CharType > const & p_text )
		{
			Logger::logWarning( p_text );
		}
		static void LogNoNL( std::basic_string< CharType > const & p_text )
		{
			Logger::logWarningNoNL( p_text );
		}
	};

	template< typename CharType >
	struct ErrorLogStreambufTraits
	{
		static void Log( std::basic_string< CharType > const & p_text )
		{
			Logger::logError( p_text );
		}
		static void LogNoNL( std::basic_string< CharType > const & p_text )
		{
			Logger::logErrorNoNL( p_text );
		}
	};

	Logger * Logger::m_singleton = nullptr;

	Logger::Logger()
		: m_impl( nullptr )
	{
		auto lock = makeUniqueLock( m_mutex );
		m_headers[size_t( LogType::eTrace )] = cuT( "***TRACE*** " );
		m_headers[size_t( LogType::eDebug )] = cuT( "***DEBUG*** " );
		m_headers[size_t( LogType::eInfo )] = String();
		m_headers[size_t( LogType::eWarning )] = cuT( "***WARNING*** " );
		m_headers[size_t( LogType::eError )] = cuT( "***ERROR*** " );

		m_cout = new LogStreambuf< char, InfoLogStreambufTraits< char > >( std::cout );
		m_cerr = new LogStreambuf< char, ErrorLogStreambufTraits< char > >( std::cerr );
		m_clog = new LogStreambuf< char, DebugLogStreambufTraits< char > >( std::clog );
		m_wcout = new LogStreambuf< wchar_t, InfoLogStreambufTraits< wchar_t > >( std::wcout );
		m_wcerr = new LogStreambuf< wchar_t, ErrorLogStreambufTraits< wchar_t > >( std::wcerr );
		m_wclog = new LogStreambuf< wchar_t, DebugLogStreambufTraits< wchar_t > >( std::wclog );
		m_initialised = false;
		m_stopped = true;
		m_threadEnded = true;
	}

	Logger::~Logger()
	{
		delete m_cout;
		delete m_cerr;
		delete m_clog;
		delete m_wcout;
		delete m_wcerr;
		delete m_wclog;
		{
			auto lock = makeUniqueLock( m_mutex );
			m_impl->cleanup();
			delete m_impl;
			m_impl = nullptr;
		}
	}

	void Logger::initialise( LogType p_eLogLevel )
	{
		if ( m_singleton )
		{
			CASTOR_EXCEPTION( ERROR_LOGGER_ALREADY_INITIALISED );
		}
		else
		{
			m_singleton = new Logger();
			Logger & logger = getSingleton();
			{
				auto lock = makeUniqueLock( logger.m_mutex );
				delete logger.m_impl;
				logger.m_impl = new LoggerImpl{ p_eLogLevel };
				logger.m_impl->initialise( logger );
			}
			logger.m_logLevel = p_eLogLevel;
			logger.doInitialiseThread();
		}
	}

	void Logger::cleanup()
	{
		if ( m_singleton )
		{
			m_singleton->doCleanupThread();
			delete m_singleton;
			m_singleton = nullptr;
		}
	}

	void Logger::registerCallback( LogCallback p_pfnCallback, void * p_pCaller )
	{
		getSingleton().doRegisterCallback( p_pfnCallback, p_pCaller );
	}

	void Logger::unregisterCallback( void * p_pCaller )
	{
		getSingleton().doUnregisterCallback( p_pCaller );
	}

	void Logger::setFileName( String const & p_logFilePath, LogType p_eLogType )
	{
		if ( getSingleton().m_impl )
		{
			getSingleton().doSetFileName( p_logFilePath, p_eLogType );
		}
	}

	void Logger::logTrace( std::string const & p_msg )
	{
		getSingleton().doPushMessage( LogType::eTrace, p_msg );
	}

	void Logger::logTrace( std::ostream const & p_msg )
	{
		auto sbuf = p_msg.rdbuf();
		std::stringstream ss;
		ss << sbuf;
		logTrace( ss.str() );
	}

	void Logger::logTrace( std::wstring const & p_msg )
	{
		getSingleton().doPushMessage( LogType::eTrace, p_msg );
	}

	void Logger::logTrace( std::wostream const & p_msg )
	{
		std::wstringstream ss;
		ss << p_msg.rdbuf();
		logTrace( ss.str() );
	}

	void Logger::logDebug( std::string const & p_msg )
	{
		getSingleton().doPushMessage( LogType::eDebug, p_msg );
	}

	void Logger::logDebug( std::ostream const & p_msg )
	{
		auto sbuf = p_msg.rdbuf();
		std::stringstream ss;
		ss << sbuf;
		logDebug( ss.str() );
	}

	void Logger::logDebug( std::wstring const & p_msg )
	{
		getSingleton().doPushMessage( LogType::eDebug, p_msg );
	}

	void Logger::logDebug( std::wostream const & p_msg )
	{
		std::wstringstream ss;
		ss << p_msg.rdbuf();
		logDebug( ss.str() );
	}

	void Logger::logInfo( std::string const & p_msg )
	{
		getSingleton().doPushMessage( LogType::eInfo, p_msg );
	}

	void Logger::logInfo( std::ostream const & p_msg )
	{
		std::stringstream ss;
		ss << p_msg.rdbuf();
		logInfo( ss.str() );
	}

	void Logger::logInfo( std::wstring const & p_msg )
	{
		getSingleton().doPushMessage( LogType::eInfo, p_msg );
	}

	void Logger::logInfo( std::wostream const & p_msg )
	{
		std::wstringstream ss;
		ss << p_msg.rdbuf();
		logInfo( ss.str() );
	}

	void Logger::logWarning( std::string const & p_msg )
	{
		getSingleton().doPushMessage( LogType::eWarning, p_msg );
	}

	void Logger::logWarning( std::ostream const & p_msg )
	{
		std::stringstream ss;
		ss << p_msg.rdbuf();
		logWarning( ss.str() );
	}

	void Logger::logWarning( std::wstring const & p_msg )
	{
		getSingleton().doPushMessage( LogType::eWarning, p_msg );
	}

	void Logger::logWarning( std::wostream const & p_msg )
	{
		std::wstringstream ss;
		ss << p_msg.rdbuf();
		logWarning( ss.str() );
	}

	void Logger::logError( std::string const & p_msg )
	{
		getSingleton().doPushMessage( LogType::eError, p_msg );
	}

	void Logger::logError( std::ostream const & p_msg )
	{
		std::stringstream ss;
		ss << p_msg.rdbuf();
		logError( ss.str() );
	}

	void Logger::logError( std::wstring const & p_msg )
	{
		getSingleton().doPushMessage( LogType::eError, p_msg );
	}

	void Logger::logError( std::wostream const & p_msg )
	{
		std::wstringstream ss;
		ss << p_msg.rdbuf();
		logError( ss.str() );
	}

	void Logger::logTraceNoNL( std::string const & p_msg )
	{
		getSingleton().doPushMessage( LogType::eTrace, p_msg, false );
	}

	void Logger::logTraceNoNL( std::ostream const & p_msg )
	{
		auto sbuf = p_msg.rdbuf();
		std::stringstream ss;
		ss << sbuf;
		logTraceNoNL( ss.str() );
	}

	void Logger::logTraceNoNL( std::wstring const & p_msg )
	{
		getSingleton().doPushMessage( LogType::eTrace, p_msg, false );
	}

	void Logger::logTraceNoNL( std::wostream const & p_msg )
	{
		std::wstringstream ss;
		ss << p_msg.rdbuf();
		logTraceNoNL( ss.str() );
	}

	void Logger::logDebugNoNL( std::string const & p_msg )
	{
		getSingleton().doPushMessage( LogType::eDebug, p_msg, false );
	}

	void Logger::logDebugNoNL( std::ostream const & p_msg )
	{
		auto sbuf = p_msg.rdbuf();
		std::stringstream ss;
		ss << sbuf;
		logDebugNoNL( ss.str() );
	}

	void Logger::logDebugNoNL( std::wstring const & p_msg )
	{
		getSingleton().doPushMessage( LogType::eDebug, p_msg, false );
	}

	void Logger::logDebugNoNL( std::wostream const & p_msg )
	{
		std::wstringstream ss;
		ss << p_msg.rdbuf();
		logDebugNoNL( ss.str() );
	}

	void Logger::logInfoNoNL( std::string const & p_msg )
	{
		getSingleton().doPushMessage( LogType::eInfo, p_msg, false );
	}

	void Logger::logInfoNoNL( std::ostream const & p_msg )
	{
		std::stringstream ss;
		ss << p_msg.rdbuf();
		logInfoNoNL( ss.str() );
	}

	void Logger::logInfoNoNL( std::wstring const & p_msg )
	{
		getSingleton().doPushMessage( LogType::eInfo, p_msg, false );
	}

	void Logger::logInfoNoNL( std::wostream const & p_msg )
	{
		std::wstringstream ss;
		ss << p_msg.rdbuf();
		logInfoNoNL( ss.str() );
	}

	void Logger::logWarningNoNL( std::string const & p_msg )
	{
		getSingleton().doPushMessage( LogType::eWarning, p_msg, false );
	}

	void Logger::logWarningNoNL( std::ostream const & p_msg )
	{
		std::stringstream ss;
		ss << p_msg.rdbuf();
		logWarningNoNL( ss.str() );
	}

	void Logger::logWarningNoNL( std::wstring const & p_msg )
	{
		getSingleton().doPushMessage( LogType::eWarning, p_msg, false );
	}

	void Logger::logWarningNoNL( std::wostream const & p_msg )
	{
		std::wstringstream ss;
		ss << p_msg.rdbuf();
		logWarningNoNL( ss.str() );
	}

	void Logger::logErrorNoNL( std::string const & p_msg )
	{
		getSingleton().doPushMessage( LogType::eError, p_msg, false );
	}

	void Logger::logErrorNoNL( std::ostream const & p_msg )
	{
		std::stringstream ss;
		ss << p_msg.rdbuf();
		logErrorNoNL( ss.str() );
	}

	void Logger::logErrorNoNL( std::wstring const & p_msg )
	{
		getSingleton().doPushMessage( LogType::eError, p_msg, false );
	}

	void Logger::logErrorNoNL( std::wostream const & p_msg )
	{
		std::wstringstream ss;
		ss << p_msg.rdbuf();
		logError( ss.str() );
	}

	Logger & Logger::getSingleton()
	{
		if ( !m_singleton )
		{
			CASTOR_EXCEPTION( cuT( "No Logger initialised." ) );
		}

		return *m_singleton;
	}

	Logger * Logger::getSingletonPtr()
	{
		return &getSingleton();
	}

	void Logger::doRegisterCallback( LogCallback p_pfnCallback, void * p_pCaller )
	{
		auto lock = makeUniqueLock( m_mutex );
		m_impl->registerCallback( p_pfnCallback, p_pCaller );
	}

	void Logger::doUnregisterCallback( void * p_pCaller )
	{
		auto lock = makeUniqueLock( m_mutex );
		m_impl->unregisterCallback( p_pCaller );
	}

	void Logger::doSetFileName( String const & logFilePath, LogType logLevel )
	{
		m_initialised = true;
		{
			auto lock = makeUniqueLock( m_mutex );
			m_impl->setFileName( logFilePath, logLevel );
		}
	}

	void Logger::doPushMessage( LogType logLevel, std::string const & message, bool p_newLine )
	{
		if ( logLevel >= m_logLevel )
		{
#if !defined( NDEBUG )
			{
				auto lock = makeUniqueLock( m_mutex );
				m_impl->printMessage( logLevel, message, p_newLine );
			}
#endif
			auto lock = makeUniqueLock( m_mutexQueue );
			m_queue.push_back( { logLevel, message, p_newLine } );
		}
	}

	void Logger::doPushMessage( LogType logLevel, std::wstring const & message, bool p_newLine )
	{
		if ( logLevel >= m_logLevel )
		{
#if !defined( NDEBUG )
			{
				auto lock = makeUniqueLock( m_mutex );
				m_impl->printMessage( logLevel, message, p_newLine );
			}
#endif
			auto lock = makeUniqueLock( m_mutexQueue );
			m_queue.push_back( { logLevel, string::stringCast< char >( message ), p_newLine } );
		}
	}

	void Logger::doFlushQueue()
	{
		if ( !m_queue.empty() )
		{
			MessageQueue queue;

			{
				auto lock = makeUniqueLock( m_mutexQueue );
				std::swap( queue, m_queue );
			}

			{
				auto lock = makeUniqueLock( m_mutex );
				m_impl->logMessageQueue( queue );
			}
		}
	}

	void Logger::doInitialiseThread()
	{
		m_stopped = false;
		m_threadEnded = false;
		m_logThread = std::thread( [this]()
		{
			while ( !m_initialised && !m_stopped )
			{
				std::this_thread::sleep_for( Milliseconds( 10 ) );
			}

			while ( !m_stopped )
			{
				doFlushQueue();
				std::this_thread::sleep_for( Milliseconds( 100 ) );
			}

			if ( m_initialised )
			{
				doFlushQueue();
			}

			m_threadEnded = true;
		} );
	}

	void Logger::doCleanupThread()
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
