#include "SubmeshUtils.hpp"

#include "Submesh.hpp"
#include "Vertex.hpp"

using namespace Castor;

namespace Castor3D
{
	void SubmeshUtils::ComputeFacesFromPolygonVertex( Submesh & p_submesh )
	{
		auto & l_points = p_submesh.GetPoints();

		if ( l_points.size() )
		{
			BufferElementGroupSPtr l_v1 = l_points[0];
			BufferElementGroupSPtr l_v2 = l_points[1];
			BufferElementGroupSPtr l_v3 = l_points[2];
			p_submesh.AddFace( 0, 1, 2 );
			Vertex::SetTexCoord( l_v1, 0.0, 0.0 );
			Vertex::SetTexCoord( l_v2, 0.0, 0.0 );
			Vertex::SetTexCoord( l_v3, 0.0, 0.0 );

			for ( uint32_t i = 2; i < uint32_t( l_points.size() - 1 ); i++ )
			{
				l_v2 = l_points[i];
				l_v3 = l_points[i + 1];
				p_submesh.AddFace( 0, i, i + 1 );
				Vertex::SetTexCoord( l_v2, 0.0, 0.0 );
				Vertex::SetTexCoord( l_v3, 0.0, 0.0 );
			}
		}
	}

	void SubmeshUtils::ComputeNormals( Submesh & p_submesh
		, bool p_reverted )
	{
		auto & l_points = p_submesh.GetPoints();
		auto & l_faces = p_submesh.GetFaces();

		Point3r l_vec2m1;
		Point3r l_vec3m1;
		Point3r l_tex2m1;
		Point3r l_tex3m1;
		Point3r l_pt1;
		Point3r l_pt2;
		Point3r l_pt3;
		Point3r l_uv1;
		Point3r l_uv2;
		Point3r l_uv3;
		BufferElementGroupSPtr l_pVtx1;
		BufferElementGroupSPtr l_pVtx2;
		BufferElementGroupSPtr l_pVtx3;
		Point3r l_vFaceNormal;
		Point3r l_vFaceTangent;

		// First we flush normals and tangents
		for ( auto & l_pt : l_points )
		{
			Vertex::SetNormal( l_pt, l_pt1 );
			Vertex::SetTangent( l_pt, l_pt1 );
		}

		Coords3r l_coord;

		// Then we compute normals and tangents
		if ( p_reverted )
		{
			for ( auto const & l_face : l_faces )
			{
				l_pVtx1 = l_points[l_face[0]];
				l_pVtx2 = l_points[l_face[1]];
				l_pVtx3 = l_points[l_face[2]];
				Vertex::GetPosition( l_pVtx1, l_pt1 );
				Vertex::GetPosition( l_pVtx2, l_pt2 );
				Vertex::GetPosition( l_pVtx3, l_pt3 );
				Vertex::GetTexCoord( l_pVtx1, l_uv1 );
				Vertex::GetTexCoord( l_pVtx2, l_uv2 );
				Vertex::GetTexCoord( l_pVtx3, l_uv3 );
				l_vec2m1 = l_pt2 - l_pt1;
				l_vec3m1 = l_pt3 - l_pt1;
				l_tex2m1 = l_uv2 - l_uv1;
				l_tex3m1 = l_uv3 - l_uv1;
				l_vFaceNormal = -( l_vec3m1 ^ l_vec2m1 );
				l_vFaceTangent = ( l_vec2m1 * l_tex3m1[1] ) - ( l_vec3m1 * l_tex2m1[1] );
				Vertex::GetNormal( l_pVtx1, l_coord ) += l_vFaceNormal;
				Vertex::GetNormal( l_pVtx2, l_coord ) += l_vFaceNormal;
				Vertex::GetNormal( l_pVtx3, l_coord ) += l_vFaceNormal;
				Vertex::GetTangent( l_pVtx1, l_coord ) += l_vFaceTangent;
				Vertex::GetTangent( l_pVtx2, l_coord ) += l_vFaceTangent;
				Vertex::GetTangent( l_pVtx3, l_coord ) += l_vFaceTangent;
			}
		}
		else
		{
			for ( auto const & l_face : l_faces )
			{
				l_pVtx1 = l_points[l_face[0]];
				l_pVtx2 = l_points[l_face[1]];
				l_pVtx3 = l_points[l_face[2]];
				Vertex::GetPosition( l_pVtx1, l_pt1 );
				Vertex::GetPosition( l_pVtx2, l_pt2 );
				Vertex::GetPosition( l_pVtx3, l_pt3 );
				Vertex::GetTexCoord( l_pVtx1, l_uv1 );
				Vertex::GetTexCoord( l_pVtx2, l_uv2 );
				Vertex::GetTexCoord( l_pVtx3, l_uv3 );
				l_vec2m1 = l_pt2 - l_pt1;
				l_vec3m1 = l_pt3 - l_pt1;
				l_tex2m1 = l_uv2 - l_uv1;
				l_tex3m1 = l_uv3 - l_uv1;
				l_vFaceNormal = l_vec3m1 ^ l_vec2m1;
				l_vFaceTangent = ( l_vec3m1 * l_tex2m1[1] ) - ( l_vec2m1 * l_tex3m1[1] );
				Vertex::GetNormal( l_pVtx1, l_coord ) += l_vFaceNormal;
				Vertex::GetNormal( l_pVtx2, l_coord ) += l_vFaceNormal;
				Vertex::GetNormal( l_pVtx3, l_coord ) += l_vFaceNormal;
				Vertex::GetTangent( l_pVtx1, l_coord ) += l_vFaceTangent;
				Vertex::GetTangent( l_pVtx2, l_coord ) += l_vFaceTangent;
				Vertex::GetTangent( l_pVtx3, l_coord ) += l_vFaceTangent;
			}
		}

		// Eventually we normalize the normals and tangents
		for ( auto l_vtx : l_points )
		{
			Coords3r l_value;
			Vertex::GetNormal( l_vtx, l_value );
			point::normalise( l_value );
			Vertex::GetTangent( l_vtx, l_value );
			point::normalise( l_value );
		}
	}

