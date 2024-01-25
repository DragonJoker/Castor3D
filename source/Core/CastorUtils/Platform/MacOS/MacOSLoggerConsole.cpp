#include "CastorUtils/Log/LoggerConsole.hpp"

#if defined( CU_PlatformApple )

#include <iomanip>

namespace castor
{
	class MacOSConsole
		: public ConsoleImpl
	{
	public:
		void beginLog( LogType logLevel )override
		{
			switch ( logLevel )
			{
			case LogType::eTrace:
				m_header = "\033[36m";
				break;
			case LogType::eDebug:
				m_header = "\033[1;36m";
				break;
			case LogType::eInfo:
				m_header = "\033[0m";
				break;
			case LogType::eWarning:
				m_header = "\033[33m";
				break;
			case LogType::eError:
				m_header = "\033[31m";
				break;
			default:
				break;
			}
		}

		void print( MbString const & toLog, bool newLine )override
		{
			printf( "%s%s\033[0m", m_header.c_str(), toLog.c_str() );

			if ( newLine )
			{
				printf( "\n" );
			}
		}

	private:
		MbString m_header;
	};

	//************************************************************************************************

	ProgramConsole::ProgramConsole( CU_UnusedParam( bool, showConsole ) )
	{
		std::ios::sync_with_stdio( false );
		m_console = castor::make_unique< MacOSConsole >();
	}

	void ProgramConsole::beginLog( LogType logLevel )
	{
		m_console->beginLog( logLevel );
	}

	void ProgramConsole::print( MbString const & toLog, bool newLine )
	{
		m_console->print( toLog, newLine );
	}

	//************************************************************************************************
}

#endif
