namespace glsl
{
	gl_PerVertex::gl_PerVertex()
		: Type( cuT( "gl_PerVertex " ) )
	{
	}

	gl_PerVertex::gl_PerVertex( GlslWriter & writer, castor::String const & name )
		: Type( cuT( "gl_PerVertex " ), &writer, name.empty() ? name : name + cuT( "." ) )
	{
		Struct dummy{ writer, cuT( "gl_PerVertex" ) };
		dummy.declMember< Vec4 >( cuT( "gl_Position" ) );
		dummy.declMember< Float >( cuT( "gl_PointSize" ) );
		dummy.declMember< Float >( cuT( "gl_ClipDistance" ) );
		dummy.end();
	}

	gl_PerVertex & gl_PerVertex::operator=( gl_PerVertex const & rhs )
	{
		if ( m_writer )
		{
			m_writer->writeAssign( *this, rhs );
		}
		else
		{
			Type::operator=( rhs );
			m_writer = rhs.m_writer;
		}

		return *this;
	}

	template< typename T >
	gl_PerVertex & gl_PerVertex::operator=( T const & rhs )
	{
		updateWriter( rhs );
		m_writer->writeAssign( *this, rhs );
		return *this;
	}

	Vec4 gl_PerVertex::gl_Position()const
	{
		return Vec4( m_writer, m_name + cuT( "gl_Position" ) );
	}

	Float gl_PerVertex::gl_PointSize()const
	{
		return Float( m_writer, m_name + cuT( "gl_PointSize" ) );
	}

	Float gl_PerVertex::gl_ClipDistance()const
	{
		return Array< Float >( m_writer, m_name + cuT( "gl_ClipDistance" ), 8 );
	}
}
