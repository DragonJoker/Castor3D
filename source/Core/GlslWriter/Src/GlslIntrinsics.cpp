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

	Vec4 texture1D( Sampler1D const & p_sampler, Type const & p_value )
	{
		return p_sampler.m_writer->Texture1D( p_sampler, p_value );
	}

	Vec4 texture2D( Sampler2D const & p_sampler, Type const & p_value )
	{
		return p_sampler.m_writer->Texture2D( p_sampler, p_value );
	}

	Vec4 texture3D( Sampler3D const & p_sampler, Type const & p_value )
	{
		return p_sampler.m_writer->Texture3D( p_sampler, p_value );
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

	Optional< Vec4 > texture1D( Optional< Sampler1D > const & p_sampler, Type const & p_value )
	{
		return p_sampler.m_writer->Texture1D( p_sampler, p_value );
	}

	Optional< Vec4 > texture2D( Optional< Sampler2D > const & p_sampler, Type const & p_value )
	{
		return p_sampler.m_writer->Texture2D( p_sampler, p_value );
	}

	Optional< Vec4 > texture3D( Optional< Sampler3D > const & p_sampler, Type const & p_value )
	{
		return p_sampler.m_writer->Texture3D( p_sampler, p_value );
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
}
