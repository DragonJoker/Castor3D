#include "File.hpp"

#include "Miscellaneous/Utils.hpp"

namespace castor
{
	File::File( Path const & p_fileName, FlagCombination< OpenMode > const & p_mode, EncodingMode p_encoding )
		: m_mode{ p_mode }
		, m_encoding{ p_encoding }
		, m_fileFullPath{ p_fileName }
	{
		REQUIRE( !p_fileName.empty() );
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
			FAILURE( "Unsupported file opening mode" );
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
			CASTOR_EXCEPTION( "Couldn't open file " + string::stringCast< char >( m_fileFullPath ) + " : " + string::stringCast< char >( System::getLastErrorText() ) );
		}

		CHECK_INVARIANTS();
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
		CHECK_INVARIANTS();
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
			}
		}

		CHECK_INVARIANTS();
		return iReturn;
	}

	long long File::getLength()
	{
		CHECK_INVARIANTS();
		m_length = 0;
		long long llPosition = castor::fileTell( m_file );
		castor::fileSeek( m_file, 0, SEEK_END );
		m_length = castor::fileTell( m_file );
		castor::fileSeek( m_file, llPosition, SEEK_SET );
		CHECK_INVARIANTS();
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
		CHECK_INVARIANTS();
		long long llReturn = 0;

		if ( m_file )
		{
			llReturn = castor::fileTell( m_file );
		}

		CHECK_INVARIANTS();
		return llReturn;
	}

	BEGIN_INVARIANT_BLOCK( File )
	CHECK_INVARIANT( m_file );
	END_INVARIANT_BLOCK()

	uint64_t File::doWrite( uint8_t const * p_buffer, uint64_t p_uiSize )
	{
		CHECK_INVARIANTS();
		REQUIRE( isOk() && ( checkFlag( m_mode, OpenMode::eWrite ) || checkFlag( m_mode, OpenMode::eAppend ) ) );
		uint64_t uiReturn = 0;

		if ( isOk() )
		{
			uiReturn = fwrite( p_buffer, 1, std::size_t( p_uiSize ), m_file );
			m_cursor += uiReturn;
			ENSURE( uiReturn <= p_uiSize );
		}

		CHECK_INVARIANTS();
		return uiReturn;
	}

	uint64_t File::doRead( uint8_t * p_buffer, uint64_t p_uiSize )
	{
		CHECK_INVARIANTS();
		REQUIRE( isOk() && checkFlag( m_mode, OpenMode::eRead ) );
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
			ENSURE( uiReturn <= p_uiSize );
		}

		CHECK_INVARIANTS();
		return uiReturn;
	}

	bool File::fileExists( Path const & p_pathFile )
	{
		std::ifstream ifile( string::stringCast< char >( p_pathFile ).c_str() );
		return ifile.is_open();
	}

	bool File::deleteFile( Path const & p_file )
	{
		bool result = false;

		if ( fileExists( p_file ) )
		{
			result = std::remove( string::stringCast< char >( p_file ).c_str() ) == 0;
		}
		else
		{
			result = true;
		}

		return result;
	}

	bool File::copyFile( Path const & p_file, Path const & p_folder )
	{
		bool result = false;
		Path file{ p_folder / p_file.getFileName() + cuT( "." ) + p_file.getExtension() };
		std::ifstream src( string::stringCast< char >( p_file ), std::ios::binary );

		if ( src.is_open() )
		{
			std::ofstream dst( string::stringCast< char >( file ), std::ios::binary );

			if ( src.is_open() )
			{
				dst << src.rdbuf();
				result = true;
			}
			else
			{
				Logger::logWarning( cuT( "copyFile - Can't open destination file : " ) + p_file );
			}
		}
		else
		{
			Logger::logWarning( cuT( "copyFile - Can't open source file : " ) + p_file );
		}

		return result;
	}
}
