#include "Castor3D/Model/Mesh/Submesh/SubmeshUtils.hpp"

#include "Castor3D/Model/Mesh/Submesh/Component/TriFaceMapping.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Vertex.hpp"

namespace castor3d
{
	void SubmeshUtils::computeFacesFromPolygonVertex( castor::Point3fArray & texcoords
		, TriFaceMapping & triFace )
	{
		if ( !texcoords.empty() )
		{
			auto * v1 = &texcoords[0];
			auto * v2 = &texcoords[1];
			auto * v3 = &texcoords[2];
			triFace.addFace( 0, 1, 2 );
			*v1 = castor::Point3f{ 0.0, 0.0, 0.0 };
			*v2 = castor::Point3f{ 0.0, 0.0, 0.0 };
			*v3 = castor::Point3f{ 0.0, 0.0, 0.0 };

			for ( uint32_t i = 2; i < uint32_t( texcoords.size() - 1 ); i++ )
			{
				v2 = &texcoords[i];
				v3 = &texcoords[i + 1];
				triFace.addFace( 0, i, i + 1 );
				*v2 = castor::Point3f{ 0.0, 0.0, 0.0 };
				*v3 = castor::Point3f{ 0.0, 0.0, 0.0 };
			}
		}
	}

	void SubmeshUtils::computeNormals( castor::Point3fArray const & positions
		, castor::Point3fArray const & texcoords
		, castor::Point3fArray & normals
		, castor::Point3fArray & tangents
		, FaceArray const & faces
		, bool reverted )
	{
		castor::Point3f pt0;

		// First we flush normals and tangents
		for ( auto & pt : normals )
		{
			pt = pt0;
		}

		for ( auto & pt : tangents )
		{
			pt = pt0;
		}

		castor::Coords3f coord;

		// Then we compute normals and tangents
		if ( reverted )
		{
			if ( texcoords.empty() )
			{
				for ( auto const & face : faces )
				{
					auto & pos1 = positions[face[0]];
					auto & pos2 = positions[face[2]];
					auto & pos3 = positions[face[1]];
					auto & nml1 = normals[face[0]];
					auto & nml2 = normals[face[2]];
					auto & nml3 = normals[face[1]];
					auto const vec2m1 = pos2 - pos1;
					auto const vec3m1 = pos3 - pos1;
					auto const faceNormal = -castor::point::cross( vec3m1, vec2m1 );
					nml1 += faceNormal;
					nml2 += faceNormal;
					nml3 += faceNormal;
				}
			}
			else
			{
				for ( auto const & face : faces )
				{
					auto & pos1 = positions[face[0]];
					auto & pos2 = positions[face[2]];
					auto & pos3 = positions[face[1]];
					auto & nml1 = normals[face[0]];
					auto & nml2 = normals[face[2]];
					auto & nml3 = normals[face[1]];
					auto const vec2m1 = pos2 - pos1;
					auto const vec3m1 = pos3 - pos1;
					auto const faceNormal = -castor::point::cross( vec3m1, vec2m1 );
					nml1 += faceNormal;
					nml2 += faceNormal;
					nml3 += faceNormal;

					auto & tex1 = texcoords[face[0]];
					auto & tex2 = texcoords[face[2]];
					auto & tex3 = texcoords[face[1]];
					auto & tan1 = tangents[face[0]];
					auto & tan2 = tangents[face[2]];
					auto & tan3 = tangents[face[1]];
					auto const tex2m1 = tex2 - tex1;
					auto const tex3m1 = tex3 - tex1;
					auto const faceTangent = ( vec3m1 * tex2m1[1] ) - ( vec2m1 * tex3m1[1] );
					tan1 += faceTangent;
					tan2 += faceTangent;
					tan3 += faceTangent;
				}
			}
		}
		else
		{
			if ( texcoords.empty() )
			{
				for ( auto const & face : faces )
				{
					auto & pos1 = positions[face[0]];
					auto & pos2 = positions[face[1]];
					auto & pos3 = positions[face[2]];
					auto & nml1 = normals[face[0]];
					auto & nml2 = normals[face[1]];
					auto & nml3 = normals[face[2]];
					auto const vec2m1 = pos2 - pos1;
					auto const vec3m1 = pos3 - pos1;
					auto const faceNormal = -castor::point::cross( vec3m1, vec2m1 );
					nml1 += faceNormal;
					nml2 += faceNormal;
					nml3 += faceNormal;
				}
			}
			else
			{
				for ( auto const & face : faces )
				{
					auto & pos1 = positions[face[0]];
					auto & pos2 = positions[face[1]];
					auto & pos3 = positions[face[2]];
					auto & nml1 = normals[face[0]];
					auto & nml2 = normals[face[1]];
					auto & nml3 = normals[face[2]];
					auto const vec2m1 = pos2 - pos1;
					auto const vec3m1 = pos3 - pos1;
					auto const faceNormal = -castor::point::cross( vec3m1, vec2m1 );
					nml1 += faceNormal;
					nml2 += faceNormal;
					nml3 += faceNormal;

					auto & tex1 = texcoords[face[0]];
					auto & tex2 = texcoords[face[1]];
					auto & tex3 = texcoords[face[2]];
					auto & tan1 = tangents[face[0]];
					auto & tan2 = tangents[face[1]];
					auto & tan3 = tangents[face[2]];
					auto const tex2m1 = tex2 - tex1;
					auto const tex3m1 = tex3 - tex1;
					auto const faceTangent = ( vec3m1 * tex2m1[1] ) - ( vec2m1 * tex3m1[1] );
					tan1 += faceTangent;
					tan2 += faceTangent;
					tan3 += faceTangent;
				}
			}
		}

		// Eventually we normalize the normals and tangents
		for ( auto & nml : normals )
		{
			castor::point::normalise( nml );
		}

		if ( !texcoords.empty() )
		{
			for ( auto & tan : tangents )
			{
				castor::point::normalise( tan );
			}
		}
	}

