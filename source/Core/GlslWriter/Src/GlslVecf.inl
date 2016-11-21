namespace GLSL
{
	//*****************************************************************************************

	inline Vec2::Vec2()
		: Type( cuT( "vec2 " ) )
	{
	}

	inline Vec2::Vec2( GlslWriter * p_writer, Castor::String const & p_name )
		: Type( cuT( "vec2 " ), p_writer, p_name )
	{
	}

	inline Vec2::~Vec2()
	{
	}

	inline Vec2 & Vec2::operator=( Vec2 const & p_rhs )
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
	inline Vec2 & Vec2::operator=( T const & p_rhs )
	{
		UpdateWriter( p_rhs );
		m_writer->WriteAssign( *this, p_rhs );
		return *this;
	}

	template< typename T >
	inline Float Vec2::operator[]( T const & p_rhs )
	{
		Float l_return{ m_writer, Castor::String( *this ) + cuT( "[" ) + Castor::String( p_rhs ) + cuT( "]" ) };
		return l_return;
	}

	inline Float Vec2::operator[]( int const & p_rhs )
	{
		Float l_return{ m_writer, Castor::String( *this ) + cuT( "[" ) + Castor::string::to_string( p_rhs ) + cuT( "]" ) };
		return l_return;
	}

	//*****************************************************************************************

	inline Vec3::Vec3()
		: Type( cuT( "vec3 " ) )
	{
	}

	inline Vec3::Vec3( GlslWriter * p_writer, Castor::String const & p_name )
		: Type( cuT( "vec3 " ), p_writer, p_name )
	{
	}

	inline Vec3::~Vec3()
	{
	}

	inline Vec3 & Vec3::operator=( Vec3 const & p_rhs )
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
	inline Vec3 & Vec3::operator=( T const & p_rhs )
	{
		UpdateWriter( p_rhs );
		m_writer->WriteAssign( *this, p_rhs );
		return *this;
	}

	template< typename T >
	inline Float Vec3::operator[]( T const & p_rhs )
	{
		Float l_return{ m_writer, Castor::String( *this ) + cuT( "[" ) + Castor::String( p_rhs ) + cuT( "]" ) };
		return l_return;
	}

	inline Float Vec3::operator[]( int const & p_rhs )
	{
		Float l_return{ m_writer, Castor::String( *this ) + cuT( "[" ) + Castor::string::to_string( p_rhs ) + cuT( "]" ) };
		return l_return;
	}

	//*****************************************************************************************

	inline Vec4::Vec4()
		: Type( cuT( "vec4 " ) )
	{
	}

	inline Vec4::Vec4( GlslWriter * p_writer, Castor::String const & p_name )
		: Type( cuT( "vec4 " ), p_writer, p_name )
	{
	}

	inline Vec4::~Vec4()
	{
	}

	inline Vec4 & Vec4::operator=( Vec4 const & p_rhs )
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
	inline Vec4 & Vec4::operator=( T const & p_rhs )
	{
		UpdateWriter( p_rhs );
		m_writer->WriteAssign( *this, p_rhs );
		return *this;
	}

	template< typename T >
	inline Float Vec4::operator[]( T const & p_rhs )
	{
		Float l_return{ m_writer, Castor::String( *this ) + cuT( "[" ) + Castor::String( p_rhs ) + cuT( "]" ) };
		return l_return;
	}

	inline Float Vec4::operator[]( int const & p_rhs )
	{
		Float l_return{ m_writer, Castor::String( *this ) + cuT( "[" ) + Castor::string::to_string( p_rhs ) + cuT( "]" ) };
		return l_return;
	}

	//*****************************************************************************************
}
