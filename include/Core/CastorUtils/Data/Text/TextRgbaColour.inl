namespace castor
{
	template< typename ComponentT >
	TextWriter< RgbaColourT< ComponentT > >::TextWriter( String const & tabs )
		: TextWriterT< RgbaColourT< ComponentT > >( tabs )
	{
	}

	template< typename ComponentT >
	bool TextWriter< RgbaColourT< ComponentT > >::operator()( RgbaColourT< ComponentT > const & colour
		, StringStream & file )
	{
		StringStream stream{ castor::makeStringStream() };
		stream.setf( std::ios::boolalpha );
		stream.setf( std::ios::showpoint );

		for ( auto component : colour )
		{
			if ( !stream.str().empty() )
			{
				stream << cuT( " " );
			}

			stream << component.value();
		}

		bool result = writeText( file, stream.str() );
		this->checkError( result, "RgbaColourT value" );
		return result;
	}
}