	void SubmeshUtils::ComputeNormals( Submesh & p_submesh
		, Face const & p_face )
	{
		auto & l_points = p_submesh.GetPoints();
		BufferElementGroupSPtr l_pVtx1, l_pVtx2, l_pVtx3;
		Point3r l_vec2m1;
		Point3r l_vec3m1;
		Point3r l_vFaceNormal;
		Point3r l_pt1;
		Point3r l_pt2;
		Point3r l_pt3;
		l_pVtx1 = l_points[p_face[0]];
		l_pVtx2 = l_points[p_face[1]];
		l_pVtx3 = l_points[p_face[2]];
		Vertex::GetPosition( l_pVtx1, l_pt1 );
		Vertex::GetPosition( l_pVtx2, l_pt2 );
		Vertex::GetPosition( l_pVtx3, l_pt3 );
		l_vec2m1 = l_pt2 - l_pt1;
		l_vec3m1 = l_pt3 - l_pt1;
		l_vFaceNormal = point::get_normalised( l_vec2m1 ^ l_vec3m1 );
		Vertex::SetNormal( l_pVtx1, l_vFaceNormal );
		Vertex::SetNormal( l_pVtx2, l_vFaceNormal );
		Vertex::SetNormal( l_pVtx3, l_vFaceNormal );
		ComputeTangents( p_submesh, p_face );
	}

	void SubmeshUtils::ComputeTangents( Submesh & p_submesh
		, Face const & p_face )
	{
		auto & l_points = p_submesh.GetPoints();
		BufferElementGroupSPtr l_pVtx1, l_pVtx2, l_pVtx3;
		Point3r l_vec2m1;
		Point3r l_vec3m1;
		Point3r l_vFaceTangent;
		Point3r l_tex2m1;
		Point3r l_tex3m1;
		Point3r l_uv1;
		Point3r l_uv2;
		Point3r l_uv3;
		Point3r l_pt1;
		Point3r l_pt2;
		Point3r l_pt3;
		l_pVtx1 = l_points[p_face[0]];
		l_pVtx2 = l_points[p_face[1]];
		l_pVtx3 = l_points[p_face[2]];
		Vertex::GetPosition( l_pVtx1, l_pt1 );
		Vertex::GetPosition( l_pVtx2, l_pt2 );
		Vertex::GetPosition( l_pVtx3, l_pt3 );
		Vertex::GetTexCoord( l_pVtx1, l_uv1 );
		Vertex::GetTexCoord( l_pVtx2, l_uv2 );
		Vertex::GetTexCoord( l_pVtx3, l_uv3 );
		l_vec2m1 = l_pt2 - l_pt1;
		l_vec3m1 = l_pt3 - l_pt1;
		l_tex2m1 = l_uv2 - l_uv1;
		l_tex3m1 = l_uv3 - l_uv1;
		l_vFaceTangent = point::get_normalised( ( l_vec2m1 * l_tex3m1[1] ) - ( l_vec3m1 * l_tex2m1[1] ) );
		Vertex::SetTangent( l_pVtx1, l_vFaceTangent );
		Vertex::SetTangent( l_pVtx2, l_vFaceTangent );
		Vertex::SetTangent( l_pVtx3, l_vFaceTangent );
	}

