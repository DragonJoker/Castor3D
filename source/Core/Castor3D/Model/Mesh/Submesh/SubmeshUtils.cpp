#include "Castor3D/Model/Mesh/Submesh/SubmeshUtils.hpp"

#include "Castor3D/Model/Mesh/Submesh/Component/TriFaceMapping.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Vertex.hpp"

#include <mikktspace.h>

namespace castor3d
{
	namespace mikkt
	{
		struct SubmeshData
		{
			castor::Point3fArray const * positions{};
			castor::Point3fArray const * texcoords{};
			castor::Point3fArray const * normals{};
			FaceArray const * faces{};
			castor::Point4fArray * tangents{};
		};

		static int getNumFaces( SMikkTSpaceContext const * pContext )
		{
			auto data = reinterpret_cast< SubmeshData * >( pContext->m_pUserData );
			return int( data->faces->size() );
		}

		static int getNumVerticesOfFace( SMikkTSpaceContext const * pContext, int const iFace )
		{
			return 3;
		}

		static void getPosition( SMikkTSpaceContext const * pContext
			, float fvPosOut[]
			, int const iFace
			, int const iVert )
		{
			auto data = reinterpret_cast< SubmeshData * >( pContext->m_pUserData );
			auto & face = ( *data->faces )[uint32_t( iFace )];
			auto & pos = ( *data->positions )[face[uint32_t( iVert )]];
			fvPosOut[0] = pos->x;
			fvPosOut[1] = pos->y;
			fvPosOut[2] = pos->z;
		}

		static void getNormal( SMikkTSpaceContext const * pContext
			, float fvNormOut[]
			, int const iFace
			, int const iVert )
		{
			auto data = reinterpret_cast< SubmeshData * >( pContext->m_pUserData );
			auto & face = ( *data->faces )[uint32_t( iFace )];
			auto & nml = ( *data->normals )[face[uint32_t( iVert )]];
			fvNormOut[0] = nml->x;
			fvNormOut[1] = nml->y;
			fvNormOut[2] = nml->z;
		}

		static void getTexCoord( SMikkTSpaceContext const * pContext
			, float fvTexcOut[]
			, int const iFace
			, int const iVert )
		{
			auto data = reinterpret_cast< SubmeshData * >( pContext->m_pUserData );
			auto & face = ( *data->faces )[uint32_t( iFace )];
			auto & tex = ( *data->texcoords )[face[uint32_t( iVert )]];
			fvTexcOut[0] = tex->x;
			fvTexcOut[1] = tex->y;
		}

		static void setTSpaceBasic( SMikkTSpaceContext const * pContext
			, float const fvTangent[]
			, float const fSign
			, int const iFace
			, int const iVert)
		{
			auto data = reinterpret_cast< SubmeshData * >( pContext->m_pUserData );
			auto & face = ( *data->faces )[uint32_t( iFace )];
			auto & tan = ( *data->tangents )[face[uint32_t( iVert )]];
			tan->x = fvTangent[0];
			tan->y = fvTangent[1];
			tan->z = fvTangent[2];
			tan->w = -fSign;
		}
	}

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
		, castor::Point3fArray & normals
		, FaceArray const & faces
		, bool reverted )
	{
		static const castor::Point3f pt0{};

		// First we flush normals and tangents
		for ( auto & pt : normals )
		{
			pt = pt0;
		}

		// Then we compute normals and tangents
		if ( reverted )
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
				auto const faceNormal = castor::point::cross( vec3m1, vec2m1 );
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
				auto const faceNormal = castor::point::cross( vec3m1, vec2m1 );
				nml1 += faceNormal;
				nml2 += faceNormal;
				nml3 += faceNormal;
			}
		}

		// Eventually we normalize the normals and tangents
		for ( auto & nml : normals )
		{
			castor::point::normalise( nml );
		}
	}

	void SubmeshUtils::computeTangentsFromNormals( castor::Point3fArray const & positions
		, castor::Point3fArray const & texcoords
		, castor::Point3fArray const & normals
		, castor::Point4fArray & tangents
		, FaceArray const & faces )
	{
		if ( texcoords.empty() )
		{
			return;
		}

		tangents.resize( positions.size() );
		mikkt::SubmeshData data{ &positions
			, &texcoords
			, &normals
			, &faces
			, &tangents };
		SMikkTSpaceInterface mikktInterface{};
		mikktInterface.m_getNumFaces = &mikkt::getNumFaces;
		mikktInterface.m_getNumVerticesOfFace = &mikkt::getNumVerticesOfFace;
		mikktInterface.m_getPosition = &mikkt::getPosition;
		mikktInterface.m_getNormal = &mikkt::getNormal;
		mikktInterface.m_getTexCoord = &mikkt::getTexCoord;
		mikktInterface.m_setTSpaceBasic = &mikkt::setTSpaceBasic;
		SMikkTSpaceContext mikktContext{ &mikktInterface, &data };
		genTangSpaceDefault( &mikktContext );
	}
}
