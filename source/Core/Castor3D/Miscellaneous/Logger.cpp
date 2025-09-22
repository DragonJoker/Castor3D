/*
See LICENSE file in root folder
*/
#include "Castor3D/Miscellaneous/Logger.hpp"

#include <CastorUtils/Log/LoggerImpl.hpp>

#include <RenderGraph/Log.hpp>

#if defined( CU_PlatformWindows )
#	if defined( CU_UseWChar )
#		include <cwchar>
#		define c3dvsprintf vswprintf_s
#	else
#		define c3dvsprintf vsprintf_s
#	endif
#elif defined( CU_UseWChar )
#	include <cwchar>
#	define c3dvsprintf vswprintf
#else
#	define c3dvsprintf vsnprintf
#endif

namespace c3d
{
	LoggerInstance * log::m_logger{ nullptr };
	LoggerStreamT< xchar, TraceLoggerStreambufT > log::trace;
	LoggerStreamT< xchar, DebugLoggerStreambufT > log::debug;
	LoggerStreamT< xchar, InfoLoggerStreambufT > log::info;
	LoggerStreamT< xchar, WarningLoggerStreambufT > log::warn;
	LoggerStreamT< xchar, ErrorLoggerStreambufT > log::error;

	LoggerInstance * log::initialise( LoggerInstance & logger )
	{
		m_logger = &logger;
		trace.set( *m_logger );
		debug.set( *m_logger );
		info.set( *m_logger );
		warn.set( *m_logger );
		error.set( *m_logger );
		ashes::Logger::setTraceCallback( []( MbString const & msg, bool newLine )
			{
				m_logger->pushMessage( LogType::eTrace, msg, newLine );
			} );
		ashes::Logger::setDebugCallback( []( MbString const & msg, bool newLine )
			{
				m_logger->pushMessage( LogType::eDebug, msg, newLine );
			} );
		ashes::Logger::setInfoCallback( []( MbString const & msg, bool newLine )
			{
				m_logger->pushMessage( LogType::eInfo, msg, newLine );
			} );
		ashes::Logger::setWarningCallback( []( MbString const & msg, bool newLine )
			{
				m_logger->pushMessage( LogType::eWarning, msg, newLine );
			} );
		ashes::Logger::setErrorCallback( []( MbString const & msg, bool newLine )
			{
				m_logger->pushMessage( LogType::eError, msg, newLine );
			} );
		crg::Logger::setTraceCallback( []( MbStringView msg, bool newLine )noexcept
			{
				m_logger->pushMessage( LogType::eTrace, MbString{ msg }, newLine );
			} );
		crg::Logger::setDebugCallback( []( MbStringView msg, bool newLine )noexcept
			{
				m_logger->pushMessage( LogType::eDebug, MbString{ msg }, newLine );
			} );
		crg::Logger::setInfoCallback( []( MbStringView msg, bool newLine )noexcept
			{
				m_logger->pushMessage( LogType::eInfo, MbString{ msg }, newLine );
			} );
		crg::Logger::setWarningCallback( []( MbStringView msg, bool newLine )noexcept
			{
				m_logger->pushMessage( LogType::eWarning, MbString{ msg }, newLine );
			} );
		crg::Logger::setErrorCallback( []( MbStringView msg, bool newLine )noexcept
			{
				m_logger->pushMessage( LogType::eError, MbString{ msg }, newLine );
			} );
		return m_logger;
	}

	void log::cleanup()
	{
		crg::Logger::setTraceCallback( []( MbStringView, bool )noexcept
			{
			} );
		crg::Logger::setDebugCallback( []( MbStringView msg, bool newLine )noexcept
			{
				std::clog << msg << ( newLine ? "\n" : "" );
			} );
		crg::Logger::setInfoCallback( []( MbStringView msg, bool newLine )noexcept
			{
				std::cout << msg << ( newLine ? "\n" : "" );
			} );
		crg::Logger::setWarningCallback( []( MbStringView msg, bool newLine )noexcept
			{
				std::cout << msg << ( newLine ? "\n" : "" );
			} );
		crg::Logger::setErrorCallback( []( MbStringView msg, bool newLine )noexcept
			{
				std::cerr << msg << ( newLine ? "\n" : "" );
			} );
		ashes::Logger::setTraceCallback( []( MbString const &, bool )
			{
			} );
		ashes::Logger::setDebugCallback( []( MbString const & msg, bool newLine )
			{
				std::clog << msg << ( newLine ? "\n" : "" );
			} );
		ashes::Logger::setInfoCallback( []( MbString const & msg, bool newLine )
			{
				std::cout << msg << ( newLine ? "\n" : "" );
			} );
		ashes::Logger::setWarningCallback( []( MbString const & msg, bool newLine )
			{
				std::cout << msg << ( newLine ? "\n" : "" );
			} );
		ashes::Logger::setErrorCallback( []( MbString const & msg, bool newLine )
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

	void log::printTrace( xchar const * fmt, ... )
	{
		if ( fmt )
		{
			Array< xchar, 1024u > text;
			memset( text.data(), 0, sizeof( xchar ) * 1024 );

			va_list ap;
			va_start( ap, fmt );
			c3dvsprintf( text.data(), 1023, fmt, ap );
			va_end( ap );

			trace << text.data();
		}
	}

	void log::printDebug( xchar const * fmt, ... )
	{
		if ( fmt )
		{
			Array< xchar, 1024u > text;
			memset( text.data(), 0, sizeof( xchar ) * 1024 );

			va_list ap;
			va_start( ap, fmt );
			c3dvsprintf( text.data(), 1023, fmt, ap );
			va_end( ap );

			debug << text.data();
		}
	}

	void log::printInfo( xchar const * fmt, ... )
	{
		if ( fmt )
		{
			Array< xchar, 1024u > text;
			memset( text.data(), 0, sizeof( xchar ) * 1024 );

			va_list ap;
			va_start( ap, fmt );
			c3dvsprintf( text.data(), 1023, fmt, ap );
			va_end( ap );

			info << text.data();
		}
	}

	void log::printWarn( xchar const * fmt, ... )
	{
		if ( fmt )
		{
			Array< xchar, 1024u > text;
			memset( text.data(), 0, sizeof( xchar ) * 1024 );

			va_list ap;
			va_start( ap, fmt );
			c3dvsprintf( text.data(), 1023, fmt, ap );
			va_end( ap );

			warn << text.data();
		}
	}

	void log::printError( xchar const * fmt, ... )
	{
		if ( fmt )
		{
			Array< xchar, 1024u > text;
			memset( text.data(), 0, sizeof( xchar ) * 1024 );

			va_list ap;
			va_start( ap, fmt );
			c3dvsprintf( text.data(), 1023, fmt, ap );
			va_end( ap );

			error << text.data();
		}
	}
}
