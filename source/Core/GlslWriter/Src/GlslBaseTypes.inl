namespace GLSL
{
	//*****************************************************************************************

	Int::Int( )
		: Type( cuT( "int " ) )
	{
	}

	Int::Int( Int && p_value )
		: Type( std::move( p_value ) )
	{
	}

	Int::Int( Int const & p_value )
		: Type( cuT( "int " ), p_value.m_writer )
	{
		m_value << Castor::String( p_value );
	}

	Int::Int( Type const & p_value )
		: Type( cuT( "int " ), p_value.m_writer )
	{
		m_value << Castor::String( p_value );
	}

	Int::Int( int p_value )
		: Type( cuT( "int " ) )
	{
		m_value << p_value;
	}

	Int::Int( float p_value )
		: Type( cuT( "int " ) )
	{
		m_value << p_value;
	}

	Int::Int( GlslWriter * p_writer, int p_value )
		: Type( cuT( "int " ), p_writer, Castor::String() )
	{
		m_value << p_value;
	}

	Int::Int( GlslWriter * p_writer, float p_value )
		: Type( cuT( "int " ), p_writer, Castor::String() )
	{
		m_value << p_value;
	}

	Int::Int( GlslWriter * p_writer, Castor::String const & p_name )
		: Type( cuT( "int " ), p_writer, p_name )
	{
	}

	Int & Int::operator=( Int const & p_rhs )
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
	Int & Int::operator=( T const & p_rhs )
	{
		UpdateWriter( p_rhs );
		m_writer->WriteAssign( *this, p_rhs );
		return *this;
	}

	template< typename T >
	Int & Int::operator=( int p_rhs )
	{
		m_writer->WriteAssign( *this, p_rhs );
		return *this;
	}

	Int::operator uint32_t()
	{
		return 0u;
	}

	Int & Int::operator++()
	{
		m_value << cuT( "++" ) << Castor::String( *this );
		return *this;
	}

	Int Int::operator++( int )
	{
		Int l_return;
		l_return.m_value << Castor::String( *this ) << cuT( "++" );
		return l_return;
	}
	//*****************************************************************************************

	UInt::UInt()
		: Type( cuT( "uint " ) )
	{
	}

	UInt::UInt( UInt && p_value )
		: Type( std::move( p_value ) )
	{
	}

	UInt::UInt( UInt const & p_value )
		: Type( cuT( "uint " ), p_value.m_writer )
	{
		m_value << Castor::String( p_value );
	}

	UInt::UInt( Type const & p_value )
		: Type( cuT( "uint " ), p_value.m_writer )
	{
		m_value << Castor::String( p_value );
	}

	UInt::UInt( unsigned int p_value )
		: Type( cuT( "uint " ) )
	{
		m_value << p_value;
	}

	UInt::UInt( float p_value )
		: Type( cuT( "uint " ) )
	{
		m_value << p_value;
	}

	UInt::UInt( GlslWriter * p_writer, unsigned int p_value )
		: Type( cuT( "uint " ), p_writer, Castor::String() )
	{
		m_value << p_value;
	}

	UInt::UInt( GlslWriter * p_writer, float p_value )
		: Type( cuT( "uint " ), p_writer, Castor::String() )
	{
		m_value << p_value;
	}

	UInt::UInt( GlslWriter * p_writer, Castor::String const & p_name )
		: Type( cuT( "uint " ), p_writer, p_name )
	{
	}

	UInt & UInt::operator=( UInt const & p_rhs )
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
	UInt & UInt::operator=( T const & p_rhs )
	{
		UpdateWriter( p_rhs );
		m_writer->WriteAssign( *this, p_rhs );
		return *this;
	}

	template< typename T >
	UInt & UInt::operator=( unsigned int p_rhs )
	{
		m_writer->WriteAssign( *this, p_rhs );
		return *this;
	}

	UInt::operator uint32_t()
	{
		return 0u;
	}

	UInt & UInt::operator++()
	{
		m_value << cuT( "++" ) << Castor::String( *this );
		return *this;
	}

	UInt UInt::operator++( int )
	{
		UInt l_return;
		l_return.m_value << Castor::String( *this ) << cuT( "++" );
		return l_return;
	}

	//*****************************************************************************************

	Float::Float()
		: Type( cuT( "float " ) )
	{
	}

	Float::Float( Float && p_value )
		: Type( std::move( p_value ) )
	{
	}

	Float::Float( Float const & p_value )
		: Type( cuT( "float " ) )
	{
		m_value << Castor::String( p_value );
	}

	Float::Float( Type const & p_value )
		: Type( cuT( "float " ) )
	{
		m_value << Castor::String( p_value );
	}

	Float::Float( int p_value )
		: Type( cuT( "float " ) )
	{
		m_value << p_value << cuT( ".0" );
	}

	Float::Float( float p_value )
		: Type( cuT( "float " ) )
	{
		m_value << p_value;

		if ( p_value - int( p_value ) <= std::numeric_limits< float >::epsilon() )
		{
			m_value << cuT( ".0" );
		}
	}

	Float::Float( double p_value )
		: Type( cuT( "float " ) )
	{
		m_value << p_value;

		if ( abs( p_value - int( p_value ) ) <= std::numeric_limits< double >::epsilon() )
		{
			m_value << cuT( ".0" );
		}
	}

	Float::Float( GlslWriter * p_writer, int p_value )
		: Type( cuT( "float " ), p_writer, Castor::String() )
	{
		m_value << p_value << cuT( ".0" );
	}

	Float::Float( GlslWriter * p_writer, float p_value )
		: Type( cuT( "float " ), p_writer, Castor::String() )
	{
		m_value << p_value;

		if ( abs( p_value - int( p_value ) ) <= std::numeric_limits< float >::epsilon() )
		{
			m_value << cuT( ".0" );
		}
	}

	Float::Float( GlslWriter * p_writer, double p_value )
		: Type( cuT( "float " ), p_writer, Castor::String() )
	{
		m_value << p_value;

		if ( abs( p_value - int( p_value ) ) <= std::numeric_limits< double >::epsilon() )
		{
			m_value << cuT( ".0" );
		}
	}

	Float::Float( GlslWriter * p_writer, Castor::String const & p_name )
		: Type( cuT( "float " ), p_writer, p_name )
	{
	}

	Float & Float::operator=( Float const & p_rhs )
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
	Float & Float::operator=( T const & p_rhs )
	{
		UpdateWriter( p_rhs );
		m_writer->WriteAssign( *this, p_rhs );
		return *this;
	}

	template< typename T >
	Float & Float::operator=( float p_rhs )
	{
		*m_writer << Castor::String( *this ) << cuT( " = " ) << Castor::string::to_string( p_rhs ) << cuT( ";" ) << Endl();
		return *this;
	}

	Float::operator float()
	{
		return 0.0f;
	}

	//*****************************************************************************************
}
