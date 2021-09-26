#include "CastorUtilsTextWriterTest.hpp"

#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/Data/Text/TextCoords.hpp>
#include <CastorUtils/Data/Text/TextPoint.hpp>
#include <CastorUtils/Data/Text/TextQuaternion.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>
#include <CastorUtils/Data/Text/TextRgbaColour.hpp>

namespace Testing
{
	CastorUtilsTextWriterTest::CastorUtilsTextWriterTest()
		: TestCase{ "CastorUtilsTextWriterTest" }
	{
	}

	void CastorUtilsTextWriterTest::doRegisterTests()
	{
		doRegisterTest( "BaseTypesTextWriterTest", std::bind( &CastorUtilsTextWriterTest::BaseTypes, this ) );
		doRegisterTest( "PointTextWriterTest", std::bind( &CastorUtilsTextWriterTest::Point, this ) );
		doRegisterTest( "QuaternionTextWriterTest", std::bind( &CastorUtilsTextWriterTest::Quaternion, this ) );
		doRegisterTest( "RgbColourTextWriterTest", std::bind( &CastorUtilsTextWriterTest::RgbColour, this ) );
		doRegisterTest( "RgbaColourTextWriterTest", std::bind( &CastorUtilsTextWriterTest::RgbaColour, this ) );
		doRegisterTest( "UnnamedBlockTextWriterTest", std::bind( &CastorUtilsTextWriterTest::UnnamedBlock, this ) );
		doRegisterTest( "NamedBlockTextWriterTest", std::bind( &CastorUtilsTextWriterTest::NamedBlock, this ) );
		doRegisterTest( "TypedBlockTextWriterTest", std::bind( &CastorUtilsTextWriterTest::TypedBlock, this ) );
	}

	void CastorUtilsTextWriterTest::BaseTypes()
	{
		auto file = castor::makeStringStream();
		castor::TextWriterBase writer;
		CT_CHECK( writer.write( file, "bool", true ) );
		CT_CHECK( writer.write( file, "int16_t", int16_t( 1 ) ) );
		CT_CHECK( writer.write( file, "uint16_t", uint16_t( 2u ) ) );
		CT_CHECK( writer.write( file, "int32_t", 3 ) );
		CT_CHECK( writer.write( file, "uint32_t", 4u ) );
		CT_CHECK( writer.write( file, "int64_t", int64_t( 5 ) ) );
		CT_CHECK( writer.write( file, "uint64_t", uint64_t( 6u ) ) );
		CT_CHECK( writer.write( file, "float", 7.0f ) );
		CT_CHECK( writer.write( file, "double", 8.0 ) );
	}

	void CastorUtilsTextWriterTest::Point()
	{
		auto file = castor::makeStringStream();
		castor::TextWriterBase writer;
		CT_CHECK( writer.writeNamedSub( file, "Point2f", castor::Point2f{ 0.0f, 1.0f } ) );
		CT_CHECK( writer.writeNamedSub( file, "Point3f", castor::Point3f{ 2.0f, 3.0f, 4.0f } ) );
		CT_CHECK( writer.writeNamedSub( file, "Point4f", castor::Point4f{ 5.0f, 6.0f, 7.0f, 8.0f } ) );
	}

	void CastorUtilsTextWriterTest::Quaternion()
	{
		auto file = castor::makeStringStream();
		castor::TextWriterBase writer;
		CT_CHECK( writer.writeNamedSub( file, "Quaternion", castor::Quaternion::fromAxisAngle( castor::Point3f{ 0.0f, 1.0f, 0.5f }, castor::Angle::fromDegrees( 90.0f ) ) ) );
	}

	void CastorUtilsTextWriterTest::RgbColour()
	{
		auto file = castor::makeStringStream();
		castor::TextWriterBase writer;
		CT_CHECK( writer.writeNamedSub( file, "RgbColour", castor::RgbColour{ 0.0f, 0.5f, 1.0f } ) );
		CT_CHECK( writer.writeNamedSub( file, "HdrRgbColour", castor::HdrRgbColour{ 1.5f, 2.0f, 2.5f } ) );
	}

	void CastorUtilsTextWriterTest::RgbaColour()
	{
		auto file = castor::makeStringStream();
		castor::TextWriterBase writer;
		CT_CHECK( writer.writeNamedSub( file, "RgbaColour", castor::RgbaColour{ 0.0f, 0.25f, 0.75f, 1.0f } ) );
		CT_CHECK( writer.writeNamedSub( file, "HdrRgbaColour", castor::HdrRgbaColour{ 1.25f, 1.5f, 1.75f, 2.0f } ) );
	}

	void CastorUtilsTextWriterTest::UnnamedBlock()
	{
		auto file = castor::makeStringStream();
		castor::TextWriterBase writer;
		auto block = writer.beginBlock( file );
		CT_CHECK( bool( block ) );
		CT_CHECK( writer.writeNamedSub( file, "RgbaColour", castor::RgbaColour{ 0.0f, 0.25f, 0.75f, 1.0f } ) );
		CT_CHECK( writer.writeNamedSub( file, "HdrRgbaColour", castor::HdrRgbaColour{ 1.25f, 1.5f, 1.75f, 2.0f } ) );
	}

	void CastorUtilsTextWriterTest::NamedBlock()
	{
		auto file = castor::makeStringStream();
		castor::TextWriterBase writer;
		auto block = writer.beginBlock( file, "colours" );
		CT_CHECK( bool( block ) );
		CT_CHECK( writer.writeNamedSub( file, "RgbaColour", castor::RgbaColour{ 0.0f, 0.25f, 0.75f, 1.0f } ) );
		CT_CHECK( writer.writeNamedSub( file, "HdrRgbaColour", castor::HdrRgbaColour{ 1.25f, 1.5f, 1.75f, 2.0f } ) );
	}

	void CastorUtilsTextWriterTest::TypedBlock()
	{
		auto file = castor::makeStringStream();
		castor::TextWriterBase writer;
		auto block = writer.beginBlock( file, "colours", "named" );
		CT_CHECK( bool( block ) );
		CT_CHECK( writer.writeNamedSub( file, "RgbaColour", castor::RgbaColour{ 0.0f, 0.25f, 0.75f, 1.0f } ) );
		CT_CHECK( writer.writeNamedSub( file, "HdrRgbaColour", castor::HdrRgbaColour{ 1.25f, 1.5f, 1.75f, 2.0f } ) );
	}
}
