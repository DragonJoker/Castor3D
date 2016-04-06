#include "Debug.hpp"

#include "StringUtils.hpp"

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
		inline void DoShowBacktrace( std::basic_ostream< CharT > & p_stream )
		{
		}

#else

		namespace
		{
			const int CALLS_TO_CAPTURE( 20 );
			const int CALLS_TO_SKIP( 2 );

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

				return string::string_cast< CharU >( l_ret );
			}

			template< typename CharT >
			inline void DoShowBacktrace( std::basic_ostream< CharT > & p_stream )
			{
				p_stream << "CALL STACK:" << std::endl;
				void * l_backTrace[CALLS_TO_CAPTURE];
				unsigned int l_num( ::backtrace( l_backTrace, CALLS_TO_CAPTURE ) );
				char ** l_fnStrings( ::backtrace_symbols( l_backTrace, l_num ) );

				for ( unsigned i = CALLS_TO_SKIP; i < l_num; ++i )
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
			inline void DoShowBacktrace( std::basic_ostream< CharT > & p_stream )
			{
				static bool SymbolsInitialised = false;
				const int MaxFnNameLen( 255 );

				void * l_backTrace[CALLS_TO_CAPTURE - CALLS_TO_SKIP];
				unsigned int l_num( ::RtlCaptureStackBackTrace( CALLS_TO_SKIP, CALLS_TO_CAPTURE - CALLS_TO_SKIP, l_backTrace, nullptr ) );

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
			DoShowBacktrace( p_stream );
			return p_stream;
		}

		std::ostream & operator<<( std::ostream & p_stream, Backtrace const & p_backtrace )
		{
			DoShowBacktrace( p_stream );
			return p_stream;
		}

	}
}
