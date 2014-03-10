#include "CastorUtils/PrecompiledHeader.hpp"

#include "CastorUtils/File.hpp"

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

// Logger.hpp MUST be the last include
#include "CastorUtils/Logger.hpp"

using namespace Castor;

#if defined( _MSC_VER )
#	pragma warning( push )
#	pragma warning( disable:4290 )
#endif

// We remove that define because Logger initialises the MemoryManager instance
#undef new

//*************************************************************************************************

#if defined( _MSC_VER )
class MsvcConsoleInfo : public IConsoleInfo
{
private:
	uint32_t				m_uiOldCP;
	HANDLE					m_hScreenBuffer;
	PCONSOLE_FONT_INFOEX	m_pOldInfos;
	bool					m_bAllocated;
	bool					m_bConsole;

public:
	MsvcConsoleInfo()
		:	m_uiOldCP			( 0						)
		,	m_hScreenBuffer		( INVALID_HANDLE_VALUE	)
		,	m_pOldInfos			( NULL					)
		,	m_bAllocated		( false					)
		,	m_bConsole			( false					)
	{
		if( ::AllocConsole() )
		{
			m_bAllocated = true;
			DoInitialiseConsole();
		}
		else
		{
			DWORD l_dwLastError = ::GetLastError();

			if( l_dwLastError == ERROR_ACCESS_DENIED )
			{
				DoInitialiseConsole();
			}
			else
			{
				std::cout << "Failed to create to a new console with error " << l_dwLastError << std::endl;
			}
		}
	}

	virtual ~MsvcConsoleInfo()
	{
		if( m_pOldInfos )
		{
			::SetCurrentConsoleFontEx( m_hScreenBuffer, FALSE, m_pOldInfos );
			delete m_pOldInfos;
		}

		if( m_hScreenBuffer != INVALID_HANDLE_VALUE )
		{
			::CloseHandle( m_hScreenBuffer );
		}

		if( m_bConsole )
		{
			::SetConsoleOutputCP( m_uiOldCP );
		}

		if( m_bAllocated )
		{
			::FreeConsole();
		}
	}

	void BeginLog( eLOG_TYPE p_eLogType )
	{
		WORD l_wAttributes;

		switch( p_eLogType )
		{
		case eLOG_TYPE_DEBUG	:	l_wAttributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;					break;
		case eLOG_TYPE_WARNING	:	l_wAttributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;					break;
		case eLOG_TYPE_ERROR	:	l_wAttributes = FOREGROUND_RED | FOREGROUND_INTENSITY;										break;
		default:					l_wAttributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;	break;
		}

		::SetConsoleTextAttribute( m_hScreenBuffer, l_wAttributes );
	}

