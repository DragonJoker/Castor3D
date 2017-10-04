namespace glsl
{
	//*********************************************************************************************

	template< typename T >
	Array< T >::Array( GlslWriter * writer
		, const castor::String & name
		, uint32_t dimension )
		: T( writer, name )
		, m_dimension( dimension )
	{
	}

	template< typename T >
	template< typename U >
	T Array< T >::operator[]( U const & offset )
	{
		return T{ T::m_writer, T::m_name + cuT( "[" ) + toString( offset ) + cuT( "]" ) };
	}

	//*********************************************************************************************

	template< SamplerType ST >
	Array< SamplerT< ST > >::Array( GlslWriter * writer
		, const castor::String & name
		, uint32_t dimension )
		: SamplerT< ST >( writer, name )
		, m_dimension( dimension )
	{
	}

	template< SamplerType ST >
	Array< SamplerT< ST > >::Array( GlslWriter * writer
		, uint32_t binding
		, const castor::String & name
		, uint32_t dimension )
		: SamplerT< ST >( writer, binding, name )
		, m_dimension( dimension )
	{
	}

	template< SamplerType ST >
	template< typename T >
	SamplerT< ST > Array< SamplerT< ST > >::operator[]( T const & offset )
	{
		return SamplerT< ST >{ SamplerT< ST >::m_writer, SamplerT< ST >::m_name + cuT( "[" ) + toString( offset ) + cuT( "]" ) };
	}

	//*********************************************************************************************
}
