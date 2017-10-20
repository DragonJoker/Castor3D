/*
See LICENSE file in root folder
*/
#ifndef ___GLSL_Vec_H___
#define ___GLSL_Vec_H___

#include "GlslSwizzle.hpp"

namespace glsl
{
	template< typename ValueT >
	struct Vec2T
		: public Type
	{
		using my_type = ValueT;
		using my_vec2 = Vec2T< ValueT >;

		inline Vec2T();
		inline Vec2T( GlslWriter * p_writer, castor::String const & p_name = castor::String() );
		inline my_vec2 & operator=( my_vec2 const & p_rhs );
		template< typename IndexT > inline my_type operator[]( IndexT const & p_rhs )const;
		inline my_type operator[]( int const & p_rhs )const;

		GLSL_FIRST_SWIZZLE( my_vec2, my_type, x );
		GLSL_SWIZZLE( my_vec2, my_type, y );
		GLSL_SWIZZLE( my_vec2, my_type, s );
		GLSL_SWIZZLE( my_vec2, my_type, t );
		GLSL_SWIZZLE( my_vec2, my_type, r );
		GLSL_LAST_SWIZZLE( my_vec2, my_type, g );
		GLSL_FIRST_SWIZZLE( my_vec2, my_vec2, xy );
		GLSL_SWIZZLE( my_vec2, my_vec2, yx );
		GLSL_SWIZZLE( my_vec2, my_vec2, xx );
		GLSL_SWIZZLE( my_vec2, my_vec2, yy );
		GLSL_SWIZZLE( my_vec2, my_type, st );
		GLSL_SWIZZLE( my_vec2, my_type, ts );
		GLSL_SWIZZLE( my_vec2, my_type, ss );
		GLSL_SWIZZLE( my_vec2, my_type, tt );
		GLSL_SWIZZLE( my_vec2, my_vec2, rg );
		GLSL_SWIZZLE( my_vec2, my_vec2, gr );
		GLSL_SWIZZLE( my_vec2, my_vec2, rr );
		GLSL_LAST_SWIZZLE( my_vec2, my_vec2, gg );
	};

	template< typename ValueT >
	inline Vec2T< ValueT > operator*( ValueT const & p_lhs, Vec2T< ValueT > const & p_rhs );

