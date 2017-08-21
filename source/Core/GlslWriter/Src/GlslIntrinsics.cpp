#include "GlslIntrinsics.hpp"

using namespace castor;

namespace glsl
{
	Vec4 operator*( Vec4 const & p_a, Mat4 const & p_b )
	{
		Vec4 result( p_a.m_writer );
		result.m_value << String( p_a ) << cuT( " * " ) << String( p_b );
		return result;
	}

	Vec4 operator*( Mat4 const & p_a, Vec4 const & p_b )
	{
		Vec4 result( p_a.m_writer );
		result.m_value << String( p_a ) << cuT( " * " ) << String( p_b );
		return result;
	}

	Vec3 operator*( Mat3 const & p_a, Vec3 const & p_b )
	{
		Vec3 result( p_a.m_writer );
		result.m_value << String( p_a ) << cuT( " * " ) << String( p_b );
		return result;
	}

	Float operator+( float p_a, Float const & p_b )
	{
		Float result( p_b.m_writer );
		result.m_value << toString( p_a ) << cuT( " + " ) << String( p_b );
		return result;
	}

	Float operator-( float p_a, Float const & p_b )
	{
		Float result( p_b.m_writer );
		result.m_value << toString( p_a ) << cuT( " - " ) << String( p_b );
		return result;
	}

	Float operator*( float p_a, Float const & p_b )
	{
		Float result( p_b.m_writer );
		result.m_value << toString( p_a ) << cuT( " * " ) << String( p_b );
		return result;
	}

	Float operator/( float p_a, Float const & p_b )
	{
		Float result( p_b.m_writer );
		result.m_value << toString( p_a ) << cuT( " / " ) << String( p_b );
		return result;
	}

	Int operator+( int p_a, Int const & p_b )
	{
		Int result( p_b.m_writer );
		result.m_value << toString( p_a ) << cuT( " + " ) << String( p_b );
		return result;
	}

	Int operator-( int p_a, Int const & p_b )
	{
		Int result( p_b.m_writer );
		result.m_value << toString( p_a ) << cuT( " - " ) << String( p_b );
		return result;
	}

	Int operator*( int p_a, Int const & p_b )
	{
		Int result( p_b.m_writer );
		result.m_value << toString( p_a ) << cuT( " * " ) << String( p_b );
		return result;
	}

	Int operator/( int p_a, Int const & p_b )
	{
		Int result( p_b.m_writer );
		result.m_value << toString( p_a ) << cuT( " / " ) << String( p_b );
		return result;
	}

	Int operator%( Int const & p_a, int p_b )
	{
		Int result( p_a.m_writer );
		result.m_value << String( p_a ) << cuT( " % " ) << toString( p_b );
		return result;
	}

	Int operator%( int p_a, Int const & p_b )
	{
		Int result( p_b.m_writer );
		result.m_value << toString( p_a ) << cuT( " % " ) << String( p_b );
		return result;
	}

	Int operator%( Int const & p_a, Int const & p_b )
	{
		Int result( p_a.m_writer );
		result.m_value << String( p_a ) << cuT( " % " ) << String( p_b );
		return result;
	}

	Int textureSize( Sampler1D const & p_sampler, Int const p_lod )
	{
		return writeFunctionCall< Int >( p_sampler.m_writer, cuT( "textureSize" ), p_sampler, p_lod );
	}

	IVec2 textureSize( Sampler2D const & p_sampler, Int const & p_lod )
	{
		return writeFunctionCall< IVec2 >( p_sampler.m_writer, cuT( "textureSize" ), p_sampler, p_lod );
	}

	IVec3 textureSize( Sampler3D const & p_sampler, Int const & p_lod )
	{
		return writeFunctionCall< IVec3 >( p_sampler.m_writer, cuT( "textureSize" ), p_sampler, p_lod );
	}

	IVec2 textureSize( SamplerCube const & p_sampler, Int const & p_lod )
	{
		return writeFunctionCall< IVec2 >( p_sampler.m_writer, cuT( "textureSize" ), p_sampler, p_lod );
	}

	IVec2 textureSize( Sampler1DArray const & p_sampler, Int const p_lod )
	{
		return writeFunctionCall< IVec2 >( p_sampler.m_writer, cuT( "textureSize" ), p_sampler, p_lod );
	}

	IVec3 textureSize( Sampler2DArray const & p_sampler, Int const & p_lod )
	{
		return writeFunctionCall< IVec3 >( p_sampler.m_writer, cuT( "textureSize" ), p_sampler, p_lod );
	}

	IVec3 textureSize( SamplerCubeArray const & p_sampler, Int const & p_lod )
	{
		return writeFunctionCall< IVec3 >( p_sampler.m_writer, cuT( "textureSize" ), p_sampler, p_lod );
	}

