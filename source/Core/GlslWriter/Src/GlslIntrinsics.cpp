#include "GlslIntrinsics.hpp"

using namespace Castor;

namespace GLSL
{
	Vec4 operator*( Vec4 const & p_a, Mat4 const & p_b )
	{
		Vec4 l_return( p_a.m_writer );
		l_return.m_value << String( p_a ) << cuT( " * " ) << String( p_b );
		return l_return;
	}

	Vec4 operator*( Mat4 const & p_a, Vec4 const & p_b )
	{
		Vec4 l_return( p_a.m_writer );
		l_return.m_value << String( p_a ) << cuT( " * " ) << String( p_b );
		return l_return;
	}

	Vec3 operator*( Mat3 const & p_a, Vec3 const & p_b )
	{
		Vec3 l_return( p_a.m_writer );
		l_return.m_value << String( p_a ) << cuT( " * " ) << String( p_b );
		return l_return;
	}

	Float operator+( float p_a, Float const & p_b )
	{
		Float l_return( p_b.m_writer );
		l_return.m_value << ToString( p_a ) << cuT( " + " ) << String( p_b );
		return l_return;
	}

	Float operator-( float p_a, Float const & p_b )
	{
		Float l_return( p_b.m_writer );
		l_return.m_value << ToString( p_a ) << cuT( " - " ) << String( p_b );
		return l_return;
	}

	Float operator*( float p_a, Float const & p_b )
	{
		Float l_return( p_b.m_writer );
		l_return.m_value << ToString( p_a ) << cuT( " * " ) << String( p_b );
		return l_return;
	}

	Float operator/( float p_a, Float const & p_b )
	{
		Float l_return( p_b.m_writer );
		l_return.m_value << ToString( p_a ) << cuT( " / " ) << String( p_b );
		return l_return;
	}

	Int operator+( int p_a, Int const & p_b )
	{
		Int l_return( p_b.m_writer );
		l_return.m_value << ToString( p_a ) << cuT( " + " ) << String( p_b );
		return l_return;
	}

	Int operator-( int p_a, Int const & p_b )
	{
		Int l_return( p_b.m_writer );
		l_return.m_value << ToString( p_a ) << cuT( " - " ) << String( p_b );
		return l_return;
	}

	Int operator*( int p_a, Int const & p_b )
	{
		Int l_return( p_b.m_writer );
		l_return.m_value << ToString( p_a ) << cuT( " * " ) << String( p_b );
		return l_return;
	}

	Int operator/( int p_a, Int const & p_b )
	{
		Int l_return( p_b.m_writer );
		l_return.m_value << ToString( p_a ) << cuT( " / " ) << String( p_b );
		return l_return;
	}

	Int operator%( Int const & p_a, int p_b )
	{
		Int l_return( p_a.m_writer );
		l_return.m_value << String( p_a ) << cuT( " % " ) << ToString( p_b );
		return l_return;
	}

	Int operator%( int p_a, Int const & p_b )
	{
		Int l_return( p_b.m_writer );
		l_return.m_value << ToString( p_a ) << cuT( " % " ) << String( p_b );
		return l_return;
	}

	Int operator%( Int const & p_a, Int const & p_b )
	{
		Int l_return( p_a.m_writer );
		l_return.m_value << String( p_a ) << cuT( " % " ) << String( p_b );
		return l_return;
	}

