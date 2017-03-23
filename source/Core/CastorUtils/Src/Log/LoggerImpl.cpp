#include "LoggerImpl.hpp"

#include "LoggerConsole.hpp"
#include "Logger.hpp"
#include "Data/TextFile.hpp"
#include "Miscellaneous/Utils.hpp"

namespace Castor
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

	void LoggerImpl::Initialise( Logger const & logger )
	{
		for ( uint8_t i = 0u; i < uint8_t( LogType::eCount ); i++ )
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
		auto l_it = m_mapCallbacks.find( p_pCaller );

		if ( l_it != m_mapCallbacks.end() )
		{
			m_mapCallbacks.erase( l_it );
		}
	}

	void LoggerImpl::SetFileName( String const & p_logFilePath, LogType p_eLogType )
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
		Castor::FOpen( file, string::string_cast< char >( p_logFilePath ).c_str(), "w" );

		if ( file )
		{
			fclose( file );
		}
	}

	void LoggerImpl::PrintMessage( LogType logLevel, std::string const & message, bool p_newLine )
	{
		DoPrintMessage( logLevel, string::string_cast< xchar >( message ), p_newLine );
	}

	void LoggerImpl::PrintMessage( LogType logLevel, std::wstring const & message, bool p_newLine )
	{
		DoPrintMessage( logLevel, string::string_cast< xchar >( message ), p_newLine );
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
		StringStream l_logs[size_t( LogType::eCount )];

		try
		{
			for ( auto & message : p_queue )
			{
				StringStream & l_stream = l_logs[size_t( message.m_type )];
				String l_toLog = message.m_message;

				if ( l_toLog.find( cuT( '\n' ) ) != String::npos )
				{
					StringArray l_array = string::split( l_toLog, cuT( "\n" ), uint32_t( std::count( l_toLog.begin(), l_toLog.end(), cuT( '\n' ) ) + 1 ) );
					auto l_it = l_array.begin();

					for ( size_t i = 0; i < l_array.size() - 1; ++i )
					{
						DoLogLine( l_timeStamp, *l_it, l_stream, message.m_type, true );
						++l_it;
					}

					DoLogLine( l_timeStamp, *l_it, l_stream, message.m_type, message.m_newLine );
				}
				else
				{
					DoLogLine( l_timeStamp, l_toLog, l_stream, message.m_type, message.m_newLine );
				}
			}

			int i = 0;

			for ( auto const & l_stream : l_logs )
			{
				String l_text = l_stream.str();

				if ( !l_text.empty() )
				{
					try
					{
						TextFile l_file{ Path{ m_logFilePath[i++] }, File::OpenMode::eAppend };
						l_file.WriteText( l_text );
					}
					catch ( Exception & )
					{
					}
				}
			}
		}
		catch ( std::exception & )
		{
			//m_pConsole->Print( cuT( "Couldn't open log file : " ) + CStrUtils::ToString( exc.what() ), true );
		}
	}

	void LoggerImpl::DoPrintMessage( LogType logLevel, String const & message, bool p_newLine )
	{
		if ( message.find( cuT( '\n' ) ) != String::npos )
		{
			StringArray l_array = string::split( message, cuT( "\n" ), uint32_t( std::count( message.begin(), message.end(), cuT( '\n' ) ) + 1 ) );
			auto l_it = l_array.begin();

			for ( size_t i = 0; i < l_array.size() - 1; ++i )
			{
				DoPrintLine( *l_it, logLevel, true );
				++l_it;
			}

			DoPrintLine( *l_it, logLevel, p_newLine );
		}
		else
		{
			DoPrintLine( message, logLevel, p_newLine );
		}
	}

	void LoggerImpl::DoPrintLine( String const & line, LogType logLevel, bool p_newLine )
	{
		m_console->BeginLog( logLevel );
		m_console->Print( line, p_newLine );
	}

	void LoggerImpl::DoLogLine( String const & timestamp, String const & line, StringStream & stream, LogType logLevel, bool p_newLine )
	{
#if defined( NDEBUG )
		DoPrintLine( line, logLevel, p_newLine );
#endif

		{
			std::lock_guard< std::mutex > l_lock( m_mutexCallbacks );

			if ( !m_mapCallbacks.empty() )
			{
				for ( auto l_it : m_mapCallbacks )
				{
					l_it.second( line, logLevel, p_newLine );
				}
			}
		}

		stream << timestamp << cuT( " - " ) << m_headers[size_t( logLevel )] << line << ( p_newLine ? "\n" : "" );
	}
}