	Int textureSize( Sampler1DShadow const & p_sampler, Int const p_lod )
	{
		return writeFunctionCall< Int >( p_sampler.m_writer, cuT( "textureSize" ), p_sampler, p_lod );
	}

	IVec2 textureSize( Sampler2DShadow const & p_sampler, Int const & p_lod )
	{
		return writeFunctionCall< IVec2 >( p_sampler.m_writer, cuT( "textureSize" ), p_sampler, p_lod );
	}

	IVec2 textureSize( SamplerCubeShadow const & p_sampler, Int const & p_lod )
	{
		return writeFunctionCall< IVec2 >( p_sampler.m_writer, cuT( "textureSize" ), p_sampler, p_lod );
	}

	IVec2 textureSize( Sampler1DArrayShadow const & p_sampler, Int const p_lod )
	{
		return writeFunctionCall< IVec2 >( p_sampler.m_writer, cuT( "textureSize" ), p_sampler, p_lod );
	}

	IVec3 textureSize( Sampler2DArrayShadow const & p_sampler, Int const & p_lod )
	{
		return writeFunctionCall< IVec3 >( p_sampler.m_writer, cuT( "textureSize" ), p_sampler, p_lod );
	}

	IVec3 textureSize( SamplerCubeArrayShadow const & p_sampler, Int const & p_lod )
	{
		return writeFunctionCall< IVec3 >( p_sampler.m_writer, cuT( "textureSize" ), p_sampler, p_lod );
	}