	Vec4 texture( Sampler1D const & p_sampler, Float const & p_value )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value );
	}

	Vec4 texture( Sampler1D const & p_sampler, Float const & p_value, Float const & p_lod )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value, p_lod );
	}

	Vec4 texture( Sampler2D const & p_sampler, Vec2 const & p_value )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value );
	}

	Vec4 texture( Sampler2D const & p_sampler, Vec2 const & p_value, Float const & p_lod )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value, p_lod );
	}

	Vec4 texture( Sampler3D const & p_sampler, Vec3 const & p_value )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value );
	}

	Vec4 texture( Sampler3D const & p_sampler, Vec3 const & p_value, Float const & p_lod )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value, p_lod );
	}

	Vec4 texture( SamplerCube const & p_sampler, Vec3 const & p_value )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value );
	}

	Vec4 texture( SamplerCube const & p_sampler, Vec3 const & p_value, Float const & p_lod )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value, p_lod );
	}

	Vec4 textureOffset( Sampler1D const & p_sampler, Float const & p_value, Int const p_offset )
	{
		return p_sampler.m_writer->TextureOffset( p_sampler, p_value, p_offset );
	}

	Vec4 textureOffset( Sampler1D const & p_sampler, Float const & p_value, Int const p_offset, Float const & p_lod )
	{
		return p_sampler.m_writer->TextureOffset( p_sampler, p_value, p_offset, p_lod );
	}

	Vec4 textureOffset( Sampler2D const & p_sampler, Vec2 const & p_value, IVec2 const p_offset )
	{
		return p_sampler.m_writer->TextureOffset( p_sampler, p_value, p_offset );
	}

	Vec4 textureOffset( Sampler2D const & p_sampler, Vec2 const & p_value, IVec2 const p_offset, Float const & p_lod )
	{
		return p_sampler.m_writer->TextureOffset( p_sampler, p_value, p_offset, p_lod );
	}

	Vec4 textureOffset( Sampler3D const & p_sampler, Vec3 const & p_value, IVec3 const p_offset )
	{
		return p_sampler.m_writer->TextureOffset( p_sampler, p_value, p_offset );
	}

	Vec4 textureOffset( Sampler3D const & p_sampler, Vec3 const & p_value, IVec3 const p_offset, Float const & p_lod )
	{
		return p_sampler.m_writer->TextureOffset( p_sampler, p_value, p_offset, p_lod );
	}

	Vec4 textureLodOffset( Sampler1D const & p_sampler, Float const & p_value, Float const & p_lod, Int const p_offset )
	{
		return p_sampler.m_writer->TextureLodOffset( p_sampler, p_value, p_lod, p_offset );
	}

	Vec4 textureLodOffset( Sampler2D const & p_sampler, Vec2 const & p_value, Float const & p_lod, IVec2 const p_offset )
	{
		return p_sampler.m_writer->TextureLodOffset( p_sampler, p_value, p_lod, p_offset );
	}

	Vec4 textureLodOffset( Sampler3D const & p_sampler, Vec3 const & p_value, Float const & p_lod, IVec3 const p_offset )
	{
		return p_sampler.m_writer->TextureLodOffset( p_sampler, p_value, p_lod, p_offset );
	}

	Vec4 texelFetch( SamplerBuffer const & p_sampler, Type const & p_value )
	{
		return p_sampler.m_writer->TexelFetch( p_sampler, p_value );
	}

	Vec4 texelFetch( Sampler1D const & p_sampler, Type const & p_value, Int const & p_modif )
	{
		return p_sampler.m_writer->TexelFetch( p_sampler, p_value, p_modif );
	}

	Vec4 texelFetch( Sampler2D const & p_sampler, Type const & p_value, Int const & p_modif )
	{
		return p_sampler.m_writer->TexelFetch( p_sampler, p_value, p_modif );
	}

	Vec4 texelFetch( Sampler3D const & p_sampler, Type const & p_value, Int const & p_modif )
	{
		return p_sampler.m_writer->TexelFetch( p_sampler, p_value, p_modif );
	}

	Vec4 texture( Sampler1DArray const & p_sampler, Vec2 const & p_value )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value );
	}

	Vec4 texture( Sampler1DArray const & p_sampler, Vec2 const & p_value, Vec4 const & p_lod )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value, p_lod );
	}

	Vec4 texture( Sampler2DArray const & p_sampler, Vec3 const & p_value )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value );
	}

	Vec4 texture( Sampler2DArray const & p_sampler, Vec3 const & p_value, Vec4 const & p_lod )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value, p_lod );
	}

	Vec4 texture( SamplerCubeArray const & p_sampler, Vec4 const & p_value )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value );
	}

	Vec4 texture( SamplerCubeArray const & p_sampler, Vec4 const & p_value, Vec4 const & p_lod )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value, p_lod );
	}

	Vec4 textureOffset( Sampler1DArray const & p_sampler, Vec2 const & p_value, Int const p_offset )
	{
		return p_sampler.m_writer->TextureOffset( p_sampler, p_value, p_offset );
	}

	Vec4 textureOffset( Sampler1DArray const & p_sampler, Vec2 const & p_value, Int const p_offset, Vec4 const & p_lod )
	{
		return p_sampler.m_writer->TextureOffset( p_sampler, p_value, p_offset, p_lod );
	}

	Vec4 textureOffset( Sampler2DArray const & p_sampler, Vec3 const & p_value, IVec2 const p_offset )
	{
		return p_sampler.m_writer->TextureOffset( p_sampler, p_value, p_offset );
	}

	Vec4 textureOffset( Sampler2DArray const & p_sampler, Vec3 const & p_value, IVec2 const p_offset, Vec4 const & p_lod )
	{
		return p_sampler.m_writer->TextureOffset( p_sampler, p_value, p_offset, p_lod );
	}

	Vec4 textureLodOffset( Sampler1DArray const & p_sampler, Vec2 const & p_value, Vec4 const & p_lod, Int const p_offset )
	{
		return p_sampler.m_writer->TextureLodOffset( p_sampler, p_value, p_lod, p_offset );
	}

	Vec4 textureLodOffset( Sampler2DArray const & p_sampler, Vec3 const & p_value, Vec4 const & p_lod, IVec2 const p_offset )
	{
		return p_sampler.m_writer->TextureLodOffset( p_sampler, p_value, p_lod, p_offset );
	}

	Float texture( Sampler1DShadow const & p_sampler, Vec2 const & p_value )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value );
	}

	Float texture( Sampler1DShadow const & p_sampler, Vec2 const & p_value, Float const & p_lod )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value, p_lod );
	}

	Float texture( Sampler2DShadow const & p_sampler, Vec3 const & p_value )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value );
	}

	Float texture( Sampler2DShadow const & p_sampler, Vec3 const & p_value, Float const & p_lod )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value, p_lod );
	}

	Float texture( SamplerCubeShadow const & p_sampler, Vec4 const & p_value )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value );
	}

	Float texture( SamplerCubeShadow const & p_sampler, Vec4 const & p_value, Float const & p_lod )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value, p_lod );
	}

	Float textureOffset( Sampler1DShadow const & p_sampler, Vec2 const & p_value, Int const p_offset )
	{
		return p_sampler.m_writer->TextureOffset( p_sampler, p_value, p_offset );
	}

	Float textureOffset( Sampler1DShadow const & p_sampler, Vec2 const & p_value, Int const p_offset, Float const & p_lod )
	{
		return p_sampler.m_writer->TextureOffset( p_sampler, p_value, p_offset, p_lod );
	}

	Float textureOffset( Sampler2DShadow const & p_sampler, Vec3 const & p_value, IVec2 const p_offset )
	{
		return p_sampler.m_writer->TextureOffset( p_sampler, p_value, p_offset );
	}

	Float textureOffset( Sampler2DShadow const & p_sampler, Vec3 const & p_value, IVec2 const p_offset, Float const & p_lod )
	{
		return p_sampler.m_writer->TextureOffset( p_sampler, p_value, p_offset, p_lod );
	}

	Float textureLodOffset( Sampler1DShadow const & p_sampler, Vec2 const & p_value, Float const & p_lod, Int const p_offset )
	{
		return p_sampler.m_writer->TextureLodOffset( p_sampler, p_value, p_lod, p_offset );
	}

	Float textureLodOffset( Sampler2DShadow const & p_sampler, Vec3 const & p_value, Float const & p_lod, IVec2 const p_offset )
	{
		return p_sampler.m_writer->TextureLodOffset( p_sampler, p_value, p_lod, p_offset );
	}

	Float texture( Sampler1DShadowArray const & p_sampler, Vec3 const & p_value )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value );
	}

	Float texture( Sampler1DShadowArray const & p_sampler, Vec3 const & p_value, Float const & p_lod )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value, p_lod );
	}

	Float texture( Sampler2DShadowArray const & p_sampler, Vec4 const & p_value )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value );
	}

	Float texture( Sampler2DShadowArray const & p_sampler, Vec4 const & p_value, Float const & p_lod )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value, p_lod );
	}

	Float texture( SamplerCubeShadowArray const & p_sampler, Vec4 const & p_value, Float const & p_layer )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value, p_layer );
	}

	Float texture( SamplerCubeShadowArray const & p_sampler, Vec4 const & p_value, Float const & p_layer, Float const & p_lod )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value, p_layer, p_lod );
	}

	Float textureOffset( Sampler1DShadowArray const & p_sampler, Vec3 const & p_value, Int const p_offset )
	{
		return p_sampler.m_writer->TextureOffset( p_sampler, p_value, p_offset );
	}

	Float textureOffset( Sampler1DShadowArray const & p_sampler, Vec3 const & p_value, Int const p_offset, Float const & p_lod )
	{
		return p_sampler.m_writer->TextureOffset( p_sampler, p_value, p_offset, p_lod );
	}

	Float textureOffset( Sampler2DShadowArray const & p_sampler, Vec4 const & p_value, IVec2 const p_offset )
	{
		return p_sampler.m_writer->TextureOffset( p_sampler, p_value, p_offset );
	}

	Float textureOffset( Sampler2DShadowArray const & p_sampler, Vec4 const & p_value, IVec2 const p_offset, Float const & p_lod )
	{
		return p_sampler.m_writer->TextureOffset( p_sampler, p_value, p_offset, p_lod );
	}

	Float textureLodOffset( Sampler1DShadowArray const & p_sampler, Vec3 const & p_value, Float const & p_lod, Int const p_offset )
	{
		return p_sampler.m_writer->TextureLodOffset( p_sampler, p_value, p_lod, p_offset );
	}

	Float textureLodOffset( Sampler2DShadowArray const & p_sampler, Vec4 const & p_value, Float const & p_lod, IVec2 const p_offset )
	{
		return p_sampler.m_writer->TextureLodOffset( p_sampler, p_value, p_lod, p_offset );
	}

	Vec2 reflect( Vec2 const & p_a, Type const & p_b )
	{
		return WriteFunctionCall< Vec2 >( p_a.m_writer, cuT( "reflect" ), p_a, p_b );
	}

	Vec3 reflect( Vec3 const & p_a, Type const & p_b )
	{
		return WriteFunctionCall< Vec3 >( p_a.m_writer, cuT( "reflect" ), p_a, p_b );
	}

	Vec4 reflect( Vec4 const & p_a, Type const & p_b )
	{
		return WriteFunctionCall< Vec4 >( p_a.m_writer, cuT( "reflect" ), p_a, p_b );
	}

	Float length( Type const & p_value )
	{
		return WriteFunctionCall< Float >( p_value.m_writer, cuT( "length" ), p_value );
	}

	Float radians( Type const & p_value )
	{
		return WriteFunctionCall< Float >( p_value.m_writer, cuT( "radians" ), p_value );
	}

	Float cos( Type const & p_value )
	{
		return WriteFunctionCall< Float >( p_value.m_writer, cuT( "cos" ), p_value );
	}

	Float sin( Type const & p_value )
	{
		return WriteFunctionCall< Float >( p_value.m_writer, cuT( "sin" ), p_value );
	}

	Float tan( Type const & p_value )
	{
		return WriteFunctionCall< Float >( p_value.m_writer, cuT( "tan" ), p_value );
	}

	Float fract( Type const & p_value )
	{
		return WriteFunctionCall< Float >( p_value.m_writer, cuT( "fract" ), p_value );
	}

	Optional< Vec4 > texture( Optional< Sampler1D > const & p_sampler, Float const & p_value )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value );
	}

	Optional< Vec4 > texture( Optional< Sampler1D > const & p_sampler, Float const & p_value, Float const & p_lod )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value, p_lod );
	}

	Optional< Vec4 > texture( Optional< Sampler2D > const & p_sampler, Vec2 const & p_value )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value );
	}

	Optional< Vec4 > texture( Optional< Sampler2D > const & p_sampler, Vec2 const & p_value, Float const & p_lod )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value, p_lod );
	}

	Optional< Vec4 > texture( Optional< Sampler3D > const & p_sampler, Vec3 const & p_value )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value );
	}

	Optional< Vec4 > texture( Optional< Sampler3D > const & p_sampler, Vec3 const & p_value, Float const & p_lod )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value, p_lod );
	}

	Optional< Vec4 > texture( Optional< SamplerCube > const & p_sampler, Vec3 const & p_value )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value );
	}

	Optional< Vec4 > texture( Optional< SamplerCube > const & p_sampler, Vec3 const & p_value, Float const & p_lod )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value, p_lod );
	}

	Optional< Vec4 > textureOffset( Optional< Sampler1D > const & p_sampler, Float const & p_value, Int const & p_offset )
	{
		return p_sampler.m_writer->TextureOffset( p_sampler, p_value, p_offset );
	}

	Optional< Vec4 > textureOffset( Optional< Sampler1D > const & p_sampler, Float const & p_value, Int const & p_offset, Float const & p_lod )
	{
		return p_sampler.m_writer->TextureOffset( p_sampler, p_value, p_offset, p_lod );
	}

	Optional< Vec4 > textureOffset( Optional< Sampler2D > const & p_sampler, Vec2 const & p_value, IVec2 const & p_offset )
	{
		return p_sampler.m_writer->TextureOffset( p_sampler, p_value, p_offset );
	}

	Optional< Vec4 > textureOffset( Optional< Sampler2D > const & p_sampler, Vec2 const & p_value, IVec2 const & p_offset, Float const & p_lod )
	{
		return p_sampler.m_writer->TextureOffset( p_sampler, p_value, p_offset, p_lod );
	}

	Optional< Vec4 > textureOffset( Optional< Sampler3D > const & p_sampler, Vec3 const & p_value, IVec3 const & p_offset )
	{
		return p_sampler.m_writer->TextureOffset( p_sampler, p_value, p_offset );
	}

	Optional< Vec4 > textureOffset( Optional< Sampler3D > const & p_sampler, Vec3 const & p_value, IVec3 const & p_offset, Float const & p_lod )
	{
		return p_sampler.m_writer->TextureOffset( p_sampler, p_value, p_offset, p_lod );
	}

	Optional< Vec4 > textureLodOffset( Optional< Sampler1D > const & p_sampler, Float const & p_value, Float const & p_lod, Int const p_offset )
	{
		return p_sampler.m_writer->TextureLodOffset( p_sampler, p_value, p_lod, p_offset );
	}

	Optional< Vec4 > textureLodOffset( Optional< Sampler2D > const & p_sampler, Vec2 const & p_value, Float const & p_lod, IVec2 const p_offset )
	{
		return p_sampler.m_writer->TextureLodOffset( p_sampler, p_value, p_lod, p_offset );
	}

	Optional< Vec4 > textureLodOffset( Optional< Sampler3D > const & p_sampler, Vec3 const & p_value, Float const & p_lod, IVec3 const p_offset )
	{
		return p_sampler.m_writer->TextureLodOffset( p_sampler, p_value, p_lod, p_offset );
	}

	Optional< Vec4 > texelFetch( Optional< SamplerBuffer > const & p_sampler, Type const & p_value )
	{
		return p_sampler.m_writer->TexelFetch( p_sampler, p_value );
	}

	Optional< Vec4 > texelFetch( Optional< Sampler1D > const & p_sampler, Type const & p_value, Int const & p_modif )
	{
		return p_sampler.m_writer->TexelFetch( p_sampler, p_value, p_modif );
	}

	Optional< Vec4 > texelFetch( Optional< Sampler2D > const & p_sampler, Type const & p_value, Int const & p_modif )
	{
		return p_sampler.m_writer->TexelFetch( p_sampler, p_value, p_modif );
	}

	Optional< Vec4 > texelFetch( Optional< Sampler3D > const & p_sampler, Type const & p_value, Int const & p_modif )
	{
		return p_sampler.m_writer->TexelFetch( p_sampler, p_value, p_modif );
	}

	Optional< Vec4 > texture( Optional< Sampler1DArray > const & p_sampler, Vec2 const & p_value )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value );
	}

	Optional< Vec4 > texture( Optional< Sampler1DArray > const & p_sampler, Vec2 const & p_value, Vec4 const & p_lod )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value, p_lod );
	}

	Optional< Vec4 > texture( Optional< Sampler2DArray > const & p_sampler, Vec3 const & p_value )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value );
	}

	Optional< Vec4 > texture( Optional< Sampler2DArray > const & p_sampler, Vec3 const & p_value, Vec4 const & p_lod )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value, p_lod );
	}

	Optional< Vec4 > texture( Optional< SamplerCubeArray > const & p_sampler, Vec4 const & p_value )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value );
	}

	Optional< Vec4 > texture( Optional< SamplerCubeArray > const & p_sampler, Vec4 const & p_value, Vec4 const & p_lod )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value, p_lod );
	}

	Optional< Vec4 > textureOffset( Optional< Sampler1DArray > const & p_sampler, Vec2 const & p_value, Int const p_offset )
	{
		return p_sampler.m_writer->TextureOffset( p_sampler, p_value, p_offset );
	}

	Optional< Vec4 > textureOffset( Optional< Sampler1DArray > const & p_sampler, Vec2 const & p_value, Int const p_offset, Vec4 const & p_lod )
	{
		return p_sampler.m_writer->TextureOffset( p_sampler, p_value, p_offset, p_lod );
	}

	Optional< Vec4 > textureOffset( Optional< Sampler2DArray > const & p_sampler, Vec3 const & p_value, IVec2 const p_offset )
	{
		return p_sampler.m_writer->TextureOffset( p_sampler, p_value, p_offset );
	}

	Optional< Vec4 > textureOffset( Optional< Sampler2DArray > const & p_sampler, Vec3 const & p_value, IVec2 const p_offset, Vec4 const & p_lod )
	{
		return p_sampler.m_writer->TextureOffset( p_sampler, p_value, p_offset, p_lod );
	}

	Optional< Vec4 > textureLodOffset( Optional< Sampler1DArray > const & p_sampler, Vec2 const & p_value, Vec4 const & p_lod, Int const p_offset )
	{
		return p_sampler.m_writer->TextureLodOffset( p_sampler, p_value, p_lod, p_offset );
	}

	Optional< Vec4 > textureLodOffset( Optional< Sampler2DArray > const & p_sampler, Vec3 const & p_value, Vec4 const & p_lod, IVec2 const p_offset )
	{
		return p_sampler.m_writer->TextureLodOffset( p_sampler, p_value, p_lod, p_offset );
	}

	Optional< Float > texture( Optional< Sampler1DShadow > const & p_sampler, Vec2 const & p_value )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value );
	}

	Optional< Float > texture( Optional< Sampler1DShadow > const & p_sampler, Vec2 const & p_value, Float const & p_lod )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value, p_lod );
	}

	Optional< Float > texture( Optional< Sampler2DShadow > const & p_sampler, Vec3 const & p_value )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value );
	}

	Optional< Float > texture( Optional< Sampler2DShadow > const & p_sampler, Vec3 const & p_value, Float const & p_lod )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value, p_lod );
	}

	Optional< Float > texture( Optional< SamplerCubeShadow > const & p_sampler, Vec4 const & p_value )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value );
	}

	Optional< Float > texture( Optional< SamplerCubeShadow > const & p_sampler, Vec4 const & p_value, Float const & p_lod )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value, p_lod );
	}

	Optional< Float > textureOffset( Optional< Sampler1DShadow > const & p_sampler, Vec2 const & p_value, Int const p_offset )
	{
		return p_sampler.m_writer->TextureOffset( p_sampler, p_value, p_offset );
	}

	Optional< Float > textureOffset( Optional< Sampler1DShadow > const & p_sampler, Vec2 const & p_value, Int const p_offset, Float const & p_lod )
	{
		return p_sampler.m_writer->TextureOffset( p_sampler, p_value, p_offset, p_lod );
	}

	Optional< Float > textureOffset( Optional< Sampler2DShadow > const & p_sampler, Vec3 const & p_value, IVec2 const p_offset )
	{
		return p_sampler.m_writer->TextureOffset( p_sampler, p_value, p_offset );
	}

	Optional< Float > textureOffset( Optional< Sampler2DShadow > const & p_sampler, Vec3 const & p_value, IVec2 const p_offset, Float const & p_lod )
	{
		return p_sampler.m_writer->TextureOffset( p_sampler, p_value, p_offset, p_lod );
	}

	Optional< Float > textureLodOffset( Optional< Sampler1DShadow > const & p_sampler, Vec2 const & p_value, Float const & p_lod, Int const p_offset )
	{
		return p_sampler.m_writer->TextureLodOffset( p_sampler, p_value, p_lod, p_offset );
	}

	Optional< Float > textureLodOffset( Optional< Sampler2DShadow > const & p_sampler, Vec3 const & p_value, Float const & p_lod, IVec2 const p_offset )
	{
		return p_sampler.m_writer->TextureLodOffset( p_sampler, p_value, p_lod, p_offset );
	}

	Optional< Float > texture( Optional< Sampler1DShadowArray > const & p_sampler, Vec3 const & p_value )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value );
	}

	Optional< Float > texture( Optional< Sampler1DShadowArray > const & p_sampler, Vec3 const & p_value, Float const & p_lod )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value, p_lod );
	}

	Optional< Float > texture( Optional< Sampler2DShadowArray > const & p_sampler, Vec4 const & p_value )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value );
	}

	Optional< Float > texture( Optional< Sampler2DShadowArray > const & p_sampler, Vec4 const & p_value, Float const & p_lod )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value, p_lod );
	}

	Optional< Float > texture( Optional< SamplerCubeShadowArray > const & p_sampler, Vec4 const & p_value, Float const & p_layer )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value, p_layer );
	}

	Optional< Float > texture( Optional< SamplerCubeShadowArray > const & p_sampler, Vec4 const & p_value, Float const & p_layer, Float const & p_lod )
	{
		return p_sampler.m_writer->Texture( p_sampler, p_value, p_layer, p_lod );
	}

	Optional< Float > textureOffset( Optional< Sampler1DShadowArray > const & p_sampler, Vec3 const & p_value, Int const p_offset )
	{
		return p_sampler.m_writer->TextureOffset( p_sampler, p_value, p_offset );
	}

	Optional< Float > textureOffset( Optional< Sampler1DShadowArray > const & p_sampler, Vec3 const & p_value, Int const p_offset, Float const & p_lod )
	{
		return p_sampler.m_writer->TextureOffset( p_sampler, p_value, p_offset, p_lod );
	}

	Optional< Float > textureOffset( Optional< Sampler2DShadowArray > const & p_sampler, Vec4 const & p_value, IVec2 const p_offset )
	{
		return p_sampler.m_writer->TextureOffset( p_sampler, p_value, p_offset );
	}

	Optional< Float > textureOffset( Optional< Sampler2DShadowArray > const & p_sampler, Vec4 const & p_value, IVec2 const p_offset, Float const & p_lod )
	{
		return p_sampler.m_writer->TextureOffset( p_sampler, p_value, p_offset, p_lod );
	}

	Optional< Float > textureLodOffset( Optional< Sampler1DShadowArray > const & p_sampler, Vec3 const & p_value, Float const & p_lod, Int const p_offset )
	{
		return p_sampler.m_writer->TextureLodOffset( p_sampler, p_value, p_lod, p_offset );
	}

	Optional< Float > textureLodOffset( Optional< Sampler2DShadowArray > const & p_sampler, Vec4 const & p_value, Float const & p_lod, IVec2 const p_offset )
	{
		return p_sampler.m_writer->TextureLodOffset( p_sampler, p_value, p_lod, p_offset );
	}

	Optional< Vec2 > reflect( Optional< Vec2 > const & p_a, Type const & p_b )
	{
		return WriteOptionalFunctionCall< Vec2 >( p_a.m_writer, cuT( "reflect" ), p_a, p_b );
	}

	Optional< Vec3 > reflect( Optional< Vec3 > const & p_a, Type const & p_b )
	{
		return WriteOptionalFunctionCall< Vec3 >( p_a.m_writer, cuT( "reflect" ), p_a, p_b );
	}

	Optional< Vec4 > reflect( Optional< Vec4 > const & p_a, Type const & p_b )
	{
		return WriteOptionalFunctionCall< Vec4 >( p_a.m_writer, cuT( "reflect" ), p_a, p_b );
	}

	Optional< Float > length( Optional< Type > const & p_value )
	{
		return WriteOptionalFunctionCall< Float >( p_value.m_writer, cuT( "length" ), p_value );
	}

	Optional< Float > radians( Optional< Type > const & p_value )
	{
		return WriteOptionalFunctionCall< Float >( p_value.m_writer, cuT( "radians" ), p_value );
	}

	Optional< Float > cos( Optional< Type > const & p_value )
	{
		return WriteOptionalFunctionCall< Float >( p_value.m_writer, cuT( "cos" ), p_value );
	}

	Optional< Float > sin( Optional< Type > const & p_value )
	{
		return WriteOptionalFunctionCall< Float >( p_value.m_writer, cuT( "sin" ), p_value );
	}

	Optional< Float > tan( Optional< Type > const & p_value )
	{
		return WriteOptionalFunctionCall< Float >( p_value.m_writer, cuT( "tan" ), p_value );
	}

	Optional< Float > fract( Optional< Type > const & p_value )
	{
		return WriteOptionalFunctionCall< Float >( p_value.m_writer, cuT( "fract" ), p_value );
	}
}