	template< typename ValueT >
	struct Vec3T
		: public Type
	{
		using my_type = ValueT;
		using my_vec2 = Vec2T< ValueT >;
		using my_vec3 = Vec3T< ValueT >;

		inline Vec3T();
		inline Vec3T( GlslWriter * p_writer, castor::String const & p_name = castor::String() );
		inline my_vec3 & operator=( my_vec3 const & p_rhs );
		template< typename IndexT > inline my_type operator[]( IndexT const & p_rhs )const;
		inline my_type operator[]( int const & p_rhs )const;

		GLSL_FIRST_SWIZZLE( my_vec3, my_type, x );
		GLSL_SWIZZLE( my_vec3, my_type, y );
		GLSL_SWIZZLE( my_vec3, my_type, z );
		GLSL_SWIZZLE( my_vec3, my_type, s );
		GLSL_SWIZZLE( my_vec3, my_type, t );
		GLSL_SWIZZLE( my_vec3, my_type, p );
		GLSL_SWIZZLE( my_vec3, my_type, r );
		GLSL_SWIZZLE( my_vec3, my_type, g );
		GLSL_LAST_SWIZZLE( my_vec3, my_type, b );
		GLSL_FIRST_SWIZZLE( my_vec3, my_vec2, xy );
		GLSL_SWIZZLE( my_vec3, my_vec2, xz );
		GLSL_SWIZZLE( my_vec3, my_vec2, yx );
		GLSL_SWIZZLE( my_vec3, my_vec2, yz );
		GLSL_SWIZZLE( my_vec3, my_vec2, zx );
		GLSL_SWIZZLE( my_vec3, my_vec2, zy );
		GLSL_SWIZZLE( my_vec2, my_vec2, xx );
		GLSL_SWIZZLE( my_vec2, my_vec2, yy );
		GLSL_SWIZZLE( my_vec2, my_vec2, zz );
		GLSL_SWIZZLE( my_vec3, my_vec2, st );
		GLSL_SWIZZLE( my_vec3, my_vec2, sp );
		GLSL_SWIZZLE( my_vec3, my_vec2, ts );
		GLSL_SWIZZLE( my_vec3, my_vec2, tp );
		GLSL_SWIZZLE( my_vec3, my_vec2, ps );
		GLSL_SWIZZLE( my_vec3, my_vec2, pt );
		GLSL_SWIZZLE( my_vec2, my_type, ss );
		GLSL_SWIZZLE( my_vec2, my_type, tt );
		GLSL_SWIZZLE( my_vec2, my_type, pp );
		GLSL_SWIZZLE( my_vec3, my_vec2, rg );
		GLSL_SWIZZLE( my_vec3, my_vec2, rb );
		GLSL_SWIZZLE( my_vec3, my_vec2, gr );
		GLSL_SWIZZLE( my_vec3, my_vec2, gb );
		GLSL_SWIZZLE( my_vec3, my_vec2, br );
		GLSL_SWIZZLE( my_vec2, my_vec2, bg );
		GLSL_SWIZZLE( my_vec2, my_vec2, rr );
		GLSL_SWIZZLE( my_vec2, my_vec2, gg );
		GLSL_LAST_SWIZZLE( my_vec3, my_vec2, bb );
		GLSL_FIRST_SWIZZLE( my_vec3, my_vec3, xyz );
		GLSL_SWIZZLE( my_vec3, my_vec3, xzy );
		GLSL_SWIZZLE( my_vec3, my_vec3, yxz );
		GLSL_SWIZZLE( my_vec3, my_vec3, yzx );
		GLSL_SWIZZLE( my_vec3, my_vec3, zxy );
		GLSL_SWIZZLE( my_vec3, my_vec3, zyx );
		GLSL_SWIZZLE( my_vec3, my_vec3, stp );
		GLSL_SWIZZLE( my_vec3, my_vec3, spt );
		GLSL_SWIZZLE( my_vec3, my_vec3, tsp );
		GLSL_SWIZZLE( my_vec3, my_vec3, tps );
		GLSL_SWIZZLE( my_vec3, my_vec3, pst );
		GLSL_SWIZZLE( my_vec3, my_vec3, pts );
		GLSL_SWIZZLE( my_vec3, my_vec3, rgb );
		GLSL_SWIZZLE( my_vec3, my_vec3, rbg );
		GLSL_SWIZZLE( my_vec3, my_vec3, grb );
		GLSL_SWIZZLE( my_vec3, my_vec3, gbr );
		GLSL_SWIZZLE( my_vec3, my_vec3, brg );
		GLSL_LAST_SWIZZLE( my_vec3, my_vec3, bgr );
	};

	template< typename ValueT >
	inline Vec3T< ValueT > operator*( ValueT const & p_lhs, Vec3T< ValueT > const & p_rhs );

