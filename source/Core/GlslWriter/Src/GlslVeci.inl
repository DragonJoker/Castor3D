namespace GLSL
{
	//*****************************************************************************************

	IVec2::IVec2()
		: Type( cuT( "ivec2 " ) )
	{
	}

	IVec2::IVec2( GlslWriter * p_writer, Castor::String const & p_name )
		: Type( cuT( "ivec2 " ), p_writer, p_name )
	{
	}

	IVec2::~IVec2()
	{
	}

	IVec2 & IVec2::operator=( IVec2 const & p_rhs )
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
	inline IVec2 & IVec2::operator[]( T const & p_rhs )
	{
		m_value << Castor::String( *this ) << cuT( "[" ) << Castor::String( p_rhs ) << cuT( "]" );
		return *this;
	}

	//*****************************************************************************************

	IVec3::IVec3()
		: Type( cuT( "ivec3 " ) )
	{
	}

	IVec3::IVec3( GlslWriter * p_writer, Castor::String const & p_name )
		: Type( cuT( "ivec3 " ), p_writer, p_name )
	{
	}

	IVec3::~IVec3()
	{
	}

	IVec3 & IVec3::operator=( IVec3 const & p_rhs )
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
	IVec3 & IVec3::operator=( T const & p_rhs )
	{
		UpdateWriter( p_rhs );
		m_writer->WriteAssign( *this, p_rhs );
		return *this;
	}

	template< typename T >
	IVec3 & IVec3::operator[]( T const & p_rhs )
	{
		m_value << Castor::String( *this ) << cuT( "[" ) << Castor::String( p_rhs ) << cuT( "]" );
		return *this;
	}

	//*****************************************************************************************

	IVec4::IVec4()
		: Type( cuT( "ivec4 " ) )
	{
	}

	IVec4::IVec4( GlslWriter * p_writer, Castor::String const & p_name )
		: Type( cuT( "ivec4 " ), p_writer, p_name )
	{
	}

	IVec4::~IVec4()
	{
	}

	IVec4 & IVec4::operator=( IVec4 const & p_rhs )
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
	IVec4 & IVec4::operator=( T const & p_rhs )
	{
		UpdateWriter( p_rhs );
		m_writer->WriteAssign( *this, p_rhs );
		return *this;
	}

	template< typename T >
	IVec4 & IVec4::operator[]( T const & p_rhs )
	{
		m_value << Castor::String( *this ) << cuT( "[" ) << Castor::String( p_rhs ) << cuT( "]" );
		return *this;
	}

	//*****************************************************************************************
}
