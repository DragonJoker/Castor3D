namespace GLSL
{
	template< typename T1, typename T2 >
	void WriteAssign( GlslWriter * p_writer, Optional< Array< T1 > > const & p_lhs, T2 const & p_rhs );

	template< typename TypeT >
	Optional< Array< TypeT > >::Optional( GlslWriter * p_writer, Castor::String const & p_name, uint32_t p_dimension, bool p_enabled )
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
	Optional< Array< TypeT > >::Optional( Castor::String const & p_name, uint32_t p_dimension, T const & p_rhs, bool p_enabled )
		: Array< TypeT >( p_rhs.m_writer, p_name, p_dimension )
		, m_enabled( p_enabled )
	{
		if ( m_enabled )
		{
			WriteAssign( TypeT::m_writer, *this, p_rhs );
		}
	}

	template< typename TypeT >
	Optional< Array< TypeT > > Optional< Array< TypeT > >::operator=( Optional< Array< TypeT > > const & p_rhs )
	{
		if ( m_enabled )
		{
			WriteAssign( Array< TypeT >::m_writer, *this, p_rhs );
		}

		return *this;
	}

	template< typename TypeT >
	template< typename T >
	Optional< Array< TypeT > > Optional< Array< TypeT > >::operator=( T const & p_rhs )
	{
		if ( m_enabled )
		{
			WriteAssign( Array< TypeT >::m_writer, *this, p_rhs );
		}

		return *this;
	}

	template< typename TypeT >
	bool Optional< Array< TypeT > >::IsEnabled()const
	{
		return m_enabled;
	}

	template< typename TypeT >
	Optional< Array< TypeT > >::operator Optional< Array< TypeT > >()const
	{
		return Optional< Array< TypeT > >( *this, IsEnabled() );
	}

	template< typename TypeT >
	Castor::String ParamToString( Castor::String & p_sep, Optional< Array< TypeT > > const & p_value )
	{
		Castor::String l_result;

		if ( p_value.IsEnabled() )
		{
			l_result = ParamToString( p_sep, static_cast< Type const & >( p_value ) );
		}

		return l_result;
	}
}
