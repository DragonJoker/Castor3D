#include "CastorUtils/Log/Logger.hpp"

#include "CastorUtils/Exception/Assertion.hpp"
#include "CastorUtils/Exception/Exception.hpp"
#include "CastorUtils/Log/LoggerConsole.hpp"
#include "CastorUtils/Log/LoggerStreambuf.hpp"

namespace castor
{
	static const std::string ERROR_LOGGER_ALREADY_INITIALISED = "Logger instance already initialised";

	std::unique_ptr< Logger > Logger::m_singleton = nullptr;

	Logger::Logger( LogType level )
		: m_console{ std::make_unique< ProgramConsole >( level < LogType::eInfo ) }
		, m_instance{ std::make_unique< LoggerInstance >( *m_console, level ) }
		, m_cout{ std::make_unique< InfoLoggerStreambufT< char > >( *m_instance, std::cout ) }
		, m_cerr{ std::make_unique< ErrorLoggerStreambufT< char > >( *m_instance, std::cerr ) }
		, m_clog{ std::make_unique< DebugLoggerStreambufT< char > >( *m_instance, std::clog ) }
		, m_wcout{ std::make_unique< InfoLoggerStreambufT< wchar_t > >( *m_instance, std::wcout ) }
		, m_wcerr{ std::make_unique< ErrorLoggerStreambufT< wchar_t > >( *m_instance, std::wcerr ) }
		, m_wclog{ std::make_unique< DebugLoggerStreambufT< wchar_t > >( *m_instance, std::wclog ) }
	{
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
			m_singleton = std::make_unique< Logger >( level );
			result = m_singleton->getInstance();
		}

