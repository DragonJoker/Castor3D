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

namespace castor::Debug
{
#if !defined( NDEBUG )

	namespace
	{
		struct DbgHelpContext
		{
			DbgHelpContext()
				: process{ ::GetCurrentProcess() }
			{
				::SymSetOptions( SYMOPT_UNDNAME | SYMOPT_LOAD_LINES );
				initialised = ( ::SymInitialize( process, nullptr, TRUE ) == TRUE );

				if ( !initialised )
				{
					std::cerr << "SymInitialize failed: " << System::getLastErrorText() << std::endl;
				}
			}

			~DbgHelpContext()
			{
				::SymCleanup( process );
			}

			void loadModule( DynamicLibrary const & CU_UnusedParam( library ) )
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
				//	libraryBaseAddress[&library] = result;
				//}
			}

			void unloadModule( DynamicLibrary const & CU_UnusedParam( library ) )
			{
				//auto address = libraryBaseAddress[&library];

				//if ( address )
				//{
				//	::SymUnloadModule64( doGetProcess(), address );
				//}
			}

			template< typename CharU, typename CharT >
			inline std::basic_string< CharU > demangle( std::basic_string< CharT > const & p_name )
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

			HANDLE process{};
			bool initialised{};

		private:
			std::map< DynamicLibrary const *, DWORD64 > libraryBaseAddress;
		};

		using DbgHelpContextPtr = std::unique_ptr< DbgHelpContext >;

		DbgHelpContextPtr & getContext()
		{
			static DbgHelpContextPtr result{ std::make_unique< DbgHelpContext >() };
			return result;
		}

		template< typename CharT >
		void showBacktrace( std::basic_ostream< CharT > & stream
			, int toCapture
			, int toSkip )
		{
			static std::mutex mutex;
			using LockType = std::unique_lock< std::mutex >;

			auto & context = getContext();

			if ( context && context->initialised )
			{
				LockType lock{ makeUniqueLock( mutex ) };
				const int MaxFnNameLen( 255 );

				std::vector< void * > backTrace( size_t( toCapture - toSkip ) );
				auto num( ::RtlCaptureStackBackTrace( DWORD( toSkip )
					, DWORD( toCapture - toSkip )
					, backTrace.data()
					, nullptr ) );

				stream << "CALL STACK:" << std::endl;

				// symbol->Name type is char [1] so there is space for \0 already
				auto size = sizeof( SYMBOL_INFO ) + ( MaxFnNameLen * sizeof( char ) );
				auto symbol( ( SYMBOL_INFO * )malloc( size ) );

				if ( symbol )
				{
					memset( symbol, 0, size );
					symbol->MaxNameLen = MaxFnNameLen;
					symbol->SizeOfStruct = sizeof( SYMBOL_INFO );

					for ( unsigned int i = 0; i < num; ++i )
					{
						if ( ::SymFromAddr( context->process, reinterpret_cast< DWORD64 >( backTrace[i] ), nullptr, symbol ) )
						{
							stream << "== " << context->demangle< CharT >( string::stringCast< char >( symbol->Name, symbol->Name + symbol->NameLen ) );
							IMAGEHLP_LINE64 line{};
							DWORD displacement{};
							line.SizeOfStruct = sizeof( IMAGEHLP_LINE64 );

							if ( ::SymGetLineFromAddr64( context->process, symbol->Address, &displacement, &line ) )
							{
								stream << "(" << string::stringCast< CharT >( line.FileName ) << ":" << line.LineNumber << ":" << displacement << ")";
							}

							stream << std::endl;
						}
						else
						{
							stream << "== Symbol not found." << std::endl;
						}
					}

					free( symbol );
				}
			}
			else
			{
				stream << "== Unable to retrieve the call stack: " << string::stringCast< CharT >( System::getLastErrorText() ) << std::endl;
			}
		}
	}

	void initialise()
	{
		getContext();
	}

	void cleanup()
	{
		getContext().reset();
	}

	void loadModule( DynamicLibrary const & library )
	{
		getContext()->loadModule( library );
	}

	void unloadModule( DynamicLibrary const & library )
	{
		getContext()->unloadModule( library );
	}

#else

	namespace
	{
		template< typename CharT >
		void showBacktrace( std::basic_ostream< CharT > & p_stream, int, int )
		{
		}
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

#endif

	std::wostream & operator<<( std::wostream & p_stream, Backtrace const & p_backtrace )
	{
		static std::locale const loc{ "C" };
		p_stream.imbue( loc );
		showBacktrace( p_stream, p_backtrace.m_toCapture, p_backtrace.m_toSkip );
		return p_stream;
	}

	std::ostream & operator<<( std::ostream & p_stream, Backtrace const & p_backtrace )
	{
		static std::locale const loc{ "C" };
		p_stream.imbue( loc );
		showBacktrace( p_stream, p_backtrace.m_toCapture, p_backtrace.m_toSkip );
		return p_stream;
	}
}

#endif
