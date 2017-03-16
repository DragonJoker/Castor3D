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

				if ( !m_pLibrary )
				{
					CASTOR_EXCEPTION( System::GetLastErrorText() );
				}

				m_pathLibrary = p_name;
			}
			catch ( std::exception & p_exc )
			{
				String l_strError = cuT( "Can't load dynamic library at [" ) + p_name + cuT( "]: " );
				l_strError += p_exc.what();
				Logger::LogError( l_strError );
				m_pLibrary = nullptr;
			}
			catch ( ... )
			{
				String l_strError = cuT( "Can't load dynamic library at [" ) + p_name + cuT( "]: " );
				l_strError += System::GetLastErrorText();
				Logger::LogError( l_strError );
				m_pLibrary = nullptr;
			}

			//::SetErrorMode( l_uiOldMode );
		}

		return m_pLibrary != nullptr;
	}

	void * DynamicLibrary::DoGetFunction( String const & p_name )throw()
	{
		void * l_result = nullptr;

		if ( m_pLibrary )
		{
			std::string l_name( string::string_cast< char >( p_name ) );
			UINT l_uiOldMode = ::SetErrorMode( SEM_FAILCRITICALERRORS );
			l_result = reinterpret_cast< void * >( ::GetProcAddress( static_cast< HMODULE >( m_pLibrary ), l_name.c_str() ) );
			::SetErrorMode( l_uiOldMode );
		}
		else
		{
			Logger::LogError( cuT( "Can't load function [" ) + p_name + cuT( "] because dynamic library is not loaded" ) );
		}

		return l_result;
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
