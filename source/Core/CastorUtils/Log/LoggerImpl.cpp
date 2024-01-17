#include "CastorUtils/Log/LoggerImpl.hpp"

#include "CastorUtils/Log/LoggerConsole.hpp"
#include "CastorUtils/Log/Logger.hpp"
#include "CastorUtils/Data/TextFile.hpp"
#include "CastorUtils/Miscellaneous/Utils.hpp"

#include "CastorUtils/Config/MultiThreadConfig.hpp"

namespace castor
{
	LoggerImpl::LoggerImpl( LoggerImpl && rhs )noexcept
		: m_parent{ rhs.m_parent }
		, m_console{ std::move( rhs.m_console ) }
		, m_logFilePath{ std::move( rhs.m_logFilePath ) }
		, m_mapCallbacks{ std::move( rhs.m_mapCallbacks ) }
	{
		rhs.m_console = nullptr;
	}

	LoggerImpl & LoggerImpl::operator=( LoggerImpl && rhs )noexcept
	{
		m_console = std::move( rhs.m_console );
		m_logFilePath = std::move( rhs.m_logFilePath );
		m_mapCallbacks = std::move( rhs.m_mapCallbacks );
		rhs.m_console = nullptr;
		return *this;
	}

	LoggerImpl::LoggerImpl( ProgramConsole & console
		, CU_UnusedParam( LogType, level )
		, LoggerInstance & parent )
		: m_parent{ parent }
		, m_console{ &console }
	{
	}

	void LoggerImpl::registerCallback( LogCallback callback, void * caller )
	{
		auto lock( makeUniqueLock( m_mutexCallbacks ) );
		m_mapCallbacks[caller] = std::move( callback );
	}

	void LoggerImpl::unregisterCallback( void * caller )
	{
		auto lock( makeUniqueLock( m_mutexCallbacks ) );
		auto it = m_mapCallbacks.find( caller );

		if ( it != m_mapCallbacks.end() )
		{
			m_mapCallbacks.erase( it );
		}
	}

	void LoggerImpl::setFileName( String const & logFilePath, LogType logType )
	{
		auto lock( makeUniqueLock( m_mutexFiles ) );

		if ( logType == LogType::eCount )
		{
			for ( auto & path : m_logFilePath )
			{
				path = logFilePath;
			}
		}
		else
		{
			m_logFilePath[size_t( logType )] = logFilePath;
		}

		FILE * file;
		castor::fileOpen( file, makePath( logFilePath ), "w" );

		if ( file )
		{
			fclose( file );
		}
	}

	void LoggerImpl::printMessage( LogType logLevel, std::string const & message, bool newLine )
	{
		doPrintMessage( logLevel, string::stringCast< xchar >( message ), newLine );
	}

	void LoggerImpl::printMessage( LogType logLevel, std::wstring const & message, bool newLine )
	{
		doPrintMessage( logLevel, string::stringCast< xchar >( message ), newLine );
	}

	void LoggerImpl::logMessageQueue( MessageQueue const & queue )
	{
		std::time_t endTime = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
		std::string time = std::ctime( &endTime );
		string::replace( time, "\n", std::string{} );
		String timeStamp = string::stringCast< xchar >( time );
		std::array< StringStream, size_t( LogType::eCount ) > logs
		{
			makeStringStream(),
			makeStringStream(),
			makeStringStream(),
			makeStringStream(),
			makeStringStream(),
		};

		try
		{
			for ( auto & message : queue )
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

			{
				auto lock( makeUniqueLock( m_mutexFiles ) );
				size_t i = 0;

				for ( auto const & stream : logs )
				{
					if ( String text = stream.str();
						!text.empty() )
					{
						try
						{
							TextFile file{ Path{ m_logFilePath[i] }, File::OpenMode::eAppend };
							file.writeText( text );
						}
						catch ( Exception & exc )
						{
							printf( "Unexpected exception while writing log messages: %s", exc.what() );
						}
					}

					++i;
				}
			}
		}
		catch ( std::exception & exc )
		{
			printf( "Couldn't open log file: %s", exc.what() );
		}
	}

	void LoggerImpl::doPrintMessage( LogType logLevel, String const & message, bool newLine )
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

			doPrintLine( *it, logLevel, newLine );
		}
		else
		{
			doPrintLine( message, logLevel, newLine );
		}
	}

	void LoggerImpl::doPrintLine( String const & line, LogType logLevel, bool newLine )
	{
		m_console->beginLog( logLevel );
		m_console->print( line, newLine );
	}

	void LoggerImpl::doLogLine( String const & timestamp, String const & line, StringStream & stream, LogType logLevel, bool newLine )
	{
#if defined( NDEBUG )
		doPrintLine( line, logLevel, newLine );
#endif

		{
			auto lock( makeUniqueLock( m_mutexCallbacks ) );

			if ( !m_mapCallbacks.empty() )
			{
				for ( auto const & [data, callback] : m_mapCallbacks )
				{
					callback( line, logLevel, newLine );
				}
			}
		}

		stream << timestamp << cuT( " - " ) << m_parent.getHeader( uint8_t( logLevel ) ) << line << ( newLine ? "\n" : "" );
	}
}
