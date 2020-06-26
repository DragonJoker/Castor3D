#include "CastorUtils/Config/PlatformConfig.hpp"

#if defined( CU_PlatformWindows )

#include "CastorUtils/Config/MultiThreadConfig.hpp"
#include "CastorUtils/Miscellaneous/MiscellaneousModule.hpp"
#include "CastorUtils/Miscellaneous/DynamicLibrary.hpp"
#include "CastorUtils/Miscellaneous/StringUtils.hpp"
#include "CastorUtils/Miscellaneous/Utils.hpp"

#if !defined( NDEBUG )
#	include <Windows.h>
#	pragma warning( push )
#	pragma warning( disable:4091 ) //'typedef ': ignored on left of '' when no variable is declared
#	include <Dbghelp.h>
#	pragma warning( pop )
#endif

namespace castor
{
	namespace Debug
	{
#if !defined( NDEBUG )

		namespace
		{
			bool & doGetInitialisationStatus()
			{
				static bool result = false;
				return result;
			}

			DWORD64 & doGetLibraryBaseAddress( DynamicLibrary const & library )
			{
				static std::map< DynamicLibrary const *, DWORD64 > result;
				return result[&library];
			}

			::HANDLE doGetProcess()
			{
				static ::HANDLE const result( ::GetCurrentProcess() );
				return result;
			}

			template< typename CharU, typename CharT >
			inline std::basic_string< CharU > Demangle( std::basic_string< CharT > const & p_name )
			{
				std::string ret = string::stringCast< char >( p_name );

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

				return string::stringCast< CharU >( ret );
			}

			template< typename CharT >
			inline void doShowBacktrace( std::basic_ostream< CharT > & p_stream, int p_toCapture, int p_toSkip )
			{
				static std::mutex mutex;
				using LockType = std::unique_lock< std::mutex >;

				if ( doGetInitialisationStatus() )
				{
					LockType lock{ makeUniqueLock( mutex ) };
					const int MaxFnNameLen( 255 );

					std::vector< void * > backTrace( p_toCapture - p_toSkip );
					unsigned int num( ::RtlCaptureStackBackTrace( p_toSkip, p_toCapture - p_toSkip, backTrace.data(), nullptr ) );

					p_stream << "CALL STACK:" << std::endl;

					// symbol->Name type is char [1] so there is space for \0 already
					auto symbol( ( SYMBOL_INFO * )malloc( sizeof( SYMBOL_INFO ) + ( MaxFnNameLen * sizeof( char ) ) ) );

					if ( symbol )
					{
						symbol->MaxNameLen = MaxFnNameLen;
						symbol->SizeOfStruct = sizeof( SYMBOL_INFO );
						for ( unsigned int i = 0; i < num; ++i )
						{
							if ( ::SymFromAddr( doGetProcess(), reinterpret_cast< DWORD64 >( backTrace[i] ), nullptr, symbol ) )
							{
								p_stream << "== " << Demangle< CharT >( string::stringCast< char >( symbol->Name, symbol->Name + symbol->NameLen ) );
								IMAGEHLP_LINE64 line;
								DWORD displacement;
								line.SizeOfStruct = sizeof( IMAGEHLP_LINE64 );

								if ( ::SymGetLineFromAddr64( doGetProcess(), symbol->Address, &displacement, &line ) )
								{
									p_stream << "(" << string::stringCast< CharT >( line.FileName ) << ":" << line.LineNumber << ")";
								}

								p_stream << std::endl;
							}
							else
							{
								p_stream << "== Symbol not found." << std::endl;
							}
						}

						free( symbol );
					}
				}
				else
				{
					p_stream << "== Unable to retrieve the call stack: " << string::stringCast< CharT >( System::getLastErrorText() ) << std::endl;
				}
			}
		}

#else

		template< typename CharT >
		inline void doShowBacktrace( std::basic_ostream< CharT > & p_stream, int, int )
		{
		}

#endif

#if !defined( NDEBUG )

		void initialise()
		{
			::SymSetOptions( SYMOPT_UNDNAME | SYMOPT_LOAD_LINES );
			doGetInitialisationStatus() = ::SymInitialize( doGetProcess(), nullptr, TRUE ) == TRUE;

			if ( !doGetInitialisationStatus() )
			{
				std::cerr << "SymInitialize failed: " << System::getLastErrorText() << std::endl;
			}
		}

		void cleanup()
		{
			if ( doGetInitialisationStatus() )
			{
				::SymCleanup( doGetProcess() );
			}
		}

		void loadModule( DynamicLibrary const & library )
		{
			//auto result = ::SymLoadModuleEx( doGetProcess()    // target process 
			//	, nullptr                                      // handle to image - not used
			//	, library.getPath().c_str()                    // name of image file
			//	, nullptr                                      // name of module - not required
			//	, 0                                            // base address - not required
			//	, 0                                            // size of image - not required
			//	, nullptr                                      // MODLOAD_DATA used for special cases 
			//	, 0 );                                         // flags - not required

			//if ( !result )
			//{
			//	std::cerr << "SymLoadModuleEx failed: " << System::getLastErrorText() << std::endl;
			//}
			//else
			//{
			//	doGetLibraryBaseAddress( library ) = result;
			//}
		}

		void unloadModule( DynamicLibrary const & library )
		{
			//auto address = doGetLibraryBaseAddress( library );

			//if ( address )
			//{
			//	::SymUnloadModule64( doGetProcess(), address );
			//}
		}

#else

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

#endif

		std::wostream & operator<<( std::wostream & p_stream, Backtrace const & p_backtrace )
		{
			static std::locale const loc{ "C" };
			p_stream.imbue( loc );
			doShowBacktrace( p_stream, p_backtrace.m_toCapture, p_backtrace.m_toSkip );
			return p_stream;
		}

		std::ostream & operator<<( std::ostream & p_stream, Backtrace const & p_backtrace )
		{
			static std::locale const loc{ "C" };
			p_stream.imbue( loc );
			doShowBacktrace( p_stream, p_backtrace.m_toCapture, p_backtrace.m_toSkip );
			return p_stream;
		}
	}
}

#endif
