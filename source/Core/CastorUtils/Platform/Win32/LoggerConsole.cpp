#include "CastorUtils/Log/LoggerConsole.hpp"

#if defined( CU_PlatformWindows )

#include "CastorUtils/Miscellaneous/StringUtils.hpp"

#include <Windows.h>
#include <tchar.h>
#include <codecvt>
#include <iomanip>

#undef min
#undef max
#undef abs

namespace castor
{
	class ConsoleHandle
	{
	public:
		bool initialise( HANDLE screenBuffer )
		{
			m_screenBuffer = screenBuffer;
			m_oldCodePage = ::GetConsoleOutputCP();
			::EnumSystemCodePagesA( &doCodePageProc, CP_INSTALLED );
			return true;
		}

		void cleanup()
		{
			if ( m_oldCodePage )
			{
				::SetConsoleOutputCP( m_oldCodePage );
				m_oldCodePage = 0;
			}
		}

		void setAttributes( WORD attributes )
		{
			::SetConsoleTextAttribute( m_screenBuffer, attributes );
		}

		void writeText( String text, bool newLine )
		{
			if ( ::IsDebuggerPresent() )
			{
				int length = MultiByteToWideChar( CP_UTF8, 0u, text.c_str(), -1, nullptr, 0u );

				if ( length > 0 )
				{
					std::vector< wchar_t > buffer( size_t( length + 1 ), wchar_t{} );
					MultiByteToWideChar( CP_UTF8, 0u, text.c_str(), -1, buffer.data(), length );
					std::wstring converted{ buffer.begin(), buffer.end() };
					::OutputDebugStringW( converted.c_str() );
				}

				if ( newLine )
				{
					::OutputDebugStringW( L"\n" );
				}
			}

			DWORD written = 0;

			if ( newLine )
			{
				text += cuT( "\n" );
			}

			::WriteConsoleA( m_screenBuffer, text.c_str(), DWORD( text.size() ), &written, nullptr );
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
			WORD attributes{};

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

		void print( String const & toLog, bool newLine )
		{
			m_handle.writeText( toLog, newLine );
		}

	private:
		void doInitialiseConsole( HANDLE handle )
		{
			if ( m_handle.initialise( handle ) )
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
		explicit ReleaseConsole( bool showConsole )
		{
			if ( showConsole )
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

		void print( String const & toLog, bool newLine )
		{
			printf( "%s", toLog.c_str() );

			if ( newLine )
			{
				printf( "\n" );
			}
		}

	private:
		void doInitialiseConsole( HANDLE handle )
		{
			FILE * dump;
			( void )freopen_s( &dump, "conout$", "w", stdout );
			( void )freopen_s( &dump, "conout$", "w", stderr );
		}

	private:
		ConsoleHandle m_handle;
		bool m_allocated{ false };
	};

	//************************************************************************************************

	ProgramConsole::ProgramConsole( bool showConsole )
	{
#if defined( NDEBUG )
		m_console = std::make_unique< ReleaseConsole >( showConsole );
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
