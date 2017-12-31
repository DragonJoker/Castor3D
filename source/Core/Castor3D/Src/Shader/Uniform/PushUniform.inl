namespace castor3d
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
	inline typename TPushUniform< Type >::return_type & TPushUniform< Type >::getValue()
	{
		return m_uniform.getValue();
	}

	template< UniformType Type >
	inline typename TPushUniform< Type >::return_const_type const & TPushUniform< Type >::getValue()const
	{
		return m_uniform.getValue();
	}

	template< UniformType Type >
	inline typename TPushUniform< Type >::return_type & TPushUniform< Type >::getValue( uint32_t p_index )
	{
		return m_uniform.getValue( p_index );
	}

	template< UniformType Type >
	inline typename TPushUniform< Type >::return_const_type const & TPushUniform< Type >::getValue( uint32_t p_index )const
	{
		return m_uniform.getValue( p_index );
	}

	template< UniformType Type >
	inline void TPushUniform< Type >::setValue( param_type const & p_value )
	{
		m_uniform.setValue( p_value );
	}

	template< UniformType Type >
	inline void TPushUniform< Type >::setValue( param_type const & p_value, uint32_t p_index )
	{
		m_uniform.setValue( p_value, p_index );
	}

	template< UniformType Type >
	template< size_t N >
	inline void TPushUniform< Type >::setValues( param_type const( &p_values )[N] )
	{
		setValues( p_values, N );
	}

	template< UniformType Type >
	template< size_t N >
	inline void TPushUniform< Type >::setValues( std::array< param_type, N > const & p_values )
	{
		setValues( p_values.data(), N );
	}

	template< UniformType Type >
	inline void TPushUniform< Type >::setValues( std::vector< param_type > const & p_values )
	{
		setValues( p_values.data(), p_values.size() );
	}

	template< UniformType Type >
	inline void TPushUniform< Type >::setValues( param_type const * p_values, size_t p_size )
	{
		m_uniform.setValues( p_values, p_size );
	}

	template< UniformType Type >
	inline typename TPushUniform< Type >::return_type & TPushUniform< Type >::operator[]( uint32_t p_index )
	{
		return m_uniform[p_index];
	}

	template< UniformType Type >
	inline typename TPushUniform< Type >::return_const_type const & TPushUniform< Type >::operator[]( uint32_t p_index )const
	{
		return m_uniform[p_index];
	}
}
