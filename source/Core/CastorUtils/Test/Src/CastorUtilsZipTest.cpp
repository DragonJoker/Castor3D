#include "CastorUtilsZipTest.hpp"

#include <ZipArchive.hpp>
#include <BinaryFile.hpp>
#include <TextFile.hpp>

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

		Logger::LogInfo( "	First folder creation" );

		if ( File::DirectoryExists( l_folder1 ) || File::DirectoryCreate( l_folder1 ) )
		{
			Logger::LogInfo( "	Second folder creation" );

			if ( File::DirectoryExists( l_folder2 ) || File::DirectoryCreate( l_folder2 ) )
			{
				Path l_binName = l_folder1 / cuT( "binFile.bin" );
				Path l_txtName = l_folder2 / cuT( "txtFile.txt" );
				Path l_zipName{ cuT( "zipFile.zip" ) };

				std::vector< uint8_t > l_inBinData( 1024 );
				String l_inTxtData( cuT( "Coucou, comment allez-vous?" ) );

				if ( !File::FileExists( l_binName ) )
				{
					Logger::LogInfo( "	Binary file creation" );
					BinaryFile l_binary( l_binName, File::eOPEN_MODE_WRITE );
					l_binary.WriteArray( l_inBinData.data(), l_inBinData.size() );
				}

				if ( !File::FileExists( l_txtName ) )
				{
					Logger::LogInfo( "	Text file creation" );
					TextFile l_text( l_txtName, File::eOPEN_MODE_WRITE );
					l_text.WriteText( l_inTxtData );
				}

				{
					Logger::LogInfo( "	Deflate the archive" );
					ZipArchive l_def( l_zipName, File::eOPEN_MODE_WRITE );
					l_def.AddFile( l_binName );
					l_def.AddFile( l_txtName );
					l_def.Deflate();
				}

				{
					Logger::LogInfo( "	Inflate the archive" );
					Path l_folder( cuT( "inflated" ) );

					if ( File::DirectoryExists( l_folder ) || File::DirectoryCreate( l_folder ) )
					{
						ZipArchive l_inf( l_zipName, File::eOPEN_MODE_READ );
						l_inf.Inflate( l_folder );

						String l_outTxtData;

						{
							Logger::LogInfo( "	Check binary file content" );
							BinaryFile l_binary( l_folder / l_binName, File::eOPEN_MODE_READ );
							std::vector< uint8_t > l_outBinData( size_t( l_binary.GetLength() ) );
							l_binary.ReadArray( l_outBinData.data(), l_outBinData.size() );
							CT_EQUAL( l_outBinData.size(), l_inBinData.size() );
							CT_CHECK( !std::memcmp( l_outBinData.data(), l_inBinData.data(), std::min( l_outBinData.size(), l_inBinData.size() ) ) );
						}

						{
							Logger::LogInfo( "	Check text file content" );
							TextFile l_text( l_folder / l_txtName, File::eOPEN_MODE_READ );
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
				Logger::LogError( "	Couldn't create second folder" );
			}
		}
		else
		{
			Logger::LogError( "	Couldn't create first folder" );
		}
	}
}
