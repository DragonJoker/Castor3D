namespace GLSL
{
	//*****************************************************************************************

	inline IVec2::IVec2()
		: Type( cuT( "ivec2 " ) )
	{
	}

	inline IVec2::IVec2( GlslWriter * p_writer, Castor::String const & p_name )
		: Type( cuT( "ivec2 " ), p_writer, p_name )
	{
	}

	inline IVec2::~IVec2()
	{
	}

	inline IVec2 & IVec2::operator=( IVec2 const & p_rhs )
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
	inline IVec2 & IVec2::operator=( T const & p_rhs )
	{
		UpdateWriter( p_rhs );
		m_writer->WriteAssign( *this, p_rhs );
		return *this;
	}

	template< typename T >
	inline Int IVec2::operator[]( T const & p_rhs )
	{
		Int l_return{ m_writer, Castor::String( *this ) + cuT( "[" ) + Castor::String( p_rhs ) + cuT( "]" ) };
		return l_return;
	}

	inline Int IVec2::operator[]( int const & p_rhs )
	{
		Int l_return{ m_writer, Castor::String( *this ) + cuT( "[" ) + Castor::string::to_string( p_rhs ) + cuT( "]" ) };
		return l_return;
	}

	//*****************************************************************************************

	inline IVec3::IVec3()
		: Type( cuT( "ivec3 " ) )
	{
	}

	inline IVec3::IVec3( GlslWriter * p_writer, Castor::String const & p_name )
		: Type( cuT( "ivec3 " ), p_writer, p_name )
	{
	}

	inline IVec3::~IVec3()
	{
	}

	inline IVec3 & IVec3::operator=( IVec3 const & p_rhs )
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
	inline IVec3 & IVec3::operator=( T const & p_rhs )
	{
		UpdateWriter( p_rhs );
		m_writer->WriteAssign( *this, p_rhs );
		return *this;
	}

	template< typename T >
	inline Int IVec3::operator[]( T const & p_rhs )
	{
		Int l_return{ m_writer, Castor::String( *this ) + cuT( "[" ) + Castor::String( p_rhs ) + cuT( "]" ) };
		return l_return;
	}

	inline Int IVec3::operator[]( int const & p_rhs )
	{
		Int l_return{ m_writer, Castor::String( *this ) + cuT( "[" ) + Castor::string::to_string( p_rhs ) + cuT( "]" ) };
		return l_return;
	}

	//*****************************************************************************************

	inline IVec4::IVec4()
		: Type( cuT( "ivec4 " ) )
	{
	}

	inline IVec4::IVec4( GlslWriter * p_writer, Castor::String const & p_name )
		: Type( cuT( "ivec4 " ), p_writer, p_name )
	{
	}

	inline IVec4::~IVec4()
	{
	}

	inline IVec4 & IVec4::operator=( IVec4 const & p_rhs )
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
	inline IVec4 & IVec4::operator=( T const & p_rhs )
	{
		UpdateWriter( p_rhs );
		m_writer->WriteAssign( *this, p_rhs );
		return *this;
	}

	template< typename T >
	inline Int IVec4::operator[]( T const & p_rhs )
	{
		Int l_return{ m_writer, Castor::String( *this ) + cuT( "[" ) + Castor::String( p_rhs ) + cuT( "]" ) };
		return l_return;
	}

	inline Int IVec4::operator[]( int const & p_rhs )
	{
		Int l_return{ m_writer, Castor::String( *this ) + cuT( "[" ) + Castor::string::to_string( p_rhs ) + cuT( "]" ) };
		return l_return;
	}

	//*****************************************************************************************
}
