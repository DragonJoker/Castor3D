namespace glsl
{
	template< typename T >
	void writeAssign( GlslWriter * p_writer, Type & p_lhs, T const & p_rhs );

	template< typename T1, typename T2 >
	void writeAssign( GlslWriter * p_writer, Optional< T1 > const & p_lhs, T2 const & p_rhs );

	template< typename TypeT >
	Optional< TypeT >::Optional( GlslWriter * p_writer, castor::String const & p_name, bool p_enabled )
		: TypeT( p_writer, p_name )
		, m_enabled( p_enabled )
	{
	}

	template< typename TypeT >
	Optional< TypeT >::Optional( TypeT const & p_other, bool p_enabled )
		: TypeT( p_other )
		, m_enabled( p_enabled )
	{
	}

	template< typename TypeT >
	template< typename T >
	Optional< TypeT >::Optional( castor::String const & p_name, T const & p_rhs, bool p_enabled )
		: TypeT( p_rhs.m_writer, p_name )
		, m_enabled( p_enabled )
	{
		if ( m_enabled )
		{
			writeAssign( TypeT::m_writer, *this, p_rhs );
		}
	}

	template< typename TypeT >
	Optional< TypeT > Optional< TypeT >::operator=( Optional< TypeT > const & p_rhs )
	{
		if ( m_enabled )
		{
			writeAssign( TypeT::m_writer, *this, p_rhs );
		}

		return *this;
	}

	template< typename TypeT >
	template< typename T >
	Optional< TypeT > Optional< TypeT >::operator=( T const & p_rhs )
	{
		if ( m_enabled )
		{
			writeAssign( TypeT::m_writer, *this, p_rhs );
		}

		return *this;
	}

	template< typename TypeT >
	bool Optional< TypeT >::isEnabled()const
	{
		return m_enabled;
	}

	template< typename TypeT >
	Optional< TypeT >::operator Optional< Type >()const
	{
		return Optional< Type >( *this, isEnabled() );
	}

	template< typename TypeT >
	castor::String paramToString( castor::String & p_sep, Optional< TypeT > const & p_value )
	{
		castor::String result;

		if ( p_value.isEnabled() )
		{
			result = paramToString( p_sep, static_cast< Type const & >( p_value ) );
		}

		return result;
	}
}
