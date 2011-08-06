#include "CastorUtils/PrecompiledHeader.hpp"

#include "CastorUtils/DynamicLibrary.hpp"

#if CHECK_MEMORYLEAKS
#	include "CastorUtils/Memory.hpp"
using namespace Castor::Utils;
#endif

#ifdef _WIN32
#	include <windows.h>
#else
#	include <dlfcn.h>
#endif

using namespace Castor;
using namespace Castor::Utils;

void DynamicLibrary :: _close()throw()
{
	if (m_library)
	{
#ifdef _WIN32
		UINT l_uiOldMode = SetErrorMode( SEM_FAILCRITICALERRORS);
#endif

		try
		{
#ifdef _WIN32
			FreeLibrary( static_cast <HMODULE> ( m_library));
#else
			dlclose( m_library);
#endif
		}
		catch ( ... )
		{
		}

#ifdef _WIN32
		SetErrorMode( l_uiOldMode);
#endif

		m_library = NULL;
	}
}

bool DynamicLibrary :: Open( xchar const * p_name)throw()
{
	return Open( String( p_name));
}

bool DynamicLibrary :: Open( String const & p_name)throw()
{
	_close();

#ifdef _WIN32
	UINT l_uiOldMode = SetErrorMode( SEM_FAILCRITICALERRORS);
#endif

	try
	{
#ifdef _WIN32
		m_library = LoadLibrary( p_name.c_str());
#else
		m_library = dlopen( p_name.char_str(), RTLD_LAZY);
#endif
		m_pathLibrary = p_name;
	}
	catch ( ... )
	{
		m_library = NULL;
	}

#ifdef _WIN32
	SetErrorMode( l_uiOldMode);
#endif

	return m_library != NULL;
}

void * DynamicLibrary :: GetFunction( xchar const * p_name)throw()
{
	return GetFunction( String( p_name));
}

void * DynamicLibrary :: GetFunction( String const & p_name)throw()
{
	REQUIRE( m_library);
	void * l_pReturn = NULL;
	std::string l_str;
	l_str = p_name.char_str();

#ifdef _WIN32
	UINT l_uiOldMode = SetErrorMode( SEM_FAILCRITICALERRORS);
#endif

	try
	{
#ifdef _WIN32
		l_pReturn = (void *)( GetProcAddress( static_cast<HMODULE>( m_library), l_str.c_str()));
#else
		l_pReturn = dlsym( m_library, l_str.c_str());
#endif
	}
	catch ( ... )
	{
		l_pReturn = NULL;
	}

#ifdef _WIN32
	SetErrorMode( l_uiOldMode);
#endif

	return l_pReturn;
}
