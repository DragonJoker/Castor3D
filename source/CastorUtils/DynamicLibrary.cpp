#include "CastorUtils/PrecompiledHeader.h"

#include "CastorUtils/DynamicLibrary.h"

#if CHECK_MEMORYLEAKS
#	include "CastorUtils/Memory.h"
using namespace Castor::Utils;
#endif

using namespace Castor;
using namespace Castor::Utils;

void DynamicLibrary :: _close()throw()
{
	if (m_library != NULL)
	{
		try
		{
#ifndef __GNUG__
			UINT l_uiOldMode = SetErrorMode( SEM_FAILCRITICALERRORS);
			FreeLibrary( static_cast <HMODULE> ( m_library));
			SetErrorMode( l_uiOldMode);
#else
			dlclose( m_library);
#endif
		}
		catch ( ... )
		{
		}
	}
}

bool DynamicLibrary :: Open( const Char * p_name)throw()
{
	_close();

	try
	{
#ifndef __GNUG__
		UINT l_uiOldMode = SetErrorMode( SEM_FAILCRITICALERRORS);
//		m_library = LoadLibraryEx( p_name, NULL, DONT_RESOLVE_DLL_REFERENCES | LOAD_IGNORE_CODE_AUTHZ_LEVEL);
		m_library = LoadLibrary( p_name);
		SetErrorMode( l_uiOldMode);
#else
		m_library = dlopen( p_name, RTLD_LAZY);
#endif
	}
	catch ( ... )
	{
	}

	return m_library != NULL;
}

bool DynamicLibrary :: Open( const String & p_name)throw()
{
	return Open( p_name.c_str());
}

void * DynamicLibrary :: GetFunction( const Char * p_name)throw()
{
	void * l_pReturn = NULL;

	if (m_library != NULL)
	{
		try
		{
#ifndef __GNUG__
			UINT l_uiOldMode = SetErrorMode( SEM_FAILCRITICALERRORS);
			std::string l_str;
			l_str.assign( String( p_name).char_str());
			l_pReturn = GetProcAddress( static_cast <HMODULE> ( m_library), l_str.c_str());
			SetErrorMode( l_uiOldMode);
#else
			l_pReturn = dlsym( m_library, p_name);
#endif
		}
		catch ( ... )
		{
		}
	}

	return l_pReturn;
}

void * DynamicLibrary :: GetFunction( const String & p_name)throw()
{
	return GetFunction( p_name.c_str());
}