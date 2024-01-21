#include "CastorUtils/Data/TextWriter.hpp"

#include "CastorUtils/Miscellaneous/Utils.hpp"

namespace castor
{
	//*********************************************************************************************

	namespace txtwrite
	{
		static String getFullName( String const & type
			, StringView name )
		{
			if ( !type.empty() && !name.empty() )
			{
				return type + cuT( " \"" ) + String{ name } + cuT( "\"" );
			}

			if ( !name.empty() )
			{
				return String{ name };
			}

			return cuEmptyString;
		}

		static bool writeRawText( StringStream & stream
			, String const & text )
		{
			stream << text;
			return true;
		}

		static bool writeBlockHeader( String const & tabs
			, String const & fullName
			, StringStream & file )
		{
			return ( ( !fullName.empty()
					? writeRawText( file, cuT( "\n" ) + tabs + fullName )
					: 1ULL ) > 0 )
				&& ( writeRawText( file, cuT( "\n" ) + tabs + cuT( "{\n" ) ) );
		}
	}

	//*********************************************************************************************

	TextWriterBase::WriterBlock::WriterBlock( TextWriterBase * writer
		, String const & type
		, StringView name
		, StringStream & file )
		: m_writer{ writer }
		, m_name{ txtwrite::getFullName( type, name ) }
		, m_file{ file }
		, m_result{ txtwrite::writeBlockHeader( writer->tabs(), m_name, file ) }
	{
		m_writer->m_indent++;
		m_writer->checkError( m_result, ( "header " + m_name ).c_str() );
	}

	TextWriterBase::WriterBlock::WriterBlock( TextWriterBase * writer
		, StringView name
		, StringStream & file )
		: WriterBlock{ writer, {}, name, file }
	{
	}

	TextWriterBase::WriterBlock::WriterBlock( TextWriterBase * writer
		, StringStream & file )
		: WriterBlock{ writer, {}, {}, file }
	{
	}

	TextWriterBase::WriterBlock::WriterBlock( WriterBlock && rhs )noexcept
		: m_writer{ rhs.m_writer }
		, m_name{ rhs.m_name }
		, m_file{ rhs.m_file }
		, m_result{ rhs.m_result }
	{
		rhs.m_writer = nullptr;
	}

	TextWriterBase::WriterBlock & TextWriterBase::WriterBlock::operator=( WriterBlock && rhs )noexcept
	{
		m_writer = rhs.m_writer;
		m_name = rhs.m_name;
		m_result = rhs.m_result;
		rhs.m_writer = nullptr;
		return *this;
	}

	TextWriterBase::WriterBlock::operator bool()const
	{
		return m_result;
	}

	TextWriterBase const * TextWriterBase::WriterBlock::operator->()const
	{
		return m_writer;
	}

	TextWriterBase::WriterBlock::~WriterBlock()noexcept
	{
		if ( m_writer )
		{
			m_writer->m_indent--;

			try
			{
				m_result = txtwrite::writeRawText( m_file, m_writer->tabs() + cuT( "}\n" ) );
			}
			catch ( ... )
			{
				// Nothing to do here...
			}

			m_writer->checkError( m_result, ( "footer " + m_name ).c_str() );
		}
	}

	//*********************************************************************************************

	TextWriterBase::TextWriterBase( String tabs
		, StringView name )
		: m_tabs{ std::move( tabs ) }
		, m_name{ name.empty() ? String{ name } : String{ name } + " - " }
	{
	}

	Path TextWriterBase::copyFile( Path const & path, Path const & folder, Path const & subfolder )
	{
		Path relative{ subfolder.empty() ? path.getFileName( true ) : subfolder / path.getFileName( true ) };

		if ( !File::directoryExists( folder / subfolder ) )
		{
			File::directoryCreate( folder / subfolder );
		}

		File::copyFile( path, folder / subfolder );
		return relative;
	}

