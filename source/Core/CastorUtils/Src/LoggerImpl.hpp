/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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

#pragma warning( push )
#pragma warning( disable:4290 )

namespace Castor
{
	/** Base class for a message representation
	*/
	struct MessageBase
	{
		/** Constructor
		@param[in] type
			The message type
		*/
		MessageBase( ELogType type )
			: m_type( type )
		{
		}

		/** Retrieves the message content
		@return
			The message text
		*/
		virtual String GetMessage() = 0;

		//! The message type
		ELogType m_type;
	};

	/** Template class, holding character type dependant message text
	*/
	template< typename Char >
	struct BasicMessage
		: public MessageBase
	{
		typedef std::basic_string< Char > string_type;

		/** Constructor
		@param[in] type
			The message type
		@param[in] message
			The message text
		*/
		BasicMessage( ELogType type, string_type const & message )
			: MessageBase( type )
			, m_message( message )
		{
		}

		//@copydoc Database::SMessageBase::GetMessage
		virtual String GetMessage()
		{
			return str_utils::string_cast< String >( m_message );
		}

		//! The message text
		string_type m_message;
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
		/** Constructor
		*/
		LoggerImpl();

		/** Destructor
		*/
		virtual ~LoggerImpl();

		/** Initialises the headers, from the given logger
		@param[in] logger
			The logger
		*/
		void Initialise( Logger const & logger );

		/** Cleans up the class
		*/
		void Cleanup();

		/** Registers a callback
		*/
		void RegisterCallback( LogCallback p_pfnCallback, void * p_pCaller );

		/** Unregisters a callback
		*/
		void UnregisterCallback( void * p_pCaller );

		/** Sets the file for given log level
		@param[in] logFilePath
			The file path
		@param[in] logLevel
			The log level.
			If ELogType_COUNT, sets the file for every log level
		*/
		void SetFileName( String const & logFilePath, ELogType logLevel );

		/** Prints a message to the console
		@param[in] logLevel
			The log level.
		@param[in] message
			The message.
		*/
		void PrintMessage( ELogType logLevel, std::string const & message );

		/** Prints a message to the console
		@param[in] logLevel
			The log level.
		@param[in] message
			The message.
		*/
		void PrintMessage( ELogType logLevel, std::wstring const & message );

		/** Logs a message queue
		@param[in] queue
			The messages
		*/
		void LogMessageQueue( MessageQueue const & queue );

	private:
		/** Prints a message to the console
		@param[in] logLevel
			The log level.
		@param[in] message
			The message.
		*/
		void DoPrintMessage( ELogType logLevel, String const & message );

		/** Prints a line to the console
		@param[in] line
			The line
		@param[in] logType
			The log level
		*/
		void DoPrintLine( String const & line, ELogType logType );

		/** Logs a line in the given file
		@param[in] timestamp
			The line timestamp
		@param[in] line
			The line
		@param logFile
			The file
		@param[in] logType
			The log level
		*/
		void DoLogLine( String const & timestamp, String const & line, FILE * logFile, ELogType logType );

	private:
		//! The files paths, per log level
		String m_logFilePath[ELogType_COUNT];
		//! The headers, per log level
		String m_headers[ELogType_COUNT];
		//! The console
		std::unique_ptr< ProgramConsole > m_console;
		//! Registered callbacks
		LoggerCallbackMap m_mapCallbacks;
	};
}

#pragma warning( pop)

#endif