		return result;
	}

	void Logger::cleanup()
	{
		if ( m_singleton )
		{
			m_singleton.reset();
		}
	}

	LoggerInstancePtr Logger::createInstance( LogType logLevel )
	{
		CU_Require( getSingleton().m_console );
		return std::make_unique< LoggerInstance >( *getSingleton().m_console, logLevel );
	}

	void Logger::registerCallback( LogCallback const & pfnCallback, void * pCaller )
	{
		CU_Require( getSingleton().m_instance );
		getSingleton().m_instance->registerCallback( pfnCallback, pCaller );
	}

	void Logger::unregisterCallback( void * pCaller )
	{
		CU_Require( getSingleton().m_instance );
		getSingleton().m_instance->unregisterCallback( pCaller );
	}

	void Logger::setFileName( Path const & logFilePath, LogType eLogType )
	{
		if ( getSingleton().m_instance )
		{
			getSingleton().m_instance->setFileName( logFilePath, eLogType );
		}
	}

	LogType Logger::getLevel()
	{
		CU_Require( getSingleton().m_instance );
		return getSingleton().m_instance->getLevel();
	}

	void Logger::logTrace( std::string const & msg )
	{
		CU_Require( getSingleton().m_instance );
		getSingleton().m_instance->pushMessage( LogType::eTrace, msg );
	}

	void Logger::logTrace( std::ostream const & msg )
	{
		auto sbuf = msg.rdbuf();
		std::stringstream ss;
		ss << sbuf;
		logTrace( ss.str() );
	}

	void Logger::logTrace( std::wstring const & msg )
	{
		CU_Require( getSingleton().m_instance );
		getSingleton().m_instance->pushMessage( LogType::eTrace, string::stringCast< char >( msg ) );
	}

	void Logger::logTrace( std::wostream const & msg )
	{
		std::wstringstream ss;
		ss << msg.rdbuf();
		logTrace( ss.str() );
	}

	void Logger::logDebug( std::string const & msg )
	{
		CU_Require( getSingleton().m_instance );
		getSingleton().m_instance->pushMessage( LogType::eDebug, msg );
	}

	void Logger::logDebug( std::ostream const & msg )
	{
		auto sbuf = msg.rdbuf();
		std::stringstream ss;
		ss << sbuf;
		logDebug( ss.str() );
	}

	void Logger::logDebug( std::wstring const & msg )
	{
		CU_Require( getSingleton().m_instance );
		getSingleton().m_instance->pushMessage( LogType::eDebug, string::stringCast< char >( msg ) );
	}

	void Logger::logDebug( std::wostream const & msg )
	{
		std::wstringstream ss;
		ss << msg.rdbuf();
		logDebug( ss.str() );
	}

	void Logger::logInfo( std::string const & msg )
	{
		CU_Require( getSingleton().m_instance );
		getSingleton().m_instance->pushMessage( LogType::eInfo, msg );
	}

	void Logger::logInfo( std::ostream const & msg )
	{
		std::stringstream ss;
		ss << msg.rdbuf();
		logInfo( ss.str() );
	}

	void Logger::logInfo( std::wstring const & msg )
	{
		CU_Require( getSingleton().m_instance );
		getSingleton().m_instance->pushMessage( LogType::eInfo, string::stringCast< char >( msg ) );
	}

	void Logger::logInfo( std::wostream const & msg )
	{
		std::wstringstream ss;
		ss << msg.rdbuf();
		logInfo( ss.str() );
	}

	void Logger::logWarning( std::string const & msg )
	{
		CU_Require( getSingleton().m_instance );
		getSingleton().m_instance->pushMessage( LogType::eWarning, msg );
	}

	void Logger::logWarning( std::ostream const & msg )
	{
		std::stringstream ss;
		ss << msg.rdbuf();
		logWarning( ss.str() );
	}

	void Logger::logWarning( std::wstring const & msg )
	{
		CU_Require( getSingleton().m_instance );
		getSingleton().m_instance->pushMessage( LogType::eWarning, string::stringCast< char >( msg ) );
	}

	void Logger::logWarning( std::wostream const & msg )
	{
		std::wstringstream ss;
		ss << msg.rdbuf();
		logWarning( ss.str() );
	}

	void Logger::logError( std::string const & msg )
	{
		CU_Require( getSingleton().m_instance );
		getSingleton().m_instance->pushMessage( LogType::eError, msg );
	}

	void Logger::logError( std::ostream const & msg )
	{
		std::stringstream ss;
		ss << msg.rdbuf();
		logError( ss.str() );
	}

	void Logger::logError( std::wstring const & msg )
	{
		CU_Require( getSingleton().m_instance );
		getSingleton().m_instance->pushMessage( LogType::eError, string::stringCast< char >( msg ) );
	}

	void Logger::logError( std::wostream const & msg )
	{
		std::wstringstream ss;
		ss << msg.rdbuf();
		logError( ss.str() );
	}

	void Logger::logTraceNoNL( std::string const & msg )
	{
		CU_Require( getSingleton().m_instance );
		getSingleton().m_instance->pushMessage( LogType::eTrace, msg, false );
	}

	void Logger::logTraceNoNL( std::ostream const & msg )
	{
		auto sbuf = msg.rdbuf();
		std::stringstream ss;
		ss << sbuf;
		logTraceNoNL( ss.str() );
	}

	void Logger::logTraceNoNL( std::wstring const & msg )
	{
		CU_Require( getSingleton().m_instance );
		getSingleton().m_instance->pushMessage( LogType::eTrace, string::stringCast< char >( msg ), false );
	}

	void Logger::logTraceNoNL( std::wostream const & msg )
	{
		std::wstringstream ss;
		ss << msg.rdbuf();
		logTraceNoNL( ss.str() );
	}

	void Logger::logDebugNoNL( std::string const & msg )
	{
		CU_Require( getSingleton().m_instance );
		getSingleton().m_instance->pushMessage( LogType::eDebug, msg, false );
	}

	void Logger::logDebugNoNL( std::ostream const & msg )
	{
		auto sbuf = msg.rdbuf();
		std::stringstream ss;
		ss << sbuf;
		logDebugNoNL( ss.str() );
	}

	void Logger::logDebugNoNL( std::wstring const & msg )
	{
		CU_Require( getSingleton().m_instance );
		getSingleton().m_instance->pushMessage( LogType::eDebug, string::stringCast< char >( msg ), false );
	}

	void Logger::logDebugNoNL( std::wostream const & msg )
	{
		std::wstringstream ss;
		ss << msg.rdbuf();
		logDebugNoNL( ss.str() );
	}

	void Logger::logInfoNoNL( std::string const & msg )
	{
		CU_Require( getSingleton().m_instance );
		getSingleton().m_instance->pushMessage( LogType::eInfo, msg, false );
	}

	void Logger::logInfoNoNL( std::ostream const & msg )
	{
		std::stringstream ss;
		ss << msg.rdbuf();
		logInfoNoNL( ss.str() );
	}

	void Logger::logInfoNoNL( std::wstring const & msg )
	{
		CU_Require( getSingleton().m_instance );
		getSingleton().m_instance->pushMessage( LogType::eInfo, string::stringCast< char >( msg ), false );
	}

	void Logger::logInfoNoNL( std::wostream const & msg )
	{
		std::wstringstream ss;
		ss << msg.rdbuf();
		logInfoNoNL( ss.str() );
	}

	void Logger::logWarningNoNL( std::string const & msg )
	{
		CU_Require( getSingleton().m_instance );
		getSingleton().m_instance->pushMessage( LogType::eWarning, msg, false );
	}

	void Logger::logWarningNoNL( std::ostream const & msg )
	{
		std::stringstream ss;
		ss << msg.rdbuf();
		logWarningNoNL( ss.str() );
	}

	void Logger::logWarningNoNL( std::wstring const & msg )
	{
		CU_Require( getSingleton().m_instance );
		getSingleton().m_instance->pushMessage( LogType::eWarning, string::stringCast< char >( msg ), false );
	}

	void Logger::logWarningNoNL( std::wostream const & msg )
	{
		std::wstringstream ss;
		ss << msg.rdbuf();
		logWarningNoNL( ss.str() );
	}

	void Logger::logErrorNoNL( std::string const & msg )
	{
		CU_Require( getSingleton().m_instance );
		getSingleton().m_instance->pushMessage( LogType::eError, msg, false );
	}

	void Logger::logErrorNoNL( std::ostream const & msg )
	{
		std::stringstream ss;
		ss << msg.rdbuf();
		logErrorNoNL( ss.str() );
	}

	void Logger::logErrorNoNL( std::wstring const & msg )
	{
		CU_Require( getSingleton().m_instance );
		getSingleton().m_instance->pushMessage( LogType::eError, string::stringCast< char >( msg ), false );
	}

	void Logger::logErrorNoNL( std::wostream const & msg )
	{
		std::wstringstream ss;
		ss << msg.rdbuf();
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
