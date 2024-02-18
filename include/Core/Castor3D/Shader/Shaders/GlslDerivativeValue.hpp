/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslDerivativeValue_H___
#define ___C3D_GlslDerivativeValue_H___

#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include <ShaderWriter/CompositeTypes/StructHelper.hpp>
#include <ShaderWriter/Intrinsics/IntrinsicFunctions.hpp>

namespace castor3d::shader
{
	template< typename ValueT
		, sdw::StringLiteralT StructNameT >
	using DerivativeValueHelperT = sdw::StructInstanceHelperT < StructNameT
		, sdw::type::MemoryLayout::eC
		, sdw::StructFieldT< ValueT, "value" >
		, sdw::StructFieldT< ValueT, "dPdx" >
		, sdw::StructFieldT< ValueT, "dPdy" > >;

	template< typename ValueT
		, sdw::StringLiteralT StructNameT >
	struct DerivativeValueT
		: public DerivativeValueHelperT< ValueT, StructNameT >
	{
		DerivativeValueT( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: DerivativeValueHelperT< ValueT, StructNameT >{ writer, castor::move( expr ), enabled }
		{
		}

		DerivativeValueT( ValueT const & pvalue
			, ValueT const & pdPdx
			, ValueT const & pdPdy )
			: DerivativeValueT{ sdw::findWriterMandat( pvalue, pdPdx, pdPdy )
				, sdw::makeAggrInit( DerivativeValueHelperT< ValueT, StructNameT >::makeType( pvalue.getType()->getTypesCache() )
					, [&pvalue, &pdPdx, &pdPdy]()
					{
						sdw::expr::ExprList result;
						result.emplace_back( makeExpr( pvalue ) );
						result.emplace_back( makeExpr( pdPdx ) );
						result.emplace_back( makeExpr( pdPdy ) );
						return result;
					}() )
				, true }
		{
		}

		auto value()const { return this->getMember< "value" >(); }
		auto dPdx()const { return this->getMember< "dPdx" >(); }
		auto dPdy()const { return this->getMember< "dPdy" >(); }

		DerivativeValueT operator-()const;
		DerivativeValueT & operator+=( DerivativeValueT const & rhs );
		DerivativeValueT & operator-=( DerivativeValueT const & rhs );
		DerivativeValueT & operator*=( DerivativeValueT const & rhs );
		DerivativeValueT & operator+=( ValueT const & rhs );
		DerivativeValueT & operator-=( ValueT const & rhs );
		DerivativeValueT & operator*=( ValueT const & rhs );
	};
}

namespace sdw
{
	template< typename ValueT, StringLiteralT StructNameT >
	struct TypeTraits< castor3d::shader::DerivativeValueT< ValueT, StructNameT > >
	{
		static ast::type::Kind constexpr TypeEnum = TypeTraits< StructInstance >::TypeEnum;
		static bool constexpr HasArithmeticOperators = TypeTraits< StructInstance >::HasArithmeticOperators;
		using CppType = typename TypeTraits< StructInstance >::CppType;
		using OperandType = typename TypeTraits< StructInstance >::OperandType;
		using LargestType = typename TypeTraits< StructInstance >::LargestType;
		using ComponentType = typename TypeTraits< StructInstance >::ComponentType;
		static size_t constexpr Size = TypeTraits< StructInstance >::Size;
		static size_t constexpr ComponentCount = TypeTraits< StructInstance >::ComponentCount;
	};
}

namespace castor3d::shader
{
	C3D_API sdw::expr::ExprPtr makeRawExpr( DerivFloat const & value );
	C3D_API sdw::expr::ExprPtr makeRawExpr( DerivVec2 const & value );
	C3D_API sdw::expr::ExprPtr makeRawExpr( DerivVec3 const & value );
	C3D_API sdw::expr::ExprPtr makeRawExpr( DerivVec4 const & value );
	C3D_API RetDerivFloat dot( DerivVec3 const lhs, DerivVec3 const rhs );
	C3D_API RetDerivVec3 cross( DerivVec3 const lhs, DerivVec3 const rhs );
	C3D_API RetDerivVec3 normalize( DerivVec3 const v );
	C3D_API RetDerivVec4 normalize( DerivVec4 const v );
	C3D_API RetDerivVec3 refract( DerivVec3 const i, DerivVec3 const n, sdw::Float const ior );
	C3D_API RetDerivFloat clamp( DerivFloat const v, sdw::Float const min, sdw::Float const max );
	C3D_API RetDerivFloat length( DerivVec3 const v );
	C3D_API RetDerivFloat max( DerivFloat const lhs, DerivFloat const rhs );
	C3D_API RetDerivVec3 mix( DerivVec3 const a, DerivVec3 const b, DerivVec3 const c );
	C3D_API RetDerivFloat fma( DerivFloat const a, DerivFloat const b, DerivFloat const c );
	C3D_API RetDerivVec2 fma( DerivVec2 const a, DerivVec2 const b, DerivVec2 const c );
	C3D_API RetDerivVec3 fma( DerivVec3 const a, DerivVec3 const b, DerivVec3 const c );
	C3D_API RetDerivVec4 fma( DerivVec4 const a, DerivVec4 const b, DerivVec4 const c );
	C3D_API sdw::Float fwidth( DerivFloat const a );
	C3D_API sdw::Vec2 fwidth( DerivVec2 const a );
	C3D_API sdw::Vec3 fwidth( DerivVec3 const a );
	C3D_API sdw::Vec4 fwidth( DerivVec4 const a );
	C3D_API DerivFloat negate( DerivFloat const a );
	C3D_API DerivVec2 negate( DerivVec2 const a );
	C3D_API DerivVec3 negate( DerivVec3 const a );
	C3D_API DerivVec4 negate( DerivVec4 const a );
	C3D_API sdw::Float computeMip( DerivVec2 const & uv
		, sdw::Vec2 const & texSize );

