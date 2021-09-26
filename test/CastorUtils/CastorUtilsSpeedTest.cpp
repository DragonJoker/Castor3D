#include "CastorUtilsSpeedTest.hpp"

#include <CastorUtils/Math/Speed.hpp>

#include <numeric>

using namespace castor;

namespace Testing
{
	CastorUtilsSpeedTest::CastorUtilsSpeedTest()
		: TestCase{ "CastorUtilsSpeedTest" }
	{
	}

	void CastorUtilsSpeedTest::doRegisterTests()
	{
		doRegisterTest( "SpeedBasicTest", std::bind( &CastorUtilsSpeedTest::BasicTest, this ) );
		doRegisterTest( "SpeedConversionTest", std::bind( &CastorUtilsSpeedTest::ConversionTest, this ) );
		doRegisterTest( "SpeedAngleTest", std::bind( &CastorUtilsSpeedTest::AngleTest, this ) );
	}

	void CastorUtilsSpeedTest::BasicTest()
	{
		{
			int value = 8;
			SpeedT< int, Seconds > speed1 = makeSpeed< Seconds >( value );
			CT_EQUAL( speed1.Unit, 1_s );
			CT_EQUAL( speed1.getValue(), value );
			CT_EQUAL( speed1.getDistance( 1_s ), value );
			CT_EQUAL( speed1.getDistance( 10_s ), value * 10 );
		}
		{
			float value = 25000.0f;
			SpeedT< float, Seconds > speed2 = makeSpeed< Seconds >( value );
			CT_EQUAL( speed2.Unit, 1_s );
			CT_EQUAL( speed2.getValue(), value );
			CT_EQUAL( speed2.getDistance( 1_s ), value );
			CT_EQUAL( speed2.getDistance( 10_s ), value * 10.0f );
		}
	}

	void CastorUtilsSpeedTest::ConversionTest()
	{
		{
			int value = 8;
			SpeedT< int, Milliseconds > speed1 = makeSpeed< Milliseconds >( value );
			CT_EQUAL( speed1.getDistance( 1_s ), value * 1000 );

			SpeedT< int, Seconds > speed2{ speed1 };
			CT_EQUAL( speed2.Unit, 1_s );
			CT_EQUAL( speed2.getValue(), value * 1000 );
		}
		{
			float value = 250.0f;
			SpeedT< float, Seconds > speed3 = makeSpeed< Seconds >( value );
			CT_EQUAL( speed3.getDistance( 1_ms ), value / 1000 );

			SpeedT< float, Milliseconds > speed4{ speed3 };
			CT_EQUAL( speed4.Unit, 1_ms );
			CT_EQUAL( speed4.getValue(), value / 1000 );
		}
	}

	void CastorUtilsSpeedTest::AngleTest()
	{
		{
			Angle value{ Angle::fromDegrees( 8.0 ) };
			Angle valueMul{ Angle::fromDegrees( 8000.0 ) };
			Angle valueDiv{ Angle::fromDegrees( 0.008 ) };
			SpeedT< Angle, Milliseconds > speed1 = makeSpeed< Milliseconds >( value );
			CT_EQUAL( speed1.getDistance( 1_s ), valueMul );
			CT_EQUAL( speed1.getDistance( 1_us ), valueDiv );

			SpeedT< Angle, Seconds > speed2{ speed1 };
			CT_EQUAL( speed2.Unit, 1_s );
			CT_EQUAL( speed2.getValue(), valueMul );

			SpeedT< Angle, Microseconds > speed3{ speed1 };
			CT_EQUAL( speed3.Unit, 1_us );
			CT_EQUAL( speed3.getValue(), valueDiv );
		}
	}
}
