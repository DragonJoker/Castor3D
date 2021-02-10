namespace castor
{
	template< typename ValueT >
	TextWriter< QuaternionT< ValueT > >::TextWriter( String const & tabs )
		: TextWriterT< QuaternionT< ValueT > >( tabs )
	{
	}

	template< typename ValueT >
	bool TextWriter< QuaternionT< ValueT > >::operator()( QuaternionT< ValueT > const & object, TextFile & file )
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
		bool result = file.print( 1024, cuT( "%s" ), stream.str().c_str() ) > 0;
		this->checkError( result, "Quaternion value" );
		return result;
	}
}
