#include "GlslIntrinsics.hpp"

using namespace castor;

namespace glsl
{
	Vec4 operator*( Vec4 const & lhs, Mat4 const & rhs )
	{
		Vec4 result( findWriter( lhs, rhs ) );
		result.m_value << String( lhs ) << cuT( " * " ) << String( rhs );
		return result;
	}

	Vec4 operator*( Mat4 const & lhs, Vec4 const & rhs )
	{
		Vec4 result( findWriter( lhs, rhs ) );
		result.m_value << String( lhs ) << cuT( " * " ) << String( rhs );
		return result;
	}

	Vec3 operator*( Mat3 const & lhs, Vec3 const & rhs )
	{
		Vec3 result( findWriter( lhs, rhs ) );
		result.m_value << String( lhs ) << cuT( " * " ) << String( rhs );
		return result;
	}

	Float operator+( float lhs, Float const & rhs )
	{
		Float result( findWriter( lhs, rhs ) );
		result.m_value << toString( lhs ) << cuT( " + " ) << String( rhs );
		return result;
	}

	Float operator-( float lhs, Float const & rhs )
	{
		Float result( findWriter( lhs, rhs ) );
		result.m_value << toString( lhs ) << cuT( " - " ) << String( rhs );
		return result;
	}

	Float operator*( float lhs, Float const & rhs )
	{
		Float result( findWriter( lhs, rhs ) );
		result.m_value << toString( lhs ) << cuT( " * " ) << String( rhs );
		return result;
	}

	Float operator/( float lhs, Float const & rhs )
	{
		Float result( findWriter( lhs, rhs ) );
		result.m_value << toString( lhs ) << cuT( " / " ) << String( rhs );
		return result;
	}

	Int operator+( int lhs, Int const & rhs )
	{
		Int result( findWriter( lhs, rhs ) );
		result.m_value << toString( lhs ) << cuT( " + " ) << String( rhs );
		return result;
	}

	Int operator-( int lhs, Int const & rhs )
	{
		Int result( findWriter( lhs, rhs ) );
		result.m_value << toString( lhs ) << cuT( " - " ) << String( rhs );
		return result;
	}

	Int operator*( int lhs, Int const & rhs )
	{
		Int result( findWriter( lhs, rhs ) );
		result.m_value << toString( lhs ) << cuT( " * " ) << String( rhs );
		return result;
	}

	Int operator/( int lhs, Int const & rhs )
	{
		Int result( findWriter( lhs, rhs ) );
		result.m_value << toString( lhs ) << cuT( " / " ) << String( rhs );
		return result;
	}

	Int operator%( Int const & lhs, int rhs )
	{
		Int result( findWriter( lhs, rhs ) );
		result.m_value << String( lhs ) << cuT( " % " ) << toString( rhs );
		return result;
	}

	Int operator%( int lhs, Int const & rhs )
	{
		Int result( findWriter( lhs, rhs ) );
		result.m_value << toString( lhs ) << cuT( " % " ) << String( rhs );
		return result;
	}

	Int operator%( Int const & lhs, Int const & rhs )
	{
		Int result( findWriter( lhs, rhs ) );
		result.m_value << String( lhs ) << cuT( " % " ) << String( rhs );
		return result;
	}

	Int bitfieldReverse( Int const & value )
	{
		return writeFunctionCall< Int >( value.m_writer, cuT( "bitfieldReverse" ), value );
	}

	UInt bitfieldReverse( UInt const & value )
	{
		return writeFunctionCall< UInt >( value.m_writer, cuT( "bitfieldReverse" ), value );
	}

	Int findMSB( Int value )
	{
		return writeFunctionCall< Int >( value.m_writer, cuT( "findMSB" ), value );
	}

	Int findMSB( UInt value )
	{
		return writeFunctionCall< Int >( value.m_writer, cuT( "findMSB" ), value );
	}

	Int textureSize( Sampler1D const & sampler, Int const lod )
	{
		return writeFunctionCall< Int >( findWriter( sampler, lod ), cuT( "textureSize" ), sampler, lod );
	}

	IVec2 textureSize( Sampler2D const & sampler, Int const & lod )
	{
		return writeFunctionCall< IVec2 >( findWriter( sampler, lod ), cuT( "textureSize" ), sampler, lod );
	}

	IVec3 textureSize( Sampler3D const & sampler, Int const & lod )
	{
		return writeFunctionCall< IVec3 >( findWriter( sampler, lod ), cuT( "textureSize" ), sampler, lod );
	}

	IVec2 textureSize( SamplerCube const & sampler, Int const & lod )
	{
		return writeFunctionCall< IVec2 >( findWriter( sampler, lod ), cuT( "textureSize" ), sampler, lod );
	}

	IVec2 textureSize( Sampler1DArray const & sampler, Int const lod )
	{
		return writeFunctionCall< IVec2 >( findWriter( sampler, lod ), cuT( "textureSize" ), sampler, lod );
	}

	IVec3 textureSize( Sampler2DArray const & sampler, Int const & lod )
	{
		return writeFunctionCall< IVec3 >( findWriter( sampler, lod ), cuT( "textureSize" ), sampler, lod );
	}

