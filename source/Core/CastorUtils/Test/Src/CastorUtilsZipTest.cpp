#include "CastorUtilsZipTest.hpp"

#include <Data/ZipArchive.hpp>
#include <Data/BinaryFile.hpp>
#include <Data/TextFile.hpp>

#include <cstring>

using namespace Castor;

namespace Testing
{
	CastorUtilsZipTest::CastorUtilsZipTest()
		:	TestCase( "CastorUtilsZipTest" )
	{
	}

	CastorUtilsZipTest::~CastorUtilsZipTest()
	{
	}

	void CastorUtilsZipTest::DoRegisterTests()
	{
		DoRegisterTest( "ZipFile", std::bind( &CastorUtilsZipTest::ZipFile, this ) );
	}

	void CastorUtilsZipTest::ZipFile()
	{
		Path folder1{ cuT( "test1" ) };
		Path folder2{ folder1 / cuT( "test2" ) };

		std::cout << "	First folder creation" << std::endl;

		if ( File::DirectoryExists( folder1 ) || File::DirectoryCreate( folder1 ) )
		{
			std::cout << "	Second folder creation" << std::endl;

			if ( File::DirectoryExists( folder2 ) || File::DirectoryCreate( folder2 ) )
			{
				Path binName = folder1 / cuT( "binFile.bin" );
				Path txtName = folder2 / cuT( "txtFile.txt" );
				Path zipName{ cuT( "zipFile.zip" ) };

				std::vector< uint8_t > inBinData( 1024 );
				String inTxtData( cuT( "Coucou, comment allez-vous?" ) );

				if ( !File::FileExists( binName ) )
				{
					std::cout << "	Binary file creation" << std::endl;
					BinaryFile binary( binName, File::OpenMode::eWrite );
					binary.WriteArray( inBinData.data(), inBinData.size() );
				}

				if ( !File::FileExists( txtName ) )
				{
					std::cout << "	Text file creation" << std::endl;
					TextFile text( txtName, File::OpenMode::eWrite );
					text.WriteText( inTxtData );
				}

				{
					std::cout << "	Deflate the archive" << std::endl;
					ZipArchive def( zipName, File::OpenMode::eWrite );
					def.AddFile( binName );
					def.AddFile( txtName );
					def.Deflate();
				}

				{
					std::cout << "	Inflate the archive" << std::endl;
					Path folder( cuT( "inflated" ) );

					if ( File::DirectoryExists( folder ) || File::DirectoryCreate( folder ) )
					{
						ZipArchive inf( zipName, File::OpenMode::eRead );
						inf.Inflate( folder );

						String outTxtData;

						{
							std::cout << "	Check binary file content" << std::endl;
							BinaryFile binary( folder / binName, File::OpenMode::eRead );
							std::vector< uint8_t > outBinData( size_t( binary.GetLength() ) );
							binary.ReadArray( outBinData.data(), outBinData.size() );
							CT_EQUAL( outBinData.size(), inBinData.size() );
							CT_CHECK( !std::memcmp( outBinData.data(), inBinData.data(), std::min( outBinData.size(), inBinData.size() ) ) );
						}

						{
							std::cout << "	Check text file content" << std::endl;
							TextFile text( folder / txtName, File::OpenMode::eRead );
							text.ReadLine( outTxtData, inTxtData.size() * sizeof( xchar ) );
							CT_EQUAL( outTxtData, inTxtData );
						}

						std::remove( string::string_cast< char >( folder / binName ).c_str() );
						std::remove( string::string_cast< char >( folder / txtName ).c_str() );
						File::DirectoryDelete( folder / folder2 );
						File::DirectoryDelete( folder / folder1 );
						File::DirectoryDelete( folder );
					}
					else
					{
						CT_CHECK( File::DirectoryExists( folder ) );
					}

					std::remove( string::string_cast< char >( binName ).c_str() );
					std::remove( string::string_cast< char >( txtName ).c_str() );
					std::remove( string::string_cast< char >( zipName ).c_str() );
					File::DirectoryDelete( folder2 );
					File::DirectoryDelete( folder1 );
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
