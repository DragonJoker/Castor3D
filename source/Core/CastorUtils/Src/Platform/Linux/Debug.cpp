#include "Config/PlatformConfig.hpp"

#if defined( CASTOR_PLATFORM_LINUX )

#include "Miscellaneous/Debug.hpp"

#include "Miscellaneous/StringUtils.hpp"
#include "Config/MultiThreadConfig.hpp"

#if !defined( NDEBUG )
#	include <execinfo.h>
#	include <cxxabi.h>
#endif


namespace Castor
{
	namespace Debug
	{
#if !defined( NDEBUG )

		template< typename CharU, typename CharT >
		inline std::basic_string< CharU > Demangle( std::basic_string< CharT > const & p_name )
		{
			std::string l_ret = string::string_cast< char >( p_name );
			size_t l_lindex = p_name.find( "(" );
			size_t l_rindex = p_name.find( "+" );

			if ( l_lindex != std::string::npos && l_rindex != std::string::npos )
			{
				l_ret = p_name.substr( l_lindex + 1, l_rindex - 1 - l_lindex );
			}

			int l_status;
			auto l_real = abi::__cxa_demangle( l_ret.c_str(), 0, 0, &l_status );

			if ( !l_status )
			{
				l_ret = p_name.substr( 0, l_lindex + 1 ) + l_real + p_name.substr( l_rindex );
			}
			else
			{
				l_ret = p_name;
			}

			free( l_real );
			return string::string_cast< CharU >( l_ret );
		}

		template< typename CharT >
		inline void DoShowBacktrace( std::basic_ostream< CharT > & p_stream, int p_toCapture, int p_toSkip )
		{
			p_stream << "CALL STACK:" << std::endl;
			std::vector< void * > l_backTrace( p_toCapture );
			unsigned int l_num( ::backtrace( l_backTrace.data(), p_toCapture ) );
			char ** l_fnStrings( ::backtrace_symbols( l_backTrace.data(), l_num ) );

			for ( unsigned i = p_toSkip; i < l_num; ++i )
			{
				p_stream << "== " << Demangle< CharT >( string::string_cast< char >( l_fnStrings[i] ) ) << std::endl;
			}

			free( l_fnStrings );
		}

#else

		template< typename CharT >
		inline void DoShowBacktrace( std::basic_ostream< CharT > & p_stream, int p_toCapture, int p_toSkip )
		{
		}

#endif

		std::wostream & operator<<( std::wostream & p_stream, Backtrace const & p_backtrace )
		{
			DoShowBacktrace( p_stream, p_backtrace.m_toCapture, p_backtrace.m_toSkip );
			return p_stream;
		}

		std::ostream & operator<<( std::ostream & p_stream, Backtrace const & p_backtrace )
		{
			DoShowBacktrace( p_stream, p_backtrace.m_toCapture, p_backtrace.m_toSkip );
			return p_stream;
		}
	}
}

#endif
