/*
See LICENSE file in root folder
*/
#ifndef ___CU_Logger_H___
#define ___CU_Logger_H___

#include "CastorUtils/Design/NonCopyable.hpp"
#include "CastorUtils/Log/LoggerInstance.hpp"

namespace castor
{
	class Logger
		: public NonMovable
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		CU_API explicit Logger( LogType level );
		/**
		 *\~english
		 *\brief		Initialises this logger instance level
		 *\param[in]	logLevel	The log level
		 *\~french
		 *\brief		Initialise l'instance du logger avec le niveau donné
		 *\param[in]	logLevel	Le niveau de log
		 */
		CU_API static LoggerInstance * initialise( LogType logLevel );
		/**
		 *\~english
		 *\brief		Destroys the Logger instance
		 *\~french
		 *\brief		Détruit l'instance du Logger
		 */
		CU_API static void cleanup();
		/**
		 *\~english
		 *\brief		Creates a logger instance of given level.
		 *\param[in]	logLevel	The log level.
		 *\~french
		 *\brief		Crée une instance de logger avec le niveau donné.
		 *\param[in]	logLevel	Le niveau de log.
		 */
		CU_API static LoggerInstancePtr createInstance( LogType logLevel );
		/**
		 *\~english
		 *\brief		Registers the logging callback
		 *\param[in]	callback	The callback
		 *\param[in]	caller		Pointer to user data, used to identify the callback
		 *\~french
		 *\brief		Enregistre un callback de log
		 *\param[in]	callback	Le callback
		 *\param[in]	caller		Pointeur sur des données utilisateur, utilisé pour identifier le callback
		 */
		CU_API static void registerCallback( LogCallback const & callback, void * caller );
		/**
		 *\~english
		 *\brief		Unregisters the logging callback
		 *\param[in]	caller	Pointer to user data
		 *\~french
		 *\brief		Désenregistre un callback de log
		 *\param[in]	caller	Pointeur sur des données utilisateur
		 */
		CU_API static void unregisterCallback( void * caller );
		/**
		 *\~english
		 *\brief		sets the log file address
		 *\param[in]	logFilePath	The log file path
		 *\param[in]	logType		The log type
		 *\~french
		 *\brief		Définit le chemin du fichier de log
		 *\param[in]	logFilePath	Le chemin du fichier
		 *\param[in]	logType		Le type de log concerné
		 */
		CU_API static void setFileName( Path const & logFilePath, LogType logType = LogType::eCount );
		/**
		 *\~english
		 *\return		The current log level.
		 *\~french
		 *\return		Le niveau de log actuel.
		 */
		CU_API static LogType getLevel();
		/**
		 *\~english
		 *\brief		Logs a trace message.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message trace.
		 *\param[in]	msg	La ligne a logger
		 */
		template< typename CharT >
		static void logTrace( CharT const * msg )
		{
			doLogMessage( LogType::eTrace, toUtf8( msg ), true );
		}
		/**
		 *\~english
		 *\brief		Logs a trace message.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message trace.
		 *\param[in]	msg	La ligne a logger
		 */
		template< typename CharT >
		static void logTrace( std::basic_string_view< CharT > msg )
		{
			doLogMessage( LogType::eTrace, toUtf8( msg ), true );
		}
		/**
		 *\~english
		 *\brief		Logs a trace message.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message trace.
		 *\param[in]	msg	La ligne a logger
		 */
		template< typename CharT >
		static void logTrace( std::basic_string< CharT > const & msg )
		{
			doLogMessage( LogType::eTrace, toUtf8( msg ), true );
		}
		/**
		 *\~english
		 *\brief		Logs a trace message.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message trace.
		 *\param[in]	msg	La ligne a logger
		 */
		template< typename CharT >
		static void logTrace( std::basic_ostream< CharT > const & msg )
		{
			auto sbuf = msg.rdbuf();
			std::basic_stringstream< CharT > ss;
			ss << sbuf;
			logTrace( ss.str() );
		}
		/**
		 *\~english
		 *\brief		Logs a trace message.
		 *\remarks		Doesn't print the new line character.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message trace.
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	msg	La ligne a logger
		 */
		template< typename CharT >
		static void logTraceNoNL( CharT const * msg )
		{
			doLogMessage( LogType::eTrace, toUtf8( msg ), false );
		}
		/**
		 *\~english
		 *\brief		Logs a trace message.
		 *\remarks		Doesn't print the new line character.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message trace.
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	msg	La ligne a logger
		 */
		template< typename CharT >
		static void logTraceNoNL( std::basic_string_view< CharT > msg )
		{
			doLogMessage( LogType::eTrace, toUtf8( msg ), false );
		}
		/**
		 *\~english
		 *\brief		Logs a trace message.
		 *\remarks		Doesn't print the new line character.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message trace.
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	msg	La ligne a logger
		 */
		template< typename CharT >
		static void logTraceNoNL( std::basic_string< CharT > const & msg )
		{
			doLogMessage( LogType::eTrace, toUtf8( msg ), false );
		}
		/**
		 *\~english
		 *\brief		Logs a trace message.
		 *\remarks		Doesn't print the new line character.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message trace.
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	msg	La ligne a logger
		 */
		template< typename CharT >
		static void logTraceNoNL( std::basic_ostream< CharT > const & msg )
		{
			auto sbuf = msg.rdbuf();
			std::basic_stringstream< CharT > ss;
			ss << sbuf;
			logTraceNoNL( ss.str() );
		}
		/**
		 *\~english
		 *\brief		Logs a debug message.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message debug.
		 *\param[in]	msg	La ligne a logger
		 */
		template< typename CharT >
		static void logDebug( CharT const * msg )
		{
			doLogMessage( LogType::eDebug, toUtf8( msg ), true );
		}
		/**
		 *\~english
		 *\brief		Logs a debug message.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message debug.
		 *\param[in]	msg	La ligne a logger
		 */
		template< typename CharT >
		static void logDebug( std::basic_string_view< CharT > msg )
		{
			doLogMessage( LogType::eDebug, toUtf8( msg ), true );
		}
		/**
		 *\~english
		 *\brief		Logs a debug message.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message debug.
		 *\param[in]	msg	La ligne a logger
		 */
		template< typename CharT >
		static void logDebug( std::basic_string< CharT > const & msg )
		{
			doLogMessage( LogType::eDebug, toUtf8( msg ), true );
		}
		/**
		 *\~english
		 *\brief		Logs a debug message.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message debug.
		 *\param[in]	msg	La ligne a logger
		 */
		template< typename CharT >
		static void logDebug( std::basic_ostream< CharT > const & msg )
		{
			auto sbuf = msg.rdbuf();
			std::basic_stringstream< CharT > ss;
			ss << sbuf;
			logDebug( ss.str() );
		}
		/**
		 *\~english
		 *\brief		Logs a debug message.
		 *\remarks		Doesn't print the new line character.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message debug.
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	msg	La ligne a logger
		 */
		template< typename CharT >
		static void logDebugNoNL( CharT const * msg )
		{
			doLogMessage( LogType::eDebug, toUtf8( msg ), false );
		}
		/**
		 *\~english
		 *\brief		Logs a debug message.
		 *\remarks		Doesn't print the new line character.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message debug.
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	msg	La ligne a logger
		 */
		template< typename CharT >
		static void logDebugNoNL( std::basic_string_view< CharT > msg )
		{
			doLogMessage( LogType::eDebug, toUtf8( msg ), false );
		}
		/**
		 *\~english
		 *\brief		Logs a debug message.
		 *\remarks		Doesn't print the new line character.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message debug.
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	msg	La ligne a logger
		 */
		template< typename CharT >
		static void logDebugNoNL( std::basic_string< CharT > const & msg )
		{
			doLogMessage( LogType::eDebug, toUtf8( msg ), false );
		}
		/**
		 *\~english
		 *\brief		Logs a debug message.
		 *\remarks		Doesn't print the new line character.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message debug.
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	msg	La ligne a logger
		 */
		template< typename CharT >
		static void logDebugNoNL( std::basic_ostream< CharT > const & msg )
		{
			auto sbuf = msg.rdbuf();
			std::basic_stringstream< CharT > ss;
			ss << sbuf;
			logDebugNoNL( ss.str() );
		}
		/**
		 *\~english
		 *\brief		Logs a message.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message.
		 *\param[in]	msg	La ligne a logger
		 */
		template< typename CharT >
		static void logInfo( CharT const * msg )
		{
			doLogMessage( LogType::eInfo, toUtf8( msg ), true );
		}
		/**
		 *\~english
		 *\brief		Logs a message.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message.
		 *\param[in]	msg	La ligne a logger
		 */
		template< typename CharT >
		static void logInfo( std::basic_string_view< CharT > msg )
		{
			doLogMessage( LogType::eInfo, toUtf8( msg ), true );
		}
		/**
		 *\~english
		 *\brief		Logs a message.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message.
		 *\param[in]	msg	La ligne a logger
		 */
		template< typename CharT >
		static void logInfo( std::basic_string< CharT > const & msg )
		{
			doLogMessage( LogType::eInfo, toUtf8( msg ), true );
		}
		/**
		 *\~english
		 *\brief		Logs a message.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message.
		 *\param[in]	msg	The line to log
		 */
		template< typename CharT >
		static void logInfo( std::basic_ostream< CharT > const & msg )
		{
			auto sbuf = msg.rdbuf();
			std::basic_stringstream< CharT > ss;
			ss << sbuf;
			logInfo( ss.str() );
		}
		/**
		 *\~english
		 *\brief		Logs a message.
		 *\remarks		Doesn't print the new line character.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message.
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	msg	La ligne a logger
		 */
		template< typename CharT >
		static void logInfoNoNL( CharT const * msg )
		{
			doLogMessage( LogType::eInfo, toUtf8( msg ), false );
		}
		/**
		 *\~english
		 *\brief		Logs a message.
		 *\remarks		Doesn't print the new line character.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message.
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	msg	La ligne a logger
		 */
		template< typename CharT >
		static void logInfoNoNL( std::basic_string_view< CharT > msg )
		{
			doLogMessage( LogType::eInfo, toUtf8( msg ), false );
		}
		/**
		 *\~english
		 *\brief		Logs a message.
		 *\remarks		Doesn't print the new line character.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message.
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	msg	La ligne a logger
		 */
		template< typename CharT >
		static void logInfoNoNL( std::basic_string< CharT > const & msg )
		{
			doLogMessage( LogType::eInfo, toUtf8( msg ), false );
		}
		/**
		 *\~english
		 *\brief		Logs a message.
		 *\remarks		Doesn't print the new line character.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message.
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	msg	The line to log
		 */
		template< typename CharT >
		static void logInfoNoNL( std::basic_ostream< CharT > const & msg )
		{
			auto sbuf = msg.rdbuf();
			std::basic_stringstream< CharT > ss;
			ss << sbuf;
			logInfoNoNL( ss.str() );
		}
		/**
		 *\~english
		 *\brief		Logs a warning.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un avertissement.
		 *\param[in]	msg	The line to log
		 */
		template< typename CharT >
		static void logWarning( CharT const * msg )
		{
			doLogMessage( LogType::eWarning, toUtf8( msg ), true );
		}
		/**
		 *\~english
		 *\brief		Logs a warning.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un avertissement.
		 *\param[in]	msg	The line to log
		 */
		template< typename CharT >
		static void logWarning( std::basic_string_view< CharT > msg )
		{
			doLogMessage( LogType::eWarning, toUtf8( msg ), true );
		}
		/**
		 *\~english
		 *\brief		Logs a warning.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un avertissement.
		 *\param[in]	msg	The line to log
		 */
		template< typename CharT >
		static void logWarning( std::basic_string< CharT > const & msg )
		{
			doLogMessage( LogType::eWarning, toUtf8( msg ), true );
		}
		/**
		 *\~english
		 *\brief		Logs a warning.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un avertissement.
		 *\param[in]	msg	The line to log
		 */
		template< typename CharT >
		static void logWarning( std::basic_ostream< CharT > const & msg )
		{
			auto sbuf = msg.rdbuf();
			std::basic_stringstream< CharT > ss;
			ss << sbuf;
			logWarning( ss.str() );
		}
		/**
		 *\~english
		 *\brief		Logs a warning.
		 *\remarks		Doesn't print the new line character.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un avertissement.
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	msg	The line to log
		 */
		template< typename CharT >
		static void logWarningNoNL( CharT const * msg )
		{
			doLogMessage( LogType::eWarning, toUtf8( msg ), false );
		}
		/**
		 *\~english
		 *\brief		Logs a warning.
		 *\remarks		Doesn't print the new line character.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un avertissement.
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	msg	The line to log
		 */
		template< typename CharT >
		static void logWarningNoNL( std::basic_string_view< CharT > msg )
		{
			doLogMessage( LogType::eWarning, toUtf8( msg ), false );
		}
		/**
		 *\~english
		 *\brief		Logs a warning.
		 *\remarks		Doesn't print the new line character.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un avertissement.
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	msg	The line to log
		 */
		template< typename CharT >
		static void logWarningNoNL( std::basic_string< CharT > const & msg )
		{
			doLogMessage( LogType::eWarning, toUtf8( msg ), false );
		}
		/**
		 *\~english
		 *\brief		Logs a warning.
		 *\remarks		Doesn't print the new line character.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un avertissement.
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	msg	The line to log
		 */
		template< typename CharT >
		static void logWarningNoNL( std::basic_ostream< CharT > const & msg )
		{
			auto sbuf = msg.rdbuf();
			std::basic_stringstream< CharT > ss;
			ss << sbuf;
			logWarningNoNL( ss.str() );
		}
		/**
		 *\~english
		 *\brief		Logs an error.
		 *\param[in]	msg		The line to log
		 *\~french
		 *\brief		Log une erreur.
		 *\param[in]	msg		The line to log
		 */
		template< typename CharT >
		static void logError( CharT const * msg )
		{
			doLogMessage( LogType::eError, toUtf8( msg ), true );
		}
		/**
		 *\~english
		 *\brief		Logs an error.
		 *\param[in]	msg		The line to log
		 *\~french
		 *\brief		Log une erreur.
		 *\param[in]	msg		The line to log
		 */
		template< typename CharT >
		static void logError( std::basic_string_view< CharT > msg )
		{
			doLogMessage( LogType::eError, toUtf8( msg ), true );
		}
		/**
		 *\~english
		 *\brief		Logs an error.
		 *\param[in]	msg		The line to log
		 *\~french
		 *\brief		Log une erreur.
		 *\param[in]	msg		The line to log
		 */
		template< typename CharT >
		static void logError( std::basic_string< CharT > const & msg )
		{
			doLogMessage( LogType::eError, toUtf8( msg ), true );
		}
		/**
		 *\~english
		 *\brief		Logs an error.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log une erreur.
		 *\param[in]	msg	The line to log
		 */
		template< typename CharT >
		static void logError( std::basic_ostream< CharT > const & msg )
		{
			auto sbuf = msg.rdbuf();
			std::basic_stringstream< CharT > ss;
			ss << sbuf;
			logError( ss.str() );
		}
		/**
		 *\~english
		 *\brief		Logs an error.
		 *\remarks		Doesn't print the new line character.
		 *\param[in]	msg		The line to log
		 *\~french
		 *\brief		Log une erreur.
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	msg		The line to log
		 */
		template< typename CharT >
		static void logErrorNoNL( CharT const * msg )
		{
			doLogMessage( LogType::eError, toUtf8( msg ), false );
		}
		/**
		 *\~english
		 *\brief		Logs an error.
		 *\remarks		Doesn't print the new line character.
		 *\param[in]	msg		The line to log
		 *\~french
		 *\brief		Log une erreur.
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	msg		The line to log
		 */
		template< typename CharT >
		static void logErrorNoNL( std::basic_string_view< CharT > msg )
		{
			doLogMessage( LogType::eError, toUtf8( msg ), false );
		}
		/**
		 *\~english
		 *\brief		Logs an error.
		 *\remarks		Doesn't print the new line character.
		 *\param[in]	msg		The line to log
		 *\~french
		 *\brief		Log une erreur.
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	msg		The line to log
		 */
		template< typename CharT >
		static void logErrorNoNL( std::basic_string< CharT > const & msg )
		{
			doLogMessage( LogType::eError, toUtf8( msg ), false );
		}
		/**
		 *\~english
		 *\brief		Logs an error.
		 *\remarks		Doesn't print the new line character.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log une erreur.
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	msg	The line to log
		 */
		template< typename CharT >
		static void logErrorNoNL( std::basic_ostream< CharT > const & msg )
		{
			auto sbuf = msg.rdbuf();
			std::basic_stringstream< CharT > ss;
			ss << sbuf;
			logErrorNoNL( ss.str() );
		}
		/**
		 *\~english
		 *\brief		Returns a reference over the instance
		 *\return		The instance
		 *\~french
		 *\brief		Retourne une référence sur l'instance
		 *\return		L'instance
		 */
		CU_API static Logger & getSingleton();
		/**
		 *\~english
		 *\brief		Returns a pointer over the instance
		 *\return		The instance
		 *\~french
		 *\brief		Retourne un pointeur sur l'instance
		 *\return		L'instance
		 */
		CU_API static Logger * getSingletonPtr();
		/**
		 *\~english
		 *\brief		Returns a pointer over the instance
		 *\return		The instance
		 *\~french
		 *\brief		Retourne un pointeur sur l'instance
		 *\return		L'instance
		 */
		inline LoggerInstance * getInstance()const
		{
			return m_instance.get();
		}

	private:
		static void CU_API doLogMessage( LogType type, MbString const & msg, bool addNL );

	private:
		static castor::RawUniquePtr< Logger > m_singleton;

		castor::RawUniquePtr< ProgramConsole > m_console;
		castor::RawUniquePtr< LoggerInstance > m_instance;
		castor::RawUniquePtr< std::streambuf > m_cout{};
		castor::RawUniquePtr< std::streambuf > m_cerr{};
		castor::RawUniquePtr< std::streambuf > m_clog{};
		castor::RawUniquePtr< std::wstreambuf > m_wcout{};
		castor::RawUniquePtr< std::wstreambuf > m_wcerr{};
		castor::RawUniquePtr< std::wstreambuf > m_wclog{};
	};
}

#endif
