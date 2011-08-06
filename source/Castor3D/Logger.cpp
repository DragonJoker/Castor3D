#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Logger.hpp"

#ifdef _WIN32
#	include <Windows.h>
#endif

using namespace Castor3D;

String Logger :: s_logFilePath[eLOG_TYPE_COUNT];
String Logger :: s_strHeaders[eLOG_TYPE_COUNT] = { cuEmptyString, cuT( "***WARNING*** "), cuT( "***ERROR*** ") };
eLOG_TYPE Logger :: m_eCurrentLogType;
MultiThreading::RecursiveMutex Logger :: s_mutex;
bool Logger :: m_bPrintDateTime = true;

Logger :: Logger()
{
#ifdef _WIN32
#   ifndef __GNUG__
#   	ifndef NDEBUG
	if (AllocConsole())
	{
		FILE * l_dump;
		freopen_s( & l_dump, "conout$", "w", stdout);
		freopen_s( & l_dump, "conout$", "w", stderr);
	}
#       endif
#	endif
#endif
}

Logger :: ~Logger()
{
#ifdef _WIN32
#   ifndef __GNUG__
#	    ifndef NDEBUG
	FreeConsole();
#	    endif
#   endif
#endif
}

void Logger :: SetFileName( String const & p_logFilePath, eLOG_TYPE p_eLogType)
{
	GetSingleton();

	if (p_eLogType == eLOG_TYPE_COUNT)
	{
		for (int i = 0 ; i < eLOG_TYPE_COUNT ; i++)
		{
			s_logFilePath[i] = p_logFilePath;
		}
	}
	else
	{
		s_logFilePath[p_eLogType] = p_logFilePath;
	}

	FILE * l_pFile;
	l_pFile = fopen( p_logFilePath.char_str(), "w");

	if (l_pFile)
	{
		fclose( l_pFile);
	}
}

void Logger :: LogMessage( char const * p_pFormat, ...)
{
	m_bPrintDateTime = true;
	xchar l_pText[256];
	va_list l_vaList;

	if (p_pFormat)
	{
		va_start( l_vaList, p_pFormat);
		String l_strFormat( p_pFormat);
		Vsnprintf( l_pText, 256, 256, l_strFormat.c_str(), l_vaList);
		va_end( l_vaList);
		_logMessage( eLOG_TYPE_MESSAGE, l_pText, true);
	}
}

void Logger :: LogMessage( std::string const & p_msg)
{
	m_bPrintDateTime = true;
	_logMessage( eLOG_TYPE_MESSAGE, p_msg, true);
}

void Logger :: LogMessage( wchar_t const * p_pFormat , ...)
{
	m_bPrintDateTime = true;
	xchar l_pText[256];
	va_list l_vaList;

	if (p_pFormat)
	{
		va_start( l_vaList, p_pFormat);
		String l_strFormat( p_pFormat);
		Vsnprintf( l_pText, 256, 256, l_strFormat.c_str(), l_vaList);
		va_end( l_vaList);
		_logMessage( eLOG_TYPE_MESSAGE, l_pText, true);
	}
}

void Logger :: LogMessage( std::wstring const & p_msg)
{
	m_bPrintDateTime = true;
	_logMessage( eLOG_TYPE_MESSAGE, p_msg, true);
}

void Logger :: LogWarning( char const * p_pFormat, ...)
{
	m_bPrintDateTime = true;
	xchar l_pText[256];
	va_list l_vaList;

	if (p_pFormat)
	{
		va_start( l_vaList, p_pFormat);
		String l_strFormat( p_pFormat);
		Vsnprintf( l_pText, 256, 256, l_strFormat.c_str(), l_vaList);
		va_end( l_vaList);
		_logMessage( eLOG_TYPE_WARNING, l_pText, true);
	}
}

void Logger :: LogWarning( std::string const & p_msg)
{
	m_bPrintDateTime = true;
	_logMessage( eLOG_TYPE_WARNING, p_msg, true);
}

