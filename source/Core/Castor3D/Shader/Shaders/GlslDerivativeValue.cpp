#include "Castor3D/Shader/Shaders/GlslDerivativeValue.hpp"

namespace castor3d::shader
{
	namespace deriv
	{
		template< typename ReturnT, typename FuncT, typename DerivT >
		ReturnT applyMbr( FuncT func, std::string const & name
			, DerivT const & pa )
		{
			auto & writer = sdw::findWriterMandat( pa );
			auto function = writer.template implementFunction< ReturnT >( name
				, [&writer, func]( DerivT const & a )
				{
					auto a00 = writer.declLocale( "a00", a.value() );
					auto a10 = writer.declLocale( "a10", a.value() + a.dPdx() );
					auto a01 = writer.declLocale( "a01", a.value() + a.dPdy() );
					auto r00 = writer.declLocale( "r00", ( a00.*func )() );
					auto r10 = writer.declLocale( "r10", ( a10.*func )() );
					auto r01 = writer.declLocale( "r01", ( a01.*func )() );
					writer.returnStmt( ReturnT{ r00, r10 - r00, r01 - r00 } );
				}
				, sdw::InParam< DerivT >{ writer, "a" } );
			return function( pa );
		}

		template< typename ReturnT, typename FuncT, typename DerivT >
		sdw::ReturnWrapperT< ReturnT > apply( FuncT func, std::string const & name
			, DerivT const & pa )
		{
			auto & writer = sdw::findWriterMandat( pa );
			auto function = writer.template implementFunction< ReturnT >( name
				, [&writer, func]( DerivT const & a )
				{
					auto a00 = writer.declLocale( "a00", a.value() );
					auto a10 = writer.declLocale( "a10", a.value() + a.dPdx() );
					auto a01 = writer.declLocale( "a01", a.value() + a.dPdy() );
					auto r00 = writer.declLocale( "r00", func( a00 ) );
					auto r10 = writer.declLocale( "r10", func( a10 ) );
					auto r01 = writer.declLocale( "r01", func( a01 ) );
					writer.returnStmt( ReturnT{ r00, r10 - r00, r01 - r00 } );
				}
				, sdw::InParam< DerivT >{ writer, "a" } );
			return function( pa );
		}

		template< typename ReturnT, typename FuncT, typename DerivT >
		sdw::ReturnWrapperT< ReturnT > apply( FuncT func, std::string const & name
			, DerivT const & pa, DerivT const & pb )
		{
			auto & writer = sdw::findWriterMandat( pa, pb );
			auto function = writer.template implementFunction< ReturnT >( name
				, [&writer, func]( DerivT const & a
					, DerivT const & b )
				{
					auto a00 = writer.declLocale( "a00", a.value() );
					auto a10 = writer.declLocale( "a10", a.value() + a.dPdx() );
					auto a01 = writer.declLocale( "a01", a.value() + a.dPdy() );
					auto b00 = writer.declLocale( "b00", b.value() );
					auto b10 = writer.declLocale( "b10", b.value() + b.dPdx() );
					auto b01 = writer.declLocale( "b01", b.value() + b.dPdy() );
					auto r00 = writer.declLocale( "r00", func( a00, b00 ) );
					auto r10 = writer.declLocale( "r10", func( a10, b10 ) );
					auto r01 = writer.declLocale( "r01", func( a01, b01 ) );
					writer.returnStmt( ReturnT{ r00, r10 - r00, r01 - r00 } );
				}
				, sdw::InParam< DerivT >{ writer, "a" }
				, sdw::InParam< DerivT >{ writer, "b" } );
			return function( pa, pb );
		}

		template< typename ReturnT, typename FuncT, typename DerivT >
		sdw::ReturnWrapperT< ReturnT > apply( FuncT func, std::string const & name
			, DerivT const & pa, DerivT const & pb, DerivT const & pc )
		{
			auto & writer = sdw::findWriterMandat( pa, pb, pc );
			auto function = writer.template implementFunction< ReturnT >( name
				, [&writer, func]( DerivT const & a
					, DerivT const & b
					, DerivT const & c )
				{
					auto a00 = writer.declLocale( "a00", a.value() );
					auto a10 = writer.declLocale( "a10", a.value() + a.dPdx() );
					auto a01 = writer.declLocale( "a01", a.value() + a.dPdy() );
					auto b00 = writer.declLocale( "b00", b.value() );
					auto b10 = writer.declLocale( "b10", b.value() + b.dPdx() );
					auto b01 = writer.declLocale( "b01", b.value() + b.dPdy() );
					auto c00 = writer.declLocale( "c00", c.value() );
					auto c10 = writer.declLocale( "c10", c.value() + c.dPdx() );
					auto c01 = writer.declLocale( "c01", c.value() + c.dPdy() );
					auto r00 = writer.declLocale( "r00", func( a00, b00, c00 ) );
					auto r10 = writer.declLocale( "r10", func( a10, b10, c10 ) );
					auto r01 = writer.declLocale( "r01", func( a01, b01, c01 ) );
					writer.returnStmt( ReturnT{ r00, r10 - r00, r01 - r00 } );
				}
				, sdw::InParam< DerivT >{ writer, "a" }
				, sdw::InParam< DerivT >{ writer, "b" }
				, sdw::InParam< DerivT >{ writer, "c" } );
			return function( pa, pb, pc );
		}