	void Print( String const & p_strToLog, bool p_bNewLine )
	{
		::OutputDebugString( p_strToLog.c_str() );
		DWORD l_dwWritten = 0;

		if( p_bNewLine )
		{
			::OutputDebugString( cuT( "\n" ) );
			CONSOLE_SCREEN_BUFFER_INFO l_csbiInfo;

			if( ::GetConsoleScreenBufferInfo( m_hScreenBuffer, &l_csbiInfo ) )
			{
				l_csbiInfo.dwCursorPosition.X = 0;
				::WriteConsole( m_hScreenBuffer, p_strToLog.c_str(), DWORD( p_strToLog.size() ), &l_dwWritten, NULL );
				SHORT l_sOffsetY = SHORT( 1 + l_dwWritten / l_csbiInfo.dwSize.X );

				if( (l_csbiInfo.dwSize.Y - l_sOffsetY) <= l_csbiInfo.dwCursorPosition.Y )
				{
					// The cursor is on the last row
					SMALL_RECT l_srctScrollRect;
					CHAR_INFO l_chiFill;
					COORD l_coordDest;
					// The scroll rectangle is from second row to last displayed row
					l_srctScrollRect.Top = 1;
					l_srctScrollRect.Bottom = l_csbiInfo.dwSize.Y - 1;
					l_srctScrollRect.Left = 0;
					l_srctScrollRect.Right = l_csbiInfo.dwSize.X - 1;
					// The destination for the scroll rectangle is one row up.
					l_coordDest.X = 0;
					l_coordDest.Y = 0;
					// Set the fill character and attributes.
					l_chiFill.Attributes = 0;
					l_chiFill.Char.AsciiChar = char( ' ' );
					// Scroll
					::ScrollConsoleScreenBuffer( m_hScreenBuffer, &l_srctScrollRect, NULL, l_coordDest, &l_chiFill );
				}
				else
				{
					// The cursor isn't on the last row
					l_csbiInfo.dwCursorPosition.Y += l_sOffsetY;
				}

				::SetConsoleCursorPosition( m_hScreenBuffer, l_csbiInfo.dwCursorPosition );
			}
		}
		else
		{
			::WriteConsole( m_hScreenBuffer, p_strToLog.c_str(), DWORD( p_strToLog.size() ), &l_dwWritten, NULL );
		}
	}

private:
	void DoInitialiseConsole()
	{
		m_hScreenBuffer = ::CreateConsoleScreenBuffer( GENERIC_WRITE | GENERIC_READ, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL );

		if( m_hScreenBuffer != INVALID_HANDLE_VALUE && ::SetConsoleActiveScreenBuffer( m_hScreenBuffer ) )
		{
			m_pOldInfos = new CONSOLE_FONT_INFOEX;
			PCONSOLE_FONT_INFOEX l_pOldInfos = m_pOldInfos;
			l_pOldInfos->cbSize = sizeof( CONSOLE_FONT_INFOEX );

			if( ::GetCurrentConsoleFontEx( m_hScreenBuffer, FALSE, l_pOldInfos ) )
			{
				CONSOLE_FONT_INFOEX l_infos = { 0 };
				l_infos.cbSize = sizeof( CONSOLE_FONT_INFOEX );
				l_infos.dwFontSize = l_pOldInfos->dwFontSize;
				l_infos.FontWeight = l_pOldInfos->FontWeight;
				l_infos.nFont = 6;
				l_infos.FontFamily = 54;
				_tcscpy( l_infos.FaceName, _T( "Lucida Console" ) );

				if( !::SetCurrentConsoleFontEx( m_hScreenBuffer, FALSE, &l_infos ) )
				{
					delete m_pOldInfos;
					m_pOldInfos = NULL;
				}
			}
			else
			{
				delete m_pOldInfos;
				m_pOldInfos = NULL;
			}
		}

		m_uiOldCP = ::GetConsoleOutputCP();
		::EnumSystemCodePages( & DoCodePageProc, CP_INSTALLED );

		FILE * l_dump;
		freopen_s( & l_dump, "conout$", "w", stdout );
		freopen_s( & l_dump, "conout$", "w", stderr );
		m_bConsole = true;
	}
	static BOOL __stdcall DoCodePageProc( xchar * pszCodePageString )
	{
		BOOL l_bReturn = TRUE;
		String l_strCP( pszCodePageString );
		int l_iCP = str_utils::to_int( l_strCP );

		if( l_iCP == CP_UTF8 )
		{
			::SetConsoleCP( l_iCP );
			::SetConsoleOutputCP( l_iCP );
			l_bReturn = FALSE;
		}

		return l_bReturn;
	}
};
#elif defined( _WIN32 )
class MswConsoleInfo : public IConsoleInfo
{
private:
	uint32_t		m_uiOldCP;
	HANDLE				m_hScreenBuffer;

public:
	MswConsoleInfo()
		:	m_uiOldCP			( 0						)
		,	m_hScreenBuffer		( INVALID_HANDLE_VALUE	)
	{
		if( ::AllocConsole() )
		{
			m_hScreenBuffer = ::CreateConsoleScreenBuffer( GENERIC_WRITE | GENERIC_READ, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL );

			if( m_hScreenBuffer != INVALID_HANDLE_VALUE )
			{
				::SetConsoleActiveScreenBuffer( m_hScreenBuffer );
			}

			m_uiOldCP = ::GetConsoleOutputCP();
			::EnumSystemCodePages( & DoCodePageProc, CP_INSTALLED );

			freopen( "conout$", "w", stdout );
			freopen( "conout$", "w", stderr );
		}
	}

	virtual ~MswConsoleInfo()
	{
		::CloseHandle( m_hScreenBuffer );
		::SetConsoleOutputCP( m_uiOldCP );
		::FreeConsole();
	}

