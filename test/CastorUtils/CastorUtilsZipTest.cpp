#include "CastorUtilsZipTest.hpp"

#include <CastorUtils/Data/ZipArchive.hpp>
#include <CastorUtils/Data/BinaryFile.hpp>
#include <CastorUtils/Data/TextFile.hpp>

#include <cstring>

using namespace castor;

namespace Testing
{
	CastorUtilsZipTest::CastorUtilsZipTest()
		:	TestCase( "CastorUtilsZipTest" )
	{
	}

	CastorUtilsZipTest::~CastorUtilsZipTest()
	{
	}

	void CastorUtilsZipTest::doRegisterTests()
	{
		doRegisterTest( "ZipFile", std::bind( &CastorUtilsZipTest::ZipFile, this ) );
	}

	void CastorUtilsZipTest::ZipFile()
	{
		Path folder1{ cuT( "test1" ) };
		Path folder2{ folder1 / cuT( "test2" ) };

		std::cout << "	First folder creation" << std::endl;

		if ( File::directoryExists( folder1 ) || File::directoryCreate( folder1 ) )
		{
			std::cout << "	Second folder creation" << std::endl;

			if ( File::directoryExists( folder2 ) || File::directoryCreate( folder2 ) )
			{
				Path binName = folder1 / cuT( "binFile.bin" );
				Path txtName = folder2 / cuT( "txtFile.txt" );
				Path zipName{ cuT( "zipFile.zip" ) };

				std::vector< uint8_t > inBinData( 1024 );
				String inTxtData( cuT( "Coucou, comment allez-vous?" ) );

				if ( !File::fileExists( binName ) )
				{
					std::cout << "	Binary file creation" << std::endl;
					BinaryFile binary( binName, File::OpenMode::eWrite );
					binary.writeArray( inBinData.data(), inBinData.size() );
				}

				if ( !File::fileExists( txtName ) )
				{
					std::cout << "	Text file creation" << std::endl;
					TextFile text( txtName, File::OpenMode::eWrite );
					text.writeText( inTxtData );
				}

				{
					std::cout << "	deflate the archive" << std::endl;
					ZipArchive def( zipName, File::OpenMode::eWrite );
					def.addFile( binName );
					def.addFile( txtName );
					def.deflate();
				}

				{
					std::cout << "	inflate the archive" << std::endl;
					Path folder( cuT( "inflated" ) );

					if ( File::directoryExists( folder ) || File::directoryCreate( folder ) )
					{
						ZipArchive inf( zipName, File::OpenMode::eRead );
						inf.inflate( folder );

						String outTxtData;

						{
							std::cout << "	Check binary file content" << std::endl;
							BinaryFile binary( folder / binName, File::OpenMode::eRead );
							std::vector< uint8_t > outBinData( size_t( binary.getLength() ) );
							binary.readArray( outBinData.data(), outBinData.size() );
							CT_EQUAL( outBinData.size(), inBinData.size() );
							CT_CHECK( !std::memcmp( outBinData.data(), inBinData.data(), std::min( outBinData.size(), inBinData.size() ) ) );
						}

						{
							std::cout << "	Check text file content" << std::endl;
							TextFile text( folder / txtName, File::OpenMode::eRead );
							text.readLine( outTxtData, inTxtData.size() * sizeof( xchar ) );
							CT_EQUAL( outTxtData, inTxtData );
						}

						std::remove( string::stringCast< char >( folder / binName ).c_str() );
						std::remove( string::stringCast< char >( folder / txtName ).c_str() );
						File::directoryDelete( folder / folder2 );
						File::directoryDelete( folder / folder1 );
						File::directoryDelete( folder );
					}
					else
					{
						CT_CHECK( File::directoryExists( folder ) );
					}

					std::remove( string::stringCast< char >( binName ).c_str() );
					std::remove( string::stringCast< char >( txtName ).c_str() );
					std::remove( string::stringCast< char >( zipName ).c_str() );
					File::directoryDelete( folder2 );
					File::directoryDelete( folder1 );
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
