#include "Castor3D/Model/Mesh/Submesh/SubmeshUtils.hpp"

#include "Castor3D/Model/Mesh/Submesh/Component/TriFaceMapping.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Vertex.hpp"

using namespace castor;

namespace castor3d
{
	void SubmeshUtils::computeFacesFromPolygonVertex( InterleavedVertexArray & points
		, TriFaceMapping & triFace )
	{
		if ( !points.empty() )
		{
			auto * v1 = &points[0];
			auto * v2 = &points[1];
			auto * v3 = &points[2];
			triFace.addFace( 0, 1, 2 );
			v1->tex = castor::Point3f{ 0.0, 0.0, 0.0 };
			v2->tex = castor::Point3f{ 0.0, 0.0, 0.0 };
			v3->tex = castor::Point3f{ 0.0, 0.0, 0.0 };

			for ( uint32_t i = 2; i < uint32_t( points.size() - 1 ); i++ )
			{
				v2 = &points[i];
				v3 = &points[i + 1];
				triFace.addFace( 0, i, i + 1 );
				v2->tex = castor::Point3f{ 0.0, 0.0, 0.0 };
				v3->tex = castor::Point3f{ 0.0, 0.0, 0.0 };
			}
		}
	}

	void SubmeshUtils::computeNormals( InterleavedVertexArray & points
		, TriFaceMapping const & triFace
		, bool reverted )
	{
		auto & faces = triFace.getFaces();
		castor::Point3f pt0;

		// First we flush normals and tangents
		for ( auto & pt : points )
		{
			pt.nml = pt0;
			pt.tan = pt0;
		}

		castor::Coords3f coord;

		// Then we compute normals and tangents
		if ( reverted )
		{
			for ( auto const & face : faces )
			{
				auto & vtx1 = points[face[0]];
				auto & vtx2 = points[face[2]];
				auto & vtx3 = points[face[1]];
				auto const vec2m1 = vtx2.pos - vtx1.pos;
				auto const vec3m1 = vtx3.pos - vtx1.pos;
				auto const tex2m1 = vtx2.tex - vtx1.tex;
				auto const tex3m1 = vtx3.tex - vtx1.tex;
				auto const faceNormal = -point::cross( vec3m1, vec2m1 );
				auto const faceTangent = ( vec3m1 * tex2m1[1] ) - ( vec2m1 * tex3m1[1] );
				vtx1.nml += faceNormal;
				vtx2.nml += faceNormal;
				vtx3.nml += faceNormal;
				vtx1.tan += faceTangent;
				vtx2.tan += faceTangent;
				vtx3.tan += faceTangent;
			}
		}
		else
		{
			for ( auto const & face : faces )
			{
				auto & vtx1 = points[face[0]];
				auto & vtx2 = points[face[1]];
				auto & vtx3 = points[face[2]];
				auto const vec2m1 = vtx2.pos - vtx1.pos;
				auto const vec3m1 = vtx3.pos - vtx1.pos;
				auto const tex2m1 = vtx2.tex - vtx1.tex;
				auto const tex3m1 = vtx3.tex - vtx1.tex;
				auto const faceNormal = -point::cross( vec3m1, vec2m1 );
				auto const faceTangent = ( vec3m1 * tex2m1[1] ) - ( vec2m1 * tex3m1[1] );
				vtx1.nml += faceNormal;
				vtx2.nml += faceNormal;
				vtx3.nml += faceNormal;
				vtx1.tan += faceTangent;
				vtx2.tan += faceTangent;
				vtx3.tan += faceTangent;
			}
		}

		// Eventually we normalize the normals and tangents
		for ( auto & vtx : points )
		{
			point::normalise( vtx.nml );
			point::normalise( vtx.tan );
		}
	}

	void SubmeshUtils::computeTangentsFromNormals( InterleavedVertexArray & points
		, TriFaceMapping const & triFace )
	{
		auto & faces = triFace.getFaces();
		castor::Point3fArray arrayTangents( points.size() );

		// Pour chaque vertex, on stocke la somme des tangentes qui peuvent lui être affectées
		for ( auto const & face : faces )
		{
			auto & vtx1 = points[face[0]];
			auto & vtx2 = points[face[1]];
			auto & vtx3 = points[face[2]];
			auto const vec2m1 = vtx2.pos - vtx1.pos;
			auto const vec3m1 = vtx3.pos - vtx1.pos;
			auto const tex2m1 = vtx2.tex - vtx1.tex;
			auto const tex3m1 = vtx3.tex - vtx1.tex;

			// Calculates the triangle's area.
			float dirCorrection = tex2m1[0] * tex3m1[1] - tex2m1[1] * tex3m1[0];
			castor::Point3f faceTangent;

			if ( std::abs( dirCorrection ) > std::numeric_limits< float >::epsilon() )
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
			castor::Point3f tangent = point::getNormalised( value );
			tangent -= point.nml * castor::point::dot( tangent, point.nml );
			point.tan = tangent;
			i++;
		}
	}

	void SubmeshUtils::computeNormals( InterleavedVertexArray & points
		, Face const & face )
	{
		auto & vtx1 = points[face[0]];
		auto & vtx2 = points[face[1]];
		auto & vtx3 = points[face[2]];
		auto const vec2m1 = vtx2.pos - vtx1.pos;
		auto const vec3m1 = vtx3.pos - vtx1.pos;
		auto const faceNormal = point::getNormalised( -point::cross( vec3m1, vec2m1 ) );
		vtx1.nml += faceNormal;
		vtx2.nml += faceNormal;
		vtx3.nml += faceNormal;
		computeTangents( points, face );
	}

	void SubmeshUtils::computeTangents( InterleavedVertexArray & points
		, Face const & face )
	{
		auto & vtx1 = points[face[0]];
		auto & vtx2 = points[face[1]];
		auto & vtx3 = points[face[2]];
		auto const vec2m1 = vtx2.pos - vtx1.pos;
		auto const vec3m1 = vtx3.pos - vtx1.pos;
		auto const tex2m1 = vtx2.tex - vtx1.tex;
		auto const tex3m1 = vtx3.tex - vtx1.tex;
		auto const faceTangent = point::getNormalised( ( vec2m1 * tex3m1[1] ) - ( vec3m1 * tex2m1[1] ) );
		vtx1.tan += faceTangent;
		vtx2.tan += faceTangent;
		vtx3.tan += faceTangent;
	}
}