	C3D_API DerivFloat derivFloat( sdw::Float const v );
	C3D_API DerivFloat derivX( DerivVec2 const v );
	C3D_API DerivFloat derivX( DerivVec3 const v );
	C3D_API DerivFloat derivX( DerivVec4 const v );
	C3D_API DerivFloat derivY( DerivVec2 const v );
	C3D_API DerivFloat derivY( DerivVec3 const v );
	C3D_API DerivFloat derivY( DerivVec4 const v );
	C3D_API DerivFloat derivZ( DerivVec3 const v );
	C3D_API DerivFloat derivZ( DerivVec4 const v );
	C3D_API DerivFloat derivW( DerivVec4 const v );
	C3D_API DerivVec2 derivVec2( sdw::Float const v );
	C3D_API DerivVec2 derivVec2( sdw::Vec2 const v );
	C3D_API DerivVec2 derivVec2( DerivFloat const v );
	C3D_API DerivVec2 derivVec2( DerivVec3 const v );
	C3D_API DerivVec2 derivVec2( DerivVec4 const v );
	C3D_API DerivVec2 derivVec2( DerivFloat const v, sdw::Float const a );
	C3D_API DerivVec3 derivVec3( sdw::Float const v );
	C3D_API DerivVec3 derivVec3( sdw::Vec3 const v );
	C3D_API DerivVec3 derivVec3( DerivFloat const v );
	C3D_API DerivVec3 derivVec3( DerivVec4 const v );
	C3D_API DerivVec4 derivVec4( sdw::Float const v );
	C3D_API DerivVec4 derivVec4( sdw::Vec4 const v );
	C3D_API DerivVec4 derivVec4( DerivFloat const v );
	C3D_API DerivVec4 derivVec4( DerivVec3 const v, sdw::Float const a );
	C3D_API DerivVec4 derivVec4( DerivVec3 const v, DerivFloat const a );
	C3D_API void negateXYZ( sdw::Vec4 in );
	C3D_API void negateXYZ( DerivVec4 in );
	C3D_API void mulXY( sdw::Vec4 in, sdw::Vec2 const mul );
	C3D_API void mulXY( DerivVec4 in, sdw::Vec2 const mul );
	C3D_API void addXYZ( sdw::Vec4 in, sdw::Vec3 const add );
	C3D_API void addXYZ( DerivVec4 in, sdw::Vec3 const add );
	C3D_API void addXYZ( DerivVec4 in, DerivVec3 const add );
	C3D_API sdw::Vec3 getRaw( sdw::Vec3 const in );
	C3D_API sdw::Vec3 getRaw( DerivVec3 const in );
	C3D_API sdw::Vec4 getRaw( sdw::Vec4 const in );
	C3D_API sdw::Vec4 getRaw( DerivVec4 const in );
	C3D_API sdw::Vec3 getRawXYZ( sdw::Vec3 const in );
	C3D_API sdw::Vec3 getRawXYZ( DerivVec3 const in );
	C3D_API sdw::Vec3 getRawXYZ( sdw::Vec4 const in );
	C3D_API sdw::Vec3 getRawXYZ( DerivVec4 const in );
	C3D_API sdw::Vec3 getXYZ( sdw::Vec4 const in );
	C3D_API DerivVec3 getXYZ( DerivVec4 const in );
	C3D_API sdw::Vec3 getXYW( sdw::Vec4 const in );
	C3D_API DerivVec3 getXYW( DerivVec4 const in );
	C3D_API sdw::Float getW( sdw::Vec4 const in );
	C3D_API DerivFloat getW( DerivVec4 const in );