	void BeginLog( eLOG_TYPE p_eLogType )
	{
		WORD l_wAttributes;

		switch( p_eLogType )
		{
		case eLOG_TYPE_DEBUG	:	l_wAttributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;										break;
		case eLOG_TYPE_WARNING	:	l_wAttributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;					break;
		case eLOG_TYPE_ERROR	:	l_wAttributes = FOREGROUND_RED | FOREGROUND_INTENSITY;										break;
		default:					l_wAttributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;	break;
		}

		::SetConsoleTextAttribute( m_hScreenBuffer, l_wAttributes );
	}

	void Print( String const & p_strToLog, bool p_bNewLine )
	{
		DWORD l_dwWritten;
		::WriteConsole( m_hScreenBuffer, p_strToLog.c_str(), p_strToLog.size(), & l_dwWritten, NULL );

		if( p_bNewLine )
		{
			CONSOLE_SCREEN_BUFFER_INFO l_csbiInfo;

			if( ::GetConsoleScreenBufferInfo( m_hScreenBuffer, & l_csbiInfo ) )
			{
				l_csbiInfo.dwCursorPosition.X = 0;

				if( (l_csbiInfo.dwSize.Y-1) == l_csbiInfo.dwCursorPosition.Y )
				{
					SMALL_RECT l_srctScrollRect, l_srctClipRect;
					CHAR_INFO l_chiFill;
					COORD l_coordDest;

					l_srctScrollRect.Left = 0;
					l_srctScrollRect.Top = 1;
					l_srctScrollRect.Right = l_csbiInfo.dwSize.X - SHORT( 1 );
					l_srctScrollRect.Bottom = l_csbiInfo.dwSize.Y - SHORT( 1 );
					// The destination for the scroll rectangle is one row up.
					l_coordDest.X = 0;
					l_coordDest.Y = 0;
					// The clipping rectangle is the same as the scrolling rectangle.
					// The destination row is left unchanged.
					l_srctClipRect = l_srctScrollRect;
					// Set the fill character and attributes.
					l_chiFill.Attributes = 0;
					l_chiFill.Char.AsciiChar = char( ' ' );
					// Scroll up one line.
					::ScrollConsoleScreenBuffer(  m_hScreenBuffer, & l_srctScrollRect, & l_srctClipRect, l_coordDest, & l_chiFill );
				}
				else
				{
					l_csbiInfo.dwCursorPosition.Y += 1;
				}

				::SetConsoleCursorPosition( m_hScreenBuffer, l_csbiInfo.dwCursorPosition );
			}
		}
	}

private:
	static BOOL __stdcall DoCodePageProc( xchar * pszCodePageString )
	{
		BOOL l_bReturn = TRUE;
		String l_strCP( pszCodePageString );
		int l_iCP = str_utils::to_int( l_strCP );

		if( l_iCP == CP_UTF8 )
		{
			::SetConsoleCP( l_iCP );
			::SetConsoleOutputCP( l_iCP );
			l_bReturn = FALSE;
		}

		return l_bReturn;
	}
};
#else
class GenConsoleInfo : public IConsoleInfo
{
private:
public:
	GenConsoleInfo()
	{
	}

	virtual ~GenConsoleInfo()
	{
	}

	void BeginLog( eLOG_TYPE p_eLogType )
	{
	}

	void Print( String const & p_strToLog, bool p_bNewLine )
	{
		Cout( p_strToLog );

		if( p_bNewLine )
		{
			Cout( std::endl );
		}
	}
};
#endif

//*************************************************************************************************

DebugConsole :: DebugConsole()
#if defined( _MSC_VER )
	:	m_pConsoleInfo	( new MsvcConsoleInfo )
#elif defined( _WIN32 )
	:	m_pConsoleInfo	( new MswConsoleInfo )
#else
	:	m_pConsoleInfo	( new GenConsoleInfo )
#endif
{

}

DebugConsole :: ~DebugConsole()
{
	delete m_pConsoleInfo;
}

void DebugConsole :: BeginLog( eLOG_TYPE p_eLogType )
{
	m_pConsoleInfo->BeginLog( p_eLogType );
}

void DebugConsole :: Print( String const & p_strToLog, bool p_bNewLine )
{
	m_pConsoleInfo->Print( p_strToLog, p_bNewLine );
}

//*************************************************************************************************

