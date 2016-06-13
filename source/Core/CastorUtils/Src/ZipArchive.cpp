#include "ZipArchive.hpp"

#include "Assertion.hpp"
#include "Logger.hpp"
#include "BinaryFile.hpp"

#include <zip.h>
#include <limits>

#if defined( MSDOS ) || defined( OS2 ) || defined( _WIN32 ) || defined( __CYGWIN__ )
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

	namespace libzip
	{
		std::string GetError( int p_error )
		{
			char l_buffer[1024] = { 0 };
			zip_error_to_str( l_buffer, 1024, errno, p_error );
			return "(code " + std::to_string( p_error ) + ") " + std::string( l_buffer );
		}

		struct ZipImpl
			: public ZipArchive::ZipImpl
		{
			ZipImpl()
				: m_zip( nullptr )
			{
			}

			virtual ~ZipImpl()
			{
			}

			virtual void Open( Path const & p_path, File::eOPEN_MODE p_mode )
			{
				int l_mode = 0;

				if ( ( p_mode & File::eOPEN_MODE_READ ) == File::eOPEN_MODE_READ )
				{
					l_mode |= ZIP_CHECKCONS;
				}

				if ( ( p_mode & File::eOPEN_MODE_WRITE ) == File::eOPEN_MODE_WRITE )
				{
					std::remove( string::string_cast< char >( p_path ).c_str() );
					l_mode |= ZIP_CREATE;
				}

				if ( ( p_mode & File::eOPEN_MODE_APPEND ) == File::eOPEN_MODE_APPEND )
				{
					l_mode |= ZIP_CREATE;
				}

				int l_result = 0;
				m_zip = zip_open( string::string_cast< char >( p_path ).c_str(), l_mode, &l_result );

				if ( !m_zip )
				{
					CASTOR_EXCEPTION( "Couldn't create archive file " + string::string_cast< char >( libzip::GetError( l_result ) ) );
				}
				else if ( l_result != ZIP_ER_OK )
				{
					CASTOR_EXCEPTION( "Couldn't create archive file " + string::string_cast< char >( libzip::GetError( l_result ) ) );
				}
			}

			virtual void Close()
			{
				if ( zip_close( m_zip ) == -1 )
				{
					int l_zep, l_sep;
					zip_error_get( m_zip, &l_zep, &l_sep );
					std::string l_error = libzip::GetError( l_zep ) + " - " + libzip::GetError( l_sep );
					CASTOR_EXCEPTION( "Error while closing ZIP archive : " + l_error );
				}

				m_zip = nullptr;
			}

			virtual bool FindFolder( String const & p_folder )
			{
				bool l_return = false;
				std::string l_folder = string::string_cast< char >( p_folder );

				//Search for the folder
				struct zip_stat l_stat = { 0 };

				if ( zip_stat( m_zip, l_folder.c_str(), 0, &l_stat ) == -1 )
				{
					int l_zep, l_sep;
					zip_error_get( m_zip, &l_zep, &l_sep );
					std::string l_error = libzip::GetError( l_zep ) + " - " + libzip::GetError( l_sep );
					Logger::LogWarning( "Couldn't retrieve ZIP archive folder [" + l_folder + "] informations : " + l_error );
				}
				else
				{
					l_return = true;
				}

				return l_return;
			}

			virtual bool FindFile( String const & p_file )
			{
				bool l_return = false;
				std::string l_file = string::string_cast< char >( p_file );

				//Search for the folder
				struct zip_stat l_stat = { 0 };

				if ( zip_stat( m_zip, l_file.c_str(), 0, &l_stat ) == -1 )
				{
					int l_zep, l_sep;
					zip_error_get( m_zip, &l_zep, &l_sep );
					std::string l_error = libzip::GetError( l_zep ) + " - " + libzip::GetError( l_sep );
					Logger::LogWarning( "Couldn't retrieve ZIP archive file [" + l_file + "] informations : " + l_error );
				}
				else
				{
					l_return = true;
				}

				return l_return;
			}

			virtual void Deflate( ZipArchive::Folder const & p_folder )
			{
				for ( ZipArchive::FolderList::const_iterator l_it = p_folder.folders.begin(); l_it != p_folder.folders.end(); ++l_it )
				{
					DoDeflate( cuT( "" ), *l_it );
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
				int l_count = zip_get_num_files( m_zip );

				if ( l_count == -1 )
				{
					CASTOR_EXCEPTION( "Couldn't retrieve ZIP archive file files count" );
				}

				for ( int64_t i = 0; i < l_count; ++i )
				{
					//Search for the file at given index
					struct zip_stat l_stat = { 0 };

					if ( zip_stat_index( m_zip, i, 0, &l_stat ) == -1 )
					{
						int l_zep, l_sep;
						zip_error_get( m_zip, &l_zep, &l_sep );
						std::string l_error = libzip::GetError( l_zep ) + " - " + libzip::GetError( l_sep );
						CASTOR_EXCEPTION( "Couldn't retrieve ZIP archive file informations : " + l_error );
					}

					if ( l_stat.size > 0 )
					{
						zip_file * l_zipfile = zip_fopen_index( m_zip, i, ZIP_FL_UNCHANGED );

						if ( !l_zipfile )
						{
							int l_zep, l_sep;
							zip_error_get( m_zip, &l_zep, &l_sep );
							std::string l_error = libzip::GetError( l_zep ) + " - " + libzip::GetError( l_sep );
							CASTOR_EXCEPTION( "Couldn't add directory to ZIP archive : " + l_error );
						}

						//Alloc memory for its uncompressed contents
						ByteArray l_contents( CHUNK );

						//Read the compressed file
						zip_uint64_t l_read = 0;
						Path l_name = Path{ string::string_cast< xchar >( l_stat.name ) };
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

						BinaryFile l_file( p_outFolder / l_name, File::eOPEN_MODE_WRITE );

						while ( l_read < l_stat.size )
						{
							zip_uint64_t l_tmp = zip_fread( l_zipfile, l_contents.data(), std::min( zip_uint64_t( CHUNK ), l_stat.size ) );

							if ( l_tmp >= 0 )
							{
								l_file.WriteArray( l_contents.data(), l_tmp );
								l_read += l_tmp;
							}
							else
							{
								int l_zep, l_sep;
								zip_error_get( m_zip, &l_zep, &l_sep );
								std::string l_error = libzip::GetError( l_zep ) + " - " + libzip::GetError( l_sep );
								zip_fclose( l_zipfile );
								CASTOR_EXCEPTION( "Couldn't read ZIP archive file " + string::string_cast< char >( l_name ) + " : " + l_error );
							}
						}

						l_result.push_back( p_outFolder / l_name );

						zip_fclose( l_zipfile );
					}
				}

				return l_result;
			}

		private:
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

				for ( ZipArchive::FolderList::const_iterator l_it = p_folder.folders.begin(); l_it != p_folder.folders.end(); ++l_it )
				{
					DoDeflate( l_path, *l_it );
				}

				DoDeflateFiles( l_path, p_folder.files );
			}

			virtual void DoDeflateFiles( String const & p_path, std::list< String > const & p_files )
			{
				for ( std::list< String >::const_iterator l_it = p_files.begin(); l_it != p_files.end(); ++l_it )
				{
					std::string l_file = string::string_cast< char >( p_path + cuT( "/" ) + ( *l_it ) );

					if ( !File::FileExists( Path{ string::string_cast< xchar >( l_file ) } ) )
					{
						CASTOR_EXCEPTION( "The file doesn't exist: " + l_file );
					}

					struct zip_source * l_source = zip_source_file( m_zip, l_file.c_str(), 0, 0 );

					if ( l_source )
					{
						if ( zip_add( m_zip, l_file.c_str(), l_source ) == -1 )
						{
							std::string l_error = zip_strerror( m_zip );
							CASTOR_EXCEPTION( "Couldn't add file to a ZIP archive: " + l_error );
						}

						struct zip_stat l_stat = { 0 };

						if ( zip_stat( m_zip, l_file.c_str(), 0, &l_stat ) == -1 )
						{
							std::string l_error = zip_strerror( m_zip );
							CASTOR_EXCEPTION( "Couldn't add file to a ZIP archive: " + l_error );
						}
						else
						{
							Logger::LogInfo( "Added file " + l_file );
						}
					}
					else
					{
						std::string l_error = zip_strerror( m_zip );
						CASTOR_EXCEPTION( "Couldn't write ZIP archive: " + l_error );
					}
				}
			}

		private:
			zip * m_zip;
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

	ZipArchive::ZipArchive( Path const & p_path, File::eOPEN_MODE p_mode )
		: m_impl( std::make_unique< libzip::ZipImpl >() )
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