	IVec3 textureSize( SamplerCubeArray const & sampler, Int const & lod )
	{
		return writeFunctionCall< IVec3 >( findWriter( sampler, lod ), cuT( "textureSize" ), sampler, lod );
	}

	Int textureSize( Sampler1DShadow const & sampler, Int const lod )
	{
		return writeFunctionCall< Int >( findWriter( sampler, lod ), cuT( "textureSize" ), sampler, lod );
	}

	IVec2 textureSize( Sampler2DShadow const & sampler, Int const & lod )
	{
		return writeFunctionCall< IVec2 >( findWriter( sampler, lod ), cuT( "textureSize" ), sampler, lod );
	}

	IVec2 textureSize( SamplerCubeShadow const & sampler, Int const & lod )
	{
		return writeFunctionCall< IVec2 >( findWriter( sampler, lod ), cuT( "textureSize" ), sampler, lod );
	}

	IVec2 textureSize( Sampler1DArrayShadow const & sampler, Int const lod )
	{
		return writeFunctionCall< IVec2 >( findWriter( sampler, lod ), cuT( "textureSize" ), sampler, lod );
	}

	IVec3 textureSize( Sampler2DArrayShadow const & sampler, Int const & lod )
	{
		return writeFunctionCall< IVec3 >( findWriter( sampler, lod ), cuT( "textureSize" ), sampler, lod );
	}

	IVec3 textureSize( SamplerCubeArrayShadow const & sampler, Int const & lod )
	{
		return writeFunctionCall< IVec3 >( findWriter( sampler, lod ), cuT( "textureSize" ), sampler, lod );
	}

	Vec4 textureGather( Sampler2D const & sampler, Vec2 const & value )
	{
		return writeFunctionCall< Vec4 >( findWriter( sampler, value ), cuT( "textureGather" ), sampler, value );
	}

	Vec4 textureGather( Sampler2DArray const & sampler, Vec3 const & value )
	{
		return writeFunctionCall< Vec4 >( findWriter( sampler, value ), cuT( "textureGather" ), sampler, value );
	}

	Vec4 textureGather( SamplerCube const & sampler, Vec3 const & value )
	{
		return writeFunctionCall< Vec4 >( findWriter( sampler, value ), cuT( "textureGather" ), sampler, value );
	}

	Vec4 textureGather( SamplerCubeArray const & sampler, Vec4 const & value )
	{
		return writeFunctionCall< Vec4 >( findWriter( sampler, value ), cuT( "textureGather" ), sampler, value );
	}

	Vec4 textureGather( Sampler2D const & sampler, Vec2 const & value, Int const & comp )
	{
		return writeFunctionCall< Vec4 >( findWriter( sampler, value, comp ), cuT( "textureGather" ), sampler, value, comp );
	}

	Vec4 textureGather( Sampler2DArray const & sampler, Vec3 const & value, Int const & comp )
	{
		return writeFunctionCall< Vec4 >( findWriter( sampler, value, comp ), cuT( "textureGather" ), sampler, value, comp );
	}

	Vec4 textureGather( SamplerCube const & sampler, Vec3 const & value, Int const & comp )
	{
		return writeFunctionCall< Vec4 >( findWriter( sampler, value, comp ), cuT( "textureGather" ), sampler, value, comp );
	}

	Vec4 textureGather( SamplerCubeArray const & sampler, Vec4 const & value, Int const & comp )
	{
		return writeFunctionCall< Vec4 >( findWriter( sampler, value, comp ), cuT( "textureGather" ), sampler, value, comp );
	}

	Vec4 textureGather( Sampler2DShadow const & sampler, Vec3 const & value, Float const & refZ )
	{
		return writeFunctionCall< Vec4 >( findWriter( sampler, value, refZ ), cuT( "textureGather" ), sampler, value, refZ );
	}

	Vec4 textureGather( Sampler2DArrayShadow const & sampler, Vec3 const & value, Float const & refZ )
	{
		return writeFunctionCall< Vec4 >( findWriter( sampler, value, refZ ), cuT( "textureGather" ), sampler, value, refZ );
	}

	Vec4 textureGather( SamplerCubeShadow const & sampler, Vec3 const & value, Float const & refZ )
	{
		return writeFunctionCall< Vec4 >( findWriter( sampler, value, refZ ), cuT( "textureGather" ), sampler, value, refZ );
	}

	Vec4 textureGather( SamplerCubeArrayShadow const & sampler, Vec4 const & value, Float const & refZ )
	{
		return writeFunctionCall< Vec4 >( findWriter( sampler, value, refZ ), cuT( "textureGather" ), sampler, value, refZ );
	}

	Vec4 texture( Sampler1D const & sampler, Float const & value )
	{
		return findWriter( sampler, value )->texture( sampler, value );
	}

	Vec4 texture( Sampler1D const & sampler, Float const & value, Float const & lod )
	{
		return findWriter( sampler, value, lod )->texture( sampler, value, lod );
	}

	Vec4 texture( Sampler2D const & sampler, Vec2 const & value )
	{
		return findWriter( sampler, value )->texture( sampler, value );
	}

