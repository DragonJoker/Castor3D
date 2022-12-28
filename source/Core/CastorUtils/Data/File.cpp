#include "CastorUtils/Data/File.hpp"

#include "CastorUtils/Miscellaneous/Utils.hpp"

#include "CastorUtils/Log/Logger.hpp"

#include <filesystem>

namespace castor
{
	File::File( Path const & fileName
		, FlagCombination< OpenMode > const & modes
		, EncodingMode encoding )
		: m_mode{ modes }
		, m_encoding{ encoding }
		, m_fileFullPath{ fileName }
	{
		CU_Require( !fileName.empty() );
		String mode;

		switch ( modes )
		{
		case uint32_t( OpenMode::eRead ):
			mode = cuT( "r" );
			break;

		case uint32_t( OpenMode::eWrite ):
			mode = cuT( "w" );
			break;

		case uint32_t( OpenMode::eAppend ):
			mode = cuT( "a" );
			break;

		case OpenMode::eRead | OpenMode::eBinary:
			mode = cuT( "rb" );
			break;

		case OpenMode::eWrite | OpenMode::eBinary:
			mode = cuT( "wb" );
			break;

		case OpenMode::eAppend | OpenMode::eBinary:
			mode = cuT( "ab" );
			break;

		case OpenMode::eRead | OpenMode::eWrite:
			mode = cuT( "r+" );
			break;

		case OpenMode::eRead | OpenMode::eWrite | OpenMode::eAppend:
			mode = cuT( "a+" );
			break;

		default:
			CU_Failure( "Unsupported file opening mode" );
			break;
		}

		if ( !checkFlag( modes, OpenMode::eBinary ) )
		{
			switch ( encoding )
			{
			case EncodingMode::eAuto:
				m_encoding = EncodingMode::eUTF8;
				mode += cuT( ", ccs=UTF-8" );
				break;

			case EncodingMode::eUTF8:
				mode += cuT( ", ccs=UTF-8" );
				break;

			case EncodingMode::eUTF16:
				mode += cuT( ", ccs=UTF-16LE" );
				break;

			default:
				// Nothing to do here
				break;
			}
		}

		fileOpen( m_file
			, makePath( m_fileFullPath )
			, string::stringCast< char >( mode ).c_str() );

		if ( m_file )
		{
			m_length = 0;
			castor::fileSeek( m_file, 0, SEEK_END );
			m_length = uint64_t( castor::fileTell( m_file ) );
			castor::fileSeek( m_file, 0, SEEK_SET );
		}
		else
		{
			CU_Exception( "Couldn't open file " + string::stringCast< char >( m_fileFullPath ) + " : " + string::stringCast< char >( System::getLastErrorText() ) );
		}

		CU_CheckInvariants();
	}

	File::~File()
	{
		if ( m_file != nullptr )
		{
			fclose( m_file );
		}
	}

	int File::seek( long long offset, OffsetMode origin )
	{
		CU_CheckInvariants();
		int iReturn = 0;

		if ( m_file )
		{
			switch ( origin )
			{
			case OffsetMode::eBeginning:
				iReturn = castor::fileSeek( m_file, offset, SEEK_SET );
				m_cursor = uint64_t( offset );
				break;

			case OffsetMode::eCurrent:
				iReturn = castor::fileSeek( m_file, offset, SEEK_CUR );
				m_cursor += offset;
				break;

			case OffsetMode::eEnd:
				iReturn = castor::fileSeek( m_file, offset, SEEK_END );
				m_cursor = uint64_t( getLength() - offset );
				break;

			default:
				// Nothing to do here
				break;
			}
		}

		CU_CheckInvariants();
		return iReturn;
	}

	long long File::getLength()
	{
		CU_CheckInvariants();
		m_length = 0;
		long long llPosition = castor::fileTell( m_file );
		castor::fileSeek( m_file, 0, SEEK_END );
		m_length = uint64_t( castor::fileTell( m_file ) );
		castor::fileSeek( m_file, llPosition, SEEK_SET );
		CU_CheckInvariants();
		return int64_t( m_length );
	}

	bool File::isOk()const
	{
		bool result = false;

		if ( m_file )
		{
			if ( ferror( m_file ) == 0 )
			{
				if ( feof( m_file ) == 0 )
				{
					result = true;
				}
			}
		}

		return result;
	}

	long long File::tell()
	{
		CU_CheckInvariants();
		long long llReturn = 0;

		if ( m_file )
		{
			llReturn = castor::fileTell( m_file );
		}

		CU_CheckInvariants();
		return llReturn;
	}

	CU_BeginInvariantBlock( File )
	CU_CheckInvariant( m_file );
	CU_EndInvariantBlock()

