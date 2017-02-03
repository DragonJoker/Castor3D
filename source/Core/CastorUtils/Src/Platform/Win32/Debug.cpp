#include "Config/PlatformConfig.hpp"

#if defined( CASTOR_PLATFORM_WINDOWS )

#include "Miscellaneous/Debug.hpp"

#include "StringUtils.hpp"
#include "Config/MultiThreadConfig.hpp"

#if !defined( NDEBUG )
#	include <Windows.h>
#	pragma warning( push )
#	pragma warning( disable:4091 ) //'typedef ': ignored on left of '' when no variable is declared
#	include <Dbghelp.h>
#	pragma warning( pop )
#endif

namespace Castor
{
	namespace Debug
	{
		namespace
		{
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
		}

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