		template< typename ReturnT, typename FuncT, typename DerivT >
		sdw::ReturnWrapperT< ReturnT > apply( FuncT func, std::string const & name
			, DerivT const & pa, DerivT const & pb, sdw::Float const & pc )
		{
			auto & writer = sdw::findWriterMandat( pa, pb, pc );
			auto function = writer.template implementFunction< ReturnT >( name
				, [&writer, func]( DerivT const & a
					, DerivT const & b
					, sdw::Float const & c )
				{
					auto a00 = writer.declLocale( "a00", a.value() );
					auto a10 = writer.declLocale( "a10", a.value() + a.dPdx() );
					auto a01 = writer.declLocale( "a01", a.value() + a.dPdy() );
					auto b00 = writer.declLocale( "b00", b.value() );
					auto b10 = writer.declLocale( "b10", b.value() + b.dPdx() );
					auto b01 = writer.declLocale( "b01", b.value() + b.dPdy() );
					auto r00 = writer.declLocale( "r00", func( a00, b00, c ) );
					auto r10 = writer.declLocale( "r10", func( a10, b10, c ) );
					auto r01 = writer.declLocale( "r01", func( a01, b01, c ) );
					writer.returnStmt( ReturnT{ r00, r10 - r00, r01 - r00 } );
				}
				, sdw::InParam< DerivT >{ writer, "a" }
				, sdw::InParam< DerivT >{ writer, "b" }
				, sdw::InParam< sdw::Float >{ writer, "c" } );
			return function( pa, pb, pc );
		}

		template< typename ReturnT, typename FuncT, typename DerivT >
		sdw::ReturnWrapperT< ReturnT > apply( FuncT func, std::string const & name
			, DerivT const & pa, sdw::Float const & pb, sdw::Float const & pc )
		{
			auto & writer = sdw::findWriterMandat( pa, pb, pc );
			auto function = writer.template implementFunction< ReturnT >( name
				, [&writer, func]( DerivT const & a
					, sdw::Float const & b
					, sdw::Float const & c )
				{
					auto a00 = writer.declLocale( "a00", a.value() );
					auto a10 = writer.declLocale( "a10", a.value() + a.dPdx() );
					auto a01 = writer.declLocale( "a01", a.value() + a.dPdy() );
					auto r00 = writer.declLocale( "r00", func( a00, b, c ) );
					auto r10 = writer.declLocale( "r10", func( a10, b, c ) );
					auto r01 = writer.declLocale( "r01", func( a01, b, c ) );
					writer.returnStmt( ReturnT{ r00, r10 - r00, r01 - r00 } );
				}
				, sdw::InParam< DerivT >{ writer, "a" }
				, sdw::InParam< sdw::Float >{ writer, "b" }
				, sdw::InParam< sdw::Float >{ writer, "c" } );
			return function( pa, pb, pc );
		}

		template< typename FuncT, typename ValueT, typename ValueU, sdw::StringLiteralT StructNameT >
		DerivativeValueT< ValueU, StructNameT > applyOp( FuncT func, std::string const & name
			, ValueT const & pa, DerivativeValueT< ValueU, StructNameT > const & pb )
		{
			auto & writer = sdw::findWriterMandat( pa, pb );
			auto function = writer.template implementFunction< DerivativeValueT< ValueU, StructNameT > >( name
				, [&writer, func]( ValueT const & a
					, DerivativeValueT< ValueU, StructNameT > const & b )
				{
					auto b00 = writer.declLocale( "a00", b.value() );
					auto b10 = writer.declLocale( "a10", b.value() + b.dPdx() );
					auto b01 = writer.declLocale( "a01", b.value() + b.dPdy() );
					auto r00 = writer.declLocale( "r00", func( a, b00 ) );
					auto r10 = writer.declLocale( "r10", func( a, b10 ) );
					auto r01 = writer.declLocale( "r01", func( a, b01 ) );
					writer.returnStmt( DerivativeValueT< ValueU, StructNameT >{ r00, r10 - r00, r01 - r00 } );
				}
				, sdw::InParam< ValueT >{ writer, "a" }
				, sdw::InParam< DerivativeValueT< ValueU, StructNameT > >{ writer, "b" } );
			return function( pa, pb );
		}

		template< typename FuncT, typename ValueT, sdw::StringLiteralT StructNameT >
		DerivativeValueT< ValueT, StructNameT > applyOp( FuncT func, std::string const & name
			, DerivativeValueT< ValueT, StructNameT > const & pa, ValueT const & pb )
		{
			auto & writer = sdw::findWriterMandat( pa, pb );
			auto function = writer.template implementFunction< DerivativeValueT< ValueT, StructNameT > >( name
				, [&writer, func]( DerivativeValueT< ValueT, StructNameT > const & a
					, ValueT const & b )
				{
					auto a00 = writer.declLocale( "a00", a.value() );
					auto a10 = writer.declLocale( "a10", a.value() + a.dPdx() );
					auto a01 = writer.declLocale( "a01", a.value() + a.dPdy() );
					auto r00 = writer.declLocale( "r00", func( a00, b ) );
					auto r10 = writer.declLocale( "r10", func( a10, b ) );
					auto r01 = writer.declLocale( "r01", func( a01, b ) );
					writer.returnStmt( DerivativeValueT< ValueT, StructNameT >{ r00, r10 - r00, r01 - r00 } );
				}
				, sdw::InParam< DerivativeValueT< ValueT, StructNameT > >{ writer, "a" }
				, sdw::InParam< ValueT >{ writer, "b" } );
			return function( pa, pb );
		}

