#include "CastorUtils/Config/PlatformConfig.hpp"

#if defined( CU_PlatformApple )

#include "CastorUtils/Config/MultiThreadConfig.hpp"
#include "CastorUtils/Miscellaneous/MiscellaneousModule.hpp"
#include "CastorUtils/Miscellaneous/StringUtils.hpp"

#if !defined( NDEBUG )
#	include <execinfo.h>
#	include <cxxabi.h>
#endif

namespace castor::debug
{
	namespace backtrace
	{
#if !defined( NDEBUG )

		static MbString demangleSymbol( MbString const & name )
		{
			MbString ret = toUtf8( name );
			size_t lindex = ret.find( "(" );
			size_t rindex = ret.find( "+" );

			if ( lindex != MbString::npos && rindex != MbString::npos )
			{
				ret = ret.substr( lindex + 1, rindex - 1 - lindex );
			}

			int status;
			auto real = abi::__cxa_demangle( ret.c_str(), nullptr, nullptr, &status );

			if ( !status )
			{
				ret = ret.substr( 0, lindex + 1 ) + real + name.substr( rindex );
			}
			else
			{
				ret = toUtf8( name );
			}

			free( real );
			return ret;
		}

		static void showBacktrace( OutputStream & stream, int toCapture, int toSkip )
		{
			stream << cuT( "CALL STACK:" ) << std::endl;
			Vector< void * > backTrace( toCapture );
			unsigned int num( ::backtrace( backTrace.data(), toCapture ) );
			char ** fnStrings( ::backtrace_symbols( backTrace.data(), num ) );

			for ( unsigned i = toSkip; i < num; ++i )
			{
				stream << "== " << makeString( demangleSymbol( fnStrings[i] ) ) << std::endl;
			}

			free( fnStrings );
		}

#else

		static void showBacktrace( OutputStream & stream, int toCapture, int toSkip )
		{
		}

#endif
	}

	void initialise()
	{
	}

	void cleanup()
	{
	}

	void loadModule( DynamicLibrary const & library )
	{
	}

	void unloadModule( DynamicLibrary const & library )
	{
	}

	OutputStream & operator<<( OutputStream & stream, Backtrace const & bt )
	{
		backtrace::showBacktrace( stream, bt.m_toCapture, bt.m_toSkip );
		return stream;
	}
}

#endif
