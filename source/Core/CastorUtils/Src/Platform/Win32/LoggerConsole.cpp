#include "Log/LoggerConsole.hpp"

#if defined( CASTOR_PLATFORM_WINDOWS )

#include "Miscellaneous/StringUtils.hpp"

#include <Windows.h>
#include <tchar.h>
#include <codecvt>
#include <iomanip>

#undef min
#undef max
#undef abs

namespace castor
{
#if defined( CASTOR_COMPILER_MSVC )

	class ConsoleHandle
	{
	public:
		bool initialise( HANDLE p_screenBuffer )
		{
			m_screenBuffer = p_screenBuffer;

			if ( m_screenBuffer == INVALID_HANDLE_VALUE )
			{
				m_screenBuffer = ::CreateConsoleScreenBuffer( GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, nullptr, CONSOLE_TEXTMODE_BUFFER, nullptr );
			}

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
				COORD coord = { std::max< SHORT >( 210, minX ), std::max< SHORT >( minX, 32766 ) };

				if ( ::SetConsoleScreenBufferSize( m_screenBuffer, coord ) )
				{
					COORD size = ::GetLargestConsoleWindowSize( m_screenBuffer );
					SMALL_RECT windowRect = { 0 };
					windowRect.Right = std::min( size.X, coord.X ) - 1;
					windowRect.Bottom = std::min( size.Y, coord.Y ) - 1;

					if ( !::SetConsoleWindowInfo( m_screenBuffer, TRUE, &windowRect ) )
					{
						StringStream text{ makeStringStream() };
						text << cuT( "Couldn't set console window size (0x" ) << std::hex << std::setw( 8 ) << std::right << std::setfill( '0' ) << ::GetLastError() << ")";
						writeText( text.str(), true );
					}
				}
				else
				{
					StringStream text{ makeStringStream() };
					text << cuT( "Couldn't set console screenbuffer size (0x" ) << std::hex << std::setw( 8 ) << std::right << std::setfill( '0' ) << ::GetLastError() << ")";
					writeText( text.str(), true );
				}
			}

			m_oldCodePage = ::GetConsoleOutputCP();
			::EnumSystemCodePages( &doCodePageProc, CP_INSTALLED );
			return m_screenBuffer != INVALID_HANDLE_VALUE;
		}

		void cleanup()
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

		void setAttributes( WORD p_attributes )
		{
			::SetConsoleTextAttribute( m_screenBuffer, p_attributes );
		}

		void writeText( String p_text, bool p_newLine )
		{
			if ( ::IsDebuggerPresent() )
			{
				std::wstring_convert< std::codecvt_utf8_utf16< wchar_t >, wchar_t > conversion;
				auto converted = conversion.from_bytes( p_text );
				::OutputDebugStringW( converted.c_str() );

				if ( p_newLine )
				{
					::OutputDebugStringW( L"\n" );
				}
			}

			CONSOLE_SCREEN_BUFFER_INFO csbiInfo;

			if ( ::GetConsoleScreenBufferInfo( m_screenBuffer, &csbiInfo ) )
			{
				// Manually managed screen buffer.
				csbiInfo.dwCursorPosition.X = 0;
				DWORD written = 0;
				::WriteConsole( m_screenBuffer, p_text.c_str(), DWORD( p_text.size() ), &written, nullptr );

				if ( p_newLine )
				{
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
						// set the fill character and attributes.
						CHAR_INFO fill;
						fill.Attributes = 0;
						fill.Char.AsciiChar = ' ';
						fill.Char.UnicodeChar = L' ';
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
				// Automatically managed screen buffer.
				DWORD written = 0;

				if ( p_newLine )
				{
					p_text += cuT( "\n" );
				}

				::WriteConsole( m_screenBuffer, p_text.c_str(), DWORD( p_text.size() ), &written, nullptr );
			}
		}

	private:
		static BOOL __stdcall doCodePageProc( xchar * szCodePage )
		{
			BOOL result = TRUE;
			String codePage( szCodePage );
			int cp = stoi( codePage );

			if ( cp == CP_UTF8 )
			{
				::SetConsoleCP( cp );
				::SetConsoleOutputCP( cp );
				result = FALSE;
			}

			return result;
		}

	private:
		uint32_t m_oldCodePage{ 0 };
		HANDLE m_screenBuffer{ INVALID_HANDLE_VALUE };
		CONSOLE_FONT_INFOEX * m_oldInfos{ nullptr };
	};

#else

	class ConsoleHandle
	{
	public:
		bool initialise()
		{
			m_screenBuffer = ::CreateConsoleScreenBuffer( GENERIC_WRITE | GENERIC_READ, 0, nullptr, CONSOLE_TEXTMODE_BUFFER, nullptr );

			if ( m_screenBuffer != INVALID_HANDLE_VALUE && ::SetConsoleActiveScreenBuffer( m_screenBuffer ) )
			{
				COORD coord = { 160, 9999 };
				::SetConsoleScreenBufferSize( m_screenBuffer, coord );
			}

			m_oldCodePage = ::getConsoleOutputCP();
			::EnumSystemCodePages( &doCodePageProc, CP_INSTALLED );
			return m_screenBuffer != INVALID_HANDLE_VALUE;
		}

