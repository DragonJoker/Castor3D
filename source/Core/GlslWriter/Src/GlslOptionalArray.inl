namespace glsl
{
	template< typename T1, typename T2 >
	void writeAssign( GlslWriter * writer
		, Optional< Array< T1 > > const & lhs
		, T2 const & rhs );

	//*********************************************************************************************

	template< typename TypeT >
	Optional< Array< TypeT > >::Optional( GlslWriter * writer
		, castor::String const & name
		, uint32_t dimension
		, bool enabled )
		: Array< TypeT >( writer, name, dimension )
		, m_enabled( enabled )
	{
	}

	template< typename TypeT >
	Optional< Array< TypeT > >::Optional( Array< TypeT > const & other
		, bool enabled )
		: Array< TypeT >( other )
		, m_enabled( enabled )
	{
	}

	template< typename TypeT >
	template< typename T >
	Optional< Array< TypeT > >::Optional( castor::String const & name
		, uint32_t dimension
		, T const & rhs
		, bool enabled )
		: Array< TypeT >( rhs.m_writer, name, dimension )
		, m_enabled( enabled )
	{
		if ( m_enabled )
		{
			writeAssign( TypeT::m_writer, *this, rhs );
		}
	}

	template< typename TypeT >
	Optional< Array< TypeT > > Optional< Array< TypeT > >::operator=( Optional< Array< TypeT > > const & rhs )
	{
		if ( m_enabled )
		{
			writeAssign( Array< TypeT >::m_writer, *this, rhs );
		}

		return *this;
	}

	template< typename TypeT >
	template< typename T >
	Optional< Array< TypeT > > Optional< Array< TypeT > >::operator=( T const & rhs )
	{
		if ( m_enabled )
		{
			writeAssign( Array< TypeT >::m_writer, *this, rhs );
		}

		return *this;
	}

	template< typename TypeT >
	bool Optional< Array< TypeT > >::isEnabled()const
	{
		return m_enabled;
	}

	template< typename TypeT >
	Optional< Array< TypeT > >::operator Optional< Array< Type > >()const
	{
		return Optional< Array< Type > >( *this, isEnabled() );
	}

	//*********************************************************************************************

	template< SamplerType ST >
	Optional< Array< SamplerT< ST > > >::Optional( GlslWriter * writer
		, castor::String const & name
		, uint32_t dimension
		, bool enabled )
		: Array< SamplerT< ST > >( writer, name, dimension )
		, m_enabled( enabled )
	{
	}

	template< SamplerType ST >
	Optional< Array< SamplerT< ST > > >::Optional( GlslWriter * writer
		, uint32_t binding
		, castor::String const & name
		, uint32_t dimension
		, bool enabled )
		: Array< SamplerT< ST > >( writer, binding, name, dimension )
		, m_enabled( enabled )
	{
	}

	template< SamplerType ST >
	Optional< Array< SamplerT< ST > > >::Optional( Array< SamplerT< ST > > const & other
		, bool enabled )
		: Array< SamplerT< ST > >( other )
		, m_enabled( enabled )
	{
	}

	template< SamplerType ST >
	template< typename T >
	Optional< Array< SamplerT< ST > > >::Optional( castor::String const & name
		, uint32_t dimension
		, T const & rhs
		, bool enabled )
		: Array< SamplerT< ST > >( rhs.m_writer, name, dimension )
		, m_enabled( enabled )
	{
		if ( m_enabled )
		{
			writeAssign( SamplerT< ST >::m_writer, *this, rhs );
		}
	}

	template< SamplerType ST >
	Optional< Array< SamplerT< ST > > > Optional< Array< SamplerT< ST > > >::operator=( Optional< Array< SamplerT< ST > > > const & rhs )
	{
		if ( m_enabled )
		{
			writeAssign( Array< SamplerT< ST > >::m_writer, *this, rhs );
		}

		return *this;
	}

	template< SamplerType ST >
	template< typename T >
	Optional< Array< SamplerT< ST > > > Optional< Array< SamplerT< ST > > >::operator=( T const & rhs )
	{
		if ( m_enabled )
		{
			writeAssign( Array< SamplerT< ST > >::m_writer, *this, rhs );
		}

		return *this;
	}

	template< SamplerType ST >
	bool Optional< Array< SamplerT< ST > > >::isEnabled()const
	{
		return m_enabled;
	}

	template< SamplerType ST >
	Optional< Array< SamplerT< ST > > >::operator Optional< Array< Type > >()const
	{
		return Optional< Array< Type > >( *this, isEnabled() );
	}

	//*********************************************************************************************

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
