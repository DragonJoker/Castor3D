#include "CastorUtilsSpeedTest.hpp"

#include <CastorUtils/Math/Speed.hpp>

#include <numeric>

namespace Testing
{
	CastorUtilsSpeedTest::CastorUtilsSpeedTest()
		: TestCase{ "CastorUtilsSpeedTest" }
	{
	}

	void CastorUtilsSpeedTest::doRegisterTests()
	{
		doRegisterTest( "SpeedBasicTest", [this](){ BasicTest(); } );
		doRegisterTest( "SpeedConversionTest", [this](){ ConversionTest(); } );
		doRegisterTest( "SpeedAngleTest", [this](){ AngleTest(); } );
	}

	void CastorUtilsSpeedTest::BasicTest()
	{
		{
			int value = 8;
			c3d::SpeedT< int, c3d::Seconds > speed1 = c3d::makeSpeed< c3d::Seconds >( value );
			CT_EQUAL( speed1.Unit, 1_s );
			CT_EQUAL( speed1.getValue(), value );
			CT_EQUAL( speed1.getDistance( 1_s ), value );
			CT_EQUAL( speed1.getDistance( 10_s ), value * 10 );
		}
		{
			float value = 25000.0f;
			c3d::SpeedT< float, c3d::Seconds > speed2 = c3d::makeSpeed< c3d::Seconds >( value );
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
			c3d::SpeedT< int, c3d::Milliseconds > speed1 = c3d::makeSpeed< c3d::Milliseconds >( value );
			CT_EQUAL( speed1.getDistance( 1_s ), value * 1000 );

			c3d::SpeedT< int, c3d::Seconds > speed2{ speed1 };
			CT_EQUAL( speed2.Unit, 1_s );
			CT_EQUAL( speed2.getValue(), value * 1000 );
		}
		{
			float value = 250.0f;
			c3d::SpeedT< float, c3d::Seconds > speed3 = c3d::makeSpeed< c3d::Seconds >( value );
			CT_EQUAL( speed3.getDistance( 1_ms ), value / 1000 );

			c3d::SpeedT< float, c3d::Milliseconds > speed4{ speed3 };
			CT_EQUAL( speed4.Unit, 1_ms );
			CT_EQUAL( speed4.getValue(), value / 1000 );
		}
	}

	void CastorUtilsSpeedTest::AngleTest()
	{
		{
			c3d::Angle value{ c3d::Angle::fromDegrees( 8.0 ) };
			c3d::Angle valueMul{ c3d::Angle::fromDegrees( 8000.0 ) };
			c3d::Angle valueDiv{ c3d::Angle::fromDegrees( 0.008 ) };
			c3d::SpeedT< c3d::Angle, c3d::Milliseconds > speed1 = c3d::makeSpeed< c3d::Milliseconds >( value );
			CT_EQUAL( speed1.getDistance( 1_s ), valueMul );
			CT_EQUAL( speed1.getDistance( 1_us ), valueDiv );

			c3d::SpeedT< c3d::Angle, c3d::Seconds > speed2{ speed1 };
			CT_EQUAL( speed2.Unit, 1_s );
			CT_EQUAL( speed2.getValue(), valueMul );

			c3d::SpeedT< c3d::Angle, c3d::Microseconds > speed3{ speed1 };
			CT_EQUAL( speed3.Unit, 1_us );
			CT_EQUAL( speed3.getValue(), valueDiv );
		}
	}
}
