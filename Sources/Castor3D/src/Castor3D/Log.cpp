#include "PrecompiledHeader.h"

#include "Log.h"

#ifdef _WIN32
#	include <Windows.h>
#endif

using namespace Castor3D;

String Log :: s_logFilePath;
Log Log :: s_singleton;

Log :: Log()
{
	s_singleton = * this;
#ifndef __GNUG__
#	ifdef _DEBUG
	if (AllocConsole() == TRUE)
	{
		FILE * l_dump;
		freopen_s( & l_dump, "conout$", "w", stdout); 
		freopen_s( & l_dump, "conout$", "w", stderr);
	}
#	endif
#endif
}

Log :: ~Log()
{
#ifndef __GNUG__
#	ifdef _DEBUG
	FreeConsole();
#	endif
#endif
}

void Log :: SetFileName( const String & p_logFilePath)
{
	s_logFilePath = p_logFilePath;
	FILE * l_pFile;
	fopen_s( & l_pFile, p_logFilePath.char_str(), "w");

	if (l_pFile != NULL)
	{
		fclose( l_pFile);
	}
}

void Log :: LogMessage( const char * p_pFormat, ...)
{
	char l_pText[256];
	va_list l_vaList;

	if (p_pFormat != NULL)
	{
		va_start( l_vaList, p_pFormat);	
#ifdef WIN32
		vsnprintf_s( l_pText, 256, 256, p_pFormat, l_vaList);  
#else
		vsnprintf( l_pText, 256, cFormat_p, ap_l);  
#endif
		va_end( l_vaList);
		_logMessage( l_pText);
	}
}

void Log :: LogMessage( const std::string & p_msg)
{
	_logMessage( p_msg);
}

void Log :: LogMessage( const wchar_t * p_pFormat , ...)
{
	char l_pText[256];
	va_list l_vaList;

	if (p_pFormat != NULL)		  
	{
		va_start( l_vaList, p_pFormat);	
		String l_strFormat( p_pFormat);
#ifdef WIN32
		vsnprintf_s( l_pText, 256, 256, l_strFormat.char_str(), l_vaList);  
#else
		vsnprintf( l_pText, 256, l_strFormat.char_str(), ap_l);  
#endif
		va_end( l_vaList);
		_logMessage( l_pText);
	}
}

void Log :: LogMessage( const std::wstring & p_msg)
{
	_logMessage( p_msg);
}

void Log :: _logMessage( const String & p_strToLog)
{
	File l_logFile( s_logFilePath, File::eAdd);

	if (l_logFile.IsOk()) 
	{
#ifdef _DEBUG
		std::cout << p_strToLog.c_str() << std::endl;
#endif
		l_logFile << p_strToLog.c_str() << "\n";
//		l_logFile.WriteArray<Char>( (p_strToLog + "\n").c_str(), p_strToLog.size() + 1);
	}
}