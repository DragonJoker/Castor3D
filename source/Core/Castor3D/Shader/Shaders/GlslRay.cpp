#include "Castor3D/Shader/Shaders/GlslRay.hpp"
#include "Castor3D/Shader/Shaders/GlslVolumeShaders.hpp"

#include <ShaderWriter/Writer.hpp>
#include <ShaderWriter/Intrinsics/IntrinsicFunctions.hpp>

namespace c3d::shader
{
	//************************************************************************************************

	Intersection::Intersection( sdw::ShaderWriter & writer )
		: Intersection{ vec3( 0.0_f ), 0_b, -1.0_f }
	{
	}

	Intersection::Intersection( sdw::Vec3 const & p )
		: Intersection{ p, 0_b, -1.0_f }
	{
	}

	Intersection::Intersection( sdw::Vec3 const & p
		, sdw::Boolean const & v )
		: Intersection{ p, v, -1.0_f }
	{
	}

	Intersection::Intersection( sdw::Vec3 const & p
		, sdw::Boolean const & v
		, sdw::Float const & t )
		: Intersection{ sdw::findWriterMandat( p, v, t )
			, sdw::makeAggrInit( makeType( sdw::findTypesCache( p, v, t ) )
				, makeExprList( sdw::makeExpr( p ), sdw::makeExpr( v ), sdw::makeExpr( t ) ) )
			, true }
	{
	}

	//************************************************************************************************

	Ray::Ray( sdw::ShaderWriter & writer )
		: Ray{ vec3( 0.0_f ), vec3( 0.0_f ) }
	{
	}

	Ray::Ray( sdw::Vec3 const & o )
		: Ray{ o, vec3( 0.0_f ) }
	{
	}

	Ray::Ray( sdw::Vec3 const & o
		, sdw::Vec3 const & d )
		: Ray{ sdw::findWriterMandat( o, d )
			, sdw::makeAggrInit( makeType( sdw::findTypesCache( o, d ) )
				, makeExprList( sdw::makeExpr( o ), sdw::makeExpr( d ) ) )
			, true }
	{
	}

	sdw::Vec3 Ray::step( sdw::Float const & t )const
	{
		return sdw::fma( direction, vec3( t ), origin );
	}

	//************************************************************************************************
}
