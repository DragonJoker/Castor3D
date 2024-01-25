#include "CastorUtils/Miscellaneous/DynamicLibrary.hpp"

#if defined( CU_PlatformAndroid )

#include <dlfcn.h>

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
			MbString name( toUtf8( m_pathLibrary ) );

			try
			{
				m_library = dlopen( name.c_str(), RTLD_LAZY );
			}
			catch ( ... )
			{
				Logger::logError( makeStringStream() << cuT( "Can't load dynamic library at [" ) << m_pathLibrary << cuT( "]" ) );
				m_library = nullptr;
				m_pathLibrary.clear();
			}
		}
	}

	VoidFnType DynamicLibrary::doGetFunction( String const & name )noexcept
	{
		void * result = nullptr;

		if ( m_library )
		{
			MbString stdname( toUtf8( name ) );

			try
			{
				dlerror();
				result = dlsym( m_library, stdname.c_str() );
				auto error = dlerror();

				if ( error != nullptr )
				{
					throw std::runtime_error( MbString( error ) );
				}
			}
			catch ( std::exception & exc )
			{
				result = nullptr;
				Logger::logError( makeStringStream() << cuT( "Can't load function [" ) << name << cuT( "]: " ) << exc.what() );
			}
			catch ( ... )
			{
				result = nullptr;
				Logger::logError( makeStringStream() << cuT( "Can't load function [" ) << name << cuT( "]: Unknown error." ) );
			}
		}
		else
		{
			Logger::logError( makeStringStream() << cuT( "Can't load function [" ) << name << cuT( "] because dynamic library is not loaded" ) );
		}

		return reinterpret_cast< VoidFnType >( result );
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
				Logger::logError( cuT( "Can't unload dynamic library" ) );
			}

			m_library = nullptr;
		}
	}
}

#endif
