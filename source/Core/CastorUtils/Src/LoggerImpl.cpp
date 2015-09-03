#include "CastorUtilsPch.hpp"

#include "LoggerImpl.hpp"

#include "LoggerConsole.hpp"
#include "Logger.hpp"
#include "StringUtils.hpp"
#include "TextFile.hpp"
#include "Utils.hpp"

namespace Castor
{
	class LoggerImpl;

	LoggerImpl::LoggerImpl()
	{
#if defined( NDEBUG )
		m_console = std::make_unique< DefaultConsole >();
#else
		m_console = std::make_unique< DebugConsole >();
#endif
	}

	LoggerImpl::~LoggerImpl()
	{
		m_console.reset();
	}

	void LoggerImpl::Initialise( Logger const & logger )
	{
		for ( int i = 0; i < ELogType_COUNT; i++ )
		{
			m_headers[i] = logger.m_headers[i];
		}
	}

	void LoggerImpl::Cleanup()
	{
	}

	void LoggerImpl::RegisterCallback( LogCallback p_pfnCallback, void * p_pCaller )
	{
		m_mapCallbacks[p_pCaller] = p_pfnCallback;
	}

	void LoggerImpl::UnregisterCallback( void * p_pCaller )
	{
		auto && l_it = m_mapCallbacks.find( p_pCaller );

		if ( l_it != m_mapCallbacks.end() )
		{
			m_mapCallbacks.erase( l_it );
		}
	}

	void LoggerImpl::SetFileName( String const & p_logFilePath, ELogType p_eLogType )
	{
		if ( p_eLogType == ELogType_COUNT )
		{
			for ( auto & path : m_logFilePath )
			{
				path = p_logFilePath;
			}
		}
		else
		{
			m_logFilePath[p_eLogType] = p_logFilePath;
		}

		FILE * file;
		Castor::FOpen( file, str_utils::to_str( p_logFilePath ).c_str(), "w" );

		if ( file )
		{
			fclose( file );
		}
	}

	void LoggerImpl::PrintMessage( ELogType logLevel, std::string const & message )
	{
		DoPrintMessage( logLevel, str_utils::from_str( message ) );
	}

	void LoggerImpl::PrintMessage( ELogType logLevel, std::wstring const & message )
	{
		DoPrintMessage( logLevel, str_utils::from_wstr( message ) );
	}

	void LoggerImpl::LogMessageQueue( MessageQueue const & p_queue )
	{
		std::tm l_dtToday = { 0 };
		time_t l_tTime;
		time( &l_tTime );
		Castor::Localtime( &l_dtToday, &l_tTime );
		char l_buffer[33] = { 0 };
		strftime( l_buffer, 32, "%Y-%m-%d %H:%M:%S", &l_dtToday );
		String l_timeStamp = str_utils::from_str( l_buffer );

		FILE * logFiles[ELogType_COUNT] = { NULL };
		std::map< String, FILE * > opened;

		try
		{
			for ( auto && message : p_queue )
			{
				FILE * file = logFiles[message->m_type];

				if ( !file )
				{
					auto && l_it = opened.find( m_logFilePath[message->m_type] );

					if ( l_it == opened.end() )
					{
						FOpen( file, str_utils::to_str( m_logFilePath[message->m_type] ).c_str(), "a" );

						if ( file )
						{
							opened.insert( std::make_pair( m_logFilePath[message->m_type], file ) );
						}
					}
					else
					{
						file = l_it->second;
					}

					logFiles[message->m_type] = file;
				}

				if ( file )
				{
					String toLog = message->GetMessage();

					if ( toLog.find( cuT( '\n' ) ) != String::npos )
					{
						StringArray array = str_utils::split( toLog, cuT( "\n" ), uint32_t( std::count( toLog.begin(), toLog.end(), cuT( '\n' ) ) + 1 ) );

						for ( auto && line : array )
						{
							DoLogLine( l_timeStamp, line, file, message->m_type );
						}
					}
					else
					{
						DoLogLine( l_timeStamp, toLog, file, message->m_type );
					}
				}
			}

			for ( auto && it : opened )
			{
				fclose( it.second );
			}
		}
		catch ( std::exception & )
		{
			//m_pConsole->Print( cuT( "Couldn't open log file : " ) + CStrUtils::ToString( exc.what() ), true );
		}
	}

	void LoggerImpl::DoPrintMessage( ELogType logLevel, String const & message )
	{
		if ( message.find( cuT( '\n' ) ) != String::npos )
		{
			StringArray array = str_utils::split( message, cuT( "\n" ), uint32_t( std::count( message.begin(), message.end(), cuT( '\n' ) ) + 1 ) );

			for ( auto && line : array )
			{
				DoPrintLine( line, logLevel );
			}
		}
		else
		{
			DoPrintLine( message, logLevel );
		}
	}

	void LoggerImpl::DoPrintLine( String const & line, ELogType logLevel )
	{
		m_console->BeginLog( logLevel );
		m_console->Print( line, true );
	}

	void LoggerImpl::DoLogLine( String const & timestamp, String const & line, FILE * logFile, ELogType logLevel )
	{
#if defined( NDEBUG )
		DoPrintLine( line, logLevel );
#endif
		for ( auto && l_it : m_mapCallbacks )
		{
			l_it.second( line, logLevel );
		}

		if ( logFile )
		{
			std::string logLine = str_utils::to_str( timestamp + cuT( " - " ) + m_headers[logLevel] + line );
			fwrite( logLine.c_str(), 1, logLine.size(), logFile );
			fwrite( "\n", 1, 1, logFile );
		}
	}
}

