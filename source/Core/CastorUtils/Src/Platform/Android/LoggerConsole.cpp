#include "Log/LoggerConsole.hpp"

#if defined( CASTOR_PLATFORM_ANDROID )

#include <android/log.h>

#include "Miscellaneous/StringUtils.hpp"

namespace Castor
{
	class LogcatConsole
		: public ConsoleImpl
	{
	public:
		LogcatConsole()
		{
		}

		void BeginLog( LogType logLevel )
		{
			switch ( logLevel )
			{
			case LogType::eTrace:
				m_logLevel = ANDROID_LOG_VERBOSE;
				break;

			case LogType::eDebug:
				m_logLevel = ANDROID_LOG_DEBUG;
				break;

			case LogType::eInfo:
				m_logLevel = ANDROID_LOG_INFO;
				break;

			case LogType::eWarning:
				m_logLevel = ANDROID_LOG_WARN;
				break;

			case LogType::eError:
				m_logLevel = ANDROID_LOG_ERROR;
				break;
			}
		}

		void Print( String const & toLog, bool newLine )
		{
			__android_log_print( m_logLevel
				, "Castor3D"
				, "%s"
				, string::string_cast< char >( toLog ).c_str() );
		}

	private:
		android_LogPriority m_logLevel;
	};

	ProgramConsole::ProgramConsole( bool p_showConsole )
	{
		m_console = std::make_unique< LogcatConsole >();
	}

	ProgramConsole::~ProgramConsole()
	{
	}

	void ProgramConsole::BeginLog( LogType logLevel )
	{
		m_console->BeginLog( logLevel );
	}

	void ProgramConsole::Print( String const & toLog, bool newLine )
	{
		m_console->Print( toLog, newLine );
	}

	//************************************************************************************************
}

#endif