ILoggerImpl :: ILoggerImpl( eLOG_TYPE p_eLogLevel )
	:	m_eLogLevel		( p_eLogLevel	)
	,	m_pConsole		( NULL			)
{
#if defined( NDEBUG )
	if( p_eLogLevel == eLOG_TYPE_DEBUG )
	{
		m_pConsole = new DebugConsole;
	}
	else
	{
		m_pConsole = new DummyConsole;
	}
#else
	m_pConsole = new DebugConsole;
#endif
}

ILoggerImpl :: ~ILoggerImpl()
{
	delete m_pConsole;
}

void ILoggerImpl :: Initialise( Logger * p_pLogger )
{
	for( int i = 0; i < eLOG_TYPE_COUNT; i++ )
	{
		m_strHeaders[i]		= p_pLogger->m_strHeaders[i];
	}
}

void ILoggerImpl :: SetCallback( PLogCallback p_pfnCallback, void * p_pCaller )
{
	if( p_pfnCallback )
	{
		stLOGGER_CALLBACK l_callback = { p_pfnCallback, p_pCaller };
		m_mapCallbacks[std::this_thread::get_id()] = l_callback;
	}
	else
	{
		LoggerCallbackMapIt l_it = m_mapCallbacks.find( std::this_thread::get_id() );

		if( l_it != m_mapCallbacks.end() )
		{
			m_mapCallbacks.erase( l_it );
		}
	}
}

void ILoggerImpl :: SetFileName( String const & p_logFilePath, eLOG_TYPE p_eLogType )
{
	if( p_eLogType == eLOG_TYPE_COUNT )
	{
		for( int i = 0; i < eLOG_TYPE_COUNT; i++ )
		{
			m_logFilePath[i] = p_logFilePath;
		}
	}
	else
	{
		m_logFilePath[p_eLogType] = p_logFilePath;
	}

	FILE * l_pFile;
	Castor::FOpen( l_pFile, str_utils::to_str( p_logFilePath ).c_str(), "w" );

	if( l_pFile )
	{
		fclose( l_pFile);
	}
}

void ILoggerImpl :: LogDebug( String const & p_strToLog )
{
	DoLogMessage( p_strToLog,  eLOG_TYPE_DEBUG );
}

void ILoggerImpl :: LogMessage( String const & p_strToLog )
{
	DoLogMessage( p_strToLog,  eLOG_TYPE_MESSAGE );
}

void ILoggerImpl :: LogWarning( String const & p_strToLog )
{
	DoLogMessage( p_strToLog,  eLOG_TYPE_WARNING );
}

bool ILoggerImpl :: LogError( String const & p_strToLog )
{
	DoLogMessage( p_strToLog,  eLOG_TYPE_ERROR );
	return true;
}

void ILoggerImpl :: DoLogMessage( String const & p_strToLog, eLOG_TYPE p_eLogType )
{
	CASTOR_MUTEX_AUTO_SCOPED_LOCK();
	StringStream l_strToLog;
	LoggerCallbackMapConstIt l_it;
	struct tm * l_dtToday = NULL;
	time_t l_tTime;

	time( &l_tTime );
	Castor::Localtime( l_dtToday, &l_tTime );

	l_strToLog << (l_dtToday->tm_year + 1900) << cuT( "-" );
	l_strToLog << (l_dtToday->tm_mon + 1 < 10 ? cuT( "0" ) : cuT( "" )) << (l_dtToday->tm_mon + 1) << cuT( "-" ) << (l_dtToday->tm_mday < 10 ? cuT( "0" ) : cuT( "" )) << l_dtToday->tm_mday;
	l_strToLog << cuT( " - " ) << (l_dtToday->tm_hour < 10 ? cuT( "0" ) : cuT( "" )) << l_dtToday->tm_hour << cuT( ":" ) << (l_dtToday->tm_min < 10 ? cuT( "0" ) : cuT( "" )) << l_dtToday->tm_min << cuT( ":" ) << (l_dtToday->tm_sec < 10 ? cuT( "0" ) : cuT( "" )) << l_dtToday->tm_sec << cuT( "s" );
	l_strToLog << cuT( " - " ) << m_strHeaders[p_eLogType];

	l_strToLog << p_strToLog;

	m_pConsole->BeginLog( p_eLogType );
	m_pConsole->Print( p_strToLog, true );

	try
	{
		TextFile l_logFile( m_logFilePath[p_eLogType], File::eOPEN_MODE_APPEND, File::eENCODING_MODE_ASCII );

		if( l_logFile.IsOk() )
		{
			String l_strLog = l_strToLog.str();
			l_logFile.WriteText( l_strLog );
			l_logFile.Print( 2 * sizeof( xchar ), cuT( "\n" ) );
			l_it = m_mapCallbacks.find( std::this_thread::get_id() );

			if( l_it != m_mapCallbacks.end() && l_it->second.m_pfnCallback )
			{
				l_it->second.m_pfnCallback( l_it->second.m_pCaller, l_strLog, p_eLogType );
			}
		}
	}
	catch( std::exception & exc )
	{
		m_pConsole->Print( cuT( "Couldn't open log file : " ) + str_utils::from_str( exc.what() ), true );
	}
}