	Vec4 texture( Sampler1D const & p_sampler, Float const & p_value )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value );
	}

	Vec4 texture( Sampler1D const & p_sampler, Float const & p_value, Float const & p_lod )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value, p_lod );
	}

	Vec4 texture( Sampler2D const & p_sampler, Vec2 const & p_value )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value );
	}

	Vec4 texture( Sampler2D const & p_sampler, Vec2 const & p_value, Float const & p_lod )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value, p_lod );
	}

	Vec4 texture( Sampler3D const & p_sampler, Vec3 const & p_value )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value );
	}

	Vec4 texture( Sampler3D const & p_sampler, Vec3 const & p_value, Float const & p_lod )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value, p_lod );
	}

	Vec4 texture( SamplerCube const & p_sampler, Vec3 const & p_value )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value );
	}

	Vec4 texture( SamplerCube const & p_sampler, Vec3 const & p_value, Float const & p_lod )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value, p_lod );
	}

	Vec4 textureOffset( Sampler1D const & p_sampler, Float const & p_value, Int const p_offset )
	{
		return p_sampler.m_writer->textureOffset( p_sampler, p_value, p_offset );
	}

	Vec4 textureOffset( Sampler1D const & p_sampler, Float const & p_value, Int const p_offset, Float const & p_lod )
	{
		return p_sampler.m_writer->textureOffset( p_sampler, p_value, p_offset, p_lod );
	}

	Vec4 textureOffset( Sampler2D const & p_sampler, Vec2 const & p_value, IVec2 const p_offset )
	{
		return p_sampler.m_writer->textureOffset( p_sampler, p_value, p_offset );
	}

	Vec4 textureOffset( Sampler2D const & p_sampler, Vec2 const & p_value, IVec2 const p_offset, Float const & p_lod )
	{
		return p_sampler.m_writer->textureOffset( p_sampler, p_value, p_offset, p_lod );
	}

	Vec4 textureOffset( Sampler3D const & p_sampler, Vec3 const & p_value, IVec3 const p_offset )
	{
		return p_sampler.m_writer->textureOffset( p_sampler, p_value, p_offset );
	}

	Vec4 textureOffset( Sampler3D const & p_sampler, Vec3 const & p_value, IVec3 const p_offset, Float const & p_lod )
	{
		return p_sampler.m_writer->textureOffset( p_sampler, p_value, p_offset, p_lod );
	}

	Vec4 textureLodOffset( Sampler1D const & p_sampler, Float const & p_value, Float const & p_lod, Int const p_offset )
	{
		return p_sampler.m_writer->textureLodOffset( p_sampler, p_value, p_lod, p_offset );
	}

	Vec4 textureLodOffset( Sampler2D const & p_sampler, Vec2 const & p_value, Float const & p_lod, IVec2 const p_offset )
	{
		return p_sampler.m_writer->textureLodOffset( p_sampler, p_value, p_lod, p_offset );
	}

	Vec4 textureLodOffset( Sampler3D const & p_sampler, Vec3 const & p_value, Float const & p_lod, IVec3 const p_offset )
	{
		return p_sampler.m_writer->textureLodOffset( p_sampler, p_value, p_lod, p_offset );
	}

	Vec4 texelFetch( SamplerBuffer const & p_sampler, Type const & p_value )
	{
		return p_sampler.m_writer->texelFetch( p_sampler, p_value );
	}

	Vec4 texelFetch( Sampler1D const & p_sampler, Type const & p_value, Int const & p_modif )
	{
		return p_sampler.m_writer->texelFetch( p_sampler, p_value, p_modif );
	}

	Vec4 texelFetch( Sampler2D const & p_sampler, Type const & p_value, Int const & p_modif )
	{
		return p_sampler.m_writer->texelFetch( p_sampler, p_value, p_modif );
	}

	Vec4 texelFetch( Sampler3D const & p_sampler, Type const & p_value, Int const & p_modif )
	{
		return p_sampler.m_writer->texelFetch( p_sampler, p_value, p_modif );
	}

	Vec4 texture( Sampler1DArray const & p_sampler, Vec2 const & p_value )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value );
	}

	Vec4 texture( Sampler1DArray const & p_sampler, Vec2 const & p_value, Vec4 const & p_lod )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value, p_lod );
	}

	Vec4 texture( Sampler2DArray const & p_sampler, Vec3 const & p_value )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value );
	}

	Vec4 texture( Sampler2DArray const & p_sampler, Vec3 const & p_value, Vec4 const & p_lod )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value, p_lod );
	}

	Vec4 texture( SamplerCubeArray const & p_sampler, Vec4 const & p_value )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value );
	}

	Vec4 texture( SamplerCubeArray const & p_sampler, Vec4 const & p_value, Vec4 const & p_lod )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value, p_lod );
	}

	Vec4 textureOffset( Sampler1DArray const & p_sampler, Vec2 const & p_value, Int const p_offset )
	{
		return p_sampler.m_writer->textureOffset( p_sampler, p_value, p_offset );
	}

	Vec4 textureOffset( Sampler1DArray const & p_sampler, Vec2 const & p_value, Int const p_offset, Vec4 const & p_lod )
	{
		return p_sampler.m_writer->textureOffset( p_sampler, p_value, p_offset, p_lod );
	}

	Vec4 textureOffset( Sampler2DArray const & p_sampler, Vec3 const & p_value, IVec2 const p_offset )
	{
		return p_sampler.m_writer->textureOffset( p_sampler, p_value, p_offset );
	}

	Vec4 textureOffset( Sampler2DArray const & p_sampler, Vec3 const & p_value, IVec2 const p_offset, Vec4 const & p_lod )
	{
		return p_sampler.m_writer->textureOffset( p_sampler, p_value, p_offset, p_lod );
	}

	Vec4 textureLodOffset( Sampler1DArray const & p_sampler, Vec2 const & p_value, Vec4 const & p_lod, Int const p_offset )
	{
		return p_sampler.m_writer->textureLodOffset( p_sampler, p_value, p_lod, p_offset );
	}

	Vec4 textureLodOffset( Sampler2DArray const & p_sampler, Vec3 const & p_value, Vec4 const & p_lod, IVec2 const p_offset )
	{
		return p_sampler.m_writer->textureLodOffset( p_sampler, p_value, p_lod, p_offset );
	}

	Float texture( Sampler1DShadow const & p_sampler, Vec2 const & p_value )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value );
	}

	Float texture( Sampler1DShadow const & p_sampler, Vec2 const & p_value, Float const & p_lod )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value, p_lod );
	}

	Float texture( Sampler2DShadow const & p_sampler, Vec3 const & p_value )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value );
	}

	Float texture( Sampler2DShadow const & p_sampler, Vec3 const & p_value, Float const & p_lod )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value, p_lod );
	}

	Float texture( SamplerCubeShadow const & p_sampler, Vec4 const & p_value )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value );
	}

	Float texture( SamplerCubeShadow const & p_sampler, Vec4 const & p_value, Float const & p_lod )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value, p_lod );
	}

	Float textureOffset( Sampler1DShadow const & p_sampler, Vec2 const & p_value, Int const p_offset )
	{
		return p_sampler.m_writer->textureOffset( p_sampler, p_value, p_offset );
	}

	Float textureOffset( Sampler1DShadow const & p_sampler, Vec2 const & p_value, Int const p_offset, Float const & p_lod )
	{
		return p_sampler.m_writer->textureOffset( p_sampler, p_value, p_offset, p_lod );
	}

	Float textureOffset( Sampler2DShadow const & p_sampler, Vec3 const & p_value, IVec2 const p_offset )
	{
		return p_sampler.m_writer->textureOffset( p_sampler, p_value, p_offset );
	}

	Float textureOffset( Sampler2DShadow const & p_sampler, Vec3 const & p_value, IVec2 const p_offset, Float const & p_lod )
	{
		return p_sampler.m_writer->textureOffset( p_sampler, p_value, p_offset, p_lod );
	}

	Float textureLodOffset( Sampler1DShadow const & p_sampler, Vec2 const & p_value, Float const & p_lod, Int const p_offset )
	{
		return p_sampler.m_writer->textureLodOffset( p_sampler, p_value, p_lod, p_offset );
	}

	Float textureLodOffset( Sampler2DShadow const & p_sampler, Vec3 const & p_value, Float const & p_lod, IVec2 const p_offset )
	{
		return p_sampler.m_writer->textureLodOffset( p_sampler, p_value, p_lod, p_offset );
	}

	Float texture( Sampler1DArrayShadow const & p_sampler, Vec3 const & p_value )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value );
	}

	Float texture( Sampler1DArrayShadow const & p_sampler, Vec3 const & p_value, Float const & p_lod )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value, p_lod );
	}

	Float texture( Sampler2DArrayShadow const & p_sampler, Vec4 const & p_value )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value );
	}

	Float texture( Sampler2DArrayShadow const & p_sampler, Vec4 const & p_value, Float const & p_lod )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value, p_lod );
	}

	Float texture( SamplerCubeArrayShadow const & p_sampler, Vec4 const & p_value, Float const & p_layer )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value, p_layer );
	}

	Float texture( SamplerCubeArrayShadow const & p_sampler, Vec4 const & p_value, Float const & p_layer, Float const & p_lod )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value, p_layer, p_lod );
	}

	Float textureOffset( Sampler1DArrayShadow const & p_sampler, Vec3 const & p_value, Int const p_offset )
	{
		return p_sampler.m_writer->textureOffset( p_sampler, p_value, p_offset );
	}

	Float textureOffset( Sampler1DArrayShadow const & p_sampler, Vec3 const & p_value, Int const p_offset, Float const & p_lod )
	{
		return p_sampler.m_writer->textureOffset( p_sampler, p_value, p_offset, p_lod );
	}

	Float textureOffset( Sampler2DArrayShadow const & p_sampler, Vec4 const & p_value, IVec2 const p_offset )
	{
		return p_sampler.m_writer->textureOffset( p_sampler, p_value, p_offset );
	}

	Float textureOffset( Sampler2DArrayShadow const & p_sampler, Vec4 const & p_value, IVec2 const p_offset, Float const & p_lod )
	{
		return p_sampler.m_writer->textureOffset( p_sampler, p_value, p_offset, p_lod );
	}

	Float textureLodOffset( Sampler1DArrayShadow const & p_sampler, Vec3 const & p_value, Float const & p_lod, Int const p_offset )
	{
		return p_sampler.m_writer->textureLodOffset( p_sampler, p_value, p_lod, p_offset );
	}

	Float textureLodOffset( Sampler2DArrayShadow const & p_sampler, Vec4 const & p_value, Float const & p_lod, IVec2 const p_offset )
	{
		return p_sampler.m_writer->textureLodOffset( p_sampler, p_value, p_lod, p_offset );
	}

	Vec2 reflect( Vec2 const & p_a, Vec2 const & p_b )
	{
		return writeFunctionCall< Vec2 >( p_a.m_writer, cuT( "reflect" ), p_a, p_b );
	}

	Vec3 reflect( Vec3 const & p_a, Vec3 const & p_b )
	{
		return writeFunctionCall< Vec3 >( p_a.m_writer, cuT( "reflect" ), p_a, p_b );
	}

	Vec4 reflect( Vec4 const & p_a, Vec4 const & p_b )
	{
		return writeFunctionCall< Vec4 >( p_a.m_writer, cuT( "reflect" ), p_a, p_b );
	}

	Vec2 refract( Vec2 const & p_a, Vec2 const & p_b, Float const & p_r )
	{
		return writeFunctionCall< Vec2 >( p_a.m_writer, cuT( "refract" ), p_a, p_b, p_r );
	}

	Vec3 refract( Vec3 const & p_a, Vec3 const & p_b, Float const & p_r )
	{
		return writeFunctionCall< Vec3 >( p_a.m_writer, cuT( "refract" ), p_a, p_b, p_r );
	}

	Vec4 refract( Vec4 const & p_a, Vec4 const & p_b, Float const & p_r )
	{
		return writeFunctionCall< Vec4 >( p_a.m_writer, cuT( "refract" ), p_a, p_b, p_r );
	}

	Float length( Type const & p_value )
	{
		return writeFunctionCall< Float >( p_value.m_writer, cuT( "length" ), p_value );
	}

	Float distance( Type const & p_a, Type const & p_b )
	{
		return writeFunctionCall< Float >( p_a.m_writer, cuT( "distance" ), p_a, p_b );
	}

	Float radians( Type const & p_value )
	{
		return writeFunctionCall< Float >( p_value.m_writer, cuT( "radians" ), p_value );
	}

	Float cos( Type const & p_value )
	{
		return writeFunctionCall< Float >( p_value.m_writer, cuT( "cos" ), p_value );
	}

	Float sin( Type const & p_value )
	{
		return writeFunctionCall< Float >( p_value.m_writer, cuT( "sin" ), p_value );
	}

	Float tan( Type const & p_value )
	{
		return writeFunctionCall< Float >( p_value.m_writer, cuT( "tan" ), p_value );
	}

	Float fract( Type const & p_value )
	{
		return writeFunctionCall< Float >( p_value.m_writer, cuT( "fract" ), p_value );
	}

	Optional< Int > textureSize( Optional< Sampler1D > const & p_sampler, Int const p_lod )
	{
		return writeOptionalFunctionCall< Int >( p_sampler.m_writer, cuT( "textureSize" ), p_sampler, p_lod );
	}

	Optional< IVec2 > textureSize( Optional< Sampler2D > const & p_sampler, Int const & p_lod )
	{
		return writeOptionalFunctionCall< IVec2 >( p_sampler.m_writer, cuT( "textureSize" ), p_sampler, p_lod );
	}

	Optional< IVec3 > textureSize( Optional< Sampler3D > const & p_sampler, Int const & p_lod )
	{
		return writeOptionalFunctionCall< IVec3 >( p_sampler.m_writer, cuT( "textureSize" ), p_sampler, p_lod );
	}

	Optional< IVec2 > textureSize( Optional< SamplerCube > const & p_sampler, Int const & p_lod )
	{
		return writeOptionalFunctionCall< IVec2 >( p_sampler.m_writer, cuT( "textureSize" ), p_sampler, p_lod );
	}

	Optional< IVec2 > textureSize( Optional< Sampler1DArray > const & p_sampler, Int const p_lod )
	{
		return writeOptionalFunctionCall< IVec2 >( p_sampler.m_writer, cuT( "textureSize" ), p_sampler, p_lod );
	}

	Optional< IVec3 > textureSize( Optional< Sampler2DArray > const & p_sampler, Int const & p_lod )
	{
		return writeOptionalFunctionCall< IVec3 >( p_sampler.m_writer, cuT( "textureSize" ), p_sampler, p_lod );
	}

	Optional< IVec3 > textureSize( Optional< SamplerCubeArray > const & p_sampler, Int const & p_lod )
	{
		return writeOptionalFunctionCall< IVec3 >( p_sampler.m_writer, cuT( "textureSize" ), p_sampler, p_lod );
	}

	Optional< Int > textureSize( Optional< Sampler1DShadow > const & p_sampler, Int const p_lod )
	{
		return writeOptionalFunctionCall< Int >( p_sampler.m_writer, cuT( "textureSize" ), p_sampler, p_lod );
	}

	Optional< IVec2 > textureSize( Optional< Sampler2DShadow > const & p_sampler, Int const & p_lod )
	{
		return writeOptionalFunctionCall< IVec2 >( p_sampler.m_writer, cuT( "textureSize" ), p_sampler, p_lod );
	}

	Optional< IVec2 > textureSize( Optional< SamplerCubeShadow > const & p_sampler, Int const & p_lod )
	{
		return writeOptionalFunctionCall< IVec2 >( p_sampler.m_writer, cuT( "textureSize" ), p_sampler, p_lod );
	}

	Optional< IVec2 > textureSize( Optional< Sampler1DArrayShadow > const & p_sampler, Int const p_lod )
	{
		return writeOptionalFunctionCall< IVec2 >( p_sampler.m_writer, cuT( "textureSize" ), p_sampler, p_lod );
	}

	Optional< IVec3 > textureSize( Optional< Sampler2DArrayShadow > const & p_sampler, Int const & p_lod )
	{
		return writeOptionalFunctionCall< IVec3 >( p_sampler.m_writer, cuT( "textureSize" ), p_sampler, p_lod );
	}

	Optional< IVec3 > textureSize( Optional< SamplerCubeArrayShadow > const & p_sampler, Int const & p_lod )
	{
		return writeOptionalFunctionCall< IVec3 >( p_sampler.m_writer, cuT( "textureSize" ), p_sampler, p_lod );
	}

	Optional< Vec4 > texture( Optional< Sampler1D > const & p_sampler, Float const & p_value )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value );
	}

	Optional< Vec4 > texture( Optional< Sampler1D > const & p_sampler, Float const & p_value, Float const & p_lod )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value, p_lod );
	}

	Optional< Vec4 > texture( Optional< Sampler2D > const & p_sampler, Vec2 const & p_value )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value );
	}

	Optional< Vec4 > texture( Optional< Sampler2D > const & p_sampler, Vec2 const & p_value, Float const & p_lod )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value, p_lod );
	}

	Optional< Vec4 > texture( Optional< Sampler3D > const & p_sampler, Vec3 const & p_value )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value );
	}

	Optional< Vec4 > texture( Optional< Sampler3D > const & p_sampler, Vec3 const & p_value, Float const & p_lod )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value, p_lod );
	}

	Optional< Vec4 > texture( Optional< SamplerCube > const & p_sampler, Vec3 const & p_value )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value );
	}

	Optional< Vec4 > texture( Optional< SamplerCube > const & p_sampler, Vec3 const & p_value, Float const & p_lod )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value, p_lod );
	}

	Optional< Vec4 > textureOffset( Optional< Sampler1D > const & p_sampler, Float const & p_value, Int const & p_offset )
	{
		return p_sampler.m_writer->textureOffset( p_sampler, p_value, p_offset );
	}

	Optional< Vec4 > textureOffset( Optional< Sampler1D > const & p_sampler, Float const & p_value, Int const & p_offset, Float const & p_lod )
	{
		return p_sampler.m_writer->textureOffset( p_sampler, p_value, p_offset, p_lod );
	}

	Optional< Vec4 > textureOffset( Optional< Sampler2D > const & p_sampler, Vec2 const & p_value, IVec2 const & p_offset )
	{
		return p_sampler.m_writer->textureOffset( p_sampler, p_value, p_offset );
	}

	Optional< Vec4 > textureOffset( Optional< Sampler2D > const & p_sampler, Vec2 const & p_value, IVec2 const & p_offset, Float const & p_lod )
	{
		return p_sampler.m_writer->textureOffset( p_sampler, p_value, p_offset, p_lod );
	}

	Optional< Vec4 > textureOffset( Optional< Sampler3D > const & p_sampler, Vec3 const & p_value, IVec3 const & p_offset )
	{
		return p_sampler.m_writer->textureOffset( p_sampler, p_value, p_offset );
	}

	Optional< Vec4 > textureOffset( Optional< Sampler3D > const & p_sampler, Vec3 const & p_value, IVec3 const & p_offset, Float const & p_lod )
	{
		return p_sampler.m_writer->textureOffset( p_sampler, p_value, p_offset, p_lod );
	}

	Optional< Vec4 > textureLodOffset( Optional< Sampler1D > const & p_sampler, Float const & p_value, Float const & p_lod, Int const p_offset )
	{
		return p_sampler.m_writer->textureLodOffset( p_sampler, p_value, p_lod, p_offset );
	}

	Optional< Vec4 > textureLodOffset( Optional< Sampler2D > const & p_sampler, Vec2 const & p_value, Float const & p_lod, IVec2 const p_offset )
	{
		return p_sampler.m_writer->textureLodOffset( p_sampler, p_value, p_lod, p_offset );
	}

	Optional< Vec4 > textureLodOffset( Optional< Sampler3D > const & p_sampler, Vec3 const & p_value, Float const & p_lod, IVec3 const p_offset )
	{
		return p_sampler.m_writer->textureLodOffset( p_sampler, p_value, p_lod, p_offset );
	}

	Optional< Vec4 > texelFetch( Optional< SamplerBuffer > const & p_sampler, Type const & p_value )
	{
		return p_sampler.m_writer->texelFetch( p_sampler, p_value );
	}

	Optional< Vec4 > texelFetch( Optional< Sampler1D > const & p_sampler, Type const & p_value, Int const & p_modif )
	{
		return p_sampler.m_writer->texelFetch( p_sampler, p_value, p_modif );
	}

	Optional< Vec4 > texelFetch( Optional< Sampler2D > const & p_sampler, Type const & p_value, Int const & p_modif )
	{
		return p_sampler.m_writer->texelFetch( p_sampler, p_value, p_modif );
	}

	Optional< Vec4 > texelFetch( Optional< Sampler3D > const & p_sampler, Type const & p_value, Int const & p_modif )
	{
		return p_sampler.m_writer->texelFetch( p_sampler, p_value, p_modif );
	}

	Optional< Vec4 > texture( Optional< Sampler1DArray > const & p_sampler, Vec2 const & p_value )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value );
	}

	Optional< Vec4 > texture( Optional< Sampler1DArray > const & p_sampler, Vec2 const & p_value, Vec4 const & p_lod )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value, p_lod );
	}

	Optional< Vec4 > texture( Optional< Sampler2DArray > const & p_sampler, Vec3 const & p_value )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value );
	}

	Optional< Vec4 > texture( Optional< Sampler2DArray > const & p_sampler, Vec3 const & p_value, Vec4 const & p_lod )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value, p_lod );
	}

	Optional< Vec4 > texture( Optional< SamplerCubeArray > const & p_sampler, Vec4 const & p_value )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value );
	}

	Optional< Vec4 > texture( Optional< SamplerCubeArray > const & p_sampler, Vec4 const & p_value, Vec4 const & p_lod )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value, p_lod );
	}

	Optional< Vec4 > textureOffset( Optional< Sampler1DArray > const & p_sampler, Vec2 const & p_value, Int const p_offset )
	{
		return p_sampler.m_writer->textureOffset( p_sampler, p_value, p_offset );
	}

	Optional< Vec4 > textureOffset( Optional< Sampler1DArray > const & p_sampler, Vec2 const & p_value, Int const p_offset, Vec4 const & p_lod )
	{
		return p_sampler.m_writer->textureOffset( p_sampler, p_value, p_offset, p_lod );
	}

	Optional< Vec4 > textureOffset( Optional< Sampler2DArray > const & p_sampler, Vec3 const & p_value, IVec2 const p_offset )
	{
		return p_sampler.m_writer->textureOffset( p_sampler, p_value, p_offset );
	}

	Optional< Vec4 > textureOffset( Optional< Sampler2DArray > const & p_sampler, Vec3 const & p_value, IVec2 const p_offset, Vec4 const & p_lod )
	{
		return p_sampler.m_writer->textureOffset( p_sampler, p_value, p_offset, p_lod );
	}

	Optional< Vec4 > textureLodOffset( Optional< Sampler1DArray > const & p_sampler, Vec2 const & p_value, Vec4 const & p_lod, Int const p_offset )
	{
		return p_sampler.m_writer->textureLodOffset( p_sampler, p_value, p_lod, p_offset );
	}

	Optional< Vec4 > textureLodOffset( Optional< Sampler2DArray > const & p_sampler, Vec3 const & p_value, Vec4 const & p_lod, IVec2 const p_offset )
	{
		return p_sampler.m_writer->textureLodOffset( p_sampler, p_value, p_lod, p_offset );
	}

	Optional< Float > texture( Optional< Sampler1DShadow > const & p_sampler, Vec2 const & p_value )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value );
	}

	Optional< Float > texture( Optional< Sampler1DShadow > const & p_sampler, Vec2 const & p_value, Float const & p_lod )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value, p_lod );
	}

	Optional< Float > texture( Optional< Sampler2DShadow > const & p_sampler, Vec3 const & p_value )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value );
	}

	Optional< Float > texture( Optional< Sampler2DShadow > const & p_sampler, Vec3 const & p_value, Float const & p_lod )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value, p_lod );
	}

	Optional< Float > texture( Optional< SamplerCubeShadow > const & p_sampler, Vec4 const & p_value )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value );
	}

	Optional< Float > texture( Optional< SamplerCubeShadow > const & p_sampler, Vec4 const & p_value, Float const & p_lod )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value, p_lod );
	}

	Optional< Float > textureOffset( Optional< Sampler1DShadow > const & p_sampler, Vec2 const & p_value, Int const p_offset )
	{
		return p_sampler.m_writer->textureOffset( p_sampler, p_value, p_offset );
	}

	Optional< Float > textureOffset( Optional< Sampler1DShadow > const & p_sampler, Vec2 const & p_value, Int const p_offset, Float const & p_lod )
	{
		return p_sampler.m_writer->textureOffset( p_sampler, p_value, p_offset, p_lod );
	}

	Optional< Float > textureOffset( Optional< Sampler2DShadow > const & p_sampler, Vec3 const & p_value, IVec2 const p_offset )
	{
		return p_sampler.m_writer->textureOffset( p_sampler, p_value, p_offset );
	}

	Optional< Float > textureOffset( Optional< Sampler2DShadow > const & p_sampler, Vec3 const & p_value, IVec2 const p_offset, Float const & p_lod )
	{
		return p_sampler.m_writer->textureOffset( p_sampler, p_value, p_offset, p_lod );
	}

	Optional< Float > textureLodOffset( Optional< Sampler1DShadow > const & p_sampler, Vec2 const & p_value, Float const & p_lod, Int const p_offset )
	{
		return p_sampler.m_writer->textureLodOffset( p_sampler, p_value, p_lod, p_offset );
	}

	Optional< Float > textureLodOffset( Optional< Sampler2DShadow > const & p_sampler, Vec3 const & p_value, Float const & p_lod, IVec2 const p_offset )
	{
		return p_sampler.m_writer->textureLodOffset( p_sampler, p_value, p_lod, p_offset );
	}

	Optional< Float > texture( Optional< Sampler1DArrayShadow > const & p_sampler, Vec3 const & p_value )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value );
	}

	Optional< Float > texture( Optional< Sampler1DArrayShadow > const & p_sampler, Vec3 const & p_value, Float const & p_lod )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value, p_lod );
	}

	Optional< Float > texture( Optional< Sampler2DArrayShadow > const & p_sampler, Vec4 const & p_value )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value );
	}

	Optional< Float > texture( Optional< Sampler2DArrayShadow > const & p_sampler, Vec4 const & p_value, Float const & p_lod )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value, p_lod );
	}

	Optional< Float > texture( Optional< SamplerCubeArrayShadow > const & p_sampler, Vec4 const & p_value, Float const & p_layer )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value, p_layer );
	}

	Optional< Float > texture( Optional< SamplerCubeArrayShadow > const & p_sampler, Vec4 const & p_value, Float const & p_layer, Float const & p_lod )
	{
		return p_sampler.m_writer->texture( p_sampler, p_value, p_layer, p_lod );
	}

	Optional< Float > textureOffset( Optional< Sampler1DArrayShadow > const & p_sampler, Vec3 const & p_value, Int const p_offset )
	{
		return p_sampler.m_writer->textureOffset( p_sampler, p_value, p_offset );
	}

	Optional< Float > textureOffset( Optional< Sampler1DArrayShadow > const & p_sampler, Vec3 const & p_value, Int const p_offset, Float const & p_lod )
	{
		return p_sampler.m_writer->textureOffset( p_sampler, p_value, p_offset, p_lod );
	}

	Optional< Float > textureOffset( Optional< Sampler2DArrayShadow > const & p_sampler, Vec4 const & p_value, IVec2 const p_offset )
	{
		return p_sampler.m_writer->textureOffset( p_sampler, p_value, p_offset );
	}

	Optional< Float > textureOffset( Optional< Sampler2DArrayShadow > const & p_sampler, Vec4 const & p_value, IVec2 const p_offset, Float const & p_lod )
	{
		return p_sampler.m_writer->textureOffset( p_sampler, p_value, p_offset, p_lod );
	}

	Optional< Float > textureLodOffset( Optional< Sampler1DArrayShadow > const & p_sampler, Vec3 const & p_value, Float const & p_lod, Int const p_offset )
	{
		return p_sampler.m_writer->textureLodOffset( p_sampler, p_value, p_lod, p_offset );
	}

	Optional< Float > textureLodOffset( Optional< Sampler2DArrayShadow > const & p_sampler, Vec4 const & p_value, Float const & p_lod, IVec2 const p_offset )
	{
		return p_sampler.m_writer->textureLodOffset( p_sampler, p_value, p_lod, p_offset );
	}

	Optional< Vec2 > reflect( Optional< Vec2 > const & p_a, Type const & p_b )
	{
		return writeOptionalFunctionCall< Vec2 >( p_a.m_writer, cuT( "reflect" ), p_a, p_b );
	}

	Optional< Vec3 > reflect( Optional< Vec3 > const & p_a, Type const & p_b )
	{
		return writeOptionalFunctionCall< Vec3 >( p_a.m_writer, cuT( "reflect" ), p_a, p_b );
	}

	Optional< Vec4 > reflect( Optional< Vec4 > const & p_a, Type const & p_b )
	{
		return writeOptionalFunctionCall< Vec4 >( p_a.m_writer, cuT( "reflect" ), p_a, p_b );
	}

	Optional< Float > length( Optional< Type > const & p_value )
	{
		return writeOptionalFunctionCall< Float >( p_value.m_writer, cuT( "length" ), p_value );
	}

	Optional< Float > radians( Optional< Type > const & p_value )
	{
		return writeOptionalFunctionCall< Float >( p_value.m_writer, cuT( "radians" ), p_value );
	}

	Optional< Float > cos( Optional< Type > const & p_value )
	{
		return writeOptionalFunctionCall< Float >( p_value.m_writer, cuT( "cos" ), p_value );
	}

	Optional< Float > sin( Optional< Type > const & p_value )
	{
		return writeOptionalFunctionCall< Float >( p_value.m_writer, cuT( "sin" ), p_value );
	}

	Optional< Float > tan( Optional< Type > const & p_value )
	{
		return writeOptionalFunctionCall< Float >( p_value.m_writer, cuT( "tan" ), p_value );
	}

	Optional< Float > fract( Optional< Type > const & p_value )
	{
		return writeOptionalFunctionCall< Float >( p_value.m_writer, cuT( "fract" ), p_value );
	}
}
