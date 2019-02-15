/*
See LICENSE file in root folder
*/
#ifndef ___CU_LOGGER_IMPL_H___
#define ___CU_LOGGER_IMPL_H___

#include "CastorUtils/CastorUtilsPrerequisites.hpp"

#include "CastorUtils/Miscellaneous/StringUtils.hpp"

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
		CU_DeclareMap( void *, LogCallback, LoggerCallback );

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		explicit LoggerImpl( LogType level );
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
		 *\param[in]	logger	The logger
		 *\~french
		 *\brief		Initialise les en-têtes, depuis le logger donné
		 *\param[in]	logger	Le logger
		 */
		void initialise( Logger const & logger );
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
		 *\param[in]	callback	The callback
		 *\param[in]	caller		The caller
		 *\~french
		 *\brief		Enregistre un callback
		 *\param[in]	callback	Le callback
		 *\param[in]	caller		L'appelant
		 */
		void registerCallback( LogCallback callback, void * caller );
		/**
		 *\~english
		 *\brief		Unregisters a callback
		 *\param[in]	caller	The caller
		 *\~french
		 *\brief		Désenregistre un callback
		 *\param[in]	caller	L'appelant
		 */
		void unregisterCallback( void * caller );
		/**
		 *\~english
		 *\brief		sets the file for given log level
		 *\param[in]	logFilePath	The file path
		 *\param[in]	logLevel	The log level. If LogType::eCount, sets the file for every log level
		 *\~french
		 *\brief		Définit le fichier pour le niveau de log donné
		 *\param[in]	logFilePath	Le chemin du fichier
		 *\param[in]	logLevel	Le niveau de log. Si LogType::eCount, définit le fichier pour tous les niveaux
		 */
		void setFileName( String const & logFilePath, LogType logLevel );
		/**
		 *\~english
		 *\brief		Prints a message to the console
		 *\param[in]	logLevel	The log level
		 *\param[in]	message		The message
		 *\param[in]	newLine		Tells if the new line character must be added
		 *\~french
		 *\brief		Affiche un message dans la console
		 *\param[in]	logLevel	Le niveau de log
		 *\param[in]	message		Le message
		 *\param[in]	newLine		Dit si le caractère de nouvelle ligne doit être ajouté
		 */
		void printMessage( LogType logLevel, std::string const & message, bool newLine );
		/**
		 *\~english
		 *\brief		Prints a message to the console
		 *\param[in]	logLevel	The log level
		 *\param[in]	message		The message
		 *\param[in]	newLine		Tells if the new line character must be added
		 *\~french
		 *\brief		Affiche un message dans la console
		 *\param[in]	logLevel	Le niveau de log
		 *\param[in]	message		Le message
		 *\param[in]	newLine		Dit si le caractère de nouvelle ligne doit être ajouté
		 */
		void printMessage( LogType logLevel, std::wstring const & message, bool newLine );
		/**
		 *\~english
		 *\brief		Logs a message queue
		 *\param[in]	queue	The message queue
		 *\~french
		 *\brief		Logge une file de messages
		 *\param[in]	queue	La file de messages
		 */
		void logMessageQueue( MessageQueue const & queue );

	private:
		/**
		 *\~english
		 *\brief		Prints a message to the console
		 *\param[in]	logLevel	The log level
		 *\param[in]	message		The message
		 *\param[in]	newLine		Tells if the new line character must be added
		 *\~french
		 *\brief		Affiche un message dans la console
		 *\param[in]	logLevel	Le niveau de log
		 *\param[in]	message		Le message
		 *\param[in]	newLine		Dit si le caractère de nouvelle ligne doit être ajouté
		 */
		void doPrintMessage( LogType logLevel, String const & message, bool newLine );
		/**
		 *\~english
		 *\brief		Prints a line to the console
		 *\param[in]	line		The line
		 *\param[in]	logLevel	The log level
		 *\param[in]	newLine		Tells if the new line character must be added
		 *\~french
		 *\brief		Affiche une ligne de texte sur la console
		 *\param[in]	line		La ligne de texte
		 *\param[in]	logLevel	Le niveau de log
		 *\param[in]	newLine		Dit si le caractère de nouvelle ligne doit être ajouté
		 */
		void doPrintLine( String const & line, LogType logLevel, bool newLine );
		/**
		 *\~english
		 *\brief		Logs a line in the given stream
		 *\param[in]	timestamp	The line timestamp
		 *\param[in]	line		The line
		 *\param[in]	stream		The stream
		 *\param[in]	logLevel	The log level
		 *\param[in]	newLine		Tells if the new line character must be added
		 *\~french
		 *\brief		Affiche une ligne de texte dans le flux donné
		 *\param[in]	timestamp	Le timestamp de la ligne
		 *\param[in]	line		La ligne de texte
		 *\param[in]	stream		Le flux
		 *\param[in]	logLevel	Le niveau de log
		 *\param[in]	newLine		Dit si le caractère de nouvelle ligne doit être ajouté
		 */
		void doLogLine( String const & timestamp, String const & line, StringStream & stream, LogType logLevel, bool newLine );

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
