#include "SubmeshUtils.hpp"

#include "SubmeshComponent/TriFaceMapping.hpp"
#include "Submesh.hpp"
#include "Vertex.hpp"

using namespace castor;

namespace castor3d
{
	void SubmeshUtils::computeFacesFromPolygonVertex( Submesh & submesh
		, TriFaceMapping & triFace )
	{
		auto & points = submesh.getPoints();

		if ( !points.empty() )
		{
			auto & v1 = points[0];
			auto & v2 = points[1];
			auto & v3 = points[2];
			triFace.addFace( 0, 1, 2 );
			v1.m_tex = Point3r{ 0.0, 0.0, 0.0 };
			v2.m_tex = Point3r{ 0.0, 0.0, 0.0 };
			v3.m_tex = Point3r{ 0.0, 0.0, 0.0 };

			for ( uint32_t i = 2; i < uint32_t( points.size() - 1 ); i++ )
			{
				auto & v2 = points[i];
				auto & v3 = points[i + 1];
				triFace.addFace( 0, i, i + 1 );
				v2.m_tex = Point3r{ 0.0, 0.0, 0.0 };
				v3.m_tex = Point3r{ 0.0, 0.0, 0.0 };
			}
		}
	}

	void SubmeshUtils::computeNormals( Submesh & submesh
		, TriFaceMapping & triFace
		, bool reverted )
	{
		auto & points = submesh.getPoints();
		auto & faces = triFace.getFaces();
		Point3r pt0;

		// First we flush normals and tangents
		for ( auto & pt : points )
		{
			pt.m_nml = pt0;
			pt.m_tan = pt0;
		}

		Coords3r coord;

		// Then we compute normals and tangents
		if ( reverted )
		{
			for ( auto const & face : faces )
			{
				auto & vtx1 = points[face[0]];
				auto & vtx2 = points[face[1]];
				auto & vtx3 = points[face[2]];
				auto const vec2m1 = vtx2.m_pos - vtx1.m_pos;
				auto const vec3m1 = vtx3.m_pos - vtx1.m_pos;
				auto const tex2m1 = vtx2.m_tex - vtx1.m_tex;
				auto const tex3m1 = vtx3.m_tex - vtx1.m_tex;
				auto const faceNormal = -point::cross( vec3m1, vec2m1 );
				auto const faceTangent = ( vec2m1 * tex3m1[1] ) - ( vec3m1 * tex2m1[1] );
				vtx1.m_nml += faceNormal;
				vtx2.m_nml += faceNormal;
				vtx3.m_nml += faceNormal;
				vtx1.m_tan += faceTangent;
				vtx2.m_tan += faceTangent;
				vtx3.m_tan += faceTangent;
			}
		}
		else
		{
			for ( auto const & face : faces )
			{
				auto & vtx1 = points[face[0]];
				auto & vtx2 = points[face[1]];
				auto & vtx3 = points[face[2]];
				auto const vec2m1 = vtx2.m_pos - vtx1.m_pos;
				auto const vec3m1 = vtx3.m_pos - vtx1.m_pos;
				auto const tex2m1 = vtx2.m_tex - vtx1.m_tex;
				auto const tex3m1 = vtx3.m_tex - vtx1.m_tex;
				auto const faceNormal = -point::cross( vec3m1, vec2m1 );
				auto const faceTangent = ( vec3m1 * tex2m1[1] ) - ( vec2m1 * tex3m1[1] );
				vtx1.m_nml += faceNormal;
				vtx2.m_nml += faceNormal;
				vtx3.m_nml += faceNormal;
				vtx1.m_tan += faceTangent;
				vtx2.m_tan += faceTangent;
				vtx3.m_tan += faceTangent;
			}
		}

		// Eventually we normalize the normals and tangents
		for ( auto vtx : points )
		{
			point::normalise( vtx.m_nml );
			point::normalise( vtx.m_tan );
		}
	}