		template< typename FuncT, typename ValueT, sdw::StringLiteralT StructNameT >
		DerivativeValueT< ValueT, StructNameT > applyOp( FuncT func, std::string const & name
			, DerivativeValueT< ValueT, StructNameT > const & pa, DerivativeValueT< ValueT, StructNameT > const & pb )
		{
			auto & writer = sdw::findWriterMandat( pa, pb );
			auto function = writer.template implementFunction< DerivativeValueT< ValueT, StructNameT > >( name
				, [&writer, func]( DerivativeValueT< ValueT, StructNameT > const & a
					, DerivativeValueT< ValueT, StructNameT > const & b )
				{
					auto a00 = writer.declLocale( "a00", a.value() );
					auto a10 = writer.declLocale( "a10", a.value() + a.dPdx() );
					auto a01 = writer.declLocale( "a01", a.value() + a.dPdy() );
					auto b00 = writer.declLocale( "b00", b.value() );
					auto b10 = writer.declLocale( "b10", b.value() + b.dPdx() );
					auto b01 = writer.declLocale( "b01", b.value() + b.dPdy() );
					auto r00 = writer.declLocale( "r00", func( a00, b00 ) );
					auto r10 = writer.declLocale( "r10", func( a10, b10 ) );
					auto r01 = writer.declLocale( "r01", func( a01, b01 ) );
					writer.returnStmt( DerivativeValueT< ValueT, StructNameT >{ r00, r10 - r00, r01 - r00 } );
				}
				, sdw::InParam< DerivativeValueT< ValueT, StructNameT > >{ writer, "a" }
				, sdw::InParam< DerivativeValueT< ValueT, StructNameT > >{ writer, "b" } );
			return function( pa, pb );
		}

		template< typename ReturnT, typename FuncT, typename ValueT, sdw::StringLiteralT StructNameT >
		sdw::ReturnWrapperT< ReturnT > applyMbrSel( FuncT func, std::string const & name
			, DerivativeValueT< ValueT, StructNameT > const & pa )
		{
			auto & writer = sdw::findWriterMandat( pa );
			auto function = writer.template implementFunction< ReturnT >( name
				, [&writer, func]( DerivativeValueT< ValueT, StructNameT > const & a )
				{
					writer.returnStmt( ReturnT{ ( a.value().*func )()
						, ( a.dPdx().*func )()
						, ( a.dPdy().*func )() } );
				}
				, sdw::InParam< DerivativeValueT< ValueT, StructNameT > >{ writer, "a" } );
			return function( pa );
		}

		template< typename ReturnT, typename FuncT, typename ValueT, sdw::StringLiteralT StructNameT >
		sdw::ReturnWrapperT< ReturnT > applySel( FuncT func, std::string const & name
			, DerivativeValueT< ValueT, StructNameT > const & pa )
		{
			auto & writer = sdw::findWriterMandat( pa );
			auto function = writer.template implementFunction< ReturnT >( name
				, [&writer, func]( DerivativeValueT< ValueT, StructNameT > const & a )
				{
					writer.returnStmt( ReturnT{ func( a.value() )
						, func( a.dPdx() )
						, func( a.dPdy() ) } );
				}
				, sdw::InParam< DerivativeValueT< ValueT, StructNameT > >{ writer, "a" } );
			return function( pa );
		}
	}

	//*********************************************************************************************

	sdw::expr::ExprPtr makeRawExpr( DerivFloat const & value )
	{
		return sdw::makeExpr( value.value() );
	}

	sdw::expr::ExprPtr makeRawExpr( DerivVec2 const & value )
	{
		return sdw::makeExpr( value.value() );
	}

	sdw::expr::ExprPtr makeRawExpr( DerivVec3 const & value )
	{
		return sdw::makeExpr( value.value() );
	}

	sdw::expr::ExprPtr makeRawExpr( DerivVec4 const & value )
	{
		return sdw::makeExpr( value.value() );
	}

	RetDerivFloat dot( DerivVec3 const x, DerivVec3 const y )
	{
		using Func = sdw::RetFloat( * )( sdw::Vec3 const, sdw::Vec3 const );
		return deriv::apply< DerivFloat, Func >( sdw::dot, "derivDot3", x, y );
	}

	RetDerivVec3 cross( DerivVec3 const x, DerivVec3 const y )
	{
		using Func = sdw::RetVec3( * )( sdw::Vec3 const, sdw::Vec3 const );
		return deriv::apply< DerivVec3, Func >( sdw::cross, "derivCross3", x, y );
	}

	RetDerivVec3 normalize( DerivVec3 const v )
	{
		using Func = sdw::RetVec3( * )( sdw::Vec3 const );
		return deriv::apply< DerivVec3, Func >( sdw::normalize, "derivNormalize3", v );
	}

	RetDerivVec4 normalize( DerivVec4 const v )
	{
		using Func = sdw::RetVec4( * )( sdw::Vec4 const );
		return deriv::apply< DerivVec4, Func >( sdw::normalize, "derivNormalize4", v );
	}

