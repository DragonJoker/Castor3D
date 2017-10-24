/*
See LICENSE file in root folder
*/
#ifndef ___CU_LOGGER_H___
#define ___CU_LOGGER_H___

#include "CastorUtilsPrerequisites.hpp"

#include <condition_variable>
#include <mutex>
#include <atomic>
#include <thread>

namespace castor
{
	/*!
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
		CU_API Logger();
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
		 *\param[in]	p_eLogLevel		The log level
		 *\~french
		 *\brief		Initialise l'instance du logger avec le niveau donné
		 *\param[in]	p_eLogLevel		Le niveau de log
		 */
		CU_API static void initialise( LogType p_eLogLevel );
		/**
		 *\~english
		 *\brief		Destroys the Logger instance
		 *\~french
		 *\brief		Détruit l'instance du Logger
		 */
		CU_API static void cleanup();
		/**
		 *\~english
		 *\brief		Registers the logging callback
		 *\param[in]	p_pfnCallback	The callback
		 *\param[in]	p_pCaller		Pointer to user data, used to identify the callback
		 *\~french
		 *\brief		Enregistre un callback de log
		 *\param[in]	p_pfnCallback	Le callback
		 *\param[in]	p_pCaller		Pointeur sur des données utilisateur, utilisé pour identifier le callback
		 */
		CU_API static void registerCallback( LogCallback p_pfnCallback, void * p_pCaller );
		/**
		 *\~english
		 *\brief		Unregisters the logging callback
		 *\param[in]	p_pCaller	Pointer to user data
		 *\~french
		 *\brief		Désenregistre un callback de log
		 *\param[in]	p_pCaller	Pointeur sur des données utilisateur
		 */
		CU_API static void unregisterCallback( void * p_pCaller );
		/**
		 *\~english
		 *\brief		sets the log file address
		 *\param[in]	p_logFilePath	The log file path
		 *\param[in]	p_eLogType		The log type
		 *\~french
		 *\brief		Définit le chemin du fichier de log
		 *\param[in]	p_logFilePath	Le chemin du fichier
		 *\param[in]	p_eLogType		Le type de log concerné
		 */
		CU_API static void setFileName( String const & p_logFilePath, LogType p_eLogType = LogType::eCount );
		/**
		 *\~english
		 *\brief		Logs a trace message, from a std::string
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un message trace, à partir d'un std::string
		 *\param[in]	p_msg	La ligne a logger
		 */
		CU_API static void logTrace( std::string const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a trace message, from a std::ostream
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un message trace, à partir d'un std::ostream
		 *\param[in]	p_msg	La ligne a logger
		 */
		CU_API static void logTrace( std::ostream const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a unicode trace message, from a std::wstring
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un message trace, à partir d'un std::wstring
		 *\param[in]	p_msg	La ligne a logger
		 */
		CU_API static void logTrace( std::wstring const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a trace message, from a std::wostream
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un message trace, à partir d'un std::wostream
		 *\param[in]	p_msg	The line to log
		 */
		CU_API static void logTrace( std::wostream const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a debug message, from a std::string
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un message debug, à partir d'un std::string
		 *\param[in]	p_msg	La ligne a logger
		 */
		CU_API static void logDebug( std::string const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a debug message, from a std::ostream
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un message debug, à partir d'un std::ostream
		 *\param[in]	p_msg	La ligne a logger
		 */
		CU_API static void logDebug( std::ostream const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a unicode debug message, from a std::wstring
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un message debug, à partir d'un std::wstring
		 *\param[in]	p_msg	La ligne a logger
		 */
		CU_API static void logDebug( std::wstring const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a debug message, from a std::wostream
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un message debug, à partir d'un std::wostream
		 *\param[in]	p_msg	The line to log
		 */
		CU_API static void logDebug( std::wostream const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a message, from a std::string
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un message, à partir d'un std::string
		 *\param[in]	p_msg	La ligne a logger
		 */
		CU_API static void logInfo( std::string const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a message, from a std::ostream
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un message, à partir d'un std::ostream
		 *\param[in]	p_msg	The line to log
		 */
		CU_API static void logInfo( std::ostream const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a message, from a std::wstring
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un message, à partir d'un std::wstring
		 *\param[in]	p_msg	The line to log
		 */
		CU_API static void logInfo( std::wstring const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a message, from a std::wostream
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un message, à partir d'un std::wostream
		 *\param[in]	p_msg	The line to log
		 */
		CU_API static void logInfo( std::wostream const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a warning, from a std::string
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un avertissement, à partir d'un std::string
		 *\param[in]	p_msg	The line to log
		 */
		CU_API static void logWarning( std::string const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a warning, from a std::ostream
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un avertissement, à partir d'un std::ostream
		 *\param[in]	p_msg	The line to log
		 */
		CU_API static void logWarning( std::ostream const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a warning, from a std::wstring
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un avertissement, à partir d'un std::wstring
		 *\param[in]	p_msg	The line to log
		 */
		CU_API static void logWarning( std::wstring const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a warning, from a std::wostream
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un avertissement, à partir d'un std::wostream
		 *\param[in]	p_msg	The line to log
		 */
		CU_API static void logWarning( std::wostream const & p_msg );
		/**
		 *\~english
		 *\brief		Logs an error, from a std::string
		 *\param[in]	p_msg		The line to log
		 *\~french
		 *\brief		Log une erreur, à partir d'un std::string
		 *\param[in]	p_msg		The line to log
		 */
		CU_API static void logError( std::string const & p_msg );
		/**
		 *\~english
		 *\brief		Logs an error, from a std::ostream
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log une erreur, à partir d'un std::ostream
		 *\param[in]	p_msg	The line to log
		 */
		CU_API static void logError( std::ostream const & p_msg );
		/**
		 *\~english
		 *\brief		Logs an error, from a std::wstring
		 *\param[in]	p_msg		The line to log
		 *\~french
		 *\brief		Log une erreur, à partir d'un std::wstring
		 *\param[in]	p_msg		The line to log
		 */
		CU_API static void logError( std::wstring const & p_msg );
		/**
		 *\~english
		 *\brief		Logs an error, from a std::wostream
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log une erreur, à partir d'un std::wostream
		 *\param[in]	p_msg	The line to log
		 */
		CU_API static void logError( std::wostream const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a trace message, from a std::string
		 *\remarks		doesn't print the new line character.
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un message trace, à partir d'un std::string
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	p_msg	La ligne a logger
		 */
		CU_API static void logTraceNoNL( std::string const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a trace message, from a std::ostream
		 *\remarks		doesn't print the new line character.
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un message trace, à partir d'un std::ostream
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	p_msg	La ligne a logger
		 */
		CU_API static void logTraceNoNL( std::ostream const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a unicode trace message, from a std::wstring
		 *\remarks		doesn't print the new line character.
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un message trace, à partir d'un std::wstring
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	p_msg	La ligne a logger
		 */
		CU_API static void logTraceNoNL( std::wstring const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a trace message, from a std::wostream
		 *\remarks		doesn't print the new line character.
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un message trace, à partir d'un std::wostream
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	p_msg	The line to log
		 */
		CU_API static void logTraceNoNL( std::wostream const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a debug message, from a std::string
		 *\remarks		doesn't print the new line character.
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un message debug, à partir d'un std::string
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	p_msg	La ligne a logger
		 */
		CU_API static void logDebugNoNL( std::string const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a debug message, from a std::ostream
		 *\remarks		doesn't print the new line character.
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un message debug, à partir d'un std::ostream
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	p_msg	La ligne a logger
		 */
		CU_API static void logDebugNoNL( std::ostream const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a unicode debug message, from a std::wstring
		 *\remarks		doesn't print the new line character.
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un message debug, à partir d'un std::wstring
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	p_msg	La ligne a logger
		 */
		CU_API static void logDebugNoNL( std::wstring const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a debug message, from a std::wostream
		 *\remarks		doesn't print the new line character.
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un message debug, à partir d'un std::wostream
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	p_msg	The line to log
		 */
		CU_API static void logDebugNoNL( std::wostream const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a message, from a std::string
		 *\remarks		doesn't print the new line character.
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un message, à partir d'un std::string
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	p_msg	La ligne a logger
		 */
		CU_API static void logInfoNoNL( std::string const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a message, from a std::ostream
		 *\remarks		doesn't print the new line character.
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un message, à partir d'un std::ostream
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	p_msg	The line to log
		 */
		CU_API static void logInfoNoNL( std::ostream const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a message, from a std::wstring
		 *\remarks		doesn't print the new line character.
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un message, à partir d'un std::wstring
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	p_msg	The line to log
		 */
		CU_API static void logInfoNoNL( std::wstring const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a message, from a std::wostream
		 *\remarks		doesn't print the new line character.
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un message, à partir d'un std::wostream
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	p_msg	The line to log
		 */
		CU_API static void logInfoNoNL( std::wostream const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a warning, from a std::string
		 *\remarks		doesn't print the new line character.
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un avertissement, à partir d'un std::string
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	p_msg	The line to log
		 */
		CU_API static void logWarningNoNL( std::string const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a warning, from a std::ostream
		 *\remarks		doesn't print the new line character.
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un avertissement, à partir d'un std::ostream
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	p_msg	The line to log
		 */
		CU_API static void logWarningNoNL( std::ostream const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a warning, from a std::wstring
		 *\remarks		doesn't print the new line character.
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un avertissement, à partir d'un std::wstring
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	p_msg	The line to log
		 */
		CU_API static void logWarningNoNL( std::wstring const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a warning, from a std::wostream
		 *\remarks		doesn't print the new line character.
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un avertissement, à partir d'un std::wostream
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	p_msg	The line to log
		 */
		CU_API static void logWarningNoNL( std::wostream const & p_msg );
		/**
		 *\~english
		 *\brief		Logs an error, from a std::string
		 *\remarks		doesn't print the new line character.
		 *\param[in]	p_msg		The line to log
		 *\~french
		 *\brief		Log une erreur, à partir d'un std::string
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	p_msg		The line to log
		 */
		CU_API static void logErrorNoNL( std::string const & p_msg );
		/**
		 *\~english
		 *\brief		Logs an error, from a std::ostream
		 *\remarks		doesn't print the new line character.
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log une erreur, à partir d'un std::ostream
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	p_msg	The line to log
		 */
		CU_API static void logErrorNoNL( std::ostream const & p_msg );
		/**
		 *\~english
		 *\brief		Logs an error, from a std::wstring
		 *\remarks		doesn't print the new line character.
		 *\param[in]	p_msg		The line to log
		 *\~french
		 *\brief		Log une erreur, à partir d'un std::wstring
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	p_msg		The line to log
		 */
		CU_API static void logErrorNoNL( std::wstring const & p_msg );
		/**
		 *\~english
		 *\brief		Logs an error, from a std::wostream
		 *\remarks		doesn't print the new line character.
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log une erreur, à partir d'un std::wostream
		 *\remarks		N'affiche pas le caractère de nouvelle ligne.
		 *\param[in]	p_msg	The line to log
		 */
		CU_API static void logErrorNoNL( std::wostream const & p_msg );
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

	private:
		void doRegisterCallback( LogCallback p_pfnCallback, void * p_pCaller );
		void doUnregisterCallback( void * p_pCaller );
		void doSetFileName( String const & p_logFilePath, LogType p_eLogType = LogType::eCount );
		void doPushMessage( LogType type, std::string const & message, bool p_newLine = true );
		void doPushMessage( LogType type, std::wstring const & message, bool p_newLine = true );
		void doInitialiseThread();
		void doCleanupThread();
		void doFlushQueue();

	private:
		friend class LoggerImpl;

		//! The logger
		static Logger * m_singleton;
		//! The streambuf used to log info messages
		std::streambuf * m_cout;
		//! The streambuf used to log error messages
		std::streambuf * m_cerr;
		//! The streambuf used to log debug messages
		std::streambuf * m_clog;
		//! The wstreambuf used to log info messages
		std::wstreambuf * m_wcout;
		//! The wstreambuf used to log error messages
		std::wstreambuf * m_wcerr;
		//! The wstreambuf used to log info messages
		std::wstreambuf * m_wclog;
		//! The logger implementation
		LoggerImpl * m_impl;
		//! the mutex used to protect the implementation
		std::recursive_mutex m_mutex;
		//! the current logging level, all logs lower than this level are ignored
		LogType m_logLevel;
		//! The header for each lg line of given log level
		std::array< String, size_t( LogType::eCount ) > m_headers;
		//! The message queue
		MessageQueue m_queue;
		//! The mutex protecting the message queue
		std::mutex m_mutexQueue;
		//! The logging thread
		std::thread m_logThread;
		//! Tells if the logger is initialised
		std::atomic_bool m_initialised;
		//! Tells if the thread must be stopped
		std::atomic_bool m_stopped;
		//! Event raised when the thread is ended
		std::condition_variable m_threadEnded;
		//! Mutex used to wait for the thread end
		std::mutex m_mutexThreadEnded;
	};
}

#endif
