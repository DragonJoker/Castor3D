#include "UnitTest.hpp"

#include <CastorUtils/Log/Logger.hpp>
#include <CastorUtils/Miscellaneous/StringUtils.hpp>

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <sstream>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

namespace Testing
{
	//*************************************************************************************************

	TestBlock::TestBlock( TestCase & testCase
		, std::string text
		, bool indent )
		: testCase{ testCase }
		, text{ castor::move( text ) }
		, indent{ indent }
	{
	}

	TestBlock::TestBlock( TestCase & testCase
		, std::wstring text
		, bool indent )
		: testCase{ testCase }
		, wtext{ castor::move( text ) }
		, indent{ indent }
	{
	}

	TestBlock::TestBlock( TestCase & testCase
		, std::u32string text
		, bool indent )
		: testCase{ testCase }
		, u32text{ castor::move( text ) }
		, indent{ indent }
	{
	}

	TestBlock::~TestBlock()noexcept
	{
		testCase.doPopBlock( this );
	}

	//*************************************************************************************************

	TestCase::TestCase( std::string const & name )
		: m_name( name )
	{
	}

	void TestCase::registerTests()
	{
		doRegisterTests();
	}

	void TestCase::execute( uint32_t & errCount, uint32_t & testCount )
	{
		m_errorCount = &errCount;
		m_testCount = &testCount;

		for ( auto test : m_tests )
		{
			std::stringstream begin;
			begin << "**** ";
			begin.width( TEST_TITLE_WIDTH - 10 );
			begin << std::left << ( "Begin test case " + test.first ) << " ****";
			std::cout << begin.str() << std::endl;

			try
			{
				test.second();
			}
			catch ( TestFailed & exc )
			{
				errCount++;
				std::cerr << "*\tTest " << test.first << " failed (" << exc.what() << ")" << std::endl;
			}
			catch ( std::exception & exc )
			{
				errCount++;
				std::cerr << "*\tTest " << test.first << " execution failed (" << exc.what() << ")" << std::endl;
			}
			catch ( ... )
			{
				errCount++;
				std::cerr << "*\tTest " << test.first << " execution failed (Unknown reason)" << std::endl;
			}

			std::stringstream end;
			end << "**** ";
			end.width( TEST_TITLE_WIDTH - 10 );
			end << std::left << ( "End test case " + test.first ) << " ****";
			std::cout << end.str() << std::endl;
		}
	}

	void TestCase::doRegisterTest( std::string const & name
		, TestFunction test )
	{
		m_tests.emplace_back( name, castor::move( test ) );
	}

	TestBlockPtr TestCase::doPushBlock( std::string const & text, bool indent )
	{
		auto result = castor::make_unique< TestBlock >( *this, text, indent );
		m_blocks.emplace_back( result.get() );
		return result;
	}

	TestBlockPtr TestCase::doPushBlock( std::wstring const & text, bool indent )
	{
		auto result = castor::make_unique< TestBlock >( *this, text, indent );
		m_blocks.emplace_back( result.get() );
		return result;
	}

	TestBlockPtr TestCase::doPushBlock( std::u32string const & text, bool indent )
	{
		auto result = castor::make_unique< TestBlock >( *this, text, indent );
		m_blocks.emplace_back( result.get() );
		return result;
	}

	void TestCase::doPopBlock( TestBlock * block )
	{
		auto it = std::find( m_blocks.begin(), m_blocks.end(), block );
		m_blocks.erase( it );
	}

	void TestCase::doPrintError( std::string const & error )const
	{
		std::string oldPrefix;
		std::wstring woldPrefix;
		std::string prefix;
		std::wstring wprefix;

		for ( auto block : m_blocks )
		{
			if ( !block->wtext.empty() )
			{
				std::wcerr << ( block->indent ? wprefix : woldPrefix ) << block->wtext << std::endl;
			}
			else if ( !block->u32text.empty() )
			{
				std::cerr << ( block->indent ? prefix : oldPrefix );

				for ( auto c : block->u32text )
				{
					auto buffer = castor::string::utf8::fromUtf8( c );
					std::cerr << buffer.data();
				}

				std::cerr << std::endl;
			}
			else
			{
				std::cerr << ( block->indent ? prefix : oldPrefix ) << block->text << std::endl;
			}

			if ( block->indent )
			{
				oldPrefix += prefix;
				woldPrefix += wprefix;
				prefix += "  ";
				wprefix += L"  ";
			}
		}

		std::cerr << prefix << error << std::endl;
	}

	//*************************************************************************************************

	TestFailed::TestFailed( std::string const & what
		, std::string const & file
		, std::string const & function
		, uint32_t line )
		: m_what( file + " - " + function + ", line " + toString( line ) + " : " + what )
	{
	}

	//*************************************************************************************************
}
