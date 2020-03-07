/*
See LICENSE file in root folder
*/
#ifndef ___CU_LoggerInstance_H___
#define ___CU_LoggerInstance_H___

#include "CastorUtils/Log/LoggerImpl.hpp"

#include <mutex>
#include <atomic>
#include <thread>

namespace castor
{
	/**
	\version	0.11.0
	\~english
	\brief		Logger instance class.
	\remarks	Implements log facilities. Create a Log with a filename, then write logs into that file.
	\~french
	\brief		Instance de logger.
	\remarks	Implémente les fonctions de logging. Initialise un log avec un nom de fichier puis écrit dedans
	*/
	class LoggerInstance
	{
	public:
		using my_string = std::string;
		using my_ostream = std::ostream;
		CU_API ~LoggerInstance();
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
		CU_API LoggerInstance( LogType logType );
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
		CU_API void registerCallback( LogCallback callback, void * caller );
		/**
		 *\~english
		 *\brief		Unregisters the logging callback
		 *\param[in]	caller	Pointer to user data
		 *\~french
		 *\brief		Désenregistre un callback de log
		 *\param[in]	caller	Pointeur sur des données utilisateur
		 */
		CU_API void unregisterCallback( void * caller );
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
		CU_API void setFileName( Path const & logFilePath
			, LogType logType = LogType::eCount );
		/**
		 *\~english
		 *\brief		Logs a trace message, from a std::string
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message trace, à partir d'un std::string
		 *\param[in]	msg	La ligne a logger
		 */
		CU_API void logTrace( my_string const & msg );
		/**
		 *\~english
		 *\brief		Logs a trace message, from a std::ostream
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message trace, à partir d'un std::ostream
		 *\param[in]	msg	La ligne a logger
		 */
		CU_API void logTrace( my_ostream const & msg );
		/**
		 *\~english
		 *\brief		Logs a debug message, from a std::string
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message debug, à partir d'un std::string
		 *\param[in]	msg	La ligne a logger
		 */
		CU_API void logDebug( my_string const & msg );
		/**
		 *\~english
		 *\brief		Logs a debug message, from a std::ostream
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message debug, à partir d'un std::ostream
		 *\param[in]	msg	La ligne a logger
		 */
		CU_API void logDebug( my_ostream const & msg );
		/**
		 *\~english
		 *\brief		Logs a message, from a std::string
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message, à partir d'un std::string
		 *\param[in]	msg	La ligne a logger
		 */
		CU_API void logInfo( my_string const & msg );
		/**
		 *\~english
		 *\brief		Logs a message, from a std::ostream
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message, à partir d'un std::ostream
		 *\param[in]	msg	The line to log
		 */
		CU_API void logInfo( my_ostream const & msg );
		/**
		 *\~english
		 *\brief		Logs a warning, from a std::string
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un avertissement, à partir d'un std::string
		 *\param[in]	msg	The line to log
		 */
		CU_API void logWarning( my_string const & msg );
		/**
		 *\~english
		 *\brief		Logs a warning, from a std::ostream
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un avertissement, à partir d'un std::ostream
		 *\param[in]	msg	The line to log
		 */
		CU_API void logWarning( my_ostream const & msg );
		/**
		 *\~english
		 *\brief		Logs an error, from a std::string
		 *\param[in]	msg		The line to log
		 *\~french
		 *\brief		Log une erreur, à partir d'un std::string
		 *\param[in]	msg		The line to log
		 */
		CU_API void logError( my_string const & msg );
		/**
		 *\~english
		 *\brief		Logs an error, from a std::ostream
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log une erreur, à partir d'un std::ostream
		 *\param[in]	msg	The line to log
		 */
		CU_API void logError( my_ostream const & msg );
		/**
		 *\~english
		 *\brief		Logs a trace message, from a std::string
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message trace, à partir d'un std::string
		 *\param[in]	msg	La ligne a logger
		 */
		CU_API void logTraceNoLF( my_string const & msg );
		/**
		 *\~english
		 *\brief		Logs a trace message, from a std::ostream
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message trace, à partir d'un std::ostream
		 *\param[in]	msg	La ligne a logger
		 */
		CU_API void logTraceNoLF( my_ostream const & msg );
		/**
		 *\~english
		 *\brief		Logs a debug message, from a std::string
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message debug, à partir d'un std::string
		 *\param[in]	msg	La ligne a logger
		 */
		CU_API void logDebugNoLF( my_string const & msg );
		/**
		 *\~english
		 *\brief		Logs a debug message, from a std::ostream
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message debug, à partir d'un std::ostream
		 *\param[in]	msg	La ligne a logger
		 */
		CU_API void logDebugNoLF( my_ostream const & msg );
		/**
		 *\~english
		 *\brief		Logs a message, from a std::string
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message, à partir d'un std::string
		 *\param[in]	msg	La ligne a logger
		 */
		CU_API void logInfoNoLF( my_string const & msg );
		/**
		 *\~english
		 *\brief		Logs a message, from a std::ostream
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message, à partir d'un std::ostream
		 *\param[in]	msg	The line to log
		 */
		CU_API void logInfoNoLF( my_ostream const & msg );
		/**
		 *\~english
		 *\brief		Logs a warning, from a std::string
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un avertissement, à partir d'un std::string
		 *\param[in]	msg	The line to log
		 */
		CU_API void logWarningNoLF( my_string const & msg );
		/**
		 *\~english
		 *\brief		Logs a warning, from a std::ostream
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un avertissement, à partir d'un std::ostream
		 *\param[in]	msg	The line to log
		 */
		CU_API void logWarningNoLF( my_ostream const & msg );
		/**
		 *\~english
		 *\brief		Logs an error, from a std::string
		 *\param[in]	msg		The line to log
		 *\~french
		 *\brief		Log une erreur, à partir d'un std::string
		 *\param[in]	msg		The line to log
		 */
		CU_API void logErrorNoLF( my_string const & msg );
		/**
		 *\~english
		 *\brief		Logs an error, from a std::ostream
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log une erreur, à partir d'un std::ostream
		 *\param[in]	msg	The line to log
		 */
		CU_API void logErrorNoLF( my_ostream const & msg );
		/**
		 *\~english
		 *\brief		Pushes a message into the queue.
		 *\param[in]	type	The message type.
		 *\param[in]	message	The message.
		 *\param[in]	addLF	Whether or not add a LF at the end.
		 *\~french
		 *\brief		Met un message dans la file.
		 *\param[in]	type	Le type de message.
		 *\param[in]	message	Le message.
		 *\param[in]	addLF	Dit si on ajoute un LF à la fin..
		 */
		CU_API void pushMessage( LogType type
			, std::string const & message
			, bool addLF = true );

		CU_API void flushQueue();

		inline String getHeader( uint8_t index )const
		{
			return m_headers[index];
		}

	private:
		CU_API void doInitialiseThread();
		CU_API void doCleanupThread();

	private:
		//! the current logging level, all logs lower than this level are ignored
		LogType m_logLevel;
		//! The logger implementation.
		LoggerImpl m_impl;
		//! The header for each lg line of given log level
		std::array< String, size_t( LogType::eCount ) > m_headers;
		//! The message queue
		MessageQueue m_queue;
		//! The mutex protecting the message queue
		std::mutex m_mutexQueue;
		//! The logging thread
		std::thread m_logThread;
		//! Tells if the logger is initialised
		std::atomic_bool m_initialised{ false };
		//! Tells if the thread must be stopped
		std::atomic_bool m_stopped{ false };
		//! Event raised when the thread is ended
		std::atomic_bool m_threadEnded{ false };
	};
}

#endif
