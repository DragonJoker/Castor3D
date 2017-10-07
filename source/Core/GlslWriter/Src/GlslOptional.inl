namespace glsl
{
	template< typename T >
	void writeAssign( GlslWriter * writer, Type & lhs, T const & rhs );

	template< typename T1, typename T2 >
	void writeAssign( GlslWriter * writer, Optional< T1 > const & lhs, T2 const & rhs );

	//*********************************************************************************************

	template< typename TypeT >
	Optional< TypeT >::Optional( GlslWriter * writer, castor::String const & name, bool enabled )
		: TypeT( writer, name )
		, m_enabled( enabled )
	{
	}

	template< typename TypeT >
	Optional< TypeT >::Optional( TypeT const & other, bool enabled )
		: TypeT( other )
		, m_enabled( enabled )
	{
	}

	template< typename TypeT >
	template< typename T >
	Optional< TypeT >::Optional( castor::String const & name, T const & rhs, bool enabled )
		: TypeT( rhs.m_writer, name )
		, m_enabled( enabled )
	{
		if ( m_enabled )
		{
			writeAssign( TypeT::m_writer, *this, rhs );
		}
	}

	template< typename TypeT >
	Optional< TypeT > Optional< TypeT >::operator=( Optional< TypeT > const & rhs )
	{
		if ( m_enabled )
		{
			writeAssign( TypeT::m_writer, *this, rhs );
		}

		return *this;
	}

	template< typename TypeT >
	template< typename T >
	Optional< TypeT > Optional< TypeT >::operator=( T const & rhs )
	{
		if ( m_enabled )
		{
			writeAssign( TypeT::m_writer, *this, rhs );
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

	//*********************************************************************************************

	template< SamplerType ST >
	Optional< SamplerT< ST > >::Optional( GlslWriter * writer, uint32_t binding, castor::String const & name, bool enabled )
		: SamplerT< ST >( writer, binding, name )
		, m_enabled( enabled )
	{
	}

	template< SamplerType ST >
	Optional< SamplerT< ST > >::Optional( SamplerT< ST > const & other, bool enabled )
		: SamplerT< ST >( other )
		, m_enabled( enabled )
	{
	}

	template< SamplerType ST >
	template< typename T >
	Optional< SamplerT< ST > >::Optional( castor::String const & name, T const & rhs, bool enabled )
		: SamplerT< ST >( rhs.m_writer, name )
		, m_enabled( enabled )
	{
		if ( m_enabled )
		{
			writeAssign( SamplerT< ST >::m_writer, *this, rhs );
		}
	}

	template< SamplerType ST >
	Optional< SamplerT< ST > > Optional< SamplerT< ST > >::operator=( Optional< SamplerT< ST > > const & rhs )
	{
		if ( m_enabled )
		{
			writeAssign( SamplerT< ST >::m_writer, *this, rhs );
		}

		return *this;
	}

	template< SamplerType ST >
	template< typename T >
	Optional< SamplerT< ST > > Optional< SamplerT< ST > >::operator=( T const & rhs )
	{
		if ( m_enabled )
		{
			writeAssign( SamplerT< ST >::m_writer, *this, rhs );
		}

		return *this;
	}

	template< SamplerType ST >
	bool Optional< SamplerT< ST > >::isEnabled()const
	{
		return m_enabled;
	}

	template< SamplerType ST >
	Optional< SamplerT< ST > >::operator Optional< Type >()const
	{
		return Optional< Type >( *this, isEnabled() );
	}

	//*********************************************************************************************

	template< typename TypeT >
	castor::String paramToString( castor::String & sep, Optional< TypeT > const & value )
	{
		castor::String result;

		if ( value.isEnabled() )
		{
			result = paramToString( sep, static_cast< Type const & >( value ) );
		}

		return result;
	}
}
