﻿#include "SubmeshUtils.hpp"

#include "Submesh.hpp"
#include "Vertex.hpp"

using namespace Castor;

namespace Castor3D
{
	void SubmeshUtils::ComputeFacesFromPolygonVertex( Submesh & p_submesh )
	{
		auto & points = p_submesh.GetPoints();

		if ( points.size() )
		{
			BufferElementGroupSPtr v1 = points[0];
			BufferElementGroupSPtr v2 = points[1];
			BufferElementGroupSPtr v3 = points[2];
			p_submesh.AddFace( 0, 1, 2 );
			Vertex::SetTexCoord( v1, 0.0, 0.0 );
			Vertex::SetTexCoord( v2, 0.0, 0.0 );
			Vertex::SetTexCoord( v3, 0.0, 0.0 );

			for ( uint32_t i = 2; i < uint32_t( points.size() - 1 ); i++ )
			{
				v2 = points[i];
				v3 = points[i + 1];
				p_submesh.AddFace( 0, i, i + 1 );
				Vertex::SetTexCoord( v2, 0.0, 0.0 );
				Vertex::SetTexCoord( v3, 0.0, 0.0 );
			}
		}
	}

	void SubmeshUtils::ComputeNormals( Submesh & p_submesh
		, bool p_reverted )
	{
		auto & points = p_submesh.GetPoints();
		auto & faces = p_submesh.GetFaces();

		Point3r vec2m1;
		Point3r vec3m1;
		Point3r tex2m1;
		Point3r tex3m1;
		Point3r pt1;
		Point3r pt2;
		Point3r pt3;
		Point3r uv1;
		Point3r uv2;
		Point3r uv3;
		BufferElementGroupSPtr pVtx1;
		BufferElementGroupSPtr pVtx2;
		BufferElementGroupSPtr pVtx3;
		Point3r vFaceNormal;
		Point3r vFaceTangent;

		// First we flush normals and tangents
		for ( auto & pt : points )
		{
			Vertex::SetNormal( pt, pt1 );
			Vertex::SetTangent( pt, pt1 );
		}

		Coords3r coord;

		// Then we compute normals and tangents
		if ( p_reverted )
		{
			for ( auto const & face : faces )
			{
				pVtx1 = points[face[0]];
				pVtx2 = points[face[1]];
				pVtx3 = points[face[2]];
				Vertex::GetPosition( pVtx1, pt1 );
				Vertex::GetPosition( pVtx2, pt2 );
				Vertex::GetPosition( pVtx3, pt3 );
				Vertex::GetTexCoord( pVtx1, uv1 );
				Vertex::GetTexCoord( pVtx2, uv2 );
				Vertex::GetTexCoord( pVtx3, uv3 );
				vec2m1 = pt2 - pt1;
				vec3m1 = pt3 - pt1;
				tex2m1 = uv2 - uv1;
				tex3m1 = uv3 - uv1;
				vFaceNormal = -( vec3m1 ^ vec2m1 );
				vFaceTangent = ( vec2m1 * tex3m1[1] ) - ( vec3m1 * tex2m1[1] );
				Vertex::GetNormal( pVtx1, coord ) += vFaceNormal;
				Vertex::GetNormal( pVtx2, coord ) += vFaceNormal;
				Vertex::GetNormal( pVtx3, coord ) += vFaceNormal;
				Vertex::GetTangent( pVtx1, coord ) += vFaceTangent;
				Vertex::GetTangent( pVtx2, coord ) += vFaceTangent;
				Vertex::GetTangent( pVtx3, coord ) += vFaceTangent;
			}
		}
		else
		{
			for ( auto const & face : faces )
			{
				pVtx1 = points[face[0]];
				pVtx2 = points[face[1]];
				pVtx3 = points[face[2]];
				Vertex::GetPosition( pVtx1, pt1 );
				Vertex::GetPosition( pVtx2, pt2 );
				Vertex::GetPosition( pVtx3, pt3 );
				Vertex::GetTexCoord( pVtx1, uv1 );
				Vertex::GetTexCoord( pVtx2, uv2 );
				Vertex::GetTexCoord( pVtx3, uv3 );
				vec2m1 = pt2 - pt1;
				vec3m1 = pt3 - pt1;
				tex2m1 = uv2 - uv1;
				tex3m1 = uv3 - uv1;
				vFaceNormal = vec3m1 ^ vec2m1;
				vFaceTangent = ( vec3m1 * tex2m1[1] ) - ( vec2m1 * tex3m1[1] );
				Vertex::GetNormal( pVtx1, coord ) += vFaceNormal;
				Vertex::GetNormal( pVtx2, coord ) += vFaceNormal;
				Vertex::GetNormal( pVtx3, coord ) += vFaceNormal;
				Vertex::GetTangent( pVtx1, coord ) += vFaceTangent;
				Vertex::GetTangent( pVtx2, coord ) += vFaceTangent;
				Vertex::GetTangent( pVtx3, coord ) += vFaceTangent;
			}
		}

		// Eventually we normalize the normals and tangents
		for ( auto vtx : points )
		{
			Coords3r value;
			Vertex::GetNormal( vtx, value );
			point::normalise( value );
			Vertex::GetTangent( vtx, value );
			point::normalise( value );
		}
	}

