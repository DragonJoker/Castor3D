#include "CastorUtilsDynamicBitsetTest.hpp"

#include <CastorUtils/Design/DynamicBitset.hpp>

#include <numeric>

namespace Testing
{
	CastorUtilsDynamicBitsetTest::CastorUtilsDynamicBitsetTest()
		: TestCase{ "CastorUtilsDynamicBitsetTest" }
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
			CT_ON( "	Size of 8, bits unset" );
			castor::DynamicBitset bitset{ 8u };
			CT_REQUIRE( bitset.getSize() == 8u );
			CT_REQUIRE( bitset.getBlockCount() == 1u );
		}
		{
			CT_ON( "	Size of 8, bits set" );
			castor::DynamicBitset bitset{ 8u, true };
			CT_REQUIRE( bitset.getSize() == 8u );
			CT_REQUIRE( bitset.getBlockCount() == 1u );
		}
		{
			CT_ON( "	Size of 37, bits set" );
			castor::DynamicBitset bitset{ 37u, true };
			CT_REQUIRE( bitset.getSize() == 37u );
			CT_REQUIRE( bitset.getBlockCount() == 2u );
		}
	}

	void CastorUtilsDynamicBitsetTest::initValueTest()
	{
		{
			CT_ON( "	Size of 8, bits unset" );
			castor::DynamicBitset bitset{ 8u };

			for ( size_t i = 0u; i < bitset.getSize(); ++i )
			{
				CT_EQUAL( bitset.get( i ), false );
			}

			castor::String test( 8u, '0' );
			CT_EQUAL( bitset.toString(), test );
		}
		{
			CT_ON( "	Size of 8, bits set" );
			castor::DynamicBitset bitset{ 8u, true };

			for ( size_t i = 0u; i < bitset.getSize(); ++i )
			{
				CT_EQUAL( bitset.get( i ), true );
			}

			castor::String test( 8u, '1' );
			CT_EQUAL( bitset.toString(), test );
		}
		{
			CT_ON( "	Size of 37, bits set" );
			castor::DynamicBitset bitset{ 37u, true };

			for ( size_t i = 0u; i < bitset.getSize(); ++i )
			{
				CT_EQUAL( bitset.get( i ), true );
			}

			castor::String test( 37u, '1' );
			CT_EQUAL( bitset.toString(), test );
		}
		{
			CT_ON( "	From string" );
			castor::MbString test = "011011010101010101010010101010101010110110";
			castor::DynamicBitset bitset{ test };
			CT_EQUAL( bitset.toString(), castor::makeString( test ) );
			CT_EQUAL( bitset.getSize(), test.size() );
		}
	}

	void CastorUtilsDynamicBitsetTest::leftShiftTest()
	{
		{
			CT_ON( "	Shift by 5" );
			castor::DynamicBitset bitset{ "011011010101010101010010101010101010110110" };
			bitset <<= 5;
			CT_EQUAL( bitset.toString(), cuT( "101010101010101001010101010101011011000000" ) );
		}
		{
			CT_ON( "	Shift by 35" );
			castor::DynamicBitset bitset{ "011011010101010101010010101010101010110110" };
			bitset <<= 35;
			CT_EQUAL( bitset.toString(), cuT( "011011000000000000000000000000000000000000" ) );
		}
		{
			CT_ON( "	Shift by size" );
			castor::DynamicBitset bitset{ "011011010101010101010010101010101010110110" };
			bitset <<= int( bitset.getSize() );
			CT_EQUAL( bitset.toString(), cuT( "000000000000000000000000000000000000000000" ) );
		}
	}

	void CastorUtilsDynamicBitsetTest::rightShiftTest()
	{
		{
			CT_ON( "	Shift by 5" );
			castor::DynamicBitset bitset{ "011011010101010101010010101010101010110110" };
			bitset >>= 5;
			CT_EQUAL( bitset.toString(), cuT( "000000110110101010101010100101010101010101" ) );
		}
		{
			CT_ON( "	Shift by 35" );
			castor::DynamicBitset bitset{ "011011010101010101010010101010101010110110" };
			bitset >>= 35;
			CT_EQUAL( bitset.toString(), cuT( "000000000000000000000000000000000000110110" ) );
		}
		{
			CT_ON( "	Shift by size" );
			castor::DynamicBitset bitset{ "011011010101010101010010101010101010110110" };
			bitset >>= int( bitset.getSize() );
			CT_EQUAL( bitset.toString(), cuT( "000000000000000000000000000000000000000000" ) );
		}
	}

	void CastorUtilsDynamicBitsetTest::andTest()
	{
		{
			castor::DynamicBitset lhs{ "011011010101010101010010101010101010110110" };
			castor::DynamicBitset rhs{ "100100101010101010101101010101010101001001" };
			auto result = lhs & rhs;
			CT_CHECK( result.none() );
		}
		{
			castor::DynamicBitset lhs{ "111" };
			castor::DynamicBitset rhs{ "010" };
			auto result = lhs & rhs;
			CT_CHECK( result == rhs );
		}
	}

	void CastorUtilsDynamicBitsetTest::orTest()
	{
		{
			castor::DynamicBitset lhs{ "011011010101010101010010101010101010110110" };
			castor::DynamicBitset rhs{ "100100101010101010101101010101010101001001" };
			auto result = lhs | rhs;
			CT_CHECK( result.all() );
		}
	}

	void CastorUtilsDynamicBitsetTest::xorTest()
	{
		{
			castor::DynamicBitset lhs{ "011011010101010101010010101010101010110110" };
			castor::DynamicBitset rhs{ "100100101010101010101101010101010101001001" };
			auto result = lhs ^ rhs;
			CT_CHECK( result.all() );
		}
		{
			castor::DynamicBitset lhs{ "111" };
			castor::DynamicBitset rhs{ "010" };
			auto result = lhs ^ rhs;
			castor::DynamicBitset test{ "101" };
			CT_CHECK( result == test );
		}
	}

	void CastorUtilsDynamicBitsetTest::setTest()
	{
		{
			castor::DynamicBitset value{ "011011010101010101010010101010101010110110" };

			value.set( 0 );
			CT_CHECK( bool( value[0] ) );
			value.set( 0, false );
			CT_CHECK( !value[0] );

			value.set( 3 );
			CT_CHECK( bool( value[3] ) );
			value.set( 3, false );
			CT_CHECK( !value[3] );
		}
	}
}
