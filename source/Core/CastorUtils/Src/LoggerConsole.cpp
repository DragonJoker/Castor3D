#include "CastorUtilsPch.hpp"

#if defined( _MSC_VER )
#	pragma warning( push )
#	pragma warning( disable:4311 )
#	pragma warning( disable:4312 )
#	pragma warning( disable:4996 )
#endif

#if defined _WIN32
#	include <Windows.h>
#	include <tchar.h>
#endif

#include "LoggerConsole.hpp"
#include "StringUtils.hpp"

namespace Castor
{
#if defined( _MSC_VER )

	class CMsvcConsoleInfo
		: public IConsole
	{
	public:
		CMsvcConsoleInfo()
			: m_oldCodePage( 0 )
			, m_screenBuffer( INVALID_HANDLE_VALUE )
			, m_oldInfos( NULL )
			, m_allocated( false )
			, m_console( false )
		{
			if ( ::AllocConsole() )
			{
				m_allocated = true;
				DoInitialiseConsole();
			}
			else
			{
				DWORD lastError = ::GetLastError();

				if ( lastError == ERROR_ACCESS_DENIED )
				{
					DoInitialiseConsole();
				}
				else
				{
					std::cout << "Failed to create to a new console with error " << lastError << std::endl;
				}
			}
		}

		virtual ~CMsvcConsoleInfo()
		{
			if ( m_oldInfos )
			{
				::SetCurrentConsoleFontEx( m_screenBuffer, FALSE, m_oldInfos );
				delete m_oldInfos;
			}

			if ( m_screenBuffer != INVALID_HANDLE_VALUE )
			{
				::CloseHandle( m_screenBuffer );
			}

			if ( m_console )
			{
				::SetConsoleOutputCP( m_oldCodePage );
			}

			if ( m_allocated )
			{
				::FreeConsole();
			}
		}

		void BeginLog( ELogType logLevel )
		{
			WORD attributes;

			switch ( logLevel )
			{
			case ELogType_DEBUG:
				attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
				break;

			case ELogType_WARNING:
				attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
				break;

			case ELogType_ERROR:
				attributes = FOREGROUND_RED | FOREGROUND_INTENSITY;
				break;

			default:
				attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
				break;
			}

			::SetConsoleTextAttribute( m_screenBuffer, attributes );
		}

		void Print( String const & toLog, bool newLine )
		{
			::OutputDebugString( toLog.c_str() );
			DWORD written = 0;

			if ( newLine )
			{
				::OutputDebugString( cuT( "\n" ) );
				CONSOLE_SCREEN_BUFFER_INFO csbiInfo;

				if ( ::GetConsoleScreenBufferInfo( m_screenBuffer, &csbiInfo ) )
				{
					csbiInfo.dwCursorPosition.X = 0;
					::WriteConsole( m_screenBuffer, toLog.c_str(), DWORD( toLog.size() ), &written, NULL );
					SHORT offsetY = SHORT( 1 + written / csbiInfo.dwSize.X );

					if ( ( csbiInfo.dwSize.Y - offsetY ) <= csbiInfo.dwCursorPosition.Y )
					{
						// The cursor is on the last row
						// The scroll rectangle is from second row to last displayed row
						SMALL_RECT scrollRect;
						scrollRect.Top = 1;
						scrollRect.Bottom = csbiInfo.dwSize.Y - 1;
						scrollRect.Left = 0;
						scrollRect.Right = csbiInfo.dwSize.X - 1;
						// The destination for the scroll rectangle is one row up.
						COORD coordDest;
						coordDest.X = 0;
						coordDest.Y = 0;
						// Set the fill character and attributes.
						CHAR_INFO fill;
						fill.Attributes = 0;
						fill.Char.AsciiChar = char( ' ' );
						// Scroll
						::ScrollConsoleScreenBuffer( m_screenBuffer, &scrollRect, NULL, coordDest, &fill );
					}
					else
					{
						// The cursor isn't on the last row
						csbiInfo.dwCursorPosition.Y += offsetY;
					}

					::SetConsoleCursorPosition( m_screenBuffer, csbiInfo.dwCursorPosition );
				}
			}
			else
			{
				::WriteConsole( m_screenBuffer, toLog.c_str(), DWORD( toLog.size() ), &written, NULL );
			}
		}

	private:
		void DoInitialiseConsole()
		{
			m_screenBuffer = ::CreateConsoleScreenBuffer( GENERIC_WRITE | GENERIC_READ, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL );

			if ( m_screenBuffer != INVALID_HANDLE_VALUE && ::SetConsoleActiveScreenBuffer( m_screenBuffer ) )
			{
				m_oldInfos = new CONSOLE_FONT_INFOEX;
				CONSOLE_FONT_INFOEX * oldInfos = m_oldInfos;
				oldInfos->cbSize = sizeof( CONSOLE_FONT_INFOEX );

				if ( ::GetCurrentConsoleFontEx( m_screenBuffer, FALSE, oldInfos ) )
				{
					CONSOLE_FONT_INFOEX newInfos = { 0 };
					newInfos.cbSize = sizeof( CONSOLE_FONT_INFOEX );
					newInfos.dwFontSize = oldInfos->dwFontSize;
					newInfos.FontWeight = oldInfos->FontWeight;
					newInfos.nFont = 6;
					newInfos.FontFamily = 54;
					wcscpy( newInfos.FaceName, L"Lucida Console" );

					if ( !::SetCurrentConsoleFontEx( m_screenBuffer, FALSE, &newInfos ) )
					{
						delete m_oldInfos;
						m_oldInfos = NULL;
					}
				}
				else
				{
					delete m_oldInfos;
					m_oldInfos = NULL;
				}

				COORD coord = { 160, 9999 };
				::SetConsoleScreenBufferSize( m_screenBuffer, coord );
			}

			m_oldCodePage = ::GetConsoleOutputCP();
			::EnumSystemCodePages( &DoCodePageProc, CP_INSTALLED );

			FILE * dump;
			freopen_s( &dump, "conout$", "w", stdout );
			freopen_s( &dump, "conout$", "w", stderr );
			m_console = true;
		}
		static BOOL __stdcall DoCodePageProc( xchar * szCodePage )
		{
			BOOL ret = TRUE;
			String codePage( szCodePage );
			int cp = stoi( codePage );

			if ( cp == CP_UTF8 )
			{
				::SetConsoleCP( cp );
				::SetConsoleOutputCP( cp );
				ret = FALSE;
			}

			return ret;
		}

	private:
		uint32_t m_oldCodePage;
		HANDLE m_screenBuffer;
		CONSOLE_FONT_INFOEX * m_oldInfos;
		bool m_allocated;
		bool m_console;
	};

#elif defined( _WIN32 )

