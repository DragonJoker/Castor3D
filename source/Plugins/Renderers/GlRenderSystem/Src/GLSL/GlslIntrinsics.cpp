#include "GlslIntrinsics.hpp"

namespace GlRender
{
	namespace GLSL
	{
		Vec4 operator*( Vec4 const & p_a, Mat4 const & p_b )
		{
			Vec4 l_return( p_a.m_writer );
			l_return.m_value << Castor::String( p_a ) << cuT( " * " ) << Castor::String( p_b );
			return l_return;
		}

		Vec4 operator*( Mat4 const & p_a, Vec4 const & p_b )
		{
			Vec4 l_return( p_a.m_writer );
			l_return.m_value << Castor::String( p_a ) << cuT( " * " ) << Castor::String( p_b );
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

		inline Float length( Type const & p_value )
		{
			return WriteFunctionCall< Float >( p_value.m_writer, cuT( "length" ), p_value );
		}

		inline Float radians( Type const & p_value )
		{
			return WriteFunctionCall< Float >( p_value.m_writer, cuT( "radians" ), p_value );
		}

		inline Float cos( Type const & p_value )
		{
			return WriteFunctionCall< Float >( p_value.m_writer, cuT( "cos" ), p_value );
		}

		inline Float sin( Type const & p_value )
		{
			return WriteFunctionCall< Float >( p_value.m_writer, cuT( "sin" ), p_value );
		}

		inline Float tan( Type const & p_value )
		{
			return WriteFunctionCall< Float >( p_value.m_writer, cuT( "tan" ), p_value );
		}
	}
}