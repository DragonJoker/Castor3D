namespace GLSL
{
	//*****************************************************************************************

	namespace details
	{
		template< typename ValueT >
		struct Mat2Traits;

		template<>
		struct Mat2Traits< Float >
		{
			static xchar const * const getName()
			{
				static xchar const * const name{ "mat2 " };
				return name;
			}
		};

		template<>
		struct Mat2Traits< Int >
		{
			static xchar const * const getName()
			{
				static xchar const * const name{ "imat2 " };
				return name;
			}
		};

		template<>
		struct Mat2Traits< Boolean >
		{
			static xchar const * const getName()
			{
				static xchar const * const name{ "bmat2 " };
				return name;
			}
		};

		//*****************************************************************************************

		template< typename ValueT >
		struct Mat3Traits;

		template<>
		struct Mat3Traits< Float >
		{
			static xchar const * const getName()
			{
				static xchar const * const name{ "mat3 " };
				return name;
			}
		};

		template<>
		struct Mat3Traits< Int >
		{
			static xchar const * const getName()
			{
				static xchar const * const name{ "imat3 " };
				return name;
			}
		};

		template<>
		struct Mat3Traits< Boolean >
		{
			static xchar const * const getName()
			{
				static xchar const * const name{ "bmat3 " };
				return name;
			}
		};

		//*****************************************************************************************

		template< typename ValueT >
		struct Mat4Traits;

		template<>
		struct Mat4Traits< Float >
		{
			static xchar const * const getName()
			{
				static xchar const * const name{ "mat4 " };
				return name;
			}
		};

		template<>
		struct Mat4Traits< Int >
		{
			static xchar const * const getName()
			{
				static xchar const * const name{ "imat4 " };
				return name;
			}
		};

		template<>
		struct Mat4Traits< Boolean >
		{
			static xchar const * const getName()
			{
				static xchar const * const name{ "bmat4 " };
				return name;
			}
		};
	}

	//*****************************************************************************************

	template< typename ValueT >
	Mat2T< ValueT >::Mat2T()
		: Type( details::Mat2Traits< ValueT >::getName() )
	{
	}

	template< typename ValueT >
	Mat2T< ValueT >::Mat2T( GlslWriter * p_writer, castor::String const & p_name )
		: Type( details::Mat2Traits< ValueT >::getName(), p_writer, p_name )
	{
	}

	template< typename ValueT >
	Mat2T< ValueT > & Mat2T< ValueT >::operator=( Mat2T< ValueT > const & p_rhs )
	{
		if ( m_writer )
		{
			m_writer->writeAssign( *this, p_rhs );
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
	Mat2T< ValueT > & Mat2T< ValueT >::operator=( RhsT const & p_rhs )
	{
		updateWriter( p_rhs );
		m_writer->writeAssign( *this, p_rhs );
		return *this;
	}

	template< typename ValueT >
	template< typename IndexT >
	Vec2T< ValueT > Mat2T< ValueT >::operator[]( IndexT const & p_rhs )const
	{
		Vec2T< ValueT > result{ m_writer, castor::String( *this ) + cuT( "[" ) + castor::String( p_rhs ) + cuT( "]" ) };
		return result;
	}

	template< typename ValueT >
	Vec2T< ValueT > Mat2T< ValueT >::operator[]( int const & p_rhs )const
	{
		Vec2T< ValueT > result{ m_writer, castor::String( *this ) + cuT( "[" ) + castor::string::toString( p_rhs ) + cuT( "]" ) };
		return result;
	}

	//*****************************************************************************************

	template< typename ValueT >
	Mat3T< ValueT >::Mat3T()
		: Type( details::Mat3Traits< ValueT >::getName() )
	{
	}

	template< typename ValueT >
	Mat3T< ValueT >::Mat3T( GlslWriter * p_writer, castor::String const & p_name )
		: Type( details::Mat3Traits< ValueT >::getName(), p_writer, p_name )
	{
	}

	template< typename ValueT >
	Mat3T< ValueT > & Mat3T< ValueT >::operator=( Mat3T< ValueT > const & p_rhs )
	{
		if ( m_writer )
		{
			m_writer->writeAssign( *this, p_rhs );
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
	Mat3T< ValueT > & Mat3T< ValueT >::operator=( RhsT const & p_rhs )
	{
		updateWriter( p_rhs );
		m_writer->writeAssign( *this, p_rhs );
		return *this;
	}

	template< typename ValueT >
	template< typename IndexT >
	Vec3T< ValueT > Mat3T< ValueT >::operator[]( IndexT const & p_rhs )const
	{
		Vec3T< ValueT > result{ m_writer, castor::String( *this ) + cuT( "[" ) + castor::String( p_rhs ) + cuT( "]" ) };
		return result;
	}

	template< typename ValueT >
	Vec3T< ValueT > Mat3T< ValueT >::operator[]( int const & p_rhs )const
	{
		Vec3T< ValueT > result{ m_writer, castor::String( *this ) + cuT( "[" ) + castor::string::toString( p_rhs ) + cuT( "]" ) };
		return result;
	}

	//*****************************************************************************************

	template< typename ValueT >
	Mat4T< ValueT >::Mat4T()
		: Type( details::Mat4Traits< ValueT >::getName() )
	{
	}

	template< typename ValueT >
	Mat4T< ValueT >::Mat4T( GlslWriter * p_writer, castor::String const & p_name )
		: Type( details::Mat4Traits< ValueT >::getName(), p_writer, p_name )
	{
	}

	template< typename ValueT >
	Mat4T< ValueT > & Mat4T< ValueT >::operator=( Mat4T< ValueT > const & p_rhs )
	{
		if ( m_writer )
		{
			m_writer->writeAssign( *this, p_rhs );
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
	Mat4T< ValueT > & Mat4T< ValueT >::operator=( RhsT const & p_rhs )
	{
		updateWriter( p_rhs );
		m_writer->writeAssign( *this, p_rhs );
		return *this;
	}

	template< typename ValueT >
	template< typename IndexT >
	Vec4T< ValueT > Mat4T< ValueT >::operator[]( IndexT const & p_rhs )const
	{
		Vec4T< ValueT > result{ m_writer, castor::String( *this ) + cuT( "[" ) + castor::String( p_rhs ) + cuT( "]" ) };
		return result;
	}

	template< typename ValueT >
	Vec4T< ValueT > Mat4T< ValueT >::operator[]( int const & p_rhs )const
	{
		Vec4T< ValueT > result{ m_writer, castor::String( *this ) + cuT( "[" ) + castor::string::toString( p_rhs ) + cuT( "]" ) };
		return result;
	}

	//*****************************************************************************************
}
