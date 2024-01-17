#include "CastorUtils/Data/ZipArchive.hpp"

#include "CastorUtils/Log/Logger.hpp"
#include "CastorUtils/Data/BinaryFile.hpp"
#include "CastorUtils/Miscellaneous/Utils.hpp"

#ifdef WIN32
#	undef HAVE_UNISTD_H
#endif

#include <minizip/unzip.h>
#include <minizip/zip.h>

#ifdef WIN32
#	define USEWIN32IOAPI
#	include <minizip/iowin32.h>
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
			return "(code " + string::toString( error ) + ")";
		}

		struct ZipImpl
			: public ZipArchive::ZipImpl
		{
			ZipImpl() = default;

			void open( Path const & path, File::OpenMode mode )override
			{
				if ( mode == File::OpenMode::eWrite )
				{
#ifdef USEWIN32IOAPI

					zlib_filefunc_def ffunc;
					fill_win32_filefunc( &ffunc );
					m_zip = zipOpen2( string::stringCast< char >( path ).c_str(), 0, nullptr, &ffunc );

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
						CU_Exception( "Couldn't open archive file: " + system::getLastErrorText() );
					}
				}
			}

			void close()override
			{
				if ( m_zip )
				{
					if ( int error = zipClose( m_zip, nullptr );
						error != UNZ_OK )
					{
						CU_Exception( "Error while closing ZIP archive : " + zlib::getError( error ) );
					}

					m_zip = nullptr;
				}

				if ( m_unzip )
				{
					if ( int error = unzClose( m_unzip );
						error != UNZ_OK )
					{
						CU_Exception( "Error while closing ZIP archive : " + zlib::getError( error ) );
					}

					m_unzip = nullptr;
				}
			}

			bool findFolder( CU_UnusedParam( String const &, infolder ) )const override
			{
				bool result = false;
				return result;
			}

			bool findFile( CU_UnusedParam( String const &, infile ) )const override
			{
				bool result = false;
				return result;
			}

			void deflate( ZipArchive::Folder const & infolder )override
			{
				for ( auto const & folder : infolder.folders )
				{
					doDeflate( cuT( "" ), folder );
				}

				doDeflateFiles( cuT( "" ), infolder.files );
			}

			StringArray inflate( Path const & outFolder, CU_UnusedParam( ZipArchive::Folder &, folder ) )override
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

				Path name{ string::stringCast< xchar >( fileNameInZip.data(), fileNameInZip.data() + fileInfo.size_filename ) };

				if ( StringArray folders = string::split( name.getPath(), string::toString( Path::NativeSeparator ), 100, false );
					!folders.empty() )
				{
					Path path = outFolder;

					if ( !File::directoryExists( path ) )
					{
						File::directoryCreate( path );
					}

					for ( auto const & folder : folders )
					{
						path /= folder;

						if ( !File::directoryExists( path ) )
						{
							File::directoryCreate( path );
						}
					}
				}

				if ( auto last = fileNameInZip[fileInfo.size_filename - 1];
					last != '/' && last != '\\' )
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

				for ( auto const & deffolder : folder.folders )
				{
					doDeflate( filePath, deffolder );
				}

				doDeflateFiles( filePath, folder.files );
			}

			virtual void doDeflateFiles( String const & path, std::list< String > const & files )
			{
				for ( auto const & name : files )
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
			unzFile m_unzip{};
			zipFile m_zip{};
		};
	}

	//*********************************************************************************************

	ZipArchive::Folder::Folder( String const & name
		, Path const & path )
		: name( name )
	{
		addFile( path );
	}

	ZipArchive::Folder * ZipArchive::Folder::findFolder( Path const & path )
	{
		ZipArchive::Folder * result = nullptr;

		if ( name.empty() )
		{
			if ( auto it = std::find_if( folders.begin()
					, folders.end()
					, [&path]( Folder & folder )
					{
						return folder.findFolder( path );
					} );
				it != folders.end() )
			{
				// The file is inside a subfolder
				Folder & folder = *it;
				result = &folder;
			}
		}
		else if ( path == name )
		{
			// The file path is this one
			result = this;
		}
		else if ( path.find( name + Path::NativeSeparator ) == 0 )
		{
			// The file is inside this folder or inside a subfolder
			Path curpath{ path.substr( name.size() + 1 ) };

			if ( auto it = std::find_if( folders.begin()
					, folders.end()
					, [&curpath]( Folder & folder )
					{
						return folder.findFolder( curpath );
					} );
				it != folders.end() )
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

	void ZipArchive::Folder::addFile( Path const & path )
	{
		Path realPath = path.getPath();

		if ( realPath.empty() )
		{
			// File path name is just a file name, add it to current folder.
			if ( auto it = std::find( files.begin(), files.end(), path );
				it == files.end() )
			{
				files.push_back( path );
			}
		}
		else if ( name.empty() )
		{
			// The current folder is the root one, add each file path's folder, recursively.
			folders.emplace_back( path.substr( 0, path.find( Path::NativeSeparator ) ), path );
		}
		else
		{
			// Try to match file path's folders to this one
			if ( path.find( name + Path::NativeSeparator ) == 0 )
			{
				// First file folder is this one, complete this folder with the file's ones
				realPath = Path{ path.substr( name.size() + 1 ) };

				if ( realPath == path.getFileName() + cuT( "." ) + path.getExtension() )
				{
					files.emplace_back( realPath );
				}
				else
				{
					size_t found = realPath.find( Path::NativeSeparator );
					folders.emplace_back( realPath.substr( 0, found )
						, Path{ realPath.substr( found + 1 ) } );
				}
			}
			else
			{
				// This file is in a subfolder
				size_t found = path.find( Path::NativeSeparator );
				folders.emplace_back( path.substr( 0, found )
					, Path{ path.substr( found + 1 ) } );
			}
		}
	}

	void ZipArchive::Folder::removeFile( CU_UnusedParam( Path const &, path ) )
	{
	}

	//*********************************************************************************************

	ZipArchive::ZipArchive( Path const & path, File::OpenMode mode )
		: m_impl( std::make_unique< zlib::ZipImpl >() )
	{
		m_impl->open( path, mode );
	}

	ZipArchive::~ZipArchive()noexcept
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
			catch ( Exception & exc )
			{
				Logger::logError( exc.what() );
			}
		}

		return result;
	}

	bool ZipArchive::inflate( Path const & folder )
	{
		bool result = false;

		try
		{
			m_rootFolder = folder;

			for ( auto & entry : m_impl->inflate( folder, m_uncompressed ) )
			{
				string::replace( entry, m_rootFolder + Path::NativeSeparator, String() );
				addFile( Path{ entry } );
			}

			result = true;
		}
		catch ( Exception & exc )
		{
			Logger::logError( exc.what() );
		}

		return result;
	}

	void ZipArchive::addFile( Path const & fileName )
	{
		Path path = fileName.getPath();
		Folder * folder = m_uncompressed.findFolder( path );

		if ( folder )
		{
			folder->addFile( fileName );
		}
		else
		{
			m_uncompressed.addFile( fileName );
		}
	}

	void ZipArchive::removeFile( Path const & fileName )
	{
		Path path = fileName.getPath();
		Folder * folder = m_uncompressed.findFolder( path );

		if ( folder )
		{
			folder->removeFile( fileName );
		}
	}

	bool ZipArchive::findFolder( String const & folder )const
	{
		return m_impl->findFolder( folder );
	}

	bool ZipArchive::findFile( String const & file )const
	{
		return m_impl->findFile( file );
	}
}