	Vec4 texture( Sampler2D const & sampler, Vec2 const & value, Float const & lod )
	{
		return findWriter( sampler, value, lod )->texture( sampler, value, lod );
	}

	Vec4 texture( Sampler3D const & sampler, Vec3 const & value )
	{
		return findWriter( sampler, value )->texture( sampler, value );
	}

	Vec4 texture( Sampler3D const & sampler, Vec3 const & value, Float const & lod )
	{
		return findWriter( sampler, value, lod )->texture( sampler, value, lod );
	}

	Vec4 texture( SamplerCube const & sampler, Vec3 const & value )
	{
		return findWriter( sampler, value )->texture( sampler, value );
	}

	Vec4 texture( SamplerCube const & sampler, Vec3 const & value, Float const & lod )
	{
		return findWriter( sampler, value, lod )->texture( sampler, value, lod );
	}

	Vec4 textureOffset( Sampler1D const & sampler, Float const & value, Int const offset )
	{
		return findWriter( sampler, value, offset )->textureOffset( sampler, value, offset );
	}

	Vec4 textureOffset( Sampler1D const & sampler, Float const & value, Int const offset, Float const & lod )
	{
		return findWriter( sampler, value, offset, lod )->textureOffset( sampler, value, offset, lod );
	}

	Vec4 textureOffset( Sampler2D const & sampler, Vec2 const & value, IVec2 const offset )
	{
		return findWriter( sampler, value, offset )->textureOffset( sampler, value, offset );
	}

	Vec4 textureOffset( Sampler2D const & sampler, Vec2 const & value, IVec2 const offset, Float const & lod )
	{
		return findWriter( sampler, value, offset, lod )->textureOffset( sampler, value, offset, lod );
	}

	Vec4 textureOffset( Sampler3D const & sampler, Vec3 const & value, IVec3 const offset )
	{
		return findWriter( sampler, value, offset )->textureOffset( sampler, value, offset );
	}

	Vec4 textureOffset( Sampler3D const & sampler, Vec3 const & value, IVec3 const offset, Float const & lod )
	{
		return findWriter( sampler, value, offset, lod )->textureOffset( sampler, value, offset, lod );
	}

	Vec4 textureLodOffset( Sampler1D const & sampler, Float const & value, Float const & lod, Int const offset )
	{
		return findWriter( sampler, value, offset )->textureLodOffset( sampler, value, lod, offset );
	}

	Vec4 textureLodOffset( Sampler2D const & sampler, Vec2 const & value, Float const & lod, IVec2 const offset )
	{
		return findWriter( sampler, value, offset )->textureLodOffset( sampler, value, lod, offset );
	}

	Vec4 textureLodOffset( Sampler3D const & sampler, Vec3 const & value, Float const & lod, IVec3 const offset )
	{
		return findWriter( sampler, value, offset, lod )->textureLodOffset( sampler, value, lod, offset );
	}

	Vec4 texelFetch( SamplerBuffer const & sampler, Type const & value )
	{
		return findWriter( sampler, value )->texelFetch( sampler, value );
	}

	Vec4 texelFetch( Sampler1D const & sampler, Type const & value, Int const & modif )
	{
		return findWriter( sampler, value, modif )->texelFetch( sampler, value, modif );
	}

	Vec4 texelFetch( Sampler2D const & sampler, Type const & value, Int const & modif )
	{
		return findWriter( sampler, value, modif )->texelFetch( sampler, value, modif );
	}

	Vec4 texelFetch( Sampler3D const & sampler, Type const & value, Int const & modif )
	{
		return findWriter( sampler, value, modif )->texelFetch( sampler, value, modif );
	}

	Vec4 texture( Sampler1DArray const & sampler, Vec2 const & value )
	{
		return findWriter( sampler, value )->texture( sampler, value );
	}

	Vec4 texture( Sampler1DArray const & sampler, Vec2 const & value, Vec4 const & lod )
	{
		return findWriter( sampler, value, lod )->texture( sampler, value, lod );
	}

	Vec4 texture( Sampler2DArray const & sampler, Vec3 const & value )
	{
		return findWriter( sampler, value )->texture( sampler, value );
	}

	Vec4 texture( Sampler2DArray const & sampler, Vec3 const & value, Vec4 const & lod )
	{
		return findWriter( sampler, value, lod )->texture( sampler, value, lod );
	}

	Vec4 texture( SamplerCubeArray const & sampler, Vec4 const & value )
	{
		return findWriter( sampler, value )->texture( sampler, value );
	}

	Vec4 texture( SamplerCubeArray const & sampler, Vec4 const & value, Vec4 const & lod )
	{
		return findWriter( sampler, value, lod )->texture( sampler, value, lod );
	}

	Vec4 textureOffset( Sampler1DArray const & sampler, Vec2 const & value, Int const offset )
	{
		return findWriter( sampler, value, offset )->textureOffset( sampler, value, offset );
	}

	Vec4 textureOffset( Sampler1DArray const & sampler, Vec2 const & value, Int const offset, Vec4 const & lod )
	{
		return findWriter( sampler, value, offset, lod )->textureOffset( sampler, value, offset, lod );
	}

