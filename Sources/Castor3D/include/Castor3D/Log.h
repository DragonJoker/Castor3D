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

namespace Castor3D
{
	//! Log management class
	/*!
	Implements log facilities. Create a Log with a filename, then write logs into that file
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class CS3D_API Log
	{
	private:
		static String s_logFilePath;	//!< The log file path
		static Log s_singleton;

	public:
		/**
		 * Constructor
		 */
		Log();
		/**
		 * Destructor
		 */
		~Log();
		/**
		 * Sets the log file address
		 *@param p_logFilePath : [in] The log file path
		 */
		static void SetFileName( const String & p_logFilePath);
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

	private:
		static void _logMessage( const String & p_strToLog);
	};
}

#endif
