namespace GLSL
{
	template< typename T1, typename T2 >
	void writeAssign( GlslWriter * p_writer, Optional< Array< T1 > > const & p_lhs, T2 const & p_rhs );

	template< typename TypeT >
	Optional< Array< TypeT > >::Optional( GlslWriter * p_writer, castor::String const & p_name, uint32_t p_dimension, bool p_enabled )
		: Array< TypeT >( p_writer, p_name, p_dimension )
		, m_enabled( p_enabled )
	{
	}

	template< typename TypeT >
	Optional< Array< TypeT > >::Optional( Array< TypeT > const & p_other, bool p_enabled )
		: Array< TypeT >( p_other )
		, m_enabled( p_enabled )
	{
	}

	template< typename TypeT >
	template< typename T >
	Optional< Array< TypeT > >::Optional( castor::String const & p_name, uint32_t p_dimension, T const & p_rhs, bool p_enabled )
		: Array< TypeT >( p_rhs.m_writer, p_name, p_dimension )
		, m_enabled( p_enabled )
	{
		if ( m_enabled )
		{
			writeAssign( TypeT::m_writer, *this, p_rhs );
		}
	}

	template< typename TypeT >
	Optional< Array< TypeT > > Optional< Array< TypeT > >::operator=( Optional< Array< TypeT > > const & p_rhs )
	{
		if ( m_enabled )
		{
			writeAssign( Array< TypeT >::m_writer, *this, p_rhs );
		}

		return *this;
	}

	template< typename TypeT >
	template< typename T >
	Optional< Array< TypeT > > Optional< Array< TypeT > >::operator=( T const & p_rhs )
	{
		if ( m_enabled )
		{
			writeAssign( Array< TypeT >::m_writer, *this, p_rhs );
		}

		return *this;
	}

	template< typename TypeT >
	bool Optional< Array< TypeT > >::isEnabled()const
	{
		return m_enabled;
	}

	template< typename TypeT >
	Optional< Array< TypeT > >::operator Optional< Array< TypeT > >()const
	{
		return Optional< Array< TypeT > >( *this, isEnabled() );
	}

	template< typename TypeT >
	castor::String paramToString( castor::String & p_sep, Optional< Array< TypeT > > const & p_value )
	{
		castor::String result;

		if ( p_value.isEnabled() )
		{
			result = paramToString( p_sep, static_cast< Type const & >( p_value ) );
		}

		return result;
	}
}