	Vec4 textureOffset( Sampler2DArray const & sampler, Vec3 const & value, IVec2 const offset )
	{
		return findWriter( sampler, value, offset )->textureOffset( sampler, value, offset );
	}

	Vec4 textureOffset( Sampler2DArray const & sampler, Vec3 const & value, IVec2 const offset, Vec4 const & lod )
	{
		return findWriter( sampler, value, offset, lod )->textureOffset( sampler, value, offset, lod );
	}

	Vec4 textureLodOffset( Sampler1DArray const & sampler, Vec2 const & value, Vec4 const & lod, Int const offset )
	{
		return findWriter( sampler, value, offset, lod )->textureLodOffset( sampler, value, lod, offset );
	}

	Vec4 textureLodOffset( Sampler2DArray const & sampler, Vec3 const & value, Vec4 const & lod, IVec2 const offset )
	{
		return findWriter( sampler, value, offset, lod )->textureLodOffset( sampler, value, lod, offset );
	}

	Float texture( Sampler1DShadow const & sampler, Vec2 const & value )
	{
		return findWriter( sampler, value )->texture( sampler, value );
	}

	Float texture( Sampler1DShadow const & sampler, Vec2 const & value, Float const & lod )
	{
		return findWriter( sampler, value, lod )->texture( sampler, value, lod );
	}

	Float texture( Sampler2DShadow const & sampler, Vec3 const & value )
	{
		return findWriter( sampler, value )->texture( sampler, value );
	}

	Float texture( Sampler2DShadow const & sampler, Vec3 const & value, Float const & lod )
	{
		return findWriter( sampler, value, lod )->texture( sampler, value, lod );
	}

	Float texture( SamplerCubeShadow const & sampler, Vec4 const & value )
	{
		return findWriter( sampler, value )->texture( sampler, value );
	}

	Float texture( SamplerCubeShadow const & sampler, Vec4 const & value, Float const & lod )
	{
		return findWriter( sampler, value, lod )->texture( sampler, value, lod );
	}

	Float textureOffset( Sampler1DShadow const & sampler, Vec2 const & value, Int const offset )
	{
		return findWriter( sampler, value, offset )->textureOffset( sampler, value, offset );
	}

	Float textureOffset( Sampler1DShadow const & sampler, Vec2 const & value, Int const offset, Float const & lod )
	{
		return findWriter( sampler, value, offset, lod )->textureOffset( sampler, value, offset, lod );
	}

	Float textureOffset( Sampler2DShadow const & sampler, Vec3 const & value, IVec2 const offset )
	{
		return findWriter( sampler, value, offset )->textureOffset( sampler, value, offset );
	}

	Float textureOffset( Sampler2DShadow const & sampler, Vec3 const & value, IVec2 const offset, Float const & lod )
	{
		return findWriter( sampler, value, offset, lod )->textureOffset( sampler, value, offset, lod );
	}

	Float textureLodOffset( Sampler1DShadow const & sampler, Vec2 const & value, Float const & lod, Int const offset )
	{
		return findWriter( sampler, value, offset, lod )->textureLodOffset( sampler, value, lod, offset );
	}

	Float textureLodOffset( Sampler2DShadow const & sampler, Vec3 const & value, Float const & lod, IVec2 const offset )
	{
		return findWriter( sampler, value, offset, lod )->textureLodOffset( sampler, value, lod, offset );
	}

	Float texture( Sampler1DArrayShadow const & sampler, Vec3 const & value )
	{
		return findWriter( sampler, value )->texture( sampler, value );
	}

	Float texture( Sampler1DArrayShadow const & sampler, Vec3 const & value, Float const & lod )
	{
		return findWriter( sampler, value, lod )->texture( sampler, value, lod );
	}

	Float texture( Sampler2DArrayShadow const & sampler, Vec4 const & value )
	{
		return findWriter( sampler, value )->texture( sampler, value );
	}

	Float texture( Sampler2DArrayShadow const & sampler, Vec4 const & value, Float const & lod )
	{
		return findWriter( sampler, value, lod )->texture( sampler, value, lod );
	}

	Float texture( SamplerCubeArrayShadow const & sampler, Vec4 const & value, Float const & layer )
	{
		return findWriter( sampler, value, layer )->texture( sampler, value, layer );
	}

	Float texture( SamplerCubeArrayShadow const & sampler, Vec4 const & value, Float const & layer, Float const & lod )
	{
		return findWriter( sampler, value, layer, lod )->texture( sampler, value, layer, lod );
	}

	Float textureOffset( Sampler1DArrayShadow const & sampler, Vec3 const & value, Int const offset )
	{
		return findWriter( sampler, value, offset )->textureOffset( sampler, value, offset );
	}

	Float textureOffset( Sampler1DArrayShadow const & sampler, Vec3 const & value, Int const offset, Float const & lod )
	{
		return findWriter( sampler, value, offset, lod )->textureOffset( sampler, value, offset, lod );
	}

	Float textureOffset( Sampler2DArrayShadow const & sampler, Vec4 const & value, IVec2 const offset )
	{
		return findWriter( sampler, value, offset )->textureOffset( sampler, value, offset );
	}

