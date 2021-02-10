namespace castor
{
	template< typename ValueT >
	bool TextWriterBase::write( TextFile & file, String const & name, ValueT const & value )const
	{
		auto result = file.writeText( tabs() + name + cuT( " " ) ) > 0
			&& TextWriter< ValueT >{ tabs() }( value, file )
			&& file.writeText( cuT( "\n" ) ) > 0;
		checkError( result, name.c_str() );
		return result;
	}

	template< typename ValueT >
	bool TextWriterBase::write( TextFile & file, ValueT const & value )const
	{
		auto result = TextWriter< ValueT >{ tabs() }( value, file );
		checkError( result, cuT( "" ) );
		return result;
	}

	template< typename Value1T, typename Value2T >
	bool TextWriterBase::write( TextFile & file, String const & name, Value1T const & value1, Value2T const & value2 )const
	{
		auto result = file.writeText( tabs() + name + cuT( " " ) ) > 0
			&& TextWriter< Value1T >{ tabs() }( value1, file )
			&& file.writeText( cuT( " " ) ) > 0
			&& TextWriter< Value2T >{ tabs() }( value2, file )
			&& file.writeText( cuT( "\n" ) ) > 0;
		checkError( result, name.c_str() );
		return result;
	}

	template< typename ValueT >
	bool TextWriterBase::write( TextFile & file, String const & name, castor::RangedValue< ValueT > const & value )const
	{
		return write( file, name, value.value() );
	}

	template< typename ValueT >
	bool TextWriterBase::write( TextFile & file, String const & name, castor::ChangeTracked< ValueT > const & value )const
	{
		return write( file, name, value.value() );
	}

	template< typename ValueT >
	bool TextWriterBase::writeOpt( TextFile & file, String const & name, ValueT const & value, ValueT const & comp )const
	{
		bool result{ true };

		if ( value != comp )
		{
			result = write( file, name, value );
		}

		return result;
	}
}