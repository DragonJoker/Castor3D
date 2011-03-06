#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/Log.h"

#ifdef _WIN32
#	include <Windows.h>
#endif

using namespace Castor3D;

String Logger :: s_logFilePath[eNbTypes];
String Logger :: s_strHeaders[eNbTypes] = { C3DEmptyString, CU_T( "***WARNING*** "), CU_T( "***ERROR*** ") };
Logger::eLOG_TYPE Logger :: m_eCurrentLogType;
MultiThreading::RecursiveMutex Logger :: s_mutex;

Logger :: Logger()
{
#ifdef _WIN32
#   ifndef __GNUG__
#   	ifdef _DEBUG
	if (AllocConsole() == TRUE)
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
#	    ifdef _DEBUG
	FreeConsole();
#	    endif
#   endif
#endif
}

void Logger :: SetFileName( const String & p_logFilePath, eLOG_TYPE p_eLogType)
{
	GetSingleton();

	if (p_eLogType == eNbTypes)
	{
		for (int i = 0 ; i < eNbTypes ; i++)
		{
			s_logFilePath[i] = p_logFilePath;
		}
	}
	else
	{
		s_logFilePath[p_eLogType] = p_logFilePath;
	}

	FILE * l_pFile;
	FOpen( l_pFile, p_logFilePath.char_str(), "w");

	if (l_pFile != NULL)
	{
		fclose( l_pFile);
	}
}

void Logger :: LogMessage( const char * p_pFormat, ...)
{
	char l_pText[256];
	va_list l_vaList;

	if (p_pFormat != NULL)
	{
		va_start( l_vaList, p_pFormat);
		Vsnprintf( l_pText, 256, 256, p_pFormat, l_vaList);
		va_end( l_vaList);
		_logMessage( eMessage, l_pText);
	}
}

void Logger :: LogMessage( const std::string & p_msg)
{
	_logMessage( eMessage, p_msg);
}

void Logger :: LogMessage( const wchar_t * p_pFormat , ...)
{
	char l_pText[256];
	va_list l_vaList;

	if (p_pFormat != NULL)
	{
		va_start( l_vaList, p_pFormat);
		String l_strFormat( p_pFormat);
		Vsnprintf( l_pText, 256, 256, l_strFormat.char_str(), l_vaList);
		va_end( l_vaList);
		_logMessage( eMessage, l_pText);
	}
}

void Logger :: LogMessage( const std::wstring & p_msg)
{
	_logMessage( eMessage, p_msg);
}

void Logger :: LogWarning( const char * p_pFormat, ...)
{
	char l_pText[256];
	va_list l_vaList;

	if (p_pFormat != NULL)
	{
		va_start( l_vaList, p_pFormat);
		Vsnprintf( l_pText, 256, 256, p_pFormat, l_vaList);
		va_end( l_vaList);
		_logMessage( eWarning, l_pText);
	}
}

void Logger :: LogWarning( const std::string & p_msg)
{
	_logMessage( eWarning, p_msg);
}

void Logger :: LogWarning( const wchar_t * p_pFormat , ...)
{
	char l_pText[256];
	va_list l_vaList;

	if (p_pFormat != NULL)
	{
		va_start( l_vaList, p_pFormat);
		String l_strFormat( p_pFormat);
		Vsnprintf( l_pText, 256, 256, l_strFormat.char_str(), l_vaList);
		va_end( l_vaList);
		_logMessage( eWarning, l_pText);
	}
}

void Logger :: LogWarning( const std::wstring & p_msg)
{
	_logMessage( eWarning, p_msg);
}


void Logger :: LogError( const char * p_pFormat, ...)
{
	char l_pText[256];
	va_list l_vaList;

	if (p_pFormat != NULL)
	{
		va_start( l_vaList, p_pFormat);
		Vsnprintf( l_pText, 256, 256, p_pFormat, l_vaList);
		va_end( l_vaList);
		_logMessage( eError, l_pText);
	}
}

void Logger :: LogError( const std::string & p_msg, bool p_bThrow)
{
	_logMessage( eError, p_msg);

	if (p_bThrow)
	{
		throw false;
	}
}

void Logger :: LogError( const wchar_t * p_pFormat , ...)
{
	char l_pText[256];
	va_list l_vaList;

	if (p_pFormat != NULL)
	{
		va_start( l_vaList, p_pFormat);
		String l_strFormat( p_pFormat);
		Vsnprintf( l_pText, 256, 256, l_strFormat.char_str(), l_vaList);
		va_end( l_vaList);
		_logMessage( eError, l_pText);
	}
}

void Logger :: LogError( const std::wstring & p_msg, bool p_bThrow)
{
	_logMessage( eError, p_msg);

	if (p_bThrow)
	{
		throw false;
	}
}

void Logger :: _logMessage( eLOG_TYPE p_eLogType, const String & p_strToLog)
{
	CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( s_mutex);
	File l_logFile( s_logFilePath[p_eLogType], File::eAppend);

	if (l_logFile.IsOk())
	{
		struct tm * l_dtToday = NULL;
		time_t l_tTime;

		time( & l_tTime);
		Localtime( l_dtToday, & l_tTime);

		String l_strToLog;
		l_strToLog << (l_dtToday->tm_year + 1900) << CU_T( "-");
        l_strToLog << (l_dtToday->tm_mon + 1 < 10 ? CU_T( "0") : CU_T( "")) << (l_dtToday->tm_mon + 1) << CU_T( "-") << (l_dtToday->tm_mday < 10 ? CU_T( "0") : CU_T( "")) << l_dtToday->tm_mday;
		l_strToLog << CU_T( " - ") << (l_dtToday->tm_hour < 10 ? CU_T( "0") : CU_T( "")) << l_dtToday->tm_hour << CU_T( ":") << (l_dtToday->tm_min < 10 ? CU_T( "0") : CU_T( "")) << l_dtToday->tm_min << CU_T( ":") << (l_dtToday->tm_sec < 10 ? CU_T( "0") : CU_T( "")) << l_dtToday->tm_sec << CU_T( "s");
		l_strToLog << CU_T( " - ") << s_strHeaders[p_eLogType] << p_strToLog;
#ifdef _DEBUG
		Cout( p_strToLog.c_str() << std::endl);
#endif
		l_logFile << l_strToLog.c_str() << CU_T( "\n");
	}
}