	RetDerivVec3 refract( DerivVec3 const i, DerivVec3 const n, sdw::Float const ior )
	{
		using Func = sdw::RetVec3( * )( sdw::Vec3 const, sdw::Vec3 const, sdw::Float const );
		return deriv::apply< DerivVec3, Func >( sdw::refract, "derivRefract3", i, n, ior );
	}

	RetDerivFloat clamp( DerivFloat const v, sdw::Float const min, sdw::Float const max )
	{
		using Func = sdw::RetFloat( * )( sdw::Float const, sdw::Float const, sdw::Float const );
		return deriv::apply< DerivFloat, Func >( sdw::clamp, "derivClamp1", v, min, max );
	}

	RetDerivFloat length( DerivVec3 const v )
	{
		using Func = sdw::RetFloat( * )( sdw::Vec3 const );
		return deriv::apply< DerivFloat, Func >( sdw::length, "derivLength3", v );
	}

	RetDerivFloat max( DerivFloat const x, DerivFloat const y )
	{
		using Func = sdw::RetFloat( * )( sdw::Float const, sdw::Float const );
		return deriv::apply< DerivFloat, Func >( sdw::max, "derivMax1", x, y );
	}

	RetDerivVec3 mix( DerivVec3 const a, DerivVec3 const b, DerivVec3 const c )
	{
		using Func = sdw::RetVec3( * )( sdw::Vec3 const, sdw::Vec3 const, sdw::Vec3 const );
		return deriv::apply< DerivVec3, Func >( sdw::mix, "derivMix3", a, b, c );
	}

	RetDerivFloat fma( DerivFloat const a, DerivFloat const b, DerivFloat const c )
	{
		using Func = sdw::RetFloat( * )( sdw::Float const, sdw::Float const, sdw::Float const );
		return deriv::apply< DerivFloat, Func >( sdw::fma, "derivFma1", a, b, c );
	}

	RetDerivVec2 fma( DerivVec2 const a, DerivVec2 const b, DerivVec2 const c )
	{
		using Func = sdw::RetVec2( * )( sdw::Vec2 const, sdw::Vec2 const, sdw::Vec2 const );
		return deriv::apply< DerivVec2, Func >( sdw::fma, "derivFma2", a, b, c );
	}

	RetDerivVec3 fma( DerivVec3 const a, DerivVec3 const b, DerivVec3 const c )
	{
		using Func = sdw::RetVec3( * )( sdw::Vec3 const, sdw::Vec3 const, sdw::Vec3 const );
		return deriv::apply< DerivVec3, Func >( sdw::fma, "derivFma3", a, b, c );
	}

	RetDerivVec4 fma( DerivVec4 const a, DerivVec4 const b, DerivVec4 const c )
	{
		using Func = sdw::RetVec4( * )( sdw::Vec4 const, sdw::Vec4 const, sdw::Vec4 const );
		return deriv::apply< DerivVec4, Func >( sdw::fma, "derivFma4", a, b, c );
	}

	sdw::Float fwidth( DerivFloat const a )
	{
		auto tmp = a.getWriter()->declLocale( "tmpFwidth1", a );
		return sdw::abs( tmp.dPdx() ) + sdw::abs( tmp.dPdy() );
	}

	sdw::Vec2 fwidth( DerivVec2 const a )
	{
		auto tmp = a.getWriter()->declLocale( "tmpFwidth2", a );
		return sdw::abs( tmp.dPdx() ) + sdw::abs( tmp.dPdy() );
	}

	sdw::Vec3 fwidth( DerivVec3 const a )
	{
		auto tmp = a.getWriter()->declLocale( "tmpFwidth3", a );
		return sdw::abs( tmp.dPdx() ) + sdw::abs( tmp.dPdy() );
	}

	sdw::Vec4 fwidth( DerivVec4 const a )
	{
		auto tmp = a.getWriter()->declLocale( "tmpFwidth4", a );
		return sdw::abs( tmp.dPdx() ) + sdw::abs( tmp.dPdy() );
	}

	DerivFloat negate( DerivFloat const a )
	{
		using Func = sdw::Float ( sdw::Float::* )()const;
		return deriv::applyMbr< DerivFloat, Func >( &sdw::Float::operator-, "derivNegate1", a );
	}

	DerivVec2 negate( DerivVec2 const a )
	{
		using Func = sdw::Vec2( sdw::Vec2::* )( )const;
		return deriv::applyMbr< DerivVec2, Func >( &sdw::Vec2::operator-, "derivNegate2", a );
	}

	DerivVec3 negate( DerivVec3 const a )
	{
		using Func = sdw::Vec3( sdw::Vec3::* )( )const;
		return deriv::applyMbr< DerivVec3, Func >( &sdw::Vec3::operator-, "derivNegate3", a );
	}

	DerivVec4 negate( DerivVec4 const a )
	{
		using Func = sdw::Vec4( sdw::Vec4::* )( )const;
		return deriv::applyMbr< DerivVec4, Func >( &sdw::Vec4::operator-, "derivNegate4", a );
	}

