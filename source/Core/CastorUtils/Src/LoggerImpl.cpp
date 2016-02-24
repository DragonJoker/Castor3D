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
		std::lock_guard< std::mutex > l_lock( m_mutexCallbacks );
		m_mapCallbacks[p_pCaller] = p_pfnCallback;
	}

	void LoggerImpl::UnregisterCallback( void * p_pCaller )
	{
		std::lock_guard< std::mutex > l_lock( m_mutexCallbacks );
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
		Castor::FOpen( file, string::string_cast< char >( p_logFilePath ).c_str(), "w" );

		if ( file )
		{
			fclose( file );
		}
	}

	void LoggerImpl::PrintMessage( ELogType logLevel, std::string const & message )
	{
		DoPrintMessage( logLevel, string::string_cast< xchar >( message ) );
	}

	void LoggerImpl::PrintMessage( ELogType logLevel, std::wstring const & message )
	{
		DoPrintMessage( logLevel, string::string_cast< xchar >( message ) );
	}

	void LoggerImpl::LogMessageQueue( MessageQueue const & p_queue )
	{
		std::tm l_dtToday = { 0 };
		time_t l_tTime;
		time( &l_tTime );
		Castor::Localtime( &l_dtToday, &l_tTime );
		char l_buffer[33] = { 0 };
		strftime( l_buffer, 32, "%Y-%m-%d %H:%M:%S", &l_dtToday );
		String l_timeStamp = string::string_cast< xchar >( l_buffer );
		StringStream l_logs[ELogType_COUNT];

		try
		{
			for ( auto && message : p_queue )
			{
				StringStream & l_stream = l_logs[message->m_type];
				String l_toLog = message->GetMessage();

				if ( l_toLog.find( cuT( '\n' ) ) != String::npos )
				{
					StringArray l_array = string::split( l_toLog, cuT( "\n" ), uint32_t( std::count( l_toLog.begin(), l_toLog.end(), cuT( '\n' ) ) + 1 ) );

					for ( auto && l_line : l_array )
					{
						DoLogLine( l_timeStamp, l_line, l_stream, message->m_type );
					}
				}
				else
				{
					DoLogLine( l_timeStamp, l_toLog, l_stream, message->m_type );
				}
			}

			int i = 0;

			for ( auto && l_stream : l_logs )
			{
				String l_text = l_stream.str();

				if ( !l_text.empty() )
				{
					FILE * l_file = nullptr;
					FOpen( l_file, string::string_cast< char >( m_logFilePath[i++] ).c_str(), "a" );

					if ( l_file )
					{
						std::string l_log = string::string_cast< char >( l_text );
						fwrite( l_log.c_str(), sizeof( char ), l_log.size(), l_file );
						fclose( l_file );
					}
				}
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
			StringArray array = string::split( message, cuT( "\n" ), uint32_t( std::count( message.begin(), message.end(), cuT( '\n' ) ) + 1 ) );

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

	void LoggerImpl::DoLogLine( String const & timestamp, String const & line, StringStream & stream, ELogType logLevel )
	{
#if defined( NDEBUG )
		DoPrintLine( line, logLevel );
#endif

		{
			std::lock_guard< std::mutex > l_lock( m_mutexCallbacks );

			if ( !m_mapCallbacks.empty() )
			{
				for ( auto && l_it : m_mapCallbacks )
				{
					l_it.second( line, logLevel );
				}
			}
		}

		stream << timestamp << cuT( " - " ) << m_headers[logLevel] << line << std::endl;
	}
}

