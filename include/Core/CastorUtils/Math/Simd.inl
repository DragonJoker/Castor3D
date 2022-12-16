namespace castor
{
	inline Float4::Float4( float const * rhs )
		: m_value( _mm_load_ps( rhs ) )
	{
	}

	inline Float4::Float4( float rhs )
		: m_value( _mm_set_ps1( rhs ) )
	{
	}

	inline void Float4::toPtr( float * rhs )
	{
		_mm_store_ps( rhs, m_value );
	}

	inline Float4 & Float4::operator+=( Float4 const & rhs )
	{
		m_value = _mm_add_ps( m_value, rhs.m_value );
		return *this;
	}

	inline Float4 & Float4::operator-=( Float4 const & rhs )
	{
		m_value = _mm_sub_ps( m_value, rhs.m_value );
		return *this;
	}

	inline Float4 & Float4::operator*=( Float4 const & rhs )
	{
		m_value = _mm_mul_ps( m_value, rhs.m_value );
		return *this;
	}

	inline Float4 & Float4::operator/=( Float4 const & rhs )
	{
		m_value = _mm_div_ps( m_value, rhs.m_value );
		return *this;
	}

	inline Float4 operator+( Float4 const & lhs, Float4 const & rhs )
	{
		Float4 result{ lhs };
		return result += rhs;
	}

	inline Float4 operator-( Float4 const & lhs, Float4 const & rhs )
	{
		Float4 result{ lhs };
		return result -= rhs;
	}

	inline Float4 operator*( Float4 const & lhs, Float4 const & rhs )
	{
		Float4 result{ lhs };
		return result *= rhs;
	}

	inline Float4 operator/( Float4 const & lhs, Float4 const & rhs )
	{
		Float4 result{ lhs };
		return result /= rhs;
	}
}
