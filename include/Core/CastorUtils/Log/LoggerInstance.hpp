/*
See LICENSE file in root folder
*/
#ifndef ___CU_LoggerInstance_H___
#define ___CU_LoggerInstance_H___

#include "CastorUtils/Log/LoggerImpl.hpp"

#include "CastorUtils/Data/DataModule.hpp"

#include "CastorUtils/Config/BeginExternHeaderGuard.hpp"
#include <atomic>
#include <mutex>
#include <thread>
#include "CastorUtils/Config/EndExternHeaderGuard.hpp"

namespace castor
{
	class LoggerInstance
	{
	public:
		LoggerInstance( LoggerInstance const & ) = delete;
		LoggerInstance & operator=( LoggerInstance const & ) = delete;
		CU_API LoggerInstance( LoggerInstance && rhs )noexcept;
		CU_API LoggerInstance & operator=( LoggerInstance && rhs )noexcept;

		CU_API ~LoggerInstance()noexcept;
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
		CU_API void registerCallback( LogCallback const & callback, void * caller );
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
		CU_API LogType getLevel()const;
		/**
		 *\~english
		 *\brief		Logs a trace message, from a std::basic_string< CharT >
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message trace, à partir d'un std::basic_string< CharT >
		 *\param[in]	msg	La ligne a logger
		 */
		template< typename CharT >
		void lockedLogTrace( std::basic_string< CharT > const & msg )
		{
			doLockedPushMessage( LogType::eTrace, toUtf8( msg ), true );
		}
		/**
		 *\~english
		 *\brief		Logs a trace message, from a std::basic_string< CharT >
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message trace, à partir d'un std::basic_string< CharT >
		 *\param[in]	msg	La ligne a logger
		 */
		template< typename CharT >
		void lockedLogTraceNoLF( std::basic_string< CharT > const & msg )
		{
			doLockedPushMessage( LogType::eTrace, toUtf8( msg ), false );
		}
		/**
		 *\~english
		 *\brief		Logs a trace message, from a std::basic_string< CharT >
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message trace, à partir d'un std::basic_string< CharT >
		 *\param[in]	msg	La ligne a logger
		 */
		template< typename CharT >
		void logTrace( std::basic_string< CharT > const & msg )
		{
			pushMessage( LogType::eTrace, msg, true );
		}
		/**
		 *\~english
		 *\brief		Logs a trace message, from a std::basic_ostream< CharT >
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message trace, à partir d'un std::basic_ostream< CharT >
		 *\param[in]	msg	La ligne a logger
		 */
		template< typename CharT >
		void logTrace( std::basic_ostream< CharT > const & msg )
		{
			auto sbuf = msg.rdbuf();
			std::basic_stringstream< CharT > ss;
			ss << sbuf;
			logTrace( ss.str() );
		}
		/**
		 *\~english
		 *\brief		Logs a trace message, from a std::basic_string< CharT >
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message trace, à partir d'un std::basic_string< CharT >
		 *\param[in]	msg	La ligne a logger
		 */
		template< typename CharT >
		void logTraceNoLF( std::basic_string< CharT > const & msg )
		{
			pushMessage( LogType::eTrace, msg, false );
		}
		/**
		 *\~english
		 *\brief		Logs a trace message, from a std::basic_ostream< CharT >
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message trace, à partir d'un std::basic_ostream< CharT >
		 *\param[in]	msg	La ligne a logger
		 */
		template< typename CharT >
		void logTraceNoLF( std::basic_ostream< CharT > const & msg )
		{
			auto sbuf = msg.rdbuf();
			std::basic_stringstream< CharT > ss;
			ss << sbuf;
			logTraceNoLF( ss.str() );
		}
		/**
		 *\~english
		 *\brief		Logs a debug message, from a std::basic_string< CharT >
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message debug, à partir d'un std::basic_string< CharT >
		 *\param[in]	msg	La ligne a logger
		 */
		template< typename CharT >
		void lockedLogDebug( std::basic_string< CharT > const & msg )
		{
			doLockedPushMessage( LogType::eDebug, toUtf8( msg ), true );
		}
		/**
		 *\~english
		 *\brief		Logs a debug message, from a std::basic_string< CharT >
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message debug, à partir d'un std::basic_string< CharT >
		 *\param[in]	msg	La ligne a logger
		 */
		template< typename CharT >
		void lockedLogDebugNoLF( std::basic_string< CharT > const & msg )
		{
			doLockedPushMessage( LogType::eDebug, toUtf8( msg ), false );
		}
		/**
		 *\~english
		 *\brief		Logs a debug message, from a std::basic_string< CharT >
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message debug, à partir d'un std::basic_string< CharT >
		 *\param[in]	msg	La ligne a logger
		 */
		template< typename CharT >
		void logDebug( std::basic_string< CharT > const & msg )
		{
			pushMessage( LogType::eDebug, msg, true );
		}
		/**
		 *\~english
		 *\brief		Logs a debug message, from a std::basic_ostream< CharT >
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message debug, à partir d'un std::basic_ostream< CharT >
		 *\param[in]	msg	La ligne a logger
		 */
		template< typename CharT >
		void logDebug( std::basic_ostream< CharT > const & msg )
		{
			auto sbuf = msg.rdbuf();
			std::basic_stringstream< CharT > ss;
			ss << sbuf;
			logDebug( ss.str() );
		}
		/**
		 *\~english
		 *\brief		Logs a debug message, from a std::basic_string< CharT >
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message debug, à partir d'un std::basic_string< CharT >
		 *\param[in]	msg	La ligne a logger
		 */
		template< typename CharT >
		void logDebugNoLF( std::basic_string< CharT > const & msg )
		{
			pushMessage( LogType::eDebug, msg, false );
		}
		/**
		 *\~english
		 *\brief		Logs a debug message, from a std::basic_ostream< CharT >
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message debug, à partir d'un std::basic_ostream< CharT >
		 *\param[in]	msg	La ligne a logger
		 */
		template< typename CharT >
		void logDebugNoLF( std::basic_ostream< CharT > const & msg )
		{
			auto sbuf = msg.rdbuf();
			std::basic_stringstream< CharT > ss;
			ss << sbuf;
			logDebugNoLF( ss.str() );
		}
		/**
		 *\~english
		 *\brief		Logs a message, from a std::basic_string< CharT >
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message, à partir d'un std::basic_string< CharT >
		 *\param[in]	msg	La ligne a logger
		 */
		template< typename CharT >
		void lockedLogInfo( std::basic_string< CharT > const & msg )
		{
			doLockedPushMessage( LogType::eInfo, toUtf8( msg ), true );
		}
		/**
		 *\~english
		 *\brief		Logs a message, from a std::basic_string< CharT >
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message, à partir d'un std::basic_string< CharT >
		 *\param[in]	msg	La ligne a logger
		 */
		template< typename CharT >
		void lockedLogInfoNoLF( std::basic_string< CharT > const & msg )
		{
			doLockedPushMessage( LogType::eInfo, toUtf8( msg ), false );
		}
		/**
		 *\~english
		 *\brief		Logs a message, from a std::basic_string< CharT >
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message, à partir d'un std::basic_string< CharT >
		 *\param[in]	msg	La ligne a logger
		 */
		template< typename CharT >
		void logInfo( std::basic_string< CharT > const & msg )
		{
			pushMessage( LogType::eInfo, msg, true );
		}
		/**
		 *\~english
		 *\brief		Logs a message, from a std::basic_ostream< CharT >
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message, à partir d'un std::basic_ostream< CharT >
		 *\param[in]	msg	The line to log
		 */
		template< typename CharT >
		void logInfo( std::basic_ostream< CharT > const & msg )
		{
			auto sbuf = msg.rdbuf();
			std::basic_stringstream< CharT > ss;
			ss << sbuf;
			logInfo( ss.str() );
		}
		/**
		 *\~english
		 *\brief		Logs a message, from a std::basic_string< CharT >
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message, à partir d'un std::basic_string< CharT >
		 *\param[in]	msg	La ligne a logger
		 */
		template< typename CharT >
		void logInfoNoLF( std::basic_string< CharT > const & msg )
		{
			pushMessage( LogType::eInfo, msg, false );
		}
		/**
		 *\~english
		 *\brief		Logs a message, from a std::basic_ostream< CharT >
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un message, à partir d'un std::basic_ostream< CharT >
		 *\param[in]	msg	The line to log
		 */
		template< typename CharT >
		void logInfoNoLF( std::basic_ostream< CharT > const & msg )
		{
			auto sbuf = msg.rdbuf();
			std::basic_stringstream< CharT > ss;
			ss << sbuf;
			logInfoNoLF( ss.str() );
		}
		/**
		 *\~english
		 *\brief		Logs a warning, from a std::basic_string< CharT >
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un avertissement, à partir d'un std::basic_string< CharT >
		 *\param[in]	msg	The line to log
		 */
		template< typename CharT >
		void lockedLogWarning( std::basic_string< CharT > const & msg )
		{
			doLockedPushMessage( LogType::eWarning, toUtf8( msg ), true );
		}
		/**
		 *\~english
		 *\brief		Logs a warning, from a std::basic_string< CharT >
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un avertissement, à partir d'un std::basic_string< CharT >
		 *\param[in]	msg	The line to log
		 */
		template< typename CharT >
		void lockedLogWarningNoLF( std::basic_string< CharT > const & msg )
		{
			doLockedPushMessage( LogType::eWarning, toUtf8( msg ), false );
		}
		/**
		 *\~english
		 *\brief		Logs a warning, from a std::basic_string< CharT >
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un avertissement, à partir d'un std::basic_string< CharT >
		 *\param[in]	msg	The line to log
		 */
		template< typename CharT >
		void logWarning( std::basic_string< CharT > const & msg )
		{
			pushMessage( LogType::eWarning, msg, true );
		}
		/**
		 *\~english
		 *\brief		Logs a warning, from a std::basic_ostream< CharT >
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un avertissement, à partir d'un std::basic_ostream< CharT >
		 *\param[in]	msg	The line to log
		 */
		template< typename CharT >
		void logWarning( std::basic_ostream< CharT > const & msg )
		{
			auto sbuf = msg.rdbuf();
			std::basic_stringstream< CharT > ss;
			ss << sbuf;
			logWarning( ss.str() );
		}
		/**
		 *\~english
		 *\brief		Logs a warning, from a std::basic_string< CharT >
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un avertissement, à partir d'un std::basic_string< CharT >
		 *\param[in]	msg	The line to log
		 */
		template< typename CharT >
		void logWarningNoLF( std::basic_string< CharT > const & msg )
		{
			pushMessage( LogType::eWarning, msg, false );
		}
		/**
		 *\~english
		 *\brief		Logs a warning, from a std::basic_ostream< CharT >
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log un avertissement, à partir d'un std::basic_ostream< CharT >
		 *\param[in]	msg	The line to log
		 */
		template< typename CharT >
		void logWarningNoLF( std::basic_ostream< CharT > const & msg )
		{
			auto sbuf = msg.rdbuf();
			std::basic_stringstream< CharT > ss;
			ss << sbuf;
			logWarningNoLF( ss.str() );
		}
		/**
		 *\~english
		 *\brief		Logs an error, from a std::basic_string< CharT >
		 *\param[in]	msg		The line to log
		 *\~french
		 *\brief		Log une erreur, à partir d'un std::basic_string< CharT >
		 *\param[in]	msg		The line to log
		 */
		template< typename CharT >
		void lockedLogError( std::basic_string< CharT > const & msg )
		{
			doLockedPushMessage( LogType::eError, toUtf8( msg ), true );
		}
		/**
		 *\~english
		 *\brief		Logs an error, from a std::basic_string< CharT >
		 *\param[in]	msg		The line to log
		 *\~french
		 *\brief		Log une erreur, à partir d'un std::basic_string< CharT >
		 *\param[in]	msg		The line to log
		 */
		template< typename CharT >
		void lockedLogErrorNoLF( std::basic_string< CharT > const & msg )
		{
			doLockedPushMessage( LogType::eError, toUtf8( msg ), false );
		}
		/**
		 *\~english
		 *\brief		Logs an error, from a std::basic_string< CharT >
		 *\param[in]	msg		The line to log
		 *\~french
		 *\brief		Log une erreur, à partir d'un std::basic_string< CharT >
		 *\param[in]	msg		The line to log
		 */
		template< typename CharT >
		void logError( std::basic_string< CharT > const & msg )
		{
			pushMessage( LogType::eError, msg, true );
		}
		/**
		 *\~english
		 *\brief		Logs an error, from a std::basic_ostream< CharT >
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log une erreur, à partir d'un std::basic_ostream< CharT >
		 *\param[in]	msg	The line to log
		 */
		template< typename CharT >
		void logError( std::basic_ostream< CharT > const & msg )
		{
			auto sbuf = msg.rdbuf();
			std::basic_stringstream< CharT > ss;
			ss << sbuf;
			logError( ss.str() );
		}
		/**
		 *\~english
		 *\brief		Logs an error, from a std::basic_string< CharT >
		 *\param[in]	msg		The line to log
		 *\~french
		 *\brief		Log une erreur, à partir d'un std::basic_string< CharT >
		 *\param[in]	msg		The line to log
		 */
		template< typename CharT >
		void logErrorNoLF( std::basic_string< CharT > const & msg )
		{
			pushMessage( LogType::eError, msg, false );
		}
		/**
		 *\~english
		 *\brief		Logs an error, from a std::basic_ostream< CharT >
		 *\param[in]	msg	The line to log
		 *\~french
		 *\brief		Log une erreur, à partir d'un std::basic_ostream< CharT >
		 *\param[in]	msg	The line to log
		 */
		template< typename CharT >
		void logErrorNoLF( std::basic_ostream< CharT > const & msg )
		{
			auto sbuf = msg.rdbuf();
			std::basic_stringstream< CharT > ss;
			ss << sbuf;
			logErrorNoLF( ss.str() );
		}
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
		template< typename CharT >
		void pushMessage( LogType type
			, std::basic_string< CharT > const & message
			, bool addLF = true )
		{
			auto lock( makeUniqueLock( m_mutexQueue ) );
			doLockedPushMessage( type, toUtf8( message ), addLF );
		}

		CU_API void flushQueue();

		MbString getHeader( uint8_t index )const noexcept
		{
			return m_headers[index];
		}

		void lock()const
		{
			m_mutexQueue.lock();
		}

		void unlock()const noexcept
		{
			m_mutexQueue.unlock();
		}

	private:
		void doInitialiseThread();
		void doCleanupThread();
		CU_API void doLockedPushMessage( LogType type
			, MbString const & message
			, bool addLF = true );

	private:
		LogType m_logLevel;
		LoggerImpl m_impl;
		Array< MbString, size_t( LogType::eCount ) > m_headers
		{
			"****TRACE**** ",
			"****DEBUG**** ",
			"              ",
			"***WARNING*** ",
			"****ERROR**** ",
		};
		MessageQueue m_queue;
		mutable castor::Mutex m_mutexQueue;
		std::thread m_logThread;
		std::atomic_bool m_initialised{ false };
		std::atomic_bool m_stopped{ false };
		std::atomic_bool m_threadEnded{ false };
	};
}

#endif
