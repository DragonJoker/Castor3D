#include "LoggerImpl.hpp"

#include "LoggerConsole.hpp"
#include "Logger.hpp"
#include "Data/TextFile.hpp"
#include "Miscellaneous/Utils.hpp"

namespace castor
{
	class LoggerImpl;

	LoggerImpl::LoggerImpl( LogType p_level )
	{
		m_console = std::make_unique< ProgramConsole >( p_level < LogType::eInfo );
	}

	LoggerImpl::~LoggerImpl()
	{
		m_console.reset();
	}

	void LoggerImpl::initialise( Logger const & logger )
	{
		for ( uint8_t i = 0u; i < uint8_t( LogType::eCount ); i++ )
		{
			m_headers[i] = logger.m_headers[i];
		}
	}

	void LoggerImpl::cleanup()
	{
	}

	void LoggerImpl::registerCallback( LogCallback p_pfnCallback, void * p_pCaller )
	{
		std::lock_guard< std::mutex > lock( m_mutexCallbacks );
		m_mapCallbacks[p_pCaller] = p_pfnCallback;
	}

	void LoggerImpl::unregisterCallback( void * p_pCaller )
	{
		std::lock_guard< std::mutex > lock( m_mutexCallbacks );
		auto it = m_mapCallbacks.find( p_pCaller );

		if ( it != m_mapCallbacks.end() )
		{
			m_mapCallbacks.erase( it );
		}
	}

	void LoggerImpl::setFileName( String const & p_logFilePath, LogType p_eLogType )
	{
		if ( p_eLogType == LogType::eCount )
		{
			for ( auto & path : m_logFilePath )
			{
				path = p_logFilePath;
			}
		}
		else
		{
			m_logFilePath[size_t( p_eLogType )] = p_logFilePath;
		}

		FILE * file;
		castor::fileOpen( file, string::stringCast< char >( p_logFilePath ).c_str(), "w" );

		if ( file )
		{
			fclose( file );
		}
	}

	void LoggerImpl::printMessage( LogType logLevel, std::string const & message, bool p_newLine )
	{
		doPrintMessage( logLevel, string::stringCast< xchar >( message ), p_newLine );
	}

	void LoggerImpl::printMessage( LogType logLevel, std::wstring const & message, bool p_newLine )
	{
		doPrintMessage( logLevel, string::stringCast< xchar >( message ), p_newLine );
	}

	void LoggerImpl::logMessageQueue( MessageQueue const & p_queue )
	{
		std::tm dtToday = { 0 };
		time_t tTime;
		time( &tTime );
		castor::getLocaltime( &dtToday, &tTime );
		char buffer[33] = { 0 };
		strftime( buffer, 32, "%Y-%m-%d %H:%M:%S", &dtToday );
		String timeStamp = string::stringCast< xchar >( buffer );
		StringStream logs[size_t( LogType::eCount )];

		try
		{
			for ( auto & message : p_queue )
			{
				StringStream & stream = logs[size_t( message.m_type )];
				String toLog = message.m_message;

				if ( toLog.find( cuT( '\n' ) ) != String::npos )
				{
					StringArray array = string::split( toLog, cuT( "\n" ), uint32_t( std::count( toLog.begin(), toLog.end(), cuT( '\n' ) ) + 1 ) );
					auto it = array.begin();

					for ( size_t i = 0; i < array.size() - 1; ++i )
					{
						doLogLine( timeStamp, *it, stream, message.m_type, true );
						++it;
					}

					doLogLine( timeStamp, *it, stream, message.m_type, message.m_newLine );
				}
				else
				{
					doLogLine( timeStamp, toLog, stream, message.m_type, message.m_newLine );
				}
			}

			int i = 0;

			for ( auto const & stream : logs )
			{
				String text = stream.str();

				if ( !text.empty() )
				{
					try
					{
						TextFile file{ Path{ m_logFilePath[i++] }, File::OpenMode::eAppend };
						file.writeText( text );
					}
					catch ( Exception & )
					{
					}
				}
			}
		}
		catch ( std::exception & )
		{
			//m_pConsole->print( cuT( "Couldn't open log file : " ) + CStrUtils::toString( exc.what() ), true );
		}
	}

	void LoggerImpl::doPrintMessage( LogType logLevel, String const & message, bool p_newLine )
	{
		if ( message.find( cuT( '\n' ) ) != String::npos )
		{
			StringArray array = string::split( message, cuT( "\n" ), uint32_t( std::count( message.begin(), message.end(), cuT( '\n' ) ) + 1 ) );
			auto it = array.begin();

			for ( size_t i = 0; i < array.size() - 1; ++i )
			{
				doPrintLine( *it, logLevel, true );
				++it;
			}

			doPrintLine( *it, logLevel, p_newLine );
		}
		else
		{
			doPrintLine( message, logLevel, p_newLine );
		}
	}

	void LoggerImpl::doPrintLine( String const & line, LogType logLevel, bool p_newLine )
	{
		m_console->beginLog( logLevel );
		m_console->print( line, p_newLine );
	}

	void LoggerImpl::doLogLine( String const & timestamp, String const & line, StringStream & stream, LogType logLevel, bool p_newLine )
	{
#if defined( NDEBUG )
		doPrintLine( line, logLevel, p_newLine );
#endif

		{
			std::lock_guard< std::mutex > lock( m_mutexCallbacks );

			if ( !m_mapCallbacks.empty() )
			{
				for ( auto it : m_mapCallbacks )
				{
					it.second( line, logLevel, p_newLine );
				}
			}
		}

		stream << timestamp << cuT( " - " ) << m_headers[size_t( logLevel )] << line << ( p_newLine ? "\n" : "" );
	}
}
