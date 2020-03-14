#include "CastorUtils/Log/Logger.hpp"

#include "CastorUtils/Exception/Assertion.hpp"
#include "CastorUtils/Exception/Exception.hpp"
#include "CastorUtils/Log/LoggerConsole.hpp"
#include "CastorUtils/Log/LoggerStreambuf.hpp"

namespace castor
{
	static const std::string ERROR_LOGGER_ALREADY_INITIALISED = "Logger instance already initialised";

	Logger * Logger::m_singleton = nullptr;

	Logger::Logger( LogType level )
		: m_console{ std::make_unique< ProgramConsole >( level < LogType::eInfo ) }
		, m_instance{ std::make_unique< LoggerInstance >( *m_console, level ) }
		, m_cout{ new InfoLoggerStreambufT< char >( *m_instance, std::cout ) }
		, m_cerr{ new ErrorLoggerStreambufT< char >( *m_instance, std::cerr ) }
		, m_clog{ new DebugLoggerStreambufT< char >( *m_instance, std::clog ) }
		, m_wcout{ new InfoLoggerStreambufT< wchar_t >( *m_instance, std::wcout ) }
		, m_wcerr{ new ErrorLoggerStreambufT< wchar_t >( *m_instance, std::wcerr ) }
		, m_wclog{ new DebugLoggerStreambufT< wchar_t >( *m_instance, std::wclog ) }
	{
	}

	Logger::~Logger()
	{
		delete m_cout;
		delete m_cerr;
		delete m_clog;
		delete m_wcout;
		delete m_wcerr;
		delete m_wclog;
	}

	LoggerInstance * Logger::initialise( LogType level )
	{
		LoggerInstance * result = nullptr;

		if ( m_singleton )
		{
			CU_Exception( ERROR_LOGGER_ALREADY_INITIALISED );
		}
		else
		{
			m_singleton = new Logger( level );
			result = m_singleton->getInstance();
		}

		return result;
	}

	void Logger::cleanup()
	{
		if ( m_singleton )
		{
			delete m_singleton;
			m_singleton = nullptr;
		}
	}

	LoggerInstance * Logger::createInstance( LogType logLevel )
	{
		CU_Require( getSingleton().m_console );
		return new LoggerInstance{ *getSingleton().m_console, logLevel };
	}

	void Logger::registerCallback( LogCallback p_pfnCallback, void * p_pCaller )
	{
		CU_Require( getSingleton().m_instance );
		getSingleton().m_instance->registerCallback( p_pfnCallback, p_pCaller );
	}

	void Logger::unregisterCallback( void * p_pCaller )
	{
		CU_Require( getSingleton().m_instance );
		getSingleton().m_instance->unregisterCallback( p_pCaller );
	}

	void Logger::setFileName( Path const & p_logFilePath, LogType p_eLogType )
	{
		if ( getSingleton().m_instance )
		{
			getSingleton().m_instance->setFileName( p_logFilePath, p_eLogType );
		}
	}

	LogType Logger::getLevel()
	{
		CU_Require( getSingleton().m_instance );
		return getSingleton().m_instance->getLevel();
	}

	void Logger::logTrace( std::string const & p_msg )
	{
		CU_Require( getSingleton().m_instance );
		getSingleton().m_instance->pushMessage( LogType::eTrace, p_msg );
	}

	void Logger::logTrace( std::ostream const & p_msg )
	{
		auto sbuf = p_msg.rdbuf();
		std::stringstream ss;
		ss << sbuf;
		logTrace( ss.str() );
	}

	void Logger::logTrace( std::wstring const & p_msg )
	{
		CU_Require( getSingleton().m_instance );
		getSingleton().m_instance->pushMessage( LogType::eTrace, string::stringCast< char >( p_msg ) );
	}

	void Logger::logTrace( std::wostream const & p_msg )
	{
		std::wstringstream ss;
		ss << p_msg.rdbuf();
		logTrace( ss.str() );
	}