	class CMswConsoleInfo
		: public IConsole
	{
	public:
		CMswConsoleInfo()
			: m_oldCodePage( 0	)
			, m_screenBuffer( INVALID_HANDLE_VALUE	)
			, m_allocated( false	)
			, m_console( false	)
		{
			if ( ::AllocConsole() )
			{
				m_allocated = true;
				DoInitialiseConsole();
			}
			else
			{
				DWORD l_dwLastError = ::GetLastError();

				if ( l_dwLastError == ERROR_ACCESS_DENIED )
				{
					DoInitialiseConsole();
				}
				else
				{
					std::cout << "Failed to create to a new console with error " << l_dwLastError << std::endl;
				}
			}
		}

		virtual ~CMswConsoleInfo()
		{
			if ( m_screenBuffer != INVALID_HANDLE_VALUE )
			{
				::CloseHandle( m_screenBuffer );
			}

			if ( m_console )
			{
				::SetConsoleOutputCP( m_oldCodePage );
			}

			if ( m_allocated )
			{
				::FreeConsole();
			}
		}

		void BeginLog( ELogType p_eLogType )
		{
			WORD l_wAttributes;

			switch ( p_eLogType )
			{
			case ELogType_DEBUG	:
				l_wAttributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
				break;

			case ELogType_WARNING	:
				l_wAttributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
				break;

			case ELogType_ERROR	:
				l_wAttributes = FOREGROUND_RED | FOREGROUND_INTENSITY;
				break;

			default:
				l_wAttributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
				break;
			}

			::SetConsoleTextAttribute( m_screenBuffer, l_wAttributes );
		}

		void Print( String const & toLog, bool newLine )
		{
			::OutputDebugString( toLog.c_str() );
			DWORD written = 0;

			if ( newLine )
			{
				::OutputDebugString( cuT( "\n" ) );
				CONSOLE_SCREEN_BUFFER_INFO csbiInfo;

				if ( ::GetConsoleScreenBufferInfo( m_screenBuffer, &csbiInfo ) )
				{
					csbiInfo.dwCursorPosition.X = 0;
					::WriteConsole( m_screenBuffer, toLog.c_str(), DWORD( toLog.size() ), &written, NULL );
					SHORT offsetY = SHORT( 1 + written / csbiInfo.dwSize.X );

					if ( ( csbiInfo.dwSize.Y - offsetY ) <= csbiInfo.dwCursorPosition.Y )
					{
						// The cursor is on the last row
						// The scroll rectangle is from second row to last displayed row
						SMALL_RECT scrollRect;
						scrollRect.Top = 1;
						scrollRect.Bottom = csbiInfo.dwSize.Y - 1;
						scrollRect.Left = 0;
						scrollRect.Right = csbiInfo.dwSize.X - 1;
						// The destination for the scroll rectangle is one row up.
						COORD coordDest;
						coordDest.X = 0;
						coordDest.Y = 0;
						// Set the fill character and attributes.
						CHAR_INFO fill;
						fill.Attributes = 0;
						fill.Char.AsciiChar = char( ' ' );
						// Scroll
						::ScrollConsoleScreenBuffer( m_screenBuffer, &scrollRect, NULL, coordDest, &fill );
					}
					else
					{
						// The cursor isn't on the last row
						csbiInfo.dwCursorPosition.Y += offsetY;
					}

					::SetConsoleCursorPosition( m_screenBuffer, csbiInfo.dwCursorPosition );
				}
			}
			else
			{
				::WriteConsoleA( m_screenBuffer, toLog.c_str(), DWORD( toLog.size() ), &written, NULL );
			}
		}

	private:
		void DoInitialiseConsole()
		{
			m_screenBuffer = ::CreateConsoleScreenBuffer( GENERIC_WRITE | GENERIC_READ, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL );

			if ( m_screenBuffer != INVALID_HANDLE_VALUE && ::SetConsoleActiveScreenBuffer( m_screenBuffer ) )
			{
				COORD coord = { 160, 9999 };
				::SetConsoleScreenBufferSize( m_screenBuffer, coord );
			}

			m_oldCodePage = ::GetConsoleOutputCP();
			::EnumSystemCodePages( & DoCodePageProc, CP_INSTALLED );
			FILE * dump;
			dump = freopen( "conout$", "w", stdout );
			dump = freopen( "conout$", "w", stderr );
			m_console = true;
		}
		static BOOL __stdcall DoCodePageProc( xchar * szCodePage )
		{
			BOOL ret = TRUE;
			String codePage( szCodePage );
			int cp = stoi( codePage );

			if ( cp == CP_UTF8 )
			{
				::SetConsoleCP( cp );
				::SetConsoleOutputCP( cp );
				ret = FALSE;
			}

			return ret;
		}

	private:
		uint32_t m_oldCodePage;
		HANDLE m_screenBuffer;
		bool m_allocated;
		bool m_console;
	};

#else

