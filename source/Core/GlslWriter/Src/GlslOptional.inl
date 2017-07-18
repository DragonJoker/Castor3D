namespace GLSL
{
	template< typename T >
	void WriteAssign( GlslWriter * p_writer, Type & p_lhs, T const & p_rhs );

	template< typename T1, typename T2 >
	void WriteAssign( GlslWriter * p_writer, Optional< T1 > const & p_lhs, T2 const & p_rhs );

	template< typename TypeT >
	Optional< TypeT >::Optional( GlslWriter * p_writer, Castor::String const & p_name, bool p_enabled )
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
	Optional< TypeT >::Optional( Castor::String const & p_name, T const & p_rhs, bool p_enabled )
		: TypeT( p_rhs.m_writer, p_name )
		, m_enabled( p_enabled )
	{
		if ( m_enabled )
		{
			WriteAssign( TypeT::m_writer, *this, p_rhs );
		}
	}

	template< typename TypeT >
	Optional< TypeT > Optional< TypeT >::operator=( Optional< TypeT > const & p_rhs )
	{
		if ( m_enabled )
		{
			WriteAssign( TypeT::m_writer, *this, p_rhs );
		}

		return *this;
	}

	template< typename TypeT >
	template< typename T >
	Optional< TypeT > Optional< TypeT >::operator=( T const & p_rhs )
	{
		if ( m_enabled )
		{
			WriteAssign( TypeT::m_writer, *this, p_rhs );
		}

		return *this;
	}

	template< typename TypeT >
	bool Optional< TypeT >::IsEnabled()const
	{
		return m_enabled;
	}

	template< typename TypeT >
	Optional< TypeT >::operator Optional< Type >()const
	{
		return Optional< Type >( *this, IsEnabled() );
	}

	template< typename TypeT >
	Castor::String ParamToString( Castor::String & p_sep, Optional< TypeT > const & p_value )
	{
		Castor::String result;

		if ( p_value.IsEnabled() )
		{
			result = ParamToString( p_sep, static_cast< Type const & >( p_value ) );
		}

		return result;
	}
}
