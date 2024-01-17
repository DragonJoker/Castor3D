#include "CastorUtils/Log/LoggerConsole.hpp"

#if defined( CU_PlatformWindows )

#include "CastorUtils/Miscellaneous/StringUtils.hpp"

#include <Windows.h>
#include <tchar.h>
#include <codecvt>
#include <iomanip>

namespace castor
{
	void printCDBConsole( String const & toLog, bool newLine )
	{
		if ( ::IsDebuggerPresent() )
		{
			::OutputDebugStringA( toLog.c_str() );

			if ( newLine )
			{
				::OutputDebugStringA( "\n" );
			}
		}
	}

	class Win32Console
		: public ConsoleImpl
	{
	public:
		explicit Win32Console( bool showConsole )
		{
			if ( showConsole )
			{
				if ( ::AllocConsole() )
				{
					m_allocated = true;
					doInitialiseConsole();
				}
				else
				{
					DWORD lastError = ::GetLastError();

					if ( lastError == ERROR_ACCESS_DENIED )
					{
						doInitialiseConsole();
					}
					else
					{
						std::cerr << "Failed to create to a new console with error " << lastError << std::endl;
					}
				}
			}
		}

		~Win32Console()noexcept override
		{
			if ( m_allocated )
			{
				::FreeConsole();
			}
		}

		void beginLog( LogType logLevel )override
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

			::SetConsoleTextAttribute( m_handle, attributes );
		}

		void print( String const & toLog, bool newLine )override
		{
			printCDBConsole( toLog, newLine );
			printf( "%s", toLog.c_str() );

			if ( newLine )
			{
				printf( "\n" );
			}
		}

	private:
		void doInitialiseConsole()
		{
			FILE * dump;
			( void )freopen_s( &dump, "conout$", "w", stdout );
			( void )freopen_s( &dump, "conout$", "w", stderr );
			m_handle = ::GetStdHandle( STD_OUTPUT_HANDLE );
		}

	private:
		bool m_allocated{ false };
		HANDLE m_handle{ INVALID_HANDLE_VALUE };
	};

	//************************************************************************************************

	ProgramConsole::ProgramConsole( bool showConsole )
	{
		std::ios::sync_with_stdio( false );
		m_console = std::make_unique< Win32Console >( showConsole );
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
