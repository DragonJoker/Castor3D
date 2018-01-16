namespace glsl
{
	//*****************************************************************************************

	template< SamplerType ST >
	SamplerT< ST >::SamplerT()
		: Type( TypeTraits< typename SamplerTypeTraits< ST >::Type >::Name + cuT( " " ) )
	{
	}

	template< SamplerType ST >
	SamplerT< ST >::SamplerT( GlslWriter * writer
		, castor::String const & name )
		: Type( TypeTraits< typename SamplerTypeTraits< ST >::Type >::Name + cuT( " " ), writer, name )
	{
	}

	template< SamplerType ST >
	SamplerT< ST >::SamplerT( GlslWriter * writer
		, uint32_t bind
		, castor::String const & name )
		: Type( TypeTraits< typename SamplerTypeTraits< ST >::Type >::Name + cuT( " " ), writer, name )
		, m_binding{ bind }
	{
	}

	template< SamplerType ST >
	SamplerT< ST >::SamplerT( GlslWriter * writer
		, uint32_t bind
		, uint32_t set
		, castor::String const & name )
		: Type( TypeTraits< typename SamplerTypeTraits< ST >::Type >::Name + cuT( " " ), writer, name )
		, m_binding{ bind }
		, m_set{ set }
	{
	}

	template< SamplerType ST >
	template< typename T >
	SamplerT< ST > & SamplerT< ST >::operator=( T const & rhs )
	{
		this->updateWriter( rhs );
		this->m_writer->writeAssign( *this, rhs );
		return *this;
	}

	template< SamplerType ST >
	SamplerT< ST >::operator uint32_t()
	{
		return 0u;
	}

	//*****************************************************************************************
}
