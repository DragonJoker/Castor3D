namespace castor
{
	template< typename ComponentT >
	TextWriter< RgbColourT< ComponentT > >::TextWriter( String const & tabs )
		: TextWriterT< RgbColourT< ComponentT > >( tabs )
	{
	}

	template< typename ComponentT >
	bool TextWriter< RgbColourT< ComponentT > >::operator()( RgbColourT< ComponentT > const & colour
		, StringStream & file )
	{
		StringStream stream{ castor::makeStringStream() };
		stream.setf( std::ios::boolalpha );
		stream.setf( std::ios::showpoint );

		for ( auto & component : colour )
		{
			if ( !stream.str().empty() )
			{
				stream << cuT( " " );
			}

			stream << component.value();
		}

		bool result = this->writeText( file, stream.str() );
		this->checkError( result, "RgbColourT value" );
		return result;
	}
}
