#include "CastorUtilsPch.hpp"

#include "Logger.hpp"
#include "Data/File.hpp"

#include "LoggerImpl.hpp"
#include "Miscellaneous/StringUtils.hpp"
#include "Exception/Exception.hpp"

#include <iostream>

namespace Castor
{
#if defined( _MSC_VER )
	int Vsnprintf( char * p_text, size_t p_count, const char * p_format, va_list p_valist )
	{
		return vsnprintf_s( p_text, p_count, p_count, p_format, p_valist );
	}

	int Vswprintf( wchar_t * p_text, size_t p_count, const wchar_t * p_format, va_list p_valist )
	{
		return vswprintf_s( p_text, p_count, p_format, p_valist );
	}
#else
	int Vsnprintf( char * p_text, size_t p_count, const char * p_format, va_list p_valist )
	{
		return vsnprintf( p_text, p_count, p_format, p_valist );
	}

#	if defined( _WIN32 )
	int Vswprintf( wchar_t * p_text, size_t p_count, const wchar_t * p_format, va_list p_valist )
	{
		return vswprintf( p_text, p_format, p_valist );
	}
#	else
	int Vswprintf( wchar_t * p_text, size_t p_count, const wchar_t * p_format, va_list p_valist )
	{
		return vswprintf( p_text, p_count, p_format, p_valist );
	}
#	endif
#endif

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
	};

	template< typename CharType >
	struct InfoLogStreambufTraits
	{
		static void Log( std::basic_string< CharType > const & p_text )
		{
			Logger::LogInfo( p_text );
		}
	};

	template< typename CharType >
	struct WarningLogStreambufTraits
	{
		static void Log( std::basic_string< CharType > const & p_text )
		{
			Logger::LogWarning( p_text );
		}
	};

	template< typename CharType >
	struct ErrorLogStreambufTraits
	{
		static void Log( std::basic_string< CharType > const & p_text )
		{
			Logger::LogError( p_text );
		}
	};

	Logger * Logger::m_singleton = nullptr;

	Logger::Logger()
		: m_impl( nullptr )
	{
		std::unique_lock< std::mutex > lock( m_mutex );
		m_headers[ELogType_DEBUG] = cuT( "***DEBUG*** " );
		m_headers[ELogType_INFO] = String();
		m_headers[ELogType_WARNING] = cuT( "***WARNING*** " );
		m_headers[ELogType_ERROR] = cuT( "***ERROR*** " );

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
		m_impl->Cleanup();
		delete m_impl;
		m_impl = nullptr;
	}

	void Logger::Initialise( ELogType p_eLogLevel )
	{
		if ( m_singleton )
		{
			CASTOR_EXCEPTION( ERROR_LOGGER_ALREADY_INITIALISED );
		}
		else
		{
			m_singleton = new Logger();
			Logger & l_logger = GetSingleton();
			delete l_logger.m_impl;
			l_logger.m_impl = new LoggerImpl;
			l_logger.m_impl->Initialise( l_logger );
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

	void Logger::SetFileName( String const & p_logFilePath, ELogType p_eLogType )
	{
		if ( GetSingleton().m_impl )
		{
			GetSingleton().DoSetFileName( p_logFilePath, p_eLogType );
		}
	}

	void Logger::LogDebug( std::string const & p_msg )
	{
		GetSingleton().DoPushMessage( ELogType_DEBUG, p_msg );
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
		GetSingleton().DoPushMessage( ELogType_DEBUG, p_msg );
	}

	void Logger::LogDebug( std::wostream const & p_msg )
	{
		std::wstringstream l_ss;
		l_ss << p_msg.rdbuf();
		LogDebug( l_ss.str() );
	}

	void Logger::LogInfo( std::string const & p_msg )
	{
		GetSingleton().DoPushMessage( ELogType_INFO, p_msg );
	}

	void Logger::LogInfo( std::ostream const & p_msg )
	{
		std::stringstream l_ss;
		l_ss << p_msg.rdbuf();
		LogInfo( l_ss.str() );
	}

	void Logger::LogInfo( std::wstring const & p_msg )
	{
		GetSingleton().DoPushMessage( ELogType_INFO, p_msg );
	}

	void Logger::LogInfo( std::wostream const & p_msg )
	{
		std::wstringstream l_ss;
		l_ss << p_msg.rdbuf();
		LogInfo( l_ss.str() );
	}

	void Logger::LogWarning( std::string const & p_msg )
	{
		GetSingleton().DoPushMessage( ELogType_WARNING, p_msg );
	}

	void Logger::LogWarning( std::ostream const & p_msg )
	{
		std::stringstream l_ss;
		l_ss << p_msg.rdbuf();
		LogWarning( l_ss.str() );
	}

	void Logger::LogWarning( std::wstring const & p_msg )
	{
		GetSingleton().DoPushMessage( ELogType_WARNING, p_msg );
	}

	void Logger::LogWarning( std::wostream const & p_msg )
	{
		std::wstringstream l_ss;
		l_ss << p_msg.rdbuf();
		LogWarning( l_ss.str() );
	}

	void Logger::LogError( std::string const & p_msg )
	{
		GetSingleton().DoPushMessage( ELogType_ERROR, p_msg );
	}

	void Logger::LogError( std::ostream const & p_msg )
	{
		std::stringstream l_ss;
		l_ss << p_msg.rdbuf();
		LogError( l_ss.str() );
	}

	void Logger::LogError( std::wstring const & p_msg )
	{
		GetSingleton().DoPushMessage( ELogType_ERROR, p_msg );
	}

	void Logger::LogError( std::wostream const & p_msg )
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
		std::unique_lock< std::mutex > lock( m_mutex );
		m_impl->RegisterCallback( p_pfnCallback, p_pCaller );
	}

	void Logger::DoUnregisterCallback( void * p_pCaller )
	{
		std::unique_lock< std::mutex > lock( m_mutex );
		m_impl->UnregisterCallback( p_pCaller );
	}

	void Logger::DoSetFileName( String const & logFilePath, ELogType logLevel )
	{
		m_initialised = true;
		{
			std::unique_lock< std::mutex > lock( m_mutex );
			m_impl->SetFileName( logFilePath, logLevel );
		}
	}

	void Logger::DoPushMessage( ELogType logLevel, std::string const & message )
	{
		if ( logLevel >= m_logLevel )
		{
#if !defined( NDEBUG )
			{
				std::unique_lock< std::mutex > lock( m_mutex );
				m_impl->PrintMessage( logLevel, message );
			}
#endif
			std::unique_lock< std::mutex > l_lock( m_mutexQueue );
			m_queue.push_back( std::make_unique< Message >( logLevel, message ) );
		}
	}

	void Logger::DoPushMessage( ELogType logLevel, std::wstring const & message )
	{
		if ( logLevel >= m_logLevel )
		{
#if !defined( NDEBUG )
			{
				std::unique_lock< std::mutex > lock( m_mutex );
				m_impl->PrintMessage( logLevel, message );
			}
#endif
			std::unique_lock< std::mutex > l_lock( m_mutexQueue );
			m_queue.push_back( std::make_unique< WMessage >( logLevel, message ) );
		}
	}

	void Logger::DoFlushQueue()
	{
		if ( !m_queue.empty() )
		{
			MessageQueue queue;

			{
				std::unique_lock< std::mutex > l_lock( m_mutexQueue );
				std::swap( queue, m_queue );
			}

			m_impl->LogMessageQueue( queue );
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
				std::unique_lock< std::mutex > l_lock( m_mutexThreadEnded );
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
				std::unique_lock< std::mutex > l_lock( m_mutexThreadEnded );
				m_threadEnded.wait( l_lock );
			}
			m_logThread.join();
		}
	}
}
