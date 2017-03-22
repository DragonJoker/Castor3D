#include "Miscellaneous/DynamicLibrary.hpp"

#if defined( CASTOR_PLATFORM_LINUX )

#include <dlfcn.h>

#include "Exception/Assertion.hpp"

namespace Castor
{
	bool DynamicLibrary::Open( Path const & p_name )throw()
	{
		if ( !m_pLibrary )
		{
			std::string l_name( string::string_cast< char >( p_name ) );

			try
			{
				m_pLibrary = dlopen( l_name.c_str(), RTLD_LAZY );
				m_pathLibrary = p_name;
			}
			catch ( ... )
			{
				Logger::LogError( std::string( "Can't load dynamic library at [" ) + l_name + std::string( "]" ) );
				m_pLibrary = nullptr;
			}
		}

		return m_pLibrary != nullptr;
	}

	void * DynamicLibrary::DoGetFunction( String const & p_name )throw()
	{
		void * l_result = nullptr;

		if ( m_pLibrary )
		{
			std::string l_name( string::string_cast< char >( p_name ) );

			try
			{
				dlerror();
				l_result = dlsym( m_pLibrary, l_name.c_str() );
				auto l_error = dlerror();

				if ( l_error != NULL )
				{
					throw std::runtime_error( std::string( l_error ) );
				}
			}
			catch ( std::exception & exc )
			{
				l_result = nullptr;
				Logger::LogError( std::string( "Can't load function [" ) + l_name + std::string( "]: " ) + exc.what() );
			}
			catch ( ... )
			{
				l_result = nullptr;
				Logger::LogError( std::string( "Can't load function [" ) + l_name + std::string( "]: Unknown error." ) );
			}
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
			try
			{
				dlclose( m_pLibrary );
			}
			catch ( ... )
			{
				Logger::LogError( std::string( "Can't unload dynamic library" ) );
			}

			m_pLibrary = nullptr;
		}
	}
}

#endif