//*************************************************************************************************

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

	virtual void LogDebug	( String const & CU_PARAM_UNUSED( p_strToLog ) ){}
};

template <> class LeveledLogger< eLOG_TYPE_WARNING > : public ILoggerImpl
{
public:
	LeveledLogger()
		:	ILoggerImpl( eLOG_TYPE_WARNING )
	{
	}

	virtual void LogDebug	( String const & CU_PARAM_UNUSED( p_strToLog ) ){}
	virtual void LogMessage	( String const & CU_PARAM_UNUSED( p_strToLog ) ){}
};

template <> class LeveledLogger< eLOG_TYPE_ERROR > : public ILoggerImpl
{
public:
	LeveledLogger()
		:	ILoggerImpl( eLOG_TYPE_ERROR )
	{
	}

	virtual void LogDebug	( String const & CU_PARAM_UNUSED( p_strToLog ) ){}
	virtual void LogMessage	( String const & CU_PARAM_UNUSED( p_strToLog ) ){}
	virtual void LogWarning	( String const & CU_PARAM_UNUSED( p_strToLog ) ){}
};

//*************************************************************************************************

Logger *	Logger :: m_pSingleton		= NULL;
bool		Logger :: m_bOwnInstance	= true;
uint32_t	Logger :: m_uiCounter		= 0;

Logger :: Logger()
	:	m_pImpl	( NULL	)
	INITIALISE_MEMORY_MANAGER( m_pMemoryManager )
{
	CASTOR_MUTEX_AUTO_SCOPED_LOCK();
	m_strHeaders[eLOG_TYPE_DEBUG	] = cuT( "***DEBUG*** "		);
	m_strHeaders[eLOG_TYPE_MESSAGE	] = cuEmptyString;
	m_strHeaders[eLOG_TYPE_WARNING	] = cuT( "***WARNING*** "	);
	m_strHeaders[eLOG_TYPE_ERROR	] = cuT( "***ERROR*** "		);
}

Logger :: ~Logger()
{
	if( m_bOwnInstance )
	{
		RELEASE_MEMORY_MANAGER( m_pMemoryManager );
		delete m_pImpl;
	}
}

void Logger :: Initialise( Logger * p_pLogger )
{
	m_uiCounter++;
#if defined( _WIN32 )
	if( m_pSingleton )
	{
		throw "Logger instance already initialised";
	}
	else
	{
		m_bOwnInstance = false;
		Logger & l_logger = GetSingleton();
		delete l_logger.m_pImpl;

		l_logger.m_pImpl = p_pLogger->m_pImpl;
		SET_MEMORY_MANAGER_FROM( l_logger.m_pMemoryManager, p_pLogger->m_pMemoryManager );

		for( int i = 0; i < eLOG_TYPE_COUNT; i++ )
		{
			l_logger.m_strHeaders[i] = p_pLogger->m_strHeaders[i];
		}
	}
#endif
}

void Logger :: Initialise( eLOG_TYPE p_eLogLevel )
{
	m_uiCounter++;
	if( m_pSingleton )
	{
		throw "Logger instance already initialised";
	}
	else
	{
		m_bOwnInstance = true;

		Logger & l_logger = GetSingleton();
		delete l_logger.m_pImpl;
		l_logger.m_pImpl = NULL;

		switch( p_eLogLevel )
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

		if( l_logger.m_pImpl )
		{
			SET_MEMORY_MANAGER( l_logger.m_pMemoryManager );
			l_logger.m_pImpl->Initialise( &l_logger );
		}
		else
		{
			throw std::range_error( "SetLogLevel subscript error" );
		}
	}
}