	void SubmeshUtils::computeNormals( Submesh & submesh
		, Face const & face )
	{
		auto & points = submesh.getPoints();
		auto & vtx1 = points[face[0]];
		auto & vtx2 = points[face[1]];
		auto & vtx3 = points[face[2]];
		auto const vec2m1 = vtx2.m_pos - vtx1.m_pos;
		auto const vec3m1 = vtx3.m_pos - vtx1.m_pos;
		auto const faceNormal = point::getNormalised( -point::cross( vec3m1, vec2m1 ) );
		vtx1.m_nml += faceNormal;
		vtx2.m_nml += faceNormal;
		vtx3.m_nml += faceNormal;
		computeTangents( submesh, face );
	}

	void SubmeshUtils::computeTangents( Submesh & submesh
		, Face const & face )
	{
		auto & points = submesh.getPoints();
		auto & vtx1 = points[face[0]];
		auto & vtx2 = points[face[1]];
		auto & vtx3 = points[face[2]];
		auto const vec2m1 = vtx2.m_pos - vtx1.m_pos;
		auto const vec3m1 = vtx3.m_pos - vtx1.m_pos;
		auto const tex2m1 = vtx2.m_tex - vtx1.m_tex;
		auto const tex3m1 = vtx3.m_tex - vtx1.m_tex;
		auto const faceTangent = point::getNormalised( ( vec2m1 * tex3m1[1] ) - ( vec3m1 * tex2m1[1] ) );
		vtx1.m_tan += faceTangent;
		vtx2.m_tan += faceTangent;
		vtx3.m_tan += faceTangent;
	}

	void SubmeshUtils::computeTangentsFromNormals( Submesh & submesh
		, TriFaceMapping & triFace )
	{
		auto & points = submesh.getPoints();
		auto & faces = triFace.getFaces();
		Point3rArray arrayTangents( points.size() );

		// Pour chaque vertex, on stocke la somme des tangentes qui peuvent lui être affectées
		for ( auto const & face : faces )
		{
			auto & vtx1 = points[face[0]];
			auto & vtx2 = points[face[1]];
			auto & vtx3 = points[face[2]];
			auto const vec2m1 = vtx2.m_pos - vtx1.m_pos;
			auto const vec3m1 = vtx3.m_pos - vtx1.m_pos;
			auto const tex2m1 = vtx2.m_tex - vtx1.m_tex;
			auto const tex3m1 = vtx3.m_tex - vtx1.m_tex;

			// Calculates the triangle's area.
			real dirCorrection = tex2m1[0] * tex3m1[1] - tex2m1[1] * tex3m1[0];
			Point3r faceTangent;

			if ( dirCorrection )
			{
				dirCorrection = 1 / dirCorrection;
				// Calculates the face tangent to the current triangle.
				faceTangent[0] = dirCorrection * ( ( vec2m1[0] * tex3m1[1] ) + ( vec3m1[0] * -tex2m1[1] ) );
				faceTangent[1] = dirCorrection * ( ( vec2m1[1] * tex3m1[1] ) + ( vec3m1[1] * -tex2m1[1] ) );
				faceTangent[2] = dirCorrection * ( ( vec2m1[2] * tex3m1[1] ) + ( vec3m1[2] * -tex2m1[1] ) );
			}

			arrayTangents[face[0]] += faceTangent;
			arrayTangents[face[1]] += faceTangent;
			arrayTangents[face[2]] += faceTangent;
		}

		uint32_t i = 0;
		// Average tangents
		for ( auto & value : arrayTangents )
		{
			auto & point = points[i];
			Point3r tangent = point::getNormalised( value );
			tangent -= point.m_nml * point::dot( tangent, point.m_nml );
			Point3r bitangent = point::cross( point.m_nml, tangent );
			point.m_tan = tangent;
			point.m_bin = bitangent;
			i++;
		}
	}

	void SubmeshUtils::computeTangentsFromBitangents( Submesh & submesh )
	{
		for ( auto & point : submesh.getPoints() )
		{
			point.m_tan = point::cross( point.m_nml, point.m_bin );
		}
	}

	void SubmeshUtils::computeBitangents( Submesh & submesh )
	{
		for ( auto & point : submesh.getPoints() )
		{
			point.m_bin = point::cross( point.m_tan, point.m_nml );
		}
	}
}
