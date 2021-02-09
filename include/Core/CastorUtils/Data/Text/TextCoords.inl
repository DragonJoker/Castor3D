namespace castor
{
	//*************************************************************************************************

	namespace details
	{
		template< typename ValueT, uint32_t CountT >
		struct CoordsTextT
		{
			static bool write( Coords< ValueT, CountT > const & object
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
	TextWriter< Coords< ValueT, 1u > >::TextWriter( String const & tabs )
		: TextWriterT< Coords< ValueT, 1u > >( tabs )
	{
	}

	template< typename ValueT >
	bool TextWriter< Coords< ValueT, 1u > >::operator()( Coords< ValueT, 1u > const & object, TextFile & file )
	{
		auto result = details::CoordsTextT< ValueT, 1u >::write( object, file );
		this->checkError( result, "Coords value" );
		return result;
	}

	//*************************************************************************************************

	template< typename ValueT >
	TextWriter< Coords< ValueT, 2u > >::TextWriter( String const & tabs )
		: TextWriterT< Coords< ValueT, 2u > >( tabs )
	{
	}

	template< typename ValueT >
	bool TextWriter< Coords< ValueT, 2u > >::operator()( Coords< ValueT, 2u > const & object, TextFile & file )
	{
		auto result = details::CoordsTextT< ValueT, 2u >::write( object, file );
		this->checkError( result, "Coords value" );
		return result;
	}

	//*************************************************************************************************

	template< typename ValueT >
	TextWriter< Coords< ValueT, 3u > >::TextWriter( String const & tabs )
		: TextWriterT< Coords< ValueT, 3u > >( tabs )
	{
	}

	template< typename ValueT >
	bool TextWriter< Coords< ValueT, 3u > >::operator()( Coords< ValueT, 3u > const & object, TextFile & file )
	{
		auto result = details::CoordsTextT< ValueT, 3u >::write( object, file );
		this->checkError( result, "Coords value" );
		return result;
	}

	//*************************************************************************************************

	template< typename ValueT >
	TextWriter< Coords< ValueT, 4u > >::TextWriter( String const & tabs )
		: TextWriterT< Coords< ValueT, 4u > >( tabs )
	{
	}

	template< typename ValueT >
	bool TextWriter< Coords< ValueT, 4u > >::operator()( Coords< ValueT, 4u > const & object, TextFile & file )
	{
		auto result = details::CoordsTextT< ValueT, 4u >::write( object, file );
		this->checkError( result, "Coords value" );
		return result;
	}

	//*************************************************************************************************
}