	sdw::Float computeMip( DerivVec2 const & uv
		, sdw::Vec2 const & texSize )
	{
		// see https://registry.khronos.org/OpenGL/extensions/EXT/EXT_shader_texture_lod.txt
		auto dSdx = uv.dPdx().x();
		auto dSdy = uv.dPdy().x();
		auto dTdx = uv.dPdx().y();
		auto dTdy = uv.dPdy().y();
		auto dUdx = texSize.x() * dSdx;
		auto dUdy = texSize.x() * dSdy;
		auto dVdx = texSize.y() * dTdx;
		auto dVdy = texSize.y() * dTdy;
		return 0.5_f * log2( max( dUdx * dUdx + dVdx * dVdx
			, dUdy * dUdy + dVdy * dVdy ) );
	}

	DerivFloat derivFloat( sdw::Float const v )
	{
		return { v, 0.0_f, 0.0_f };
	}

	DerivFloat derivX( DerivVec2 const v )
	{
		using Func = sdw::Float( sdw::Vec2::* )( )const;
		return deriv::applyMbrSel< DerivFloat, Func >( &sdw::Vec2::x, "derivDerivX2", v );
	}

	DerivFloat derivX( DerivVec3 const v )
	{
		using Func = sdw::Float( sdw::Vec3::* )( )const;
		return deriv::applyMbrSel< DerivFloat, Func >( &sdw::Vec3::x, "derivDerivX3", v );
	}

	DerivFloat derivX( DerivVec4 const v )
	{
		using Func = sdw::Float( sdw::Vec4::* )( )const;
		return deriv::applyMbrSel< DerivFloat, Func >( &sdw::Vec4::x, "derivDerivX4", v );
	}

	DerivFloat derivY( DerivVec2 const v )
	{
		using Func = sdw::Float( sdw::Vec2::* )( )const;
		return deriv::applyMbrSel< DerivFloat, Func >( &sdw::Vec2::y, "derivDerivY2", v );
	}

	DerivFloat derivY( DerivVec3 const v )
	{
		using Func = sdw::Float( sdw::Vec3::* )( )const;
		return deriv::applyMbrSel< DerivFloat, Func >( &sdw::Vec3::y, "derivDerivY3", v );
	}

	DerivFloat derivY( DerivVec4 const v )
	{
		using Func = sdw::Float( sdw::Vec4::* )( )const;
		return deriv::applyMbrSel< DerivFloat, Func >( &sdw::Vec4::y, "derivDerivY4", v );
	}

	DerivFloat derivZ( DerivVec3 const v )
	{
		using Func = sdw::Float( sdw::Vec3::* )( )const;
		return deriv::applyMbrSel< DerivFloat, Func >( &sdw::Vec3::z, "derivDerivZ3", v );
	}

	DerivFloat derivZ( DerivVec4 const v )
	{
		using Func = sdw::Float( sdw::Vec4::* )( )const;
		return deriv::applyMbrSel< DerivFloat, Func >( &sdw::Vec4::z, "derivDerivZ4", v );
	}

	DerivFloat derivW( DerivVec4 const v )
	{
		using Func = sdw::Float( sdw::Vec4::* )( )const;
		return deriv::applyMbrSel< DerivFloat, Func >( &sdw::Vec4::w, "derivDerivW4", v );
	}

	DerivVec2 derivVec2( sdw::Float const v )
	{
		return { vec2( v ), vec2( 0.0_f ), vec2( 0.0_f ) };
	}

	DerivVec2 derivVec2( sdw::Vec2 const v )
	{
		return { v, vec2( 0.0_f ), vec2( 0.0_f ) };
	}

	DerivVec2 derivVec2( DerivFloat const v )
	{
		auto tmp = v.getWriter()->declLocale( "tmpVDerivVec21", v );
		return { vec2( tmp.value() ), vec2( tmp.dPdx() ), vec2( tmp.dPdy() ) };
	}

	DerivVec2 derivVec2( DerivVec3 const v )
	{
		auto tmp = v.getWriter()->declLocale( "tmpVDerivVec23", v );
		return { tmp.value().xy(), tmp.dPdx().xy(), tmp.dPdy().xy() };
	}

	DerivVec2 derivVec2( DerivVec4 const v )
	{
		auto tmp = v.getWriter()->declLocale( "tmpVDerivVec24", v );
		return { tmp.value().xy(), tmp.dPdx().xy(), tmp.dPdy().xy() };
	}

	DerivVec2 derivVec2( DerivFloat const v, sdw::Float const a )
	{
		auto tmp = v.getWriter()->declLocale( "tmpVDerivVec211", v );
		return { vec2( tmp.value(), a ), vec2( tmp.dPdx(), 0.0_f ), vec2( tmp.dPdy(), 0.0_f ) };
	}

	DerivVec3 derivVec3( sdw::Float const v )
	{
		return { vec3( v ), vec3( 0.0_f ), vec3( 0.0_f ) };
	}

	DerivVec3 derivVec3( sdw::Vec3 const v )
	{
		return { v, vec3( 0.0_f ), vec3( 0.0_f ) };
	}

	DerivVec3 derivVec3( DerivFloat const v )
	{
		auto tmp = v.getWriter()->declLocale( "tmpVDerivVec31", v );
		return { vec3( tmp.value() ), vec3( tmp.dPdx() ), vec3( tmp.dPdy() ) };
	}

	DerivVec3 derivVec3( DerivVec4 const v )
	{
		auto tmp = v.getWriter()->declLocale( "tmpVDerivVec34", v );
		return { tmp.value().xyz(), tmp.dPdx().xyz(), tmp.dPdy().xyz() };
	}

