#include "CastorUtils/Log/LoggerConsole.hpp"

#if defined( CU_PlatformAndroid )

#include <android/log.h>

#include "CastorUtils/Miscellaneous/StringUtils.hpp"

namespace castor
{
	//************************************************************************************************

	class LogcatConsole
		: public ConsoleImpl
	{
	public:
		LogcatConsole()
		{
		}

		void beginLog( LogType logLevel )override
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
			default:
				break;
			}
		}

		void print( MbString const & toLog, bool newLine )override
		{
			__android_log_print( m_logLevel
				, "Castor3D"
				, "%s"
				, toLog.c_str() );
		}

	private:
		android_LogPriority m_logLevel;
	};

	//************************************************************************************************

	ProgramConsole::ProgramConsole( CU_UnusedParam( bool, showConsole ) )
	{
		std::ios::sync_with_stdio( false );
		m_console = castor::make_unique< LogcatConsole >();
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
