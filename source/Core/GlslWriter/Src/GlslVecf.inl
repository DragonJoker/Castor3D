namespace GLSL
{
	//*****************************************************************************************

	Vec2::Vec2()
		: Type( cuT( "vec2 " ) )
	{
	}

	Vec2::Vec2( GlslWriter * p_writer, Castor::String const & p_name )
		: Type( cuT( "vec2 " ), p_writer, p_name )
	{
	}

	Vec2::~Vec2()
	{
	}

	Vec2 & Vec2::operator=( Vec2 const & p_rhs )
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
	Vec2 & Vec2::operator=( T const & p_rhs )
	{
		UpdateWriter( p_rhs );
		m_writer->WriteAssign( *this, p_rhs );
		return *this;
	}

	template< typename T >
	Vec2 & Vec2::operator[]( T const & p_rhs )
	{
		m_value << Castor::String( *this ) << cuT( "[" ) << Castor::String( p_rhs ) << cuT( "]" );
		return *this;
	}

	//*****************************************************************************************

	Vec3::Vec3()
		: Type( cuT( "vec3 " ) )
	{
	}

	Vec3::Vec3( GlslWriter * p_writer, Castor::String const & p_name )
		: Type( cuT( "vec3 " ), p_writer, p_name )
	{
	}

	Vec3::~Vec3()
	{
	}

	Vec3 & Vec3::operator=( Vec3 const & p_rhs )
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
	Vec3 & Vec3::operator=( T const & p_rhs )
	{
		UpdateWriter( p_rhs );
		m_writer->WriteAssign( *this, p_rhs );
		return *this;
	}

	template< typename T >
	Vec3 & Vec3::operator[]( T const & p_rhs )
	{
		m_value << Castor::String( *this ) << cuT( "[" ) << Castor::String( p_rhs ) << cuT( "]" );
		return *this;
	}

	//*****************************************************************************************

	Vec4::Vec4()
		: Type( cuT( "vec4 " ) )
	{
	}

	Vec4::Vec4( GlslWriter * p_writer, Castor::String const & p_name )
		: Type( cuT( "vec4 " ), p_writer, p_name )
	{
	}

	Vec4::~Vec4()
	{
	}

	Vec4 & Vec4::operator=( Vec4 const & p_rhs )
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
	Vec4 & Vec4::operator=( T const & p_rhs )
	{
		UpdateWriter( p_rhs );
		m_writer->WriteAssign( *this, p_rhs );
		return *this;
	}

	template< typename T >
	Vec4 & Vec4::operator[]( T const & p_rhs )
	{
		m_value << Castor::String( *this ) << cuT( "[" ) << Castor::String( p_rhs ) << cuT( "]" );
		return *this;
	}

	//*****************************************************************************************
}
