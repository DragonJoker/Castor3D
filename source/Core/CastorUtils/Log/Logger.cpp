#include "CastorUtils/Log/Logger.hpp"

#include "CastorUtils/Exception/Assertion.hpp"
#include "CastorUtils/Exception/Exception.hpp"
#include "CastorUtils/Log/LoggerConsole.hpp"
#include "CastorUtils/Log/LoggerStreambuf.hpp"

namespace c3d
{
	static const MbString ERROR_LOGGER_ALREADY_INITIALISED = "Logger instance already initialised";

	c3d::RawUniquePtr< Logger > Logger::m_singleton = nullptr;

	Logger::Logger( LogType level )
		: m_console{ c3d::makeRawUnique< ProgramConsole >( level < LogType::eInfo ) }
		, m_instance{ c3d::makeRawUnique< LoggerInstance >( *m_console, level ) }
		, m_cout{ c3d::makeRawUnique< InfoLoggerStreambufT< char > >( *m_instance, std::cout ) }
		, m_cerr{ c3d::makeRawUnique< ErrorLoggerStreambufT< char > >( *m_instance, std::cerr ) }
		, m_clog{ c3d::makeRawUnique< DebugLoggerStreambufT< char > >( *m_instance, std::clog ) }
		, m_wcout{ c3d::makeRawUnique< InfoLoggerStreambufT< wchar_t > >( *m_instance, std::wcout ) }
		, m_wcerr{ c3d::makeRawUnique< ErrorLoggerStreambufT< wchar_t > >( *m_instance, std::wcerr ) }
		, m_wclog{ c3d::makeRawUnique< DebugLoggerStreambufT< wchar_t > >( *m_instance, std::wclog ) }
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
			m_singleton = c3d::makeRawUnique< Logger >( level );
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
		return c3d::makeRawUnique< LoggerInstance >( *getSingleton().m_console, logLevel );
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
