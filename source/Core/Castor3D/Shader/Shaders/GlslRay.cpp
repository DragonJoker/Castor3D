#include "Castor3D/Shader/Shaders/GlslRay.hpp"

#include <ShaderWriter/Writer.hpp>
#include <ShaderWriter/Intrinsics/IntrinsicFunctions.hpp>

namespace castor3d::shader
{
	namespace ray
	{
		static sdw::expr::ExprList getIntersectionInit( sdw::Vec3 const & p
			, sdw::Boolean const & v
			, sdw::Float const & t )
		{
			sdw::expr::ExprList result;
			result.emplace_back( makeExpr( p ) );
			result.emplace_back( makeExpr( v ) );
			result.emplace_back( makeExpr( t ) );
			return result;
		}

		static sdw::expr::ExprList getRayInit( sdw::Vec3 const & o
			, sdw::Vec3 const & d )
		{
			sdw::expr::ExprList result;
			result.emplace_back( makeExpr( o ) );
			result.emplace_back( makeExpr( d ) );
			return result;
		}
	}

	//************************************************************************************************

	Intersection::Intersection( sdw::ShaderWriter & writer )
		: Intersection{ writer, vec3( 0.0_f ), 0_b, -1.0_f }
	{
	}

	Intersection::Intersection( sdw::ShaderWriter & writer
		, sdw::Vec3 const & p )
		: Intersection{ writer, p, 0_b, -1.0_f }
	{
	}

	Intersection::Intersection( sdw::ShaderWriter & writer
		, sdw::Vec3 const & p
		, sdw::Boolean const & v )
		: Intersection{ writer, p, v, -1.0_f }
	{
	}

	Intersection::Intersection( sdw::ShaderWriter & writer
		, sdw::Vec3 const & p
		, sdw::Boolean const & v
		, sdw::Float const & t )
		: Intersection{ writer
			, sdw::makeAggrInit( makeType( writer.getTypesCache() ), ray::getIntersectionInit( p, v, t ) )
			, true }
	{
	}

	//************************************************************************************************

	Ray::Ray( sdw::ShaderWriter & writer )
		: Ray{ writer, vec3( 0.0_f ), vec3( 0.0_f ) }
	{
	}

	Ray::Ray( sdw::ShaderWriter & writer
		, sdw::Vec3 const & o )
		: Ray{ writer, o, vec3( 0.0_f ) }
	{
	}

	Ray::Ray( sdw::ShaderWriter & writer
		, sdw::Vec3 const & o
		, sdw::Vec3 const & d )
		: Ray{ writer
			, sdw::makeAggrInit( makeType( writer.getTypesCache() ), ray::getRayInit( o, d ) )
			, true }
	{
	}

	sdw::Vec3 Ray::step( sdw::Float const & t )const
	{
		return sdw::fma( direction, vec3( t ), origin );
	}

	//************************************************************************************************
}