	void TextWriterBase::checkError( bool error, xchar const * const action )const
	{
		if ( !error )
		{
			Logger::logError( makeStringStream() << m_name << action << " writing failed." );
		}
	}

	void TextWriterBase::checkError( bool error, String const & action )const
	{
		if ( !error )
		{
			Logger::logError( makeStringStream() << m_name << action << " writing failed." );
		}
	}

	void TextWriterBase::checkError( bool error, StringView const & action )const
	{
		if ( !error )
		{
			Logger::logError( makeStringStream() << m_name << action << " writing failed." );
		}
	}

	TextWriterBase::WriterBlock TextWriterBase::beginBlock( StringStream & file )
	{
		return WriterBlock{ this
			, file };
	}

	TextWriterBase::WriterBlock TextWriterBase::beginBlock( StringStream & file, StringView name )
	{
		return WriterBlock{ this
			, name
			, file };
	}

	TextWriterBase::WriterBlock TextWriterBase::beginBlock( StringStream & file, String const & type
		, StringView name )
	{
		return WriterBlock{ this
			, type
			, name
			, file };
	}

	bool TextWriterBase::writeMask( StringStream & file, StringView name, uint32_t mask )const
	{
		auto stream = makeStringStream();
		stream << cuT( "0x" ) << std::hex << std::setw( 8u ) << std::setfill( cuT( '0' ) ) << mask;
		auto result = txtwrite::writeRawText( file, tabs() + String{ name } + cuT( " " ) + stream.str() + cuT( "\n" ) );
		checkError( result, name );
		return result;
	}

	bool TextWriterBase::writeMask( StringStream & file, StringView name, uint64_t mask )const
	{
		auto stream = makeStringStream();
		stream << cuT( "0x" ) << std::hex << std::setw( 16u ) << std::setfill( cuT( '0' ) ) << mask;
		auto result = txtwrite::writeRawText( file, tabs() + String{ name } + cuT( " " ) + stream.str() + cuT( "\n" ) );
		checkError( result, name );
		return result;
	}

	bool TextWriterBase::writeComment( StringStream & file, String const & comment )const
	{
		auto result = txtwrite::writeRawText( file, tabs() + cuT( "// " ) + comment + cuT( "\n" ) );
		checkError( result, comment );
		return result;
	}

	bool TextWriterBase::write( StringStream & file, String const & value )const
	{
		auto result = txtwrite::writeRawText( file, tabs() + value + cuT( "\n" ) );
		checkError( result, value );
		return result;
	}

	bool TextWriterBase::write( StringStream & file, StringView name, float value )const
	{
		auto stream = makeStringStream();
		stream << std::showpoint << value;
		auto result = txtwrite::writeRawText( file, tabs() + String{ name } + cuT( " " ) + stream.str() + cuT( "\n" ) );
		checkError( result, name );
		return result;
	}

	bool TextWriterBase::write( StringStream & file, StringView name, double value )const
	{
		auto stream = makeStringStream();
		stream << std::showpoint << value;
		auto result = txtwrite::writeRawText( file, tabs() + String{ name } + cuT( " " ) + stream.str() + cuT( "\n" ) );
		checkError( result, name );
		return result;
	}

	bool TextWriterBase::write( StringStream & file, StringView name, uint16_t value )const
	{
		auto stream = makeStringStream();
		stream << value;
		auto result = txtwrite::writeRawText( file, tabs() + String{ name } + cuT( " " ) + stream.str() + cuT( "\n" ) );
		checkError( result, name );
		return result;
	}

	bool TextWriterBase::write( StringStream & file, StringView name, int16_t value )const
	{
		auto stream = makeStringStream();
		stream << value;
		auto result = txtwrite::writeRawText( file, tabs() + String{ name } + cuT( " " ) + stream.str() + cuT( "\n" ) );
		checkError( result, name );
		return result;
	}

