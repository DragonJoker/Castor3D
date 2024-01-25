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
		CT_CHECK( writer.write( file, cuT( "bool" ), true ) );
		CT_CHECK( writer.write( file, cuT( "int16_t" ), int16_t( 1 ) ) );
		CT_CHECK( writer.write( file, cuT( "uint16_t" ), uint16_t( 2u ) ) );
		CT_CHECK( writer.write( file, cuT( "int32_t" ), 3 ) );
		CT_CHECK( writer.write( file, cuT( "uint32_t" ), 4u ) );
		CT_CHECK( writer.write( file, cuT( "int64_t" ), int64_t( 5 ) ) );
		CT_CHECK( writer.write( file, cuT( "uint64_t" ), uint64_t( 6u ) ) );
		CT_CHECK( writer.write( file, cuT( "float" ), 7.0f ) );
		CT_CHECK( writer.write( file, cuT( "double" ), 8.0 ) );
	}

	void CastorUtilsTextWriterTest::Point()
	{
		auto file = castor::makeStringStream();
		castor::TextWriterBase writer;
		CT_CHECK( writer.writeNamedSub( file, cuT( "Point2f" ), castor::Point2f{ 0.0f, 1.0f } ) );
		CT_CHECK( writer.writeNamedSub( file, cuT( "Point3f" ), castor::Point3f{ 2.0f, 3.0f, 4.0f } ) );
		CT_CHECK( writer.writeNamedSub( file, cuT( "Point4f" ), castor::Point4f{ 5.0f, 6.0f, 7.0f, 8.0f } ) );
	}

	void CastorUtilsTextWriterTest::Quaternion()
	{
		auto file = castor::makeStringStream();
		castor::TextWriterBase writer;
		CT_CHECK( writer.writeNamedSub( file, cuT( "Quaternion" ), castor::Quaternion::fromAxisAngle( castor::Point3f{ 0.0f, 1.0f, 0.5f }, castor::Angle::fromDegrees( 90.0f ) ) ) );
	}

	void CastorUtilsTextWriterTest::RgbColour()
	{
		auto file = castor::makeStringStream();
		castor::TextWriterBase writer;
		CT_CHECK( writer.writeNamedSub( file, cuT( "RgbColour" ), castor::RgbColour{ 0.0f, 0.5f, 1.0f } ) );
		CT_CHECK( writer.writeNamedSub( file, cuT( "HdrRgbColour" ), castor::HdrRgbColour{ 1.5f, 2.0f, 2.5f } ) );
	}

	void CastorUtilsTextWriterTest::RgbaColour()
	{
		auto file = castor::makeStringStream();
		castor::TextWriterBase writer;
		CT_CHECK( writer.writeNamedSub( file, cuT( "RgbaColour" ), castor::RgbaColour{ 0.0f, 0.25f, 0.75f, 1.0f } ) );
		CT_CHECK( writer.writeNamedSub( file, cuT( "HdrRgbaColour" ), castor::HdrRgbaColour{ 1.25f, 1.5f, 1.75f, 2.0f } ) );
	}

	void CastorUtilsTextWriterTest::UnnamedBlock()
	{
		auto file = castor::makeStringStream();
		castor::TextWriterBase writer;
		auto block = writer.beginBlock( file );
		CT_CHECK( bool( block ) );
		CT_CHECK( writer.writeNamedSub( file, cuT( "RgbaColour" ), castor::RgbaColour{ 0.0f, 0.25f, 0.75f, 1.0f } ) );
		CT_CHECK( writer.writeNamedSub( file, cuT( "HdrRgbaColour" ), castor::HdrRgbaColour{ 1.25f, 1.5f, 1.75f, 2.0f } ) );
	}

	void CastorUtilsTextWriterTest::NamedBlock()
	{
		auto file = castor::makeStringStream();
		castor::TextWriterBase writer;
		auto block = writer.beginBlock( file, cuT( "colours" ) );
		CT_CHECK( bool( block ) );
		CT_CHECK( writer.writeNamedSub( file, cuT( "RgbaColour" ), castor::RgbaColour{ 0.0f, 0.25f, 0.75f, 1.0f } ) );
		CT_CHECK( writer.writeNamedSub( file, cuT( "HdrRgbaColour" ), castor::HdrRgbaColour{ 1.25f, 1.5f, 1.75f, 2.0f } ) );
	}

	void CastorUtilsTextWriterTest::TypedBlock()
	{
		auto file = castor::makeStringStream();
		castor::TextWriterBase writer;
		auto block = writer.beginBlock( file, cuT( "colours" ), cuT( "named" ) );
		CT_CHECK( bool( block ) );
		CT_CHECK( writer.writeNamedSub( file, cuT( "RgbaColour" ), castor::RgbaColour{ 0.0f, 0.25f, 0.75f, 1.0f } ) );
		CT_CHECK( writer.writeNamedSub( file, cuT( "HdrRgbaColour" ), castor::HdrRgbaColour{ 1.25f, 1.5f, 1.75f, 2.0f } ) );
	}
}
