/*
See LICENSE file in root folder
*/
#ifndef ___CU_LoggerInstance_H___
#define ___CU_LoggerInstance_H___

#include "CastorUtils/Log/LoggerImpl.hpp"

#include "CastorUtils/Data/DataModule.hpp"

#pragma warning( push )
#pragma warning( disable:4365 )
#include <atomic>
#include <mutex>
#include <thread>
#pragma warning( pop )

namespace castor
{
	class LoggerInstance
	{
	public:
		using my_string = std::string;
		using my_ostream = std::ostream;

		LoggerInstance( LoggerInstance const & ) = delete;
		LoggerInstance & operator=( LoggerInstance const & ) = delete;
		CU_API LoggerInstance( LoggerInstance && rhs );
		CU_API LoggerInstance & operator=( LoggerInstance && rhs );

		CU_API ~LoggerInstance();
		/**
		 *\~english
		 *\brief		sets the log file address.
		 *\param[in]	console	The console where messages will output.
		 *\param[in]	logType	The log type.
		 *\~french
		 *\brief		Définit le chemin du fichier de log.
		 *\param[in]	console	La console vers laquelle les messages sortiront.
		 *\param[in]	logType	Le type de log concerné.
		 */
		CU_API LoggerInstance( ProgramConsole & console
			, LogType logType );
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
		 *\return		The current log level.
		 *\~french
		 *\return		Le niveau de log actuel.
		 */
		CU_API LogType getLevel();
		/**
		 *\~english
		 *\brief		Logs a trace message, from a std::string
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message trace, à partir d'un std::string
		 *\param[in]	msg	La ligne a logger
		 */
		CU_API void lockedLogTrace( my_string const & msg );
		/**
		 *\~english
		 *\brief		Logs a debug message, from a std::string
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message debug, à partir d'un std::string
		 *\param[in]	msg	La ligne a logger
		 */
		CU_API void lockedLogDebug( my_string const & msg );
		/**
		 *\~english
		 *\brief		Logs a message, from a std::string
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message, à partir d'un std::string
		 *\param[in]	msg	La ligne a logger
		 */
		CU_API void lockedLogInfo( my_string const & msg );
		/**
		 *\~english
		 *\brief		Logs a warning, from a std::string
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un avertissement, à partir d'un std::string
		 *\param[in]	msg	The line to log
		 */
		CU_API void lockedLogWarning( my_string const & msg );
		/**
		 *\~english
		 *\brief		Logs an error, from a std::string
		 *\param[in]	msg		The line to log
		 *\~french
		 *\brief		Log une erreur, à partir d'un std::string
		 *\param[in]	msg		The line to log
		 */
		CU_API void lockedLogError( my_string const & msg );
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
		CU_API void lockedLogTraceNoLF( my_string const & msg );
		/**
		 *\~english
		 *\brief		Logs a debug message, from a std::string
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message debug, à partir d'un std::string
		 *\param[in]	msg	La ligne a logger
		 */
		CU_API void lockedLogDebugNoLF( my_string const & msg );
		/**
		 *\~english
		 *\brief		Logs a message, from a std::string
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message, à partir d'un std::string
		 *\param[in]	msg	La ligne a logger
		 */
		CU_API void lockedLogInfoNoLF( my_string const & msg );
		/**
		 *\~english
		 *\brief		Logs a warning, from a std::string
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un avertissement, à partir d'un std::string
		 *\param[in]	msg	The line to log
		 */
		CU_API void lockedLogWarningNoLF( my_string const & msg );
		/**
		 *\~english
		 *\brief		Logs an error, from a std::string
		 *\param[in]	msg		The line to log
		 *\~french
		 *\brief		Log une erreur, à partir d'un std::string
		 *\param[in]	msg		The line to log
		 */
		CU_API void lockedLogErrorNoLF( my_string const & msg );
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

		String getHeader( uint8_t index )const
		{
			return m_headers[index];
		}

		void lock()const
		{
			m_mutexQueue.lock();
		}

		void unlock()const
		{
			m_mutexQueue.unlock();
		}

	private:
		void doInitialiseThread();
		void doCleanupThread();
		void doLockedPushMessage( LogType type
			, std::string const & message
			, bool addLF = true );

	private:
		LogType m_logLevel;
		LoggerImpl m_impl;
		std::array< String, size_t( LogType::eCount ) > m_headers;
		MessageQueue m_queue;
		mutable std::mutex m_mutexQueue;
		std::thread m_logThread;
		std::atomic_bool m_initialised{ false };
		std::atomic_bool m_stopped{ false };
		std::atomic_bool m_threadEnded{ false };
	};
}

#endif
