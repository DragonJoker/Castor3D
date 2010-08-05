
#ifndef ___C3D_Log___
#define ___C3D_Log___

#include "Module_Utils.h"

namespace Castor3D
{
	//! Log management class
	/*!
	Implements log facilities. Create a Log with a filename, then write logs into that file
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class Log
	{
	private:
		//! The log file path
		static String s_logFilePath;

	public:
		/**
		 * Constructor
		 *@param p_logFilePath : The file path
		 */
		Log( String p_logFilePath);
		/**
		 * Destructor
		 */
		~Log();
		/**
		 * Logs a line in the log file, using va_args
		 *@param p_format : the line format
		 *@param ... : the arguments, following printf format
		 */
		static void LogMessage( const char * p_format, ...);
		/**
		 * Logs a line, from a string
		 *@param p_msg : The line to log
		 */
		static void LogMessage( const std::string & p_msg);
		/**
		 * Logs an unicode line in the log file, using va_args
		 *@param p_format : the line format
		 *@param ... : the arguments, following printf format
		 */
		static void LogMessage( const wchar_t * p_format , ...);
		/**
		 * Logs a line, from a wstring
		 *@param p_msg : The line to log
		 */
		static void LogMessage( const std::wstring & p_msg);
	};
}

#endif