	Float textureOffset( Sampler2DArrayShadow const & sampler, Vec4 const & value, IVec2 const offset, Float const & lod )
	{
		return findWriter( sampler, value, offset, lod )->textureOffset( sampler, value, offset, lod );
	}

	Float textureLodOffset( Sampler1DArrayShadow const & sampler, Vec3 const & value, Float const & lod, Int const offset )
	{
		return findWriter( sampler, value, offset, lod )->textureLodOffset( sampler, value, lod, offset );
	}

	Float textureLodOffset( Sampler2DArrayShadow const & sampler, Vec4 const & value, Float const & lod, IVec2 const offset )
	{
		return findWriter( sampler, value, offset, lod )->textureLodOffset( sampler, value, lod, offset );
	}

	Vec2 reflect( Vec2 const & incident, Vec2 const & normal )
	{
		return writeFunctionCall< Vec2 >( findWriter( incident, normal ), cuT( "reflect" ), incident, normal );
	}

	Vec3 reflect( Vec3 const & incident, Vec3 const & normal )
	{
		return writeFunctionCall< Vec3 >( findWriter( incident, normal ), cuT( "reflect" ), incident, normal );
	}

	Vec4 reflect( Vec4 const & incident, Vec4 const & normal )
	{
		return writeFunctionCall< Vec4 >( findWriter( incident, normal ), cuT( "reflect" ), incident, normal );
	}

	Vec2 refract( Vec2 const & incident, Vec2 const & normal, Float const & ratio )
	{
		return writeFunctionCall< Vec2 >( findWriter( incident, normal, ratio ), cuT( "refract" ), incident, normal, ratio );
	}

	Vec3 refract( Vec3 const & incident, Vec3 const & normal, Float const & ratio )
	{
		return writeFunctionCall< Vec3 >( findWriter( incident, normal, ratio ), cuT( "refract" ), incident, normal, ratio );
	}

	Vec4 refract( Vec4 const & incident, Vec4 const & normal, Float const & ratio )
	{
		return writeFunctionCall< Vec4 >( findWriter( incident, normal, ratio ), cuT( "refract" ), incident, normal, ratio );
	}

	Float length( Type const & value )
	{
		return writeFunctionCall< Float >( value.m_writer, cuT( "length" ), value );
	}

	Float distance( Type const & lhs, Type const & rhs )
	{
		return writeFunctionCall< Float >( findWriter( lhs, rhs ), cuT( "distance" ), lhs, rhs );
	}

	Float radians( Type const & value )
	{
		return writeFunctionCall< Float >( value.m_writer, cuT( "radians" ), value );
	}

	Float cos( Type const & value )
	{
		return writeFunctionCall< Float >( value.m_writer, cuT( "cos" ), value );
	}

	Float sin( Type const & value )
	{
		return writeFunctionCall< Float >( value.m_writer, cuT( "sin" ), value );
	}

	Float tan( Type const & value )
	{
		return writeFunctionCall< Float >( value.m_writer, cuT( "tan" ), value );
	}

	Optional< Int > textureSize( Optional< Sampler1D > const & sampler, Int const lod )
	{
		return writeFunctionCall< Int >( findWriter( sampler, lod ), cuT( "textureSize" ), sampler, lod );
	}

	Optional< IVec2 > textureSize( Optional< Sampler2D > const & sampler, Int const & lod )
	{
		return writeFunctionCall< IVec2 >( findWriter( sampler, lod ), cuT( "textureSize" ), sampler, lod );
	}

	Optional< IVec3 > textureSize( Optional< Sampler3D > const & sampler, Int const & lod )
	{
		return writeFunctionCall< IVec3 >( findWriter( sampler, lod ), cuT( "textureSize" ), sampler, lod );
	}

	Optional< IVec2 > textureSize( Optional< SamplerCube > const & sampler, Int const & lod )
	{
		return writeFunctionCall< IVec2 >( findWriter( sampler, lod ), cuT( "textureSize" ), sampler, lod );
	}

	Optional< IVec2 > textureSize( Optional< Sampler1DArray > const & sampler, Int const lod )
	{
		return writeFunctionCall< IVec2 >( findWriter( sampler, lod ), cuT( "textureSize" ), sampler, lod );
	}

	Optional< IVec3 > textureSize( Optional< Sampler2DArray > const & sampler, Int const & lod )
	{
		return writeFunctionCall< IVec3 >( findWriter( sampler, lod ), cuT( "textureSize" ), sampler, lod );
	}

	Optional< IVec3 > textureSize( Optional< SamplerCubeArray > const & sampler, Int const & lod )
	{
		return writeFunctionCall< IVec3 >( findWriter( sampler, lod ), cuT( "textureSize" ), sampler, lod );
	}

	Optional< Int > textureSize( Optional< Sampler1DShadow > const & sampler, Int const lod )
	{
		return writeFunctionCall< Int >( findWriter( sampler, lod ), cuT( "textureSize" ), sampler, lod );
	}

	Optional< IVec2 > textureSize( Optional< Sampler2DShadow > const & sampler, Int const & lod )
	{
		return writeFunctionCall< IVec2 >( findWriter( sampler, lod ), cuT( "textureSize" ), sampler, lod );
	}

