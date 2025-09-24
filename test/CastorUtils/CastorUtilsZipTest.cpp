#include "CastorUtilsZipTest.hpp"

#include <CastorUtils/Data/ZipArchive.hpp>
#include <CastorUtils/Data/BinaryFile.hpp>
#include <CastorUtils/Data/TextFile.hpp>

#include <cstring>

namespace Testing
{
	CastorUtilsZipTest::CastorUtilsZipTest()
		:	TestCase( "CastorUtilsZipTest" )
	{
	}

	void CastorUtilsZipTest::doRegisterTests()
	{
		doRegisterTest( "ZipFile", [this](){ ZipFile(); } );
	}

	void CastorUtilsZipTest::ZipFile()
	{
		c3d::Path folder1{ cuT( "test1" ) };
		c3d::Path folder2{ folder1 / cuT( "test2" ) };

		std::cout << "\tFirst folder creation" << std::endl;

		if ( c3d::File::directoryExists( folder1 )
			|| c3d::File::directoryCreate( folder1 ) )
		{
			std::cout << "\tSecond folder creation" << std::endl;

			if ( c3d::File::directoryExists( folder2 )
				|| c3d::File::directoryCreate( folder2 ) )
			{
				c3d::Path binName = folder1 / cuT( "binFile.bin" );
				c3d::Path txtName = folder2 / cuT( "txtFile.txt" );
				c3d::Path zipName{ cuT( "zipFile.zip" ) };

				c3d::Vector< uint8_t > inBinData( 1024 );
				c3d::String inTxtData( cuT( "Coucou, comment allez-vous?" ) );

				if ( !c3d::File::fileExists( binName ) )
				{
					std::cout << "\tBinary file creation" << std::endl;
					c3d::BinaryFile binary( binName, c3d::File::OpenMode::eWrite );
					binary.writeArray( inBinData.data(), inBinData.size() );
				}

				if ( !c3d::File::fileExists( txtName ) )
				{
					std::cout << "\tText file creation" << std::endl;
					c3d::TextFile text( txtName, c3d::File::OpenMode::eWrite );
					text.writeText( inTxtData );
				}

				{
					std::cout << "\tdeflate the archive" << std::endl;
					c3d::ZipArchive def( zipName, c3d::File::OpenMode::eWrite );
					def.addFile( binName );
					def.addFile( txtName );
					def.deflate();
				}

				{
					std::cout << "\tinflate the archive" << std::endl;
					if ( c3d::Path folder( cuT( "inflated" ) );
						c3d::File::directoryExists( folder )
							|| c3d::File::directoryCreate( folder ) )
					{
						c3d::ZipArchive inf( zipName, c3d::File::OpenMode::eRead );
						inf.inflate( folder );

						c3d::String outTxtData;

						{
							std::cout << "\tCheck binary file content" << std::endl;
							c3d::BinaryFile binary( folder / binName, c3d::File::OpenMode::eRead );
							c3d::Vector< uint8_t > outBinData( size_t( binary.getLength() ) );
							binary.readArray( outBinData.data(), outBinData.size() );
							CT_EQUAL( outBinData.size(), inBinData.size() );
							CT_CHECK( !std::memcmp( outBinData.data(), inBinData.data(), std::min( outBinData.size(), inBinData.size() ) ) );
						}

						{
							std::cout << "\tCheck text file content" << std::endl;
							c3d::TextFile text( folder / txtName, c3d::File::OpenMode::eRead );
							text.readLine( outTxtData, inTxtData.size() * sizeof( c3d::xchar ) );
							CT_EQUAL( outTxtData, inTxtData );
						}

						std::remove( c3d::toUtf8( folder / binName ).c_str() );
						std::remove( c3d::toUtf8( folder / txtName ).c_str() );
						c3d::File::directoryDelete( folder / folder2 );
						c3d::File::directoryDelete( folder / folder1 );
						c3d::File::directoryDelete( folder );
					}
					else
					{
						CT_CHECK( c3d::File::directoryExists( folder ) );
					}

					std::remove( c3d::toUtf8( binName ).c_str() );
					std::remove( c3d::toUtf8( txtName ).c_str() );
					std::remove( c3d::toUtf8( zipName ).c_str() );
					c3d::File::directoryDelete( folder2 );
					c3d::File::directoryDelete( folder1 );
				}
			}
			else
			{
				std::cout << "\tCouldn't create second folder" << std::endl;
			}
		}
		else
		{
			std::cout << "\tCouldn't create first folder" << std::endl;
		}
	}
}
