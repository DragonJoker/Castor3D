#include "CastorUtils/Log/Logger.hpp"

#include "CastorUtils/Exception/Assertion.hpp"
#include "CastorUtils/Exception/Exception.hpp"
#include "CastorUtils/Log/LoggerConsole.hpp"
#include "CastorUtils/Log/LoggerStreambuf.hpp"

namespace castor
{
	static const MbString ERROR_LOGGER_ALREADY_INITIALISED = "Logger instance already initialised";

	castor::RawUniquePtr< Logger > Logger::m_singleton = nullptr;

	Logger::Logger( LogType level )
		: m_console{ castor::make_unique< ProgramConsole >( level < LogType::eInfo ) }
		, m_instance{ castor::make_unique< LoggerInstance >( *m_console, level ) }
		, m_cout{ castor::make_unique< InfoLoggerStreambufT< char > >( *m_instance, std::cout ) }
		, m_cerr{ castor::make_unique< ErrorLoggerStreambufT< char > >( *m_instance, std::cerr ) }
		, m_clog{ castor::make_unique< DebugLoggerStreambufT< char > >( *m_instance, std::clog ) }
		, m_wcout{ castor::make_unique< InfoLoggerStreambufT< wchar_t > >( *m_instance, std::wcout ) }
		, m_wcerr{ castor::make_unique< ErrorLoggerStreambufT< wchar_t > >( *m_instance, std::wcerr ) }
		, m_wclog{ castor::make_unique< DebugLoggerStreambufT< wchar_t > >( *m_instance, std::wclog ) }
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
			m_singleton = castor::make_unique< Logger >( level );
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
		return castor::make_unique< LoggerInstance >( *getSingleton().m_console, logLevel );
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

	void Logger::doLogMessage( LogType type, MbString const & msg, bool addNL )
	{
		CU_Require( getSingleton().m_instance );
		getSingleton().m_instance->pushMessage( type, msg, addNL );
	}

	Logger & Logger::getSingleton()
	{
		if ( !m_singleton )
		{
			CU_Exception( "No Logger initialised." );
		}

		return *m_singleton;
	}

	Logger * Logger::getSingletonPtr()
	{
		return &getSingleton();
	}
}
