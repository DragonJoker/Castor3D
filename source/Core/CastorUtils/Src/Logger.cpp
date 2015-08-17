#include "CastorUtilsPch.hpp"

#include "Logger.hpp"
#include "File.hpp"

#include "LoggerImpl.hpp"
#include "StringUtils.hpp"
#include "Exception.hpp"

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
		typedef typename std::basic_streambuf< CharType >::int_type int_type;
		typedef typename std::basic_streambuf< CharType >::traits_type traits_type;

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
				sync();
			}
			else
			{
				m_buffer += traits_type::to_char_type( c );
			}

			return c;
		}

		virtual int sync()
		{
			if ( !m_buffer.empty() )
			{
				LogStreambufTraits::Log( m_buffer );
				m_buffer.clear();
			}

			return 0;
		}

	private:
		std::basic_string< CharType > m_buffer;
		std::basic_ostream< CharType > & m_stream;
		std::basic_streambuf< CharType > * m_old;
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

	Logger * Logger::m_singleton = NULL;
	bool Logger::m_ownInstance = true;
	uint32_t Logger::m_counter = 0;

	Logger::Logger()
		: m_impl( NULL )
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
	}

	Logger::~Logger()
	{
		delete m_cout;
		delete m_cerr;
		delete m_clog;
		delete m_wcout;
		delete m_wcerr;
		delete m_wclog;

		if ( m_ownInstance )
		{
			m_impl->Cleanup();
			delete m_impl;
			m_impl = NULL;
		}
	}

	void Logger::Initialise( Logger * p_pLogger )
	{
		m_counter++;
#if defined( _WIN32 )

		if ( m_singleton )
		{
			m_counter--;
			Logger::LogWarning( ERROR_LOGGER_ALREADY_INITIALISED );
		}
		else
		{
			m_ownInstance = false;
			Logger & l_logger = GetSingleton();
			delete l_logger.m_impl;
			l_logger.m_impl = p_pLogger->m_impl;

			for ( int i = 0; i < ELogType_COUNT; i++ )
			{
				l_logger.m_headers[i] = p_pLogger->m_headers[i];
			}

			l_logger.m_logLevel = p_pLogger->m_logLevel;
			l_logger.DoInitialiseThread();
		}

#endif
	}

	void Logger::Initialise( ELogType p_eLogLevel )
	{
		m_counter++;

		if ( m_singleton )
		{
			CASTOR_EXCEPTION( ERROR_LOGGER_ALREADY_INITIALISED );
		}
		else
		{
			m_ownInstance = true;
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
		GetSingleton().DoCleanupThread();

		if ( m_counter > 0 )
		{
			m_counter--;
			delete m_singleton;
			m_singleton = NULL;
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

	void Logger::LogDebug( char const * p_pFormat, ... )
	{
		if ( p_pFormat )
		{
			char l_pText[256];
			va_list l_vaList;
			va_start( l_vaList, p_pFormat );
			Vsnprintf( l_pText, p_pFormat, l_vaList );
			va_end( l_vaList );
			LogDebug( std::string( l_pText ) );
		}
	}

	void Logger::LogDebug( std::string const & p_msg )
	{
		GetSingleton().DoPushMessage( ELogType_DEBUG, p_msg );
	}

	void Logger::LogDebug( std::ostream const & p_msg )
	{
		std::stringstream l_ss;
		l_ss << p_msg.rdbuf();
		LogDebug( l_ss.str() );
	}

	void Logger::LogDebug( wchar_t const * p_pFormat , ... )
	{
		if ( p_pFormat )
		{
			wchar_t l_pText[256];
			va_list l_vaList;
			va_start( l_vaList, p_pFormat );
			Vswprintf( l_pText, p_pFormat, l_vaList );
			va_end( l_vaList );
			LogDebug( std::wstring( l_pText ) );
		}
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

	void Logger::LogInfo( char const * p_pFormat, ... )
	{
		if ( p_pFormat )
		{
			char l_pText[256];
			va_list l_vaList;
			va_start( l_vaList, p_pFormat );
			Vsnprintf( l_pText, p_pFormat, l_vaList );
			va_end( l_vaList );
			LogInfo( std::string( l_pText ) );
		}
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

	void Logger::LogInfo( wchar_t const * p_pFormat , ... )
	{
		if ( p_pFormat )
		{
			wchar_t l_pText[256];
			va_list l_vaList;
			va_start( l_vaList, p_pFormat );
			Vswprintf( l_pText, p_pFormat, l_vaList );
			va_end( l_vaList );
			LogInfo( std::wstring( l_pText ) );
		}
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

	void Logger::LogWarning( char const * p_pFormat, ... )
	{
		if ( p_pFormat )
		{
			char l_pText[256];
			va_list l_vaList;
			va_start( l_vaList, p_pFormat );
			Vsnprintf( l_pText, p_pFormat, l_vaList );
			va_end( l_vaList );
			LogWarning( std::string( l_pText ) );
		}
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

	void Logger::LogWarning( wchar_t const * p_pFormat , ... )
	{
		if ( p_pFormat )
		{
			wchar_t l_pText[256];
			va_list l_vaList;
			va_start( l_vaList, p_pFormat );
			Vswprintf( l_pText, p_pFormat, l_vaList );
			va_end( l_vaList );
			LogWarning( std::wstring( l_pText ) );
		}
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

	void Logger::LogError( char const * p_pFormat, ... )
	{
		if ( p_pFormat )
		{
			char l_pText[256];
			va_list l_vaList;
			va_start( l_vaList, p_pFormat );
			Vsnprintf( l_pText, p_pFormat, l_vaList );
			va_end( l_vaList );
			LogError( std::string( l_pText ) );
		}
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

	void Logger::LogError( wchar_t const * p_pFormat , ... )
	{
		if ( p_pFormat )
		{
			wchar_t l_pText[256];
			va_list l_vaList;
			va_start( l_vaList, p_pFormat );
			Vswprintf( l_pText, p_pFormat, l_vaList );
			va_end( l_vaList );
			LogError( std::wstring( l_pText ) );
		}
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
			m_singleton = new Logger();
		}

		return *m_singleton;
	}

	Logger * Logger::GetSingletonPtr()
	{
		return &GetSingleton();
	}

	void Logger::DoRegisterCallback( LogCallback p_pfnCallback, void * p_pCaller )
	{
		m_impl->RegisterCallback( p_pfnCallback, p_pCaller );
	}

	void Logger::DoUnregisterCallback( void * p_pCaller )
	{
		m_impl->UnregisterCallback( p_pCaller );
	}

	void Logger::DoSetFileName( String const & logFilePath, ELogType logLevel )
	{
		std::unique_lock< std::mutex > lock( m_mutex );
		m_impl->SetFileName( logFilePath, logLevel );
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
			while ( !m_stopped )
			{
				DoFlushQueue();
				std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
			}

			DoFlushQueue();
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
