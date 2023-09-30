#include "CastorUtils/Log/LoggerConsole.hpp"

#if defined( CU_PlatformLinux )

#include <iomanip>

namespace castor
{
	class LinuxConsole
		: public ConsoleImpl
	{
	public:
		void beginLog( LogType logLevel )override
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

			default:
				break;
			}
		}

		void print( String const & toLog, bool newLine )override
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

	ProgramConsole::ProgramConsole( bool CU_UnusedParam( showConsole ) )
	{
		std::ios::sync_with_stdio( false );
		m_console = std::make_unique< LinuxConsole >();
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