void Logger :: Cleanup()
{
	m_uiCounter--;
#if !defined( _WIN32 )
	if( m_uiCounter <= 0 )
	{
		delete m_pSingleton;
		m_pSingleton = NULL;
	}
#else
	delete m_pSingleton;
	m_pSingleton = NULL;
#endif
}

void Logger :: SetCallback( PLogCallback p_pfnCallback, void * p_pCaller )
{
	GetSingleton().DoSetCallback( p_pfnCallback, p_pCaller );
}

void Logger :: SetFileName( String const & p_logFilePath, eLOG_TYPE p_eLogType )
{
	if( GetSingleton().m_pImpl )
	{
		GetSingleton().DoSetFileName( p_logFilePath, p_eLogType );
	}
}

void Logger :: LogDebug( char const * p_pFormat, ... )
{
	if( p_pFormat )
	{
		char l_pText[256];
		va_list l_vaList;
		va_start( l_vaList, p_pFormat );
		std::string l_strFormat( p_pFormat );
		vsnprintf( l_pText, 256, l_strFormat.c_str(), l_vaList );
		va_end( l_vaList );

		if( GetSingleton().m_pImpl )
		{
			GetSingleton().m_pImpl->LogDebug( str_utils::from_str( l_pText ) );
		}
	}
}

void Logger :: LogDebug( std::string const & p_msg )
{
	if( GetSingleton().m_pImpl )
	{
		GetSingleton().m_pImpl->LogDebug( str_utils::from_str( p_msg ) );
	}
}

void Logger :: LogDebug( wchar_t const * p_pFormat , ... )
{
	if( p_pFormat )
	{
		wchar_t l_pText[256];
		va_list l_vaList;
		va_start( l_vaList, p_pFormat );
		std::wstring l_strFormat( p_pFormat );
#if defined( _MSC_VER ) || !defined( _WIN32 )
		vswprintf( l_pText, 256, l_strFormat.c_str(), l_vaList );
#else
		vswprintf( l_pText, l_strFormat.c_str(), l_vaList );
#endif
		va_end( l_vaList );

		if( GetSingleton().m_pImpl )
		{
			GetSingleton().m_pImpl->LogDebug( str_utils::from_wstr( l_pText ) );
		}
	}
}

void Logger :: LogDebug( std::wstring const & p_msg )
{
	if( GetSingleton().m_pImpl )
	{
		GetSingleton().m_pImpl->LogDebug( str_utils::from_wstr( p_msg ) );
	}
}

void Logger :: LogMessage( char const * p_pFormat, ... )
{
	if( p_pFormat )
	{
		char l_pText[256];
		va_list l_vaList;
		va_start( l_vaList, p_pFormat );
		std::string l_strFormat( p_pFormat );
		vsnprintf( l_pText, 256, l_strFormat.c_str(), l_vaList );
		va_end( l_vaList );

		if( GetSingleton().m_pImpl )
		{
			GetSingleton().m_pImpl->LogMessage( str_utils::from_str( l_pText ) );
		}
	}
}

void Logger :: LogMessage( std::string const & p_msg )
{
	if( GetSingleton().m_pImpl )
	{
		GetSingleton().m_pImpl->LogMessage( str_utils::from_str( p_msg ) );
	}
}

void Logger :: LogMessage( wchar_t const * p_pFormat , ... )
{
	if( p_pFormat )
	{
		wchar_t l_pText[256];
		va_list l_vaList;
		va_start( l_vaList, p_pFormat );
		std::wstring l_strFormat( p_pFormat );
#if defined( _MSC_VER ) || !defined( _WIN32 )
		vswprintf( l_pText, 256, l_strFormat.c_str(), l_vaList );
#else
		vswprintf( l_pText, l_strFormat.c_str(), l_vaList );
#endif
		va_end( l_vaList );

		if( GetSingleton().m_pImpl )
		{
			GetSingleton().m_pImpl->LogMessage( str_utils::from_wstr( l_pText ) );
		}
	}
}

void Logger :: LogMessage( std::wstring const & p_msg )
{
	if( GetSingleton().m_pImpl )
	{
		GetSingleton().m_pImpl->LogMessage( str_utils::from_wstr( p_msg ) );
	}
}