	template< typename ValueT >
	struct Vec4T
		: public Type
	{
		using my_type = ValueT;
		using my_vec2 = Vec2T< ValueT >;
		using my_vec3 = Vec3T< ValueT >;
		using my_vec4 = Vec4T< ValueT >;

		inline Vec4T();
		inline Vec4T( GlslWriter * p_writer, castor::String const & p_name = castor::String() );
		inline my_vec4 & operator=( my_vec4 const & p_rhs );
		template< typename IndexT > inline my_type operator[]( IndexT const & p_rhs )const;
		inline my_type operator[]( int const & p_rhs )const;

		GLSL_FIRST_SWIZZLE( my_vec4, my_type, x );
		GLSL_SWIZZLE( my_vec4, my_type, y );
		GLSL_SWIZZLE( my_vec4, my_type, z );
		GLSL_SWIZZLE( my_vec4, my_type, w );
		GLSL_SWIZZLE( my_vec4, my_type, s );
		GLSL_SWIZZLE( my_vec4, my_type, t );
		GLSL_SWIZZLE( my_vec4, my_type, p );
		GLSL_SWIZZLE( my_vec4, my_type, q );
		GLSL_SWIZZLE( my_vec4, my_type, r );
		GLSL_SWIZZLE( my_vec4, my_type, g );
		GLSL_SWIZZLE( my_vec4, my_type, b );
		GLSL_LAST_SWIZZLE( my_vec4, my_type, a );
		GLSL_FIRST_SWIZZLE( my_vec4, my_vec2, xy );
		GLSL_SWIZZLE( my_vec4, my_vec2, xz );
		GLSL_SWIZZLE( my_vec4, my_vec2, xw );
		GLSL_SWIZZLE( my_vec4, my_vec2, yx );
		GLSL_SWIZZLE( my_vec4, my_vec2, yz );
		GLSL_SWIZZLE( my_vec4, my_vec2, yw );
		GLSL_SWIZZLE( my_vec4, my_vec2, zx );
		GLSL_SWIZZLE( my_vec4, my_vec2, zy );
		GLSL_SWIZZLE( my_vec4, my_vec2, zw );
		GLSL_SWIZZLE( my_vec4, my_vec2, wx );
		GLSL_SWIZZLE( my_vec4, my_vec2, wy );
		GLSL_SWIZZLE( my_vec4, my_vec2, wz );
		GLSL_SWIZZLE( my_vec2, my_vec2, xx );
		GLSL_SWIZZLE( my_vec2, my_vec2, yy );
		GLSL_SWIZZLE( my_vec2, my_vec2, zz );
		GLSL_SWIZZLE( my_vec2, my_vec2, ww );
		GLSL_SWIZZLE( my_vec4, my_vec2, st );
		GLSL_SWIZZLE( my_vec4, my_vec2, sp );
		GLSL_SWIZZLE( my_vec4, my_vec2, sq );
		GLSL_SWIZZLE( my_vec4, my_vec2, ts );
		GLSL_SWIZZLE( my_vec4, my_vec2, tp );
		GLSL_SWIZZLE( my_vec4, my_vec2, tq );
		GLSL_SWIZZLE( my_vec4, my_vec2, ps );
		GLSL_SWIZZLE( my_vec4, my_vec2, pt );
		GLSL_SWIZZLE( my_vec4, my_vec2, pq );
		GLSL_SWIZZLE( my_vec4, my_vec2, qs );
		GLSL_SWIZZLE( my_vec4, my_vec2, qt );
		GLSL_SWIZZLE( my_vec4, my_vec2, qp );
		GLSL_SWIZZLE( my_vec2, my_type, ss );
		GLSL_SWIZZLE( my_vec2, my_type, tt );
		GLSL_SWIZZLE( my_vec2, my_type, pp );
		GLSL_SWIZZLE( my_vec2, my_type, qq );
		GLSL_SWIZZLE( my_vec4, my_vec2, rg );
		GLSL_SWIZZLE( my_vec4, my_vec2, rb );
		GLSL_SWIZZLE( my_vec4, my_vec2, ra );
		GLSL_SWIZZLE( my_vec4, my_vec2, gr );
		GLSL_SWIZZLE( my_vec4, my_vec2, gb );
		GLSL_SWIZZLE( my_vec4, my_vec2, ga );
		GLSL_SWIZZLE( my_vec4, my_vec2, br );
		GLSL_SWIZZLE( my_vec4, my_vec2, bg );
		GLSL_SWIZZLE( my_vec4, my_vec2, ba );
		GLSL_SWIZZLE( my_vec4, my_vec2, ar );
		GLSL_SWIZZLE( my_vec4, my_vec2, ag );
		GLSL_SWIZZLE( my_vec4, my_vec2, ab );
		GLSL_SWIZZLE( my_vec2, my_vec2, rr );
		GLSL_SWIZZLE( my_vec2, my_vec2, gg );
		GLSL_SWIZZLE( my_vec2, my_vec2, bb );
		GLSL_LAST_SWIZZLE( my_vec3, my_vec2, aa );
		GLSL_FIRST_SWIZZLE( my_vec4, my_vec3, xyz );
		GLSL_SWIZZLE( my_vec4, my_vec3, xyw );
		GLSL_SWIZZLE( my_vec4, my_vec3, xzy );
		GLSL_SWIZZLE( my_vec4, my_vec3, xzw );
		GLSL_SWIZZLE( my_vec4, my_vec3, xwy );
		GLSL_SWIZZLE( my_vec4, my_vec3, xwz );
		GLSL_SWIZZLE( my_vec4, my_vec3, yxz );
		GLSL_SWIZZLE( my_vec4, my_vec3, yxw );
		GLSL_SWIZZLE( my_vec4, my_vec3, yzx );
		GLSL_SWIZZLE( my_vec4, my_vec3, yzw );
		GLSL_SWIZZLE( my_vec4, my_vec3, ywx );
		GLSL_SWIZZLE( my_vec4, my_vec3, ywz );
		GLSL_SWIZZLE( my_vec4, my_vec3, zxy );
		GLSL_SWIZZLE( my_vec4, my_vec3, zxw );
		GLSL_SWIZZLE( my_vec4, my_vec3, zyx );
		GLSL_SWIZZLE( my_vec4, my_vec3, zyw );
		GLSL_SWIZZLE( my_vec4, my_vec3, zwx );
		GLSL_SWIZZLE( my_vec4, my_vec3, zwy );
		GLSL_SWIZZLE( my_vec4, my_vec3, wxy );
		GLSL_SWIZZLE( my_vec4, my_vec3, wxz );
		GLSL_SWIZZLE( my_vec4, my_vec3, wyx );
		GLSL_SWIZZLE( my_vec4, my_vec3, wyz );
		GLSL_SWIZZLE( my_vec4, my_vec3, wzx );
		GLSL_SWIZZLE( my_vec4, my_vec3, wzy );
		GLSL_SWIZZLE( my_vec4, my_vec3, xxx );
		GLSL_SWIZZLE( my_vec4, my_vec3, yyy );
		GLSL_SWIZZLE( my_vec4, my_vec3, zzz );
		GLSL_SWIZZLE( my_vec4, my_vec3, www );
		GLSL_SWIZZLE( my_vec4, my_vec3, stp );
		GLSL_SWIZZLE( my_vec4, my_vec3, stq );
		GLSL_SWIZZLE( my_vec4, my_vec3, spt );
		GLSL_SWIZZLE( my_vec4, my_vec3, spq );
		GLSL_SWIZZLE( my_vec4, my_vec3, sqt );
		GLSL_SWIZZLE( my_vec4, my_vec3, sqp );
		GLSL_SWIZZLE( my_vec4, my_vec3, tsp );
		GLSL_SWIZZLE( my_vec4, my_vec3, tsq );
		GLSL_SWIZZLE( my_vec4, my_vec3, tps );
		GLSL_SWIZZLE( my_vec4, my_vec3, tpq );
		GLSL_SWIZZLE( my_vec4, my_vec3, tqs );
		GLSL_SWIZZLE( my_vec4, my_vec3, tqp );
		GLSL_SWIZZLE( my_vec4, my_vec3, pst );
		GLSL_SWIZZLE( my_vec4, my_vec3, psq );
		GLSL_SWIZZLE( my_vec4, my_vec3, pts );
		GLSL_SWIZZLE( my_vec4, my_vec3, ptq );
		GLSL_SWIZZLE( my_vec4, my_vec3, pqs );
		GLSL_SWIZZLE( my_vec4, my_vec3, pqt );
		GLSL_SWIZZLE( my_vec4, my_vec3, qst );
		GLSL_SWIZZLE( my_vec4, my_vec3, qsp );
		GLSL_SWIZZLE( my_vec4, my_vec3, qts );
		GLSL_SWIZZLE( my_vec4, my_vec3, qtp );
		GLSL_SWIZZLE( my_vec4, my_vec3, qps );
		GLSL_SWIZZLE( my_vec4, my_vec3, qpt );
		GLSL_SWIZZLE( my_vec4, my_vec3, rgb );
		GLSL_SWIZZLE( my_vec4, my_vec3, rga );
		GLSL_SWIZZLE( my_vec4, my_vec3, rbg );
		GLSL_SWIZZLE( my_vec4, my_vec3, rba );
		GLSL_SWIZZLE( my_vec4, my_vec3, rag );
		GLSL_SWIZZLE( my_vec4, my_vec3, rab );
		GLSL_SWIZZLE( my_vec4, my_vec3, grb );
		GLSL_SWIZZLE( my_vec4, my_vec3, gra );
		GLSL_SWIZZLE( my_vec4, my_vec3, gbr );
		GLSL_SWIZZLE( my_vec4, my_vec3, gba );
		GLSL_SWIZZLE( my_vec4, my_vec3, gar );
		GLSL_SWIZZLE( my_vec4, my_vec3, gab );
		GLSL_SWIZZLE( my_vec4, my_vec3, brg );
		GLSL_SWIZZLE( my_vec4, my_vec3, bra );
		GLSL_SWIZZLE( my_vec4, my_vec3, bgr );
		GLSL_SWIZZLE( my_vec4, my_vec3, bga );
		GLSL_SWIZZLE( my_vec4, my_vec3, bar );
		GLSL_SWIZZLE( my_vec4, my_vec3, bag );
		GLSL_SWIZZLE( my_vec4, my_vec3, arg );
		GLSL_SWIZZLE( my_vec4, my_vec3, arb );
		GLSL_SWIZZLE( my_vec4, my_vec3, agr );
		GLSL_SWIZZLE( my_vec4, my_vec3, agb );
		GLSL_SWIZZLE( my_vec4, my_vec3, abr );
		GLSL_LAST_SWIZZLE( my_vec4, my_vec3, abg );
		GLSL_FIRST_SWIZZLE( my_vec4, my_vec4, xyzw );
		GLSL_SWIZZLE( my_vec4, my_vec4, xyxy );
		GLSL_SWIZZLE( my_vec4, my_vec4, xxzz );
		GLSL_SWIZZLE( my_vec4, my_vec4, xyww );
		GLSL_SWIZZLE( my_vec4, my_vec4, xywz );
		GLSL_SWIZZLE( my_vec4, my_vec4, xzyw );
		GLSL_SWIZZLE( my_vec4, my_vec4, xzwy );
		GLSL_SWIZZLE( my_vec4, my_vec4, xwyz );
		GLSL_SWIZZLE( my_vec4, my_vec4, xwzy );
		GLSL_SWIZZLE( my_vec4, my_vec4, yxzw );
		GLSL_SWIZZLE( my_vec4, my_vec4, yxwz );
		GLSL_SWIZZLE( my_vec4, my_vec4, yzxw );
		GLSL_SWIZZLE( my_vec4, my_vec4, yzwx );
		GLSL_SWIZZLE( my_vec4, my_vec4, ywxz );
		GLSL_SWIZZLE( my_vec4, my_vec4, ywzx );
		GLSL_SWIZZLE( my_vec4, my_vec4, zxyw );
		GLSL_SWIZZLE( my_vec4, my_vec4, zxwy );
		GLSL_SWIZZLE( my_vec4, my_vec4, zyxw );
		GLSL_SWIZZLE( my_vec4, my_vec4, zywx );
		GLSL_SWIZZLE( my_vec4, my_vec4, zwxy );
		GLSL_SWIZZLE( my_vec4, my_vec4, zwyx );
		GLSL_SWIZZLE( my_vec4, my_vec4, wxyz );
		GLSL_SWIZZLE( my_vec4, my_vec4, wxzy );
		GLSL_SWIZZLE( my_vec4, my_vec4, wyxz );
		GLSL_SWIZZLE( my_vec4, my_vec4, wyzx );
		GLSL_SWIZZLE( my_vec4, my_vec4, wzxy );
		GLSL_SWIZZLE( my_vec4, my_vec4, wzyx );
		GLSL_SWIZZLE( my_vec4, my_vec4, stpq );
		GLSL_SWIZZLE( my_vec4, my_vec4, stqq );
		GLSL_SWIZZLE( my_vec4, my_vec4, stqp );
		GLSL_SWIZZLE( my_vec4, my_vec4, sptq );
		GLSL_SWIZZLE( my_vec4, my_vec4, spqt );
		GLSL_SWIZZLE( my_vec4, my_vec4, sqtp );
		GLSL_SWIZZLE( my_vec4, my_vec4, sqpt );
		GLSL_SWIZZLE( my_vec4, my_vec4, tspq );
		GLSL_SWIZZLE( my_vec4, my_vec4, tsqp );
		GLSL_SWIZZLE( my_vec4, my_vec4, tpsq );
		GLSL_SWIZZLE( my_vec4, my_vec4, tpqs );
		GLSL_SWIZZLE( my_vec4, my_vec4, tqsp );
		GLSL_SWIZZLE( my_vec4, my_vec4, tqps );
		GLSL_SWIZZLE( my_vec4, my_vec4, pstq );
		GLSL_SWIZZLE( my_vec4, my_vec4, psqt );
		GLSL_SWIZZLE( my_vec4, my_vec4, ptsq );
		GLSL_SWIZZLE( my_vec4, my_vec4, ptqs );
		GLSL_SWIZZLE( my_vec4, my_vec4, pqst );
		GLSL_SWIZZLE( my_vec4, my_vec4, pqts );
		GLSL_SWIZZLE( my_vec4, my_vec4, qstp );
		GLSL_SWIZZLE( my_vec4, my_vec4, qspt );
		GLSL_SWIZZLE( my_vec4, my_vec4, qtsp );
		GLSL_SWIZZLE( my_vec4, my_vec4, qtps );
		GLSL_SWIZZLE( my_vec4, my_vec4, qpst );
		GLSL_SWIZZLE( my_vec4, my_vec4, qpts );
		GLSL_SWIZZLE( my_vec4, my_vec4, rgba );
		GLSL_SWIZZLE( my_vec4, my_vec4, rgab );
		GLSL_SWIZZLE( my_vec4, my_vec4, rbga );
		GLSL_SWIZZLE( my_vec4, my_vec4, rbag );
		GLSL_SWIZZLE( my_vec4, my_vec4, ragb );
		GLSL_SWIZZLE( my_vec4, my_vec4, rabg );
		GLSL_SWIZZLE( my_vec4, my_vec4, grba );
		GLSL_SWIZZLE( my_vec4, my_vec4, grab );
		GLSL_SWIZZLE( my_vec4, my_vec4, gbra );
		GLSL_SWIZZLE( my_vec4, my_vec4, gbar );
		GLSL_SWIZZLE( my_vec4, my_vec4, garb );
		GLSL_SWIZZLE( my_vec4, my_vec4, gabr );
		GLSL_SWIZZLE( my_vec4, my_vec4, brga );
		GLSL_SWIZZLE( my_vec4, my_vec4, brag );
		GLSL_SWIZZLE( my_vec4, my_vec4, bgra );
		GLSL_SWIZZLE( my_vec4, my_vec4, bgar );
		GLSL_SWIZZLE( my_vec4, my_vec4, barg );
		GLSL_SWIZZLE( my_vec4, my_vec4, bagr );
		GLSL_SWIZZLE( my_vec4, my_vec4, argb );
		GLSL_SWIZZLE( my_vec4, my_vec4, arbg );
		GLSL_SWIZZLE( my_vec4, my_vec4, agrb );
		GLSL_SWIZZLE( my_vec4, my_vec4, agbr );
		GLSL_SWIZZLE( my_vec4, my_vec4, abrg );
		GLSL_LAST_SWIZZLE( my_vec4, my_vec4, abgr );
	};

	template< typename ValueT >
	inline Vec4T< ValueT > operator*( ValueT const & p_lhs, Vec4T< ValueT > const & p_rhs );
}

#include "GlslVec.inl"

#endif
