#include "CastorUtils/Data/ZipArchive.hpp"

#include "CastorUtils/Log/Logger.hpp"
#include "CastorUtils/Data/BinaryFile.hpp"
#include "CastorUtils/Miscellaneous/Utils.hpp"

#ifdef WIN32
#	undef HAVE_UNISTD_H
#endif

#include "MiniZip/unzip.h"
#include "MiniZip/zip.h"

#ifdef WIN32
#	define USEWIN32IOAPI
#	include "MiniZip/iowin32.h"
#endif

#if defined( CU_PlatformWindows )
#	include <fcntl.h>
#	include <io.h>
#endif

namespace castor
{
	static const size_t CHUNK = 16384;

	//*********************************************************************************************

	namespace zlib
	{
		static std::string getError( int error )
		{
			//std::string error( zError( p_error ) );
			return "(code " + string::toString( error ) + ")";
		}

		struct ZipImpl
			: public ZipArchive::ZipImpl
		{
			ZipImpl()
				: m_unzip( nullptr )
				, m_zip( nullptr )
			{
			}

			~ZipImpl()override
			{
			}

			void open( Path const & path, File::OpenMode mode )override
			{
				if ( mode == File::OpenMode::eWrite )
				{
#ifdef USEWIN32IOAPI

					zlib_filefunc_def ffunc;
					fill_win32_filefunc( &ffunc );
					m_zip = zipOpen2( string::stringCast< char >( path ).c_str(), 0, NULL, &ffunc );

#else

					m_zip = zipOpen( string::stringCast< char >( path ).c_str(), 0 );

#endif

					if ( !m_zip )
					{
						CU_Exception( "Couldn't create archive file" );
					}
				}
				else
				{
#ifdef USEWIN32IOAPI

					zlib_filefunc_def ffunc;
					fill_win32_filefunc( &ffunc );
					m_unzip = unzOpen2( string::stringCast< char >( path ).c_str(), &ffunc );

#else

					m_unzip = unzOpen( string::stringCast< char >( path ).c_str() );

#endif

					if ( !m_unzip )
					{
						CU_Exception( "Couldn't open archive file: " + System::getLastErrorText() );
					}
				}
			}

			void close()override
			{
				if ( m_zip )
				{
					int error = zipClose( m_zip, nullptr );

					if ( error != UNZ_OK )
					{
						CU_Exception( "Error while closing ZIP archive : " + zlib::getError( error ) );
					}

					m_zip = nullptr;
				}

				if ( m_unzip )
				{
					int error = unzClose( m_unzip );

					if ( error != UNZ_OK )
					{
						CU_Exception( "Error while closing ZIP archive : " + zlib::getError( error ) );
					}

					m_unzip = nullptr;
				}
			}

			bool findFolder( String const & CU_UnusedParam( infolder ) )override
			{
				bool result = false;
				//std::string folder = string::stringCast< char >( infolder );

				////Search for the folder
				//struct zip_stat stat = { 0 };

				//if ( zip_stat( m_zip, folder.c_str(), 0, &stat ) == -1 )
				//{
				//	int zep, sep;
				//	zip_error_get( m_zip, &zep, &sep );
				//	std::string error = libzip::getError( zep ) + " - " + libzip::getError( sep );
				//	Logger::logWarning( "Couldn't retrieve ZIP archive folder [" + folder + "] informations : " + error );
				//}
				//else
				//{
				//	result = true;
				//}

				return result;
			}

			bool findFile( String const & CU_UnusedParam( infile ) )override
			{
				bool result = false;
				//std::string file = string::stringCast< char >( infile );

				////Search for the folder
				//struct zip_stat stat = { 0 };

				//if ( zip_stat( m_zip, file.c_str(), 0, &stat ) == -1 )
				//{
				//	int zep, sep;
				//	zip_error_get( m_zip, &zep, &sep );
				//	std::string error = libzip::getError( zep ) + " - " + libzip::getError( sep );
				//	Logger::logWarning( "Couldn't retrieve ZIP archive file [" + file + "] informations : " + error );
				//}
				//else
				//{
				//	result = true;
				//}

				return result;
			}

			void deflate( ZipArchive::Folder const & infolder )override
			{
				for ( auto folder : infolder.folders )
				{
					doDeflate( cuT( "" ), folder );
				}

				doDeflateFiles( cuT( "" ), infolder.files );
			}

			StringArray inflate( Path const & outFolder, ZipArchive::Folder & CU_UnusedParam( folder ) )override
			{
				if ( !File::directoryExists( outFolder ) )
				{
					File::directoryCreate( outFolder );
				}

				StringArray result;
				unz_global_info gi;

				auto error = unzGetGlobalInfo( m_unzip, &gi );

				if ( error != UNZ_OK )
				{
					CU_Exception( "Error in unzgetGlobalInfo: " + zlib::getError( error ) );
				}

				error = unzGoToFirstFile( m_unzip );

				if ( error != UNZ_OK )
				{
					CU_Exception( "Error in unzGoToFirstFile: " + zlib::getError( error ) );
				}

				for ( uLong i = 0; i < gi.number_entry; ++i )
				{
					doInflateCurrentFile( outFolder, result );

					if ( ( i + 1 ) < gi.number_entry )
					{
						error = unzGoToNextFile( m_unzip );

						if ( error != UNZ_OK )
						{
							CU_Exception( "Error in unzGoToNextFile: " + zlib::getError( error ) );
						}
					}
				}

				return result;
			}

		private:
			void doInflateCurrentFile( Path const & outFolder, StringArray & result )
			{
				std::array< char, 256 > fileNameInZip;
				unz_file_info fileInfo;
				auto error = unzGetCurrentFileInfo( m_unzip
					, &fileInfo
					, fileNameInZip.data()
					, sizeof( fileNameInZip )
					, nullptr
					, 0
					, nullptr
					, 0 );

				if ( error != UNZ_OK )
				{
					CU_Exception( "Error in unzgetCurrentFileInfo: " + zlib::getError( error ) );
				}

				Path name = Path{ string::stringCast< xchar >( fileNameInZip.data(), fileNameInZip.data() + fileInfo.size_filename ) };
				StringArray folders = string::split( name.getPath(), string::toString( Path::NativeSeparator ), 100, false );

				if ( !folders.empty() )
				{
					Path path = outFolder;

					if ( !File::directoryExists( path ) )
					{
						File::directoryCreate( path );
					}

					for ( StringArrayConstIt it = folders.begin(); it != folders.end(); ++it )
					{
						path /= ( *it );

						if ( !File::directoryExists( path ) )
						{
							File::directoryCreate( path );
						}
					}
				}

				auto last = fileNameInZip[fileInfo.size_filename - 1];

				if ( last != '/' && last != '\\' )
				{
					error = unzOpenCurrentFile( m_unzip );

					if ( error != UNZ_OK )
					{
						CU_Exception( "Error in unzOpenCurrentFilePassword: " + zlib::getError( error ) );
					}

					BinaryFile file( outFolder / name, File::OpenMode::eWrite );
					std::vector< uint8_t > buffer( CHUNK );

					try
					{
						do
						{
							error = unzReadCurrentFile( m_unzip
								, buffer.data()
								, static_cast< unsigned int >( buffer.size() ) );

							if ( error < 0 )
							{
								CU_Exception( "Error in unzReadCurrentFile: " + zlib::getError( error ) );
							}

							if ( error > 0 )
							{
								file.writeArray( buffer.data(), uint64_t( error ) );
							}
						}
						while ( error > 0 );

						unzCloseCurrentFile( m_unzip ); /* don't lose the error */
						result.push_back( outFolder / name );
					}
					catch ( Exception & )
					{
						error = unzCloseCurrentFile( m_unzip );

						if ( error != UNZ_OK )
						{
							CU_Exception( "Error in unzCloseCurrentFile: " + zlib::getError( error ) );
						}
					}
				}
			}

			virtual void doDeflate( String const & path, ZipArchive::Folder const & folder )
			{
				String filePath;

				if ( path.empty() )
				{
					filePath = folder.name;
				}
				else
				{
					filePath = path + cuT( "/" ) + folder.name;
				}

				for ( auto deffolder : folder.folders )
				{
					doDeflate( filePath, deffolder );
				}

				doDeflateFiles( filePath, folder.files );
			}

			virtual void doDeflateFiles( String const & path, std::list< String > const & files )
			{
				for ( auto name : files )
				{
					std::string filePath = string::stringCast< char >( path + cuT( "/" ) + name );

					if ( !File::fileExists( Path{ string::stringCast< xchar >( filePath ) } ) )
					{
						CU_Exception( "The file doesn't exist: " + filePath );
					}

					std::fstream file( filePath, std::ios::binary | std::ios::in );

					if ( !file.is_open() )
					{
						CU_Exception( "Couldn't open file: " + filePath );
					}

					file.seekg( 0, std::ios::end );
					auto size = size_t( file.tellg() );
					file.seekg( 0, std::ios::beg );

					std::vector< char > buffer( size );

					if ( size == 0 || file.read( buffer.data(), std::streamsize( size ) ) )
					{
						zip_fileinfo zfi{};

						if ( UNZ_OK == zipOpenNewFileInZip( m_zip, filePath.c_str(), &zfi, nullptr, 0, nullptr, 0, nullptr, Z_DEFLATED, Z_DEFAULT_COMPRESSION ) )
						{
							zipWriteInFileInZip( m_zip, size == 0 ? "" : buffer.data(), unsigned( size ) );
							zipCloseFileInZip( m_zip );
						}
					}
				}
			}

		private:
			unzFile m_unzip;
			zipFile m_zip;
		};
	}

