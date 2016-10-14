namespace GLSL
{
	//***********************************************************************************************

	namespace
	{
		template< typename TypeR, typename TypeA, typename TypeB >
		struct Operator
		{
			using Ret = TypeR;
			using T = TypeA;
			using U = TypeB;

			static Ret Write( char const * const p_operator, T const & p_a, U const & p_b )
			{
				Ret l_return( p_a.m_writer );
				l_return.m_value << Castor::String( p_a ) << ' ' << p_operator << ' ' << ToString( p_b );
				return l_return;
			}
		};

		template< typename TypeR, typename TypeA, typename TypeB >
		struct Operator< Optional< TypeR >, Optional< TypeA >, TypeB >
		{
			using Ret = Optional< TypeR >;
			using T = Optional< TypeA >;
			using U = TypeB;

			static Ret Write( char const * const p_operator, T const & p_a, U const & p_b )
			{
				Ret l_return( p_a.m_writer, Castor::String(), p_a.IsEnabled() );
				l_return.m_value << Castor::String( p_a ) << ' ' << p_operator << ' ' << ToString( p_b );
				return l_return;
			}
		};

		template< typename TypeR, typename TypeA, typename TypeB >
		struct Operator< Optional< TypeR >, TypeA, Optional< TypeB > >
		{
			using Ret = Optional< TypeR >;
			using T = TypeA;
			using U = Optional< TypeB >;

			static Ret Write( char const * const p_operator, T const & p_a, U const & p_b )
			{
				Ret l_return( p_a.m_writer, Castor::String(), p_b.IsEnabled() );
				l_return.m_value << Castor::String( p_a ) << ' ' << p_operator << ' ' << ToString( p_b );
				return l_return;
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
			: public Operator< GlslBool, TypeA, TypeB >
		{
			using OperatorType = Operator< GlslBool, TypeA, TypeB >;
			using Ret = typename OperatorType::Ret;
			using T = typename OperatorType::T;
			using U = typename  OperatorType::U;
		};
	}

#	define GLSL_IMPLEMENT_OPERATOR( RetType, LhsType, RhsType, Operator, OperatorType )\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( LhsType const & p_lhs, RhsType const & p_rhs )\
	{\
		return OperatorType< LhsType, RhsType >::Write( #Operator, p_lhs, p_rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( LhsType const & p_lhs, InParam< RhsType > const & p_rhs )\
	{\
		return OperatorType< LhsType, RhsType >::Write( #Operator, p_lhs, p_rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( LhsType const & p_lhs, InOutParam< RhsType > const & p_rhs )\
	{\
		return OperatorType< LhsType, RhsType >::Write( #Operator, p_lhs, p_rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( LhsType const & p_lhs, OutParam< RhsType > const & p_rhs )\
	{\
		return OperatorType< LhsType, RhsType >::Write( #Operator, p_lhs, p_rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( LhsType const & p_lhs, Optional< RhsType > const & p_rhs )\
	{\
		return OperatorType< LhsType, RhsType >::Write( #Operator, p_lhs, p_rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( InParam< LhsType > const & p_lhs, RhsType const & p_rhs )\
	{\
		return OperatorType< LhsType, RhsType >::Write( #Operator, p_lhs, p_rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( InParam< LhsType > const & p_lhs, InParam< RhsType > const & p_rhs )\
	{\
		return OperatorType< LhsType, RhsType >::Write( #Operator, p_lhs, p_rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( InParam< LhsType > const & p_lhs, InOutParam< RhsType > const & p_rhs )\
	{\
		return OperatorType< LhsType, RhsType >::Write( #Operator, p_lhs, p_rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( InParam< LhsType > const & p_lhs, OutParam< RhsType > const & p_rhs )\
	{\
		return OperatorType< LhsType, RhsType >::Write( #Operator, p_lhs, p_rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( InParam< LhsType > const & p_lhs, Optional< RhsType > const & p_rhs )\
	{\
		return OperatorType< LhsType, RhsType >::Write( #Operator, p_lhs, p_rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( InOutParam< LhsType > const & p_lhs, RhsType const & p_rhs )\
	{\
		return OperatorType< LhsType, RhsType >::Write( #Operator, p_lhs, p_rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( InOutParam< LhsType > const & p_lhs, InParam< RhsType > const & p_rhs )\
	{\
		return OperatorType< LhsType, RhsType >::Write( #Operator, p_lhs, p_rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( InOutParam< LhsType > const & p_lhs, InOutParam< RhsType > const & p_rhs )\
	{\
		return OperatorType< LhsType, RhsType >::Write( #Operator, p_lhs, p_rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( InOutParam< LhsType > const & p_lhs, OutParam< RhsType > const & p_rhs )\
	{\
		return OperatorType< LhsType, RhsType >::Write( #Operator, p_lhs, p_rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( InOutParam< LhsType > const & p_lhs, Optional< RhsType > const & p_rhs )\
	{\
		return OperatorType< LhsType, RhsType >::Write( #Operator, p_lhs, p_rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( OutParam< LhsType > const & p_lhs, RhsType const & p_rhs )\
	{\
		return OperatorType< LhsType, RhsType >::Write( #Operator, p_lhs, p_rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( OutParam< LhsType > const & p_lhs, InParam< RhsType > const & p_rhs )\
	{\
		return OperatorType< LhsType, RhsType >::Write( #Operator, p_lhs, p_rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( OutParam< LhsType > const & p_lhs, InOutParam< RhsType > const & p_rhs )\
	{\
		return OperatorType< LhsType, RhsType >::Write( #Operator, p_lhs, p_rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( OutParam< LhsType > const & p_lhs, OutParam< RhsType > const & p_rhs )\
	{\
		return OperatorType< LhsType, RhsType >::Write( #Operator, p_lhs, p_rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( OutParam< LhsType > const & p_lhs, Optional< RhsType > const & p_rhs )\
	{\
		return OperatorType< LhsType, RhsType >::Write( #Operator, p_lhs, p_rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( Optional< LhsType > const & p_lhs, RhsType const & p_rhs )\
	{\
		return OperatorType< LhsType, RhsType >::Write( #Operator, p_lhs, p_rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( Optional< LhsType > const & p_lhs, InParam< RhsType > const & p_rhs )\
	{\
		return OperatorType< LhsType, RhsType >::Write( #Operator, p_lhs, p_rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( Optional< LhsType > const & p_lhs, InOutParam< RhsType > const & p_rhs )\
	{\
		return OperatorType< LhsType, RhsType >::Write( #Operator, p_lhs, p_rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( Optional< LhsType > const & p_lhs, OutParam< RhsType > const & p_rhs )\
	{\
		return OperatorType< LhsType, RhsType >::Write( #Operator, p_lhs, p_rhs );\
	}\
	template< typename LhsType, typename RhsType, typename Enable >\
	RetType operator Operator( Optional< LhsType > const & p_lhs, Optional< RhsType > const & p_rhs )\
	{\
		return OperatorType< LhsType, RhsType >::Write( #Operator, p_lhs, p_rhs );\
	}

#	define GLSL_IMPLEMENT_ARITHMETIC_OPERATOR( LhsType, RhsType, Operator )\
	GLSL_IMPLEMENT_OPERATOR( LhsType, LhsType, RhsType, Operator, ArithmeticOperator )

#	define GLSL_IMPLEMENT_BOOLEAN_OPERATOR( LhsType, RhsType, Operator )\
	GLSL_IMPLEMENT_OPERATOR( GlslBool, LhsType, RhsType, Operator, BooleanOperator )

	GLSL_IMPLEMENT_ARITHMETIC_OPERATOR( TypeA, TypeB, + )
	GLSL_IMPLEMENT_ARITHMETIC_OPERATOR( TypeA, TypeB, - )
	GLSL_IMPLEMENT_ARITHMETIC_OPERATOR( TypeA, TypeB, * )
	GLSL_IMPLEMENT_ARITHMETIC_OPERATOR( TypeA, TypeB, / )
	GLSL_IMPLEMENT_BOOLEAN_OPERATOR( TypeA, TypeB, == )
	GLSL_IMPLEMENT_BOOLEAN_OPERATOR( TypeA, TypeB, != )
	GLSL_IMPLEMENT_BOOLEAN_OPERATOR( TypeA, TypeB, < )
	GLSL_IMPLEMENT_BOOLEAN_OPERATOR( TypeA, TypeB, <= )
	GLSL_IMPLEMENT_BOOLEAN_OPERATOR( TypeA, TypeB, > )
	GLSL_IMPLEMENT_BOOLEAN_OPERATOR( TypeA, TypeB, >= )

	//***********************************************************************************************

	template< typename ... Values >
	inline Float dot( Type const & p_value, Values const & ... p_values )
	{
		return WriteFunctionCall< Float, Type, Values... >( p_value.m_writer, cuT( "dot" ), p_value, p_values... );
	}

	template< typename ... Values >
	inline Float inversesqrt( Type const & p_value, Values const & ... p_values )
	{
		return WriteFunctionCall< Float, Type, Values... >( p_value.m_writer, cuT( "inversesqrt" ), p_value, p_values... );
	}

	template< typename ... Values >
	inline Float sqrt( Type const & p_value, Values const & ... p_values )
	{
		return WriteFunctionCall< Float, Type, Values... >( p_value.m_writer, cuT( "sqrt" ), p_value, p_values... );
	}

	template< typename ... Values >
	inline Float pow( Type const & p_value, Values const & ... p_values )
	{
		return WriteFunctionCall< Float, Type, Values... >( p_value.m_writer, cuT( "pow" ), p_value, p_values... );
	}

	template< typename Value, typename ... Values >
	inline Value cross( Value const & p_value, Values const & ... p_values )
	{
		return WriteFunctionCall< Value, Type, Values... >( p_value.m_writer, cuT( "cross" ), p_value, p_values... );
	}

	template< typename Value, typename ... Values >
	inline Value clamp( Value const & p_value, Values const & ... p_values )
	{
		return WriteFunctionCall< Value, Type, Values... >( p_value.m_writer, cuT( "clamp" ), p_value, p_values... );
	}

	template< typename Value, typename ... Values >
	inline Value min( Value const & p_value, Values const & ... p_values )
	{
		return WriteFunctionCall< Value, Type, Values... >( p_value.m_writer, cuT( "min" ), p_value, p_values... );
	}

	template< typename Value, typename ... Values >
	inline Value max( Value const & p_value, Values const & ... p_values )
	{
		return WriteFunctionCall< Value, Type, Values... >( p_value.m_writer, cuT( "max" ), p_value, p_values... );
	}

	template< typename Value, typename ... Values >
	inline Value mix( Value const & p_value, Values const & ... p_values )
	{
		return WriteFunctionCall< Value, Type, Values... >( p_value.m_writer, cuT( "mix" ), p_value, p_values... );
	}

	template< typename Value, typename ... Values >
	inline Value reflect( Type const & p_value, Values const & ... p_values )
	{
		return WriteFunctionCall< Value, Type, Values... >( p_value.m_writer, cuT( "reflect" ), p_value, p_values... );
	}

	template< typename Value >
	Value normalize( Value const & p_value )
	{
		return WriteFunctionCall< Value >( p_value.m_writer, cuT( "normalize" ), p_value );
	}

	template< typename Input, typename Output >
	inline Output normalize( Swizzle< Input, Output > const & p_value )
	{
		return WriteFunctionCall< Output >( p_value.m_writer, cuT( "normalize" ), static_cast< Output const >( p_value ) );
	}

	template< typename Value >
	inline Value transpose( Value const & p_value )
	{
		return WriteFunctionCall< Value >( p_value.m_writer, cuT( "transpose" ), p_value );
	}

	template< typename Value >
	inline Value inverse( Value const & p_value )
	{
		return WriteFunctionCall< Value >( p_value.m_writer, cuT( "inverse" ), p_value );
	}

	template< typename Value >
	inline Value abs( Value const & p_value )
	{
		return WriteFunctionCall< Value >( p_value.m_writer, cuT( "abs" ), p_value );
	}

	template< typename Value >
	inline Value neg( Value const & p_value )
	{
		return WriteFunctionCall< Value >( p_value.m_writer, cuT( "-" ), p_value );
	}

	template< typename Input, typename Output >
	inline Output neg( Swizzle< Input, Output > const & p_value )
	{
		return WriteFunctionCall< Output >( p_value.m_input->m_writer, cuT( "-" ), static_cast< Output >( p_value ) );
	}

	template< typename Value >
	inline Value log( Value const & p_value )
	{
		return WriteFunctionCall< Value >( p_value.m_input->m_writer, cuT( "log" ), p_value );
	}

	template< typename Value >
	inline Value exp( Value const & p_value )
	{
		return WriteFunctionCall< Value >( p_value.m_writer, cuT( "exp" ), p_value );
	}

	template< typename Value >
	inline Value log2( Value const & p_value )
	{
		return WriteFunctionCall< Value >( p_value.m_writer, cuT( "log2" ), p_value );
	}

	template< typename Value >
	inline Value exp2( Value const & p_value )
	{
		return WriteFunctionCall< Value >( p_value.m_writer, cuT( "exp2" ), p_value );
	}

	template< typename Value >
	inline Value tan( Value const & p_value )
	{
		return WriteFunctionCall< Value >( p_value.m_writer, cuT( "tan" ), p_value );
	}

	template< typename Value >
	inline Value sin( Value const & p_value )
	{
		return WriteFunctionCall< Value >( p_value.m_writer, cuT( "sin" ), p_value );
	}

	template< typename Value >
	inline Value cos( Value const & p_value )
	{
		return WriteFunctionCall< Value >( p_value.m_writer, cuT( "cos" ), p_value );
	}

	template< typename Value >
	inline Value atan( Value const & p_value )
	{
		return WriteFunctionCall< Value >( p_value.m_writer, cuT( "atan" ), p_value );
	}

	template< typename Value >
	inline Value asin( Value const & p_value )
	{
		return WriteFunctionCall< Value >( p_value.m_writer, cuT( "asin" ), p_value );
	}

	template< typename Value >
	inline Value acos( Value const & p_value )
	{
		return WriteFunctionCall< Value >( p_value.m_writer, cuT( "acos" ), p_value );
	}

	//***********************************************************************************************

	template< typename ... Values >
	inline Vec2 vec2( Type const & p_value, Values const & ... p_values )
	{
		return WriteFunctionCall< Vec2, Type, Values... >( p_value.m_writer, cuT( "vec2" ), p_value, p_values... );
	}

	template< typename ... Values >
	inline Vec3 vec3( Type const & p_value, Values const & ... p_values )
	{
		return WriteFunctionCall< Vec3, Type, Values... >( p_value.m_writer, cuT( "vec3" ), p_value, p_values... );
	}

	template< typename ... Values >
	inline Vec4 vec4( Type const & p_value, Values const & ... p_values )
	{
		return WriteFunctionCall< Vec4, Type, Values... >( p_value.m_writer, cuT( "vec4" ), p_value, p_values... );
	}

	template< typename ... Values >
	inline IVec2 ivec2( Type const & p_value, Values const & ... p_values )
	{
		return WriteFunctionCall< IVec2, Type, Values... >( p_value.m_writer, cuT( "ivec2" ), p_value, p_values... );
	}

	template< typename ... Values >
	inline IVec3 ivec3( Type const & p_value, Values const & ... p_values )
	{
		return WriteFunctionCall< IVec3, Type, Values... >( p_value.m_writer, cuT( "ivec3" ), p_value, p_values... );
	}

	template< typename ... Values >
	inline IVec4 ivec4( Type const & p_value, Values const & ... p_values )
	{
		return WriteFunctionCall< IVec4, Type, Values... >( p_value.m_writer, cuT( "ivec4" ), p_value, p_values... );
	}

	template< typename ... Values >
	inline Mat3 mat3( Type const & p_value, Values const & ... p_values )
	{
		return WriteFunctionCall< Mat3, Type, Values... >( p_value.m_writer, cuT( "mat3" ), p_value, p_values... );
	}

	template< typename ... Values >
	inline Mat4 mat4( Type const & p_value, Values const & ... p_values )
	{
		return WriteFunctionCall< Mat4, Type, Values... >( p_value.m_writer, cuT( "mat4" ), p_value, p_values... );
	}
}
