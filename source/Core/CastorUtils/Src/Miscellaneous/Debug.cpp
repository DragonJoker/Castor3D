#include "Debug.hpp"

#include "StringUtils.hpp"
#include "Config/MultiThreadConfig.hpp"

#if !defined( NDEBUG )
#	if !defined( _WIN32 )
#		include <execinfo.h>
#		include <stdlib.h>
#	else
#		include <Windows.h>
#		pragma warning( push )
#		pragma warning( disable:4091 ) //'typedef ': ignored on left of '' when no variable is declared
#		include <Dbghelp.h>
#		pragma warning( pop )
#	endif
#endif

#if !defined( _WIN32 )
#	include <signal.h>
#endif

#if defined( __GNUG__ )
#include <cxxabi.h>
#else
#endif

namespace Castor
{
	namespace Debug
	{
#if defined( NDEBUG )

		template< typename CharT >
		inline void DoShowBacktrace( std::basic_ostream< CharT > & p_stream, int, int )
		{
		}

#else

		namespace
		{
#	if defined( __GNUG__ )

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

#	elif defined( _WIN32 )

			template< typename CharU, typename CharT >
			inline std::basic_string< CharU > Demangle( std::basic_string< CharT > const & p_name )
			{
				std::string l_ret = string::string_cast< char >( p_name );

				try
				{
					char l_real[2048] = { 0 };

					if ( ::UnDecorateSymbolName( l_ret.c_str(), l_real, sizeof( l_real ), UNDNAME_COMPLETE ) )
					{
						l_ret = l_real;
					}
				}
				catch ( ... )
				{
					// What to do...
				}

				return string::string_cast< CharU >( l_ret );
			}

			template< typename CharT >
			inline void DoShowBacktrace( std::basic_ostream< CharT > & p_stream, int p_toCapture, int p_toSkip )
			{
				static std::mutex l_mutex;
				auto l_lock = make_unique_lock( l_mutex );
				static bool SymbolsInitialised = false;
				const int MaxFnNameLen( 255 );

				std::vector< void * > l_backTrace( p_toCapture - p_toSkip );
				unsigned int l_num( ::RtlCaptureStackBackTrace( p_toSkip, p_toCapture - p_toSkip, l_backTrace.data(), nullptr ) );

				::HANDLE l_process( ::GetCurrentProcess() );
				p_stream << "CALL STACK:" << std::endl;

				// symbol->Name type is char [1] so there is space for \0 already
				SYMBOL_INFO * l_symbol( ( SYMBOL_INFO * )malloc( sizeof( SYMBOL_INFO ) + ( MaxFnNameLen * sizeof( char ) ) ) );

				if ( l_symbol )
				{
					l_symbol->MaxNameLen = MaxFnNameLen;
					l_symbol->SizeOfStruct = sizeof( SYMBOL_INFO );

					if ( !SymbolsInitialised )
					{
						SymbolsInitialised = ::SymInitialize( l_process, nullptr, TRUE ) == TRUE;
					}

					if ( SymbolsInitialised )
					{
						for ( unsigned int i = 0; i < l_num; ++i )
						{
							if ( ::SymFromAddr( l_process, reinterpret_cast< DWORD64 >( l_backTrace[i] ), 0, l_symbol ) )
							{
								p_stream << "== " << Demangle< CharT >( string::string_cast< char >( l_symbol->Name, l_symbol->Name + l_symbol->NameLen ) ) << std::endl;
							}
						}
					}
					else
					{
						p_stream << "== Unable to retrieve the call stack" << std::endl;
					}

					free( l_symbol );
				}
			}

#	endif

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
