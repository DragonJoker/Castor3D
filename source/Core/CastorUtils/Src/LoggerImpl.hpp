/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___CU_LOGGER_IMPL_H___
#define ___CU_LOGGER_IMPL_H___

#include "CastorUtilsPrerequisites.hpp"

#include "ELogType.hpp"
#include "StringUtils.hpp"

#include <mutex>

namespace Castor
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		27/08/2012
	\version	0.7.0.0
	\~english
	\brief		Base class for a message representation
	\~french
	\brief		Classe de base pour la représentation d'un message
	*/
	struct MessageBase
	{
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_type		The message type.
		 *\param[in]	p_message	The message text.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_type		Le type de message.
		 *\param[in]	p_message	Le texte du message.
		 */
		inline MessageBase( ELogType p_type, std::string const & p_message )
			: m_type( p_type )
			, m_message( p_message )
		{
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructor.
		 */
		virtual ~MessageBase()
		{
		}
		/**
		 *\~english
		 *\return		The message content.
		 *\~french
		 *\return		Le contenu du message.
		 */
		inline std::string const & GetMessage()const
		{
			return m_message;
		}

		//! The message type.
		ELogType m_type;
		//! The message text.
		std::string m_message;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		27/08/2012
	\version	0.7.0.0
	\~english
	\brief		Template class, holding character type dependant message text
	\~french
	\brief		Classe template contenant le texte du message en fonction du type de caractère
	*/
	template< typename Char >
	struct BasicMessage
		: public MessageBase
	{
		typedef std::basic_string< Char > string_type;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_type		The message type.
		 *\param[in]	p_message	The message text.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_type		Le type de message.
		 *\param[in]	p_message	Le texte du message.
		 */
		inline BasicMessage( ELogType p_type, string_type const & p_message )
			: MessageBase( p_type, string::string_cast< char >( p_message ) )
		{
		}
	};

	//! A char message
	typedef BasicMessage< char > Message;
	//! A wchar_t message
	typedef BasicMessage< wchar_t > WMessage;

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
		LoggerImpl();
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
		void Initialise( Logger const & p_logger );
		/**
		 *\~english
		 *\brief		Cleans up the instance
		 *\~french
		 *\brief		Nettoie l'instance
		 */
		void Cleanup();
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
		void RegisterCallback( LogCallback p_callback, void * p_caller );
		/**
		 *\~english
		 *\brief		Unregisters a callback
		 *\param[in]	p_caller	The caller
		 *\~french
		 *\brief		Désenregistre un callback
		 *\param[in]	p_caller	L'appelant
		 */
		void UnregisterCallback( void * p_caller );
		/**
		 *\~english
		 *\brief		Sets the file for given log level
		 *\param[in]	p_logFilePath	The file path
		 *\param[in]	p_logLevel		The log level. If ELogType_COUNT, sets the file for every log level
		 *\~french
		 *\brief		Définit le fichier pour le niveau de log donné
		 *\param[in]	p_logFilePath	Le chemin du fichier
		 *\param[in]	p_logLevel		Le niveau de log. Si ELogType_COUNT, définit le fichier pour tous les niveaux
		 */
		void SetFileName( String const & p_logFilePath, ELogType p_logLevel );
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
		void PrintMessage( ELogType p_logLevel, std::string const & p_message );
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
		void PrintMessage( ELogType p_logLevel, std::wstring const & p_message );
		/**
		 *\~english
		 *\brief		Logs a message queue
		 *\param[in]	p_queue	The message queue
		 *\~french
		 *\brief		Logge une file de messages
		 *\param[in]	p_queue	La file de messages
		 */
		void LogMessageQueue( MessageQueue const & p_queue );

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
		void DoPrintMessage( ELogType p_logLevel, String const & p_message );
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
		void DoPrintLine( String const & p_line, ELogType p_logLevel );
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
		void DoLogLine( String const & p_timestamp, String const & p_line, StringStream & p_stream, ELogType p_logLevel );

	private:
		//! The files paths, per log level
		String m_logFilePath[ELogType_COUNT];
		//! The headers, per log level
		String m_headers[ELogType_COUNT];
		//! The console
		std::unique_ptr< ProgramConsole > m_console;
		//! Registered callbacks
		LoggerCallbackMap m_mapCallbacks;
		//! Protects the registered callbacks map
		std::mutex m_mutexCallbacks;
	};
}

#endif