	void SubmeshUtils::ComputeNormals( Submesh & p_submesh
		, Face const & p_face )
	{
		auto & points = p_submesh.GetPoints();
		BufferElementGroupSPtr pVtx1, pVtx2, pVtx3;
		Point3r vec2m1;
		Point3r vec3m1;
		Point3r vFaceNormal;
		Point3r pt1;
		Point3r pt2;
		Point3r pt3;
		pVtx1 = points[p_face[0]];
		pVtx2 = points[p_face[1]];
		pVtx3 = points[p_face[2]];
		Vertex::GetPosition( pVtx1, pt1 );
		Vertex::GetPosition( pVtx2, pt2 );
		Vertex::GetPosition( pVtx3, pt3 );
		vec2m1 = pt2 - pt1;
		vec3m1 = pt3 - pt1;
		vFaceNormal = point::get_normalised( vec2m1 ^ vec3m1 );
		Vertex::SetNormal( pVtx1, vFaceNormal );
		Vertex::SetNormal( pVtx2, vFaceNormal );
		Vertex::SetNormal( pVtx3, vFaceNormal );
		ComputeTangents( p_submesh, p_face );
	}

	void SubmeshUtils::ComputeTangents( Submesh & p_submesh
		, Face const & p_face )
	{
		auto & points = p_submesh.GetPoints();
		BufferElementGroupSPtr pVtx1, pVtx2, pVtx3;
		Point3r vec2m1;
		Point3r vec3m1;
		Point3r vFaceTangent;
		Point3r tex2m1;
		Point3r tex3m1;
		Point3r uv1;
		Point3r uv2;
		Point3r uv3;
		Point3r pt1;
		Point3r pt2;
		Point3r pt3;
		pVtx1 = points[p_face[0]];
		pVtx2 = points[p_face[1]];
		pVtx3 = points[p_face[2]];
		Vertex::GetPosition( pVtx1, pt1 );
		Vertex::GetPosition( pVtx2, pt2 );
		Vertex::GetPosition( pVtx3, pt3 );
		Vertex::GetTexCoord( pVtx1, uv1 );
		Vertex::GetTexCoord( pVtx2, uv2 );
		Vertex::GetTexCoord( pVtx3, uv3 );
		vec2m1 = pt2 - pt1;
		vec3m1 = pt3 - pt1;
		tex2m1 = uv2 - uv1;
		tex3m1 = uv3 - uv1;
		vFaceTangent = point::get_normalised( ( vec2m1 * tex3m1[1] ) - ( vec3m1 * tex2m1[1] ) );
		Vertex::SetTangent( pVtx1, vFaceTangent );
		Vertex::SetTangent( pVtx2, vFaceTangent );
		Vertex::SetTangent( pVtx3, vFaceTangent );
	}

