namespace castor
{
	//*************************************************************************************************

	namespace details
	{
		template< typename ValueT, uint32_t CountT >
		struct PointTextT
		{
			static bool write( Point< ValueT, CountT > const & object
				, TextFile & file )
			{
				StringStream stream{ makeStringStream() };
				stream.setf( std::ios::boolalpha );
				stream.setf( std::ios::showpoint );
				stream << object[0];

				for ( uint32_t i = 1; i < CountT; ++i )
				{
					stream << cuT( " " );
					stream << object[i];
				}

				return file.print( 1024, cuT( "%s" ), stream.str().c_str() ) > 0;
			}
		};
	}

	//*************************************************************************************************

	template< typename ValueT >
	TextWriter< Point< ValueT, 1u > >::TextWriter( String const & tabs )
		: TextWriterT< Point< ValueT, 1u > >( tabs )
	{
	}

	template< typename ValueT >
	bool TextWriter< Point< ValueT, 1u > >::operator()( Point< ValueT, 1u > const & object, TextFile & file )
	{
		auto result = details::PointTextT< ValueT, 1u >::write( object, file );
		this->checkError( result, "Point value" );
		return result;
	}

	//*************************************************************************************************

	template< typename ValueT >
	TextWriter< Point< ValueT, 2u > >::TextWriter( String const & tabs )
		: TextWriterT< Point< ValueT, 2u > >( tabs )
	{
	}

	template< typename ValueT >
	bool TextWriter< Point< ValueT, 2u > >::operator()( Point< ValueT, 2u > const & object, TextFile & file )
	{
		auto result = details::PointTextT< ValueT, 2u >::write( object, file );
		this->checkError( result, "Point value" );
		return result;
	}

	//*************************************************************************************************

	template< typename ValueT >
	TextWriter< Point< ValueT, 3u > >::TextWriter( String const & tabs )
		: TextWriterT< Point< ValueT, 3u > >( tabs )
	{
	}

	template< typename ValueT >
	bool TextWriter< Point< ValueT, 3u > >::operator()( Point< ValueT, 3u > const & object, TextFile & file )
	{
		auto result = details::PointTextT< ValueT, 3u >::write( object, file );
		this->checkError( result, "Point value" );
		return result;
	}

	//*************************************************************************************************

	template< typename ValueT >
	TextWriter< Point< ValueT, 4u > >::TextWriter( String const & tabs )
		: TextWriterT< Point< ValueT, 4u > >( tabs )
	{
	}

	template< typename ValueT >
	bool TextWriter< Point< ValueT, 4u > >::operator()( Point< ValueT, 4u > const & object, TextFile & file )
	{
		auto result = details::PointTextT< ValueT, 4u >::write( object, file );
		this->checkError( result, "Point value" );
		return result;
	}

	//*************************************************************************************************
}