	class CLinuxConsoleInfo
		: public IConsole
	{
	public:
		CLinuxConsoleInfo()
		{
		}

		virtual ~CLinuxConsoleInfo()
		{
		}

		void BeginLog( ELogType logLevel )
		{
			switch ( logLevel )
			{
			case ELogType_DEBUG:
				m_header = cuT( "\033[36m" );
				break;

			case ELogType_INFO:
				m_header = cuT( "\033[0m" );
				break;

			case ELogType_WARNING:
				m_header = cuT( "\033[33m" );
				break;

			case ELogType_ERROR:
				m_header = cuT( "\033[31m" );
				break;
			}
		}

		void Print( String const & toLog, bool newLine )
		{
#if CASTOR_USE_UNICODE
			printf( "%ls%ls\033[0m", m_header.c_str(), toLog.c_str() );
#else
			printf( "%s%s\033[0m", m_header.c_str(), toLog.c_str() );
#endif

			if ( newLine )
			{
				printf( "\n" );
			}
		}

	private:
		String m_header;
	};

#endif

	class CGenericConsoleInfo
		: public IConsole
	{
	public:
		CGenericConsoleInfo()
		{
		}

		virtual ~CGenericConsoleInfo()
		{
		}

		void BeginLog( ELogType logLevel )
		{
		}

		void Print( String const & toLog, bool newLine )
		{
#if CASTOR_USE_UNICODE
			printf( "%ls", toLog.c_str() );
#else
			printf( "%s", toLog.c_str() );
#endif

			if ( newLine )
			{
				printf( "\n" );
			}
		}
	};

	DefaultConsole::DefaultConsole()
	{
		m_console = std::make_unique< CGenericConsoleInfo >();
	}

	DefaultConsole::~DefaultConsole()
	{
		m_console.reset();
	}

	void DefaultConsole::BeginLog( ELogType p_eLogType )
	{
		m_console->BeginLog( p_eLogType );
	}

	void DefaultConsole::Print( String const & toLog, bool newLine )
	{
		m_console->Print( toLog, newLine );
	}

	DebugConsole::DebugConsole()
	{
#if defined( _MSC_VER )
		m_console = std::make_unique< CMsvcConsoleInfo >();
#elif defined( _WIN32 )
		m_console = std::make_unique< CMswConsoleInfo >();
#else
		m_console = std::make_unique< CLinuxConsoleInfo >();
#endif
	}

	DebugConsole::~DebugConsole()
	{
		m_console.reset();
	}

	void DebugConsole::BeginLog( ELogType logLevel )
	{
		m_console->BeginLog( logLevel );
	}

	void DebugConsole::Print( String const & toLog, bool newLine )
	{
		m_console->Print( toLog, newLine );
	}
}

#if defined( _MSC_VER )
#	pragma warning( pop )
#endif
