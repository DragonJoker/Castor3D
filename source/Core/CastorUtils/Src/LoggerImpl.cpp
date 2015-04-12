#include "LoggerImpl.hpp"
#include "Logger.hpp"
#include "TextFile.hpp"
#include "Utils.hpp"

namespace Castor
{
	ILoggerImpl::ILoggerImpl( eLOG_TYPE p_eLogLevel )
		:	m_eLogLevel( p_eLogLevel	)
		,	m_pConsole( NULL	)
	{
#if defined( NDEBUG )

		if ( p_eLogLevel == eLOG_TYPE_DEBUG )
		{
			m_pConsole = new DebugConsole;
		}
		else
		{
			m_pConsole = new DummyConsole;
		}

#else
		m_pConsole = new DebugConsole;
#endif
	}

	ILoggerImpl::~ILoggerImpl()
	{
		delete m_pConsole;
	}

	void ILoggerImpl::Initialise( Logger * p_pLogger )
	{
		for ( int i = 0; i < eLOG_TYPE_COUNT; i++ )
		{
			m_strHeaders[i] = p_pLogger->m_strHeaders[i];
		}

		m_outThread = std::thread( [&]()
		{
			std::string l_line;
			std::wstring l_wline;
			bool ended = false;

			while ( !ended )
			{
				std::unique_lock< std::mutex > lock( m_outMutex );

				while ( !m_cout.eof() )
				{
					std::getline( m_cout, l_line );

					if ( !l_line.empty() )
					{
						DoLogMessage( str_utils::from_str( l_line ), eLOG_TYPE_MESSAGE );
					}
				}

				while ( !m_wcout.eof() )
				{
					std::getline( m_wcout, l_wline );
					
					if ( !l_wline.empty() )
					{
						DoLogMessage( str_utils::from_wstr( l_wline ), eLOG_TYPE_MESSAGE );
					}
				}
				
				m_cout.clear();
				m_wcout.clear();

				while ( !m_cerr.eof() )
				{
					std::getline( m_cerr, l_line );
					
					if ( !l_line.empty() )
					{
						DoLogMessage( str_utils::from_str( l_line ), eLOG_TYPE_ERROR );
					}
				}

				while ( !m_wcerr.eof() )
				{
					std::getline( m_wcerr, l_wline );
					
					if ( !l_wline.empty() )
					{
						DoLogMessage( str_utils::from_wstr( l_wline ), eLOG_TYPE_ERROR );
					}
				}
				
				m_cerr.clear();
				m_wcerr.clear();

				while ( !m_clog.eof() )
				{
					std::getline( m_clog, l_line );

					if ( !l_line.empty() )
					{
						DoLogMessage( str_utils::from_str( l_line ), eLOG_TYPE_DEBUG );
					}
				}

				while ( !m_wclog.eof() )
				{
					std::getline( m_wclog, l_wline );

					if ( !l_wline.empty() )
					{
						DoLogMessage( str_utils::from_wstr( l_wline ), eLOG_TYPE_DEBUG );
					}
				}
				
				m_clog.clear();
				m_wclog.clear();

				ended = m_end.wait_for( lock, std::chrono::milliseconds( 5 ) ) != std::cv_status::timeout;
			}
		} );
	}

	void ILoggerImpl::Cleanup()
	{
		if ( m_outThread.joinable() )
		{
			m_end.notify_one();
			m_outThread.join();
		}
	}

	void ILoggerImpl::SetCallback( PLogCallback p_pfnCallback, void * p_pCaller )
	{
		if ( p_pfnCallback )
		{
			stLOGGER_CALLBACK l_callback = { p_pfnCallback, p_pCaller };
			m_mapCallbacks[std::this_thread::get_id()] = l_callback;
		}
		else
		{
			LoggerCallbackMapIt l_it = m_mapCallbacks.find( std::this_thread::get_id() );

			if ( l_it != m_mapCallbacks.end() )
			{
				m_mapCallbacks.erase( l_it );
			}
		}
	}

