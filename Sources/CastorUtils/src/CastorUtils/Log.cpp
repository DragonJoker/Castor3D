#include "Log.h"

using namespace Castor3D;

String Log :: s_logFilePath;


Log :: Log( String p_logFilePath)
{
	s_logFilePath = p_logFilePath;
	FILE * l_logFile;
	fopen_s( & l_logFile, ToStdString( s_logFilePath).c_str(), "w");

	if (l_logFile == NULL) 
	{
		return;
	}

	fclose( l_logFile);
}



Log :: ~Log()
{
}



void Log :: LogMessage( const char * cFormat_p, ...)
{
	FILE * l_logFile;
	fopen_s( & l_logFile, ToStdString( s_logFilePath).c_str(), "a+");

	if (l_logFile == NULL) 
	{
		return;
	}

	char text_l[256];
	va_list ap_l;

	if (cFormat_p != NULL)		  
	{
		va_start( ap_l, cFormat_p);	
#ifdef WIN32
        vsnprintf_s( text_l, 256, 256, cFormat_p, ap_l);  
#else
        vsnprintf( text_l, 256, cFormat_p, ap_l);  
#endif
		va_end( ap_l);			 

		std::cout << text_l << "\n";
		fprintf( l_logFile, "%s\n", text_l);
	}

	fclose( l_logFile);
}



void Log :: LogMessage( const std::string & p_msg)
{
	FILE * l_logFile;
	fopen_s( & l_logFile, ToStdString( s_logFilePath).c_str(), "a+");

	if (l_logFile == NULL) 
	{
		return;
	}
	std::cout << p_msg.c_str() << "\n";
	fprintf( l_logFile, "%s\n", p_msg.c_str());

	fclose( l_logFile);
}



void Log :: LogMessage( const wchar_t * cFormat_p , ...)
{
	FILE * l_logFile;
	fopen_s( & l_logFile, ToStdString( s_logFilePath).c_str(), "a+");

	if (l_logFile == NULL) 
	{
		return;
	}

	char text_l[256];
	va_list ap_l;

	if (cFormat_p != NULL)		  
	{
		va_start( ap_l, cFormat_p);	
#ifdef WIN32
        vsnprintf_s( text_l, 256, 256, ConvertToString( cFormat_p).c_str(), ap_l);  
#else
        vsnprintf( text_l, 256, ConvertToString( cFormat_p).c_str(), ap_l);  
#endif
		va_end( ap_l);			 

		std::cout << text_l << "\n";
		fprintf( l_logFile, "%s\n", text_l);
	}

	fclose( l_logFile);
}



void Log :: LogMessage( const std::wstring & p_msg)
{
	FILE * l_logFile;
	fopen_s( & l_logFile, ToStdString( s_logFilePath).c_str(), "a+");

	if (l_logFile == NULL) 
	{
		return;
	}
	std::wcout << p_msg.c_str() << L"\n";
	fprintf( l_logFile, "%s\n", ConvertToString( p_msg).c_str());

	fclose( l_logFile);
}


