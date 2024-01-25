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
		doRegisterTest( "SpeedBasicTest", std::bind( &CastorUtilsSpeedTest::BasicTest, this ) );
		doRegisterTest( "SpeedConversionTest", std::bind( &CastorUtilsSpeedTest::ConversionTest, this ) );
		doRegisterTest( "SpeedAngleTest", std::bind( &CastorUtilsSpeedTest::AngleTest, this ) );
	}

	void CastorUtilsSpeedTest::BasicTest()
	{
		{
			int value = 8;
			castor::SpeedT< int, castor::Seconds > speed1 = castor::makeSpeed< castor::Seconds >( value );
			CT_EQUAL( speed1.Unit, 1_s );
			CT_EQUAL( speed1.getValue(), value );
			CT_EQUAL( speed1.getDistance( 1_s ), value );
			CT_EQUAL( speed1.getDistance( 10_s ), value * 10 );
		}
		{
			float value = 25000.0f;
			castor::SpeedT< float, castor::Seconds > speed2 = castor::makeSpeed< castor::Seconds >( value );
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
			castor::SpeedT< int, castor::Milliseconds > speed1 = castor::makeSpeed< castor::Milliseconds >( value );
			CT_EQUAL( speed1.getDistance( 1_s ), value * 1000 );

			castor::SpeedT< int, castor::Seconds > speed2{ speed1 };
			CT_EQUAL( speed2.Unit, 1_s );
			CT_EQUAL( speed2.getValue(), value * 1000 );
		}
		{
			float value = 250.0f;
			castor::SpeedT< float, castor::Seconds > speed3 = castor::makeSpeed< castor::Seconds >( value );
			CT_EQUAL( speed3.getDistance( 1_ms ), value / 1000 );

			castor::SpeedT< float, castor::Milliseconds > speed4{ speed3 };
			CT_EQUAL( speed4.Unit, 1_ms );
			CT_EQUAL( speed4.getValue(), value / 1000 );
		}
	}

	void CastorUtilsSpeedTest::AngleTest()
	{
		{
			castor::Angle value{ castor::Angle::fromDegrees( 8.0 ) };
			castor::Angle valueMul{ castor::Angle::fromDegrees( 8000.0 ) };
			castor::Angle valueDiv{ castor::Angle::fromDegrees( 0.008 ) };
			castor::SpeedT< castor::Angle, castor::Milliseconds > speed1 = castor::makeSpeed< castor::Milliseconds >( value );
			CT_EQUAL( speed1.getDistance( 1_s ), valueMul );
			CT_EQUAL( speed1.getDistance( 1_us ), valueDiv );

			castor::SpeedT< castor::Angle, castor::Seconds > speed2{ speed1 };
			CT_EQUAL( speed2.Unit, 1_s );
			CT_EQUAL( speed2.getValue(), valueMul );

			castor::SpeedT< castor::Angle, castor::Microseconds > speed3{ speed1 };
			CT_EQUAL( speed3.Unit, 1_us );
			CT_EQUAL( speed3.getValue(), valueDiv );
		}
	}
}
