#include "PlyImporter.hpp"

#include <CameraManager.hpp>
#include <Engine.hpp>
#include <GeometryManager.hpp>
#include <MaterialManager.hpp>
#include <MeshManager.hpp>
#include <SceneNodeManager.hpp>
#include <SceneManager.hpp>

#include <Event/Frame/InitialiseEvent.hpp>
#include <Manager/ManagerView.hpp>
#include <Material/Pass.hpp>
#include <Mesh/Face.hpp>
#include <Mesh/Submesh.hpp>
#include <Mesh/Vertex.hpp>
#include <Plugin/Plugin.hpp>
#include <Miscellaneous/Version.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/Viewport.hpp>
#include <Texture/TextureUnit.hpp>

using namespace Castor3D;
using namespace Castor;

namespace C3dPly
{
	PlyImporter::PlyImporter( Engine & p_pEngine )
		: Importer( p_pEngine )
	{
	}

	SceneSPtr PlyImporter::DoImportScene()
	{
		SceneSPtr l_scene = GetEngine()->GetSceneManager().Create( cuT( "Scene_PLY" ), *GetEngine() );
		MeshSPtr l_mesh = DoImportMesh( *l_scene );

		if ( l_mesh )
		{
			SceneNodeSPtr l_node = l_scene->GetSceneNodeManager().Create( l_mesh->GetName(), l_scene->GetObjectRootNode() );
			GeometrySPtr l_geometry = l_scene->GetGeometryManager().Create( l_mesh->GetName(), l_node );

			for ( auto && l_submesh : *l_mesh )
			{
				GetEngine()->PostEvent( MakeInitialiseEvent( *l_submesh ) );
			}

			l_geometry->SetMesh( l_mesh );
		}

		return l_scene;
	}

