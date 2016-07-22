#include "CastorUtilsPch.hpp"

#if defined( _WIN32 )
#	include <Windows.h>
#	include <tchar.h>
#	include <codecvt>
#	undef min
#	undef max
#	undef abs
#endif

#include <iomanip>

#include "LoggerConsole.hpp"
#include "Miscellaneous/StringUtils.hpp"

namespace Castor
{
#if defined( _WIN32 )
#	if defined( _MSC_VER )

	class ConsoleHandle
	{
	public:
		bool Initialise( HANDLE p_screenBuffer )
		{
			m_screenBuffer = p_screenBuffer;

			if ( m_screenBuffer == INVALID_HANDLE_VALUE )
			{
				m_screenBuffer = ::CreateConsoleScreenBuffer( GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, nullptr, CONSOLE_TEXTMODE_BUFFER, nullptr );
			}

			if ( m_screenBuffer != INVALID_HANDLE_VALUE && ::SetConsoleActiveScreenBuffer( m_screenBuffer ) )
			{
				m_oldInfos = new CONSOLE_FONT_INFOEX;
				CONSOLE_FONT_INFOEX * l_oldInfos = m_oldInfos;
				l_oldInfos->cbSize = sizeof( CONSOLE_FONT_INFOEX );

				if ( ::GetCurrentConsoleFontEx( m_screenBuffer, FALSE, l_oldInfos ) )
				{
					CONSOLE_FONT_INFOEX l_newInfos = { 0 };
					l_newInfos.cbSize = sizeof( CONSOLE_FONT_INFOEX );
					l_newInfos.dwFontSize = l_oldInfos->dwFontSize;
					l_newInfos.FontWeight = l_oldInfos->FontWeight;
					l_newInfos.nFont = 6;
					l_newInfos.FontFamily = 54;
					wcscpy( l_newInfos.FaceName, L"Lucida Console" );

					if ( !::SetCurrentConsoleFontEx( m_screenBuffer, FALSE, &l_newInfos ) )
					{
						delete m_oldInfos;
						m_oldInfos = nullptr;
					}
				}
				else
				{
					delete m_oldInfos;
					m_oldInfos = nullptr;
				}

				SHORT minX{ SHORT( ::GetSystemMetrics( SM_CXMIN ) ) };
				SHORT minY{ SHORT( ::GetSystemMetrics( SM_CYMIN ) ) };
				COORD l_coord = { std::max< SHORT >( 210, minX ), std::max< SHORT >( minX, 32766 ) };
				
				if ( ::SetConsoleScreenBufferSize( m_screenBuffer, l_coord ) )
				{
					COORD l_size = ::GetLargestConsoleWindowSize( m_screenBuffer );
					SMALL_RECT l_windowRect = { 0 };
					l_windowRect.Right = std::min( l_size.X, l_coord.X ) - 1;
					l_windowRect.Bottom = std::min( l_size.Y, l_coord.Y ) - 1;

					if ( !::SetConsoleWindowInfo( m_screenBuffer, TRUE, &l_windowRect ) )
					{
						StringStream text;
						text << cuT( "Couldn't set console window size (0x" ) << std::hex << std::setw( 8 ) << std::right << std::setfill( '0' ) << ::GetLastError() << ")";
						WriteText( text.str(), true );
					}
				}
				else
				{
					StringStream text;
					text << cuT( "Couldn't set console screenbuffer size (0x" ) << std::hex << std::setw( 8 ) << std::right << std::setfill( '0' ) << ::GetLastError() << ")";
					WriteText( text.str(), true );
				}
			}

			m_oldCodePage = ::GetConsoleOutputCP();
			::EnumSystemCodePages( &DoCodePageProc, CP_INSTALLED );
			return m_screenBuffer != INVALID_HANDLE_VALUE;
		}

		void Cleanup()
		{
			if ( m_screenBuffer != INVALID_HANDLE_VALUE )
			{
				if ( m_oldInfos )
				{
					::SetCurrentConsoleFontEx( m_screenBuffer, FALSE, m_oldInfos );
					delete m_oldInfos;
					m_oldInfos = nullptr;
				}

				::CloseHandle( m_screenBuffer );
				m_screenBuffer = INVALID_HANDLE_VALUE;
			}

			if ( m_oldCodePage )
			{
				::SetConsoleOutputCP( m_oldCodePage );
				m_oldCodePage = 0;
			}
		}

		void SetAttributes( WORD p_attributes )
		{
			::SetConsoleTextAttribute( m_screenBuffer, p_attributes );
		}

