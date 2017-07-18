#include "Miscellaneous/DynamicLibrary.hpp"

#if defined( CASTOR_PLATFORM_ANDROID )

#include <dlfcn.h>

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

			try
			{
				m_pLibrary = dlopen( name.c_str(), RTLD_LAZY );
				m_pathLibrary = p_name;
			}
			catch ( ... )
			{
				Logger::LogError( std::string( "Can't load dynamic library at [" ) + name + std::string( "]" ) );
				m_pLibrary = nullptr;
			}
		}

		return m_pLibrary != nullptr;
	}

	void * DynamicLibrary::DoGetFunction( String const & p_name )throw()
	{
		void * result = nullptr;

		if ( m_pLibrary )
		{
			std::string name( string::string_cast< char >( p_name ) );

			try
			{
				dlerror();
				result = dlsym( m_pLibrary, name.c_str() );
				auto error = dlerror();

				if ( error != NULL )
				{
					throw std::runtime_error( std::string( error ) );
				}
			}
			catch ( std::exception & exc )
			{
				result = nullptr;
				Logger::LogError( std::string( "Can't load function [" ) + name + std::string( "]: " ) + exc.what() );
			}
			catch ( ... )
			{
				result = nullptr;
				Logger::LogError( std::string( "Can't load function [" ) + name + std::string( "]: Unknown error." ) );
			}
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
