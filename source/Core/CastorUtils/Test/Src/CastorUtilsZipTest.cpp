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
		Path l_folder1{ cuT( "test1" ) };
		Path l_folder2{ l_folder1 / cuT( "test2" ) };

		std::cout << "	First folder creation" << std::endl;

		if ( File::DirectoryExists( l_folder1 ) || File::DirectoryCreate( l_folder1 ) )
		{
			std::cout << "	Second folder creation" << std::endl;

			if ( File::DirectoryExists( l_folder2 ) || File::DirectoryCreate( l_folder2 ) )
			{
				Path l_binName = l_folder1 / cuT( "binFile.bin" );
				Path l_txtName = l_folder2 / cuT( "txtFile.txt" );
				Path l_zipName{ cuT( "zipFile.zip" ) };

				std::vector< uint8_t > l_inBinData( 1024 );
				String l_inTxtData( cuT( "Coucou, comment allez-vous?" ) );

				if ( !File::FileExists( l_binName ) )
				{
					std::cout << "	Binary file creation" << std::endl;
					BinaryFile l_binary( l_binName, File::OpenMode::eWrite );
					l_binary.WriteArray( l_inBinData.data(), l_inBinData.size() );
				}

				if ( !File::FileExists( l_txtName ) )
				{
					std::cout << "	Text file creation" << std::endl;
					TextFile l_text( l_txtName, File::OpenMode::eWrite );
					l_text.WriteText( l_inTxtData );
				}

				{
					std::cout << "	Deflate the archive" << std::endl;
					ZipArchive l_def( l_zipName, File::OpenMode::eWrite );
					l_def.AddFile( l_binName );
					l_def.AddFile( l_txtName );
					l_def.Deflate();
				}

				{
					std::cout << "	Inflate the archive" << std::endl;
					Path l_folder( cuT( "inflated" ) );

					if ( File::DirectoryExists( l_folder ) || File::DirectoryCreate( l_folder ) )
					{
						ZipArchive l_inf( l_zipName, File::OpenMode::eRead );
						l_inf.Inflate( l_folder );

						String l_outTxtData;

						{
							std::cout << "	Check binary file content" << std::endl;
							BinaryFile l_binary( l_folder / l_binName, File::OpenMode::eRead );
							std::vector< uint8_t > l_outBinData( size_t( l_binary.GetLength() ) );
							l_binary.ReadArray( l_outBinData.data(), l_outBinData.size() );
							CT_EQUAL( l_outBinData.size(), l_inBinData.size() );
							CT_CHECK( !std::memcmp( l_outBinData.data(), l_inBinData.data(), std::min( l_outBinData.size(), l_inBinData.size() ) ) );
						}

						{
							std::cout << "	Check text file content" << std::endl;
							TextFile l_text( l_folder / l_txtName, File::OpenMode::eRead );
							l_text.ReadLine( l_outTxtData, l_inTxtData.size() * sizeof( xchar ) );
							CT_EQUAL( l_outTxtData, l_inTxtData );
						}

						std::remove( string::string_cast< char >( l_folder / l_binName ).c_str() );
						std::remove( string::string_cast< char >( l_folder / l_txtName ).c_str() );
						File::DirectoryDelete( l_folder / l_folder2 );
						File::DirectoryDelete( l_folder / l_folder1 );
						File::DirectoryDelete( l_folder );
					}
					else
					{
						CT_CHECK( File::DirectoryExists( l_folder ) );
					}

					std::remove( string::string_cast< char >( l_binName ).c_str() );
					std::remove( string::string_cast< char >( l_txtName ).c_str() );
					std::remove( string::string_cast< char >( l_zipName ).c_str() );
					File::DirectoryDelete( l_folder2 );
					File::DirectoryDelete( l_folder1 );
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
