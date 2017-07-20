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
			std::string name( string::string_cast< char >( p_name ) );
			//UINT uiOldMode = ::SetErrorMode( SEM_FAILCRITICALERRORS );

			try
			{
				m_pLibrary = ::LoadLibraryA( name.c_str() );

				if ( !m_pLibrary )
				{
					CASTOR_EXCEPTION( System::GetLastErrorText() );
				}

				m_pathLibrary = p_name;
			}
			catch ( std::exception & p_exc )
			{
				String strError = cuT( "Can't load dynamic library at [" ) + p_name + cuT( "]: " );
				strError += p_exc.what();
				Logger::LogError( strError );
				m_pLibrary = nullptr;
			}
			catch ( ... )
			{
				String strError = cuT( "Can't load dynamic library at [" ) + p_name + cuT( "]: " );
				strError += System::GetLastErrorText();
				Logger::LogError( strError );
				m_pLibrary = nullptr;
			}

			//::SetErrorMode( uiOldMode );
		}

		return m_pLibrary != nullptr;
	}

	void * DynamicLibrary::DoGetFunction( String const & p_name )throw()
	{
		void * result = nullptr;

		if ( m_pLibrary )
		{
			std::string name( string::string_cast< char >( p_name ) );
			UINT uiOldMode = ::SetErrorMode( SEM_FAILCRITICALERRORS );
			result = reinterpret_cast< void * >( ::GetProcAddress( static_cast< HMODULE >( m_pLibrary ), name.c_str() ) );
			::SetErrorMode( uiOldMode );
		}
		else
		{
			Logger::LogError( cuT( "Can't load function [" ) + p_name + cuT( "] because dynamic library is not loaded" ) );
		}

		return result;
	}

	void DynamicLibrary::DoClose()throw()
	{
		if ( m_pLibrary )
		{
			UINT uiOldMode = ::SetErrorMode( SEM_FAILCRITICALERRORS );
			::FreeLibrary( static_cast< HMODULE >( m_pLibrary ) );
			::SetErrorMode( uiOldMode );
			m_pLibrary = nullptr;
		}
	}
}

#endif
