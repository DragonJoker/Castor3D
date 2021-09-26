namespace castor
{
	template< typename Value1T, typename Value2T >
	bool TextWriterBase::write( StringStream & file, String const & name, Value1T const & value1, Value2T const & value2 )const
	{
		auto result = writeText( file, tabs() + name + cuT( " " ) )
			&& TextWriter< Value1T >{ tabs() }( value1, file )
			&& writeText( file, cuT( " " ) )
			&& TextWriter< Value2T >{ tabs() }( value2, file )
			&& writeText( file, cuT( "\n" ) );
		checkError( result, name.c_str() );
		return result;
	}

	template< typename ValueT >
	bool TextWriterBase::write( StringStream & file, String const & name, castor::RangedValue< ValueT > const & value )const
	{
		return write( file, name, value.value() );
	}

	template< typename ValueT >
	bool TextWriterBase::write( StringStream & file, String const & name, castor::ChangeTracked< ValueT > const & value )const
	{
		return write( file, name, value.value() );
	}

	template< typename ValueT >
	bool TextWriterBase::writeOpt( StringStream & file, String const & name, ValueT const & value, ValueT const & comp )const
	{
		bool result{ true };

		if ( value != comp )
		{
			result = write( file, name, value );
		}

		return result;
	}

	template< typename ValueT, typename ... ParamsT >
	bool TextWriterBase::writeSub( StringStream & file, ValueT const & value, ParamsT const & ... params )const
	{
		auto result = TextWriter< ValueT >{ tabs(), params... }( value, file );
		checkError( result, cuT( "" ) );
		return result;
	}

	template< typename ValueT, typename ... ParamsT >
	bool TextWriterBase::writeNamedSub( StringStream & file, String const & name, ValueT const & value, ParamsT const & ... params )const
	{
		auto result = writeText( file, tabs() + name + cuT( " " ) )
			&& TextWriter< ValueT >{ tabs(), params... }( value, file )
			&& writeText( file, cuT( "\n" ) );
		checkError( result, name.c_str() );
		return result;
	}

	template< typename ValueT, typename ... ParamsT >
	bool TextWriterBase::writeSubOpt( StringStream & file, ValueT const & value, ValueT const & comp, ParamsT const & ... params )const
	{
		bool result{ true };

		if ( value != comp )
		{
			result = writeSub( file, value, params... );
		}

		return result;
	}

	template< typename ValueT, typename ... ParamsT >
	bool TextWriterBase::writeNamedSubOpt( StringStream & file, String const & name, ValueT const & value, ValueT const & comp, ParamsT const & ... params )const
	{
		bool result{ true };

		if ( value != comp )
		{
			result = writeNamedSub( file, name, value, params... );
		}

		return result;
	}
}
