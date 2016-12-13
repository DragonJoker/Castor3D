namespace Castor3D
{
	template< UniformType Type  >
	TPushUniform< Type >::TPushUniform( ShaderProgram & p_program, uint32_t p_occurences )
		: PushUniform{ p_program }
		, m_uniform{ p_occurences }
	{
	}

	template< UniformType Type >
	TPushUniform< Type >::~TPushUniform()
	{
	}

	template< UniformType Type >
	inline typename TPushUniform< Type >::return_type & TPushUniform< Type >::GetValue()
	{
		return m_uniform.GetValue();
	}

	template< UniformType Type >
	inline typename TPushUniform< Type >::const_return_type const & TPushUniform< Type >::GetValue()const
	{
		return m_uniform.GetValue();
	}

	template< UniformType Type >
	inline typename TPushUniform< Type >::return_type & TPushUniform< Type >::GetValue( uint32_t p_index )
	{
		return m_uniform.GetValue( p_index );
	}

	template< UniformType Type >
	inline typename TPushUniform< Type >::const_return_type const & TPushUniform< Type >::GetValue( uint32_t p_index )const
	{
		return m_uniform.GetValue( p_index );
	}

	template< UniformType Type >
	inline void TPushUniform< Type >::SetValue( param_type const & p_value )
	{
		m_uniform.SetValue( p_value );
	}

	template< UniformType Type >
	inline void TPushUniform< Type >::SetValue( param_type const & p_value, uint32_t p_index )
	{
		m_uniform.SetValue( p_value, p_index );
	}

	template< UniformType Type >
	template< size_t N >
	inline void TPushUniform< Type >::SetValues( param_type const( &p_values )[N] )
	{
		SetValues( p_values, N );
	}

	template< UniformType Type >
	template< size_t N >
	inline void TPushUniform< Type >::SetValues( std::array< param_type, N > const & p_values )
	{
		SetValues( p_values.data(), N );
	}

	template< UniformType Type >
	inline void TPushUniform< Type >::SetValues( std::vector< param_type > const & p_values )
	{
		SetValues( p_values.data(), p_values.size() );
	}

	template< UniformType Type >
	inline void TPushUniform< Type >::SetValues( param_type const * p_values, size_t p_size )
	{
		m_uniform.SetValues( p_values, p_size );
	}

	template< UniformType Type >
	inline typename TPushUniform< Type >::return_type & TPushUniform< Type >::operator[]( uint32_t p_index )
	{
		return m_uniform[p_index];
	}

	template< UniformType Type >
	inline typename TPushUniform< Type >::const_return_type const & TPushUniform< Type >::operator[]( uint32_t p_index )const
	{
		return m_uniform[p_index];
	}
}