	void SubmeshUtils::ComputeTangentsFromNormals( Submesh & p_submesh )
	{
		auto & points = p_submesh.GetPoints();
		auto & faces = p_submesh.GetFaces();
		Point3rArray arrayTangents( points.size() );

		// Pour chaque vertex, on stocke la somme des tangentes qui peuvent lui être affectées
		for ( auto const & face : faces )
		{
			BufferElementGroupSPtr	pVtx1 = points[face[0]];
			BufferElementGroupSPtr	pVtx2 = points[face[1]];
			BufferElementGroupSPtr	pVtx3 = points[face[2]];
			Point3r pt1;
			Point3r pt2;
			Point3r pt3;
			Point3r uv1;
			Point3r uv2;
			Point3r uv3;
			Vertex::GetPosition( pVtx1, pt1 );
			Vertex::GetPosition( pVtx2, pt2 );
			Vertex::GetPosition( pVtx3, pt3 );
			Vertex::GetTexCoord( pVtx1, uv1 );
			Vertex::GetTexCoord( pVtx2, uv2 );
			Vertex::GetTexCoord( pVtx3, uv3 );
			Point3r vec2m1 = pt2 - pt1;
			Point3r vec3m1 = pt3 - pt1;
			Point3r vec3m2 = pt3 - pt2;
			Point3r tex2m1 = uv2 - uv1;
			Point3r tex3m1 = uv3 - uv1;
			// Calculates the triangle's area.
			real rDirCorrection = tex2m1[0] * tex3m1[1] - tex2m1[1] * tex3m1[0];
			Point3r vFaceTangent;

			if ( rDirCorrection )
			{
				rDirCorrection = 1 / rDirCorrection;
				// Calculates the face tangent to the current triangle.
				vFaceTangent[0] = rDirCorrection * ( ( vec2m1[0] * tex3m1[1] ) + ( vec3m1[0] * -tex2m1[1] ) );
				vFaceTangent[1] = rDirCorrection * ( ( vec2m1[1] * tex3m1[1] ) + ( vec3m1[1] * -tex2m1[1] ) );
				vFaceTangent[2] = rDirCorrection * ( ( vec2m1[2] * tex3m1[1] ) + ( vec3m1[2] * -tex2m1[1] ) );
			}

			arrayTangents[face[0]] += vFaceTangent;
			arrayTangents[face[1]] += vFaceTangent;
			arrayTangents[face[2]] += vFaceTangent;
		}

		uint32_t i = 0;
		//On effectue la moyennes des tangentes
		for ( auto & value : arrayTangents )
		{
			Point3r normal;
			Vertex::GetNormal( points[i], normal );
			Point3r tangent = point::get_normalised( value );
			tangent -= normal * point::dot( tangent, normal );
			Point3r bitangent = normal ^ tangent;
			Vertex::SetTangent( points[i], tangent );
			Vertex::SetBitangent( points[i], bitangent );
			i++;
		}
	}

	void SubmeshUtils::ComputeTangentsFromBitangents( Submesh & p_submesh )
	{
		for ( auto & point : p_submesh.GetPoints() )
		{
			Point3r normal;
			Point3r bitangent;
			Vertex::GetNormal( point, normal );
			Vertex::GetBitangent( point, bitangent );
			Point3r tangent = normal ^ bitangent;
			Vertex::SetTangent( point, tangent );
		}
	}

	void SubmeshUtils::ComputeBitangents( Submesh & p_submesh )
	{
		for ( auto & point : p_submesh.GetPoints() )
		{
			Point3r normal;
			Point3r tangent;
			Vertex::GetNormal( point, normal );
			Vertex::GetTangent( point, tangent );
			Point3r bitangent = tangent ^ normal;
			Vertex::SetBitangent( point, bitangent );
		}
	}
}