	void ILoggerImpl::SetFileName( String const & p_logFilePath, eLOG_TYPE p_eLogType )
	{
		if ( p_eLogType == eLOG_TYPE_COUNT )
		{
			for ( int i = 0; i < eLOG_TYPE_COUNT; i++ )
			{
				m_logFilePath[i] = p_logFilePath;

				if ( i == eLOG_TYPE_MESSAGE )
				{
					std::cout.rdbuf( m_cout.rdbuf() );
					std::wcout.rdbuf( m_wcout.rdbuf() );
				}
				else if ( i == eLOG_TYPE_DEBUG )
				{
					std::clog.rdbuf( m_clog.rdbuf() );
					std::wclog.rdbuf( m_wclog.rdbuf() );
				}
				else if ( i == eLOG_TYPE_ERROR )
				{
					std::cerr.rdbuf( m_cerr.rdbuf() );
					std::wcerr.rdbuf( m_wcerr.rdbuf() );
				}
			}
		}
		else
		{
			m_logFilePath[p_eLogType] = p_logFilePath;

			if ( p_eLogType == eLOG_TYPE_MESSAGE )
			{
				std::cout.rdbuf( m_cout.rdbuf() );
				std::wcout.rdbuf( m_wcout.rdbuf() );
			}
			else if ( p_eLogType == eLOG_TYPE_DEBUG )
			{
				std::clog.rdbuf( m_clog.rdbuf() );
				std::wclog.rdbuf( m_wclog.rdbuf() );
			}
			else if ( p_eLogType == eLOG_TYPE_ERROR )
			{
				std::cerr.rdbuf( m_cerr.rdbuf() );
				std::wcerr.rdbuf( m_wcerr.rdbuf() );
			}
		}

		FILE * l_pFile;
		Castor::FOpen( l_pFile, str_utils::to_str( p_logFilePath ).c_str(), "w" );

		if ( l_pFile )
		{
			fclose( l_pFile );
		}
	}

	void ILoggerImpl::LogDebug( String const & p_strToLog )
	{
		std::unique_lock< std::mutex > lock( m_outMutex );
		DoLogMessage( p_strToLog, eLOG_TYPE_DEBUG );
	}

	void ILoggerImpl::LogMessage( String const & p_strToLog )
	{
		std::unique_lock< std::mutex > lock( m_outMutex );
		DoLogMessage( p_strToLog, eLOG_TYPE_MESSAGE );
	}

	void ILoggerImpl::LogWarning( String const & p_strToLog )
	{
		std::unique_lock< std::mutex > lock( m_outMutex );
		DoLogMessage( p_strToLog, eLOG_TYPE_WARNING );
	}

	bool ILoggerImpl::LogError( String const & p_strToLog )
	{
		std::unique_lock< std::mutex > lock( m_outMutex );
		DoLogMessage( p_strToLog, eLOG_TYPE_ERROR );
		return true;
	}

	void ILoggerImpl::DoLogMessage( String const & p_strToLog, eLOG_TYPE p_eLogType )
	{
		CASTOR_MUTEX_AUTO_SCOPED_LOCK();
		StringStream l_strToLog;
		LoggerCallbackMapConstIt l_it;
		std::tm l_dtToday = { 0 };
		time_t l_tTime;
		time( &l_tTime );
		Castor::Localtime( &l_dtToday, &l_tTime );
		l_strToLog << ( l_dtToday.tm_year + 1900 ) << cuT( "-" );
		l_strToLog << ( l_dtToday.tm_mon + 1 < 10 ? cuT( "0" ) : cuT( "" ) ) << ( l_dtToday.tm_mon + 1 ) << cuT( "-" ) << ( l_dtToday.tm_mday < 10 ? cuT( "0" ) : cuT( "" ) ) << l_dtToday.tm_mday;
		l_strToLog << cuT( " - " ) << ( l_dtToday.tm_hour < 10 ? cuT( "0" ) : cuT( "" ) ) << l_dtToday.tm_hour << cuT( ":" ) << ( l_dtToday.tm_min < 10 ? cuT( "0" ) : cuT( "" ) ) << l_dtToday.tm_min << cuT( ":" ) << ( l_dtToday.tm_sec < 10 ? cuT( "0" ) : cuT( "" ) ) << l_dtToday.tm_sec << cuT( "s" );
		l_strToLog << cuT( " - " ) << m_strHeaders[p_eLogType];
		l_strToLog << p_strToLog;
		m_pConsole->BeginLog( p_eLogType );
		m_pConsole->Print( p_strToLog, true );

		try
		{
			TextFile l_logFile( m_logFilePath[p_eLogType], File::eOPEN_MODE_APPEND, File::eENCODING_MODE_ASCII );

			if ( l_logFile.IsOk() )
			{
				String l_strLog = l_strToLog.str();
				l_logFile.WriteText( l_strLog );
				l_logFile.Print( 2 * sizeof( xchar ), cuT( "\n" ) );
				l_it = m_mapCallbacks.find( std::this_thread::get_id() );

				if ( l_it != m_mapCallbacks.end() && l_it->second.m_pfnCallback )
				{
					l_it->second.m_pfnCallback( l_it->second.m_pCaller, l_strLog, p_eLogType );
				}
			}
		}
		catch ( std::exception & )
		{
			//m_pConsole->Print( cuT( "Couldn't open log file [" ) + m_logFilePath[p_eLogType] + cuT( "] : " ) + str_utils::from_str( exc.what() ), true );
		}
	}
}
