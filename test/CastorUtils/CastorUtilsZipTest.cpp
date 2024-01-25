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
		doRegisterTest( "ZipFile", std::bind( &CastorUtilsZipTest::ZipFile, this ) );
	}

	void CastorUtilsZipTest::ZipFile()
	{
		castor::Path folder1{ cuT( "test1" ) };
		castor::Path folder2{ folder1 / cuT( "test2" ) };

		std::cout << "	First folder creation" << std::endl;

		if ( castor::File::directoryExists( folder1 )
			|| castor::File::directoryCreate( folder1 ) )
		{
			std::cout << "	Second folder creation" << std::endl;

			if ( castor::File::directoryExists( folder2 )
				|| castor::File::directoryCreate( folder2 ) )
			{
				castor::Path binName = folder1 / cuT( "binFile.bin" );
				castor::Path txtName = folder2 / cuT( "txtFile.txt" );
				castor::Path zipName{ cuT( "zipFile.zip" ) };

				castor::Vector< uint8_t > inBinData( 1024 );
				castor::String inTxtData( cuT( "Coucou, comment allez-vous?" ) );

				if ( !castor::File::fileExists( binName ) )
				{
					std::cout << "	Binary file creation" << std::endl;
					castor::BinaryFile binary( binName, castor::File::OpenMode::eWrite );
					binary.writeArray( inBinData.data(), inBinData.size() );
				}

				if ( !castor::File::fileExists( txtName ) )
				{
					std::cout << "	Text file creation" << std::endl;
					castor::TextFile text( txtName, castor::File::OpenMode::eWrite );
					text.writeText( inTxtData );
				}

				{
					std::cout << "	deflate the archive" << std::endl;
					castor::ZipArchive def( zipName, castor::File::OpenMode::eWrite );
					def.addFile( binName );
					def.addFile( txtName );
					def.deflate();
				}

				{
					std::cout << "	inflate the archive" << std::endl;
					castor::Path folder( cuT( "inflated" ) );

					if ( castor::File::directoryExists( folder )
						|| castor::File::directoryCreate( folder ) )
					{
						castor::ZipArchive inf( zipName, castor::File::OpenMode::eRead );
						inf.inflate( folder );

						castor::String outTxtData;

						{
							std::cout << "	Check binary file content" << std::endl;
							castor::BinaryFile binary( folder / binName, castor::File::OpenMode::eRead );
							castor::Vector< uint8_t > outBinData( size_t( binary.getLength() ) );
							binary.readArray( outBinData.data(), outBinData.size() );
							CT_EQUAL( outBinData.size(), inBinData.size() );
							CT_CHECK( !std::memcmp( outBinData.data(), inBinData.data(), std::min( outBinData.size(), inBinData.size() ) ) );
						}

						{
							std::cout << "	Check text file content" << std::endl;
							castor::TextFile text( folder / txtName, castor::File::OpenMode::eRead );
							text.readLine( outTxtData, inTxtData.size() * sizeof( castor::xchar ) );
							CT_EQUAL( outTxtData, inTxtData );
						}

						std::remove( castor::toUtf8( folder / binName ).c_str() );
						std::remove( castor::toUtf8( folder / txtName ).c_str() );
						castor::File::directoryDelete( folder / folder2 );
						castor::File::directoryDelete( folder / folder1 );
						castor::File::directoryDelete( folder );
					}
					else
					{
						CT_CHECK( castor::File::directoryExists( folder ) );
					}

					std::remove( castor::toUtf8( binName ).c_str() );
					std::remove( castor::toUtf8( txtName ).c_str() );
					std::remove( castor::toUtf8( zipName ).c_str() );
					castor::File::directoryDelete( folder2 );
					castor::File::directoryDelete( folder1 );
				}
			}
			else
			{
				std::cout << "	Couldn't create second folder" << std::endl;
			}
		}
		else
		{
			std::cout << "	Couldn't create first folder" << std::endl;
		}
	}
}
