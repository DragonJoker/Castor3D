namespace glsl
{
	namespace
	{
		template< typename T, typename U > struct are_equal : public std::false_type
		{
		};

		template< typename T > struct are_equal< T, T > : public std::true_type
		{
		};

		template< typename TInput, typename TOutput, typename UInput, typename UOutput, typename Enable = void >
		struct SwizzleOperator
		{
		};

		template< typename TInput, typename TOutput, typename UInput, typename UOutput >
		struct SwizzleOperator < TInput, TOutput, UInput, UOutput, typename std::enable_if < are_equal< TOutput, UOutput >::value && !are_equal< TOutput, Float >::value && !are_equal< TOutput, Int >::value >::type >
		{
			Swizzle< TInput, TOutput > & operator()( castor::String const & p_operator, Swizzle< TInput, TOutput > & p_lhs, Swizzle< UInput, TOutput > const & p_rhs )
			{
				p_lhs.updateWriter( *p_lhs.m_input );
				*p_lhs.m_writer << p_lhs.m_value.str() << p_operator << castor::String( static_cast< TOutput >( p_rhs ) ) << cuT( ";" ) << glsl::endl;
				return p_lhs;
			}
		};

		template< typename TInput, typename TOutput, typename UInput, typename UOutput >
		struct SwizzleOperator< TInput, TOutput, UInput, UOutput, typename std::enable_if< are_equal< TOutput, Float >::value >::type >
		{
			Swizzle< TInput, TOutput > & operator()( castor::String const & p_operator, Swizzle< TInput, TOutput > & p_lhs, Swizzle< UInput, Float > const & p_rhs )
			{
				p_lhs.updateWriter( *p_lhs.m_input );
				*p_lhs.m_writer << p_lhs.m_value.str() << p_operator << castor::String( static_cast< Float >( p_rhs ) ) << cuT( ";" ) << glsl::endl;
				return p_lhs;
			}
		};

		template< typename TInput, typename TOutput, typename UInput, typename UOutput >
		struct SwizzleOperator< TInput, TOutput, UInput, UOutput, typename std::enable_if< are_equal< TOutput, Int >::value >::type >
		{
			Swizzle< TInput, TOutput > & operator()( castor::String const & p_operator, Swizzle< TInput, TOutput > & p_lhs, Swizzle< UInput, Int > const & p_rhs )
			{
				p_lhs.updateWriter( *p_lhs.m_input );
				*p_lhs.m_writer << p_lhs.m_value.str() << p_operator << castor::String( static_cast< Int >( p_rhs ) ) << cuT( ";" ) << glsl::endl;
				return p_lhs;
			}
		};

		template< typename Input, typename Output, typename T >
		struct SwizzleOperator< Input, Output, T, T >
		{
			Swizzle< Input, Output > & operator()( castor::String const & p_operator, Swizzle< Input, Output > & p_lhs, T p_rhs )
			{
				p_lhs.updateWriter( *p_lhs.m_input );
				*p_lhs.m_writer << p_lhs.m_value.str() << p_operator << castor::string::toString( p_rhs, Expr::getLocale() ) << cuT( ";" ) << glsl::endl;
				return p_lhs;
			}
		};
	}

	template< typename Input, typename Output >
	Swizzle< Input, Output >::Swizzle( castor::String const & p_name, Input * p_input )
		: Expr( p_input->m_writer, p_name )
		, m_input( p_input )
	{
	}

	template< typename Input, typename Output >
	Swizzle< Input, Output > & Swizzle< Input, Output >::operator=( Swizzle const & p_rhs )
	{
		if ( m_writer )
		{
			m_writer->writeAssign( *this, p_rhs );
		}
		else
		{
			Expr::operator=( p_rhs );
			m_writer = p_rhs.m_writer;
		}

		return *this;
	}

	template< typename Input, typename Output >
	template< typename T >
	inline Swizzle< Input, Output > & Swizzle< Input, Output >::operator=( T const & p_rhs )
	{
		updateWriter( p_rhs );
		m_writer->writeAssign( *this, p_rhs );
		return *this;
	}

	template< typename Input, typename Output >
	Swizzle< Input, Output >::operator Output()const
	{
		castor::String result = m_input->m_value.str();

		if ( result.empty() )
		{
			if ( m_input->m_name.empty() )
			{
				result = m_input->m_type;
			}
			else
			{
				result = m_input->m_name;
			}
		}

		Output output( m_input->m_writer, result );
		output.m_value << output.m_name << "." << m_value.rdbuf();
		return output;
	}

	template< typename Input, typename Output >
	template< typename UInput, typename UOutput >
	Swizzle< Input, Output > & Swizzle< Input, Output >::operator+=( Swizzle< UInput, UOutput > const & p_type )
	{
		return SwizzleOperator< Input, Output, UInput, UOutput >()( cuT( " += " ), *this, p_type );
	}

	template< typename Input, typename Output >
	template< typename UInput, typename UOutput >
	Swizzle< Input, Output > & Swizzle< Input, Output >::operator-=( Swizzle< UInput, UOutput > const & p_type )
	{
		return SwizzleOperator< Input, Output, UInput, UOutput >()( cuT( " -= " ), *this, p_type );
	}

	template< typename Input, typename Output >
	template< typename UInput, typename UOutput >
	Swizzle< Input, Output > & Swizzle< Input, Output >::operator*=( Swizzle< UInput, UOutput > const & p_type )
	{
		return SwizzleOperator< Input, Output, UInput, UOutput >()( cuT( " *= " ), *this, p_type );
	}

	template< typename Input, typename Output >
	template< typename UInput, typename UOutput >
	Swizzle< Input, Output > & Swizzle< Input, Output >::operator/=( Swizzle< UInput, UOutput > const & p_type )
	{
		return SwizzleOperator< Input, Output, UInput, UOutput >()( cuT( " /= " ), *this, p_type );
	}

	template< typename Input, typename Output >
	Swizzle< Input, Output > & Swizzle< Input, Output >::operator+=( float p_type )
	{
		return SwizzleOperator< Input, Output, float, float >()( cuT( " += " ), *this, p_type );
	}

	template< typename Input, typename Output >
	Swizzle< Input, Output > & Swizzle< Input, Output >::operator-=( float p_type )
	{
		return SwizzleOperator< Input, Output, float, float >()( cuT( " -= " ), *this, p_type );
	}

	template< typename Input, typename Output >
	Swizzle< Input, Output > & Swizzle< Input, Output >::operator*=( float p_type )
	{
		return SwizzleOperator< Input, Output, float, float >()( cuT( " *= " ), *this, p_type );
	}

	template< typename Input, typename Output >
	Swizzle< Input, Output > & Swizzle< Input, Output >::operator/=( float p_type )
	{
		return SwizzleOperator< Input, Output, float, float >()( cuT( " /= " ), *this, p_type );
	}

	template< typename Input, typename Output >
	Swizzle< Input, Output > & Swizzle< Input, Output >::operator+=( int p_type )
	{
		return SwizzleOperator< Input, Output, float, float >()( cuT( " += " ), *this, p_type );
	}

	template< typename Input, typename Output >
	Swizzle< Input, Output > & Swizzle< Input, Output >::operator-=( int p_type )
	{
		return SwizzleOperator< Input, Output, float, float >()( cuT( " -= " ), *this, p_type );
	}

	template< typename Input, typename Output >
	Swizzle< Input, Output > & Swizzle< Input, Output >::operator*=( int p_type )
	{
		return SwizzleOperator< Input, Output, float, float >()( cuT( " *= " ), *this, p_type );
	}

	template< typename Input, typename Output >
	Swizzle< Input, Output > & Swizzle< Input, Output >::operator/=( int p_type )
	{
		return SwizzleOperator< Input, Output, float, float >()( cuT( " /= " ), *this, p_type );
	}

	//*****************************************************************************************

	template< typename TInput, typename UInput, typename Output >
	Output operator+( Swizzle< TInput, Output > const & p_a, Swizzle< UInput, Output > const & p_b )
	{
		Output result( p_a.m_input->m_writer, p_a.m_input->m_type );
		result.m_value << castor::String( static_cast< Output >( p_a ) ) << cuT( " + " ) << castor::String( static_cast< Output >( p_b ) );
		return result;
	}

	template< typename TInput, typename UInput, typename Output >
	Output operator-( Swizzle< TInput, Output > const & p_a, Swizzle< UInput, Output > const & p_b )
	{
		Output result( p_a.m_input->m_writer, p_a.m_input->m_type );
		result.m_value << castor::String( static_cast< Output >( p_a ) ) << cuT( " - " ) << castor::String( static_cast< Output >( p_b ) );
		return result;
	}

	template< typename TInput, typename UInput, typename Output >
	Output operator*( Swizzle< TInput, Output > const & p_a, Swizzle< UInput, Output > const & p_b )
	{
		Output result( p_a.m_input->m_writer, p_a.m_input->m_type );
		result.m_value << castor::String( static_cast< Output >( p_a ) ) << cuT( " * " ) << castor::String( static_cast< Output >( p_b ) );
		return result;
	}

	template< typename TInput, typename UInput, typename Output >
	Output operator/( Swizzle< TInput, Output > const & p_a, Swizzle< UInput, Output > const & p_b )
	{
		Output result( p_a.m_input->m_writer, p_a.m_input->m_type );
		result.m_value << castor::String( static_cast< Output >( p_a ) ) << cuT( " / " ) << castor::String( static_cast< Output >( p_b ) );
		return result;
	}
	template< typename TInput, typename UInput, typename Output >
	Output operator+( Swizzle< TInput, Output > const & p_a, Swizzle< UInput, Float > const & p_b )
	{
		Output result( p_a.m_input->m_writer, p_a.m_input->m_type );
		result.m_value << castor::String( static_cast< Output >( p_a ) ) << cuT( " + " ) << castor::String( static_cast< Float >( p_b ) );
		return result;
	}

	template< typename TInput, typename UInput, typename Output >
	Output operator-( Swizzle< TInput, Output > const & p_a, Swizzle< UInput, Float > const & p_b )
	{
		Output result( p_a.m_input->m_writer, p_a.m_input->m_type );
		result.m_value << castor::String( static_cast< Output >( p_a ) ) << cuT( " - " ) << castor::String( static_cast< Float >( p_b ) );
		return result;
	}

	template< typename TInput, typename UInput, typename Output >
	Output operator*( Swizzle< TInput, Output > const & p_a, Swizzle< UInput, Float > const & p_b )
	{
		Output result( p_a.m_input->m_writer, p_a.m_input->m_type );
		result.m_value << castor::String( static_cast< Output >( p_a ) ) << cuT( " * " ) << castor::String( static_cast< Float >( p_b ) );
		return result;
	}

	template< typename TInput, typename UInput, typename Output >
	Output operator/( Swizzle< TInput, Output > const & p_a, Swizzle< UInput, Float > const & p_b )
	{
		Output result( p_a.m_input->m_writer, p_a.m_input->m_type );
		result.m_value << castor::String( static_cast< Output >( p_a ) ) << cuT( " / " ) << castor::String( static_cast< Float >( p_b ) );
		return result;
	}

	template< typename TInput, typename UInput, typename Output >
	Output operator+( Swizzle< TInput, Output > const & p_a, Swizzle< UInput, Int > const & p_b )
	{
		Output result( p_a.m_input->m_writer, p_a.m_input->m_type );
		result.m_value << castor::String( static_cast< Output >( p_a ) ) << cuT( " + " ) << castor::String( static_cast< Int >( p_b ) );
		return result;
	}

	template< typename TInput, typename UInput, typename Output >
	Output operator-( Swizzle< TInput, Output > const & p_a, Swizzle< UInput, Int > const & p_b )
	{
		Output result( p_a.m_input->m_writer, p_a.m_input->m_type );
		result.m_value << castor::String( static_cast< Output >( p_a ) ) << cuT( " - " ) << castor::String( static_cast< Int >( p_b ) );
		return result;
	}

	template< typename TInput, typename UInput, typename Output >
	Output operator*( Swizzle< TInput, Output > const & p_a, Swizzle< UInput, Int > const & p_b )
	{
		Output result( p_a.m_input->m_writer, p_a.m_input->m_type );
		result.m_value << castor::String( static_cast< Output >( p_a ) ) << cuT( " * " ) << castor::String( static_cast< Int >( p_b ) );
		return result;
	}

	template< typename TInput, typename UInput, typename Output >
	Output operator/( Swizzle< TInput, Output > const & p_a, Swizzle< UInput, Int > const & p_b )
	{
		Output result( p_a.m_input->m_writer, p_a.m_input->m_type );
		result.m_value << castor::String( static_cast< Output >( p_a ) ) << cuT( " / " ) << castor::String( static_cast< Int >( p_b ) );
		return result;
	}

	template< typename Input, typename Output >
	Output operator+( Swizzle< Input, Output > const & p_a, float p_b )
	{
		Output result( p_a.m_input->m_writer, p_a.m_input->m_type );
		result.m_value << castor::String( static_cast< Output >( p_a ) ) << cuT( " + " ) << castor::string::toString( p_b, Expr::getLocale() );
		return result;
	}

	template< typename Input, typename Output >
	Output operator-( Swizzle< Input, Output > const & p_a, float p_b )
	{
		Output result( p_a.m_input->m_writer, p_a.m_input->m_type );
		result.m_value << castor::String( static_cast< Output >( p_a ) ) << cuT( " - " ) << castor::string::toString( p_b, Expr::getLocale() );
		return result;
	}

	template< typename Input, typename Output >
	Output operator*( Swizzle< Input, Output > const & p_a, float p_b )
	{
		Output result( p_a.m_input->m_writer, p_a.m_input->m_type );
		result.m_value << castor::String( static_cast< Output >( p_a ) ) << cuT( " * " ) << castor::string::toString( p_b, Expr::getLocale() );
		return result;
	}

	template< typename Input, typename Output >
	Output operator/( Swizzle< Input, Output > const & p_a, float p_b )
	{
		Output result( p_a.m_input->m_writer, p_a.m_input->m_type );
		result.m_value << castor::String( static_cast< Output >( p_a ) ) << cuT( " / " ) << castor::string::toString( p_b, Expr::getLocale() );
		return result;
	}

	template< typename Input, typename Output >
	Output operator+( Swizzle< Input, Output > const & p_a, int p_b )
	{
		Output result( p_a.m_input->m_writer, p_a.m_input->m_type );
		result.m_value << castor::String( static_cast< Output >( p_a ) ) << cuT( " + " ) << castor::string::toString( p_b, 10, Expr::getLocale() );
		return result;
	}

	template< typename Input, typename Output >
	Output operator-( Swizzle< Input, Output > const & p_a, int p_b )
	{
		Output result( p_a.m_input->m_writer, p_a.m_input->m_type );
		result.m_value << castor::String( static_cast< Output >( p_a ) ) << cuT( " - " ) << castor::string::toString( p_b, 10, Expr::getLocale() );
		return result;
	}

	template< typename Input, typename Output >
	Output operator*( Swizzle< Input, Output > const & p_a, int p_b )
	{
		Output result( p_a.m_input->m_writer, p_a.m_input->m_type );
		result.m_value << castor::String( static_cast< Output >( p_a ) ) << cuT( " * " ) << castor::string::toString( p_b, 10, Expr::getLocale() );
		return result;
	}

	template< typename Input, typename Output >
	Output operator/( Swizzle< Input, Output > const & p_a, int p_b )
	{
		Output result( p_a.m_input->m_writer, p_a.m_input->m_type );
		result.m_value << castor::String( static_cast< Output >( p_a ) ) << cuT( " / " ) << castor::string::toString( p_b, 10, Expr::getLocale() );
		return result;
	}

	//*****************************************************************************************
}
