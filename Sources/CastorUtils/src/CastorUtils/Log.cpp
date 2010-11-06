#include "PrecompiledHeader.h"

#include "Log.h"

#ifdef _WIN32
#	include <Windows.h>
#endif

#if CHECK_MEMORYLEAKS
#	include "Memory.h"
using namespace Castor::Utils;
#endif

using namespace Castor;
using namespace Castor::Utils;

String Log :: s_logFilePath;

Log :: Log( const String & p_logFilePath)
{
	s_logFilePath = p_logFilePath;
	FILE * l_logFile;
	fopen_s( & l_logFile, s_logFilePath.char_str(), "w");

	if (l_logFile == NULL) 
	{
		return;
	}

	fclose( l_logFile);

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

void Log :: LogMessage( const char * p_pFormat, ...)
{
	FILE * l_logFile;
	fopen_s( & l_logFile, s_logFilePath.char_str(), "a+");

	if (l_logFile == NULL) 
	{
		return;
	}

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

#ifdef _DEBUG
		std::cout << l_pText << "\n";
#endif
		fprintf( l_logFile, "%s\n", l_pText);
	}

	fclose( l_logFile);
}

void Log :: LogMessage( const std::string & p_msg)
{
	FILE * l_logFile;
	fopen_s( & l_logFile, s_logFilePath.char_str(), "a+");

	if (l_logFile == NULL) 
	{
		return;
	}
#ifdef _DEBUG
	std::cout << p_msg.c_str() << "\n";
#endif
	fprintf( l_logFile, "%s\n", p_msg.c_str());

	fclose( l_logFile);
}

void Log :: LogMessage( const wchar_t * p_pFormat , ...)
{
	FILE * l_logFile;
	fopen_s( & l_logFile, s_logFilePath.char_str(), "a+");

	if (l_logFile == NULL) 
	{
		return;
	}

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

#ifdef _DEBUG
		std::cout << l_pText << "\n";
#endif
		fprintf( l_logFile, "%s\n", l_pText);
	}

	fclose( l_logFile);
}

void Log :: LogMessage( const std::wstring & p_msg)
{
	FILE * l_logFile;
	fopen_s( & l_logFile, s_logFilePath.char_str(), "a+");

	if (l_logFile == NULL) 
	{
		return;
	}
	
	String l_strMsg( p_msg);

#ifdef _DEBUG
	std::wcout << p_msg.c_str() << L"\n";
#endif
	fprintf( l_logFile, "%s\n", l_strMsg.char_str());

	fclose( l_logFile);
}