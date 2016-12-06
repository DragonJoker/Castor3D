/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___CU_LOGGER_H___
#define ___CU_LOGGER_H___

#include "CastorUtilsPrerequisites.hpp"

#include <condition_variable>
#include <mutex>
#include <atomic>
#include <thread>

namespace Castor
{
	/**
	 *\~english
	 *\brief		Write formatted data from variable argument list to sized buffer
	 *\param[out]	p_text		Receives the formatted text
	 *\param[in]	p_count		The output buffer size
	 *\param[in]	p_format	The format string used to interprete the arguments list
	 *\param[in]	p_valist	The arguments list
	 *\return		The number of characters that would have been written if p_count had been sufficiently large, not counting the terminating null character
	 *\~french
	 *\brief		Ecrit des données formatées à partir d'une liste variable d'arguments, dans un tampon à taille fixe
	 *\param[out]	p_text		Reçoit le texte formaté
	 *\param[in]	p_count		La taille du tampon
	 *\param[in]	p_format	La chaîne contenant le format permettant l'interprétation de la liste d'arguments
	 *\param[in]	p_valist	La liste d'arguments
	 *\return		Le nombre de caractères qui auraient été écrits si p_count avait été suffisamment grand, sans compter le caractère de terminaison
	*/
	CU_API int Vsnprintf( char * p_text, size_t p_count, const char * p_format, va_list p_valist );
	/**
	 *\~english
	 *\brief		Write formatted data from variable argument list to sized buffer
	 *\param[out]	p_text		Receives the formatted text
	 *\param[in]	p_format	The format string used to interprete the arguments list
	 *\param[in]	p_valist	The arguments list
	 *\return		The number of characters that would have been written if p_count had been sufficiently large, not counting the terminating null character
	 *\~french
	 *\brief		Ecrit des données formatées à partir d'une liste variable d'arguments, dans un tampon à taille fixe
	 *\param[out]	p_text		Reçoit le texte formaté
	 *\param[in]	p_format	La chaîne contenant le format permettant l'interprétation de la liste d'arguments
	 *\param[in]	p_valist	La liste d'arguments
	 *\return		Le nombre de caractères qui auraient été écrits si p_count avait été suffisamment grand, sans compter le caractère de terminaison
	*/
	template< size_t Count >
	int Vsnprintf( char( &p_text )[Count], const char * p_format, va_list p_valist )
	{
		return Vsnprintf( p_text, Count, p_format, p_valist );
	}
	/**
	 *\~english
	 *\brief		Write formatted data from variable argument list to sized buffer
	 *\param[out]	p_text		Receives the formatted text
	 *\param[in]	p_count		The output buffer size
	 *\param[in]	p_format	The format string used to interprete the arguments list
	 *\param[in]	p_valist	The arguments list
	 *\return		The number of characters that would have been written if p_count had been sufficiently large, not counting the terminating null character
	 *\~french
	 *\brief		Ecrit des données formatées à partir d'une liste variable d'arguments, dans un tampon à taille fixe
	 *\param[out]	p_text		Reçoit le texte formaté
	 *\param[in]	p_count		La taille du tampon
	 *\param[in]	p_format	La chaîne contenant le format permettant l'interprétation de la liste d'arguments
	 *\param[in]	p_valist	La liste d'arguments
	 *\return		Le nombre de caractères qui auraient été écrits si p_count avait été suffisamment grand, sans compter le caractère de terminaison
	*/
	CU_API int Vswprintf( wchar_t * p_text, size_t p_count, const wchar_t * p_format, va_list p_valist );
	/**
	 *\~english
	 *\brief		Write formatted data from variable argument list to sized buffer
	 *\param[out]	p_text		Receives the formatted text
	 *\param[in]	p_format	The format string used to interprete the arguments list
	 *\param[in]	p_valist	The arguments list
	 *\return		The number of characters that would have been written if p_count had been sufficiently large, not counting the terminating null character
	 *\~french
	 *\brief		Ecrit des données formatées à partir d'une liste variable d'arguments, dans un tampon à taille fixe
	 *\param[out]	p_text		Reçoit le texte formaté
	 *\param[in]	p_format	La chaîne contenant le format permettant l'interprétation de la liste d'arguments
	 *\param[in]	p_valist	La liste d'arguments
	 *\return		Le nombre de caractères qui auraient été écrits si p_count avait été suffisamment grand, sans compter le caractère de terminaison
	*/
	template< size_t Count >
	int Vswprintf( wchar_t ( &p_text )[Count], const wchar_t * p_format, va_list p_valist )
	{
		return Vswprintf( p_text, Count, p_format, p_valist );
	}
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
		CU_API static void Initialise( LogType p_eLogLevel );
		/**
		 *\~english
		 *\brief		Destroys the Logger instance
		 *\~french
		 *\brief		Détruit l'instance du Logger
		 */
		CU_API static void Cleanup();
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
		CU_API static void RegisterCallback( LogCallback p_pfnCallback, void * p_pCaller );
		/**
		 *\~english
		 *\brief		Unregisters the logging callback
		 *\param[in]	p_pCaller	Pointer to user data
		 *\~french
		 *\brief		Désenregistre un callback de log
		 *\param[in]	p_pCaller	Pointeur sur des données utilisateur
		 */
		CU_API static void UnregisterCallback( void * p_pCaller );
		/**
		 *\~english
		 *\brief		Sets the log file address
		 *\param[in]	p_logFilePath	The log file path
		 *\param[in]	p_eLogType		The log type
		 *\~french
		 *\brief		Définit le chemin du fichier de log
		 *\param[in]	p_logFilePath	Le chemin du fichier
		 *\param[in]	p_eLogType		Le type de log concerné
		 */
		CU_API static void SetFileName( String const & p_logFilePath, LogType p_eLogType = LogType::eCount );
		/**
		 *\~english
		 *\brief		Logs a trace message, from a std::string
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un message trace, à partir d'un std::string
		 *\param[in]	p_msg	La ligne a logger
		 */
		CU_API static void LogTrace( std::string const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a trace message, from a std::ostream
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un message trace, à partir d'un std::ostream
		 *\param[in]	p_msg	La ligne a logger
		 */
		CU_API static void LogTrace( std::ostream const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a unicode trace message, from a std::wstring
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un message trace, à partir d'un std::wstring
		 *\param[in]	p_msg	La ligne a logger
		 */
		CU_API static void LogTrace( std::wstring const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a trace message, from a std::wostream
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un message trace, à partir d'un std::wostream
		 *\param[in]	p_msg	The line to log
		 */
		CU_API static void LogTrace( std::wostream const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a debug message, from a std::string
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un message debug, à partir d'un std::string
		 *\param[in]	p_msg	La ligne a logger
		 */
		CU_API static void LogDebug( std::string const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a debug message, from a std::ostream
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un message debug, à partir d'un std::ostream
		 *\param[in]	p_msg	La ligne a logger
		 */
		CU_API static void LogDebug( std::ostream const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a unicode debug message, from a std::wstring
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un message debug, à partir d'un std::wstring
		 *\param[in]	p_msg	La ligne a logger
		 */
		CU_API static void LogDebug( std::wstring const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a debug message, from a std::wostream
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un message debug, à partir d'un std::wostream
		 *\param[in]	p_msg	The line to log
		 */
		CU_API static void LogDebug( std::wostream const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a message, from a std::string
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un message, à partir d'un std::string
		 *\param[in]	p_msg	La ligne a logger
		 */
		CU_API static void LogInfo( std::string const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a message, from a std::ostream
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un message, à partir d'un std::ostream
		 *\param[in]	p_msg	The line to log
		 */
		CU_API static void LogInfo( std::ostream const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a message, from a std::wstring
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un message, à partir d'un std::wstring
		 *\param[in]	p_msg	The line to log
		 */
		CU_API static void LogInfo( std::wstring const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a message, from a std::wostream
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un message, à partir d'un std::wostream
		 *\param[in]	p_msg	The line to log
		 */
		CU_API static void LogInfo( std::wostream const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a warning, from a std::string
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un avertissement, à partir d'un std::string
		 *\param[in]	p_msg	The line to log
		 */
		CU_API static void LogWarning( std::string const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a warning, from a std::ostream
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un avertissement, à partir d'un std::ostream
		 *\param[in]	p_msg	The line to log
		 */
		CU_API static void LogWarning( std::ostream const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a warning, from a std::wstring
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un avertissement, à partir d'un std::wstring
		 *\param[in]	p_msg	The line to log
		 */
		CU_API static void LogWarning( std::wstring const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a warning, from a std::wostream
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un avertissement, à partir d'un std::wostream
		 *\param[in]	p_msg	The line to log
		 */
		CU_API static void LogWarning( std::wostream const & p_msg );
		/**
		 *\~english
		 *\brief		Logs an error, from a std::string
		 *\param[in]	p_msg		The line to log
		 *\~french
		 *\brief		Log une erreur, à partir d'un std::string
		 *\param[in]	p_msg		The line to log
		 */
		CU_API static void LogError( std::string const & p_msg );
		/**
		 *\~english
		 *\brief		Logs an error, from a std::ostream
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log une erreur, à partir d'un std::ostream
		 *\param[in]	p_msg	The line to log
		 */
		CU_API static void LogError( std::ostream const & p_msg );
		/**
		 *\~english
		 *\brief		Logs an error, from a std::wstring
		 *\param[in]	p_msg		The line to log
		 *\~french
		 *\brief		Log une erreur, à partir d'un std::wstring
		 *\param[in]	p_msg		The line to log
		 */
		CU_API static void LogError( std::wstring const & p_msg );
		/**
		 *\~english
		 *\brief		Logs an error, from a std::wostream
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log une erreur, à partir d'un std::wostream
		 *\param[in]	p_msg	The line to log
		 */
		CU_API static void LogError( std::wostream const & p_msg );
		/**
		 *\~english
		 *\brief		Returns a reference over the instance
		 *\return		The instance
		 *\~french
		 *\brief		Retourne une référence sur l'instance
		 *\return		L'instance
		 */
		CU_API static Logger & GetSingleton();
		/**
		 *\~english
		 *\brief		Returns a pointer over the instance
		 *\return		The instance
		 *\~french
		 *\brief		Retourne un pointeur sur l'instance
		 *\return		L'instance
		 */
		CU_API static Logger * GetSingletonPtr();

	private:
		void DoRegisterCallback( LogCallback p_pfnCallback, void * p_pCaller );
		void DoUnregisterCallback( void * p_pCaller );
		void DoSetFileName( String const & p_logFilePath, LogType p_eLogType = LogType::eCount );
		void DoPushMessage( LogType type, std::string const & message );
		void DoPushMessage( LogType type, std::wstring const & message );
		void DoInitialiseThread();
		void DoCleanupThread();
		void DoFlushQueue();

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
