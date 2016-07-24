namespace GLSL
{
	//*****************************************************************************************

	Mat3::Mat3()
		: Type( cuT( "mat3 " ) )
	{
	}

	Mat3::Mat3( GlslWriter * p_writer, Castor::String const & p_name )
		: Type( cuT( "mat3 " ), p_writer, p_name )
	{
	}

	Mat3 & Mat3::operator=( Mat3 const & p_rhs )
	{
		if ( m_writer )
		{
			m_writer->WriteAssign( *this, p_rhs );
		}
		else
		{
			Type::operator=( p_rhs );
			m_writer = p_rhs.m_writer;
		}

		return *this;
	}

	template< typename T >
	Mat3 & Mat3::operator=( T const & p_rhs )
	{
		UpdateWriter( p_rhs );
		m_writer->WriteAssign( *this, p_rhs );
		return *this;
	}

	template< typename T >
	Mat3 & Mat3::operator[]( T const & p_rhs )
	{
		m_value << Castor::String( *this ) << cuT( "[" ) << Castor::String( p_rhs ) << cuT( "]" );
		return *this;
	}

	//*****************************************************************************************

	Mat4::Mat4()
		: Type( cuT( "mat4 " ) )
	{
	}

	Mat4::Mat4( GlslWriter * p_writer, Castor::String const & p_name )
		: Type( cuT( "mat4 " ), p_writer, p_name )
	{
	}

	Mat4 & Mat4::operator=( Mat4 const & p_rhs )
	{
		if ( m_writer )
		{
			m_writer->WriteAssign( *this, p_rhs );
		}
		else
		{
			Type::operator=( p_rhs );
			m_writer = p_rhs.m_writer;
		}

		return *this;
	}

	template< typename T >
	Mat4 & Mat4::operator=( T const & p_rhs )
	{
		UpdateWriter( p_rhs );
		m_writer->WriteAssign( *this, p_rhs );
		return *this;
	}

	template< typename T >
	Mat4 & Mat4::operator[]( T const & p_rhs )
	{
		m_value << Castor::String( *this ) << cuT( "[" ) << Castor::String( p_rhs ) << cuT( "]" );
		return *this;
	}

	//*****************************************************************************************
}