	void SubmeshUtils::computeTangentsFromNormals( castor::Point3fArray const & positions
		, castor::Point3fArray const & texcoords
		, castor::Point3fArray const & normals
		, castor::Point3fArray & tangents
		, FaceArray const & faces )
	{
		if ( texcoords.empty() )
		{
			return;
		}

		tangents.resize( positions.size() );

		// Pour chaque vertex, on stocke la somme des tangentes qui peuvent lui être affectées
		for ( auto const & face : faces )
		{
			auto & pos1 = positions[face[0]];
			auto & pos2 = positions[face[1]];
			auto & pos3 = positions[face[2]];
			auto & tex1 = texcoords[face[0]];
			auto & tex2 = texcoords[face[1]];
			auto & tex3 = texcoords[face[2]];
			auto const vec2m1 = pos2 - pos1;
			auto const vec3m1 = pos3 - pos1;
			auto const tex2m1 = tex2 - tex1;
			auto const tex3m1 = tex3 - tex1;

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

			tangents[face[0]] += faceTangent;
			tangents[face[1]] += faceTangent;
			tangents[face[2]] += faceTangent;
		}

		uint32_t i = 0;
		// Average tangents
		for ( auto & value : tangents )
		{
			auto & nml = normals[i];
			castor::Point3f tangent = castor::point::getNormalised( castor::Point3f{ value } );
			tangent -= nml * castor::point::dot( tangent, nml );
			value = tangent;
			i++;
		}
	}

	void SubmeshUtils::computeNormals( castor::Point3fArray const & positions
		, castor::Point3fArray const & texcoords
		, castor::Point3fArray & normals
		, castor::Point3fArray & tangents
		, Face const & face )
	{
		auto & vtx1 = positions[face[0]];
		auto & vtx2 = positions[face[1]];
		auto & vtx3 = positions[face[2]];
		auto & nml1 = normals[face[0]];
		auto & nml2 = normals[face[1]];
		auto & nml3 = normals[face[2]];
		auto const vec2m1 = vtx2 - vtx1;
		auto const vec3m1 = vtx3 - vtx1;
		auto const faceNormal = castor::point::getNormalised( -castor::point::cross( vec3m1, vec2m1 ) );
		nml1 += faceNormal;
		nml2 += faceNormal;
		nml3 += faceNormal;
		computeTangents( positions, texcoords, tangents, face );
	}

	void SubmeshUtils::computeTangents( castor::Point3fArray const & positions
		, castor::Point3fArray const & texcoords
		, castor::Point3fArray & tangents
		, Face const & face )
	{
		if ( texcoords.empty() )
		{
			return;
		}

		auto & vtx1 = positions[face[0]];
		auto & vtx2 = positions[face[1]];
		auto & vtx3 = positions[face[2]];
		auto & tan1 = tangents[face[0]];
		auto & tan2 = tangents[face[1]];
		auto & tan3 = tangents[face[2]];
		auto & tex1 = texcoords[face[0]];
		auto & tex2 = texcoords[face[1]];
		auto & tex3 = texcoords[face[2]];
		auto const vec2m1 = vtx2 - vtx1;
		auto const vec3m1 = vtx3 - vtx1;
		auto const tex2m1 = tex2 - tex1;
		auto const tex3m1 = tex3 - tex1;
		auto const faceTangent = castor::point::getNormalised( ( vec2m1 * tex3m1[1] ) - ( vec3m1 * tex2m1[1] ) );
		tan1 += faceTangent;
		tan2 += faceTangent;
		tan3 += faceTangent;
	}
}
