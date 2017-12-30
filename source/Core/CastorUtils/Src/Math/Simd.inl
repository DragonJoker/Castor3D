namespace castor
{
	inline Float4::Float4( float const * p_values )
		: m_value( _mm_load_ps( p_values ) )
	{
	}

	inline Float4::Float4( float p_value )
		: m_value( _mm_set_ps1( p_value ) )
	{
	}

	inline void Float4::toPtr( float * p_values )
	{
		_mm_store_ps( p_values, m_value );
	}

	inline Float4 & Float4::operator+=( Float4 const & p_rhs )
	{
		m_value = _mm_add_ps( m_value, p_rhs.m_value );
		return *this;
	}

	inline Float4 & Float4::operator-=( Float4 const & p_rhs )
	{
		m_value = _mm_sub_ps( m_value, p_rhs.m_value );
		return *this;
	}

	inline Float4 & Float4::operator*=( Float4 const & p_rhs )
	{
		m_value = _mm_mul_ps( m_value, p_rhs.m_value );
		return *this;
	}

	inline Float4 & Float4::operator/=( Float4 const & p_rhs )
	{
		m_value = _mm_div_ps( m_value, p_rhs.m_value );
		return *this;
	}

	inline Float4 operator+( Float4 const & p_lhs, Float4 const & p_rhs )
	{
		Float4 lhs{ p_lhs };
		return lhs += p_rhs;
	}

	inline Float4 operator-( Float4 const & p_lhs, Float4 const & p_rhs )
	{
		Float4 lhs{ p_lhs };
		return lhs -= p_rhs;
	}

	inline Float4 operator*( Float4 const & p_lhs, Float4 const & p_rhs )
	{
		Float4 lhs{ p_lhs };
		return lhs *= p_rhs;
	}

	inline Float4 operator/( Float4 const & p_lhs, Float4 const & p_rhs )
	{
		Float4 lhs{ p_lhs };
		return lhs /= p_rhs;
	}
}
