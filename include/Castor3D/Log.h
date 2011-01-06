/*
This source file is part of Castor3D (http://dragonjoker.co.cc

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
#ifndef ___Castor_Log___
#define ___Castor_Log___

#include "Prerequisites.h"

namespace Castor3D
{
	//! Log management class
	/*!
	Implements log facilities. Create a Log with a filename, then write logs into that file
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API Logger : protected Theory::AutoSingleton<Logger>, public MemoryTraced<Logger>
	{
	public:
		/*!
		Defines the various log types
		*/
		typedef enum
		{
			eMessage,	//!< Message type log
			eWarning,	//!< Warning type log
			eError,		//!< Error type log
			eNbTypes,	//!< Number of log types
		}
		eLOG_TYPE;

	private:
		friend class Theory::AutoSingleton<Logger>;
		static String s_logFilePath[eNbTypes];
		static String s_strHeaders[eNbTypes];
		static MultiThreading::RecursiveMutex s_mutex;

	private:
		/**
		 * Constructor
		 */
		Logger();
		/**
		 * Destructor
		 */
		~Logger();

	public:
		/**
		 * Sets the log file address
		 *@param p_logFilePath : [in] The log file path
		 *@param p_eLogType : [in] The log type
		 */
		static void SetFileName( const String & p_logFilePath, eLOG_TYPE p_eLogType=eNbTypes);
		
		/**
		 * Logs a line in the log file, using va_args
		 *@param p_eLogType : [in] The log type
		 *@param p_format : [in] the line format
		 *@param ... : POD arguments, following printf format
		 */
		static void Log( eLOG_TYPE p_eLogType, const char * p_format, ...);
		/**
		 * Logs a line, from a string
		 *@param p_eLogType : [in] The log type
		 *@param p_msg : [in] The line to log
		 */
		static void Log( eLOG_TYPE p_eLogType, const std::string & p_msg);
		/**
		 * Logs an unicode line in the log file, using va_args
		 *@param p_eLogType : [in] The log type
		 *@param p_format : [in] the line format
		 *@param ... : POD arguments, following printf format
		 */
		static void Log( eLOG_TYPE p_eLogType, const wchar_t * p_format , ...);
		/**
		 * Logs a line, from a wstring
		 *@param p_eLogType : [in] The log type
		 *@param p_msg : [in] The line to log
		 */
		static void Log( eLOG_TYPE p_eLogType, const std::wstring & p_msg);

		/**
		 * Logs a line in the log file, using va_args
		 *@param p_format : [in] the line format
		 *@param ... : POD arguments, following printf format
		 */
		static void LogMessage( const char * p_format, ...);
		/**
		 * Logs a line, from a string
		 *@param p_msg : [in] The line to log
		 */
		static void LogMessage( const std::string & p_msg);
		/**
		 * Logs an unicode line in the log file, using va_args
		 *@param p_format : [in] the line format
		 *@param ... : POD arguments, following printf format
		 */
		static void LogMessage( const wchar_t * p_format , ...);
		/**
		 * Logs a line, from a wstring
		 *@param p_msg : [in] The line to log
		 */
		static void LogMessage( const std::wstring & p_msg);

		/**
		 * Logs a line in the log file, using va_args
		 *@param p_format : [in] the line format
		 *@param ... : POD arguments, following printf format
		 */
		static void LogWarning( const char * p_format, ...);
		/**
		 * Logs a line, from a string
		 *@param p_msg : [in] The line to log
		 */
		static void LogWarning( const std::string & p_msg);
		/**
		 * Logs an unicode line in the log file, using va_args
		 *@param p_format : [in] the line format
		 *@param ... : POD arguments, following printf format
		 */
		static void LogWarning( const wchar_t * p_format , ...);
		/**
		 * Logs a line, from a wstring
		 *@param p_msg : [in] The line to log
		 */
		static void LogWarning( const std::wstring & p_msg);

		/**
		 * Logs a line in the log file, using va_args
		 *@param p_format : [in] the line format
		 *@param ... : POD arguments, following printf format
		 */
		static void LogError( const char * p_format, ...);
		/**
		 * Logs a line, from a string
		 *@param p_msg : [in] The line to log
		 */
		static void LogError( const std::string & p_msg);
		/**
		 * Logs an unicode line in the log file, using va_args
		 *@param p_format : [in] the line format
		 *@param ... : POD arguments, following printf format
		 */
		static void LogError( const wchar_t * p_format , ...);
		/**
		 * Logs a line, from a wstring
		 *@param p_msg : [in] The line to log
		 */
		static void LogError( const std::wstring & p_msg);

	private:
		static void _logMessage( eLOG_TYPE p_eLogType, const String & p_strToLog);
	};
}

#endif
