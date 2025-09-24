namespace c3d
{
	//*************************************************************************************************

	namespace details
	{
		template< typename ValueT, uint32_t CountT >
		struct PointViewTextT
		{
			static bool write( PointView< ValueT, CountT > const & object
				, StringStream & file )
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

				file << stream.str();
				return true;
			}
		};
	}

	//*************************************************************************************************

	template< typename ValueT >
	TextWriter< PointView< ValueT, 1u > >::TextWriter( String const & tabs )
		: TextWriterT< PointView< ValueT, 1u > >( tabs )
	{
	}

	template< typename ValueT >
	bool TextWriter< PointView< ValueT, 1u > >::operator()( PointView< ValueT, 1u > const & object, StringStream & file )
	{
		auto result = details::PointViewTextT< ValueT, 1u >::write( object, file );
		this->checkError( result, cuT( "PointView value" ) );
		return result;
	}

	//*************************************************************************************************

	template< typename ValueT >
	TextWriter< PointView< ValueT, 2u > >::TextWriter( String const & tabs )
		: TextWriterT< PointView< ValueT, 2u > >( tabs )
	{
	}

	template< typename ValueT >
	bool TextWriter< PointView< ValueT, 2u > >::operator()( PointView< ValueT, 2u > const & object, StringStream & file )
	{
		auto result = details::PointViewTextT< ValueT, 2u >::write( object, file );
		this->checkError( result, cuT( "PointView value" ) );
		return result;
	}

	//*************************************************************************************************

	template< typename ValueT >
	TextWriter< PointView< ValueT, 3u > >::TextWriter( String const & tabs )
		: TextWriterT< PointView< ValueT, 3u > >( tabs )
	{
	}

	template< typename ValueT >
	bool TextWriter< PointView< ValueT, 3u > >::operator()( PointView< ValueT, 3u > const & object, StringStream & file )
	{
		auto result = details::PointViewTextT< ValueT, 3u >::write( object, file );
		this->checkError( result, cuT( "PointView value" ) );
		return result;
	}

	//*************************************************************************************************

	template< typename ValueT >
	TextWriter< PointView< ValueT, 4u > >::TextWriter( String const & tabs )
		: TextWriterT< PointView< ValueT, 4u > >( tabs )
	{
	}

	template< typename ValueT >
	bool TextWriter< PointView< ValueT, 4u > >::operator()( PointView< ValueT, 4u > const & object, StringStream & file )
	{
		auto result = details::PointViewTextT< ValueT, 4u >::write( object, file );
		this->checkError( result, cuT( "PointView value" ) );
		return result;
	}

	//*************************************************************************************************
}
