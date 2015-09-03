#include "Debug.hpp"

#include "StringUtils.hpp"

#if !defined( NDEBUG )
#	if !defined( _WIN32 )
#		include <execinfo.h>
#		include <stdlib.h>
#	else
#		include <Windows.h>
#		include <Dbghelp.h>
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

		static void ShowBacktrace( StringStream & p_stream )
		{
		}

#else
		namespace
		{
			const int NumOfFnCallsToCapture( 20 );
			const int NumOfFnCallsToSkip( 2 );

#	if defined( __GNUG__ )

			String Demangle( const std::string & p_name )
			{
				std::string l_ret( p_name );
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

				return str_utils::to_string( l_ret );
			}

#	elif defined( _MSC_VER )

			String Demangle( const std::string & p_name )
			{
				char l_real[1024] = { 0 };
				std::string l_ret;

				if ( UnDecorateSymbolName( p_name.c_str(), l_real, sizeof( l_real ), UNDNAME_COMPLETE ) )
				{
					l_ret = l_real;
				}
				else
				{
					l_ret = p_name;
				}

				return str_utils::string_cast< xchar >( l_ret );
			}

#	else

			String Demangle( const std::string & p_name )
			{
				return str_utils::to_string( p_name );
			}

#	endif

		}

#	if !defined( _WIN32 )

		void ShowBacktrace( StringStream & p_stream )
		{
			p_stream << cuT( "CALL STACK:" ) << std::endl;
			void * l_backTrace[NumOfFnCallsToCapture];
			unsigned int l_num( ::backtrace( l_backTrace, NumOfFnCallsToCapture ) );
			char ** l_fnStrings( ::backtrace_symbols( l_backTrace, l_num ) );

			for ( unsigned i = NumOfFnCallsToSkip; i < l_num; ++i )
			{
				p_stream << cuT( "== " ) << Demangle( l_fnStrings[i] ) << std::endl;
			}

			free( l_fnStrings );
		}

#	else

		void ShowBacktrace( StringStream & p_stream )
		{
			static bool SymbolsInitialised = false;
			const int MaxFnNameLen( 255 );

			void * l_backTrace[NumOfFnCallsToCapture - NumOfFnCallsToSkip];
			unsigned int l_num( ::CaptureStackBackTrace( NumOfFnCallsToSkip, NumOfFnCallsToCapture - NumOfFnCallsToSkip, l_backTrace, NULL ) );

			::HANDLE l_process( ::GetCurrentProcess() );
			p_stream << cuT( "CALL STACK:" ) << std::endl;

			// symbol->Name type is char [1] so there is space for \0 already
			SYMBOL_INFO * l_symbol( ( SYMBOL_INFO * ) malloc( sizeof( SYMBOL_INFO ) + ( MaxFnNameLen * sizeof( char ) ) ) );
			l_symbol->MaxNameLen = MaxFnNameLen;
			l_symbol->SizeOfStruct = sizeof( SYMBOL_INFO );

			if ( !SymbolsInitialised )
			{
				SymbolsInitialised = SymInitialize( l_process, NULL, TRUE ) == TRUE;
			}

			if ( SymbolsInitialised )
			{
				for ( unsigned int i = 0; i < l_num; ++i )
				{
					if ( SymFromAddr( l_process, reinterpret_cast< DWORD64 >( l_backTrace[i] ), 0, l_symbol ) )
					{
						p_stream << cuT( "== " ) << Demangle( std::string( l_symbol->Name, l_symbol->Name + l_symbol->NameLen ) ) << std::endl;
					}
				}

				free( l_symbol );
			}
			else
			{
				p_stream << "== Unable to retrieve the call stack" << std::endl;
			}
		}

#	endif
#endif

	}
}