	bool TextWriterBase::write( StringStream & file, StringView name, uint32_t value )const
	{
		auto stream = makeStringStream();
		stream << value;
		auto result = txtwrite::writeRawText( file, tabs() + String{ name } + cuT( " " ) + stream.str() + cuT( "\n" ) );
		checkError( result, name );
		return result;
	}

	bool TextWriterBase::write( StringStream & file, StringView name, int32_t value )const
	{
		auto stream = makeStringStream();
		stream << value;
		auto result = txtwrite::writeRawText( file, tabs() + String{ name } + cuT( " " ) + stream.str() + cuT( "\n" ) );
		checkError( result, name );
		return result;
	}

	bool TextWriterBase::write( StringStream & file, StringView name, uint64_t value )const
	{
		auto stream = makeStringStream();
		stream << value;
		auto result = txtwrite::writeRawText( file, tabs() + String{ name } + cuT( " " ) + stream.str() + cuT( "\n" ) );
		checkError( result, name );
		return result;
	}

	bool TextWriterBase::write( StringStream & file, StringView name, int64_t value )const
	{
		auto stream = makeStringStream();
		stream << value;
		auto result = txtwrite::writeRawText( file, tabs() + String{ name } + cuT( " " ) + stream.str() + cuT( "\n" ) );
		checkError( result, name );
		return result;
	}

	bool TextWriterBase::write( StringStream & file, StringView name, bool value )const
	{
		auto stream = makeStringStream();
		stream.setf( std::ios::boolalpha );
		stream << value;
		auto result = txtwrite::writeRawText( file, tabs() + String{ name } + cuT( " " ) + stream.str() + cuT( "\n" ) );
		checkError( result, name );
		return result;
	}

	bool TextWriterBase::write( StringStream & file, StringView name, String const & value )const
	{
		auto result = txtwrite::writeRawText( file, tabs() + String{ name } + cuT( " " ) + value + cuT( "\n" ) );
		checkError( result, name );
		return result;
	}

	bool TextWriterBase::writeText( StringStream & file, String const & value )const
	{
		return txtwrite::writeRawText( file, value );
	}

	bool TextWriterBase::writeOpt( StringStream & file, StringView name, bool value )const
	{
		return writeOpt( file, name, value, false );
	}

	bool TextWriterBase::writeName( StringStream & file, StringView name, String const & value )const
	{
		auto result = txtwrite::writeRawText( file, tabs() + String{ name } + cuT( " \"" ) + value + cuT( "\"\n" ) );
		checkError( result, name );
		return result;
	}

	bool TextWriterBase::writeNameOpt( StringStream & file, StringView name, String const & value, String const & comp )const
	{
		if ( value != comp )
		{
			return writeName( file, name, value );
		}

		return true;
	}

	bool TextWriterBase::writePath( StringStream & file, StringView name, Path const & value )const
	{
		auto result = txtwrite::writeRawText( file, tabs() + String{ name } + cuT( " \"" ) + value.toGeneric() + cuT( "\"\n" ) );
		checkError( result, name );
		return result;
	}

	bool TextWriterBase::writeFile( StringStream & file, StringView name, Path const & source, Path const & folder, String const & subfolder )const
	{
		Path relative{ copyFile( source, folder, Path{ subfolder } ) };
		return writePath( file, name, relative );
	}

	String TextWriterBase::tabs()const
	{
		return m_tabs + String( m_indent, '\t' );
	}

	//*********************************************************************************************

	TextWriter< int8_t >::TextWriter( String tabs )
		: TextWriterT< int8_t >{ std::move( tabs ) }
	{
	}

	bool TextWriter< int8_t >::operator()( int8_t const & value
		, StringStream & file )
	{
		auto result = txtwrite::writeRawText( file, string::toString( value ) );
		checkError( result, "" );
		return result;
	}

	//*********************************************************************************************

	TextWriter< uint8_t >::TextWriter( String tabs )
		: TextWriterT< uint8_t >{ std::move( tabs ) }
	{
	}

