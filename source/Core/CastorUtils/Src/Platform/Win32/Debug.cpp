#include "Config/PlatformConfig.hpp"

#if defined( CASTOR_PLATFORM_WINDOWS )

#include "Miscellaneous/Debug.hpp"

#include "Miscellaneous/StringUtils.hpp"
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
#if !defined( NDEBUG )

		namespace
		{
			template< typename CharU, typename CharT >
			inline std::basic_string< CharU > Demangle( std::basic_string< CharT > const & p_name )
			{
				std::string ret = string::string_cast< char >( p_name );

				try
				{
					char real[2048] = { 0 };

					if ( ::UnDecorateSymbolName( ret.c_str(), real, sizeof( real ), UNDNAME_COMPLETE ) )
					{
						ret = real;
					}
				}
				catch ( ... )
				{
					// What to do...
				}

				return string::string_cast< CharU >( ret );
			}

			template< typename CharT >
			inline void DoShowBacktrace( std::basic_ostream< CharT > & p_stream, int p_toCapture, int p_toSkip )
			{
				static std::mutex mutex;
				auto lock = make_unique_lock( mutex );
				static bool SymbolsInitialised = false;
				const int MaxFnNameLen( 255 );

				std::vector< void * > backTrace( p_toCapture - p_toSkip );
				unsigned int num( ::RtlCaptureStackBackTrace( p_toSkip, p_toCapture - p_toSkip, backTrace.data(), nullptr ) );

				::HANDLE process( ::GetCurrentProcess() );
				p_stream << "CALL STACK:" << std::endl;

				// symbol->Name type is char [1] so there is space for \0 already
				SYMBOL_INFO * symbol( ( SYMBOL_INFO * )malloc( sizeof( SYMBOL_INFO ) + ( MaxFnNameLen * sizeof( char ) ) ) );

				if ( symbol )
				{
					symbol->MaxNameLen = MaxFnNameLen;
					symbol->SizeOfStruct = sizeof( SYMBOL_INFO );

					if ( !SymbolsInitialised )
					{
						SymbolsInitialised = ::SymInitialize( process, nullptr, TRUE ) == TRUE;
					}

					if ( SymbolsInitialised )
					{
						for ( unsigned int i = 0; i < num; ++i )
						{
							if ( ::SymFromAddr( process, reinterpret_cast< DWORD64 >( backTrace[i] ), 0, symbol ) )
							{
								p_stream << "== " << Demangle< CharT >( string::string_cast< char >( symbol->Name, symbol->Name + symbol->NameLen ) ) << std::endl;
							}
						}
					}
					else
					{
						p_stream << "== Unable to retrieve the call stack" << std::endl;
					}

					free( symbol );
				}
			}
		}

#else

		template< typename CharT >
		inline void DoShowBacktrace( std::basic_ostream< CharT > & p_stream, int, int )
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
