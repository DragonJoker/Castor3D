#include "ZipArchive.hpp"

#include "Exception/Assertion.hpp"
#include "Log/Logger.hpp"
#include "BinaryFile.hpp"

#ifdef WIN32
#	undef HAVE_UNISTD_H
#endif

#include "MiniZip/unzip.h"
#include "MiniZip/zip.h"
#include <limits>

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
			//std::string l_error( zError( p_error ) );
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

					zlib_filefunc_def l_ffunc;
					fill_win32_filefunc( &l_ffunc );
					m_zip = zipOpen2( string::string_cast< char >( p_path ).c_str(), 0, NULL, &l_ffunc );

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

					zlib_filefunc_def l_ffunc;
					fill_win32_filefunc( &l_ffunc );
					m_unzip = unzOpen2( string::string_cast< char >( p_path ).c_str(), &l_ffunc );

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
					int l_error = zipClose( m_zip, nullptr );

					if ( l_error != UNZ_OK )
					{
						CASTOR_EXCEPTION( "Error while closing ZIP archive : " + zlib::GetError( l_error ) );
					}

					m_zip = nullptr;
				}

				if ( m_unzip )
				{
					int l_error = unzClose( m_unzip );

					if ( l_error != UNZ_OK )
					{
						CASTOR_EXCEPTION( "Error while closing ZIP archive : " + zlib::GetError( l_error ) );
					}

					m_unzip = nullptr;
				}
			}

			virtual bool FindFolder( String const & p_folder )
			{
				bool l_return = false;
				//std::string l_folder = string::string_cast< char >( p_folder );

				////Search for the folder
				//struct zip_stat l_stat = { 0 };

				//if ( zip_stat( m_zip, l_folder.c_str(), 0, &l_stat ) == -1 )
				//{
				//	int l_zep, l_sep;
				//	zip_error_get( m_zip, &l_zep, &l_sep );
				//	std::string l_error = libzip::GetError( l_zep ) + " - " + libzip::GetError( l_sep );
				//	Logger::LogWarning( "Couldn't retrieve ZIP archive folder [" + l_folder + "] informations : " + l_error );
				//}
				//else
				//{
				//	l_return = true;
				//}

				return l_return;
			}

			virtual bool FindFile( String const & p_file )
			{
				bool l_return = false;
				//std::string l_file = string::string_cast< char >( p_file );

				////Search for the folder
				//struct zip_stat l_stat = { 0 };

				//if ( zip_stat( m_zip, l_file.c_str(), 0, &l_stat ) == -1 )
				//{
				//	int l_zep, l_sep;
				//	zip_error_get( m_zip, &l_zep, &l_sep );
				//	std::string l_error = libzip::GetError( l_zep ) + " - " + libzip::GetError( l_sep );
				//	Logger::LogWarning( "Couldn't retrieve ZIP archive file [" + l_file + "] informations : " + l_error );
				//}
				//else
				//{
				//	l_return = true;
				//}

				return l_return;
			}

			virtual void Deflate( ZipArchive::Folder const & p_folder )
			{
				for ( auto l_folder : p_folder.folders )
				{
					DoDeflate( cuT( "" ), l_folder );
				}

				DoDeflateFiles( cuT( "" ), p_folder.files );
			}

			virtual StringArray Inflate( Path const & p_outFolder, ZipArchive::Folder & p_folder )
			{
				if ( !File::DirectoryExists( p_outFolder ) )
				{
					File::DirectoryCreate( p_outFolder );
				}

				StringArray l_result;
				unz_global_info l_gi;

				auto l_error = unzGetGlobalInfo( m_unzip, &l_gi );

				if ( l_error != UNZ_OK )
				{
					CASTOR_EXCEPTION( "Error in unzGetGlobalInfo: " + zlib::GetError( l_error ) );
				}

				l_error = unzGoToFirstFile( m_unzip );

				if ( l_error != UNZ_OK )
				{
					CASTOR_EXCEPTION( "Error in unzGoToFirstFile: " + zlib::GetError( l_error ) );
				}

				for ( uLong i = 0; i < l_gi.number_entry; ++i )
				{
					DoInflateCurrentFile( p_outFolder, l_result );

					if ( ( i + 1 ) < l_gi.number_entry )
					{
						l_error = unzGoToNextFile( m_unzip );

						if ( l_error != UNZ_OK )
						{
							CASTOR_EXCEPTION( "Error in unzGoToNextFile: " + zlib::GetError( l_error ) );
						}
					}
				}

				return l_result;
			}

		private:
			virtual void DoInflateCurrentFile( Path const & p_outFolder, StringArray & p_result )
			{
				std::array< char, 256 > l_fileNameInZip;
				unz_file_info l_fileInfo;
				auto l_error = unzGetCurrentFileInfo( m_unzip, &l_fileInfo, l_fileNameInZip.data(), sizeof( l_fileNameInZip ), NULL, 0, NULL, 0 );

				if ( l_error != UNZ_OK )
				{
					CASTOR_EXCEPTION( "Error in unzGetCurrentFileInfo: " + zlib::GetError( l_error ) );
				}

				Path l_name = Path{ string::string_cast< xchar >( l_fileNameInZip.data(), l_fileNameInZip.data() + l_fileInfo.size_filename ) };
				StringArray l_folders = string::split( l_name.GetPath(), string::to_string( Path::Separator ), 100, false );

				if ( !l_folders.empty() )
				{
					Path l_path = p_outFolder;

					if ( !File::DirectoryExists( l_path ) )
					{
						File::DirectoryCreate( l_path );
					}

					for ( StringArrayConstIt l_it = l_folders.begin(); l_it != l_folders.end(); ++l_it )
					{
						l_path /= ( *l_it );

						if ( !File::DirectoryExists( l_path ) )
						{
							File::DirectoryCreate( l_path );
						}
					}
				}

				auto l_last = l_fileNameInZip[l_fileInfo.size_filename - 1];

				if ( l_last != '/' && l_last != '\\' )
				{
					std::string l_outputFileName;
					int skip = 0;

					l_error = unzOpenCurrentFile( m_unzip );

					if ( l_error != UNZ_OK )
					{
						CASTOR_EXCEPTION( "Error in unzOpenCurrentFilePassword: " + zlib::GetError( l_error ) );
					}

					BinaryFile l_file( p_outFolder / l_name, File::OpenMode::eWrite );
					std::vector< uint8_t > l_buffer( CHUNK );

					try
					{
						do
						{
							l_error = unzReadCurrentFile( m_unzip, l_buffer.data(), static_cast< unsigned int >( l_buffer.size() ) );

							if ( l_error < 0 )
							{
								CASTOR_EXCEPTION( "Error in unzReadCurrentFile: " + zlib::GetError( l_error ) );
							}

							if ( l_error > 0 )
							{
								l_file.WriteArray( l_buffer.data(), l_error );
							}
						}
						while ( l_error > 0 );

						unzCloseCurrentFile( m_unzip ); /* don't lose the error */
						p_result.push_back( p_outFolder / l_name );
					}
					catch ( Exception & )
					{
						l_error = unzCloseCurrentFile( m_unzip );

						if ( l_error != UNZ_OK )
						{
							CASTOR_EXCEPTION( "Error in unzCloseCurrentFile: " + zlib::GetError( l_error ) );
						}
					}
				}
			}

			virtual void DoDeflate( String const & p_path, ZipArchive::Folder const & p_folder )
			{
				String l_path;

				if ( p_path.empty() )
				{
					l_path = p_folder.name;
				}
				else
				{
					l_path = p_path + cuT( "/" ) + p_folder.name;
				}

				for ( auto l_folder : p_folder.folders )
				{
					DoDeflate( l_path, l_folder );
				}

				DoDeflateFiles( l_path, p_folder.files );
			}

			virtual void DoDeflateFiles( String const & p_path, std::list< String > const & p_files )
			{
				for ( auto l_name : p_files )
				{
					std::string l_filePath = string::string_cast< char >( p_path + cuT( "/" ) + l_name );

					if ( !File::FileExists( Path{ string::string_cast< xchar >( l_filePath ) } ) )
					{
						CASTOR_EXCEPTION( "The file doesn't exist: " + l_filePath );
					}

					std::fstream l_file( l_filePath, std::ios::binary | std::ios::in );

					if ( !l_file.is_open() )
					{
						CASTOR_EXCEPTION( "Couldn't open file: " + l_filePath );
					}

					l_file.seekg( 0, std::ios::end );
					long l_size = long( l_file.tellg() );
					l_file.seekg( 0, std::ios::beg );

					std::vector< char > l_buffer( l_size );

					if ( l_size == 0 || l_file.read( l_buffer.data(), l_size ) )
					{
						zip_fileinfo l_zfi{ 0 };

						if ( UNZ_OK == zipOpenNewFileInZip( m_zip, l_filePath.c_str(), &l_zfi, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION ) )
						{
							zipWriteInFileInZip( m_zip, l_size == 0 ? "" : l_buffer.data(), l_size );
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
		ZipArchive::Folder * l_return = nullptr;

		if ( name.empty() )
		{
			FolderList::iterator l_it = std::find_if( folders.begin(), folders.end(), [&]( Folder & p_folder )
			{
				return p_folder.FindFolder( p_path );
			} );

			if ( l_it != folders.end() )
			{
				// The file is inside a subfolder
				Folder & l_folder = *l_it;
				l_return = &l_folder;
			}
		}
		else if ( p_path == name )
		{
			// The file path is this one
			l_return = this;
		}
		else if ( p_path.find( name + Path::Separator ) == 0 )
		{
			// The file is inside this folder or inside a subfolder
			Path l_path = Path{ p_path.substr( name.size() + 1 ) };

			FolderList::iterator l_it = std::find_if( folders.begin(), folders.end(), [&]( Folder & p_folder )
			{
				return p_folder.FindFolder( l_path );
			} );

			if ( l_it != folders.end() )
			{
				// The file is inside a subfolder
				Folder & l_folder = *l_it;
				l_return = &l_folder;
			}
			else
			{
				// This folder is the farthest we can go
				l_return = this;
			}
		}

		return l_return;
	}

	void ZipArchive::Folder::AddFile( Path const & p_path )
	{
		Path l_path = p_path.GetPath();

		if ( l_path.empty() )
		{
			// File path name is just a file name, add it to current folder.
			std::list< String >::iterator l_it = std::find( files.begin(), files.end(), p_path );

			if ( l_it == files.end() )
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
				l_path = Path{ p_path.substr( name.size() + 1 ) };

				if ( l_path == p_path.GetFileName() + cuT( "." ) + p_path.GetExtension() )
				{
					files.push_back( l_path );
				}
				else
				{
					size_t l_found = l_path.find( Path::Separator );
					folders.push_back( Folder( l_path.substr( 0, l_found ), Path{ l_path.substr( l_found + 1 ) } ) );
				}
			}
			else
			{
				// This file is in a subfolder
				size_t l_found = p_path.find( Path::Separator );
				folders.push_back( Folder( p_path.substr( 0, l_found ), Path{ p_path.substr( l_found + 1 ) } ) );
			}
		}
	}

	void ZipArchive::Folder::RemoveFile( Path const & p_path )
	{
		Path l_path = p_path.GetPath();
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
		bool l_return = false;

		if ( !m_uncompressed.files.empty() || !m_uncompressed.folders.empty() )
		{
			try
			{
				m_impl->Deflate( m_uncompressed );
				l_return = true;
			}
			catch ( std::exception & exc )
			{
				Logger::LogError( exc.what() );
			}
		}

		return l_return;
	}

	bool ZipArchive::Inflate( Path const & p_folder )
	{
		bool l_return = false;

		try
		{
			m_rootFolder = p_folder;
			StringArray l_entries = m_impl->Inflate( p_folder, m_uncompressed );

			for ( StringArray::iterator l_it = l_entries.begin(); l_it != l_entries.end(); ++l_it )
			{
				string::replace( *l_it, m_rootFolder + Path::Separator, String() );
				AddFile( Path{ *l_it } );
			}

			l_return = true;
		}
		catch ( std::exception & exc )
		{
			Logger::LogError( exc.what() );
		}

		return l_return;
	}

	void ZipArchive::AddFile( Path const & p_fileName )
	{
		Path l_path = p_fileName.GetPath();
		Folder * l_folder = m_uncompressed.FindFolder( l_path );

		if ( l_folder )
		{
			l_folder->AddFile( p_fileName );
		}
		else
		{
			m_uncompressed.AddFile( p_fileName );
		}
	}

	void ZipArchive::RemoveFile( Path const & p_fileName )
	{
		Path l_path = p_fileName.GetPath();
		Folder * l_folder = m_uncompressed.FindFolder( l_path );

		if ( l_folder )
		{
			l_folder->RemoveFile( p_fileName );
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
