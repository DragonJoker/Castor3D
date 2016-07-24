namespace GLSL
{
	template< typename T >
	Array< T >::Array( GlslWriter * p_writer, const Castor::String & p_name, uint32_t p_dimension )
		: T( p_writer, p_name )
		, m_dimension( p_dimension )
	{
	}

	template< typename T >
	template< typename U >
	T & Array< T >::operator[]( U const & p_offset )
	{
		T::m_value << Castor::String( *static_cast< T * >( this ) ) << cuT( "[" ) << ToString( p_offset ) << cuT( "]" );
		return *this;
	}
}
