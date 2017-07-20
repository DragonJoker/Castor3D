#include "PlyImporter.hpp"

#include <Engine.hpp>

#include <Event/Frame/InitialiseEvent.hpp>
#include <Cache/CacheView.hpp>
#include <Cache/CameraCache.hpp>
#include <Cache/GeometryCache.hpp>
#include <Cache/MaterialCache.hpp>
#include <Cache/MeshCache.hpp>
#include <Cache/SceneNodeCache.hpp>
#include <Cache/SceneCache.hpp>
#include <Material/Material.hpp>
#include <Material/Pass.hpp>
#include <Mesh/Face.hpp>
#include <Mesh/Submesh.hpp>
#include <Mesh/Vertex.hpp>
#include <Plugin/Plugin.hpp>
#include <Miscellaneous/Version.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/Viewport.hpp>
#include <Scene/Geometry.hpp>
#include <Scene/Scene.hpp>
#include <Texture/TextureUnit.hpp>

using namespace Castor3D;
using namespace Castor;

namespace C3dPly
{
	PlyImporter::PlyImporter( Engine & p_engine )
		: Importer( p_engine )
	{
	}

	ImporterUPtr PlyImporter::Create( Engine & p_engine )
	{
		return std::make_unique< PlyImporter >( p_engine );
	}

	bool PlyImporter::DoImportScene( Scene & p_scene )
	{
		auto mesh = p_scene.GetMeshCache().Add( cuT( "Mesh_PLY" ) );
		bool result = DoImportMesh( *mesh );

		if ( result )
		{
			SceneNodeSPtr node = p_scene.GetSceneNodeCache().Add( mesh->GetName(), p_scene.GetObjectRootNode() );
			GeometrySPtr geometry = p_scene.GetGeometryCache().Add( mesh->GetName(), node, nullptr );
			geometry->SetMesh( mesh );
			m_geometries.insert( { geometry->GetName(), geometry } );
		}

		return result;
	}

	bool PlyImporter::DoImportMesh( Mesh & p_mesh )
	{
		bool result{ false };
		UIntArray faces;
		RealArray sizes;
		String name = m_fileName.GetFileName();
		String meshName = name.substr( 0, name.find_last_of( '.' ) );
		String materialName = meshName;
		std::ifstream isFile;
		isFile.open( string::string_cast< char >( m_fileName ).c_str(), std::ios::in );
		std::string strLine;
		std::istringstream ssToken;
		String::size_type stIndex;
		int iNbProperties = 0;
		VertexSPtr pVertex;
		Coords3r ptNml;
		Coords2r ptTex;
		SubmeshSPtr submesh = p_mesh.CreateSubmesh();
		MaterialSPtr pMaterial = p_mesh.GetScene()->GetMaterialView().Find( materialName );

		if ( !pMaterial )
		{
			pMaterial = p_mesh.GetScene()->GetMaterialView().Add( materialName, MaterialType::eLegacy );
			pMaterial->CreatePass();
		}

		pMaterial->GetPass( 0 )->SetTwoSided( true );
		submesh->SetDefaultMaterial( pMaterial );
		// Parsing the ply identification line
		std::getline( isFile, strLine );

		if ( strLine == "ply" )
		{
			// Parsing the format specification line
			std::getline( isFile, strLine );

			if ( strLine == "format ascii 1.0" )
			{
				int iNbVertex = 0;
				int iNbFaces = 0;

				// Parsing number of vertices
				while ( isFile.good() )
				{
					std::getline( isFile, strLine );

					if ( ( stIndex = strLine.find( "element vertex " ) ) == std::string::npos )
					{
						continue;
					}
					else
					{
						ssToken.str( strLine.substr( std::string( "element vertex " ).length() ) );
						ssToken >> iNbVertex;
						ssToken.clear( std::istringstream::goodbit );
						Logger::LogInfo( StringStream() << cuT( "Vertices: " ) << iNbVertex );
						break;
					}
				}

				// Parsing number of vertex properties
				while ( isFile.good() )
				{
					std::getline( isFile, strLine );

					if ( ( stIndex = strLine.find( "property " ) ) != std::string::npos )
					{
						iNbProperties++;
					}
					else
					{
						isFile.seekg( -isFile.gcount() ); // Unget last line
						Logger::LogDebug( StringStream() << cuT( "Vertex properties: " ) << iNbProperties );
						break;
					}
				}

				// Parsing number of triangles
				while ( isFile.good() )
				{
					std::getline( isFile, strLine );

					if ( ( stIndex = strLine.find( "element face " ) ) == std::string::npos )
					{
						continue;
					}
					else
					{
						ssToken.str( strLine.substr( std::string( "element face " ).size() ) );
						ssToken >> iNbFaces;
						ssToken.clear( std::istringstream::goodbit );
						Logger::LogInfo( StringStream() << cuT( "Triangles: " ) << iNbFaces );
						break;
					}
				}

				// Parsing end of the header
				while ( isFile.good() )
				{
					std::getline( isFile, strLine );

					if ( ( stIndex = strLine.find( "end_header" ) ) == std::string::npos )
					{
						continue;
					}
					else
					{
						break;
					}
				}

				std::vector< InterleavedVertex > vertices{ size_t( iNbVertex ) };

				if ( iNbProperties >= 8 )
				{
					// Parsing vertices : position + normal + texture
					for ( auto & vertex : vertices )
					{
						std::getline( isFile, strLine );
						ssToken.str( strLine );
						ssToken >> vertex.m_pos[0] >> vertex.m_pos[1] >> vertex.m_pos[2];
						ssToken >> vertex.m_nml[0] >> vertex.m_nml[1] >> vertex.m_nml[2];
						ssToken >> vertex.m_tex[0] >> vertex.m_tex[1];
						ssToken.clear( std::istringstream::goodbit );
					}
				}
				else if ( iNbProperties >= 6 )
				{
					// Parsing vertices : position + normal
					for ( auto & vertex : vertices )
					{
						std::getline( isFile, strLine );
						ssToken.str( strLine );
						ssToken >> vertex.m_pos[0] >> vertex.m_pos[1] >> vertex.m_pos[2];
						ssToken >> vertex.m_nml[0] >> vertex.m_nml[1] >> vertex.m_nml[2];
						ssToken.clear( std::istringstream::goodbit );
					}
				}
				else
				{
					// Parsing vertices : position
					for ( auto & vertex : vertices )
					{
						std::getline( isFile, strLine );
						ssToken.str( strLine );
						ssToken >> vertex.m_pos[0] >> vertex.m_pos[1] >> vertex.m_pos[2];
						ssToken.clear( std::istringstream::goodbit );
					}
				}

				submesh->AddPoints( vertices );
				// Parsing triangles
				FaceSPtr pFace;
				std::vector< FaceIndices > faces( iNbFaces );
				FaceIndices * pFaces = &faces[0];

				for ( int i = 0; i < iNbFaces; i++ )
				{
					std::getline( isFile, strLine );
					ssToken.str( strLine );
					ssToken >> iNbVertex;

					if ( iNbVertex >= 3 )
					{
						ssToken >> pFaces->m_index[0] >> pFaces->m_index[1] >> pFaces->m_index[2];
						pFaces++;
					}

					ssToken.clear( std::istringstream::goodbit );
				}

				submesh->AddFaceGroup( faces );
			}

			result = true;
		}

		submesh->ComputeContainers();

		if ( iNbProperties < 6 )
		{
			submesh->ComputeNormals( false );
		}
		else
		{
			submesh->ComputeTangentsFromNormals();
		}

		isFile.close();
		return result;
	}
}
