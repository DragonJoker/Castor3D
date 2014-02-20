#include "CastorUtils/PrecompiledHeader.hpp"

#include "CastorUtils/DynamicLibrary.hpp"
#include "CastorUtils/Assertion.hpp"
#include "CastorUtils/Logger.hpp"

#if defined( _WIN32 )
#	if defined( _MSC_VER )
#		pragma warning( push )
#		pragma warning( disable:4311 )
#		pragma warning( disable:4312 )
#	endif
#	include <windows.h>
#	if defined( _MSC_VER )
#		pragma warning( pop )
#	endif
#elif defined( __linux__ )
#	include <dlfcn.h>
#endif

using namespace Castor;

DynamicLibrary :: DynamicLibrary()throw()
	:	m_pLibrary		( NULL	)
	,	m_pathLibrary	(		)
{
}

DynamicLibrary :: DynamicLibrary( DynamicLibrary const & p_lib )throw()
	:	m_pLibrary		( NULL	)
	,	m_pathLibrary	(		)
{
	if( p_lib.m_pLibrary )
	{
		Open( p_lib.m_pathLibrary );
	}
}

DynamicLibrary :: DynamicLibrary( DynamicLibrary && p_lib )throw()
	:	m_pLibrary		( std::move( p_lib.m_pLibrary )		)
	,	m_pathLibrary	( std::move( p_lib.m_pathLibrary )	)
{
	p_lib.m_pLibrary = NULL;
	p_lib.m_pathLibrary.clear();
}

DynamicLibrary :: ~DynamicLibrary()throw()
{
	DoClose();
}

DynamicLibrary & DynamicLibrary :: operator =( DynamicLibrary const & p_lib)
{
	DoClose();

	if( p_lib.m_pLibrary )
	{
		Open( p_lib.m_pathLibrary );
	}

	return *this;
}

DynamicLibrary & DynamicLibrary :: operator =( DynamicLibrary && p_lib)
{
	if( this != &p_lib )
	{
		m_pLibrary		= std::move( p_lib.m_pLibrary );
		m_pathLibrary	= std::move( p_lib.m_pathLibrary );
		p_lib.m_pLibrary = NULL;
		p_lib.m_pathLibrary.clear();
	}

	return *this;
}

bool DynamicLibrary :: Open( xchar const * p_name )throw()
{
	return Open( Path( p_name ) );
}

bool DynamicLibrary :: Open( String const & p_name )throw()
{
	return Open( Path( p_name ) );
}

bool DynamicLibrary :: Open( Path const & p_name )throw()
{
	if( !m_pLibrary )
	{
		std::string l_strName( str_utils::to_str( p_name ) );

#if defined( _WIN32 )
		UINT l_uiOldMode = ::SetErrorMode( SEM_FAILCRITICALERRORS );
	
		try
		{
			m_pLibrary = ::LoadLibraryA( l_strName.c_str() );
			m_pathLibrary = p_name;
		}
		catch ( ... )
		{
			Logger::LogError( std::string( "Can't load dynamic library at [" ) + l_strName + std::string( "]" ), false );
			m_pLibrary = NULL;
		}
	
		::SetErrorMode( l_uiOldMode );
#else
		try
		{
			m_pLibrary = dlopen( l_strName.c_str(), RTLD_LAZY );
			m_pathLibrary = p_name;
		}
		catch ( ... )
		{
			Logger::LogError( std::string( "Can't load dynamic library at [" ) + l_strName + std::string( "]" ), false );
			m_pLibrary = NULL;
		}
#endif
	}

	return m_pLibrary != NULL;
}

void * DynamicLibrary :: DoGetFunction( String const & p_name )throw()
{
	void * l_pReturn = NULL;
	REQUIRE( m_pLibrary );

	if( m_pLibrary )
	{
		std::string l_strName( str_utils::to_str( p_name ) );
	
#if defined( _WIN32 )
		UINT l_uiOldMode = ::SetErrorMode( SEM_FAILCRITICALERRORS );
		l_pReturn = reinterpret_cast< void * >( ::GetProcAddress( static_cast< HMODULE >( m_pLibrary ), l_strName.c_str() ) );
		::SetErrorMode( l_uiOldMode );
#else
		try
		{
			l_pReturn = dlsym( m_pLibrary, l_strName.c_str() );
		}
		catch ( ... )
		{
			l_pReturn = NULL;
			Logger::LogError( std::string( "Can't load function [" ) + l_strName + std::string( "]" ), false );
		}
#endif
	}
	else
	{
		Logger::LogError( cuT( "Can't load function [" ) + p_name + cuT( "] because dynamic library is not loaded" ), false );
	}

	return l_pReturn;
}

void DynamicLibrary :: DoClose()throw()
{
	if( m_pLibrary )
	{
#if defined( _WIN32 )
		UINT l_uiOldMode = ::SetErrorMode( SEM_FAILCRITICALERRORS );
		::FreeLibrary( static_cast< HMODULE >( m_pLibrary ) );
		::SetErrorMode( l_uiOldMode );
#else
		try
		{
			dlclose( m_pLibrary );
		}
		catch( ... )
		{
			Logger::LogError( std::string( "Can't unload dynamic library" ), false );
		}
#endif

		m_pLibrary = NULL;
	}
}