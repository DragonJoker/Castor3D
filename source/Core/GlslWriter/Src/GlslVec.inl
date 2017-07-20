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
				static xchar const * const name{ "vec2 " };
				return name;
			}
		};

		template<>
		struct Vec2Traits< Int >
		{
			static xchar const * const GetName()
			{
				static xchar const * const name{ "ivec2 " };
				return name;
			}
		};

		template<>
		struct Vec2Traits< Boolean >
		{
			static xchar const * const GetName()
			{
				static xchar const * const name{ "bvec2 " };
				return name;
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
				static xchar const * const name{ "vec3 " };
				return name;
			}
		};

		template<>
		struct Vec3Traits< Int >
		{
			static xchar const * const GetName()
			{
				static xchar const * const name{ "ivec3 " };
				return name;
			}
		};

		template<>
		struct Vec3Traits< Boolean >
		{
			static xchar const * const GetName()
			{
				static xchar const * const name{ "bvec3 " };
				return name;
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
				static xchar const * const name{ "vec4 " };
				return name;
			}
		};

		template<>
		struct Vec4Traits< Int >
		{
			static xchar const * const GetName()
			{
				static xchar const * const name{ "ivec4 " };
				return name;
			}
		};

		template<>
		struct Vec4Traits< Boolean >
		{
			static xchar const * const GetName()
			{
				static xchar const * const name{ "bvec4 " };
				return name;
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
	template< typename IndexT >
	inline ValueT Vec2T< ValueT >::operator[]( IndexT const & p_rhs )const
	{
		ValueT result{ m_writer, Castor::String( *this ) + cuT( "[" ) + Castor::String( p_rhs ) + cuT( "]" ) };
		return result;
	}

	template< typename ValueT >
	inline ValueT Vec2T< ValueT >::operator[]( int const & p_rhs )const
	{
		ValueT result{ m_writer, Castor::String( *this ) + cuT( "[" ) + Castor::string::to_string( p_rhs ) + cuT( "]" ) };
		return result;
	}

	template< typename ValueT >
	inline Vec2T< ValueT > operator*( ValueT const & p_lhs, Vec2T< ValueT > const & p_rhs )
	{
		Vec2T< ValueT > result( p_lhs.m_writer );
		result.m_value << Castor::String( p_lhs ) << " * " << Castor::String( p_rhs );
		return result;
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
	template< typename IndexT >
	inline ValueT Vec3T< ValueT >::operator[]( IndexT const & p_rhs )const
	{
		ValueT result{ m_writer, Castor::String( *this ) + cuT( "[" ) + Castor::String( p_rhs ) + cuT( "]" ) };
		return result;
	}

	template< typename ValueT >
	inline ValueT Vec3T< ValueT >::operator[]( int const & p_rhs )const
	{
		ValueT result{ m_writer, Castor::String( *this ) + cuT( "[" ) + Castor::string::to_string( p_rhs ) + cuT( "]" ) };
		return result;
	}

	template< typename ValueT >
	inline Vec3T< ValueT > operator*( ValueT const & p_lhs, Vec3T< ValueT > const & p_rhs )
	{
		Vec3T< ValueT > result( p_lhs.m_writer );
		result.m_value << Castor::String( p_lhs ) << " * " << Castor::String( p_rhs );
		return result;
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
	template< typename IndexT >
	inline ValueT Vec4T< ValueT >::operator[]( IndexT const & p_rhs )const
	{
		ValueT result{ m_writer, Castor::String( *this ) + cuT( "[" ) + Castor::String( p_rhs ) + cuT( "]" ) };
		return result;
	}

	template< typename ValueT >
	inline ValueT Vec4T< ValueT >::operator[]( int const & p_rhs )const
	{
		ValueT result{ m_writer, Castor::String( *this ) + cuT( "[" ) + Castor::string::to_string( p_rhs ) + cuT( "]" ) };
		return result;
	}

	template< typename ValueT >
	inline Vec4T< ValueT > operator*( ValueT const & p_lhs, Vec4T< ValueT > const & p_rhs )
	{
		Vec4T< ValueT > result( p_lhs.m_writer );
		result.m_value << Castor::String( p_lhs ) << " * " << Castor::String( p_rhs );
		return result;
	}

	//*****************************************************************************************
}
