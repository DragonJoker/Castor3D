#include "Log/LoggerConsole.hpp"

#if defined( CASTOR_PLATFORM_LINUX )

#include <iomanip>

namespace castor
{
	class DebugConsole
		: public ConsoleImpl
	{
	public:
		void beginLog( LogType logLevel )
		{
			switch ( logLevel )
			{
			case LogType::eTrace:
				m_header = cuT( "\033[36m" );
				break;

			case LogType::eDebug:
				m_header = cuT( "\033[1;36m" );
				break;

			case LogType::eInfo:
				m_header = cuT( "\033[0m" );
				break;

			case LogType::eWarning:
				m_header = cuT( "\033[33m" );
				break;

			case LogType::eError:
				m_header = cuT( "\033[31m" );
				break;
			}
		}

		void print( String const & toLog, bool newLine )
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

	//************************************************************************************************

	class ReleaseConsole
		: public ConsoleImpl
	{
	public:
		explicit ReleaseConsole( bool p_showConsole )
		{
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