	DerivVec4 derivVec4( sdw::Float const v )
	{
		return { vec4( v ), vec4( 0.0_f ), vec4( 0.0_f ) };
	}

	DerivVec4 derivVec4( sdw::Vec4 const v )
	{
		return { v, vec4( 0.0_f ), vec4( 0.0_f ) };
	}

	DerivVec4 derivVec4( DerivFloat const v )
	{
		auto tmp = v.getWriter()->declLocale( "tmpVDerivVec41", v );
		return { vec4( tmp.value() ), vec4( tmp.dPdx() ), vec4( tmp.dPdy() ) };
	}

	DerivVec4 derivVec4( DerivVec3 const v, sdw::Float const a )
	{
		auto tmp = v.getWriter()->declLocale( "tmpVDerivVec43N1", v );
		return { vec4( tmp.value(), a ), vec4( tmp.dPdx(), 0.0_f ), vec4( tmp.dPdy(), 0.0_f ) };
	}

	DerivVec4 derivVec4( DerivVec3 const v, DerivFloat const a )
	{
		auto tmpV = v.getWriter()->declLocale( "tmpVDerivVec43D1", v );
		auto tmpA = v.getWriter()->declLocale( "tmpADerivVec43D1", a );
		return { vec4( tmpV.value(), tmpA.value() ), vec4( tmpV.dPdx(), tmpA.dPdx() ), vec4( tmpV.dPdy(), tmpA.dPdy() ) };
	}

	void negateXYZ( sdw::Vec4 in )
	{
		in.xyz() = -in.xyz();
	}

	void negateXYZ( DerivVec4 in )
	{
		auto tmp = in.getWriter()->declLocale( "tmpNegateXYZ", in );
		negateXYZ( tmp.value() );
		negateXYZ( tmp.dPdx() );
		negateXYZ( tmp.dPdy() );
		in = tmp;
	}

	void mulXY( sdw::Vec4 in, sdw::Vec2 const mul )
	{
		in.xy() *= mul;
	}

	void mulXY( DerivVec4 in, sdw::Vec2 const mul )
	{
		auto tmp = in.getWriter()->declLocale( "tmpMulXY", in );
		mulXY( tmp.value(), mul );
		mulXY( tmp.dPdx(), mul );
		mulXY( tmp.dPdy(), mul );
		in = tmp;
	}

	void addXYZ( sdw::Vec4 in, sdw::Vec3 const add )
	{
		in.xyz() += add;
	}

	void addXYZ( DerivVec4 in, sdw::Vec3 const add )
	{
		addXYZ( in.value(), add );
	}

	void addXYZ( DerivVec4 in, DerivVec3 const add )
	{
		auto tmp = in.getWriter()->declLocale( "tmpAddXYZ", getXYZ( in ) );
		tmp += add;
		in.value().xyz() = tmp.value();
		in.dPdx().xyz() = tmp.dPdx();
		in.dPdy().xyz() = tmp.dPdy();
	}

	sdw::Vec3 getRaw( sdw::Vec3 const in )
	{
		return in;
	}

	sdw::Vec3 getRaw( DerivVec3 const in )
	{
		return getRaw( in.value() );
	}

	sdw::Vec4 getRaw( sdw::Vec4 const in )
	{
		return in;
	}

	sdw::Vec4 getRaw( DerivVec4 const in )
	{
		return getRaw( in.value() );
	}

	sdw::Vec3 getRawXYZ( sdw::Vec3 const in )
	{
		return in;
	}

	sdw::Vec3 getRawXYZ( DerivVec3 const in )
	{
		return getRawXYZ( in.value() );
	}

	sdw::Vec3 getRawXYZ( sdw::Vec4 const in )
	{
		return in.xyz();
	}

	sdw::Vec3 getRawXYZ( DerivVec4 const in )
	{
		return getRawXYZ( in.value() );
	}

	sdw::Vec3 getXYZ( sdw::Vec4 const in )
	{
		return in.xyz();
	}

	DerivVec3 getXYZ( DerivVec4 const in )
	{
		using Func = sdw::Vec3( * )( sdw::Vec4 const );
		return deriv::applySel< DerivVec3, Func >( getXYZ, "derivGetXYZ4", in );
	}

	sdw::Vec3 getXYW( sdw::Vec4 const in )
	{
		return in.xyw();
	}

	DerivVec3 getXYW( DerivVec4 const in )
	{
		using Func = sdw::Vec3( * )( sdw::Vec4 const );
		return deriv::applySel< DerivVec3, Func >( getXYW, "derivGetXYW4", in );
	}

	sdw::Float getW( sdw::Vec4 const in )
	{
		return in.w();
	}

	DerivFloat getW( DerivVec4 const in )
	{
		using Func = sdw::Float( * )( sdw::Vec4 const );
		return deriv::applySel< DerivFloat, Func >( getW, "derivGetW4", in );
	}

	DerivVec4 operator*( sdw::Mat4 const lhs, DerivVec4 const rhs )
	{
		using Func = sdw::Vec4( * )( sdw::Mat4 const &, sdw::Vec4 const & );
		return deriv::applyOp< Func >( sdw::operator*, "derivMultiplayM4D4", lhs, rhs );
	}

