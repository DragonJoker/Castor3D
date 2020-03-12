/*
See LICENSE file in root folder
*/
#include "Castor3D/Miscellaneous/Logger.hpp"

#include <CastorUtils/Log/LoggerImpl.hpp>

namespace castor3d
{
	castor::LoggerInstance * log::m_logger{ nullptr };
	castor::LoggerStreamT< char, castor::TraceLoggerStreambufT > log::trace;
	castor::LoggerStreamT< char, castor::DebugLoggerStreambufT > log::debug;
	castor::LoggerStreamT< char, castor::InfoLoggerStreambufT > log::info;
	castor::LoggerStreamT< char, castor::WarningLoggerStreambufT > log::warn;
	castor::LoggerStreamT< char, castor::ErrorLoggerStreambufT > log::error;

	castor::LoggerInstance * log::initialise( castor::LogType type
		, castor::Path const & filePath )
	{
		m_logger = castor::Logger::createInstance( type );
		m_logger->setFileName( filePath );
		trace.set( *m_logger );
		debug.set( *m_logger );
		info.set( *m_logger );
		warn.set( *m_logger );
		error.set( *m_logger );
		ashes::Logger::setTraceCallback( []( std::string const & msg, bool newLine )
			{
				m_logger->pushMessage( castor::LogType::eTrace, msg, newLine );
			} );
		ashes::Logger::setDebugCallback( []( std::string const & msg, bool newLine )
			{
				m_logger->pushMessage( castor::LogType::eDebug, msg, newLine );
			} );
		ashes::Logger::setInfoCallback( []( std::string const & msg, bool newLine )
			{
				m_logger->pushMessage( castor::LogType::eInfo, msg, newLine );
			} );
		ashes::Logger::setWarningCallback( []( std::string const & msg, bool newLine )
			{
				m_logger->pushMessage( castor::LogType::eWarning, msg, newLine );
			} );
		ashes::Logger::setErrorCallback( []( std::string const & msg, bool newLine )
			{
				m_logger->pushMessage( castor::LogType::eError, msg, newLine );
			} );
		return m_logger;
	}

	void log::cleanup()
	{
		ashes::Logger::setTraceCallback( []( std::string const & msg, bool newLine )
			{
			} );
		ashes::Logger::setDebugCallback( []( std::string const & msg, bool newLine )
			{
				std::clog << msg << newLine ? "\n" : "";
			} );
		ashes::Logger::setInfoCallback( []( std::string const & msg, bool newLine )
			{
				std::cout << msg << newLine ? "\n" : "";
			} );
		ashes::Logger::setWarningCallback( []( std::string const & msg, bool newLine )
			{
				std::cout << msg << newLine ? "\n" : "";
			} );
		ashes::Logger::setErrorCallback( []( std::string const & msg, bool newLine )
			{
				std::cerr << msg << newLine ? "\n" : "";
			} );
		trace.reset();
		debug.reset();
		info.reset();
		warn.reset();
		error.reset();
		delete m_logger;
		m_logger = nullptr;
	}
}