	//*********************************************************************************************

	ZipArchive::Folder::Folder()
	{
	}

	ZipArchive::Folder::Folder( String const & p_name, Path const & p_path )
		: name( p_name )
	{
		addFile( p_path );
	}

	ZipArchive::Folder * ZipArchive::Folder::findFolder( Path const & p_path )
	{
		ZipArchive::Folder * result = nullptr;

		if ( name.empty() )
		{
			FolderList::iterator it = std::find_if( folders.begin(), folders.end(), [&]( Folder & p_folder )
			{
				return p_folder.findFolder( p_path );
			} );

			if ( it != folders.end() )
			{
				// The file is inside a subfolder
				Folder & folder = *it;
				result = &folder;
			}
		}
		else if ( p_path == name )
		{
			// The file path is this one
			result = this;
		}
		else if ( p_path.find( name + Path::NativeSeparator ) == 0 )
		{
			// The file is inside this folder or inside a subfolder
			Path path = Path{ p_path.substr( name.size() + 1 ) };

			FolderList::iterator it = std::find_if( folders.begin(), folders.end(), [&]( Folder & p_folder )
			{
				return p_folder.findFolder( path );
			} );

			if ( it != folders.end() )
			{
				// The file is inside a subfolder
				Folder & folder = *it;
				result = &folder;
			}
			else
			{
				// This folder is the farthest we can go
				result = this;
			}
		}

		return result;
	}

