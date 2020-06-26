#include "CastorUtils/Miscellaneous/DynamicLibrary.hpp"

#if defined( CU_PlatformWindows )

#include <windows.h>

#include "CastorUtils/Exception/Assertion.hpp"
#include "CastorUtils/Exception/Exception.hpp"
#include "CastorUtils/Log/Logger.hpp"
#include "CastorUtils/Miscellaneous/Utils.hpp"

namespace castor
{
	void DynamicLibrary::doOpen()noexcept
	{
		if ( !m_library )
		{
			std::string name( string::stringCast< char >( m_pathLibrary ) );
			//UINT oldMode = ::SetErrorMode( SEM_FAILCRITICALERRORS );

			try
			{
				m_library = ::LoadLibraryA( name.c_str() );

				if ( !m_library )
				{
					String strError = cuT( "Can't load dynamic library at [" ) + m_pathLibrary + cuT( "]: " );
					strError += System::getLastErrorText();
					Logger::logError( strError );
					m_pathLibrary.clear();
				}
			}
			catch ( std::exception & p_exc )
			{
				String strError = cuT( "Can't load dynamic library at [" ) + m_pathLibrary + cuT( "]: " );
				strError += p_exc.what();
				Logger::logError( strError );
				m_library = nullptr;
				m_pathLibrary.clear();
			}
			catch ( ... )
			{
				String strError = cuT( "Can't load dynamic library at [" ) + m_pathLibrary + cuT( "]: " );
				strError += System::getLastErrorText();
				Logger::logError( strError );
				m_library = nullptr;
				m_pathLibrary.clear();
			}

			//::SetErrorMode( oldMode );
		}
	}

	void * DynamicLibrary::doGetFunction( String const & name )noexcept
	{
		void * result = nullptr;

		if ( m_library )
		{
			std::string stdname( string::stringCast< char >( name ) );
			UINT oldMode = ::SetErrorMode( SEM_FAILCRITICALERRORS );
			result = reinterpret_cast< void * >( ::GetProcAddress( static_cast< HMODULE >( m_library ), stdname.c_str() ) );
			::SetErrorMode( oldMode );
		}
		else
		{
			Logger::logError( cuT( "Can't load function [" ) + name + cuT( "] because dynamic library is not loaded" ) );
		}

		return result;
	}

	void DynamicLibrary::doClose()noexcept
	{
		if ( m_library )
		{
			UINT oldMode = ::SetErrorMode( SEM_FAILCRITICALERRORS );
			::FreeLibrary( static_cast< HMODULE >( m_library ) );
			::SetErrorMode( oldMode );
			m_library = nullptr;
		}
	}
}

#endif
