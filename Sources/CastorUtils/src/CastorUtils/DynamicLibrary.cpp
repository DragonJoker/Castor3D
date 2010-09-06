#include "DynamicLibrary.h"

using namespace General::Utils;

void DynamicLibrary :: _close()
{
	if (m_library != NULL)
	{
#ifndef __GNUG__
		FreeLibrary( static_cast <HMODULE> ( m_library));
#else
		dlclose( m_library);
#endif
	}
}

bool DynamicLibrary :: Open( const Char * p_name)
{
	_close();
#ifndef __GNUG__
	m_library = LoadLibrary( p_name);
#else
	m_library = dlopen( p_name, RTLD_LAZY);
#endif
	return m_library != NULL;
}

bool DynamicLibrary :: Open( const String & p_name)
{
	return Open( p_name.c_str());
}

void * DynamicLibrary :: GetFunction( const Char * p_name)
{
	if (m_library != NULL)
	{
#ifndef __GNUG__
		return GetProcAddress( static_cast <HMODULE> ( m_library), String( p_name).char_str());
#else
		return dlsym( m_library, p_name);
#endif
	}

	return NULL;
}

void * DynamicLibrary :: GetFunction( const String & p_name)
{
	return GetFunction( p_name.c_str());
}