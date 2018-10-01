namespace glsl
{
	//***********************************************************************************************

	namespace details
	{
		template< typename TypeR, typename TypeA, typename TypeB >
		struct Operator
		{
			using Ret = TypeR;
			using T = TypeA;
			using U = TypeB;

			static Ret write( char const * const p_operator, T const & lhs, U const & rhs )
			{
				Ret result( findWriter( lhs, rhs ) );
				result.m_value << castor::String( lhs ) << ' ' << p_operator << ' ' << toString( rhs );
				return result;
			}
		};

		template< typename TypeR, typename TypeA, typename TypeB >
		struct Operator< Optional< TypeR >, Optional< TypeA >, TypeB >
		{
			using Ret = Optional< TypeR >;
			using T = Optional< TypeA >;
			using U = TypeB;

			static Ret write( char const * const p_operator, T const & lhs, U const & rhs )
			{
				Ret result( findWriter( lhs, rhs ), castor::String(), lhs.isEnabled() );
				result.m_value << castor::String( lhs ) << ' ' << p_operator << ' ' << toString( rhs );
				return result;
			}
		};

		template< typename TypeR, typename TypeA, typename TypeB >
		struct Operator< Optional< TypeR >, TypeA, Optional< TypeB > >
		{
			using Ret = Optional< TypeR >;
			using T = TypeA;
			using U = Optional< TypeB >;

			static Ret write( char const * const p_operator, T const & lhs, U const & rhs )
			{
				Ret result( findWriter( lhs, rhs ), castor::String(), rhs.isEnabled() );
				result.m_value << castor::String( lhs ) << ' ' << p_operator << ' ' << toString( rhs );
				return result;
			}
		};

		template< typename TypeA, typename TypeB >
		struct ArithmeticOperator
			: public Operator< TypeA, TypeA, TypeB >
		{
			using OperatorType = Operator< TypeA, TypeA, TypeB >;
			using Ret = typename OperatorType::Ret;
			using T = typename OperatorType::T;
			using U = typename OperatorType::U;
		};

		template< typename TypeA, typename TypeB >
		struct ArithmeticOperator< Optional< TypeA >, TypeB >
			: public Operator< Optional< TypeA >, Optional< TypeA >, TypeB >
		{
			using OperatorType = Operator< Optional< TypeA >, Optional< TypeA >, TypeB >;
			using Ret = typename OperatorType::Ret;
			using T = typename OperatorType::T;
			using U = typename OperatorType::U;
		};

		template< typename TypeA, typename TypeB >
		struct ArithmeticOperator< TypeA, Optional< TypeB > >
			: public Operator< Optional< TypeA >, TypeA, Optional< TypeB > >
		{
			using OperatorType = Operator< Optional< TypeA >, TypeA, Optional< TypeB > >;
			using Ret = typename OperatorType::Ret;
			using T = typename OperatorType::T;
			using U = typename OperatorType::U;
		};

		template< typename TypeA, typename TypeB >
		struct ArithmeticOperator< Optional< TypeA >, Optional< TypeB > >
			: public Operator< Optional< TypeA >, Optional< TypeA >, Optional< TypeB > >
		{
			using OperatorType = Operator< Optional< TypeA >, Optional< TypeA >, Optional< TypeB > >;
			using Ret = typename OperatorType::Ret;
			using T = typename OperatorType::T;
			using U = typename  OperatorType::U;
		};

		template< typename TypeA, typename TypeB >
		struct BooleanOperator
			: public Operator< Boolean, TypeA, TypeB >
		{
			using OperatorType = Operator< Boolean, TypeA, TypeB >;
			using Ret = typename OperatorType::Ret;
			using T = typename OperatorType::T;
			using U = typename  OperatorType::U;
		};

		template< typename V, typename U, typename Enable = void >
		struct Step;

		template< typename V, typename U >
		struct Step< V, U, typename std::enable_if< std::is_same< U, V >::value >::type >
		{
			static V step( V const & edge, U const & x )
			{
				return writeFunctionCall< V >( findWriter( edge, x ), cuT( "step" ), edge, x );
			}
		};

		template< typename V, typename U >
		struct Step< V, U, typename std::enable_if< std::is_same< U, Float >::value && !std::is_same< V, Float >::value >::type >
		{
			static V step( V const & edge, U const & x )
			{
				return writeFunctionCall< V >( findWriter( edge, x ), cuT( "step" ), edge, x );
			}
		};
	}

#	define GLSL_IMPLEMENT_OPERATOR( RetType, LhsType, RhsType, Operator, OperatorType )\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( LhsType const & lhs, RhsType const & rhs )\
	{\
		return details::OperatorType< LhsType, RhsType >::write( #Operator, lhs, rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( LhsType const & lhs, InParam< RhsType > const & rhs )\
	{\
		return details::OperatorType< LhsType, RhsType >::write( #Operator, lhs, rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( LhsType const & lhs, InOutParam< RhsType > const & rhs )\
	{\
		return details::OperatorType< LhsType, RhsType >::write( #Operator, lhs, rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( LhsType const & lhs, OutParam< RhsType > const & rhs )\
	{\
		return details::OperatorType< LhsType, RhsType >::write( #Operator, lhs, rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( LhsType const & lhs, Optional< RhsType > const & rhs )\
	{\
		return details::OperatorType< LhsType, RhsType >::write( #Operator, lhs, rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( InParam< LhsType > const & lhs, RhsType const & rhs )\
	{\
		return details::OperatorType< LhsType, RhsType >::write( #Operator, lhs, rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( InParam< LhsType > const & lhs, InParam< RhsType > const & rhs )\
	{\
		return details::OperatorType< LhsType, RhsType >::write( #Operator, lhs, rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( InParam< LhsType > const & lhs, InOutParam< RhsType > const & rhs )\
	{\
		return details::OperatorType< LhsType, RhsType >::write( #Operator, lhs, rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( InParam< LhsType > const & lhs, OutParam< RhsType > const & rhs )\
	{\
		return details::OperatorType< LhsType, RhsType >::write( #Operator, lhs, rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( InParam< LhsType > const & lhs, Optional< RhsType > const & rhs )\
	{\
		return details::OperatorType< LhsType, RhsType >::write( #Operator, lhs, rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( InOutParam< LhsType > const & lhs, RhsType const & rhs )\
	{\
		return details::OperatorType< LhsType, RhsType >::write( #Operator, lhs, rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( InOutParam< LhsType > const & lhs, InParam< RhsType > const & rhs )\
	{\
		return details::OperatorType< LhsType, RhsType >::write( #Operator, lhs, rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( InOutParam< LhsType > const & lhs, InOutParam< RhsType > const & rhs )\
	{\
		return details::OperatorType< LhsType, RhsType >::write( #Operator, lhs, rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( InOutParam< LhsType > const & lhs, OutParam< RhsType > const & rhs )\
	{\
		return details::OperatorType< LhsType, RhsType >::write( #Operator, lhs, rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( InOutParam< LhsType > const & lhs, Optional< RhsType > const & rhs )\
	{\
		return details::OperatorType< LhsType, RhsType >::write( #Operator, lhs, rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( OutParam< LhsType > const & lhs, RhsType const & rhs )\
	{\
		return details::OperatorType< LhsType, RhsType >::write( #Operator, lhs, rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( OutParam< LhsType > const & lhs, InParam< RhsType > const & rhs )\
	{\
		return details::OperatorType< LhsType, RhsType >::write( #Operator, lhs, rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( OutParam< LhsType > const & lhs, InOutParam< RhsType > const & rhs )\
	{\
		return details::OperatorType< LhsType, RhsType >::write( #Operator, lhs, rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( OutParam< LhsType > const & lhs, OutParam< RhsType > const & rhs )\
	{\
		return details::OperatorType< LhsType, RhsType >::write( #Operator, lhs, rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( OutParam< LhsType > const & lhs, Optional< RhsType > const & rhs )\
	{\
		return details::OperatorType< LhsType, RhsType >::write( #Operator, lhs, rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( Optional< LhsType > const & lhs, RhsType const & rhs )\
	{\
		return details::OperatorType< LhsType, RhsType >::write( #Operator, lhs, rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( Optional< LhsType > const & lhs, InParam< RhsType > const & rhs )\
	{\
		return details::OperatorType< LhsType, RhsType >::write( #Operator, lhs, rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( Optional< LhsType > const & lhs, InOutParam< RhsType > const & rhs )\
	{\
		return details::OperatorType< LhsType, RhsType >::write( #Operator, lhs, rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( Optional< LhsType > const & lhs, OutParam< RhsType > const & rhs )\
	{\
		return details::OperatorType< LhsType, RhsType >::write( #Operator, lhs, rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( Optional< LhsType > const & lhs, Optional< RhsType > const & rhs )\
	{\
		return details::OperatorType< LhsType, RhsType >::write( #Operator, lhs, rhs );\
	}

#	define GLSL_IMPLEMENT_ARITHMETIC_OPERATOR( LhsType, RhsType, Operator )\
	GLSL_IMPLEMENT_OPERATOR( LhsType, LhsType, RhsType, Operator, ArithmeticOperator )

#	define GLSL_IMPLEMENT_BOOLEAN_OPERATOR( LhsType, RhsType, Operator )\
	GLSL_IMPLEMENT_OPERATOR( Boolean, LhsType, RhsType, Operator, BooleanOperator )

	GLSL_IMPLEMENT_ARITHMETIC_OPERATOR( TypeA, TypeB, + )
	GLSL_IMPLEMENT_ARITHMETIC_OPERATOR( TypeA, TypeB, - )
	GLSL_IMPLEMENT_ARITHMETIC_OPERATOR( TypeA, TypeB, * )
	GLSL_IMPLEMENT_ARITHMETIC_OPERATOR( TypeA, TypeB, / )
	GLSL_IMPLEMENT_ARITHMETIC_OPERATOR( TypeA, TypeB, ^ )
	GLSL_IMPLEMENT_BOOLEAN_OPERATOR( TypeA, TypeB, == )
	GLSL_IMPLEMENT_BOOLEAN_OPERATOR( TypeA, TypeB, != )
	GLSL_IMPLEMENT_BOOLEAN_OPERATOR( TypeA, TypeB, < )
	GLSL_IMPLEMENT_BOOLEAN_OPERATOR( TypeA, TypeB, <= )
	GLSL_IMPLEMENT_BOOLEAN_OPERATOR( TypeA, TypeB, > )
	GLSL_IMPLEMENT_BOOLEAN_OPERATOR( TypeA, TypeB, >= )

	//***********************************************************************************************

	template< typename ... Values >
	inline Float dot( Type const & value, Values const & ... values )
	{
		return writeFunctionCall< Float, Type, Values... >( findWriter( value, values... ), cuT( "dot" ), value, values... );
	}

	template< typename ... Values >
	inline Float inversesqrt( Type const & value, Values const & ... values )
	{
		return writeFunctionCall< Float, Type, Values... >( findWriter( value, values... ), cuT( "inversesqrt" ), value, values... );
	}

	template< typename Value, typename ... Values >
	inline Value cross( Value const & value, Values const & ... values )
	{
		return writeFunctionCall< Value, Type, Values... >( findWriter( value, values... ), cuT( "cross" ), value, values... );
	}

	template< typename Value, typename ... Values >
	inline Value clamp( Value const & value, Values const & ... values )
	{
		return writeFunctionCall< Value, Type, Values... >( findWriter( value, values... ), cuT( "clamp" ), value, values... );
	}

	template< typename Value, typename ... Values >
	inline Value min( Value const & value, Values const & ... values )
	{
		return writeFunctionCall< Value, Type, Values... >( findWriter( value, values... ), cuT( "min" ), value, values... );
	}

	template< typename Value, typename ... Values >
	inline Value max( Value const & value, Values const & ... values )
	{
		return writeFunctionCall< Value, Type, Values... >( findWriter( value, values... ), cuT( "max" ), value, values... );
	}

	template< typename Value, typename ... Values >
	inline Value mix( Value const & value, Values const & ... values )
	{
		return writeFunctionCall< Value, Type, Values... >( findWriter( value, values... ), cuT( "mix" ), value, values... );
	}

	template< typename Value, typename ... Values >
	inline Value reflect( Type const & value, Values const & ... values )
	{
		return writeFunctionCall< Value, Type, Values... >( findWriter( value, values... ), cuT( "reflect" ), value, values... );
	}

	template< typename Value >
	inline Value sqrt( Value const & value )
	{
		return writeFunctionCall< Value, Value >( findWriter( value ), cuT( "sqrt" ), value );
	}

	template< typename Value >
	inline Value pow( Value const & x, Value const & y )
	{
		return writeFunctionCall< Value >( x.m_writer, cuT( "pow" ), x, y );
	}

	template< typename Value >
	inline Value pow( Value const & x, Optional< Value > const & y )
	{
		return writeFunctionCall< Value >( x.m_writer, cuT( "pow" ), x, y );
	}

	template< typename Value >
	inline Value pow2( Value const & p )
	{
		return writeFunctionCall< Value >( p.m_writer, cuT( "pow2" ), p );
	}

	template< typename Value >
	Value normalize( Value const & value )
	{
		return writeFunctionCall< Value >( findWriter( value ), cuT( "normalize" ), value );
	}

	template< typename Input, typename Output >
	inline Output normalize( Swizzle< Input, Output > const & value )
	{
		return writeFunctionCall< Output >( findWriter( value ), cuT( "normalize" ), static_cast< Output const >( value ) );
	}

	template< typename Value >
	inline Value transpose( Value const & value )
	{
		return writeFunctionCall< Value >( findWriter( value ), cuT( "transpose" ), value );
	}

	template< typename Value >
	inline Value inverse( Value const & value )
	{
		return writeFunctionCall< Value >( findWriter( value ), cuT( "inverse" ), value );
	}

	template< typename Value >
	inline Value abs( Value const & value )
	{
		return writeFunctionCall< Value >( findWriter( value ), cuT( "abs" ), value );
	}

	template< typename Value >
	inline Value isinf( Value const & value )
	{
		return writeFunctionCall< Value >( findWriter( value ), cuT( "isinf" ), value );
	}

	template< typename ValueA, typename ValueB >
	ValueB smoothstep( ValueA const & edge0, ValueA const & edge1, ValueB const & x )
	{
		return writeFunctionCall< ValueB >( findWriter( edge0, edge1, x ), cuT( "smoothstep" ), edge0, edge1,x );
	}

	template< typename Value >
	inline Value dFdx( Value const & value )
	{
		return writeFunctionCall< Value >( findWriter( value ), cuT( "dFdx" ), value );
	}

	template< typename Value >
	inline Value dFdy( Value const & value )
	{
		return writeFunctionCall< Value >( findWriter( value ), cuT( "dFdy" ), value );
	}

	template< typename Value >
	inline Value dFdxCoarse( Value const & value )
	{
		return writeFunctionCall< Value >( findWriter( value ), cuT( "dFdyCoarse" ), value );
	}

	template< typename Value >
	inline Value dFdyCoarse( Value const & value )
	{
		return writeFunctionCall< Value >( findWriter( value ), cuT( "dFdyCoarse" ), value );
	}

	template< typename Value >
	inline Value dFdxFine( Value const & value )
	{
		return writeFunctionCall< Value >( findWriter( value ), cuT( "dFdxFine" ), value );
	}

	template< typename Value >
	inline Value dFdyFine( Value const & value )
	{
		return writeFunctionCall< Value >( findWriter( value ), cuT( "dFdyFine" ), value );
	}

	template< typename Value >
	inline Value sign( Value const & value )
	{
		return writeFunctionCall< Value >( findWriter( value ), cuT( "sign" ), value );
	}

	template< typename Value >
	inline Value mod( Value const & x, Value const & y )
	{
		return writeFunctionCall< Value >( x.m_writer, cuT( "mod" ), x, y );
	}

	template< typename Value >
	inline Value mod( Value const & x, Float const & y )
	{
		return writeFunctionCall< Value >( findWriter( x, y ), cuT( "mod" ), x, y );
	}

	template< typename Value >
	inline Value fract( Value const & value )
	{
		return writeFunctionCall< Value >( findWriter( value ), cuT( "fract" ), value );
	}

	template< typename Value >
	inline Value floor( Value const & value )
	{
		return writeFunctionCall< Value >( findWriter( value ), cuT( "floor" ), value );
	}

	template< typename Value >
	inline Value trunc( Value const & value )
	{
		return writeFunctionCall< Value >( findWriter( value ), cuT( "trunc" ), value );
	}

	template< typename Value >
	inline Value round( Value const & value )
	{
		return writeFunctionCall< Value >( findWriter( value ), cuT( "round" ), value );
	}

	template< typename ValueE, typename ValueX >
	inline ValueE step( ValueE const & edge, ValueX const & x )
	{
		return details::Step< ValueE, ValueX >::step( edge, x );
	}

	template< typename Value >
	inline Value step( Value const & edge, Float const & x )
	{
		return writeFunctionCall< Value >( findWriter( edge, x ), cuT( "step" ), edge, x );
	}

	template< typename Value >
	inline Value neg( Value const & value )
	{
		return writeFunctionCall< Value >( findWriter( value ), cuT( "-" ), value );
	}

	template< typename Input, typename Output >
	inline Output neg( Swizzle< Input, Output > const & value )
	{
		return writeFunctionCall< Output >( findWriter( value ), cuT( "-" ), static_cast< Output >( value ) );
	}

	template< typename Value >
	inline Value log( Value const & value )
	{
		return writeFunctionCall< Value >( findWriter( value ), cuT( "log" ), value );
	}

	template< typename Value >
	inline Value exp( Value const & value )
	{
		return writeFunctionCall< Value >( findWriter( value ), cuT( "exp" ), value );
	}

	template< typename Value >
	inline Value log2( Value const & value )
	{
		return writeFunctionCall< Value >( findWriter( value ), cuT( "log2" ), value );
	}

	template< typename Value >
	inline Value exp2( Value const & value )
	{
		return writeFunctionCall< Value >( findWriter( value ), cuT( "exp2" ), value );
	}

	template< typename Value >
	inline Value tan( Value const & value )
	{
		return writeFunctionCall< Value >( findWriter( value ), cuT( "tan" ), value );
	}

	template< typename Value >
	inline Value sin( Value const & value )
	{
		return writeFunctionCall< Value >( findWriter( value ), cuT( "sin" ), value );
	}

	template< typename Value >
	inline Value cos( Value const & value )
	{
		return writeFunctionCall< Value >( findWriter( value ), cuT( "cos" ), value );
	}

	template< typename Value >
	inline Value atan( Value const & value )
	{
		return writeFunctionCall< Value >( findWriter( value ), cuT( "atan" ), value );
	}

	template< typename Value >
	inline Value atan( Value const & y, Value const & x )
	{
		return writeFunctionCall< Value >( findWriter( y, x ), cuT( "atan" ), y, x );
	}

	template< typename Value >
	inline Value asin( Value const & value )
	{
		return writeFunctionCall< Value >( findWriter( value ), cuT( "asin" ), value );
	}

	template< typename Value >
	inline Value acos( Value const & value )
	{
		return writeFunctionCall< Value >( findWriter( value ), cuT( "acos" ), value );
	}

	template< typename Value >
	inline Value fma( Value const & a, Value const & b, Value const & c )
	{
		auto writer = findWriter( a, b, c );
		Value result{ writer };
		result.m_value << cuT( "((" ) << toString( a ) << cuT( ") * (" ) << toString( b ) << cuT( ") + (" ) << toString( c ) << cuT( "))" );
		return result;
	}

	template< typename Value >
	inline Value fma( Optional< Value > const & a, Value const & b, Value const & c )
	{
		auto writer = findWriter( a, b, c );

		if ( a.isEnabled() )
		{
			Value result{ writer };
			result.m_value << cuT( "((" ) << toString( a ) << cuT( ") * (" ) << toString( b ) << cuT( ") + (" ) << toString( c ) << cuT( "))" );
			return result;
		}
	}

	template< typename Value >
	inline Value fma( Value const & a, Optional< Value > const & b, Value const & c )
	{
		auto writer = findWriter( a, b, c );

		if ( b.isEnabled() )
		{
			Value result{ writer };
			result.m_value << cuT( "((" ) << toString( a ) << cuT( ") * (" ) << toString( b ) << cuT( ") + (" ) << toString( c ) << cuT( "))" );
			return result;
		}
	}

	template< typename Value >
	inline Value fma( Value const & a, Value const & b, Optional< Value > const & c )
	{
		auto writer = findWriter( a, b, c );

		if ( c.isEnabled() )
		{
			Value result{ writer };
			result.m_value << cuT( "((" ) << toString( a ) << cuT( ") * (" ) << toString( b ) << cuT( ") + (" ) << toString( c ) << cuT( "))" );
			return result;
		}
	}

	template< typename Value >
	inline Value fma( Optional< Value > const & a, Optional< Value > const & b, Value const & c )
	{
		auto writer = findWriter( a, b, c );

		if ( a.isEnabled() && b.isEnabled() )
		{
			Value result{ writer };
			result.m_value << cuT( "((" ) << toString( a ) << cuT( ") * (" ) << toString( b ) << cuT( ") + (" ) << toString( c ) << cuT( "))" );
			return result;
		}
	}

	template< typename Value >
	inline Value fma( Optional< Value > const & a, Value const & b, Optional< Value > const & c )
	{
		auto writer = findWriter( a, b, c );

		if ( a.isEnabled() && c.isEnabled() )
		{
			Value result{ writer };
			result.m_value << cuT( "((" ) << toString( a ) << cuT( ") * (" ) << toString( b ) << cuT( ") + (" ) << toString( c ) << cuT( "))" );
			return result;
		}
	}

	template< typename Value >
	inline Value fma( Value const & a, Optional< Value > const & b, Optional< Value > const & c )
	{
		auto writer = findWriter( a, b, c );

		if ( b.isEnabled() && c.isEnabled() )
		{
			Value result{ writer };
			result.m_value << cuT( "((" ) << toString( a ) << cuT( ") * (" ) << toString( b ) << cuT( ") + (" ) << toString( c ) << cuT( "))" );
			return result;
		}
	}

	template< typename Value >
	inline Value fma( Optional< Value > const & a, Optional< Value > const & b, Optional< Value > const & c )
	{
		auto writer = findWriter( a, b, c );

		if ( a.isEnabled() && b.isEnabled() && c.isEnabled() )
		{
			Value result{ writer };
			result.m_value << cuT( "((" ) << toString( a ) << cuT( ") * (" ) << toString( b ) << cuT( ") + (" ) << toString( c ) << cuT( "))" );
			return result;
		}
	}

	template< typename Value >
	inline Value fma( InParam< Value > const & a, Value const & b, Value const & c )
	{
		auto writer = findWriter( a, b, c );
		Value result{ writer };
		result.m_value << cuT( "((" ) << toString( a ) << cuT( ") * (" ) << toString( b ) << cuT( ") + (" ) << toString( c ) << cuT( "))" );
		return result;
	}

	template< typename Value >
	inline Value fma( Value const & a, InParam< Value > const & b, Value const & c )
	{
		auto writer = findWriter( a, b, c );
		Value result{ writer };
		result.m_value << cuT( "((" ) << toString( a ) << cuT( ") * (" ) << toString( b ) << cuT( ") + (" ) << toString( c ) << cuT( "))" );
		return result;
	}

	template< typename Value >
	inline Value fma( Value const & a, Value const & b, InParam< Value > const & c )
	{
		auto writer = findWriter( a, b, c );
		Value result{ writer };
		result.m_value << cuT( "((" ) << toString( a ) << cuT( ") * (" ) << toString( b ) << cuT( ") + (" ) << toString( c ) << cuT( "))" );
		return result;
	}

	template< typename Value >
	inline Value fma( InParam< Value > const & a, InParam< Value > const & b, Value const & c )
	{
		auto writer = findWriter( a, b, c );
		Value result{ writer };
		result.m_value << cuT( "((" ) << toString( a ) << cuT( ") * (" ) << toString( b ) << cuT( ") + (" ) << toString( c ) << cuT( "))" );
		return result;
	}

	template< typename Value >
	inline Value fma( InParam< Value > const & a, Value const & b, InParam< Value > const & c )
	{
		auto writer = findWriter( a, b, c );
		Value result{ writer };
		result.m_value << cuT( "((" ) << toString( a ) << cuT( ") * (" ) << toString( b ) << cuT( ") + (" ) << toString( c ) << cuT( "))" );
		return result;
	}

	template< typename Value >
	inline Value fma( Value const & a, InParam< Value > const & b, InParam< Value > const & c )
	{
		auto writer = findWriter( a, b, c );
		Value result{ writer };
		result.m_value << cuT( "((" ) << toString( a ) << cuT( ") * (" ) << toString( b ) << cuT( ") + (" ) << toString( c ) << cuT( "))" );
		return result;
	}

	template< typename Value >
	inline Value fma( InParam< Value > const & a, InParam< Value > const & b, InParam< Value > const & c )
	{
		auto writer = findWriter( a, b, c );
		Value result{ writer };
		result.m_value << cuT( "((" ) << toString( a ) << cuT( ") * (" ) << toString( b ) << cuT( ") + (" ) << toString( c ) << cuT( "))" );
		return result;
	}

	template< typename Value >
	inline Value fma( OutParam< Value > const & a, Value const & b, Value const & c )
	{
		auto writer = findWriter( a, b, c );
		Value result{ writer };
		result.m_value << cuT( "((" ) << toString( a ) << cuT( ") * (" ) << toString( b ) << cuT( ") + (" ) << toString( c ) << cuT( "))" );
		return result;
	}

	template< typename Value >
	inline Value fma( Value const & a, OutParam< Value > const & b, Value const & c )
	{
		auto writer = findWriter( a, b, c );
		Value result{ writer };
		result.m_value << cuT( "((" ) << toString( a ) << cuT( ") * (" ) << toString( b ) << cuT( ") + (" ) << toString( c ) << cuT( "))" );
		return result;
	}

	template< typename Value >
	inline Value fma( Value const & a, Value const & b, OutParam< Value > const & c )
	{
		auto writer = findWriter( a, b, c );
		Value result{ writer };
		result.m_value << cuT( "((" ) << toString( a ) << cuT( ") * (" ) << toString( b ) << cuT( ") + (" ) << toString( c ) << cuT( "))" );
		return result;
	}

	template< typename Value >
	inline Value fma( OutParam< Value > const & a, OutParam< Value > const & b, Value const & c )
	{
		auto writer = findWriter( a, b, c );
		Value result{ writer };
		result.m_value << cuT( "((" ) << toString( a ) << cuT( ") * (" ) << toString( b ) << cuT( ") + (" ) << toString( c ) << cuT( "))" );
		return result;
	}

	template< typename Value >
	inline Value fma( OutParam< Value > const & a, Value const & b, OutParam< Value > const & c )
	{
		auto writer = findWriter( a, b, c );
		Value result{ writer };
		result.m_value << cuT( "((" ) << toString( a ) << cuT( ") * (" ) << toString( b ) << cuT( ") + (" ) << toString( c ) << cuT( "))" );
		return result;
	}

	template< typename Value >
	inline Value fma( Value const & a, OutParam< Value > const & b, OutParam< Value > const & c )
	{
		auto writer = findWriter( a, b, c );
		Value result{ writer };
		result.m_value << cuT( "((" ) << toString( a ) << cuT( ") * (" ) << toString( b ) << cuT( ") + (" ) << toString( c ) << cuT( "))" );
		return result;
	}

	template< typename Value >
	inline Value fma( OutParam< Value > const & a, OutParam< Value > const & b, OutParam< Value > const & c )
	{
		auto writer = findWriter( a, b, c );
		Value result{ writer };
		result.m_value << cuT( "((" ) << toString( a ) << cuT( ") * (" ) << toString( b ) << cuT( ") + (" ) << toString( c ) << cuT( "))" );
		return result;
	}

	template< typename Value >
	inline Value fma( InOutParam< Value > const & a, Value const & b, Value const & c )
	{
		auto writer = findWriter( a, b, c );
		Value result{ writer };
		result.m_value << cuT( "((" ) << toString( a ) << cuT( ") * (" ) << toString( b ) << cuT( ") + (" ) << toString( c ) << cuT( "))" );
		return result;
	}

	template< typename Value >
	inline Value fma( Value const & a, InOutParam< Value > const & b, Value const & c )
	{
		auto writer = findWriter( a, b, c );
		Value result{ writer };
		result.m_value << cuT( "((" ) << toString( a ) << cuT( ") * (" ) << toString( b ) << cuT( ") + (" ) << toString( c ) << cuT( "))" );
		return result;
	}

	template< typename Value >
	inline Value fma( Value const & a, Value const & b, InOutParam< Value > const & c )
	{
		auto writer = findWriter( a, b, c );
		Value result{ writer };
		result.m_value << cuT( "((" ) << toString( a ) << cuT( ") * (" ) << toString( b ) << cuT( ") + (" ) << toString( c ) << cuT( "))" );
		return result;
	}

	template< typename Value >
	inline Value fma( InOutParam< Value > const & a, InOutParam< Value > const & b, Value const & c )
	{
		auto writer = findWriter( a, b, c );
		Value result{ writer };
		result.m_value << cuT( "((" ) << toString( a ) << cuT( ") * (" ) << toString( b ) << cuT( ") + (" ) << toString( c ) << cuT( "))" );
		return result;
	}

	template< typename Value >
	inline Value fma( InOutParam< Value > const & a, Value const & b, InOutParam< Value > const & c )
	{
		auto writer = findWriter( a, b, c );
		Value result{ writer };
		result.m_value << cuT( "((" ) << toString( a ) << cuT( ") * (" ) << toString( b ) << cuT( ") + (" ) << toString( c ) << cuT( "))" );
		return result;
	}

	template< typename Value >
	inline Value fma( Value const & a, InOutParam< Value > const & b, InOutParam< Value > const & c )
	{
		auto writer = findWriter( a, b, c );
		Value result{ writer };
		result.m_value << cuT( "((" ) << toString( a ) << cuT( ") * (" ) << toString( b ) << cuT( ") + (" ) << toString( c ) << cuT( "))" );
		return result;
	}

	template< typename Value >
	inline Value fma( InOutParam< Value > const & a, InOutParam< Value > const & b, InOutParam< Value > const & c )
	{
		auto writer = findWriter( a, b, c );
		Value result{ writer };
		result.m_value << cuT( "((" ) << toString( a ) << cuT( ") * (" ) << toString( b ) << cuT( ") + (" ) << toString( c ) << cuT( "))" );
		return result;
	}

	//***********************************************************************************************

	template< typename ... Values >
	inline Vec2 vec2( Type const & value, Values const & ... values )
	{
		return writeFunctionCall< Vec2, Type, Values... >( findWriter( value, values... ), cuT( "vec2" ), value, values... );
	}

	template< typename ... Values >
	inline Vec3 vec3( Type const & value, Values const & ... values )
	{
		return writeFunctionCall< Vec3, Type, Values... >( findWriter( value, values... ), cuT( "vec3" ), value, values... );
	}

	template< typename ... Values >
	inline Vec4 vec4( Type const & value, Values const & ... values )
	{
		return writeFunctionCall< Vec4, Type, Values... >( findWriter( value, values... ), cuT( "vec4" ), value, values... );
	}

	template< typename ... Values >
	inline IVec2 ivec2( Type const & value, Values const & ... values )
	{
		return writeFunctionCall< IVec2, Type, Values... >( findWriter( value, values... ), cuT( "ivec2" ), value, values... );
	}

	template< typename ... Values >
	inline IVec3 ivec3( Type const & value, Values const & ... values )
	{
		return writeFunctionCall< IVec3, Type, Values... >( findWriter( value, values... ), cuT( "ivec3" ), value, values... );
	}

	template< typename ... Values >
	inline IVec4 ivec4( Type const & value, Values const & ... values )
	{
		return writeFunctionCall< IVec4, Type, Values... >( findWriter( value, values... ), cuT( "ivec4" ), value, values... );
	}

	template< typename ... Values >
	inline BVec2 bvec2( Type const & value, Values const & ... values )
	{
		return writeFunctionCall< BVec2, Type, Values... >( findWriter( value, values... ), cuT( "bvec2" ), value, values... );
	}

	template< typename ... Values >
	inline BVec3 bvec3( Type const & value, Values const & ... values )
	{
		return writeFunctionCall< BVec3, Type, Values... >( findWriter( value, values... ), cuT( "bvec3" ), value, values... );
	}

	template< typename ... Values >
	inline BVec4 bvec4( Type const & value, Values const & ... values )
	{
		return writeFunctionCall< BVec4, Type, Values... >( findWriter( value, values... ), cuT( "bvec4" ), value, values... );
	}

	template< typename ... Values >
	inline Mat3 mat3( Type const & value, Values const & ... values )
	{
		return writeFunctionCall< Mat3, Type, Values... >( findWriter( value, values... ), cuT( "mat3" ), value, values... );
	}

	template< typename ... Values >
	inline Mat2 mat2( Type const & value, Values const & ... values )
	{
		return writeFunctionCall< Mat2, Type, Values... >( findWriter( value, values... ), cuT( "mat2" ), value, values... );
	}

	template< typename ... Values >
	inline Mat4 mat4( Type const & value, Values const & ... values )
	{
		return writeFunctionCall< Mat4, Type, Values... >( findWriter( value, values... ), cuT( "mat4" ), value, values... );
	}
}
