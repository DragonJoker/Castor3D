#include "Application.hpp"

#include "FileUtils.hpp"
#include "MainFrame.hpp"

#include <wx/cmdline.h>

#include <fstream>

namespace common
{
	namespace
	{
		/**
		*\brief
		*	Surcharge de std::streambuf, permettant de personnaliser la sortie.
		*\param LogStreambufTraits
		*	Permet la personnalisation de la sortie.\n
		*	Doit impl�menter la fonction statique suivante :\n
		*	Log( string_type appName, string_type buffer )
		*/
		template< typename LogStreambufTraits >
		class LogStreambuf
			: public std::streambuf
		{
		public:
			using CharType = char;
			using string_type = std::basic_string< CharType >;
			using ostream_type = std::basic_ostream< CharType >;
			using streambuf_type = std::basic_streambuf< CharType >;
			using int_type = typename std::basic_streambuf< CharType >::int_type;
			using traits_type = typename std::basic_streambuf< CharType >::traits_type;
			/**
			*\brief
			*	Constructeur, �change le tampon du flux donn� avec celui-ci.
			*\param[in] appName
			*	Le nom de l'application.
			*\param[in,out] stream
			*	Le flux recevant ce tampon de flux.
			*/
			LogStreambuf( string_type const & appName, ostream_type & stream )
				: m_stream( stream )
				, m_appName{ appName }
			{
				m_old = m_stream.rdbuf( this );
			}
			/**
			*\brief
			*	Destructeur, r�tablit le tampon d'origine du flux.
			*/
			~LogStreambuf()
			{
				m_stream.rdbuf( m_old );
			}

		private:
			int_type overflow( int_type c = traits_type::eof() )override
			{
				if ( traits_type::eq_int_type( c, traits_type::eof() ) )
				{
					doSync();
				}
				else if ( c == '\n' )
				{
					doSync();
				}
				else if ( c == '\r' )
				{
					m_buffer += '\r';
					doSyncNoNl();
				}
				else
				{
					m_buffer += traits_type::to_char_type( c );
				}

				return c;
			}

			int doSync()
			{
				if ( !m_buffer.empty() )
				{
					LogStreambufTraits::Log( m_appName, m_buffer );
					m_buffer.clear();
				}

				return 0;
			}

			int doSyncNoNl()
			{
				if ( !m_buffer.empty() )
				{
					LogStreambufTraits::Log( m_appName, m_buffer );
					m_buffer.clear();
				}

				return 0;
			}

		private:
			string_type m_buffer;
			ostream_type & m_stream;
			streambuf_type * m_old;
			string_type m_appName;
		};

		enum class LogType
		{
			eDebug,
			eInfo,
			eWarning,
			eError,
		};
		/**
		*\brief
		*	Affiche la ligne donn�e dans la console de d�bogage.
		*\param[in] log
		*	La ligne � logger.
		*/
		void logDebugString( std::string const & log );
		/**
		*\brief
		*	Classe trait, pour afficher une ligne dans le logcat.
		*/
		template< LogType Type >
		struct TLogStreambufTraits
		{
			using CharType = char;
			using string_type = std::basic_string< CharType >;

			/**
			*\brief
			*	Affiche la ligne donn�e dans le logcat.
			*\param[in] appName
			*	Le nom de l'application.
			*\param[in] text
			*	La ligne � logger.
			*/
			static void Log( string_type const & appName
				, string_type const & text )
			{
				static std::string const LogName[]
				{
					"Debug",
					"Info",
					"Warning",
					"Error",
				};
				auto log = appName + " - " + LogName[size_t( Type )] + ": " + text + "\n";
				printf( "%s", log.c_str() );
				logDebugString( log );
			}
		};

		//! Sp�cialisation de TLogStreambufTraits pour les logs de d�bogage.
		using DebugLogStreambufTraits = TLogStreambufTraits< LogType::eDebug >;
		//! Sp�cialisation de TLogStreambufTraits pour les logs informatifs.
		using InfoLogStreambufTraits = TLogStreambufTraits< LogType::eInfo >;
		//! Sp�cialisation de TLogStreambufTraits pour les logs de warning.
		using WarningLogStreambufTraits = TLogStreambufTraits< LogType::eWarning >;
		//! Sp�cialisation de TLogStreambufTraits pour les logs d'erreur.
		using ErrorLogStreambufTraits = TLogStreambufTraits< LogType::eError >;

		std::string const LogFileName = "Debug.log";