void Logger :: LogWarning( wchar_t const * p_pFormat , ...)
{
	m_bPrintDateTime = true;
	xchar l_pText[256];
	va_list l_vaList;

	if (p_pFormat)
	{
		va_start( l_vaList, p_pFormat);
		String l_strFormat( p_pFormat);
		Vsnprintf( l_pText, 256, 256, l_strFormat.c_str(), l_vaList);
		va_end( l_vaList);
		_logMessage( eLOG_TYPE_WARNING, l_pText, true);
	}
}

void Logger :: LogWarning( std::wstring const & p_msg)
{
	m_bPrintDateTime = true;
	_logMessage( eLOG_TYPE_WARNING, p_msg, true);
}


void Logger :: LogError( char const * p_pFormat, ...)
{
	m_bPrintDateTime = true;
	xchar l_pText[256];
	va_list l_vaList;

	if (p_pFormat)
	{
		va_start( l_vaList, p_pFormat);
		String l_strFormat( p_pFormat);
		Vsnprintf( l_pText, 256, 256, l_strFormat.c_str(), l_vaList);
		va_end( l_vaList);
		_logMessage( eLOG_TYPE_ERROR, l_pText, true);
	}
}

void Logger :: LogError( std::string const & p_msg, bool p_bThrow)throw( bool)
{
	m_bPrintDateTime = true;
	_logMessage( eLOG_TYPE_ERROR, p_msg, true);

	if (p_bThrow)
	{
		throw false;
	}
}

void Logger :: LogError( wchar_t const * p_pFormat , ...)
{
	m_bPrintDateTime = true;
	xchar l_pText[256];
	va_list l_vaList;

	if (p_pFormat)
	{
		va_start( l_vaList, p_pFormat);
		String l_strFormat( p_pFormat);
		Vsnprintf( l_pText, 256, 256, l_strFormat.c_str(), l_vaList);
		va_end( l_vaList);
		_logMessage( eLOG_TYPE_ERROR, l_pText, true);
	}
}

void Logger :: LogError( std::wstring const & p_msg, bool p_bThrow)throw( bool)
{
	m_bPrintDateTime = true;
	_logMessage( eLOG_TYPE_ERROR, p_msg, true);

	if (p_bThrow)
	{
		throw false;
	}
}

void Logger :: _logMessage( eLOG_TYPE p_eLogType, String const & p_strToLog, bool p_bNewLine)
{
	String l_strToLog;

	if (m_bPrintDateTime)
	{
		struct tm * l_dtToday = nullptr;
		time_t l_tTime;

		time( & l_tTime);
		Localtime( l_dtToday, & l_tTime);

		l_strToLog << (l_dtToday->tm_year + 1900) << cuT( "-");
		l_strToLog << (l_dtToday->tm_mon + 1 < 10 ? cuT( "0") : cuT( "")) << (l_dtToday->tm_mon + 1) << cuT( "-") << (l_dtToday->tm_mday < 10 ? cuT( "0") : cuT( "")) << l_dtToday->tm_mday;
		l_strToLog << cuT( " - ") << (l_dtToday->tm_hour < 10 ? cuT( "0") : cuT( "")) << l_dtToday->tm_hour << cuT( ":") << (l_dtToday->tm_min < 10 ? cuT( "0") : cuT( "")) << l_dtToday->tm_min << cuT( ":") << (l_dtToday->tm_sec < 10 ? cuT( "0") : cuT( "")) << l_dtToday->tm_sec << cuT( "s");
		l_strToLog << cuT( " - ") << s_strHeaders[p_eLogType];
	}

	l_strToLog << p_strToLog;

#ifndef NDEBUG
	Cout( p_strToLog.c_str());
	if (p_bNewLine)
	{
		Cout( std::endl);
	}
#endif

	do
	{
		CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( s_mutex);
		File l_logFile( s_logFilePath[p_eLogType], File::eOPEN_MODE_APPEND, File::eENCODING_MODE_ASCII);

		if (l_logFile.IsOk())
		{
			l_logFile.WriteLine( l_strToLog);

			if (p_bNewLine)
			{
				l_logFile.Print( 2, cuT( "\n"));
			}
		}
	}
	while (0);
}
