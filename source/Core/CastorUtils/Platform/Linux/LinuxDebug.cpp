#include "CastorUtils/Config/PlatformConfig.hpp"

#if defined( CU_PlatformLinux )

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

		template< typename CharU, typename CharT >
		static std::basic_string< CharU > demangleSymbol( std::basic_string< CharT > const & name )
		{
			std::string ret = string::stringCast< char >( name );
			size_t lindex = name.find( "(" );
			size_t rindex = name.find( "+" );

			if ( lindex != std::string::npos && rindex != std::string::npos )
			{
				ret = name.substr( lindex + 1, rindex - 1 - lindex );
			}

			int status;
			auto real = abi::__cxa_demangle( ret.c_str(), nullptr, nullptr, &status );

			if ( !status )
			{
				ret = name.substr( 0, lindex + 1 ) + real + name.substr( rindex );
			}
			else
			{
				ret = name;
			}

			free( real );
			return string::stringCast< CharU >( ret );
		}

		template< typename CharT >
		static void showBacktrace( std::basic_ostream< CharT > & stream, int toCapture, int toSkip )
		{
			stream << "CALL STACK:" << std::endl;
			std::vector< void * > backTrace( toCapture );
			unsigned int num( ::backtrace( backTrace.data(), toCapture ) );
			char ** fnStrings( ::backtrace_symbols( backTrace.data(), num ) );

			for ( unsigned i = toSkip; i < num; ++i )
			{
				stream << "== " << demangleSymbol< CharT >( string::stringCast< char >( fnStrings[i] ) ) << std::endl;
			}

			free( fnStrings );
		}

#else

		template< typename CharT >
		static void showBacktrace( std::basic_ostream< CharT > & stream, int toCapture, int toSkip )
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

	std::wostream & operator<<( std::wostream & stream, Backtrace const & bt )
	{
		backtrace::showBacktrace( stream, bt.m_toCapture, bt.m_toSkip );
		return stream;
	}

	std::ostream & operator<<( std::ostream & stream, Backtrace const & bt )
	{
		backtrace::showBacktrace( stream, bt.m_toCapture, bt.m_toSkip );
		return stream;
	}
}

#endif
