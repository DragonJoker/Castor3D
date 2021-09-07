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

#define CU_UseAnsiCode 0

namespace castor
{
	void printCDBConsole( String const & toLog, bool newLine )
	{
		if ( ::IsDebuggerPresent() )
		{
			int length = MultiByteToWideChar( CP_UTF8, 0u, toLog.c_str(), -1, nullptr, 0u );

			if ( length > 0 )
			{
				std::vector< wchar_t > buffer( size_t( length + 1 ), wchar_t{} );
				MultiByteToWideChar( CP_UTF8, 0u, toLog.c_str(), -1, buffer.data(), length );
				std::wstring converted{ buffer.begin(), buffer.end() };
				::OutputDebugStringW( converted.c_str() );
			}

			if ( newLine )
			{
				::OutputDebugStringW( L"\n" );
			}
		}
	}

	class DebugConsole
		: public ConsoleImpl
	{
	public:
		DebugConsole()
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

		virtual ~DebugConsole()
		{
			if ( m_allocated )
			{
				::FreeConsole();
			}
		}

		void beginLog( LogType logLevel )
		{
#if CU_UseAnsiCode
			switch ( logLevel )
			{
			case LogType::eTrace:
				m_header = cuT( "[36m" );
				break;

			case LogType::eDebug:
				m_header = cuT( "[1;36m" );
				break;

			case LogType::eInfo:
				m_header = cuT( "[0m" );
				break;

			case LogType::eWarning:
				m_header = cuT( "[33m" );
				break;

			case LogType::eError:
				m_header = cuT( "[31m" );
				break;

			default:
				break;
			}
#else
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
#endif
		}

		void print( String const & toLog, bool newLine )
		{
			printCDBConsole( toLog, newLine );
#if CU_UseAnsiCode
			printf( "%s%s", m_header.c_str(), toLog.c_str() );
#else
			printf( "%s", toLog.c_str() );
#endif

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

		void beginLog( LogType CU_UnusedParam( logLevel ) )
		{
		}

		void print( String const & toLog, bool newLine )
		{
			printCDBConsole( toLog, newLine );
			printf( "%s", toLog.c_str() );

			if ( newLine )
			{
				printf( "\n" );
			}
		}

	private:
		void doInitialiseConsole( HANDLE CU_UnusedParam( handle ) )
		{
			FILE * dump;
			( void )freopen_s( &dump, "conout$", "w", stdout );
			( void )freopen_s( &dump, "conout$", "w", stderr );
		}

	private:
		bool m_allocated{ false };
	};

	//************************************************************************************************

	ProgramConsole::ProgramConsole( bool CU_UnusedParam( showConsole ) )
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
