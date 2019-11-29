#include "CastorUtils/Miscellaneous/DynamicLibrary.hpp"

#if defined( CU_PlatformApple )

#include <dlfcn.h>

#include "CastorUtils/Exception/Assertion.hpp"

namespace castor
{
	void DynamicLibrary::doOpen()noexcept
	{
		if ( !m_library )
		{
			std::string name( string::stringCast< char >( m_pathLibrary ) );

			try
			{
				m_library = dlopen( name.c_str(), RTLD_LAZY );
			}
			catch ( ... )
			{
				Logger::logError( std::string( "Can't load dynamic library at [" ) + m_pathLibrary + std::string( "]" ) );
				m_library = nullptr;
				m_pathLibrary.clear();
			}
		}
	}

	void * DynamicLibrary::doGetFunction( String const & name )noexcept
	{
		void * result = nullptr;

		if ( m_library )
		{
			std::string stdname( string::stringCast< char >( name ) );

			try
			{
				dlerror();
				result = dlsym( m_library, stdname.c_str() );
				auto error = dlerror();

				if ( error != NULL )
				{
					throw std::runtime_error( std::string( error ) );
				}
			}
			catch ( std::exception & exc )
			{
				result = nullptr;
				Logger::logError( std::string( "Can't load function [" ) + stdname + std::string( "]: " ) + exc.what() );
			}
			catch ( ... )
			{
				result = nullptr;
				Logger::logError( std::string( "Can't load function [" ) + stdname + std::string( "]: Unknown error." ) );
			}
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
			try
			{
				dlclose( m_library );
			}
			catch ( ... )
			{
				Logger::logError( std::string( "Can't unload dynamic library" ) );
			}

			m_library = nullptr;
		}
	}
}

#endif