void Logger :: LogWarning( char const * p_pFormat, ... )
{
	if( p_pFormat )
	{
		char l_pText[256];
		va_list l_vaList;
		va_start( l_vaList, p_pFormat );
		std::string l_strFormat( p_pFormat );
		vsnprintf( l_pText, 256, l_strFormat.c_str(), l_vaList );
		va_end( l_vaList );

		if( GetSingleton().m_pImpl )
		{
			GetSingleton().m_pImpl->LogWarning( str_utils::from_str( l_pText ) );
		}
	}
}

void Logger :: LogWarning( std::string const & p_msg )
{
	if( GetSingleton().m_pImpl )
	{
		GetSingleton().m_pImpl->LogWarning( str_utils::from_str( p_msg ) );
	}
}

void Logger :: LogWarning( wchar_t const * p_pFormat , ... )
{
	if( p_pFormat )
	{
		wchar_t l_pText[256];
		va_list l_vaList;
		va_start( l_vaList, p_pFormat );
		std::wstring l_strFormat( p_pFormat );
#if defined( _MSC_VER ) || !defined( _WIN32 )
		vswprintf( l_pText, 256, l_strFormat.c_str(), l_vaList );
#else
		vswprintf( l_pText, l_strFormat.c_str(), l_vaList );
#endif
		va_end( l_vaList );

		if( GetSingleton().m_pImpl )
		{
			GetSingleton().m_pImpl->LogWarning( str_utils::from_wstr( l_pText ) );
		}
	}
}

void Logger :: LogWarning( std::wstring const & p_msg )
{
	if( GetSingleton().m_pImpl )
	{
		GetSingleton().m_pImpl->LogWarning( str_utils::from_wstr( p_msg ) );
	}
}


void Logger :: LogError( char const * p_pFormat, ... )
{
	if( p_pFormat )
	{
		char l_pText[256];
		va_list l_vaList;
		va_start( l_vaList, p_pFormat );
		std::string l_strFormat( p_pFormat );
		vsnprintf( l_pText, 256, l_strFormat.c_str(), l_vaList );
		va_end( l_vaList );

		if( GetSingleton().m_pImpl )
		{
			GetSingleton().m_pImpl->LogError( str_utils::from_str( l_pText ) );
		}
	}
}

void Logger :: LogError( std::string const & p_msg, bool p_bThrow )throw( bool )
{
	if( GetSingleton().m_pImpl->LogError( str_utils::from_str( p_msg ) ) )
	{
		if( p_bThrow )
		{
			throw false;
		}
	}
}

void Logger :: LogError( wchar_t const * p_pFormat , ... )
{
	if( p_pFormat )
	{
		wchar_t l_pText[256];
		va_list l_vaList;
		va_start( l_vaList, p_pFormat );
		std::wstring l_strFormat( p_pFormat );
#if defined( _MSC_VER ) || !defined( _WIN32 )
		vswprintf( l_pText, 256, l_strFormat.c_str(), l_vaList );
#else
		vswprintf( l_pText, l_strFormat.c_str(), l_vaList );
#endif
		va_end( l_vaList );

		if( GetSingleton().m_pImpl )
		{
			GetSingleton().m_pImpl->LogError( str_utils::from_wstr( l_pText ) );
		}
	}
}

void Logger :: LogError( std::wstring const & p_msg, bool p_bThrow )throw( bool )
{
	if( GetSingleton().m_pImpl->LogError( str_utils::from_wstr( p_msg ) ) )
	{
		if( p_bThrow )
		{
			throw false;
		}
	}
}

Logger & Logger :: GetSingleton()
{
	if( !m_pSingleton )
	{
		m_pSingleton = new Logger();
	}

	return *m_pSingleton;
}

Logger * Logger :: GetSingletonPtr()
{
	return &GetSingleton();
}

void Logger :: DoSetCallback( PLogCallback p_pfnCallback, void * p_pCaller )
{
	m_pImpl->SetCallback( p_pfnCallback, p_pCaller );
}

void Logger :: DoSetFileName( String const & p_logFilePath, eLOG_TYPE p_eLogType )
{
	CASTOR_MUTEX_AUTO_SCOPED_LOCK();
	m_pImpl->SetFileName( p_logFilePath, p_eLogType );
}

//*************************************************************************************************

#if defined( _MSC_VER )
#	pragma warning( pop )
#endif
