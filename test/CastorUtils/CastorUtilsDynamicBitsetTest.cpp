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
		doRegisterTest( "DynamicBitsetSizeTest", [this](){ sizeTest(); } );
		doRegisterTest( "DynamicBitsetInitValueTest", [this](){ initValueTest(); } );
		doRegisterTest( "DynamicBitsetLeftShiftTest", [this](){ leftShiftTest(); } );
		doRegisterTest( "DynamicBitsetRightShiftTest", [this](){ rightShiftTest(); } );
		doRegisterTest( "DynamicBitsetAndTest", [this](){ andTest(); } );
		doRegisterTest( "DynamicBitsetOrTest", [this](){ orTest(); } );
		doRegisterTest( "DynamicBitsetXorTest", [this](){ xorTest(); } );
		doRegisterTest( "DynamicBitsetSetTest", [this](){ setTest(); } );
	}

	void CastorUtilsDynamicBitsetTest::sizeTest()
	{
		{
			CT_ON( "	Size of 8, bits unset" );
			c3d::DynamicBitset bitset{ 8u };
			CT_REQUIRE( bitset.getSize() == 8u );
			CT_REQUIRE( bitset.getBlockCount() == 1u );
		}
		{
			CT_ON( "	Size of 8, bits set" );
			c3d::DynamicBitset bitset{ 8u, true };
			CT_REQUIRE( bitset.getSize() == 8u );
			CT_REQUIRE( bitset.getBlockCount() == 1u );
		}
		{
			CT_ON( "	Size of 37, bits set" );
			c3d::DynamicBitset bitset{ 37u, true };
			CT_REQUIRE( bitset.getSize() == 37u );
			CT_REQUIRE( bitset.getBlockCount() == 2u );
		}
	}

	void CastorUtilsDynamicBitsetTest::initValueTest()
	{
		{
			CT_ON( "	Size of 8, bits unset" );
			c3d::DynamicBitset bitset{ 8u };

			for ( size_t i = 0u; i < bitset.getSize(); ++i )
			{
				CT_EQUAL( bitset.get( i ), false );
			}

			c3d::String test( 8u, '0' );
			CT_EQUAL( bitset.toString(), test );
		}
		{
			CT_ON( "	Size of 8, bits set" );
			c3d::DynamicBitset bitset{ 8u, true };

			for ( size_t i = 0u; i < bitset.getSize(); ++i )
			{
				CT_EQUAL( bitset.get( i ), true );
			}

			c3d::String test( 8u, '1' );
			CT_EQUAL( bitset.toString(), test );
		}
		{
			CT_ON( "	Size of 37, bits set" );
			c3d::DynamicBitset bitset{ 37u, true };

			for ( size_t i = 0u; i < bitset.getSize(); ++i )
			{
				CT_EQUAL( bitset.get( i ), true );
			}

			c3d::String test( 37u, '1' );
			CT_EQUAL( bitset.toString(), test );
		}
		{
			CT_ON( "	From string" );
			c3d::MbString test = "011011010101010101010010101010101010110110";
			c3d::DynamicBitset bitset{ test };
			CT_EQUAL( bitset.toString(), c3d::makeString( test ) );
			CT_EQUAL( bitset.getSize(), test.size() );
		}
	}

	void CastorUtilsDynamicBitsetTest::leftShiftTest()
	{
		{
			CT_ON( "	Shift by 5" );
			c3d::DynamicBitset bitset{ "011011010101010101010010101010101010110110" };
			bitset <<= 5;
			CT_EQUAL( bitset.toString(), cuT( "101010101010101001010101010101011011000000" ) );
		}
		{
			CT_ON( "	Shift by 35" );
			c3d::DynamicBitset bitset{ "011011010101010101010010101010101010110110" };
			bitset <<= 35;
			CT_EQUAL( bitset.toString(), cuT( "011011000000000000000000000000000000000000" ) );
		}
		{
			CT_ON( "	Shift by size" );
			c3d::DynamicBitset bitset{ "011011010101010101010010101010101010110110" };
			bitset <<= int( bitset.getSize() );
			CT_EQUAL( bitset.toString(), cuT( "000000000000000000000000000000000000000000" ) );
		}
	}

	void CastorUtilsDynamicBitsetTest::rightShiftTest()
	{
		{
			CT_ON( "	Shift by 5" );
			c3d::DynamicBitset bitset{ "011011010101010101010010101010101010110110" };
			bitset >>= 5;
			CT_EQUAL( bitset.toString(), cuT( "000000110110101010101010100101010101010101" ) );
		}
		{
			CT_ON( "	Shift by 35" );
			c3d::DynamicBitset bitset{ "011011010101010101010010101010101010110110" };
			bitset >>= 35;
			CT_EQUAL( bitset.toString(), cuT( "000000000000000000000000000000000000110110" ) );
		}
		{
			CT_ON( "	Shift by size" );
			c3d::DynamicBitset bitset{ "011011010101010101010010101010101010110110" };
			bitset >>= int( bitset.getSize() );
			CT_EQUAL( bitset.toString(), cuT( "000000000000000000000000000000000000000000" ) );
		}
	}

	void CastorUtilsDynamicBitsetTest::andTest()
	{
		{
			c3d::DynamicBitset lhs{ "011011010101010101010010101010101010110110" };
			c3d::DynamicBitset rhs{ "100100101010101010101101010101010101001001" };
			auto result = lhs & rhs;
			CT_CHECK( result.none() );
		}
		{
			c3d::DynamicBitset lhs{ "111" };
			c3d::DynamicBitset rhs{ "010" };
			auto result = lhs & rhs;
			CT_CHECK( result == rhs );
		}
	}

	void CastorUtilsDynamicBitsetTest::orTest()
	{
		{
			c3d::DynamicBitset lhs{ "011011010101010101010010101010101010110110" };
			c3d::DynamicBitset rhs{ "100100101010101010101101010101010101001001" };
			auto result = lhs | rhs;
			CT_CHECK( result.all() );
		}
	}

	void CastorUtilsDynamicBitsetTest::xorTest()
	{
		{
			c3d::DynamicBitset lhs{ "011011010101010101010010101010101010110110" };
			c3d::DynamicBitset rhs{ "100100101010101010101101010101010101001001" };
			auto result = lhs ^ rhs;
			CT_CHECK( result.all() );
		}
		{
			c3d::DynamicBitset lhs{ "111" };
			c3d::DynamicBitset rhs{ "010" };
			auto result = lhs ^ rhs;
			c3d::DynamicBitset test{ "101" };
			CT_CHECK( result == test );
		}
	}

	void CastorUtilsDynamicBitsetTest::setTest()
	{
		{
			c3d::DynamicBitset value{ "011011010101010101010010101010101010110110" };

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
