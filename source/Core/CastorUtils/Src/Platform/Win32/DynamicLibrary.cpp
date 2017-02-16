#include "Miscellaneous/DynamicLibrary.hpp"

#if defined( CASTOR_PLATFORM_WINDOWS )

#include <windows.h>

#include "Exception/Assertion.hpp"
#include "Log/Logger.hpp"
#include "Miscellaneous/Utils.hpp"

namespace Castor
{
	bool DynamicLibrary::Open( Path const & p_name )throw()
	{
		if ( !m_pLibrary )
		{
			std::string l_name( string::string_cast< char >( p_name ) );
			//UINT l_uiOldMode = ::SetErrorMode( SEM_FAILCRITICALERRORS );

			try
			{
				m_pLibrary = ::LoadLibraryA( l_name.c_str() );
				m_pathLibrary = p_name;
			}
			catch ( ... )
			{
				Logger::LogError( std::string( "Can't load dynamic library at [" ) + l_name + std::string( "]" ) );
				m_pLibrary = nullptr;
			}

			if ( !m_pLibrary )
			{
				String l_strError = cuT( "Can't load dynamic library at [" ) + p_name + cuT( "]: " );
				l_strError += System::GetLastErrorText();
				Logger::LogError( l_strError );
			}

			//::SetErrorMode( l_uiOldMode );
		}

		return m_pLibrary != nullptr;
	}

	void * DynamicLibrary::DoGetFunction( String const & p_name )throw()
	{
		void * l_return = nullptr;

		if ( m_pLibrary )
		{
			std::string l_name( string::string_cast< char >( p_name ) );
			UINT l_uiOldMode = ::SetErrorMode( SEM_FAILCRITICALERRORS );
			l_return = reinterpret_cast< void * >( ::GetProcAddress( static_cast< HMODULE >( m_pLibrary ), l_name.c_str() ) );
			::SetErrorMode( l_uiOldMode );
		}
		else
		{
			Logger::LogError( cuT( "Can't load function [" ) + p_name + cuT( "] because dynamic library is not loaded" ) );
		}

		return l_return;
	}

	void DynamicLibrary::DoClose()throw()
	{
		if ( m_pLibrary )
		{
			UINT l_uiOldMode = ::SetErrorMode( SEM_FAILCRITICALERRORS );
			::FreeLibrary( static_cast< HMODULE >( m_pLibrary ) );
			::SetErrorMode( l_uiOldMode );
			m_pLibrary = nullptr;
		}
	}
}

#endif
