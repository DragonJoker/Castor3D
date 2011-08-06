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
#ifndef ___Castor_Log___
#define ___Castor_Log___

#include "Prerequisites.hpp"

#pragma warning( push)
#pragma warning( disable:4290)

namespace Castor3D
{
	/*!
	Defines the various log types
	*/
	typedef enum
	{	eLOG_TYPE_MESSAGE	//!< Message type log
	,	eLOG_TYPE_WARNING	//!< Warning type log
	,	eLOG_TYPE_ERROR		//!< Error type log
	,	eLOG_TYPE_COUNT		//!< Number of log types
	}	eLOG_TYPE;
	//! Log management class C3D_API
	/*!
	Implements log facilities. Create a Log with a filename, then write logs into that file
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API Logger : protected Theory::AutoSingleton<Logger>, public MemoryTraced<Logger>
	{
	private:
		friend class Theory::AutoSingleton<Logger>;
		static String s_logFilePath[eLOG_TYPE_COUNT];
		static String s_strHeaders[eLOG_TYPE_COUNT];
		static MultiThreading::RecursiveMutex s_mutex;
		static eLOG_TYPE m_eCurrentLogType;
		static bool m_bPrintDateTime;

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
		static void SetFileName( String const & p_logFilePath, eLOG_TYPE p_eLogType=eLOG_TYPE_COUNT);
		/**
		 * Logs a line in the log file, using va_args
		 *@param p_format : [in] the line format
		 *@param ... : POD arguments, following printf format
		 */
		static void LogMessage( char const * p_format, ...);
		/**
		 * Logs a line, from a string
		 *@param p_msg : [in] The line to log
		 */
		static void LogMessage( std::string const & p_msg);
		/**
		 * Logs an unicode line in the log file, using va_args
		 *@param p_format : [in] the line format
		 *@param ... : POD arguments, following printf format
		 */
		static void LogMessage( wchar_t const * p_format , ...);
		/**
		 * Logs a line, from a wstring
		 *@param p_msg : [in] The line to log
		 */
		static void LogMessage( std::wstring const & p_msg);

		/**
		 * Logs a line in the log file, using va_args
		 *@param p_format : [in] the line format
		 *@param ... : POD arguments, following printf format
		 */
		static void LogWarning( char const * p_format, ...);
		/**
		 * Logs a line, from a string
		 *@param p_msg : [in] The line to log
		 */
		static void LogWarning( std::string const & p_msg);
		/**
		 * Logs an unicode line in the log file, using va_args
		 *@param p_format : [in] the line format
		 *@param ... : POD arguments, following printf format
		 */
		static void LogWarning( wchar_t const * p_format , ...);
		/**
		 * Logs a line, from a wstring
		 *@param p_msg : [in] The line to log
		 */
		static void LogWarning( std::wstring const & p_msg);

		/**
		 * Logs a line in the log file, using va_args
		 *@param p_format : [in] the line format
		 *@param ... : POD arguments, following printf format
		 */
		static void LogError( char const * p_format, ...);
		/**
		 * Logs a line, from a string
		 *@param p_msg : [in] The line to log
		 */
		static void LogError( std::string const & p_msg, bool p_bThrow=true)throw( bool);
		/**
		 * Logs an unicode line in the log file, using va_args
		 *@param p_format : [in] the line format
		 *@param ... : POD arguments, following printf format
		 */
		static void LogError( wchar_t const * p_format , ...);
		/**
		 * Logs a line, from a wstring
		 *@param p_msg : [in] The line to log
		 */
		static void LogError( std::wstring const & p_msg, bool p_bThrow=true)throw( bool);

		static Logger & Log( eLOG_TYPE p_eLogType)
		{
			m_eCurrentLogType = p_eLogType;
			m_bPrintDateTime = true;
			return GetSingleton();
		}

		template <class T> Logger & operator <<( T const & tToLog)
		{
			String stream;
			stream << tToLog;
			_logMessage( m_eCurrentLogType, stream, false);
			m_bPrintDateTime = false;
			return * this;
		}

	private:
		static void _logMessage( eLOG_TYPE p_eLogType, String const & p_strToLog, bool p_bNewLine);
	};
}

#pragma warning( pop)

#endif
