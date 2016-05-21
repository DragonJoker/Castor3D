#include "CastorUtilsPch.hpp"

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
			WORD l_attributes;

			switch ( logLevel )
			{
			case ELogType_DEBUG:
				l_attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
				break;

			case ELogType_WARNING:
				l_attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
				break;

			case ELogType_ERROR:
				l_attributes = FOREGROUND_RED | FOREGROUND_INTENSITY;
				break;

			default:
				l_attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
				break;
			}

			::SetConsoleTextAttribute( m_screenBuffer, l_attributes );
		}

		void Print( String const & p_toLog, bool p_newLine )
		{
			::OutputDebugString( p_toLog.c_str() );

			if ( p_newLine )
			{
				::OutputDebugString( cuT( "\n" ) );
			}

			CONSOLE_SCREEN_BUFFER_INFO l_csbiInfo;

			if ( ::GetConsoleScreenBufferInfo( m_screenBuffer, &l_csbiInfo ) )
			{
				l_csbiInfo.dwCursorPosition.X = 0;
				DWORD l_written = 0;
				::WriteConsole( m_screenBuffer, p_toLog.c_str(), DWORD( p_toLog.size() ), &l_written, nullptr );
				SHORT l_offsetY = SHORT( 1 + l_written / l_csbiInfo.dwSize.X );

				if ( ( l_csbiInfo.dwSize.Y - l_offsetY ) <= l_csbiInfo.dwCursorPosition.Y )
				{
					// The cursor is on the last row
					// The scroll rectangle is from second row to last displayed row
					SMALL_RECT l_scrollRect;
					l_scrollRect.Top = 1;
					l_scrollRect.Bottom = l_csbiInfo.dwSize.Y - 1;
					l_scrollRect.Left = 0;
					l_scrollRect.Right = l_csbiInfo.dwSize.X - 1;
					// The destination for the scroll rectangle is one row up.
					COORD l_coordDest;
					l_coordDest.X = 0;
					l_coordDest.Y = 0;
					// Set the fill character and attributes.
					CHAR_INFO l_fill;
					l_fill.Attributes = 0;
					l_fill.Char.AsciiChar = ' ';
					l_fill.Char.UnicodeChar = L' ';
					// Scroll
					::ScrollConsoleScreenBuffer( m_screenBuffer, &l_scrollRect, nullptr, l_coordDest, &l_fill );
				}
				else
				{
					// The cursor isn't on the last row
					l_csbiInfo.dwCursorPosition.Y += l_offsetY;
				}

				::SetConsoleCursorPosition( m_screenBuffer, l_csbiInfo.dwCursorPosition );
			}
		}

	private:
		void DoInitialiseConsole()
		{
			if ( m_screenBuffer == INVALID_HANDLE_VALUE )
			{
				m_screenBuffer = ::CreateConsoleScreenBuffer( GENERIC_WRITE | GENERIC_READ, 0, nullptr, CONSOLE_TEXTMODE_BUFFER, nullptr );
				DoInitialiseConsoleHandle( m_screenBuffer, m_oldInfos, m_oldCodePage );
				FILE * l_dump;
				freopen_s( &l_dump, "conout$", "w", stdout );
				freopen_s( &l_dump, "conout$", "w", stderr );
				m_console = true;
			}
			else
			{
				DoInitialiseConsoleHandle( m_screenBuffer, m_oldInfos, m_oldCodePage );
			}
		}

		void DoInitialiseConsoleHandle( HANDLE p_screenBuffer, CONSOLE_FONT_INFOEX *& p_oldInfos, uint32_t & p_oldCodePage )
		{
			if ( p_screenBuffer != INVALID_HANDLE_VALUE && ::SetConsoleActiveScreenBuffer( p_screenBuffer ) )
			{
				p_oldInfos = new CONSOLE_FONT_INFOEX;
				CONSOLE_FONT_INFOEX * l_oldInfos = p_oldInfos;
				l_oldInfos->cbSize = sizeof( CONSOLE_FONT_INFOEX );

				if ( ::GetCurrentConsoleFontEx( p_screenBuffer, FALSE, l_oldInfos ) )
				{
					CONSOLE_FONT_INFOEX l_newInfos = { 0 };
					l_newInfos.cbSize = sizeof( CONSOLE_FONT_INFOEX );
					l_newInfos.dwFontSize = l_oldInfos->dwFontSize;
					l_newInfos.FontWeight = l_oldInfos->FontWeight;
					l_newInfos.nFont = 6;
					l_newInfos.FontFamily = 54;
					wcscpy( l_newInfos.FaceName, L"Lucida Console" );

					if ( !::SetCurrentConsoleFontEx( p_screenBuffer, FALSE, &l_newInfos ) )
					{
						delete p_oldInfos;
						p_oldInfos = nullptr;
					}
				}
				else
				{
					delete p_oldInfos;
					p_oldInfos = nullptr;
				}

				COORD l_coord = { 160, 9999 };
				::SetConsoleScreenBufferSize( p_screenBuffer, l_coord );
			}

			p_oldCodePage = ::GetConsoleOutputCP();
			::EnumSystemCodePages( &DoCodePageProc, CP_INSTALLED );
		}

		static BOOL __stdcall DoCodePageProc( xchar * szCodePage )
		{
			BOOL l_return = TRUE;
			String l_codePage( szCodePage );
			int l_cp = stoi( l_codePage );

			if ( l_cp == CP_UTF8 )
			{
				::SetConsoleCP( l_cp );
				::SetConsoleOutputCP( l_cp );
				l_return = FALSE;
			}

			return l_return;
		}

	private:
		uint32_t m_oldCodePage{ 0 };
		HANDLE m_screenBuffer{ INVALID_HANDLE_VALUE };
		CONSOLE_FONT_INFOEX * m_oldInfos{ nullptr };
		bool m_allocated{ false };
		bool m_console{ false };

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
					::WriteConsole( m_screenBuffer, toLog.c_str(), DWORD( toLog.size() ), &written, nullptr );
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
						::ScrollConsoleScreenBuffer( m_screenBuffer, &scrollRect, nullptr, coordDest, &fill );
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
				::WriteConsoleA( m_screenBuffer, toLog.c_str(), DWORD( toLog.size() ), &written, nullptr );
			}
		}

	private:
		void DoInitialiseConsole()
		{
			m_screenBuffer = ::CreateConsoleScreenBuffer( GENERIC_WRITE | GENERIC_READ, 0, nullptr, CONSOLE_TEXTMODE_BUFFER, nullptr );

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
			printf( "%s%s\033[0m", m_header.c_str(), toLog.c_str() );

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
			printf( "%s", toLog.c_str() );

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