	void Logger::logDebug( std::string const & p_msg )
	{
		CU_Require( getSingleton().m_instance );
		getSingleton().m_instance->pushMessage( LogType::eDebug, p_msg );
	}

	void Logger::logDebug( std::ostream const & p_msg )
	{
		auto sbuf = p_msg.rdbuf();
		std::stringstream ss;
		ss << sbuf;
		logDebug( ss.str() );
	}

	void Logger::logDebug( std::wstring const & p_msg )
	{
		CU_Require( getSingleton().m_instance );
		getSingleton().m_instance->pushMessage( LogType::eDebug, string::stringCast< char >( p_msg ) );
	}

	void Logger::logDebug( std::wostream const & p_msg )
	{
		std::wstringstream ss;
		ss << p_msg.rdbuf();
		logDebug( ss.str() );
	}

	void Logger::logInfo( std::string const & p_msg )
	{
		CU_Require( getSingleton().m_instance );
		getSingleton().m_instance->pushMessage( LogType::eInfo, p_msg );
	}

	void Logger::logInfo( std::ostream const & p_msg )
	{
		std::stringstream ss;
		ss << p_msg.rdbuf();
		logInfo( ss.str() );
	}

	void Logger::logInfo( std::wstring const & p_msg )
	{
		CU_Require( getSingleton().m_instance );
		getSingleton().m_instance->pushMessage( LogType::eInfo, string::stringCast< char >( p_msg ) );
	}

	void Logger::logInfo( std::wostream const & p_msg )
	{
		std::wstringstream ss;
		ss << p_msg.rdbuf();
		logInfo( ss.str() );
	}

	void Logger::logWarning( std::string const & p_msg )
	{
		CU_Require( getSingleton().m_instance );
		getSingleton().m_instance->pushMessage( LogType::eWarning, p_msg );
	}

	void Logger::logWarning( std::ostream const & p_msg )
	{
		std::stringstream ss;
		ss << p_msg.rdbuf();
		logWarning( ss.str() );
	}

	void Logger::logWarning( std::wstring const & p_msg )
	{
		CU_Require( getSingleton().m_instance );
		getSingleton().m_instance->pushMessage( LogType::eWarning, string::stringCast< char >( p_msg ) );
	}

	void Logger::logWarning( std::wostream const & p_msg )
	{
		std::wstringstream ss;
		ss << p_msg.rdbuf();
		logWarning( ss.str() );
	}

	void Logger::logError( std::string const & p_msg )
	{
		CU_Require( getSingleton().m_instance );
		getSingleton().m_instance->pushMessage( LogType::eError, p_msg );
	}

	void Logger::logError( std::ostream const & p_msg )
	{
		std::stringstream ss;
		ss << p_msg.rdbuf();
		logError( ss.str() );
	}

	void Logger::logError( std::wstring const & p_msg )
	{
		CU_Require( getSingleton().m_instance );
		getSingleton().m_instance->pushMessage( LogType::eError, string::stringCast< char >( p_msg ) );
	}

	void Logger::logError( std::wostream const & p_msg )
	{
		std::wstringstream ss;
		ss << p_msg.rdbuf();
		logError( ss.str() );
	}

	void Logger::logTraceNoNL( std::string const & p_msg )
	{
		CU_Require( getSingleton().m_instance );
		getSingleton().m_instance->pushMessage( LogType::eTrace, p_msg, false );
	}

	void Logger::logTraceNoNL( std::ostream const & p_msg )
	{
		auto sbuf = p_msg.rdbuf();
		std::stringstream ss;
		ss << sbuf;
		logTraceNoNL( ss.str() );
	}

	void Logger::logTraceNoNL( std::wstring const & p_msg )
	{
		CU_Require( getSingleton().m_instance );
		getSingleton().m_instance->pushMessage( LogType::eTrace, string::stringCast< char >( p_msg ), false );
	}

