#include "CastorUtils/Data/File.hpp"

#include "CastorUtils/Miscellaneous/Utils.hpp"

#include "CastorUtils/Log/Logger.hpp"

#include <filesystem>

namespace castor
{
	File::File( Path const & p_fileName, FlagCombination< OpenMode > const & p_mode, EncodingMode p_encoding )
		: m_mode{ p_mode }
		, m_encoding{ p_encoding }
		, m_fileFullPath{ p_fileName }
	{
		CU_Require( !p_fileName.empty() );
		String mode;

		switch ( p_mode )
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

		if ( !checkFlag( p_mode, OpenMode::eBinary ) )
		{
			switch ( p_encoding )
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

		fileOpen( m_file, string::stringCast< char >( m_fileFullPath ).c_str(), string::stringCast< char >( mode ).c_str() );

		if ( m_file )
		{
			m_length = 0;
			castor::fileSeek( m_file, 0, SEEK_END );
			m_length = castor::fileTell( m_file );
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

	int File::seek( long long p_offset, OffsetMode p_origin )
	{
		CU_CheckInvariants();
		int iReturn = 0;

		if ( m_file )
		{
			switch ( p_origin )
			{
			case OffsetMode::eBeginning:
				iReturn = castor::fileSeek( m_file, p_offset, SEEK_SET );
				m_cursor = p_offset;
				break;

			case OffsetMode::eCurrent:
				iReturn = castor::fileSeek( m_file, p_offset, SEEK_CUR );
				m_cursor += p_offset;
				break;

			case OffsetMode::eEnd:
				iReturn = castor::fileSeek( m_file, p_offset, SEEK_END );
				m_cursor = getLength() - p_offset;
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
		m_length = castor::fileTell( m_file );
		castor::fileSeek( m_file, llPosition, SEEK_SET );
		CU_CheckInvariants();
		return m_length;
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

	uint64_t File::doWrite( uint8_t const * p_buffer, uint64_t p_uiSize )
	{
		CU_CheckInvariants();
		CU_Require( isOk() && ( checkFlag( m_mode, OpenMode::eWrite ) || checkFlag( m_mode, OpenMode::eAppend ) ) );
		uint64_t uiReturn = 0;

		if ( isOk() )
		{
			uiReturn = fwrite( p_buffer, 1, std::size_t( p_uiSize ), m_file );
			m_cursor += uiReturn;
			CU_Ensure( uiReturn <= p_uiSize );
		}

		CU_CheckInvariants();
		return uiReturn;
	}

	uint64_t File::doRead( uint8_t * p_buffer, uint64_t p_uiSize )
	{
		CU_CheckInvariants();
		CU_Require( isOk() && checkFlag( m_mode, OpenMode::eRead ) );
		uint64_t uiReturn = 0;

		if ( isOk() )
		{
			uint64_t uiPrev = 1;

			while ( uiReturn < p_uiSize && uiPrev != uiReturn )
			{
				uiPrev = uiReturn;
				uiReturn += fread( p_buffer, 1, std::size_t( p_uiSize - uiReturn ), m_file );
			}

			m_cursor += uiReturn;
			CU_Ensure( uiReturn <= p_uiSize );
		}

		CU_CheckInvariants();
		return uiReturn;
	}

	bool File::directoryDelete( Path const & folder )
	{
		HitFileFunction fileFunction = []( Path const & folder
			, String const & name )
		{
			File::deleteFile( folder / name );
		};
		TraverseDirFunction directoryFunction;
		directoryFunction = [&fileFunction, &directoryFunction]( Path const & folder )
		{
			bool result = traverseDirectory( folder
				, directoryFunction
				, fileFunction );

			if ( result )
			{
				result = deleteEmptyDirectory( folder );
			}

			return result;
		};
		return directoryFunction( folder );
	}

	bool File::fileExists( Path const & fileName )
	{
		return std::filesystem::exists( string::stringCast< wchar_t >( fileName ) );
	}

	bool File::deleteFile( Path const & fileName )
	{
		bool result = false;

		if ( fileExists( fileName ) )
		{
			result = std::filesystem::remove( string::stringCast< wchar_t >( fileName ).c_str() ) == 0;
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
			result = std::filesystem::copy_file( string::stringCast< wchar_t >( srcFileName )
				, string::stringCast< wchar_t >( dstFileName )
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

	bool File::listDirectoryFiles( Path const & folderPath, PathArray & files, bool recursive )
	{
		files = filterDirectoryFiles( folderPath
			, []( Path const & folder
				, String const & name )
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
			directoryFunction = []( Path const & path )
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