	DerivVec3 operator*( sdw::Mat3 const lhs, DerivVec3 const rhs )
	{
		using Func = sdw::Vec3( * )( sdw::Mat3 const &, sdw::Vec3 const & );
		return deriv::applyOp< Func >( sdw::operator*, "derivMultiplayM3D3", lhs, rhs );
	}

	DerivFloat operator+( DerivFloat const lhs, DerivFloat const rhs )
	{
		using Func = sdw::Float( * )( sdw::Float const &, sdw::Float const & );
		return deriv::applyOp< Func >( sdw::operator+, "derivAdd1D1", lhs, rhs );
	}

	DerivFloat operator+( DerivFloat const lhs, sdw::Float const rhs )
	{
		using Func = sdw::Float( * )( sdw::Float const &, sdw::Float const & );
		return deriv::applyOp< Func >( sdw::operator+, "derivAdd1N1", lhs, rhs );
	}

	DerivFloat operator-( DerivFloat const lhs, DerivFloat const rhs )
	{
		using Func = sdw::Float( * )( sdw::Float const &, sdw::Float const & );
		return deriv::applyOp< Func >( sdw::operator-, "derivSubtract1D1", lhs, rhs );
	}

	DerivFloat operator-( DerivFloat const lhs, sdw::Float const rhs )
	{
		using Func = sdw::Float( * )( sdw::Float const &, sdw::Float const & );
		return deriv::applyOp< Func >( sdw::operator-, "derivSubtract1N1", lhs, rhs );
	}

	DerivFloat operator*( DerivFloat const lhs, DerivFloat const rhs )
	{
		using Func = sdw::Float( * )( sdw::Float const &, sdw::Float const & );
		return deriv::applyOp< Func >( sdw::operator*, "derivMultiply1D1", lhs, rhs );
	}

	DerivFloat operator*( DerivFloat const lhs, sdw::Float const rhs )
	{
		using Func = sdw::Float( * )( sdw::Float const &, sdw::Float const & );
		return deriv::applyOp< Func >( sdw::operator*, "derivMultiply1N1", lhs, rhs );
	}

	DerivVec2 operator+( DerivVec2 const lhs, DerivVec2 const rhs )
	{
		using Func = sdw::Vec2( * )( sdw::Vec2 const &, sdw::Vec2 const & );
		return deriv::applyOp< Func >( sdw::operator+, "derivAdd2D2", lhs, rhs );
	}

	DerivVec2 operator+( DerivVec2 const lhs, sdw::Vec2 const rhs )
	{
		using Func = sdw::Vec2( * )( sdw::Vec2 const &, sdw::Vec2 const & );
		return deriv::applyOp< Func >( sdw::operator+, "derivAdd2N2", lhs, rhs );
	}

	DerivVec2 operator+( DerivVec2 const lhs, DerivFloat const rhs )
	{
		return lhs + derivVec2( rhs );
	}

	DerivVec2 operator+( DerivVec2 const lhs, sdw::Float const rhs )
	{
		return lhs + derivVec2( rhs );
	}

	DerivVec2 operator-( DerivVec2 const lhs, DerivVec2 const rhs )
	{
		using Func = sdw::Vec2( * )( sdw::Vec2 const &, sdw::Vec2 const & );
		return deriv::applyOp< Func >( sdw::operator-, "derivSubtract2D2", lhs, rhs );
	}

	DerivVec2 operator-( DerivVec2 const lhs, sdw::Vec2 const rhs )
	{
		using Func = sdw::Vec2( * )( sdw::Vec2 const &, sdw::Vec2 const & );
		return deriv::applyOp< Func >( sdw::operator-, "derivSubtract2N2", lhs, rhs );
	}

	DerivVec2 operator-( DerivVec2 const lhs, DerivFloat const rhs )
	{
		return lhs - derivVec2( rhs );
	}

	DerivVec2 operator-( DerivVec2 const lhs, sdw::Float const rhs )
	{
		return lhs - derivVec2( rhs );
	}

	DerivVec2 operator*( DerivVec2 const lhs, DerivVec2 const rhs )
	{
		using Func = sdw::Vec2( * )( sdw::Vec2 const &, sdw::Vec2 const & );
		return deriv::applyOp< Func >( sdw::operator*, "derivMultiply2D2", lhs, rhs );
	}

	DerivVec2 operator*( DerivVec2 const lhs, sdw::Vec2 const rhs )
	{
		using Func = sdw::Vec2( * )( sdw::Vec2 const &, sdw::Vec2 const & );
		return deriv::applyOp< Func >( sdw::operator*, "derivMultiply2N2", lhs, rhs );
	}

	DerivVec2 operator*( DerivVec2 const lhs, DerivFloat const rhs )
	{
		return lhs * derivVec2( rhs );
	}

	DerivVec2 operator*( DerivVec2 const lhs, sdw::Float const rhs )
	{
		return lhs * derivVec2( rhs );
	}

	DerivVec3 operator+( DerivVec3 const lhs, DerivVec3 const rhs )
	{
		using Func = sdw::Vec3( * )( sdw::Vec3 const &, sdw::Vec3 const & );
		return deriv::applyOp< Func >( sdw::operator+, "derivAdd3D3", lhs, rhs );
	}