	Optional< IVec2 > textureSize( Optional< SamplerCubeShadow > const & sampler, Int const & lod )
	{
		return writeFunctionCall< IVec2 >( findWriter( sampler, lod ), cuT( "textureSize" ), sampler, lod );
	}

	Optional< IVec2 > textureSize( Optional< Sampler1DArrayShadow > const & sampler, Int const lod )
	{
		return writeFunctionCall< IVec2 >( findWriter( sampler, lod ), cuT( "textureSize" ), sampler, lod );
	}

	Optional< IVec3 > textureSize( Optional< Sampler2DArrayShadow > const & sampler, Int const & lod )
	{
		return writeFunctionCall< IVec3 >( findWriter( sampler, lod ), cuT( "textureSize" ), sampler, lod );
	}

	Optional< IVec3 > textureSize( Optional< SamplerCubeArrayShadow > const & sampler, Int const & lod )
	{
		return writeFunctionCall< IVec3 >( findWriter( sampler, lod ), cuT( "textureSize" ), sampler, lod );
	}

	Optional< Vec4 > texture( Optional< Sampler1D > const & sampler, Float const & value )
	{
		return findWriter( sampler, value )->texture( sampler, value );
	}

	Optional< Vec4 > texture( Optional< Sampler1D > const & sampler, Float const & value, Float const & lod )
	{
		return findWriter( sampler, value, lod )->texture( sampler, value, lod );
	}

	Optional< Vec4 > texture( Optional< Sampler2D > const & sampler, Vec2 const & value )
	{
		return findWriter( sampler, value )->texture( sampler, value );
	}

	Optional< Vec4 > texture( Optional< Sampler2D > const & sampler, Vec2 const & value, Float const & lod )
	{
		return findWriter( sampler, value, lod )->texture( sampler, value, lod );
	}

	Optional< Vec4 > texture( Optional< Sampler3D > const & sampler, Vec3 const & value )
	{
		return findWriter( sampler, value )->texture( sampler, value );
	}

	Optional< Vec4 > texture( Optional< Sampler3D > const & sampler, Vec3 const & value, Float const & lod )
	{
		return findWriter( sampler, value, lod )->texture( sampler, value, lod );
	}

	Optional< Vec4 > texture( Optional< SamplerCube > const & sampler, Vec3 const & value )
	{
		return findWriter( sampler, value )->texture( sampler, value );
	}

	Optional< Vec4 > texture( Optional< SamplerCube > const & sampler, Vec3 const & value, Float const & lod )
	{
		return findWriter( sampler, value, lod )->texture( sampler, value, lod );
	}

	Optional< Vec4 > textureOffset( Optional< Sampler1D > const & sampler, Float const & value, Int const & offset )
	{
		return findWriter( sampler, value, offset )->textureOffset( sampler, value, offset );
	}

	Optional< Vec4 > textureOffset( Optional< Sampler1D > const & sampler, Float const & value, Int const & offset, Float const & lod )
	{
		return findWriter( sampler, value, offset, lod )->textureOffset( sampler, value, offset, lod );
	}

	Optional< Vec4 > textureOffset( Optional< Sampler2D > const & sampler, Vec2 const & value, IVec2 const & offset )
	{
		return findWriter( sampler, value, offset )->textureOffset( sampler, value, offset );
	}

	Optional< Vec4 > textureOffset( Optional< Sampler2D > const & sampler, Vec2 const & value, IVec2 const & offset, Float const & lod )
	{
		return findWriter( sampler, value, offset, lod )->textureOffset( sampler, value, offset, lod );
	}

	Optional< Vec4 > textureOffset( Optional< Sampler3D > const & sampler, Vec3 const & value, IVec3 const & offset )
	{
		return findWriter( sampler, value, offset )->textureOffset( sampler, value, offset );
	}

	Optional< Vec4 > textureOffset( Optional< Sampler3D > const & sampler, Vec3 const & value, IVec3 const & offset, Float const & lod )
	{
		return findWriter( sampler, value, offset, lod )->textureOffset( sampler, value, offset, lod );
	}

	Optional< Vec4 > textureLodOffset( Optional< Sampler1D > const & sampler, Float const & value, Float const & lod, Int const offset )
	{
		return findWriter( sampler, value, offset, lod )->textureLodOffset( sampler, value, lod, offset );
	}

	Optional< Vec4 > textureLodOffset( Optional< Sampler2D > const & sampler, Vec2 const & value, Float const & lod, IVec2 const offset )
	{
		return findWriter( sampler, value, offset, lod )->textureLodOffset( sampler, value, lod, offset );
	}

	Optional< Vec4 > textureLodOffset( Optional< Sampler3D > const & sampler, Vec3 const & value, Float const & lod, IVec3 const offset )
	{
		return findWriter( sampler, value, offset, lod )->textureLodOffset( sampler, value, lod, offset );
	}

	Optional< Vec4 > texelFetch( Optional< SamplerBuffer > const & sampler, Type const & value )
	{
		return findWriter( sampler, value )->texelFetch( sampler, value );
	}

	Optional< Vec4 > texelFetch( Optional< Sampler1D > const & sampler, Type const & value, Int const & modif )
	{
		return findWriter( sampler, value, modif )->texelFetch( sampler, value, modif );
	}

