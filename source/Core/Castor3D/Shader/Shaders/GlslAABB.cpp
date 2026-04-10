#include "Castor3D/Shader/Shaders/GlslAABB.hpp"

#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

namespace c3d::shader
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

	void Plane::set( sdw::Vec3 const & p1
			, sdw::Vec3 const & p2
			, sdw::Vec3 const & p3 )
	{
		auto & writer = sdw::findWriterMandat( *this );
		auto u = writer.declLocale( "u", p2 - p1 );
		auto v = writer.declLocale( "v", p3 - p1 );
		normal() = normalize( cross( v, u ) );
		distance() = -dot( p3, normal() );
	}

	sdw::RetFloat Plane::distance( sdw::Vec3 const & p )
	{
		return dot( normal(), p ) + distance();
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

	void AABB::set( sdw::Vec4 const & pmin
		, sdw::Vec4 const & pmax )
	{
		min() = pmin;
		max() = pmax;
	}

	void AABB::set( sdw::Vec3 const & position
		, sdw::Float const & range )
	{
		min() = vec4( position - vec3( range ), range );
		max() = vec4( position + vec3( range ), range );
	}

	sdw::RetBoolean AABB::intersectAABB( AABB const & rhs )const
	{
		return intersectAABBCoarse( rhs );
	}

	sdw::RetBoolean AABB::intersectAABBCoarse( AABB const & rhs )const
	{
		if ( !m_intersectAABBCoarse )
		{
			// Check to see if on AABB intersects another AABB.
			// Source: Real-time collision detection, Christer Ericson (2005)
			auto & writer = sdw::findWriterMandat( *this );
			m_intersectAABBCoarse = writer.implementFunction< sdw::Boolean >( "c3d_aabbIntersectAABB"
				, [&writer]( shader::AABB const & a
					, shader::AABB const & b )
				{
					auto result = writer.declLocale( "result"
						, 1_b );

					for ( int i = 0; i < 3; ++i )
					{
						result = result
							&& ( a.max()[i] >= b.min()[i]
								&& a.min()[i] <= b.max()[i] );
					}

					writer.returnStmt( result );
				}
				, shader::InAABB{ writer, "a" }
				, shader::InAABB{ writer, "b" } );
		}
		return m_intersectAABBCoarse( *this, rhs );
	}

	sdw::RetBoolean AABB::intersectSphere( sdw::Vec4 const & rhs )const
	{
		if ( !m_intersectSphere )
		{
			auto & writer = sdw::findWriterMandat( *this );
			m_intersectSphere = writer.implementFunction< sdw::Boolean >( "c3d_aabbIntersectSphere"
				, [&writer]( shader::AABB const & aabb
					, sdw::Vec4 const & sphere )
				{
					auto sqDistance = writer.declLocale( "sqDistance"
						, 0.0_f );
					auto v = writer.declLocale( "v"
						, 0.0_f );

					for ( int i = 0; i < 3; ++i )
					{
						v = sphere[i];

						sdwIF( writer, v < aabb.min()[i] )
						{
							sqDistance += pow( aabb.min()[i] - v, 2.0_f );
						}
						sdwFI
						sdwIF( writer, v > aabb.max()[i] )
						{
							sqDistance += pow( v - aabb.max()[i], 2.0_f );
						}
						sdwFI
					}

					writer.returnStmt( sqDistance <= sphere.w() * sphere.w() );
				}
				, shader::InAABB{ writer, "aabb" }
				, sdw::InVec4{ writer, "sphere" } );
		}
		return m_intersectSphere( *this, rhs );
	}

	sdw::RetBoolean AABB::intersectCone( Cone const & rhs )const
	{
		if ( !m_intersectCone )
		{
			auto & writer = sdw::findWriterMandat( *this );
			m_intersectCone = writer.implementFunction< sdw::Boolean >( "c3d_aabbIntersectCone"
				, [&writer]( shader::AABB const & aabb
					, shader::Cone const & cone )
				{
					auto aabbCenter = writer.declLocale( "aabbCenter"
						, aabb.min().xyz() + ( aabb.max().xyz() - aabb.min().xyz() ) / 2.0_f );
					auto sphere = writer.declLocale( "sphere"
						, vec4( aabbCenter, distance( aabb.max().xyz(), aabbCenter ) ) );
					auto V = writer.declLocale( "V"
						, sphere.xyz() - cone.apex() );
					auto lenSqV = writer.declLocale( "lenSqV"
						, dot( V, V ) );
					auto lenV1 = writer.declLocale( "lenV1"
						, dot( V, cone.direction() ) );
					auto distanceClosestPoint = writer.declLocale( "distanceClosestPoint"
						, cone.apertureCos() * sqrt( lenSqV - lenV1 * lenV1 ) - lenV1 * cone.apertureSin() );

					auto angleCull = distanceClosestPoint > sphere.w();
					auto frontCull = lenV1 > sphere.w() + cone.range();
					auto backCull = lenV1 < -sphere.w();

					writer.returnStmt( !( angleCull || frontCull || backCull ) );
				}
				, shader::InAABB{ writer, "aabb" }
				, shader::InCone{ writer, "cone" } );
		}
		return m_intersectCone( *this, rhs );
	}

	sdw::RetVec3 AABB::getPositiveVertex( sdw::Vec3 const & pnormal )const
	{
		if ( !m_getPositiveVertex )
		{
			auto & writer = sdw::findWriterMandat( *this );
			m_getPositiveVertex = writer.implementFunction< sdw::Vec3 >( "c3d_getPositiveVertex"
				, [&writer]( shader::AABB const & aabb
					, sdw::Vec3 const & normal )
				{
					auto result = writer.declLocale( "", aabb.min().xyz() );
					sdwIF( writer, normal.x() >= 0.0f )
					{
						result.x() = aabb.max().x();
					}
					sdwFI
					sdwIF( writer, normal.y() >= 0.0f )
					{
						result.y() = aabb.max().y();
					}
					sdwFI
					sdwIF( writer, normal.z() >= 0.0f )
					{
						result.z() = aabb.max().z();
					}
					sdwFI
					writer.returnStmt( result );
				}
				, shader::InAABB{ writer, "aabb" }
				, sdw::InVec3{ writer, "normal" } );
		}
		return m_getPositiveVertex( *this, pnormal );
	}

	//*********************************************************************************************
}