	DerivVec3 operator+( DerivVec3 const lhs, sdw::Vec3 const rhs )
	{
		using Func = sdw::Vec3( * )( sdw::Vec3 const &, sdw::Vec3 const & );
		return deriv::applyOp< Func >( sdw::operator+, "derivAdd3N3", lhs, rhs );
	}

	DerivVec3 operator+( DerivVec3 const lhs, DerivFloat const rhs )
	{
		return lhs + derivVec3( rhs );
	}

	DerivVec3 operator+( DerivVec3 const lhs, sdw::Float const rhs )
	{
		return lhs + derivVec3( rhs );
	}

	DerivVec3 operator-( DerivVec3 const lhs, DerivVec3 const rhs )
	{
		using Func = sdw::Vec3( * )( sdw::Vec3 const &, sdw::Vec3 const & );
		return deriv::applyOp< Func >( sdw::operator-, "derivSubtract3D3", lhs, rhs );
	}

	DerivVec3 operator-( DerivVec3 const lhs, sdw::Vec3 const rhs )
	{
		using Func = sdw::Vec3( * )( sdw::Vec3 const &, sdw::Vec3 const & );
		return deriv::applyOp< Func >( sdw::operator-, "derivSubtract3N3", lhs, rhs );
	}

	DerivVec3 operator-( DerivVec3 const lhs, DerivFloat const rhs )
	{
		return lhs - derivVec3( rhs );
	}

	DerivVec3 operator-( DerivVec3 const lhs, sdw::Float const rhs )
	{
		return lhs - derivVec3( rhs );
	}

	DerivVec3 operator*( DerivVec3 const lhs, DerivVec3 const rhs )
	{
		using Func = sdw::Vec3( * )( sdw::Vec3 const &, sdw::Vec3 const & );
		return deriv::applyOp< Func >( sdw::operator*, "derivMultiply3D3", lhs, rhs );
	}

	DerivVec3 operator*( DerivVec3 const lhs, sdw::Vec3 const rhs )
	{
		using Func = sdw::Vec3( * )( sdw::Vec3 const &, sdw::Vec3 const & );
		return deriv::applyOp< Func >( sdw::operator*, "derivMultiply3N3", lhs, rhs );
	}

	DerivVec3 operator*( DerivVec3 const lhs, DerivFloat const rhs )
	{
		return lhs * derivVec3( rhs );
	}

	DerivVec3 operator*( DerivVec3 const lhs, sdw::Float const rhs )
	{
		return lhs * derivVec3( rhs );
	}

	DerivVec4 operator+( DerivVec4 const lhs, DerivVec4 const rhs )
	{
		using Func = sdw::Vec4( * )( sdw::Vec4 const &, sdw::Vec4 const & );
		return deriv::applyOp< Func >( sdw::operator+, "derivAdd4D4", lhs, rhs );
	}

	DerivVec4 operator+( DerivVec4 const lhs, sdw::Vec4 const rhs )
	{
		using Func = sdw::Vec4( * )( sdw::Vec4 const &, sdw::Vec4 const & );
		return deriv::applyOp< Func >( sdw::operator+, "derivAdd4N4", lhs, rhs );
	}

	DerivVec4 operator+( DerivVec4 const lhs, DerivFloat const rhs )
	{
		return lhs + derivVec4( rhs );
	}

	DerivVec4 operator+( DerivVec4 const lhs, sdw::Float const rhs )
	{
		return lhs + derivVec4( rhs );
	}

	DerivVec4 operator-( DerivVec4 const lhs, DerivVec4 const rhs )
	{
		using Func = sdw::Vec4( * )( sdw::Vec4 const &, sdw::Vec4 const & );
		return deriv::applyOp< Func >( sdw::operator-, "derivSubtract4D4", lhs, rhs );
	}

	DerivVec4 operator-( DerivVec4 const lhs, sdw::Vec4 const rhs )
	{
		using Func = sdw::Vec4( * )( sdw::Vec4 const &, sdw::Vec4 const & );
		return deriv::applyOp< Func >( sdw::operator-, "derivSubtract4N4", lhs, rhs );
	}

	DerivVec4 operator-( DerivVec4 const lhs, DerivFloat const rhs )
	{
		return lhs - derivVec4( rhs );
	}

	DerivVec4 operator-( DerivVec4 const lhs, sdw::Float const rhs )
	{
		return lhs - derivVec4( rhs );
	}

	DerivVec4 operator*( DerivVec4 const lhs, DerivVec4 const rhs )
	{
		using Func = sdw::Vec4( * )( sdw::Vec4 const &, sdw::Vec4 const & );
		return deriv::applyOp< Func >( sdw::operator*, "derivMultiply4D4", lhs, rhs );
	}

	DerivVec4 operator*( DerivVec4 const lhs, sdw::Vec4 const rhs )
	{
		using Func = sdw::Vec4( * )( sdw::Vec4 const &, sdw::Vec4 const & );
		return deriv::applyOp< Func >( sdw::operator*, "derivMultiply4N4", lhs, rhs );
	}

	DerivVec4 operator*( DerivVec4 const lhs, DerivFloat const rhs )
	{
		return lhs * derivVec4( rhs );
	}

	DerivVec4 operator*( DerivVec4 const lhs, sdw::Float const rhs )
	{
		return lhs * derivVec4( rhs );
	}

	//*********************************************************************************************
}
