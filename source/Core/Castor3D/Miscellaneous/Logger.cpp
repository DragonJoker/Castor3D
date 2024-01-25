/*
See LICENSE file in root folder
*/
#include "Castor3D/Miscellaneous/Logger.hpp"

#include <CastorUtils/Log/LoggerImpl.hpp>

#include <RenderGraph/Log.hpp>

namespace castor3d
{
	castor::LoggerInstance * log::m_logger{ nullptr };
	castor::LoggerStreamT< xchar, castor::TraceLoggerStreambufT > log::trace;
	castor::LoggerStreamT< xchar, castor::DebugLoggerStreambufT > log::debug;
	castor::LoggerStreamT< xchar, castor::InfoLoggerStreambufT > log::info;
	castor::LoggerStreamT< xchar, castor::WarningLoggerStreambufT > log::warn;
	castor::LoggerStreamT< xchar, castor::ErrorLoggerStreambufT > log::error;

	castor::LoggerInstance * log::initialise( castor::LoggerInstance & logger )
	{
		m_logger = &logger;
		trace.set( *m_logger );
		debug.set( *m_logger );
		info.set( *m_logger );
		warn.set( *m_logger );
		error.set( *m_logger );
		ashes::Logger::setTraceCallback( []( castor::MbString const & msg, bool newLine )
			{
				m_logger->pushMessage( castor::LogType::eTrace, msg, newLine );
			} );
		ashes::Logger::setDebugCallback( []( castor::MbString const & msg, bool newLine )
			{
				m_logger->pushMessage( castor::LogType::eDebug, msg, newLine );
			} );
		ashes::Logger::setInfoCallback( []( castor::MbString const & msg, bool newLine )
			{
				m_logger->pushMessage( castor::LogType::eInfo, msg, newLine );
			} );
		ashes::Logger::setWarningCallback( []( castor::MbString const & msg, bool newLine )
			{
				m_logger->pushMessage( castor::LogType::eWarning, msg, newLine );
			} );
		ashes::Logger::setErrorCallback( []( castor::MbString const & msg, bool newLine )
			{
				m_logger->pushMessage( castor::LogType::eError, msg, newLine );
			} );
		crg::Logger::setTraceCallback( []( castor::MbString const & msg, bool newLine )
			{
				m_logger->pushMessage( castor::LogType::eTrace, msg, newLine );
			} );
		crg::Logger::setDebugCallback( []( castor::MbString const & msg, bool newLine )
			{
				m_logger->pushMessage( castor::LogType::eDebug, msg, newLine );
			} );
		crg::Logger::setInfoCallback( []( castor::MbString const & msg, bool newLine )
			{
				m_logger->pushMessage( castor::LogType::eInfo, msg, newLine );
			} );
		crg::Logger::setWarningCallback( []( castor::MbString const & msg, bool newLine )
			{
				m_logger->pushMessage( castor::LogType::eWarning, msg, newLine );
			} );
		crg::Logger::setErrorCallback( []( castor::MbString const & msg, bool newLine )
			{
				m_logger->pushMessage( castor::LogType::eError, msg, newLine );
			} );
		return m_logger;
	}

	void log::cleanup()
	{
		crg::Logger::setTraceCallback( []( castor::MbString const &, bool )
			{
			} );
		crg::Logger::setDebugCallback( []( castor::MbString const & msg, bool newLine )
			{
				std::clog << msg << ( newLine ? "\n" : "" );
			} );
		crg::Logger::setInfoCallback( []( castor::MbString const & msg, bool newLine )
			{
				std::cout << msg << ( newLine ? "\n" : "" );
			} );
		crg::Logger::setWarningCallback( []( castor::MbString const & msg, bool newLine )
			{
				std::cout << msg << ( newLine ? "\n" : "" );
			} );
		crg::Logger::setErrorCallback( []( castor::MbString const & msg, bool newLine )
			{
				std::cerr << msg << ( newLine ? "\n" : "" );
			} );
		ashes::Logger::setTraceCallback( []( castor::MbString const &, bool )
			{
			} );
		ashes::Logger::setDebugCallback( []( castor::MbString const & msg, bool newLine )
			{
				std::clog << msg << ( newLine ? "\n" : "" );
			} );
		ashes::Logger::setInfoCallback( []( castor::MbString const & msg, bool newLine )
			{
				std::cout << msg << ( newLine ? "\n" : "" );
			} );
		ashes::Logger::setWarningCallback( []( castor::MbString const & msg, bool newLine )
			{
				std::cout << msg << ( newLine ? "\n" : "" );
			} );
		ashes::Logger::setErrorCallback( []( castor::MbString const & msg, bool newLine )
			{
				std::cerr << msg << ( newLine ? "\n" : "" );
			} );
		trace.reset();
		debug.reset();
		info.reset();
		warn.reset();
		error.reset();
		m_logger = nullptr;
	}
}
