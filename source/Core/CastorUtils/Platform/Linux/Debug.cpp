#include "CastorUtils/Config/PlatformConfig.hpp"

#if defined( CU_PlatformLinux )

#include "CastorUtils/Config/MultiThreadConfig.hpp"
#include "CastorUtils/Miscellaneous/MiscellaneousModule.hpp"
#include "CastorUtils/Miscellaneous/StringUtils.hpp"

#if !defined( NDEBUG )
#	include <execinfo.h>
#	include <cxxabi.h>
#endif

namespace castor
{
	namespace Debug
	{
#if !defined( NDEBUG )

		template< typename CharU, typename CharT >
		inline std::basic_string< CharU > Demangle( std::basic_string< CharT > const & p_name )
		{
			std::string ret = string::stringCast< char >( p_name );
			size_t lindex = p_name.find( "(" );
			size_t rindex = p_name.find( "+" );

			if ( lindex != std::string::npos && rindex != std::string::npos )
			{
				ret = p_name.substr( lindex + 1, rindex - 1 - lindex );
			}

			int status;
			auto real = abi::__cxa_demangle( ret.c_str(), nullptr, nullptr, &status );

			if ( !status )
			{
				ret = p_name.substr( 0, lindex + 1 ) + real + p_name.substr( rindex );
			}
			else
			{
				ret = p_name;
			}

			free( real );
			return string::stringCast< CharU >( ret );
		}

		template< typename CharT >
		inline void doShowBacktrace( std::basic_ostream< CharT > & p_stream, int p_toCapture, int p_toSkip )
		{
			p_stream << "CALL STACK:" << std::endl;
			std::vector< void * > backTrace( p_toCapture );
			unsigned int num( ::backtrace( backTrace.data(), p_toCapture ) );
			char ** fnStrings( ::backtrace_symbols( backTrace.data(), num ) );

			for ( unsigned i = p_toSkip; i < num; ++i )
			{
				p_stream << "== " << Demangle< CharT >( string::stringCast< char >( fnStrings[i] ) ) << std::endl;
			}

			free( fnStrings );
		}

#else

		template< typename CharT >
		inline void doShowBacktrace( std::basic_ostream< CharT > & p_stream, int p_toCapture, int p_toSkip )
		{
		}

#endif

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

		std::wostream & operator<<( std::wostream & p_stream, Backtrace const & p_backtrace )
		{
			doShowBacktrace( p_stream, p_backtrace.m_toCapture, p_backtrace.m_toSkip );
			return p_stream;
		}

		std::ostream & operator<<( std::ostream & p_stream, Backtrace const & p_backtrace )
		{
			doShowBacktrace( p_stream, p_backtrace.m_toCapture, p_backtrace.m_toSkip );
			return p_stream;
		}
	}
}

#endif