	MeshSPtr PlyImporter::DoImportMesh( Scene & p_scene )
	{
		UIntArray l_faces;
		RealArray l_sizes;
		String l_name = m_fileName.GetFileName();
		String l_meshName = l_name.substr( 0, l_name.find_last_of( '.' ) );
		String l_materialName = l_meshName;
		MeshSPtr l_mesh = p_scene.GetMeshView().Create( l_meshName, eMESH_TYPE_CUSTOM, l_faces, l_sizes );
		std::ifstream l_isFile;
		l_isFile.open( string::string_cast< char >( m_fileName ).c_str(), std::ios::in );
		std::string l_strLine;
		std::istringstream l_ssToken;
		String::size_type l_stIndex;
		int l_iNbProperties = 0;
		VertexSPtr l_pVertex;
		Coords3r l_ptNml;
		Coords2r l_ptTex;
		SubmeshSPtr l_pSubmesh = l_mesh->CreateSubmesh();
		MaterialSPtr l_pMaterial = p_scene.GetMaterialView().Find( l_materialName );

		if ( !l_pMaterial )
		{
			l_pMaterial = p_scene.GetMaterialView().Create( l_materialName, *GetEngine() );
			l_pMaterial->CreatePass();
		}

		l_pMaterial->GetPass( 0 )->SetTwoSided( true );
		l_pSubmesh->SetDefaultMaterial( l_pMaterial );
		// Parsing the ply identification line
		std::getline( l_isFile, l_strLine );

		if ( l_strLine == "ply" )
		{
			// Parsing the format specification line
			std::getline( l_isFile, l_strLine );

			if ( l_strLine == "format ascii 1.0" )
			{
				int l_iNbVertex = 0;
				int l_iNbFaces = 0;

				// Parsing number of vertices
				while ( l_isFile.good() )
				{
					std::getline( l_isFile, l_strLine );

					if ( ( l_stIndex = l_strLine.find( "element vertex " ) ) == std::string::npos )
					{
						continue;
					}
					else
					{
						l_ssToken.str( l_strLine.substr( std::string( "element vertex " ).length() ) );
						l_ssToken >> l_iNbVertex;
						l_ssToken.clear( std::istringstream::goodbit );
						Logger::LogInfo( StringStream() << cuT( "Vertices: " ) << l_iNbVertex );
						break;
					}
				}

				// Parsing number of vertex properties
				while ( l_isFile.good() )
				{
					std::getline( l_isFile, l_strLine );

					if ( ( l_stIndex = l_strLine.find( "property " ) ) != std::string::npos )
					{
						l_iNbProperties++;
					}
					else
					{
						l_isFile.seekg( -l_isFile.gcount() ); // Unget last line
						Logger::LogDebug( StringStream() << cuT( "Vertex properties: " ) << l_iNbProperties );
						break;
					}
				}

				// Parsing number of triangles
				while ( l_isFile.good() )
				{
					std::getline( l_isFile, l_strLine );

					if ( ( l_stIndex = l_strLine.find( "element face " ) ) == std::string::npos )
					{
						continue;
					}
					else
					{
						l_ssToken.str( l_strLine.substr( std::string( "element face " ).size() ) );
						l_ssToken >> l_iNbFaces;
						l_ssToken.clear( std::istringstream::goodbit );
						Logger::LogInfo( StringStream() << cuT( "Triangles: " ) << l_iNbFaces );
						break;
					}
				}

				// Parsing end of the header
				while ( l_isFile.good() )
				{
					std::getline( l_isFile, l_strLine );

					if ( ( l_stIndex = l_strLine.find( "end_header" ) ) == std::string::npos )
					{
						continue;
					}
					else
					{
						break;
					}
				}

				stVERTEX_GROUP l_stVertices = { 0 };
				std::vector< real > l_pVtx( l_iNbVertex * 3 );
				std::vector< real > l_pNml;
				std::vector< real > l_pTex;
				real * l_pBufVtx = &l_pVtx[0];
				l_stVertices.m_uiCount = l_iNbVertex;
				l_stVertices.m_pVtx = &l_pVtx[0];

				if ( l_iNbProperties >= 8 )
				{
					l_pNml.resize( l_iNbVertex * 3 );
					l_pTex.resize( l_iNbVertex * 3 );
					real * l_pBufNml = &l_pNml[0];
					real * l_pBufTex = &l_pTex[0];

					// Parsing vertices
					for ( int i = 0; i < l_iNbVertex; i++ )
					{
						std::getline( l_isFile, l_strLine );
						l_ssToken.str( l_strLine );
						l_ssToken >> l_pBufVtx[0] >> l_pBufVtx[1] >> l_pBufVtx[2];
						l_ssToken >> l_pBufNml[0] >> l_pBufNml[1] >> l_pBufNml[2];
						l_ssToken >> l_pBufTex[0] >> l_pBufTex[1];
						l_pBufVtx += 3;
						l_pBufNml += 3;
						l_pBufTex += 3;
						l_ssToken.clear( std::istringstream::goodbit );
					}

					l_stVertices.m_pNml = &l_pNml[0];
					l_stVertices.m_pTex = &l_pTex[0];
				}
				else if ( l_iNbProperties >= 6 )
				{
					l_pNml.resize( l_iNbVertex * 3 );
					real * l_pBufNml = &l_pNml[0];

					// Parsing vertices
					for ( int i = 0; i < l_iNbVertex; i++ )
					{
						std::getline( l_isFile, l_strLine );
						l_ssToken.str( l_strLine );
						l_ssToken >> l_pBufVtx[0] >> l_pBufVtx[1] >> l_pBufVtx[2];
						l_ssToken >> l_pBufNml[0] >> l_pBufNml[1] >> l_pBufNml[2];
						l_pBufVtx += 3;
						l_pBufNml += 3;
						l_ssToken.clear( std::istringstream::goodbit );
					}

					l_stVertices.m_pNml = &l_pNml[0];
				}
				else
				{
					// Parsing vertices
					for ( int i = 0; i < l_iNbVertex; i++ )
					{
						std::getline( l_isFile, l_strLine );
						l_ssToken.str( l_strLine );
						l_ssToken >> l_pBufVtx[0] >> l_pBufVtx[1] >> l_pBufVtx[2];
						l_pBufVtx += 3;
						l_ssToken.clear( std::istringstream::goodbit );
					}
				}

				l_pSubmesh->AddPoints( l_stVertices );
				// Parsing triangles
				FaceSPtr l_pFace;
				std::vector< stFACE_INDICES > l_faces( l_iNbFaces );
				stFACE_INDICES * l_pFaces = &l_faces[0];

				for ( int i = 0; i < l_iNbFaces; i++ )
				{
					std::getline( l_isFile, l_strLine );
					l_ssToken.str( l_strLine );
					l_ssToken >> l_iNbVertex;

					if ( l_iNbVertex >= 3 )
					{
						l_ssToken >> l_pFaces->m_uiVertexIndex[0] >> l_pFaces->m_uiVertexIndex[1] >> l_pFaces->m_uiVertexIndex[2];
						l_pFaces++;
					}

					l_ssToken.clear( std::istringstream::goodbit );
				}

				l_pSubmesh->AddFaceGroup( &l_faces[0], l_iNbFaces );
			}
		}

		l_pSubmesh->ComputeContainers();

		if ( l_iNbProperties < 6 )
		{
			l_mesh->ComputeNormals( false );
		}

		l_isFile.close();
		return l_mesh;
	}
}