		void WriteText( String p_text, bool p_newLine )
		{
			if ( ::IsDebuggerPresent() )
			{
				std::wstring_convert< std::codecvt_utf8_utf16< wchar_t >, wchar_t > l_conversion;
				auto l_converted = l_conversion.from_bytes( p_text );
				::OutputDebugStringW( l_converted.c_str() );

				if ( p_newLine )
				{
					::OutputDebugStringW( L"\n" );
				}
			}

			CONSOLE_SCREEN_BUFFER_INFO l_csbiInfo;

			if ( ::GetConsoleScreenBufferInfo( m_screenBuffer, &l_csbiInfo ) )
			{
				// Manually managed screen buffer.
				l_csbiInfo.dwCursorPosition.X = 0;
				DWORD l_written = 0;
				::WriteConsole( m_screenBuffer, p_text.c_str(), DWORD( p_text.size() ), &l_written, nullptr );
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
			else
			{
				// Automatically managed screen buffer.
				DWORD l_written = 0;

				if ( p_newLine )
				{
					p_text += cuT( "\n" );
				}

				::WriteConsole( m_screenBuffer, p_text.c_str(), DWORD( p_text.size() ), &l_written, nullptr );
			}
		}

	private:
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
	};

#	else

	class ConsoleHandle
	{
	public:
		bool Initialise()
		{
			m_screenBuffer = ::CreateConsoleScreenBuffer( GENERIC_WRITE | GENERIC_READ, 0, nullptr, CONSOLE_TEXTMODE_BUFFER, nullptr );

			if ( m_screenBuffer != INVALID_HANDLE_VALUE && ::SetConsoleActiveScreenBuffer( m_screenBuffer ) )
			{
				COORD l_coord = { 160, 9999 };
				::SetConsoleScreenBufferSize( m_screenBuffer, l_coord );
			}

			m_oldCodePage = ::GetConsoleOutputCP();
			::EnumSystemCodePages( &DoCodePageProc, CP_INSTALLED );
			return m_screenBuffer != INVALID_HANDLE_VALUE;
		}

		void Cleanup()
		{
			if ( m_screenBuffer != INVALID_HANDLE_VALUE )
			{
				::CloseHandle( m_screenBuffer );
				m_screenBuffer = INVALID_HANDLE_VALUE;
			}

			if ( m_oldCodePage )
			{
				::SetConsoleOutputCP( m_oldCodePage );
				m_oldCodePage = 0;
			}
		}

		void SetAttributes( WORD p_attributes )
		{
			::SetConsoleTextAttribute( m_screenBuffer, p_attributes );
		}

		void WriteText( String const & p_text, bool p_newLine )
		{
			CONSOLE_SCREEN_BUFFER_INFO l_csbiInfo;

			if ( ::GetConsoleScreenBufferInfo( m_screenBuffer, &l_csbiInfo ) )
			{
				l_csbiInfo.dwCursorPosition.X = 0;
				DWORD l_written = 0;
				::WriteConsole( m_screenBuffer, p_text.c_str(), DWORD( p_text.size() ), &l_written, nullptr );
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
	};

#	endif

	class DebugConsole
		: public ConsoleImpl
	{
	public:
		DebugConsole()
		{
			if ( ::AllocConsole() )
			{
				m_allocated = true;
				DoInitialiseConsole( INVALID_HANDLE_VALUE );
			}
			else
			{
				DWORD lastError = ::GetLastError();

				if ( lastError == ERROR_ACCESS_DENIED )
				{
					DoInitialiseConsole( ::GetStdHandle( STD_OUTPUT_HANDLE ) );
				}
				else
				{
					std::cerr << "Failed to create to a new console with error " << lastError << std::endl;
				}
			}
		}

		virtual ~DebugConsole()
		{
			m_handle.Cleanup();

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

			m_handle.SetAttributes( l_attributes );
		}

		void Print( String const & p_toLog, bool p_newLine )
		{
			m_handle.WriteText( p_toLog, p_newLine );
		}

	private:
		void DoInitialiseConsole( HANDLE p_handle )
		{
			if ( m_handle.Initialise( p_handle ) )
			{
				FILE * l_dump;
				freopen_s( &l_dump, "conout$", "w", stdout );
				freopen_s( &l_dump, "conout$", "w", stderr );
			}
		}

	private:
		ConsoleHandle m_handle;
		bool m_allocated{ false };
	};

#else

	class DebugConsole
		: public ConsoleImpl
	{
	public:
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

	//************************************************************************************************

	class ReleaseConsole
		: public ConsoleImpl
	{
	public:
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

	//************************************************************************************************

	ProgramConsole::ProgramConsole()
	{
#if defined( NDEBUG )
		m_console = std::make_unique< ReleaseConsole >();
#else
		m_console = std::make_unique< DebugConsole >();
#endif
	}

	ProgramConsole::~ProgramConsole()
	{
	}

	void ProgramConsole::BeginLog( ELogType logLevel )
	{
		m_console->BeginLog( logLevel );
	}

	void ProgramConsole::Print( String const & toLog, bool newLine )
	{
		m_console->Print( toLog, newLine );
	}

	//************************************************************************************************
}
