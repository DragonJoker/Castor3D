namespace Castor3D
{
	template< UniformType Type  >
	TPushUniform< Type >::TPushUniform( ShaderProgram & p_program, typename UniformTyper< Type >::type & p_uniform, uint32_t p_occurences )
		: PushUniform( p_program, p_uniform )
	{
	}

	template< UniformType Type >
	TPushUniform< Type >::~TPushUniform()
	{
	}

	template< UniformType Type >
	inline typename TPushUniform< Type >::value_type & TPushUniform< Type >::GetValue()
	{
		return m_uniform.GetValue();
	}

	template< UniformType Type >
	inline typename TPushUniform< Type >::value_type const & TPushUniform< Type >::GetValue()const
	{
		return m_uniform.GetValue();
	}

	template< UniformType Type >
	inline typename TPushUniform< Type >::value_type & TPushUniform< Type >::GetValue( uint32_t p_index )
	{
		return m_uniform.GetValue( p_index );
	}

	template< UniformType Type >
	inline typename TPushUniform< Type >::value_type const & TPushUniform< Type >::GetValue( uint32_t p_index )const
	{
		return m_uniform.GetValue( p_index );
	}

	template< UniformType Type >
	inline void TPushUniform< Type >::SetValue( value_type const & p_value )
	{
		m_uniform.SetValue( p_value );
	}

	template< UniformType Type >
	inline void TPushUniform< Type >::SetValue( value_type const & p_value, uint32_t p_index )
	{
		m_uniform.SetValue( p_value, p_index );
	}
	template< UniformType Type >

	inline void TPushUniform< Type >::SetValues( value_type const * p_values, size_t p_size )
	{
		m_uniform.SetValues( p_values, p_size );
	}

	template< UniformType Type >
	inline typename TPushUniform< Type >::value_type & TPushUniform< Type >::operator[]( uint32_t p_index )
	{
		return m_uniform[p_index];
	}

	template< UniformType Type >
	inline typename TPushUniform< Type >::value_type const & TPushUniform< Type >::operator[]( uint32_t p_index )const
	{
		return m_uniform[p_index];
	}
}
