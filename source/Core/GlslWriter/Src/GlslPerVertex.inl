namespace glsl
{
	gl_PerVertex::gl_PerVertex()
		: Type( cuT( "gl_PerVertex " ) )
	{
	}

	gl_PerVertex::gl_PerVertex( GlslWriter * p_writer, castor::String const & p_name )
		: Type( cuT( "gl_PerVertex " ), p_writer, p_name )
	{
	}

	gl_PerVertex & gl_PerVertex::operator=( gl_PerVertex const & p_rhs )
	{
		if ( m_writer )
		{
			m_writer->writeAssign( *this, p_rhs );
		}
		else
		{
			Type::operator=( p_rhs );
			m_writer = p_rhs.m_writer;
		}

		return *this;
	}

	template< typename T >
	gl_PerVertex & gl_PerVertex::operator=( T const & p_rhs )
	{
		updateWriter( p_rhs );
		m_writer->writeAssign( *this, p_rhs );
		return *this;
	}

	Vec4 gl_PerVertex::gl_Position()const
	{
		return Vec4( m_writer, m_name + cuT( ".gl_Position" ) );
	}

	Float gl_PerVertex::gl_PointSize()const
	{
		return Float( m_writer, m_name + cuT( ".gl_PointSize" ) );
	}

	Float gl_PerVertex::gl_ClipDistance()const
	{
		return Array< Float >( m_writer, m_name + cuT( ".gl_ClipDistance" ), 8 );
	}
}