	void SubmeshUtils::ComputeTangentsFromNormals( Submesh & p_submesh )
	{
		auto & l_points = p_submesh.GetPoints();
		auto & l_faces = p_submesh.GetFaces();
		Point3rArray l_arrayTangents( l_points.size() );

		// Pour chaque vertex, on stocke la somme des tangentes qui peuvent lui être affectées
		for ( auto const & l_face : l_faces )
		{
			BufferElementGroupSPtr	l_pVtx1 = l_points[l_face[0]];
			BufferElementGroupSPtr	l_pVtx2 = l_points[l_face[1]];
			BufferElementGroupSPtr	l_pVtx3 = l_points[l_face[2]];
			Point3r l_pt1;
			Point3r l_pt2;
			Point3r l_pt3;
			Point3r l_uv1;
			Point3r l_uv2;
			Point3r l_uv3;
			Vertex::GetPosition( l_pVtx1, l_pt1 );
			Vertex::GetPosition( l_pVtx2, l_pt2 );
			Vertex::GetPosition( l_pVtx3, l_pt3 );
			Vertex::GetTexCoord( l_pVtx1, l_uv1 );
			Vertex::GetTexCoord( l_pVtx2, l_uv2 );
			Vertex::GetTexCoord( l_pVtx3, l_uv3 );
			Point3r l_vec2m1 = l_pt2 - l_pt1;
			Point3r l_vec3m1 = l_pt3 - l_pt1;
			Point3r l_vec3m2 = l_pt3 - l_pt2;
			Point3r l_tex2m1 = l_uv2 - l_uv1;
			Point3r l_tex3m1 = l_uv3 - l_uv1;
			// Calculates the triangle's area.
			real l_rDirCorrection = l_tex2m1[0] * l_tex3m1[1] - l_tex2m1[1] * l_tex3m1[0];
			Point3r l_vFaceTangent;

			if ( l_rDirCorrection )
			{
				l_rDirCorrection = 1 / l_rDirCorrection;
				// Calculates the face tangent to the current triangle.
				l_vFaceTangent[0] = l_rDirCorrection * ( ( l_vec2m1[0] * l_tex3m1[1] ) + ( l_vec3m1[0] * -l_tex2m1[1] ) );
				l_vFaceTangent[1] = l_rDirCorrection * ( ( l_vec2m1[1] * l_tex3m1[1] ) + ( l_vec3m1[1] * -l_tex2m1[1] ) );
				l_vFaceTangent[2] = l_rDirCorrection * ( ( l_vec2m1[2] * l_tex3m1[1] ) + ( l_vec3m1[2] * -l_tex2m1[1] ) );
			}

			l_arrayTangents[l_face[0]] += l_vFaceTangent;
			l_arrayTangents[l_face[1]] += l_vFaceTangent;
			l_arrayTangents[l_face[2]] += l_vFaceTangent;
		}

		uint32_t i = 0;
		//On effectue la moyennes des tangentes
		for ( auto & l_value : l_arrayTangents )
		{
			Point3r l_normal;
			Vertex::GetNormal( l_points[i], l_normal );
			Point3r l_tangent = point::get_normalised( l_value );
			l_tangent -= l_normal * point::dot( l_tangent, l_normal );
			Point3r l_bitangent = l_normal ^ l_tangent;
			Vertex::SetTangent( l_points[i], l_tangent );
			Vertex::SetBitangent( l_points[i], l_bitangent );
			i++;
		}
	}

	void SubmeshUtils::ComputeTangentsFromBitangents( Submesh & p_submesh )
	{
		for ( auto & l_point : p_submesh.GetPoints() )
		{
			Point3r l_normal;
			Point3r l_bitangent;
			Vertex::GetNormal( l_point, l_normal );
			Vertex::GetBitangent( l_point, l_bitangent );
			Point3r l_tangent = l_normal ^ l_bitangent;
			Vertex::SetTangent( l_point, l_tangent );
		}
	}

	void SubmeshUtils::ComputeBitangents( Submesh & p_submesh )
	{
		for ( auto & l_point : p_submesh.GetPoints() )
		{
			Point3r l_normal;
			Point3r l_tangent;
			Vertex::GetNormal( l_point, l_normal );
			Vertex::GetTangent( l_point, l_tangent );
			Point3r l_bitangent = l_tangent ^ l_normal;
			Vertex::SetBitangent( l_point, l_bitangent );
		}
	}
}