	C3D_API DerivVec4 operator*( sdw::Mat4 const lhs, DerivVec4 const rhs );
	C3D_API DerivVec3 operator*( sdw::Mat3 const lhs, DerivVec3 const rhs );

	C3D_API DerivFloat operator+( DerivFloat const lhs, DerivFloat const rhs );
	C3D_API DerivFloat operator+( DerivFloat const lhs, sdw::Float const rhs );
	C3D_API DerivFloat operator*( DerivFloat const lhs, DerivFloat const rhs );
	C3D_API DerivFloat operator*( DerivFloat const lhs, sdw::Float const rhs );
	C3D_API DerivFloat operator-( DerivFloat const lhs, DerivFloat const rhs );
	C3D_API DerivFloat operator-( DerivFloat const lhs, sdw::Float const rhs );

	C3D_API DerivVec2 operator+( DerivVec2 const lhs, DerivVec2 const rhs );
	C3D_API DerivVec2 operator+( DerivVec2 const lhs, sdw::Vec2 const rhs );
	C3D_API DerivVec2 operator+( DerivVec2 const lhs, DerivFloat const rhs );
	C3D_API DerivVec2 operator+( DerivVec2 const lhs, sdw::Float const rhs );
	C3D_API DerivVec2 operator-( DerivVec2 const lhs, DerivVec2 const rhs );
	C3D_API DerivVec2 operator-( DerivVec2 const lhs, sdw::Vec2 const rhs );
	C3D_API DerivVec2 operator-( DerivVec2 const lhs, DerivFloat const rhs );
	C3D_API DerivVec2 operator-( DerivVec2 const lhs, sdw::Float const rhs );
	C3D_API DerivVec2 operator*( DerivVec2 const lhs, DerivVec2 const rhs );
	C3D_API DerivVec2 operator*( DerivVec2 const lhs, sdw::Vec2 const rhs );
	C3D_API DerivVec2 operator*( DerivVec2 const lhs, DerivFloat const rhs );
	C3D_API DerivVec2 operator*( DerivVec2 const lhs, sdw::Float const rhs );

	C3D_API DerivVec3 operator+( DerivVec3 const lhs, DerivVec3 const rhs );
	C3D_API DerivVec3 operator+( DerivVec3 const lhs, sdw::Vec3 const rhs );
	C3D_API DerivVec3 operator+( DerivVec3 const lhs, DerivFloat const rhs );
	C3D_API DerivVec3 operator+( DerivVec3 const lhs, sdw::Float const rhs );
	C3D_API DerivVec3 operator-( DerivVec3 const lhs, DerivVec3 const rhs );
	C3D_API DerivVec3 operator-( DerivVec3 const lhs, sdw::Vec3 const rhs );
	C3D_API DerivVec3 operator-( DerivVec3 const lhs, DerivFloat const rhs );
	C3D_API DerivVec3 operator-( DerivVec3 const lhs, sdw::Float const rhs );
	C3D_API DerivVec3 operator*( DerivVec3 const lhs, DerivVec3 const rhs );
	C3D_API DerivVec3 operator*( DerivVec3 const lhs, sdw::Vec3 const rhs );
	C3D_API DerivVec3 operator*( DerivVec3 const lhs, DerivFloat const rhs );
	C3D_API DerivVec3 operator*( DerivVec3 const lhs, sdw::Float const rhs );

	C3D_API DerivVec4 operator+( DerivVec4 const lhs, DerivVec4 const rhs );
	C3D_API DerivVec4 operator+( DerivVec4 const lhs, sdw::Vec4 const rhs );
	C3D_API DerivVec4 operator+( DerivVec4 const lhs, DerivFloat const rhs );
	C3D_API DerivVec4 operator+( DerivVec4 const lhs, sdw::Float const rhs );
	C3D_API DerivVec4 operator-( DerivVec4 const lhs, DerivVec4 const rhs );
	C3D_API DerivVec4 operator-( DerivVec4 const lhs, sdw::Vec4 const rhs );
	C3D_API DerivVec4 operator-( DerivVec4 const lhs, DerivFloat const rhs );
	C3D_API DerivVec4 operator-( DerivVec4 const lhs, sdw::Float const rhs );
	C3D_API DerivVec4 operator*( DerivVec4 const lhs, DerivVec4 const rhs );
	C3D_API DerivVec4 operator*( DerivVec4 const lhs, sdw::Vec4 const rhs );
	C3D_API DerivVec4 operator*( DerivVec4 const lhs, DerivFloat const rhs );
	C3D_API DerivVec4 operator*( DerivVec4 const lhs, sdw::Float const rhs );

	//@}
	//@}
}

#include "GlslDerivativeValue.inl"

#endif