	bool TextWriter< uint8_t >::operator()( uint8_t const & value
		, StringStream & file )
	{
		auto result = txtwrite::writeRawText( file, string::toString( value ) );
		checkError( result, "" );
		return result;
	}

	//*********************************************************************************************

	TextWriter< int16_t >::TextWriter( String tabs )
		: TextWriterT< int16_t >{ std::move( tabs ) }
	{
	}

	bool TextWriter< int16_t >::operator()( int16_t const & value
		, StringStream & file )
	{
		auto result = txtwrite::writeRawText( file, string::toString( value ) );
		checkError( result, "" );
		return result;
	}

	//*********************************************************************************************

	TextWriter< uint16_t >::TextWriter( String tabs )
		: TextWriterT< uint16_t >{ std::move( tabs ) }
	{
	}

	bool TextWriter< uint16_t >::operator()( uint16_t const & value
		, StringStream & file )
	{
		auto result = txtwrite::writeRawText( file, string::toString( value ) );
		checkError( result, "" );
		return result;
	}

	//*********************************************************************************************

	TextWriter< int32_t >::TextWriter( String tabs )
		: TextWriterT< int32_t >{ std::move( tabs ) }
	{
	}

	bool TextWriter< int32_t >::operator()( int32_t const & value
		, StringStream & file )
	{
		auto result = txtwrite::writeRawText( file, string::toString( value ) );
		checkError( result, "" );
		return result;
	}

	//*********************************************************************************************

	TextWriter< uint32_t >::TextWriter( String tabs )
		: TextWriterT< uint32_t >{ std::move( tabs ) }
	{
	}

	bool TextWriter< uint32_t >::operator()( uint32_t const & value
		, StringStream & file )
	{
		auto result = txtwrite::writeRawText( file, string::toString( value ) );
		checkError( result, "" );
		return result;
	}

	//*********************************************************************************************

	TextWriter< int64_t >::TextWriter( String tabs )
		: TextWriterT< int64_t >{ std::move( tabs ) }
	{
	}

	bool TextWriter< int64_t >::operator()( int64_t const & value
		, StringStream & file )
	{
		auto result = txtwrite::writeRawText( file, string::toString( value ) );
		checkError( result, "" );
		return result;
	}

	//*********************************************************************************************

	TextWriter< uint64_t >::TextWriter( String tabs )
		: TextWriterT< uint64_t >{ std::move( tabs ) }
	{
	}

	bool TextWriter< uint64_t >::operator()( uint64_t const & value
		, StringStream & file )
	{
		auto result = txtwrite::writeRawText( file, string::toString( value ) );
		checkError( result, "" );
		return result;
	}

	//*********************************************************************************************

	TextWriter< float >::TextWriter( String tabs )
		: TextWriterT< float >{ std::move( tabs ) }
	{
	}

	bool TextWriter< float >::operator()( float const & value
		, StringStream & file )
	{
		auto stream = makeStringStream();
		stream << std::showpoint << value;
		auto result = txtwrite::writeRawText( file, stream.str() );
		checkError( result, stream.str() );
		return result;
	}

	//*********************************************************************************************

	TextWriter< double >::TextWriter( String tabs )
		: TextWriterT< double >{ std::move( tabs ) }
	{
	}

	bool TextWriter< double >::operator()( double const & value
		, StringStream & file )
	{
		auto stream = makeStringStream();
		stream << std::showpoint << value;
		auto result = txtwrite::writeRawText( file, stream.str() );
		checkError( result, stream.str() );
		return result;
	}

	//*********************************************************************************************

	TextWriter< String >::TextWriter( String tabs )
		: TextWriterT< String >{ std::move( tabs ) }
	{
	}

	bool TextWriter< String >::operator()( String const & value
		, StringStream & file )
	{
		auto result = txtwrite::writeRawText( file, value );
		checkError( result, value );
		return result;
	}

	//*********************************************************************************************
}