		void logDebugString( std::string const & log )
		{
#if _MSC_VER
			::OutputDebugStringA( log.c_str() );
#endif
			std::tm today = { 0 };
			time_t ttime;
			time( &ttime );
#if _MSC_VER
			localtime_s( &today, &ttime );
#else
			today = *localtime( &ttime );
#endif			
			char buffer[33] = { 0 };
			strftime( buffer, 32, "%Y-%m-%d %H:%M:%S", &today );
			std::string timeStamp = buffer;

			std::ofstream file{ LogFileName, std::ios::app };

			if ( file )
			{
				file << timeStamp << " - " << log;
			}
		}

		void flushLogFile()
		{
			if ( wxFileExists( LogFileName ) )
			{
				wxRemoveFile( LogFileName );
			}
		}
	}

	App::App ( wxString const & name )
		: m_name{ name }
	{
	}

	bool App::OnInit()
	{
		flushLogFile();
		m_cout = new LogStreambuf< InfoLogStreambufTraits >( m_name.ToStdString()
			, std::cout );
		m_cerr = new LogStreambuf< ErrorLogStreambufTraits >( m_name.ToStdString()
			, std::cerr );
		m_clog = new LogStreambuf< DebugLogStreambufTraits >( m_name.ToStdString()
			, std::clog );

#if !defined( NDEBUG )
#	if defined( _WIN32 )

		if ( ::AllocConsole() )
		{
			m_allocated = true;
			FILE * dump;
			freopen_s( &dump, "conout$", "w", stdout );
			freopen_s( &dump, "conout$", "w", stderr );
		}
		else
		{
			DWORD lastError = ::GetLastError();

			if ( lastError == ERROR_ACCESS_DENIED )
			{
				FILE * dump;
				freopen_s( &dump, "conout$", "w", stdout );
				freopen_s( &dump, "conout$", "w", stderr );
			}
		}

#	endif
#endif

		wxInitAllImageHandlers();
		bool result = false;
		StringArray files;

		if ( listDirectoryFiles( getExecutableDirectory(), files, false ) )
		{
			for ( auto file : files )
			{
				if ( file.find( ".dll" ) != std::string::npos 
					|| file.find( ".so" ) != std::string::npos )
				try
				{
					renderer::DynamicLibrary lib{ file };
					m_plugins.emplace_back( std::move( lib )
						, m_factory );
				}
				catch ( std::exception & exc )
				{
					std::clog << exc.what() << std::endl;
				}
			}
		}

		try
		{
			if ( doParseCommandLine() )
			{
				m_mainFrame = doCreateMainFrame( m_rendererName );
				m_mainFrame->initialise();
				SetTopWindow( m_mainFrame );
				result = true;
			}
		}
		catch ( std::exception & exc )
		{
			std::cerr << exc.what() << std::endl;
		}

		return result;
	}

	int App::OnExit()
	{
		wxImage::CleanUpHandlers();

#if !defined( NDEBUG )
#	if defined( _WIN32 )

		if ( m_allocated )
		{
			::FreeConsole();
		}

#	endif
#endif

		m_plugins.clear();
		delete m_cout;
		delete m_cerr;
		delete m_clog;
		return wxApp::OnExit();
	}

	void App::updateFps( std::chrono::microseconds const & duration )
	{
		m_mainFrame->updateFps( duration );
	}

	bool App::doParseCommandLine()
	{
		wxCmdLineParser parser( wxApp::argc, wxApp::argv );
		parser.AddSwitch( wxT( "h" ), wxT( "help" ), _( "Displays this help" ) );
		parser.AddSwitch( wxT( "gl" ), wxEmptyString, _( "Defines the renderer to OpenGl" ) );
		parser.AddSwitch( wxT( "vk" ), wxEmptyString, _( "Defines the renderer to Vulkan" ) );
		bool result = parser.Parse( false ) == 0;

		// S'il y avait des erreurs ou "-h" ou "--help", on affiche l'aide et on sort
		if ( !result || parser.Found( wxT( 'h' ) ) )
		{
			parser.Usage();
			result = false;
		}

		if ( result )
		{
			m_rendererName = wxT( "vk" );

			if ( parser.Found( wxT( "vk" ) ) )
			{
				m_rendererName = wxT( "vk" );
			}
			else if ( parser.Found( wxT( "gl" ) ) )
			{
				m_rendererName = wxT( "gl" );
			}
		}

		return result;
	}
};
