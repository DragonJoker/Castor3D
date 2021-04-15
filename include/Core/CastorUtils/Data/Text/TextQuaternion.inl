namespace castor
{
	template< typename ValueT >
	TextWriter< QuaternionT< ValueT > >::TextWriter( String const & tabs )
		: TextWriterT< QuaternionT< ValueT > >( tabs )
	{
	}

	template< typename ValueT >
	bool TextWriter< QuaternionT< ValueT > >::operator()( QuaternionT< ValueT > const & object
		, StringStream & file )
	{
		StringStream stream{ castor::makeStringStream() };
		Point3< ValueT > axis;
		Angle angle;
		object.toAxisAngle( axis, angle );
		stream << axis[0];

		for ( uint32_t i = 1; i < 3; ++i )
		{
			stream << cuT( " " );
			stream << axis[i];
		}

		stream << cuT( " " ) << angle.degrees();
		bool result = writeText( file, stream.str() );
		this->checkError( result, "Quaternion value" );
		return result;
	}
}
