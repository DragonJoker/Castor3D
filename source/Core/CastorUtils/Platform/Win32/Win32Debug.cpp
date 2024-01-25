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

namespace castor::debug
{
#if !defined( NDEBUG )

	namespace dbg
	{
		struct DbgHelpContext
		{
			DbgHelpContext( DbgHelpContext const & ) = delete;
			DbgHelpContext( DbgHelpContext && )noexcept = delete;
			DbgHelpContext & operator=( DbgHelpContext const & ) = delete;
			DbgHelpContext & operator=( DbgHelpContext && )noexcept = delete;
			DbgHelpContext()
				: process{ ::GetCurrentProcess() }
			{
				::SymSetOptions( SYMOPT_UNDNAME | SYMOPT_LOAD_LINES );
				initialised = ( ::SymInitialize( process, nullptr, TRUE ) == TRUE );

				if ( !initialised )
				{
					std::cerr << "SymInitialize failed: " << toUtf8( system::getLastErrorText() ) << std::endl;
				}
			}

			~DbgHelpContext()noexcept
			{
				::SymCleanup( process );
			}

			void loadModule( CU_UnusedParam( DynamicLibrary const &, library ) )const
			{
/**
				auto result = ::SymLoadModuleEx( doGetProcess()    // target process 
					, nullptr                                      // handle to image - not used
					, library.getPath().c_str()                    // name of image file
					, nullptr                                      // name of module - not required
					, 0                                            // base address - not required
					, 0                                            // size of image - not required
					, nullptr                                      // MODLOAD_DATA used for special cases 
					, 0 );                                         // flags - not required

				if ( !result )
				{
					std::cerr << "SymLoadModuleEx failed: " << system::getLastErrorText() << std::endl;
				}
				else
				{
					libraryBaseAddress[&library] = result;
				}
*/
			}

			void unloadModule( CU_UnusedParam( DynamicLibrary const &, library ) )const
			{
/**
				auto address = libraryBaseAddress[&library];

				if ( address )
				{
					::SymUnloadModule64( doGetProcess(), address );
				}
*/
			}

			inline MbString demangle( MbString const & name )const
			{
				MbString ret = name;

				try
				{
					if ( Array< char, 2048 > real{};
						::UnDecorateSymbolName( ret.c_str(), real.data(), DWORD( real.size() ), UNDNAME_COMPLETE ) )
					{
						ret = real.data();
					}
				}
				catch ( ... )
				{
					// What to do...
				}

				return ret;
			}

			HANDLE process{};
			bool initialised{};

		private:
			Map< DynamicLibrary const *, DWORD64 > libraryBaseAddress;
		};

		using DbgHelpContextPtr = castor::RawUniquePtr< DbgHelpContext >;

		static DbgHelpContextPtr & getContext()
		{
			static DbgHelpContextPtr result{ castor::make_unique< DbgHelpContext >() };
			return result;
		}

		static void showSymbol( DbgHelpContext const & context
			, DWORD64 symbolAddress
			, SYMBOL_INFO * symbolInfo
			, OutputStream & stream )
		{
			if ( ::SymFromAddr( context.process, symbolAddress, nullptr, symbolInfo ) )
			{
				stream << "== " << makeString( context.demangle( std::string{ symbolInfo->Name, symbolInfo->Name + symbolInfo->NameLen } ) );
				IMAGEHLP_LINE64 line{};
				DWORD displacement{};
				line.SizeOfStruct = sizeof( IMAGEHLP_LINE64 );

				if ( ::SymGetLineFromAddr64( context.process, symbolInfo->Address, &displacement, &line ) )
				{
					stream << "(" << makeString( line.FileName ) << ":" << line.LineNumber << ":" << displacement << ")";
				}

				stream << std::endl;
			}
			else
			{
				stream << "== Symbol not found." << std::endl;
			}
		}

		static void showBacktrace( OutputStream & stream
			, int toCapture
			, int toSkip )
		{
			static castor::Mutex mutex;
			using LockType = castor::UniqueLock< castor::Mutex >;

			if ( auto const & context = getContext();
				context && context->initialised )
			{
				LockType lock{ makeUniqueLock( mutex ) };
				const int MaxFnNameLen( 255 );

				Vector< void * > backTrace( size_t( toCapture - toSkip ) );
				auto num( ::RtlCaptureStackBackTrace( DWORD( toSkip )
					, DWORD( toCapture - toSkip )
					, backTrace.data()
					, nullptr ) );

				stream << cuT( "CALL STACK:" ) << std::endl;

				// symbol->Name type is char [1] so there is space for \0 already
				auto size = sizeof( SYMBOL_INFO ) + ( MaxFnNameLen * sizeof( char ) );

				if ( auto symbol = static_cast< SYMBOL_INFO * >( malloc( size ) ) )
				{
					memset( symbol, 0, size );
					symbol->MaxNameLen = MaxFnNameLen;
					symbol->SizeOfStruct = sizeof( SYMBOL_INFO );

					for ( unsigned int i = 0; i < num; ++i )
					{
						showSymbol( *context, DWORD64( backTrace[i] ), symbol, stream );
					}

					free( symbol );
				}
			}
			else
			{
				stream << cuT( "== Unable to retrieve the call stack: " ) << system::getLastErrorText() << std::endl;
			}
		}
	}

	void initialise()
	{
		dbg::getContext();
	}

	void cleanup()
	{
		dbg::getContext().reset();
	}

	void loadModule( DynamicLibrary const & library )
	{
		dbg::getContext()->loadModule( library );
	}

	void unloadModule( DynamicLibrary const & library )
	{
		dbg::getContext()->unloadModule( library );
	}

#else

	namespace dbg
	{
		template< typename CharT >
		void showBacktrace( std::basic_ostream< CharT > &, int, int )
		{
		}
	}

	void initialise()
	{
	}

	void cleanup()
	{
	}

	void loadModule( DynamicLibrary const & )
	{
	}

	void unloadModule( DynamicLibrary const & )
	{
	}

#endif

	OutputStream & operator<<( OutputStream & stream, Backtrace const & backtrace )
	{
		static std::locale const loc{ "C" };
		auto oldLoc = stream.imbue( loc );
		dbg::showBacktrace( stream, backtrace.m_toCapture, backtrace.m_toSkip );
		stream.imbue( oldLoc );
		return stream;
	}
}

#endif
