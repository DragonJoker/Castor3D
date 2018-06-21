#include "CastorUtilsDynamicBitsetTest.hpp"

#include <Design/DynamicBitset.hpp>

#include <numeric>

using namespace castor;

namespace Testing
{
	CastorUtilsDynamicBitsetTest::CastorUtilsDynamicBitsetTest()
		: TestCase{ "CastorUtilsDynamicBitsetTest" }
	{
	}

	CastorUtilsDynamicBitsetTest::~CastorUtilsDynamicBitsetTest()
	{
	}

	void CastorUtilsDynamicBitsetTest::doRegisterTests()
	{
		doRegisterTest( "DynamicBitsetSizeTest", std::bind( &CastorUtilsDynamicBitsetTest::sizeTest, this ) );
		doRegisterTest( "DynamicBitsetInitValueTest", std::bind( &CastorUtilsDynamicBitsetTest::initValueTest, this ) );
		doRegisterTest( "DynamicBitsetLeftShiftTest", std::bind( &CastorUtilsDynamicBitsetTest::leftShiftTest, this ) );
		doRegisterTest( "DynamicBitsetRightShiftTest", std::bind( &CastorUtilsDynamicBitsetTest::rightShiftTest, this ) );
		doRegisterTest( "DynamicBitsetAndTest", std::bind( &CastorUtilsDynamicBitsetTest::andTest, this ) );
		doRegisterTest( "DynamicBitsetOrTest", std::bind( &CastorUtilsDynamicBitsetTest::orTest, this ) );
		doRegisterTest( "DynamicBitsetXorTest", std::bind( &CastorUtilsDynamicBitsetTest::xorTest, this ) );
		doRegisterTest( "DynamicBitsetSetTest", std::bind( &CastorUtilsDynamicBitsetTest::setTest, this ) );
	}

	void CastorUtilsDynamicBitsetTest::sizeTest()
	{
		{
			DynamicBitset bitset{ 8u };
			CT_REQUIRE( bitset.getSize() == 8u );
			CT_REQUIRE( bitset.getBlockCount() == 1u );
		}
		{
			DynamicBitset bitset{ 8u, true };
			CT_REQUIRE( bitset.getSize() == 8u );
			CT_REQUIRE( bitset.getBlockCount() == 1u );
		}
		{
			DynamicBitset bitset{ 37u, true };
			CT_REQUIRE( bitset.getSize() == 37u );
			CT_REQUIRE( bitset.getBlockCount() == 2u );
		}
	}

	void CastorUtilsDynamicBitsetTest::initValueTest()
	{
		{
			DynamicBitset bitset{ 8u };

			for ( size_t i = 0u; i < bitset.getSize(); ++i )
			{
				CT_EQUAL( bitset.get( i ), false );
			}

			String test( 8u, '0' );
			CT_EQUAL( bitset.toString(), test );
		}
		{
			DynamicBitset bitset{ 8u, true };

			for ( size_t i = 0u; i < bitset.getSize(); ++i )
			{
				CT_EQUAL( bitset.get( i ), true );
			}

			String test( 8u, '1' );
			CT_EQUAL( bitset.toString(), test );
		}
		{
			DynamicBitset bitset{ 37u, true };

			for ( size_t i = 0u; i < bitset.getSize(); ++i )
			{
				CT_EQUAL( bitset.get( i ), true );
			}

			String test( 37u, '1' );
			CT_EQUAL( bitset.toString(), test );
		}
		{
			String test = cuT( "011011010101010101010010101010101010110110" );
			DynamicBitset bitset{ test };
			CT_EQUAL( bitset.toString(), test );
			CT_EQUAL( bitset.getSize(), test.size() );
		}
	}

	void CastorUtilsDynamicBitsetTest::leftShiftTest()
	{
		{
			DynamicBitset bitset{ cuT( "011011010101010101010010101010101010110110" ) };
			bitset <<= 5;
			CT_EQUAL( bitset.toString(), cuT( "101010101010101001010101010101011011000000" ) );
		}
		{
			DynamicBitset bitset{ cuT( "011011010101010101010010101010101010110110" ) };
			bitset <<= 35;
			CT_EQUAL( bitset.toString(), cuT( "011011000000000000000000000000000000000000" ) );
		}
		{
			DynamicBitset bitset{ cuT( "011011010101010101010010101010101010110110" ) };
			bitset <<= int( bitset.getSize() );
			CT_EQUAL( bitset.toString(), cuT( "000000000000000000000000000000000000000000" ) );
		}
	}

	void CastorUtilsDynamicBitsetTest::rightShiftTest()
	{
		{
			DynamicBitset bitset{ cuT( "011011010101010101010010101010101010110110" ) };
			bitset >>= 5;
			CT_EQUAL( bitset.toString(), cuT( "000000110110101010101010100101010101010101" ) );
		}
		{
			DynamicBitset bitset{ cuT( "011011010101010101010010101010101010110110" ) };
			bitset >>= 35;
			CT_EQUAL( bitset.toString(), cuT( "000000000000000000000000000000000000110110" ) );
		}
		{
			DynamicBitset bitset{ cuT( "011011010101010101010010101010101010110110" ) };
			bitset >>= int( bitset.getSize() );
			CT_EQUAL( bitset.toString(), cuT( "000000000000000000000000000000000000000000" ) );
		}
	}

	void CastorUtilsDynamicBitsetTest::andTest()
	{
		{
			DynamicBitset lhs{ cuT( "011011010101010101010010101010101010110110" ) };
			DynamicBitset rhs{ cuT( "100100101010101010101101010101010101001001" ) };
			auto result = lhs & rhs;
			CT_CHECK( result.none() );
		}
		{
			DynamicBitset lhs{ cuT( "111" ) };
			DynamicBitset rhs{ cuT( "010" ) };
			auto result = lhs & rhs;
			CT_CHECK( result == rhs );
		}
	}

	void CastorUtilsDynamicBitsetTest::orTest()
	{
		{
			DynamicBitset lhs{ cuT( "011011010101010101010010101010101010110110" ) };
			DynamicBitset rhs{ cuT( "100100101010101010101101010101010101001001" ) };
			auto result = lhs | rhs;
			CT_CHECK( result.all() );
		}
	}

	void CastorUtilsDynamicBitsetTest::xorTest()
	{
		{
			DynamicBitset lhs{ cuT( "011011010101010101010010101010101010110110" ) };
			DynamicBitset rhs{ cuT( "100100101010101010101101010101010101001001" ) };
			auto result = lhs ^ rhs;
			CT_CHECK( result.all() );
		}
		{
			DynamicBitset lhs{ cuT( "111" ) };
			DynamicBitset rhs{ cuT( "010" ) };
			auto result = lhs ^ rhs;
			DynamicBitset test{ cuT( "101" ) };
			CT_CHECK( result == test );
		}
	}

	void CastorUtilsDynamicBitsetTest::setTest()
	{
		{
			DynamicBitset value{ cuT( "011011010101010101010010101010101010110110" ) };

			value.set( 0 );
			CT_CHECK( value[0] );
			value.set( 0, false );
			CT_CHECK( !value[0] );

			value.set( 3 );
			CT_CHECK( value[3] );
			value.set( 3, false );
			CT_CHECK( !value[3] );
		}
	}
}