	Optional< Vec4 > texelFetch( Optional< Sampler2D > const & sampler, Type const & value, Int const & modif )
	{
		return findWriter( sampler, value, modif )->texelFetch( sampler, value, modif );
	}

	Optional< Vec4 > texelFetch( Optional< Sampler3D > const & sampler, Type const & value, Int const & modif )
	{
		return findWriter( sampler, value, modif )->texelFetch( sampler, value, modif );
	}

	Optional< Vec4 > texture( Optional< Sampler1DArray > const & sampler, Vec2 const & value )
	{
		return findWriter( sampler, value )->texture( sampler, value );
	}

	Optional< Vec4 > texture( Optional< Sampler1DArray > const & sampler, Vec2 const & value, Vec4 const & lod )
	{
		return findWriter( sampler, value, lod )->texture( sampler, value, lod );
	}

	Optional< Vec4 > texture( Optional< Sampler2DArray > const & sampler, Vec3 const & value )
	{
		return findWriter( sampler, value )->texture( sampler, value );
	}

	Optional< Vec4 > texture( Optional< Sampler2DArray > const & sampler, Vec3 const & value, Vec4 const & lod )
	{
		return findWriter( sampler, value, lod )->texture( sampler, value, lod );
	}

	Optional< Vec4 > texture( Optional< SamplerCubeArray > const & sampler, Vec4 const & value )
	{
		return findWriter( sampler, value )->texture( sampler, value );
	}

	Optional< Vec4 > texture( Optional< SamplerCubeArray > const & sampler, Vec4 const & value, Vec4 const & lod )
	{
		return findWriter( sampler, value, lod )->texture( sampler, value, lod );
	}

	Optional< Vec4 > textureOffset( Optional< Sampler1DArray > const & sampler, Vec2 const & value, Int const offset )
	{
		return findWriter( sampler, value, offset )->textureOffset( sampler, value, offset );
	}

	Optional< Vec4 > textureOffset( Optional< Sampler1DArray > const & sampler, Vec2 const & value, Int const offset, Vec4 const & lod )
	{
		return findWriter( sampler, value, offset, lod )->textureOffset( sampler, value, offset, lod );
	}

	Optional< Vec4 > textureOffset( Optional< Sampler2DArray > const & sampler, Vec3 const & value, IVec2 const offset )
	{
		return findWriter( sampler, value, offset )->textureOffset( sampler, value, offset );
	}

	Optional< Vec4 > textureOffset( Optional< Sampler2DArray > const & sampler, Vec3 const & value, IVec2 const offset, Vec4 const & lod )
	{
		return findWriter( sampler, value, offset, lod )->textureOffset( sampler, value, offset, lod );
	}

	Optional< Vec4 > textureLodOffset( Optional< Sampler1DArray > const & sampler, Vec2 const & value, Vec4 const & lod, Int const offset )
	{
		return findWriter( sampler, value, offset, lod )->textureLodOffset( sampler, value, lod, offset );
	}

	Optional< Vec4 > textureLodOffset( Optional< Sampler2DArray > const & sampler, Vec3 const & value, Vec4 const & lod, IVec2 const offset )
	{
		return findWriter( sampler, value, offset, lod )->textureLodOffset( sampler, value, lod, offset );
	}

	Optional< Float > texture( Optional< Sampler1DShadow > const & sampler, Vec2 const & value )
	{
		return findWriter( sampler, value )->texture( sampler, value );
	}

	Optional< Float > texture( Optional< Sampler1DShadow > const & sampler, Vec2 const & value, Float const & lod )
	{
		return findWriter( sampler, value, lod )->texture( sampler, value, lod );
	}

	Optional< Float > texture( Optional< Sampler2DShadow > const & sampler, Vec3 const & value )
	{
		return findWriter( sampler, value )->texture( sampler, value );
	}

	Optional< Float > texture( Optional< Sampler2DShadow > const & sampler, Vec3 const & value, Float const & lod )
	{
		return findWriter( sampler, value, lod )->texture( sampler, value, lod );
	}

	Optional< Float > texture( Optional< SamplerCubeShadow > const & sampler, Vec4 const & value )
	{
		return findWriter( sampler, value )->texture( sampler, value );
	}

	Optional< Float > texture( Optional< SamplerCubeShadow > const & sampler, Vec4 const & value, Float const & lod )
	{
		return findWriter( sampler, value, lod )->texture( sampler, value, lod );
	}

	Optional< Float > textureOffset( Optional< Sampler1DShadow > const & sampler, Vec2 const & value, Int const offset )
	{
		return findWriter( sampler, value, offset )->textureOffset( sampler, value, offset );
	}

	Optional< Float > textureOffset( Optional< Sampler1DShadow > const & sampler, Vec2 const & value, Int const offset, Float const & lod )
	{
		return findWriter( sampler, value, offset, lod )->textureOffset( sampler, value, offset, lod );
	}

	Optional< Float > textureOffset( Optional< Sampler2DShadow > const & sampler, Vec3 const & value, IVec2 const offset )
	{
		return findWriter( sampler, value, offset )->textureOffset( sampler, value, offset );
	}