	uint64_t File::doWrite( uint8_t const * buffer, uint64_t uiSize )
	{
		CU_CheckInvariants();
		CU_Require( isOk() && ( checkFlag( m_mode, OpenMode::eWrite ) || checkFlag( m_mode, OpenMode::eAppend ) ) );
		uint64_t uiReturn = 0;

		if ( isOk() )
		{
			uiReturn = fwrite( buffer, 1, size_t( uiSize ), m_file );
			m_cursor += uiReturn;
			CU_Ensure( uiReturn <= uiSize );
		}

		CU_CheckInvariants();
		return uiReturn;
	}

	uint64_t File::doRead( uint8_t * buffer, uint64_t uiSize )
	{
		CU_CheckInvariants();
		CU_Require( isOk() && checkFlag( m_mode, OpenMode::eRead ) );
		uint64_t uiReturn = 0;

		if ( isOk() )
		{
			uint64_t uiPrev = 1;

			while ( uiReturn < uiSize && uiPrev != uiReturn )
			{
				uiPrev = uiReturn;
				uiReturn += fread( buffer, 1, size_t( uiSize - uiReturn ), m_file );
			}

			m_cursor += uiReturn;
			CU_Ensure( uiReturn <= uiSize );
		}

		CU_CheckInvariants();
		return uiReturn;
	}

	bool File::directoryDelete( Path const & folder )
	{
		HitFileFunction fileFunction = []( Path const & pfolder
			, String const & pname )
		{
			File::deleteFile( pfolder / pname );
		};
		TraverseDirFunction directoryFunction;
		directoryFunction = [&fileFunction, &directoryFunction]( Path const & pfolder )
		{
			bool result = traverseDirectory( pfolder
				, directoryFunction
				, fileFunction );

			if ( result )
			{
				result = deleteEmptyDirectory( pfolder );
			}

			return result;
		};
		return directoryFunction( folder );
	}

	bool File::fileExists( Path const & fileName )
	{
		return std::filesystem::exists( makePath( fileName ) );
	}

	bool File::deleteFile( Path const & fileName )
	{
		bool result = false;

		if ( fileExists( fileName ) )
		{
			result = std::filesystem::remove( makePath( fileName ) ) == 0;
		}
		else
		{
			result = true;
		}

		return result;
	}

	bool File::copyFile( Path const & srcFileName
		, Path const & dstFolder
		, bool allowReplace )
	{
		return copyFileName( srcFileName
			, dstFolder / srcFileName.getFileName( true )
			, allowReplace );
	}

	bool File::copyFileName( Path const & srcFileName
		, Path const & dstFileName
		, bool allowReplace )
	{
		bool result = false;

		if ( fileExists( srcFileName ) )
		{
			std::filesystem::copy_options options{ ( allowReplace
				? std::filesystem::copy_options::update_existing
				: std::filesystem::copy_options::skip_existing ) };
			std::error_code error{};
			result = std::filesystem::copy_file( makePath( srcFileName )
				, makePath( dstFileName )
				, options
				, error );
			
			if ( !result )
			{
				Logger::logWarning( cuT( "copyFile - Can't copy [" ) + srcFileName + cuT( "] to [" ) + dstFileName + cuT( "]: " ) + error.message() );
			}
		}
		else
		{
			Logger::logWarning( cuT( "copyFile - Source file [" ) + srcFileName + cuT( "] doesn't exist." ) );
		}

		return result;
	}

	bool File::listDirectoryFiles( Path const & folderPath
		, PathArray & files
		, bool recursive )
	{
		files = filterDirectoryFiles( folderPath
			, []( Path const & CU_UnusedParam( folder )
				, String const & CU_UnusedParam( name ) )
			{
				return true;
			}
			, recursive );
		return true;
	}

	PathArray File::filterDirectoryFiles( Path const & folderPath
		, FilterFunction onFile
		, bool recursive )
	{
		PathArray files;
		HitFileFunction fileFunction = [&files, onFile]( Path const & folder
			, String const & name )
		{
			if ( onFile( folder, name ) )
			{
				files.push_back( folder / name );
			}

			return true;
		};
		TraverseDirFunction directoryFunction;

		if ( recursive )
		{
			directoryFunction = [&fileFunction, &directoryFunction]( Path const & path )
			{
				return traverseDirectory( path
					, directoryFunction
					, fileFunction );
			};
		}
		else
		{
			directoryFunction = []( Path const & CU_UnusedParam( path ) )
			{
				return true;
			};
		}

		traverseDirectory( folderPath
			, directoryFunction
			, fileFunction );
		return files;
	}
}