		void cleanup()
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

		void setAttributes( WORD p_attributes )
		{
			::SetConsoleTextAttribute( m_screenBuffer, p_attributes );
		}

		void writeText( String const & p_text, bool p_newLine )
		{
			CONSOLE_SCREEN_BUFFER_INFO csbiInfo;

			if ( ::GetConsoleScreenBufferInfo( m_screenBuffer, &csbiInfo ) )
			{
				csbiInfo.dwCursorPosition.X = 0;
				DWORD written = 0;
				::WriteConsole( m_screenBuffer, p_text.c_str(), DWORD( p_text.size() ), &written, nullptr );
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
					// set the fill character and attributes.
					CHAR_INFO fill;
					fill.Attributes = 0;
					fill.Char.AsciiChar = ' ';
					fill.Char.UnicodeChar = L' ';
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

	private:
		static BOOL __stdcall doCodePageProc( xchar * szCodePage )
		{
			BOOL result = TRUE;
			String codePage( szCodePage );
			int cp = stoi( codePage );

			if ( cp == CP_UTF8 )
			{
				::SetConsoleCP( cp );
				::SetConsoleOutputCP( cp );
				result = FALSE;
			}

			return result;
		}

	private:
		uint32_t m_oldCodePage{ 0 };
		HANDLE m_screenBuffer{ INVALID_HANDLE_VALUE };
	};

#endif

	class DebugConsole
		: public ConsoleImpl
	{
	public:
		DebugConsole()
		{
			if ( ::AllocConsole() )
			{
				m_allocated = true;
				doInitialiseConsole( INVALID_HANDLE_VALUE );
			}
			else
			{
				DWORD lastError = ::GetLastError();

				if ( lastError == ERROR_ACCESS_DENIED )
				{
					doInitialiseConsole( ::GetStdHandle( STD_OUTPUT_HANDLE ) );
				}
				else
				{
					std::cerr << "Failed to create to a new console with error " << lastError << std::endl;
				}
			}
		}

		virtual ~DebugConsole()
		{
			m_handle.cleanup();

			if ( m_allocated )
			{
				::FreeConsole();
			}
		}

		void beginLog( LogType logLevel )
		{
			WORD attributes;

			switch ( logLevel )
			{
			case LogType::eTrace:
				attributes = FOREGROUND_BLUE | FOREGROUND_GREEN;
				break;

			case LogType::eDebug:
				attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
				break;

			case LogType::eWarning:
				attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
				break;

			case LogType::eError:
				attributes = FOREGROUND_RED | FOREGROUND_INTENSITY;
				break;

			default:
				attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
				break;
			}

			m_handle.setAttributes( attributes );
		}

		void print( String const & p_toLog, bool p_newLine )
		{
			m_handle.writeText( p_toLog, p_newLine );
		}

	private:
		void doInitialiseConsole( HANDLE p_handle )
		{
			if ( m_handle.initialise( p_handle ) )
			{
				FILE * dump;
				freopen_s( &dump, "conout$", "w", stdout );
				freopen_s( &dump, "conout$", "w", stderr );
			}
		}

	private:
		ConsoleHandle m_handle;
		bool m_allocated{ false };
	};

	class ReleaseConsole
		: public ConsoleImpl
	{
	public:
		explicit ReleaseConsole( bool p_showConsole )
		{
			if ( p_showConsole )
			{
				if ( ::AllocConsole() )
				{
					m_allocated = true;
					doInitialiseConsole( INVALID_HANDLE_VALUE );
				}
				else
				{
					DWORD lastError = ::GetLastError();

					if ( lastError == ERROR_ACCESS_DENIED )
					{
						doInitialiseConsole( ::GetStdHandle( STD_OUTPUT_HANDLE ) );
					}
					else
					{
						std::cerr << "Failed to create to a new console with error " << lastError << std::endl;
					}
				}
			}
		}

		virtual ~ReleaseConsole()
		{
			if ( m_allocated )
			{
				::FreeConsole();
			}
		}

		void beginLog( LogType logLevel )
		{
		}

		void print( String const & p_toLog, bool p_newLine )
		{
			printf( "%s", p_toLog.c_str() );

			if ( p_newLine )
			{
				printf( "\n" );
			}
		}

	private:
		void doInitialiseConsole( HANDLE p_handle )
		{
			FILE * dump;
			freopen_s( &dump, "conout$", "w", stdout );
			freopen_s( &dump, "conout$", "w", stderr );
		}

	private:
		ConsoleHandle m_handle;
		bool m_allocated{ false };
	};

	//************************************************************************************************

	ProgramConsole::ProgramConsole( bool p_showConsole )
	{
#if defined( NDEBUG )
		m_console = std::make_unique< ReleaseConsole >( p_showConsole );
#else
		m_console = std::make_unique< DebugConsole >();
#endif
	}

	ProgramConsole::~ProgramConsole()
	{
	}

	void ProgramConsole::beginLog( LogType logLevel )
	{
		m_console->beginLog( logLevel );
	}

	void ProgramConsole::print( String const & toLog, bool newLine )
	{
		m_console->print( toLog, newLine );
	}

	//************************************************************************************************
}

#endif
