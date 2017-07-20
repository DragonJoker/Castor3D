#include "ZipArchive.hpp"

#include "Log/Logger.hpp"
#include "BinaryFile.hpp"

#ifdef WIN32
#	undef HAVE_UNISTD_H
#endif

#include "MiniZip/unzip.h"
#include "MiniZip/zip.h"

#ifdef WIN32
#	define USEWIN32IOAPI
#	include "MiniZip/iowin32.h"
#endif

#if defined( CASTOR_PLATFORM_WINDOWS )
#	include <fcntl.h>
#	include <io.h>
#	define SET_BINARY_MODE( file ) setmode( fileno( file ), O_BINARY )
#else
#	define SET_BINARY_MODE(file)
#endif

namespace Castor
{
	static const size_t CHUNK = 16384;

	//*********************************************************************************************

	namespace zlib
	{
		std::string GetError( int p_error )
		{
			//std::string error( zError( p_error ) );
			return "(code " + string::to_string( p_error ) + ")";
		}

		struct ZipImpl
			: public ZipArchive::ZipImpl
		{
			ZipImpl()
				: m_unzip( nullptr )
				, m_zip( nullptr )
			{
			}

			virtual ~ZipImpl()
			{
			}

			virtual void Open( Path const & p_path, File::OpenMode p_mode )
			{
				if ( p_mode == File::OpenMode::eWrite )
				{
#ifdef USEWIN32IOAPI

					zlib_filefunc_def ffunc;
					fill_win32_filefunc( &ffunc );
					m_zip = zipOpen2( string::string_cast< char >( p_path ).c_str(), 0, NULL, &ffunc );

#else

					m_zip = zipOpen( string::string_cast< char >( p_path ).c_str(), 0 );

#endif

					if ( !m_zip )
					{
						CASTOR_EXCEPTION( "Couldn't create archive file" );
					}
				}
				else
				{
#ifdef USEWIN32IOAPI

					zlib_filefunc_def ffunc;
					fill_win32_filefunc( &ffunc );
					m_unzip = unzOpen2( string::string_cast< char >( p_path ).c_str(), &ffunc );

#else

					m_unzip = unzOpen( string::string_cast< char >( p_path ).c_str() );

#endif

					if ( !m_unzip )
					{
						CASTOR_EXCEPTION( "Couldn't open archive file" );
					}
				}
			}

			virtual void Close()
			{
				if ( m_zip )
				{
					int error = zipClose( m_zip, nullptr );

					if ( error != UNZ_OK )
					{
						CASTOR_EXCEPTION( "Error while closing ZIP archive : " + zlib::GetError( error ) );
					}

					m_zip = nullptr;
				}

				if ( m_unzip )
				{
					int error = unzClose( m_unzip );

					if ( error != UNZ_OK )
					{
						CASTOR_EXCEPTION( "Error while closing ZIP archive : " + zlib::GetError( error ) );
					}

					m_unzip = nullptr;
				}
			}

			virtual bool FindFolder( String const & p_folder )
			{
				bool result = false;
				//std::string folder = string::string_cast< char >( p_folder );

				////Search for the folder
				//struct zip_stat stat = { 0 };

				//if ( zip_stat( m_zip, folder.c_str(), 0, &stat ) == -1 )
				//{
				//	int zep, sep;
				//	zip_error_get( m_zip, &zep, &sep );
				//	std::string error = libzip::GetError( zep ) + " - " + libzip::GetError( sep );
				//	Logger::LogWarning( "Couldn't retrieve ZIP archive folder [" + folder + "] informations : " + error );
				//}
				//else
				//{
				//	result = true;
				//}

				return result;
			}

			virtual bool FindFile( String const & p_file )
			{
				bool result = false;
				//std::string file = string::string_cast< char >( p_file );

				////Search for the folder
				//struct zip_stat stat = { 0 };

				//if ( zip_stat( m_zip, file.c_str(), 0, &stat ) == -1 )
				//{
				//	int zep, sep;
				//	zip_error_get( m_zip, &zep, &sep );
				//	std::string error = libzip::GetError( zep ) + " - " + libzip::GetError( sep );
				//	Logger::LogWarning( "Couldn't retrieve ZIP archive file [" + file + "] informations : " + error );
				//}
				//else
				//{
				//	result = true;
				//}

				return result;
			}

			virtual void Deflate( ZipArchive::Folder const & p_folder )
			{
				for ( auto folder : p_folder.folders )
				{
					DoDeflate( cuT( "" ), folder );
				}

				DoDeflateFiles( cuT( "" ), p_folder.files );
			}

			virtual StringArray Inflate( Path const & p_outFolder, ZipArchive::Folder & p_folder )
			{
				if ( !File::DirectoryExists( p_outFolder ) )
				{
					File::DirectoryCreate( p_outFolder );
				}

				StringArray result;
				unz_global_info gi;

				auto error = unzGetGlobalInfo( m_unzip, &gi );

				if ( error != UNZ_OK )
				{
					CASTOR_EXCEPTION( "Error in unzGetGlobalInfo: " + zlib::GetError( error ) );
				}

				error = unzGoToFirstFile( m_unzip );

				if ( error != UNZ_OK )
				{
					CASTOR_EXCEPTION( "Error in unzGoToFirstFile: " + zlib::GetError( error ) );
				}

				for ( uLong i = 0; i < gi.number_entry; ++i )
				{
					DoInflateCurrentFile( p_outFolder, result );

					if ( ( i + 1 ) < gi.number_entry )
					{
						error = unzGoToNextFile( m_unzip );

						if ( error != UNZ_OK )
						{
							CASTOR_EXCEPTION( "Error in unzGoToNextFile: " + zlib::GetError( error ) );
						}
					}
				}

				return result;
			}

		private:
			virtual void DoInflateCurrentFile( Path const & p_outFolder, StringArray & p_result )
			{
				std::array< char, 256 > fileNameInZip;
				unz_file_info fileInfo;
				auto error = unzGetCurrentFileInfo( m_unzip, &fileInfo, fileNameInZip.data(), sizeof( fileNameInZip ), NULL, 0, NULL, 0 );

				if ( error != UNZ_OK )
				{
					CASTOR_EXCEPTION( "Error in unzGetCurrentFileInfo: " + zlib::GetError( error ) );
				}

				Path name = Path{ string::string_cast< xchar >( fileNameInZip.data(), fileNameInZip.data() + fileInfo.size_filename ) };
				StringArray folders = string::split( name.GetPath(), string::to_string( Path::Separator ), 100, false );

				if ( !folders.empty() )
				{
					Path path = p_outFolder;

					if ( !File::DirectoryExists( path ) )
					{
						File::DirectoryCreate( path );
					}

					for ( StringArrayConstIt it = folders.begin(); it != folders.end(); ++it )
					{
						path /= ( *it );

						if ( !File::DirectoryExists( path ) )
						{
							File::DirectoryCreate( path );
						}
					}
				}

				auto last = fileNameInZip[fileInfo.size_filename - 1];

				if ( last != '/' && last != '\\' )
				{
					std::string outputFileName;
					int skip = 0;

					error = unzOpenCurrentFile( m_unzip );

					if ( error != UNZ_OK )
					{
						CASTOR_EXCEPTION( "Error in unzOpenCurrentFilePassword: " + zlib::GetError( error ) );
					}

					BinaryFile file( p_outFolder / name, File::OpenMode::eWrite );
					std::vector< uint8_t > buffer( CHUNK );

					try
					{
						do
						{
							error = unzReadCurrentFile( m_unzip, buffer.data(), static_cast< unsigned int >( buffer.size() ) );

							if ( error < 0 )
							{
								CASTOR_EXCEPTION( "Error in unzReadCurrentFile: " + zlib::GetError( error ) );
							}

							if ( error > 0 )
							{
								file.WriteArray( buffer.data(), error );
							}
						}
						while ( error > 0 );

						unzCloseCurrentFile( m_unzip ); /* don't lose the error */
						p_result.push_back( p_outFolder / name );
					}
					catch ( Exception & )
					{
						error = unzCloseCurrentFile( m_unzip );

						if ( error != UNZ_OK )
						{
							CASTOR_EXCEPTION( "Error in unzCloseCurrentFile: " + zlib::GetError( error ) );
						}
					}
				}
			}

			virtual void DoDeflate( String const & p_path, ZipArchive::Folder const & p_folder )
			{
				String path;

				if ( p_path.empty() )
				{
					path = p_folder.name;
				}
				else
				{
					path = p_path + cuT( "/" ) + p_folder.name;
				}

				for ( auto folder : p_folder.folders )
				{
					DoDeflate( path, folder );
				}

				DoDeflateFiles( path, p_folder.files );
			}

			virtual void DoDeflateFiles( String const & p_path, std::list< String > const & p_files )
			{
				for ( auto name : p_files )
				{
					std::string filePath = string::string_cast< char >( p_path + cuT( "/" ) + name );

					if ( !File::FileExists( Path{ string::string_cast< xchar >( filePath ) } ) )
					{
						CASTOR_EXCEPTION( "The file doesn't exist: " + filePath );
					}

					std::fstream file( filePath, std::ios::binary | std::ios::in );

					if ( !file.is_open() )
					{
						CASTOR_EXCEPTION( "Couldn't open file: " + filePath );
					}

					file.seekg( 0, std::ios::end );
					long size = long( file.tellg() );
					file.seekg( 0, std::ios::beg );

					std::vector< char > buffer( size );

					if ( size == 0 || file.read( buffer.data(), size ) )
					{
						zip_fileinfo zfi{ 0 };

						if ( UNZ_OK == zipOpenNewFileInZip( m_zip, filePath.c_str(), &zfi, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION ) )
						{
							zipWriteInFileInZip( m_zip, size == 0 ? "" : buffer.data(), size );
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
		AddFile( p_path );
	}

	ZipArchive::Folder * ZipArchive::Folder::FindFolder( Path const & p_path )
	{
		ZipArchive::Folder * result = nullptr;

		if ( name.empty() )
		{
			FolderList::iterator it = std::find_if( folders.begin(), folders.end(), [&]( Folder & p_folder )
			{
				return p_folder.FindFolder( p_path );
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
		else if ( p_path.find( name + Path::Separator ) == 0 )
		{
			// The file is inside this folder or inside a subfolder
			Path path = Path{ p_path.substr( name.size() + 1 ) };

			FolderList::iterator it = std::find_if( folders.begin(), folders.end(), [&]( Folder & p_folder )
			{
				return p_folder.FindFolder( path );
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

	void ZipArchive::Folder::AddFile( Path const & p_path )
	{
		Path path = p_path.GetPath();

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
			folders.push_back( Folder( p_path.substr( 0, p_path.find( Path::Separator ) ), p_path ) );
		}
		else
		{
			// Try to match file path's folders to this one
			if ( p_path.find( name + Path::Separator ) == 0 )
			{
				// First file folder is this one, complete this folder with the file's ones
				path = Path{ p_path.substr( name.size() + 1 ) };

				if ( path == p_path.GetFileName() + cuT( "." ) + p_path.GetExtension() )
				{
					files.push_back( path );
				}
				else
				{
					size_t found = path.find( Path::Separator );
					folders.push_back( Folder( path.substr( 0, found ), Path{ path.substr( found + 1 ) } ) );
				}
			}
			else
			{
				// This file is in a subfolder
				size_t found = p_path.find( Path::Separator );
				folders.push_back( Folder( p_path.substr( 0, found ), Path{ p_path.substr( found + 1 ) } ) );
			}
		}
	}

	void ZipArchive::Folder::RemoveFile( Path const & p_path )
	{
		Path path = p_path.GetPath();
	}

	//*********************************************************************************************

	ZipArchive::ZipArchive( Path const & p_path, File::OpenMode p_mode )
		: m_impl( std::make_unique< zlib::ZipImpl >() )
	{
		m_impl->Open( p_path, p_mode );
	}

	ZipArchive::~ZipArchive()
	{
		m_impl->Close();
	}

	bool ZipArchive::Deflate()const
	{
		bool result = false;

		if ( !m_uncompressed.files.empty() || !m_uncompressed.folders.empty() )
		{
			try
			{
				m_impl->Deflate( m_uncompressed );
				result = true;
			}
			catch ( std::exception & exc )
			{
				Logger::LogError( exc.what() );
			}
		}

		return result;
	}

	bool ZipArchive::Inflate( Path const & p_folder )
	{
		bool result = false;

		try
		{
			m_rootFolder = p_folder;
			StringArray entries = m_impl->Inflate( p_folder, m_uncompressed );

			for ( StringArray::iterator it = entries.begin(); it != entries.end(); ++it )
			{
				string::replace( *it, m_rootFolder + Path::Separator, String() );
				AddFile( Path{ *it } );
			}

			result = true;
		}
		catch ( std::exception & exc )
		{
			Logger::LogError( exc.what() );
		}

		return result;
	}

	void ZipArchive::AddFile( Path const & p_fileName )
	{
		Path path = p_fileName.GetPath();
		Folder * folder = m_uncompressed.FindFolder( path );

		if ( folder )
		{
			folder->AddFile( p_fileName );
		}
		else
		{
			m_uncompressed.AddFile( p_fileName );
		}
	}

	void ZipArchive::RemoveFile( Path const & p_fileName )
	{
		Path path = p_fileName.GetPath();
		Folder * folder = m_uncompressed.FindFolder( path );

		if ( folder )
		{
			folder->RemoveFile( p_fileName );
		}
	}

	bool ZipArchive::FindFolder( String const & p_folder )
	{
		return m_impl->FindFolder( p_folder );
	}

	bool ZipArchive::FindFile( String const & p_file )
	{
		return m_impl->FindFile( p_file );
	}
}
