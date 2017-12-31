namespace glsl
{
	inline Boolean::Boolean()
		: Type( cuT( "bool " ) )
	{
	}

	inline Boolean::Boolean( bool value )
		: Type( cuT( "bool " ) )
	{
		m_value << value;
	}

	inline Boolean::Boolean( GlslWriter * writer
		, castor::String const & name )
		: Type( cuT( "bool " ), writer, name )
	{
	}

	template< typename T >
	inline Boolean & Boolean::operator=( T const & rhs )
	{
		updateWriter( rhs );
		writeAssign( m_writer, *this, rhs );
		return *this;
	}

	inline Boolean & Boolean::operator=( int rhs )
	{
		writeAssign( m_writer, *this, rhs );
		return *this;
	}

	inline Boolean::operator uint32_t()
	{
		return 0u;
	}
}
