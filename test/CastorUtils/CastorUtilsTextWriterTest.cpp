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

	CastorUtilsTextWriterTest::~CastorUtilsTextWriterTest()
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
		castor::Path fileName{ "resultBaseTypes.log" };
		{
			castor::TextFile file{ fileName, castor::File::OpenMode::eWrite };
			castor::TextWriterBase writer;
			CT_CHECK( writer.write( "bool", true, file ) );
			CT_CHECK( writer.write( "int16_t", int16_t( 1 ), file ) );
			CT_CHECK( writer.write( "uint16_t", uint16_t( 2u ), file ) );
			CT_CHECK( writer.write( "int32_t", int32_t( 3 ), file ) );
			CT_CHECK( writer.write( "uint32_t", uint32_t( 4u ), file ) );
			CT_CHECK( writer.write( "int64_t", int64_t( 5 ), file ) );
			CT_CHECK( writer.write( "uint64_t", uint64_t( 6u ), file ) );
			CT_CHECK( writer.write( "float", 7.0f, file ) );
			CT_CHECK( writer.write( "double", 8.0, file ) );
		}
		castor::File::deleteFile( fileName );
	}

	void CastorUtilsTextWriterTest::Point()
	{
		castor::Path fileName{ "resultPoint.log" };
		{
			castor::TextFile file{ fileName, castor::File::OpenMode::eWrite };
			castor::TextWriterBase writer;
			CT_CHECK( writer.write( "Point2f", castor::Point2f{ 0.0f, 1.0f }, file ) );
			CT_CHECK( writer.write( "Point3f", castor::Point3f{ 2.0f, 3.0f, 4.0f }, file ) );
			CT_CHECK( writer.write( "Point4f", castor::Point4f{ 5.0f, 6.0f, 7.0f, 8.0f }, file ) );
		}
		castor::File::deleteFile( fileName );
	}

	void CastorUtilsTextWriterTest::Quaternion()
	{
		castor::Path fileName{ "resultQuaternion.log" };
		{
			castor::TextFile file{ fileName, castor::File::OpenMode::eWrite };
			castor::TextWriterBase writer;
			CT_CHECK( writer.write( "Quaternion", castor::Quaternion::fromAxisAngle( castor::Point3f{ 0.0f, 1.0f, 0.5f }, castor::Angle::fromDegrees( 90.0f ) ), file ) );
		}
		castor::File::deleteFile( fileName );
	}

	void CastorUtilsTextWriterTest::RgbColour()
	{
		castor::Path fileName{ "resultRgbColour.log" };
		{
			castor::TextFile file{ fileName, castor::File::OpenMode::eWrite };
			castor::TextWriterBase writer;
			CT_CHECK( writer.write( "RgbColour", castor::RgbColour{ 0.0f, 0.5f, 1.0f }, file ) );
			CT_CHECK( writer.write( "HdrRgbColour", castor::HdrRgbColour{ 1.5f, 2.0f, 2.5f }, file ) );
		}
		castor::File::deleteFile( fileName );
	}

	void CastorUtilsTextWriterTest::RgbaColour()
	{
		castor::Path fileName{ "resultRgbaColour.log" };
		{
			castor::TextFile file{ fileName, castor::File::OpenMode::eWrite };
			castor::TextWriterBase writer;
			CT_CHECK( writer.write( "RgbaColour", castor::RgbaColour{ 0.0f, 0.25f, 0.75f, 1.0f }, file ) );
			CT_CHECK( writer.write( "HdrRgbaColour", castor::HdrRgbaColour{ 1.25f, 1.5f, 1.75f, 2.0f }, file ) );
		}
		castor::File::deleteFile( fileName );
	}

	void CastorUtilsTextWriterTest::UnnamedBlock()
	{
		castor::Path fileName{ "resultUnnamedBlock.log" };
		{
			castor::TextFile file{ fileName, castor::File::OpenMode::eWrite };
			castor::TextWriterBase writer;
			auto block = writer.beginBlock( file );
			CT_CHECK( bool( block ) );
			CT_CHECK( writer.write( "RgbaColour", castor::RgbaColour{ 0.0f, 0.25f, 0.75f, 1.0f }, file ) );
			CT_CHECK( writer.write( "HdrRgbaColour", castor::HdrRgbaColour{ 1.25f, 1.5f, 1.75f, 2.0f }, file ) );
		}
		castor::File::deleteFile( fileName );
	}

	void CastorUtilsTextWriterTest::NamedBlock()
	{
		castor::Path fileName{ "resultNamedBlock.log" };
		{
			castor::TextFile file{ fileName, castor::File::OpenMode::eWrite };
			castor::TextWriterBase writer;
			auto block = writer.beginBlock( "colours", file );
			CT_CHECK( bool( block ) );
			CT_CHECK( writer.write( "RgbaColour", castor::RgbaColour{ 0.0f, 0.25f, 0.75f, 1.0f }, file ) );
			CT_CHECK( writer.write( "HdrRgbaColour", castor::HdrRgbaColour{ 1.25f, 1.5f, 1.75f, 2.0f }, file ) );
		}
		castor::File::deleteFile( fileName );
	}

	void CastorUtilsTextWriterTest::TypedBlock()
	{
		castor::Path fileName{ "resultTypedBlock.log" };
		{
			castor::TextFile file{ fileName, castor::File::OpenMode::eWrite };
			castor::TextWriterBase writer;
			auto block = writer.beginBlock( "colours", "named", file );
			CT_CHECK( bool( block ) );
			CT_CHECK( writer.write( "RgbaColour", castor::RgbaColour{ 0.0f, 0.25f, 0.75f, 1.0f }, file ) );
			CT_CHECK( writer.write( "HdrRgbaColour", castor::HdrRgbaColour{ 1.25f, 1.5f, 1.75f, 2.0f }, file ) );
		}
		castor::File::deleteFile( fileName );
	}
}
