#include "CastorUtilsPch.hpp"

#include "Logger.hpp"
#include "LoggerImpl.hpp"

#include "Config/MultiThreadConfig.hpp"
#include "Data/File.hpp"
#include "Miscellaneous/StringUtils.hpp"
#include "Exception/Exception.hpp"

#include <iostream>

namespace Castor
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

		LogStreambuf( std::basic_ostream< CharType > & p_stream )
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
			Logger::LogDebug( p_text );
		}
		static void LogNoNL( std::basic_string< CharType > const & p_text )
		{
			Logger::LogDebugNoNL( p_text );
		}
	};

	template< typename CharType >
	struct InfoLogStreambufTraits
	{
		static void Log( std::basic_string< CharType > const & p_text )
		{
			Logger::LogInfo( p_text );
		}
		static void LogNoNL( std::basic_string< CharType > const & p_text )
		{
			Logger::LogInfoNoNL( p_text );
		}
	};

	template< typename CharType >
	struct WarningLogStreambufTraits
	{
		static void Log( std::basic_string< CharType > const & p_text )
		{
			Logger::LogWarning( p_text );
		}
		static void LogNoNL( std::basic_string< CharType > const & p_text )
		{
			Logger::LogWarningNoNL( p_text );
		}
	};

	template< typename CharType >
	struct ErrorLogStreambufTraits
	{
		static void Log( std::basic_string< CharType > const & p_text )
		{
			Logger::LogError( p_text );
		}
		static void LogNoNL( std::basic_string< CharType > const & p_text )
		{
			Logger::LogErrorNoNL( p_text );
		}
	};

	Logger * Logger::m_singleton = nullptr;

	Logger::Logger()
		: m_impl( nullptr )
	{
		auto l_lock = make_unique_lock( m_mutex );
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
			auto l_lock = make_unique_lock( m_mutex );
			m_impl->Cleanup();
			delete m_impl;
			m_impl = nullptr;
		}
	}

	void Logger::Initialise( LogType p_eLogLevel )
	{
		if ( m_singleton )
		{
			CASTOR_EXCEPTION( ERROR_LOGGER_ALREADY_INITIALISED );
		}
		else
		{
			m_singleton = new Logger();
			Logger & l_logger = GetSingleton();
			{
				auto l_lock = make_unique_lock( l_logger.m_mutex );
				delete l_logger.m_impl;
				l_logger.m_impl = new LoggerImpl{ p_eLogLevel };
				l_logger.m_impl->Initialise( l_logger );
			}
			l_logger.m_logLevel = p_eLogLevel;
			l_logger.DoInitialiseThread();
		}
	}

	void Logger::Cleanup()
	{
		if ( m_singleton )
		{
			m_singleton->DoCleanupThread();
			delete m_singleton;
			m_singleton = nullptr;
		}
	}

	void Logger::RegisterCallback( LogCallback p_pfnCallback, void * p_pCaller )
	{
		GetSingleton().DoRegisterCallback( p_pfnCallback, p_pCaller );
	}

	void Logger::UnregisterCallback( void * p_pCaller )
	{
		GetSingleton().DoUnregisterCallback( p_pCaller );
	}

	void Logger::SetFileName( String const & p_logFilePath, LogType p_eLogType )
	{
		if ( GetSingleton().m_impl )
		{
			GetSingleton().DoSetFileName( p_logFilePath, p_eLogType );
		}
	}

	void Logger::LogTrace( std::string const & p_msg )
	{
		GetSingleton().DoPushMessage( LogType::eTrace, p_msg );
	}

	void Logger::LogTrace( std::ostream const & p_msg )
	{
		auto l_sbuf = p_msg.rdbuf();
		std::stringstream l_ss;
		l_ss << l_sbuf;
		LogTrace( l_ss.str() );
	}

	void Logger::LogTrace( std::wstring const & p_msg )
	{
		GetSingleton().DoPushMessage( LogType::eTrace, p_msg );
	}

	void Logger::LogTrace( std::wostream const & p_msg )
	{
		std::wstringstream l_ss;
		l_ss << p_msg.rdbuf();
		LogTrace( l_ss.str() );
	}

	void Logger::LogDebug( std::string const & p_msg )
	{
		GetSingleton().DoPushMessage( LogType::eDebug, p_msg );
	}

	void Logger::LogDebug( std::ostream const & p_msg )
	{
		auto l_sbuf = p_msg.rdbuf();
		std::stringstream l_ss;
		l_ss << l_sbuf;
		LogDebug( l_ss.str() );
	}

	void Logger::LogDebug( std::wstring const & p_msg )
	{
		GetSingleton().DoPushMessage( LogType::eDebug, p_msg );
	}

	void Logger::LogDebug( std::wostream const & p_msg )
	{
		std::wstringstream l_ss;
		l_ss << p_msg.rdbuf();
		LogDebug( l_ss.str() );
	}

	void Logger::LogInfo( std::string const & p_msg )
	{
		GetSingleton().DoPushMessage( LogType::eInfo, p_msg );
	}

	void Logger::LogInfo( std::ostream const & p_msg )
	{
		std::stringstream l_ss;
		l_ss << p_msg.rdbuf();
		LogInfo( l_ss.str() );
	}

	void Logger::LogInfo( std::wstring const & p_msg )
	{
		GetSingleton().DoPushMessage( LogType::eInfo, p_msg );
	}

	void Logger::LogInfo( std::wostream const & p_msg )
	{
		std::wstringstream l_ss;
		l_ss << p_msg.rdbuf();
		LogInfo( l_ss.str() );
	}

	void Logger::LogWarning( std::string const & p_msg )
	{
		GetSingleton().DoPushMessage( LogType::eWarning, p_msg );
	}

	void Logger::LogWarning( std::ostream const & p_msg )
	{
		std::stringstream l_ss;
		l_ss << p_msg.rdbuf();
		LogWarning( l_ss.str() );
	}

	void Logger::LogWarning( std::wstring const & p_msg )
	{
		GetSingleton().DoPushMessage( LogType::eWarning, p_msg );
	}

	void Logger::LogWarning( std::wostream const & p_msg )
	{
		std::wstringstream l_ss;
		l_ss << p_msg.rdbuf();
		LogWarning( l_ss.str() );
	}

	void Logger::LogError( std::string const & p_msg )
	{
		GetSingleton().DoPushMessage( LogType::eError, p_msg );
	}

	void Logger::LogError( std::ostream const & p_msg )
	{
		std::stringstream l_ss;
		l_ss << p_msg.rdbuf();
		LogError( l_ss.str() );
	}

	void Logger::LogError( std::wstring const & p_msg )
	{
		GetSingleton().DoPushMessage( LogType::eError, p_msg );
	}

	void Logger::LogError( std::wostream const & p_msg )
	{
		std::wstringstream l_ss;
		l_ss << p_msg.rdbuf();
		LogError( l_ss.str() );
	}

	void Logger::LogTraceNoNL( std::string const & p_msg )
	{
		GetSingleton().DoPushMessage( LogType::eTrace, p_msg, false );
	}

	void Logger::LogTraceNoNL( std::ostream const & p_msg )
	{
		auto l_sbuf = p_msg.rdbuf();
		std::stringstream l_ss;
		l_ss << l_sbuf;
		LogTraceNoNL( l_ss.str() );
	}

	void Logger::LogTraceNoNL( std::wstring const & p_msg )
	{
		GetSingleton().DoPushMessage( LogType::eTrace, p_msg, false );
	}

	void Logger::LogTraceNoNL( std::wostream const & p_msg )
	{
		std::wstringstream l_ss;
		l_ss << p_msg.rdbuf();
		LogTraceNoNL( l_ss.str() );
	}

	void Logger::LogDebugNoNL( std::string const & p_msg )
	{
		GetSingleton().DoPushMessage( LogType::eDebug, p_msg, false );
	}

	void Logger::LogDebugNoNL( std::ostream const & p_msg )
	{
		auto l_sbuf = p_msg.rdbuf();
		std::stringstream l_ss;
		l_ss << l_sbuf;
		LogDebugNoNL( l_ss.str() );
	}

	void Logger::LogDebugNoNL( std::wstring const & p_msg )
	{
		GetSingleton().DoPushMessage( LogType::eDebug, p_msg, false );
	}

	void Logger::LogDebugNoNL( std::wostream const & p_msg )
	{
		std::wstringstream l_ss;
		l_ss << p_msg.rdbuf();
		LogDebugNoNL( l_ss.str() );
	}

	void Logger::LogInfoNoNL( std::string const & p_msg )
	{
		GetSingleton().DoPushMessage( LogType::eInfo, p_msg, false );
	}

	void Logger::LogInfoNoNL( std::ostream const & p_msg )
	{
		std::stringstream l_ss;
		l_ss << p_msg.rdbuf();
		LogInfoNoNL( l_ss.str() );
	}

	void Logger::LogInfoNoNL( std::wstring const & p_msg )
	{
		GetSingleton().DoPushMessage( LogType::eInfo, p_msg, false );
	}

	void Logger::LogInfoNoNL( std::wostream const & p_msg )
	{
		std::wstringstream l_ss;
		l_ss << p_msg.rdbuf();
		LogInfoNoNL( l_ss.str() );
	}

	void Logger::LogWarningNoNL( std::string const & p_msg )
	{
		GetSingleton().DoPushMessage( LogType::eWarning, p_msg, false );
	}

	void Logger::LogWarningNoNL( std::ostream const & p_msg )
	{
		std::stringstream l_ss;
		l_ss << p_msg.rdbuf();
		LogWarningNoNL( l_ss.str() );
	}

	void Logger::LogWarningNoNL( std::wstring const & p_msg )
	{
		GetSingleton().DoPushMessage( LogType::eWarning, p_msg, false );
	}

	void Logger::LogWarningNoNL( std::wostream const & p_msg )
	{
		std::wstringstream l_ss;
		l_ss << p_msg.rdbuf();
		LogWarningNoNL( l_ss.str() );
	}

	void Logger::LogErrorNoNL( std::string const & p_msg )
	{
		GetSingleton().DoPushMessage( LogType::eError, p_msg, false );
	}

	void Logger::LogErrorNoNL( std::ostream const & p_msg )
	{
		std::stringstream l_ss;
		l_ss << p_msg.rdbuf();
		LogErrorNoNL( l_ss.str() );
	}

	void Logger::LogErrorNoNL( std::wstring const & p_msg )
	{
		GetSingleton().DoPushMessage( LogType::eError, p_msg, false );
	}

	void Logger::LogErrorNoNL( std::wostream const & p_msg )
	{
		std::wstringstream l_ss;
		l_ss << p_msg.rdbuf();
		LogError( l_ss.str() );
	}

	Logger & Logger::GetSingleton()
	{
		if ( !m_singleton )
		{
			CASTOR_EXCEPTION( cuT( "No Logger initialised." ) );
		}

		return *m_singleton;
	}

	Logger * Logger::GetSingletonPtr()
	{
		return &GetSingleton();
	}

	void Logger::DoRegisterCallback( LogCallback p_pfnCallback, void * p_pCaller )
	{
		auto l_lock = make_unique_lock( m_mutex );
		m_impl->RegisterCallback( p_pfnCallback, p_pCaller );
	}

	void Logger::DoUnregisterCallback( void * p_pCaller )
	{
		auto l_lock = make_unique_lock( m_mutex );
		m_impl->UnregisterCallback( p_pCaller );
	}

	void Logger::DoSetFileName( String const & logFilePath, LogType logLevel )
	{
		m_initialised = true;
		{
			auto l_lock = make_unique_lock( m_mutex );
			m_impl->SetFileName( logFilePath, logLevel );
		}
	}

	void Logger::DoPushMessage( LogType logLevel, std::string const & message, bool p_newLine )
	{
		if ( logLevel >= m_logLevel )
		{
#if !defined( NDEBUG )
			{
				auto l_lock = make_unique_lock( m_mutex );
				m_impl->PrintMessage( logLevel, message, p_newLine );
			}
#endif
			auto l_lock = make_unique_lock( m_mutexQueue );
			m_queue.push_back( { logLevel, message, p_newLine } );
		}
	}

	void Logger::DoPushMessage( LogType logLevel, std::wstring const & message, bool p_newLine )
	{
		if ( logLevel >= m_logLevel )
		{
#if !defined( NDEBUG )
			{
				auto l_lock = make_unique_lock( m_mutex );
				m_impl->PrintMessage( logLevel, message, p_newLine );
			}
#endif
			auto l_lock = make_unique_lock( m_mutexQueue );
			m_queue.push_back( { logLevel, string::string_cast< char >( message ), p_newLine } );
		}
	}

	void Logger::DoFlushQueue()
	{
		if ( !m_queue.empty() )
		{
			MessageQueue queue;

			{
				auto l_lock = make_unique_lock( m_mutexQueue );
				std::swap( queue, m_queue );
			}

			{
				auto l_lock = make_unique_lock( m_mutex );
				m_impl->LogMessageQueue( queue );
			}
		}
	}

	void Logger::DoInitialiseThread()
	{
		m_stopped = false;
		m_logThread = std::thread( [this]()
		{
			while ( !m_initialised && !m_stopped )
			{
				std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
			}

			while ( !m_stopped )
			{
				DoFlushQueue();
				std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
			}

			if ( m_initialised )
			{
				DoFlushQueue();
			}

			{
				auto l_lock = make_unique_lock( m_mutexThreadEnded );
				m_threadEnded.notify_all();
			}
		} );
	}

	void Logger::DoCleanupThread()
	{
		if ( !m_stopped )
		{
			m_stopped = true;
			{
				auto l_lock = make_unique_lock( m_mutexThreadEnded );
				m_threadEnded.wait( l_lock );
			}
			m_logThread.join();
		}
	}
}