	void ZipArchive::Folder::addFile( Path const & p_path )
	{
		Path path = p_path.getPath();

		if ( path.empty() )
		{
			// File path name is just a file name, add it to current folder.
			std::list< String >::iterator it = std::find( files.begin(), files.end(), p_path );

			if ( it == files.end() )
			{
				files.push_back( p_path );
			}
		}
		else if ( name.empty() )
		{
			// The current folder is the root one, add each file path's folder, recursively.
			folders.push_back( Folder( p_path.substr( 0, p_path.find( Path::NativeSeparator ) ), p_path ) );
		}
		else
		{
			// Try to match file path's folders to this one
			if ( p_path.find( name + Path::NativeSeparator ) == 0 )
			{
				// First file folder is this one, complete this folder with the file's ones
				path = Path{ p_path.substr( name.size() + 1 ) };

				if ( path == p_path.getFileName() + cuT( "." ) + p_path.getExtension() )
				{
					files.push_back( path );
				}
				else
				{
					size_t found = path.find( Path::NativeSeparator );
					folders.push_back( Folder( path.substr( 0, found ), Path{ path.substr( found + 1 ) } ) );
				}
			}
			else
			{
				// This file is in a subfolder
				size_t found = p_path.find( Path::NativeSeparator );
				folders.push_back( Folder( p_path.substr( 0, found ), Path{ p_path.substr( found + 1 ) } ) );
			}
		}
	}

	void ZipArchive::Folder::removeFile( Path const & CU_UnusedParam( path ) )
	{
	}

	//*********************************************************************************************

	ZipArchive::ZipArchive( Path const & p_path, File::OpenMode p_mode )
		: m_impl( std::make_unique< zlib::ZipImpl >() )
	{
		m_impl->open( p_path, p_mode );
	}

	ZipArchive::~ZipArchive()
	{
		m_impl->close();
	}

	bool ZipArchive::deflate()const
	{
		bool result = false;

		if ( !m_uncompressed.files.empty() || !m_uncompressed.folders.empty() )
		{
			try
			{
				m_impl->deflate( m_uncompressed );
				result = true;
			}
			catch ( std::exception & exc )
			{
				Logger::logError( exc.what() );
			}
		}

		return result;
	}

	bool ZipArchive::inflate( Path const & p_folder )
	{
		bool result = false;

		try
		{
			m_rootFolder = p_folder;
			StringArray entries = m_impl->inflate( p_folder, m_uncompressed );

			for ( StringArray::iterator it = entries.begin(); it != entries.end(); ++it )
			{
				string::replace( *it, m_rootFolder + Path::NativeSeparator, String() );
				addFile( Path{ *it } );
			}

			result = true;
		}
		catch ( std::exception & exc )
		{
			Logger::logError( exc.what() );
		}

		return result;
	}

	void ZipArchive::addFile( Path const & p_fileName )
	{
		Path path = p_fileName.getPath();
		Folder * folder = m_uncompressed.findFolder( path );

		if ( folder )
		{
			folder->addFile( p_fileName );
		}
		else
		{
			m_uncompressed.addFile( p_fileName );
		}
	}

	void ZipArchive::removeFile( Path const & p_fileName )
	{
		Path path = p_fileName.getPath();
		Folder * folder = m_uncompressed.findFolder( path );

		if ( folder )
		{
			folder->removeFile( p_fileName );
		}
	}

	bool ZipArchive::findFolder( String const & p_folder )
	{
		return m_impl->findFolder( p_folder );
	}

	bool ZipArchive::findFile( String const & p_file )
	{
		return m_impl->findFile( p_file );
	}
}
