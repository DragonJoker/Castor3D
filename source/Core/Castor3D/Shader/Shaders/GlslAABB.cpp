#include "Castor3D/Shader/Shaders/GlslAABB.hpp"

#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

namespace castor3d::shader
{
	//*********************************************************************************************

	Plane::Plane( sdw::Vec3 const & normal
		, sdw::Float const & distance
		, bool enabled )
		: Plane{ sdw::findWriterMandat( normal, distance )
			, sdw::makeAggrInit( makeType( sdw::findTypesCache( normal, distance ) )
				, makeExprList( sdw::makeExpr( normal ), sdw::makeExpr( distance ) ) )
			, enabled }
	{
	}

	//*********************************************************************************************

	Cone::Cone( sdw::Vec3 const & apex
		, sdw::Vec3 const & direction
		, sdw::Float const & range
		, sdw::Float const & apertureCos
		, sdw::Float const & apertureSin
		, sdw::Float const & apertureTan
		, bool enabled )
		: Cone{ sdw::findWriterMandat( apex, range, direction, apertureCos, apertureSin, apertureTan )
			, sdw::makeAggrInit( makeType( sdw::findTypesCache( apex, range, direction, apertureCos, apertureSin, apertureTan ) )
				, makeExprList( sdw::makeExpr( apex ), sdw::makeExpr( range ), sdw::makeExpr( direction )
					, sdw::makeExpr( apertureCos ), sdw::makeExpr( apertureSin ), sdw::makeExpr( apertureTan ) ) )
			, enabled }
	{
	}

	//*********************************************************************************************

	AABB::AABB( sdw::Vec4 const & min
		, sdw::Vec4 const & max
		, bool enabled )
		: AABB{ sdw::findWriterMandat( min, max )
			, sdw::makeAggrInit( makeType( sdw::findTypesCache( min, max ) )
				, makeExprList( sdw::makeExpr( min ), sdw::makeExpr( max ) ) )
			, enabled }
	{
	}

	AABB::AABB( sdw::Vec3 const & position
		, sdw::Float const & range
		, bool enabled )
		: AABB{ sdw::findWriterMandat( position, range )
			, sdw::makeAggrInit( makeType( sdw::findTypesCache( position, range ) )
				, makeExprList( sdw::makeExpr( vec4( position - vec3( range ), range ) )
					, sdw::makeExpr( vec4( position + vec3( range ), range ) ) ) )
			, enabled }
	{
	}

	//*********************************************************************************************
}
