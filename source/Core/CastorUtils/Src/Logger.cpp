#include "File.hpp"
#include "LoggerImpl.hpp"

#if defined _WIN32
#	if defined( _MSC_VER )
#		pragma warning( push )
#		pragma warning( disable:4311 )
#		pragma warning( disable:4312 )
#	endif
#	include <windows.h>
#	if defined( _MSC_VER )
#		pragma warning( pop )
#	endif
#endif

#include "Logger.hpp"

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
	template< eLOG_TYPE Level >	class LeveledLogger;

	template <> class LeveledLogger< eLOG_TYPE_DEBUG > : public ILoggerImpl
	{
	public:
		LeveledLogger()
			:	ILoggerImpl( eLOG_TYPE_DEBUG )
		{
		}
	};

	template <> class LeveledLogger< eLOG_TYPE_MESSAGE > : public ILoggerImpl
	{
	public:
		LeveledLogger()
			:	ILoggerImpl( eLOG_TYPE_MESSAGE )
		{
		}

		virtual void LogDebug( String const & CU_PARAM_UNUSED( p_strToLog ) ) {}
	};

	template <> class LeveledLogger< eLOG_TYPE_WARNING > : public ILoggerImpl
	{
	public:
		LeveledLogger()
			:	ILoggerImpl( eLOG_TYPE_WARNING )
		{
		}

		virtual void LogDebug( String const & CU_PARAM_UNUSED( p_strToLog ) ) {}
		virtual void LogMessage( String const & CU_PARAM_UNUSED( p_strToLog ) ) {}
	};

	template <> class LeveledLogger< eLOG_TYPE_ERROR > : public ILoggerImpl
	{
	public:
		LeveledLogger()
			:	ILoggerImpl( eLOG_TYPE_ERROR )
		{
		}

		virtual void LogDebug( String const & CU_PARAM_UNUSED( p_strToLog ) ) {}
		virtual void LogMessage( String const & CU_PARAM_UNUSED( p_strToLog ) ) {}
		virtual void LogWarning( String const & CU_PARAM_UNUSED( p_strToLog ) ) {}
	};

	Logger * Logger::m_pSingleton = NULL;
	bool Logger::m_bOwnInstance = true;
	uint32_t Logger::m_uiCounter = 0;
	std::mutex Logger::m_mutex;

	Logger::Logger()
		:	m_pImpl( NULL	)
	{
		CASTOR_MUTEX_AUTO_SCOPED_LOCK();
		m_strHeaders[eLOG_TYPE_DEBUG	] = cuT( "***DEBUG*** "	);
		m_strHeaders[eLOG_TYPE_MESSAGE	] = cuEmptyString;
		m_strHeaders[eLOG_TYPE_WARNING	] = cuT( "***WARNING*** "	);
		m_strHeaders[eLOG_TYPE_ERROR	] = cuT( "***ERROR*** "	);
	}

	Logger::~Logger()
	{
		CASTOR_MUTEX_AUTO_SCOPED_LOCK();

		if ( m_bOwnInstance )
		{
			delete m_pImpl;
			m_pImpl = NULL;
		}
	}

	void Logger::Initialise( Logger * p_pLogger )
	{
		m_uiCounter++;
#if defined( _WIN32 )

		if ( m_pSingleton )
		{
			m_uiCounter--;
			Logger::LogWarning( cuT( "Logger instance already initialised" ) );
		}
		else
		{
			Logger & l_logger = GetSingleton();
			CASTOR_MUTEX_AUTO_SCOPED_LOCK();
			m_bOwnInstance = false;
			delete l_logger.m_pImpl;
			l_logger.m_pImpl = p_pLogger->m_pImpl;

			for ( int i = 0; i < eLOG_TYPE_COUNT; i++ )
			{
				l_logger.m_strHeaders[i] = p_pLogger->m_strHeaders[i];
			}
		}

#endif
	}

	void Logger::Initialise( eLOG_TYPE p_eLogLevel )
	{
		m_uiCounter++;

		if ( m_pSingleton )
		{
			throw "Logger instance already initialised";
		}
		else
		{
			Logger & l_logger = GetSingleton();
			CASTOR_MUTEX_AUTO_SCOPED_LOCK();
			m_bOwnInstance = true;
			delete l_logger.m_pImpl;
			l_logger.m_pImpl = NULL;

			switch ( p_eLogLevel )
			{
			case eLOG_TYPE_DEBUG:
				l_logger.m_pImpl = new LeveledLogger< eLOG_TYPE_DEBUG >();
				break;

			case eLOG_TYPE_MESSAGE:
				l_logger.m_pImpl = new LeveledLogger< eLOG_TYPE_MESSAGE >();
				break;

			case eLOG_TYPE_WARNING:
				l_logger.m_pImpl = new LeveledLogger< eLOG_TYPE_WARNING >();
				break;

			case eLOG_TYPE_ERROR:
				l_logger.m_pImpl = new LeveledLogger< eLOG_TYPE_ERROR >();
				break;
			}

			if ( l_logger.m_pImpl )
			{
				l_logger.m_pImpl->Initialise( &l_logger );
			}
			else
			{
				throw std::range_error( "SetLogLevel subscript error" );
			}
		}
	}

	void Logger::Cleanup()
	{
		m_uiCounter--;
#if !defined( _WIN32 )

		if ( m_uiCounter <= 0 )
		{
			delete m_pSingleton;
			m_pSingleton = NULL;
		}

#else
		if ( m_bOwnInstance )
		{
			Logger & l_logger = GetSingleton();
			CASTOR_MUTEX_AUTO_SCOPED_LOCK();

			if ( l_logger.m_pImpl )
			{
				l_logger.m_pImpl->Cleanup();
			}
		}

		delete m_pSingleton;
		m_pSingleton = NULL;
#endif
	}

	void Logger::SetCallback( PLogCallback p_pfnCallback, void * p_pCaller )
	{
		GetSingleton().DoSetCallback( p_pfnCallback, p_pCaller );
	}

	void Logger::SetFileName( String const & p_logFilePath, eLOG_TYPE p_eLogType )
	{
		GetSingleton().DoSetFileName( p_logFilePath, p_eLogType );
	}

	void Logger::LogDebug( char const * p_pFormat, ... )
	{
		if ( p_pFormat )
		{
			char l_pText[256];
			va_list l_vaList;
			va_start( l_vaList, p_pFormat );
			std::string l_strFormat( p_pFormat );
			Vsnprintf( l_pText, l_strFormat.c_str(), l_vaList );
			va_end( l_vaList );

			CASTOR_MUTEX_AUTO_SCOPED_LOCK();

			if ( GetSingleton().m_pImpl )
			{
				GetSingleton().m_pImpl->LogDebug( str_utils::from_str( l_pText ) );
			}
		}
	}

	void Logger::LogDebug( std::string const & p_msg )
	{
		CASTOR_MUTEX_AUTO_SCOPED_LOCK();

		if ( GetSingleton().m_pImpl )
		{
			GetSingleton().m_pImpl->LogDebug( str_utils::from_str( p_msg ) );
		}
	}

	void Logger::LogDebug( wchar_t const * p_pFormat , ... )
	{
		if ( p_pFormat )
		{
			wchar_t l_pText[256];
			va_list l_vaList;
			va_start( l_vaList, p_pFormat );
			std::wstring l_strFormat( p_pFormat );
			Vswprintf( l_pText, l_strFormat.c_str(), l_vaList );
			va_end( l_vaList );

			CASTOR_MUTEX_AUTO_SCOPED_LOCK();

			if ( GetSingleton().m_pImpl )
			{
				GetSingleton().m_pImpl->LogDebug( str_utils::from_wstr( l_pText ) );
			}
		}
	}

	void Logger::LogDebug( std::wstring const & p_msg )
	{
		CASTOR_MUTEX_AUTO_SCOPED_LOCK();

		if ( GetSingleton().m_pImpl )
		{
			GetSingleton().m_pImpl->LogDebug( str_utils::from_wstr( p_msg ) );
		}
	}

	void Logger::LogMessage( char const * p_pFormat, ... )
	{
		if ( p_pFormat )
		{
			char l_pText[256];
			va_list l_vaList;
			va_start( l_vaList, p_pFormat );
			std::string l_strFormat( p_pFormat );
			Vsnprintf( l_pText, l_strFormat.c_str(), l_vaList );
			va_end( l_vaList );

			CASTOR_MUTEX_AUTO_SCOPED_LOCK();

			if ( GetSingleton().m_pImpl )
			{
				GetSingleton().m_pImpl->LogMessage( str_utils::from_str( l_pText ) );
			}
		}
	}

	void Logger::LogMessage( std::string const & p_msg )
	{
		CASTOR_MUTEX_AUTO_SCOPED_LOCK();

		if ( GetSingleton().m_pImpl )
		{
			GetSingleton().m_pImpl->LogMessage( str_utils::from_str( p_msg ) );
		}
	}

	void Logger::LogMessage( wchar_t const * p_pFormat , ... )
	{
		if ( p_pFormat )
		{
			wchar_t l_pText[256];
			va_list l_vaList;
			va_start( l_vaList, p_pFormat );
			std::wstring l_strFormat( p_pFormat );
			Vswprintf( l_pText, l_strFormat.c_str(), l_vaList );
			va_end( l_vaList );

			CASTOR_MUTEX_AUTO_SCOPED_LOCK();

			if ( GetSingleton().m_pImpl )
			{
				GetSingleton().m_pImpl->LogMessage( str_utils::from_wstr( l_pText ) );
			}
		}
	}

	void Logger::LogMessage( std::wstring const & p_msg )
	{
		CASTOR_MUTEX_AUTO_SCOPED_LOCK();

		if ( GetSingleton().m_pImpl )
		{
			GetSingleton().m_pImpl->LogMessage( str_utils::from_wstr( p_msg ) );
		}
	}

	void Logger::LogWarning( char const * p_pFormat, ... )
	{
		if ( p_pFormat )
		{
			char l_pText[256];
			va_list l_vaList;
			va_start( l_vaList, p_pFormat );
			std::string l_strFormat( p_pFormat );
			Vsnprintf( l_pText, l_strFormat.c_str(), l_vaList );
			va_end( l_vaList );

			CASTOR_MUTEX_AUTO_SCOPED_LOCK();

			if ( GetSingleton().m_pImpl )
			{
				GetSingleton().m_pImpl->LogWarning( str_utils::from_str( l_pText ) );
			}
		}
	}

	void Logger::LogWarning( std::string const & p_msg )
	{
		CASTOR_MUTEX_AUTO_SCOPED_LOCK();

		if ( GetSingleton().m_pImpl )
		{
			GetSingleton().m_pImpl->LogWarning( str_utils::from_str( p_msg ) );
		}
	}

	void Logger::LogWarning( wchar_t const * p_pFormat , ... )
	{
		if ( p_pFormat )
		{
			wchar_t l_pText[256];
			va_list l_vaList;
			va_start( l_vaList, p_pFormat );
			std::wstring l_strFormat( p_pFormat );
			Vswprintf( l_pText, l_strFormat.c_str(), l_vaList );
			va_end( l_vaList );

			CASTOR_MUTEX_AUTO_SCOPED_LOCK();

			if ( GetSingleton().m_pImpl )
			{
				GetSingleton().m_pImpl->LogWarning( str_utils::from_wstr( l_pText ) );
			}
		}
	}

	void Logger::LogWarning( std::wstring const & p_msg )
	{
		CASTOR_MUTEX_AUTO_SCOPED_LOCK();

		if ( GetSingleton().m_pImpl )
		{
			GetSingleton().m_pImpl->LogWarning( str_utils::from_wstr( p_msg ) );
		}
	}


	void Logger::LogError( char const * p_pFormat, ... )
	{
		if ( p_pFormat )
		{
			char l_pText[256];
			va_list l_vaList;
			va_start( l_vaList, p_pFormat );
			std::string l_strFormat( p_pFormat );
			Vsnprintf( l_pText, l_strFormat.c_str(), l_vaList );
			va_end( l_vaList );

			CASTOR_MUTEX_AUTO_SCOPED_LOCK();

			if ( GetSingleton().m_pImpl )
			{
				GetSingleton().m_pImpl->LogError( str_utils::from_str( l_pText ) );
			}
		}
	}

	void Logger::LogError( std::string const & p_msg )
	{
		CASTOR_MUTEX_AUTO_SCOPED_LOCK();

		if ( GetSingleton().m_pImpl )
		{
			GetSingleton().m_pImpl->LogError( str_utils::from_str( p_msg ) );
		}
	}

	void Logger::LogError( wchar_t const * p_pFormat , ... )
	{
		if ( p_pFormat )
		{
			wchar_t l_pText[256];
			va_list l_vaList;
			va_start( l_vaList, p_pFormat );
			std::wstring l_strFormat( p_pFormat );
			Vswprintf( l_pText, l_strFormat.c_str(), l_vaList );
			va_end( l_vaList );

			CASTOR_MUTEX_AUTO_SCOPED_LOCK();

			if ( GetSingleton().m_pImpl )
			{
				GetSingleton().m_pImpl->LogError( str_utils::from_wstr( l_pText ) );
			}
		}
	}

	void Logger::LogError( std::wstring const & p_msg )
	{
		CASTOR_MUTEX_AUTO_SCOPED_LOCK();

		if ( GetSingleton().m_pImpl )
		{
			GetSingleton().m_pImpl->LogError( str_utils::from_wstr( p_msg ) );
		}
	}

	Logger & Logger::GetSingleton()
	{
		if ( !m_pSingleton )
		{
			m_pSingleton = new Logger();
		}

		return *m_pSingleton;
	}

	Logger * Logger::GetSingletonPtr()
	{
		return &GetSingleton();
	}

	void Logger::DoSetCallback( PLogCallback p_pfnCallback, void * p_pCaller )
	{
		CASTOR_MUTEX_AUTO_SCOPED_LOCK();

		if ( m_pImpl )
		{
			m_pImpl->SetCallback( p_pfnCallback, p_pCaller );
		}
	}

	void Logger::DoSetFileName( String const & p_logFilePath, eLOG_TYPE p_eLogType )
	{
		CASTOR_MUTEX_AUTO_SCOPED_LOCK();

		if ( m_pImpl )
		{
			m_pImpl->SetFileName( p_logFilePath, p_eLogType );
		}
	}
}
