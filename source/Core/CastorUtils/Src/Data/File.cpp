#include "File.hpp"

#include "Miscellaneous/Utils.hpp"

namespace Castor
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

		if ( !CheckFlag( p_mode, OpenMode::eBinary ) )
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

		FOpen( m_file, string::string_cast< char >( m_fileFullPath ).c_str(), string::string_cast< char >( mode ).c_str() );

		if ( m_file )
		{
			m_length = 0;
			Castor::FSeek( m_file, 0, SEEK_END );
			m_length = Castor::FTell( m_file );
			Castor::FSeek( m_file, 0, SEEK_SET );
		}
		else
		{
			CASTOR_EXCEPTION( "Couldn't open file " + string::string_cast< char >( m_fileFullPath ) + " : " + string::string_cast< char >( System::GetLastErrorText() ) );
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

	int File::Seek( long long p_offset, OffsetMode p_origin )
	{
		CHECK_INVARIANTS();
		int iReturn = 0;

		if ( m_file )
		{
			switch ( p_origin )
			{
			case OffsetMode::eBeginning:
				iReturn = Castor::FSeek( m_file, p_offset, SEEK_SET );
				m_cursor = p_offset;
				break;

			case OffsetMode::eCurrent:
				iReturn = Castor::FSeek( m_file, p_offset, SEEK_CUR );
				m_cursor += p_offset;
				break;

			case OffsetMode::eEnd:
				iReturn = Castor::FSeek( m_file, p_offset, SEEK_END );
				m_cursor = GetLength() - p_offset;
				break;
			}
		}

		CHECK_INVARIANTS();
		return iReturn;
	}

	long long File::GetLength()
	{
		CHECK_INVARIANTS();
		m_length = 0;
		long long llPosition = Castor::FTell( m_file );
		Castor::FSeek( m_file, 0, SEEK_END );
		m_length = Castor::FTell( m_file );
		Castor::FSeek( m_file, llPosition, SEEK_SET );
		CHECK_INVARIANTS();
		return m_length;
	}

	bool File::IsOk()const
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

	long long File::Tell()
	{
		CHECK_INVARIANTS();
		long long llReturn = 0;

		if ( m_file )
		{
			llReturn = Castor::FTell( m_file );
		}

		CHECK_INVARIANTS();
		return llReturn;
	}

	BEGIN_INVARIANT_BLOCK( File )
	CHECK_INVARIANT( m_file );
	END_INVARIANT_BLOCK()

	uint64_t File::DoWrite( uint8_t const * p_buffer, uint64_t p_uiSize )
	{
		CHECK_INVARIANTS();
		REQUIRE( IsOk() && ( CheckFlag( m_mode, OpenMode::eWrite ) || CheckFlag( m_mode, OpenMode::eAppend ) ) );
		uint64_t uiReturn = 0;

		if ( IsOk() )
		{
			uiReturn = fwrite( p_buffer, 1, std::size_t( p_uiSize ), m_file );
			m_cursor += uiReturn;
			ENSURE( uiReturn <= p_uiSize );
		}

		CHECK_INVARIANTS();
		return uiReturn;
	}

	uint64_t File::DoRead( uint8_t * p_buffer, uint64_t p_uiSize )
	{
		CHECK_INVARIANTS();
		REQUIRE( IsOk() && CheckFlag( m_mode, OpenMode::eRead ) );
		uint64_t uiReturn = 0;
		uint64_t uiPrev = 1;

		if ( IsOk() )
		{
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

	bool File::FileExists( Path const & p_pathFile )
	{
		std::ifstream ifile( string::string_cast< char >( p_pathFile ).c_str() );
		return ifile.is_open();
	}

	bool File::DeleteFile( Path const & p_file )
	{
		bool result = false;

		if ( FileExists( p_file ) )
		{
			result = std::remove( string::string_cast< char >( p_file ).c_str() ) == 0;
		}
		else
		{
			result = true;
		}

		return result;
	}

	bool File::CopyFile( Path const & p_file, Path const & p_folder )
	{
		bool result = false;
		Path file{ p_folder / p_file.GetFileName() + cuT( "." ) + p_file.GetExtension() };
		std::ifstream src( string::string_cast< char >( p_file ), std::ios::binary );

		if ( src.is_open() )
		{
			std::ofstream dst( string::string_cast< char >( file ), std::ios::binary );

			if ( src.is_open() )
			{
				dst << src.rdbuf();
				result = true;
			}
			else
			{
				Logger::LogWarning( cuT( "CopyFile - Can't open destination file : " ) + p_file );
			}
		}
		else
		{
			Logger::LogWarning( cuT( "CopyFile - Can't open source file : " ) + p_file );
		}

		return result;
	}
}
