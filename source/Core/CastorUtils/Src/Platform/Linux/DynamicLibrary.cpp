#include "Miscellaneous/DynamicLibrary.hpp"

#if defined( CASTOR_PLATFORM_LINUX )

#include <dlfcn.h>

#include "Exception/Assertion.hpp"

namespace castor
{
	bool DynamicLibrary::open( Path const & p_name )throw()
	{
		if ( !m_pLibrary )
		{
			std::string name( string::stringCast< char >( p_name ) );

			try
			{
				m_pLibrary = dlopen( name.c_str(), RTLD_LAZY );
				m_pathLibrary = p_name;
			}
			catch ( ... )
			{
				Logger::logError( std::string( "Can't load dynamic library at [" ) + name + std::string( "]" ) );
				m_pLibrary = nullptr;
			}
		}

		return m_pLibrary != nullptr;
	}

	void * DynamicLibrary::doGetFunction( String const & p_name )throw()
	{
		void * result = nullptr;

		if ( m_pLibrary )
		{
			std::string name( string::stringCast< char >( p_name ) );

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
				Logger::logError( std::string( "Can't load function [" ) + name + std::string( "]: " ) + exc.what() );
			}
			catch ( ... )
			{
				result = nullptr;
				Logger::logError( std::string( "Can't load function [" ) + name + std::string( "]: Unknown error." ) );
			}
		}
		else
		{
			Logger::logError( cuT( "Can't load function [" ) + p_name + cuT( "] because dynamic library is not loaded" ) );
		}

		return result;
	}

	void DynamicLibrary::doClose()throw()
	{
		if ( m_pLibrary )
		{
			try
			{
				dlclose( m_pLibrary );
			}
			catch ( ... )
			{
				Logger::logError( std::string( "Can't unload dynamic library" ) );
			}

			m_pLibrary = nullptr;
		}
	}
}

#endif
