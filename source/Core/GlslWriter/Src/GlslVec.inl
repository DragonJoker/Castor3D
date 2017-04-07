namespace GLSL
{
	//*****************************************************************************************

	namespace details
	{
		template< typename ValueT >
		struct Vec2Traits;

		template<>
		struct Vec2Traits< Float >
		{
			static xchar const * const GetName()
			{
				static xchar const * const l_name{ "vec2 " };
				return l_name;
			}
		};

		template<>
		struct Vec2Traits< Int >
		{
			static xchar const * const GetName()
			{
				static xchar const * const l_name{ "ivec2 " };
				return l_name;
			}
		};

		template<>
		struct Vec2Traits< Boolean >
		{
			static xchar const * const GetName()
			{
				static xchar const * const l_name{ "bvec2 " };
				return l_name;
			}
		};

		//*****************************************************************************************

		template< typename ValueT >
		struct Vec3Traits;

		template<>
		struct Vec3Traits< Float >
		{
			static xchar const * const GetName()
			{
				static xchar const * const l_name{ "vec3 " };
				return l_name;
			}
		};

		template<>
		struct Vec3Traits< Int >
		{
			static xchar const * const GetName()
			{
				static xchar const * const l_name{ "ivec3 " };
				return l_name;
			}
		};

		template<>
		struct Vec3Traits< Boolean >
		{
			static xchar const * const GetName()
			{
				static xchar const * const l_name{ "bvec3 " };
				return l_name;
			}
		};

		//*****************************************************************************************

		template< typename ValueT >
		struct Vec4Traits;

		template<>
		struct Vec4Traits< Float >
		{
			static xchar const * const GetName()
			{
				static xchar const * const l_name{ "vec4 " };
				return l_name;
			}
		};

		template<>
		struct Vec4Traits< Int >
		{
			static xchar const * const GetName()
			{
				static xchar const * const l_name{ "ivec4 " };
				return l_name;
			}
		};

		template<>
		struct Vec4Traits< Boolean >
		{
			static xchar const * const GetName()
			{
				static xchar const * const l_name{ "bvec4 " };
				return l_name;
			}
		};
	}

	//*****************************************************************************************

	template< typename ValueT >
	inline Vec2T< ValueT >::Vec2T()
		: Type( details::Vec2Traits< ValueT >::GetName() )
	{
	}

	template< typename ValueT >
	inline Vec2T< ValueT >::Vec2T( GlslWriter * p_writer, Castor::String const & p_name )
		: Type( details::Vec2Traits< ValueT >::GetName(), p_writer, p_name )
	{
	}

	template< typename ValueT >
	inline Vec2T< ValueT > & Vec2T< ValueT >::operator=( Vec2T< ValueT > const & p_rhs )
	{
		if ( m_writer )
		{
			m_writer->WriteAssign( *this, p_rhs );
		}
		else
		{
			Type::operator=( p_rhs );
			m_writer = p_rhs.m_writer;
		}

		return *this;
	}

	template< typename ValueT >
	template< typename RhsT >
	inline Vec2T< ValueT > & Vec2T< ValueT >::operator=( RhsT const & p_rhs )
	{
		UpdateWriter( p_rhs );
		m_writer->WriteAssign( *this, p_rhs );
		return *this;
	}

	template< typename ValueT >
	template< typename IndexT >
	inline ValueT Vec2T< ValueT >::operator[]( IndexT const & p_rhs )const
	{
		ValueT l_return{ m_writer, Castor::String( *this ) + cuT( "[" ) + Castor::String( p_rhs ) + cuT( "]" ) };
		return l_return;
	}

	template< typename ValueT >
	inline ValueT Vec2T< ValueT >::operator[]( int const & p_rhs )const
	{
		ValueT l_return{ m_writer, Castor::String( *this ) + cuT( "[" ) + Castor::string::to_string( p_rhs ) + cuT( "]" ) };
		return l_return;
	}

	//*****************************************************************************************

	template< typename ValueT >
	inline Vec3T< ValueT >::Vec3T()
		: Type( details::Vec3Traits< ValueT >::GetName() )
	{
	}

	template< typename ValueT >
	inline Vec3T< ValueT >::Vec3T( GlslWriter * p_writer, Castor::String const & p_name )
		: Type( details::Vec3Traits< ValueT >::GetName(), p_writer, p_name )
	{
	}

	template< typename ValueT >
	inline Vec3T< ValueT > & Vec3T< ValueT >::operator=( Vec3T< ValueT > const & p_rhs )
	{
		if ( m_writer )
		{
			m_writer->WriteAssign( *this, p_rhs );
		}
		else
		{
			Type::operator=( p_rhs );
			m_writer = p_rhs.m_writer;
		}

		return *this;
	}

	template< typename ValueT >
	template< typename RhsT >
	inline Vec3T< ValueT > & Vec3T< ValueT >::operator=( RhsT const & p_rhs )
	{
		UpdateWriter( p_rhs );
		m_writer->WriteAssign( *this, p_rhs );
		return *this;
	}

	template< typename ValueT >
	template< typename IndexT >
	inline ValueT Vec3T< ValueT >::operator[]( IndexT const & p_rhs )const
	{
		ValueT l_return{ m_writer, Castor::String( *this ) + cuT( "[" ) + Castor::String( p_rhs ) + cuT( "]" ) };
		return l_return;
	}

	template< typename ValueT >
	inline ValueT Vec3T< ValueT >::operator[]( int const & p_rhs )const
	{
		ValueT l_return{ m_writer, Castor::String( *this ) + cuT( "[" ) + Castor::string::to_string( p_rhs ) + cuT( "]" ) };
		return l_return;
	}

	//*****************************************************************************************

	template< typename ValueT >
	inline Vec4T< ValueT >::Vec4T()
		: Type( details::Vec4Traits< ValueT >::GetName() )
	{
	}

	template< typename ValueT >
	inline Vec4T< ValueT >::Vec4T( GlslWriter * p_writer, Castor::String const & p_name )
		: Type( details::Vec4Traits< ValueT >::GetName(), p_writer, p_name )
	{
	}

	template< typename ValueT >
	inline Vec4T< ValueT > & Vec4T< ValueT >::operator=( Vec4T< ValueT > const & p_rhs )
	{
		if ( m_writer )
		{
			m_writer->WriteAssign( *this, p_rhs );
		}
		else
		{
			Type::operator=( p_rhs );
			m_writer = p_rhs.m_writer;
		}

		return *this;
	}

	template< typename ValueT >
	template< typename RhsT >
	inline Vec4T< ValueT > & Vec4T< ValueT >::operator=( RhsT const & p_rhs )
	{
		UpdateWriter( p_rhs );
		m_writer->WriteAssign( *this, p_rhs );
		return *this;
	}

	template< typename ValueT >
	template< typename IndexT >
	inline ValueT Vec4T< ValueT >::operator[]( IndexT const & p_rhs )const
	{
		ValueT l_return{ m_writer, Castor::String( *this ) + cuT( "[" ) + Castor::String( p_rhs ) + cuT( "]" ) };
		return l_return;
	}

	template< typename ValueT >
	inline ValueT Vec4T< ValueT >::operator[]( int const & p_rhs )const
	{
		ValueT l_return{ m_writer, Castor::String( *this ) + cuT( "[" ) + Castor::string::to_string( p_rhs ) + cuT( "]" ) };
		return l_return;
	}

	//*****************************************************************************************
}
