/*
See LICENSE file in root folder
*/
#ifndef ___CU_LOGGER_IMPL_H___
#define ___CU_LOGGER_IMPL_H___

#include "CastorUtilsPrerequisites.hpp"

#include "Miscellaneous/StringUtils.hpp"

#include <mutex>

namespace castor
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		27/08/2012
	\version	0.7.0.0
	\~english
	\brief		Helper class for Logger, level dependant
	\~french
	\brief		Classe d'aide pour Logger, dépendante du niveau de log
	*/
	class LoggerImpl
	{
	private:
		DECLARE_MAP( void *, LogCallback, LoggerCallback );

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		explicit LoggerImpl( LogType p_level );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~LoggerImpl();
		/**
		 *\~english
		 *\brief		Initialises the headers, from the given logger
		 *\param[in]	p_logger	The logger
		 *\~french
		 *\brief		Initialise les en-têtes, depuis le logger donné
		 *\param[in]	p_logger	Le logger
		 */
		void initialise( Logger const & p_logger );
		/**
		 *\~english
		 *\brief		Cleans up the instance
		 *\~french
		 *\brief		Nettoie l'instance
		 */
		void cleanup();
		/**
		 *\~english
		 *\brief		Registers a callback
		 *\param[in]	p_callback	The callback
		 *\param[in]	p_caller	The caller
		 *\~french
		 *\brief		Enregistre un callback
		 *\param[in]	p_callback	Le callback
		 *\param[in]	p_caller	L'appelant
		 */
		void registerCallback( LogCallback p_callback, void * p_caller );
		/**
		 *\~english
		 *\brief		Unregisters a callback
		 *\param[in]	p_caller	The caller
		 *\~french
		 *\brief		Désenregistre un callback
		 *\param[in]	p_caller	L'appelant
		 */
		void unregisterCallback( void * p_caller );
		/**
		 *\~english
		 *\brief		sets the file for given log level
		 *\param[in]	p_logFilePath	The file path
		 *\param[in]	p_logLevel		The log level. If LogType::eCount, sets the file for every log level
		 *\~french
		 *\brief		Définit le fichier pour le niveau de log donné
		 *\param[in]	p_logFilePath	Le chemin du fichier
		 *\param[in]	p_logLevel		Le niveau de log. Si LogType::eCount, définit le fichier pour tous les niveaux
		 */
		void setFileName( String const & p_logFilePath, LogType p_logLevel );
		/**
		 *\~english
		 *\brief		Prints a message to the console
		 *\param[in]	p_logLevel	The log level
		 *\param[in]	p_message	The message
		 *\param[in]	p_newLine	Tells if the new line character must be added
		 *\~french
		 *\brief		Affiche un message dans la console
		 *\param[in]	p_logLevel	Le niveau de log
		 *\param[in]	p_message	Le message
		 *\param[in]	p_newLine	Dit si le caractère de nouvelle ligne doit être ajouté
		 */
		void printMessage( LogType p_logLevel, std::string const & p_message, bool p_newLine );
		/**
		 *\~english
		 *\brief		Prints a message to the console
		 *\param[in]	p_logLevel	The log level
		 *\param[in]	p_message	The message
		 *\param[in]	p_newLine	Tells if the new line character must be added
		 *\~french
		 *\brief		Affiche un message dans la console
		 *\param[in]	p_logLevel	Le niveau de log
		 *\param[in]	p_message	Le message
		 *\param[in]	p_newLine	Dit si le caractère de nouvelle ligne doit être ajouté
		 */
		void printMessage( LogType p_logLevel, std::wstring const & p_message, bool p_newLine );
		/**
		 *\~english
		 *\brief		Logs a message queue
		 *\param[in]	p_queue	The message queue
		 *\~french
		 *\brief		Logge une file de messages
		 *\param[in]	p_queue	La file de messages
		 */
		void logMessageQueue( MessageQueue const & p_queue );

	private:
		/**
		 *\~english
		 *\brief		Prints a message to the console
		 *\param[in]	p_logLevel	The log level
		 *\param[in]	p_message	The message
		 *\~french
		 *\brief		Affiche un message dans la console
		 *\param[in]	p_logLevel	Le niveau de log
		 *\param[in]	p_message	Le message
		 */
		void doPrintMessage( LogType p_logLevel, String const & p_message, bool p_newLine );
		/**
		 *\~english
		 *\brief		Prints a line to the console
		 *\param[in]	p_line		The line
		 *\param[in]	p_logLevel	The log level
		 *\~french
		 *\brief		Affiche une ligne de texte sur la console
		 *\param[in]	p_line		La ligne de texte
		 *\param[in]	p_logLevel	Le niveau de log
		 */
		void doPrintLine( String const & p_line, LogType p_logLevel, bool p_newLine );
		/**
		 *\~english
		 *\brief		Logs a line in the given stream
		 *\param[in]	p_timestamp	The line timestamp
		 *\param[in]	p_line		The line
		 *\param[in]	p_stream	The stream
		 *\param[in]	p_logLevel	The log level
		 *\~french
		 *\brief		Affiche une ligne de texte dans le flux donné
		 *\param[in]	p_timestamp	Le timestamp de la ligne
		 *\param[in]	p_line		La ligne de texte
		 *\param[in]	p_stream	Le flux
		 *\param[in]	p_logLevel	Le niveau de log
		 */
		void doLogLine( String const & p_timestamp, String const & p_line, StringStream & p_stream, LogType p_logLevel, bool p_newLine );

	private:
		//! The files paths, per log level
		std::array< String, size_t( LogType::eCount ) > m_logFilePath;
		//! The headers, per log level
		std::array< String, size_t( LogType::eCount ) > m_headers;
		//! The console
		std::unique_ptr< ProgramConsole > m_console;
		//! Registered callbacks
		LoggerCallbackMap m_mapCallbacks;
		//! Protects the registered callbacks map
		std::mutex m_mutexCallbacks;
	};
}

#endif
