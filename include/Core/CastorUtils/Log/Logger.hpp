/*
See LICENSE file in root folder
*/
#ifndef ___CU_Logger_H___
#define ___CU_Logger_H___

#include "CastorUtils/Log/LoggerInstance.hpp"

namespace castor
{
	/**
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Log management class
	\remarks	Implements log facilities. Create a Log with a filename, then write logs into that file
	\~french
	\brief		Classe de gestion de logs
	\remarks	Implémente les fonctions de logging. Initialise un log avec un nom de fichier puis écrit dedans
	*/
	class Logger
	{
	private:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		CU_API Logger( LogType level );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		CU_API ~Logger();

	public:
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
		CU_API static LoggerInstance * createInstance( LogType logLevel );
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
		CU_API static void registerCallback( LogCallback callback, void * caller );
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
		 *\brief		Logs a trace message, from a std::string
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message trace, à partir d'un std::string
		 *\param[in]	msg	La ligne a logger
		 */
		CU_API static void logTrace( std::string const & msg );
		/**
		 *\~english
		 *\brief		Logs a trace message, from a std::ostream
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message trace, à partir d'un std::ostream
		 *\param[in]	msg	La ligne a logger
		 */
		CU_API static void logTrace( std::ostream const & msg );
		/**
		 *\~english
		 *\brief		Logs a unicode trace message, from a std::wstring
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message trace, à partir d'un std::wstring
		 *\param[in]	msg	La ligne a logger
		 */
		CU_API static void logTrace( std::wstring const & msg );
		/**
		 *\~english
		 *\brief		Logs a trace message, from a std::wostream
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message trace, à partir d'un std::wostream
		 *\param[in]	msg	The line to log
		 */
		CU_API static void logTrace( std::wostream const & msg );
		/**
		 *\~english
		 *\brief		Logs a debug message, from a std::string
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message debug, à partir d'un std::string
		 *\param[in]	msg	La ligne a logger
		 */
		CU_API static void logDebug( std::string const & msg );
		/**
		 *\~english
		 *\brief		Logs a debug message, from a std::ostream
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message debug, à partir d'un std::ostream
		 *\param[in]	msg	La ligne a logger
		 */
		CU_API static void logDebug( std::ostream const & msg );
		/**
		 *\~english
		 *\brief		Logs a unicode debug message, from a std::wstring
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message debug, à partir d'un std::wstring
		 *\param[in]	msg	La ligne a logger
		 */
		CU_API static void logDebug( std::wstring const & msg );
		/**
		 *\~english
		 *\brief		Logs a debug message, from a std::wostream
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message debug, à partir d'un std::wostream
		 *\param[in]	msg	The line to log
		 */
		CU_API static void logDebug( std::wostream const & msg );
		/**
		 *\~english
		 *\brief		Logs a message, from a std::string
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message, à partir d'un std::string
		 *\param[in]	msg	La ligne a logger
		 */
		CU_API static void logInfo( std::string const & msg );
		/**
		 *\~english
		 *\brief		Logs a message, from a std::ostream
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message, à partir d'un std::ostream
		 *\param[in]	msg	The line to log
		 */
		CU_API static void logInfo( std::ostream const & msg );
		/**
		 *\~english
		 *\brief		Logs a message, from a std::wstring
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message, à partir d'un std::wstring
		 *\param[in]	msg	The line to log
		 */
		CU_API static void logInfo( std::wstring const & msg );
		/**
		 *\~english
		 *\brief		Logs a message, from a std::wostream
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message, à partir d'un std::wostream
		 *\param[in]	msg	The line to log
		 */
		CU_API static void logInfo( std::wostream const & msg );
		/**
		 *\~english
		 *\brief		Logs a warning, from a std::string
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un avertissement, à partir d'un std::string
		 *\param[in]	msg	The line to log
		 */
		CU_API static void logWarning( std::string const & msg );
		/**
		 *\~english
		 *\brief		Logs a warning, from a std::ostream
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un avertissement, à partir d'un std::ostream
		 *\param[in]	msg	The line to log
		 */
		CU_API static void logWarning( std::ostream const & msg );
		/**
		 *\~english
		 *\brief		Logs a warning, from a std::wstring
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un avertissement, à partir d'un std::wstring
		 *\param[in]	msg	The line to log
		 */
		CU_API static void logWarning( std::wstring const & msg );
		/**
		 *\~english
		 *\brief		Logs a warning, from a std::wostream
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un avertissement, à partir d'un std::wostream
		 *\param[in]	msg	The line to log
		 */
		CU_API static void logWarning( std::wostream const & msg );
		/**
		 *\~english
		 *\brief		Logs an error, from a std::string
		 *\param[in]	msg		The line to log
		 *\~french
		 *\brief		Log une erreur, à partir d'un std::string
		 *\param[in]	msg		The line to log
		 */
		CU_API static void logError( std::string const & msg );
		/**
		 *\~english
		 *\brief		Logs an error, from a std::ostream
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log une erreur, à partir d'un std::ostream
		 *\param[in]	msg	The line to log
		 */
		CU_API static void logError( std::ostream const & msg );
		/**
		 *\~english
		 *\brief		Logs an error, from a std::wstring
		 *\param[in]	msg		The line to log
		 *\~french
		 *\brief		Log une erreur, à partir d'un std::wstring
		 *\param[in]	msg		The line to log
		 */
		CU_API static void logError( std::wstring const & msg );
		/**
		 *\~english
		 *\brief		Logs an error, from a std::wostream
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log une erreur, à partir d'un std::wostream
		 *\param[in]	msg	The line to log
		 */
		CU_API static void logError( std::wostream const & msg );
		/**
		 *\~english
		 *\brief		Logs a trace message, from a std::string
		 *\remarks		Doesn't print the new line character.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message trace, à partir d'un std::string
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	msg	La ligne a logger
		 */
		CU_API static void logTraceNoNL( std::string const & msg );
		/**
		 *\~english
		 *\brief		Logs a trace message, from a std::ostream
		 *\remarks		Doesn't print the new line character.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message trace, à partir d'un std::ostream
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	msg	La ligne a logger
		 */
		CU_API static void logTraceNoNL( std::ostream const & msg );
		/**
		 *\~english
		 *\brief		Logs a unicode trace message, from a std::wstring
		 *\remarks		Doesn't print the new line character.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message trace, à partir d'un std::wstring
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	msg	La ligne a logger
		 */
		CU_API static void logTraceNoNL( std::wstring const & msg );
		/**
		 *\~english
		 *\brief		Logs a trace message, from a std::wostream
		 *\remarks		Doesn't print the new line character.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message trace, à partir d'un std::wostream
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	msg	The line to log
		 */
		CU_API static void logTraceNoNL( std::wostream const & msg );
		/**
		 *\~english
		 *\brief		Logs a debug message, from a std::string
		 *\remarks		Doesn't print the new line character.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message debug, à partir d'un std::string
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	msg	La ligne a logger
		 */
		CU_API static void logDebugNoNL( std::string const & msg );
		/**
		 *\~english
		 *\brief		Logs a debug message, from a std::ostream
		 *\remarks		Doesn't print the new line character.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message debug, à partir d'un std::ostream
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	msg	La ligne a logger
		 */
		CU_API static void logDebugNoNL( std::ostream const & msg );
		/**
		 *\~english
		 *\brief		Logs a unicode debug message, from a std::wstring
		 *\remarks		Doesn't print the new line character.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message debug, à partir d'un std::wstring
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	msg	La ligne a logger
		 */
		CU_API static void logDebugNoNL( std::wstring const & msg );
		/**
		 *\~english
		 *\brief		Logs a debug message, from a std::wostream
		 *\remarks		Doesn't print the new line character.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message debug, à partir d'un std::wostream
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	msg	The line to log
		 */
		CU_API static void logDebugNoNL( std::wostream const & msg );
		/**
		 *\~english
		 *\brief		Logs a message, from a std::string
		 *\remarks		Doesn't print the new line character.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message, à partir d'un std::string
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	msg	La ligne a logger
		 */
		CU_API static void logInfoNoNL( std::string const & msg );
		/**
		 *\~english
		 *\brief		Logs a message, from a std::ostream
		 *\remarks		Doesn't print the new line character.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message, à partir d'un std::ostream
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	msg	The line to log
		 */
		CU_API static void logInfoNoNL( std::ostream const & msg );
		/**
		 *\~english
		 *\brief		Logs a message, from a std::wstring
		 *\remarks		Doesn't print the new line character.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message, à partir d'un std::wstring
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	msg	The line to log
		 */
		CU_API static void logInfoNoNL( std::wstring const & msg );
		/**
		 *\~english
		 *\brief		Logs a message, from a std::wostream
		 *\remarks		Doesn't print the new line character.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message, à partir d'un std::wostream
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	msg	The line to log
		 */
		CU_API static void logInfoNoNL( std::wostream const & msg );
		/**
		 *\~english
		 *\brief		Logs a warning, from a std::string
		 *\remarks		Doesn't print the new line character.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un avertissement, à partir d'un std::string
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	msg	The line to log
		 */
		CU_API static void logWarningNoNL( std::string const & msg );
		/**
		 *\~english
		 *\brief		Logs a warning, from a std::ostream
		 *\remarks		Doesn't print the new line character.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un avertissement, à partir d'un std::ostream
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	msg	The line to log
		 */
		CU_API static void logWarningNoNL( std::ostream const & msg );
		/**
		 *\~english
		 *\brief		Logs a warning, from a std::wstring
		 *\remarks		Doesn't print the new line character.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un avertissement, à partir d'un std::wstring
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	msg	The line to log
		 */
		CU_API static void logWarningNoNL( std::wstring const & msg );
		/**
		 *\~english
		 *\brief		Logs a warning, from a std::wostream
		 *\remarks		Doesn't print the new line character.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un avertissement, à partir d'un std::wostream
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	msg	The line to log
		 */
		CU_API static void logWarningNoNL( std::wostream const & msg );
		/**
		 *\~english
		 *\brief		Logs an error, from a std::string
		 *\remarks		Doesn't print the new line character.
		 *\param[in]	msg		The line to log
		 *\~french
		 *\brief		Log une erreur, à partir d'un std::string
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	msg		The line to log
		 */
		CU_API static void logErrorNoNL( std::string const & msg );
		/**
		 *\~english
		 *\brief		Logs an error, from a std::ostream
		 *\remarks		Doesn't print the new line character.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log une erreur, à partir d'un std::ostream
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	msg	The line to log
		 */
		CU_API static void logErrorNoNL( std::ostream const & msg );
		/**
		 *\~english
		 *\brief		Logs an error, from a std::wstring
		 *\remarks		Doesn't print the new line character.
		 *\param[in]	msg		The line to log
		 *\~french
		 *\brief		Log une erreur, à partir d'un std::wstring
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	msg		The line to log
		 */
		CU_API static void logErrorNoNL( std::wstring const & msg );
		/**
		 *\~english
		 *\brief		Logs an error, from a std::wostream
		 *\remarks		Doesn't print the new line character.
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log une erreur, à partir d'un std::wostream
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	msg	The line to log
		 */
		CU_API static void logErrorNoNL( std::wostream const & msg );
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
		static Logger * m_singleton;

		std::unique_ptr< ProgramConsole > m_console;
		std::unique_ptr< LoggerInstance > m_instance;
		std::streambuf * m_cout{ nullptr };
		std::streambuf * m_cerr{ nullptr };
		std::streambuf * m_clog{ nullptr };
		std::wstreambuf * m_wcout{ nullptr };
		std::wstreambuf * m_wcerr{ nullptr };
		std::wstreambuf * m_wclog{ nullptr };
	};
}

#endif