	Optional< Float > textureOffset( Optional< Sampler2DShadow > const & sampler, Vec3 const & value, IVec2 const offset, Float const & lod )
	{
		return findWriter( sampler, value, offset, lod )->textureOffset( sampler, value, offset, lod );
	}

	Optional< Float > textureLodOffset( Optional< Sampler1DShadow > const & sampler, Vec2 const & value, Float const & lod, Int const offset )
	{
		return findWriter( sampler, value, offset, lod )->textureLodOffset( sampler, value, lod, offset );
	}

	Optional< Float > textureLodOffset( Optional< Sampler2DShadow > const & sampler, Vec3 const & value, Float const & lod, IVec2 const offset )
	{
		return findWriter( sampler, value, offset, lod )->textureLodOffset( sampler, value, lod, offset );
	}

	Optional< Float > texture( Optional< Sampler1DArrayShadow > const & sampler, Vec3 const & value )
	{
		return findWriter( sampler, value )->texture( sampler, value );
	}

	Optional< Float > texture( Optional< Sampler1DArrayShadow > const & sampler, Vec3 const & value, Float const & lod )
	{
		return findWriter( sampler, value, lod )->texture( sampler, value, lod );
	}

	Optional< Float > texture( Optional< Sampler2DArrayShadow > const & sampler, Vec4 const & value )
	{
		return findWriter( sampler, value )->texture( sampler, value );
	}

	Optional< Float > texture( Optional< Sampler2DArrayShadow > const & sampler, Vec4 const & value, Float const & lod )
	{
		return findWriter( sampler, value, lod )->texture( sampler, value, lod );
	}

	Optional< Float > texture( Optional< SamplerCubeArrayShadow > const & sampler, Vec4 const & value, Float const & layer )
	{
		return findWriter( sampler, value, layer )->texture( sampler, value, layer );
	}

	Optional< Float > texture( Optional< SamplerCubeArrayShadow > const & sampler, Vec4 const & value, Float const & layer, Float const & lod )
	{
		return findWriter( sampler, value, layer, lod )->texture( sampler, value, layer, lod );
	}

	Optional< Float > textureOffset( Optional< Sampler1DArrayShadow > const & sampler, Vec3 const & value, Int const offset )
	{
		return findWriter( sampler, value, offset )->textureOffset( sampler, value, offset );
	}

	Optional< Float > textureOffset( Optional< Sampler1DArrayShadow > const & sampler, Vec3 const & value, Int const offset, Float const & lod )
	{
		return findWriter( sampler, value, offset, lod )->textureOffset( sampler, value, offset, lod );
	}

	Optional< Float > textureOffset( Optional< Sampler2DArrayShadow > const & sampler, Vec4 const & value, IVec2 const offset )
	{
		return findWriter( sampler, value, offset )->textureOffset( sampler, value, offset );
	}

	Optional< Float > textureOffset( Optional< Sampler2DArrayShadow > const & sampler, Vec4 const & value, IVec2 const offset, Float const & lod )
	{
		return findWriter( sampler, value, offset, lod )->textureOffset( sampler, value, offset, lod );
	}

	Optional< Float > textureLodOffset( Optional< Sampler1DArrayShadow > const & sampler, Vec3 const & value, Float const & lod, Int const offset )
	{
		return findWriter( sampler, value, offset, lod )->textureLodOffset( sampler, value, lod, offset );
	}

	Optional< Float > textureLodOffset( Optional< Sampler2DArrayShadow > const & sampler, Vec4 const & value, Float const & lod, IVec2 const offset )
	{
		return findWriter( sampler, value, offset, lod )->textureLodOffset( sampler, value, lod, offset );
	}

	Optional< Vec2 > reflect( Optional< Vec2 > const & lhs, Type const & rhs )
	{
		return writeFunctionCall< Vec2 >( findWriter( lhs, rhs ), cuT( "reflect" ), lhs, rhs );
	}

	Optional< Vec3 > reflect( Optional< Vec3 > const & lhs, Type const & rhs )
	{
		return writeFunctionCall< Vec3 >( findWriter( lhs, rhs ), cuT( "reflect" ), lhs, rhs );
	}

	Optional< Vec4 > reflect( Optional< Vec4 > const & lhs, Type const & rhs )
	{
		return writeFunctionCall< Vec4 >( findWriter( lhs, rhs ), cuT( "reflect" ), lhs, rhs );
	}

	Optional< Float > length( Optional< Type > const & value )
	{
		return writeFunctionCall< Float >( value.m_writer, cuT( "length" ), value );
	}

	Optional< Float > radians( Optional< Type > const & value )
	{
		return writeFunctionCall< Float >( value.m_writer, cuT( "radians" ), value );
	}

	Optional< Float > cos( Optional< Type > const & value )
	{
		return writeFunctionCall< Float >( value.m_writer, cuT( "cos" ), value );
	}

	Optional< Float > sin( Optional< Type > const & value )
	{
		return writeFunctionCall< Float >( value.m_writer, cuT( "sin" ), value );
	}

	Optional< Float > tan( Optional< Type > const & value )
	{
		return writeFunctionCall< Float >( value.m_writer, cuT( "tan" ), value );
	}
}