	void Logger::logTraceNoNL( std::wostream const & p_msg )
	{
		std::wstringstream ss;
		ss << p_msg.rdbuf();
		logTraceNoNL( ss.str() );
	}

	void Logger::logDebugNoNL( std::string const & p_msg )
	{
		CU_Require( getSingleton().m_instance );
		getSingleton().m_instance->pushMessage( LogType::eDebug, p_msg, false );
	}

	void Logger::logDebugNoNL( std::ostream const & p_msg )
	{
		auto sbuf = p_msg.rdbuf();
		std::stringstream ss;
		ss << sbuf;
		logDebugNoNL( ss.str() );
	}

	void Logger::logDebugNoNL( std::wstring const & p_msg )
	{
		CU_Require( getSingleton().m_instance );
		getSingleton().m_instance->pushMessage( LogType::eDebug, string::stringCast< char >( p_msg ), false );
	}

	void Logger::logDebugNoNL( std::wostream const & p_msg )
	{
		std::wstringstream ss;
		ss << p_msg.rdbuf();
		logDebugNoNL( ss.str() );
	}

	void Logger::logInfoNoNL( std::string const & p_msg )
	{
		CU_Require( getSingleton().m_instance );
		getSingleton().m_instance->pushMessage( LogType::eInfo, p_msg, false );
	}

	void Logger::logInfoNoNL( std::ostream const & p_msg )
	{
		std::stringstream ss;
		ss << p_msg.rdbuf();
		logInfoNoNL( ss.str() );
	}

	void Logger::logInfoNoNL( std::wstring const & p_msg )
	{
		CU_Require( getSingleton().m_instance );
		getSingleton().m_instance->pushMessage( LogType::eInfo, string::stringCast< char >( p_msg ), false );
	}

	void Logger::logInfoNoNL( std::wostream const & p_msg )
	{
		std::wstringstream ss;
		ss << p_msg.rdbuf();
		logInfoNoNL( ss.str() );
	}

	void Logger::logWarningNoNL( std::string const & p_msg )
	{
		CU_Require( getSingleton().m_instance );
		getSingleton().m_instance->pushMessage( LogType::eWarning, p_msg, false );
	}

	void Logger::logWarningNoNL( std::ostream const & p_msg )
	{
		std::stringstream ss;
		ss << p_msg.rdbuf();
		logWarningNoNL( ss.str() );
	}

	void Logger::logWarningNoNL( std::wstring const & p_msg )
	{
		CU_Require( getSingleton().m_instance );
		getSingleton().m_instance->pushMessage( LogType::eWarning, string::stringCast< char >( p_msg ), false );
	}

	void Logger::logWarningNoNL( std::wostream const & p_msg )
	{
		std::wstringstream ss;
		ss << p_msg.rdbuf();
		logWarningNoNL( ss.str() );
	}

	void Logger::logErrorNoNL( std::string const & p_msg )
	{
		CU_Require( getSingleton().m_instance );
		getSingleton().m_instance->pushMessage( LogType::eError, p_msg, false );
	}

	void Logger::logErrorNoNL( std::ostream const & p_msg )
	{
		std::stringstream ss;
		ss << p_msg.rdbuf();
		logErrorNoNL( ss.str() );
	}

	void Logger::logErrorNoNL( std::wstring const & p_msg )
	{
		CU_Require( getSingleton().m_instance );
		getSingleton().m_instance->pushMessage( LogType::eError, string::stringCast< char >( p_msg ), false );
	}

	void Logger::logErrorNoNL( std::wostream const & p_msg )
	{
		std::wstringstream ss;
		ss << p_msg.rdbuf();
		logError( ss.str() );
	}

	Logger & Logger::getSingleton()
	{
		if ( !m_singleton )
		{
			CU_Exception( cuT( "No Logger initialised." ) );
		}

		return *m_singleton;
	}

	Logger * Logger::getSingletonPtr()
	{
		return &getSingleton();
	}
}
