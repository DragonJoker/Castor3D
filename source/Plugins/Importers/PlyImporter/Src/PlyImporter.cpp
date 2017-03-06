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
		auto l_mesh = p_scene.GetMeshCache().Add( cuT( "Mesh_PLY" ) );
		bool l_return = DoImportMesh( *l_mesh );

		if ( l_return )
		{
			SceneNodeSPtr l_node = p_scene.GetSceneNodeCache().Add( l_mesh->GetName(), p_scene.GetObjectRootNode() );
			GeometrySPtr l_geometry = p_scene.GetGeometryCache().Add( l_mesh->GetName(), l_node, nullptr );
			l_geometry->SetMesh( l_mesh );
			m_geometries.insert( { l_geometry->GetName(), l_geometry } );
		}

		return l_return;
	}

	bool PlyImporter::DoImportMesh( Mesh & p_mesh )
	{
		bool l_return{ false };
		UIntArray l_faces;
		RealArray l_sizes;
		String l_name = m_fileName.GetFileName();
		String l_meshName = l_name.substr( 0, l_name.find_last_of( '.' ) );
		String l_materialName = l_meshName;
		std::ifstream l_isFile;
		l_isFile.open( string::string_cast< char >( m_fileName ).c_str(), std::ios::in );
		std::string l_strLine;
		std::istringstream l_ssToken;
		String::size_type l_stIndex;
		int l_iNbProperties = 0;
		VertexSPtr l_pVertex;
		Coords3r l_ptNml;
		Coords2r l_ptTex;
		SubmeshSPtr l_submesh = p_mesh.CreateSubmesh();
		MaterialSPtr l_pMaterial = p_mesh.GetScene()->GetMaterialView().Find( l_materialName );

		if ( !l_pMaterial )
		{
			l_pMaterial = p_mesh.GetScene()->GetMaterialView().Add( l_materialName, MaterialType::eLegacy );
			l_pMaterial->CreatePass();
		}

		l_pMaterial->GetPass( 0 )->SetTwoSided( true );
		l_submesh->SetDefaultMaterial( l_pMaterial );
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

				std::vector< InterleavedVertex > l_vertices{ size_t( l_iNbVertex ) };

				if ( l_iNbProperties >= 8 )
				{
					// Parsing vertices : position + normal + texture
					for ( auto & l_vertex : l_vertices )
					{
						std::getline( l_isFile, l_strLine );
						l_ssToken.str( l_strLine );
						l_ssToken >> l_vertex.m_pos[0] >> l_vertex.m_pos[1] >> l_vertex.m_pos[2];
						l_ssToken >> l_vertex.m_nml[0] >> l_vertex.m_nml[1] >> l_vertex.m_nml[2];
						l_ssToken >> l_vertex.m_tex[0] >> l_vertex.m_tex[1];
						l_ssToken.clear( std::istringstream::goodbit );
					}
				}
				else if ( l_iNbProperties >= 6 )
				{
					// Parsing vertices : position + normal
					for ( auto & l_vertex : l_vertices )
					{
						std::getline( l_isFile, l_strLine );
						l_ssToken.str( l_strLine );
						l_ssToken >> l_vertex.m_pos[0] >> l_vertex.m_pos[1] >> l_vertex.m_pos[2];
						l_ssToken >> l_vertex.m_nml[0] >> l_vertex.m_nml[1] >> l_vertex.m_nml[2];
						l_ssToken.clear( std::istringstream::goodbit );
					}
				}
				else
				{
					// Parsing vertices : position
					for ( auto & l_vertex : l_vertices )
					{
						std::getline( l_isFile, l_strLine );
						l_ssToken.str( l_strLine );
						l_ssToken >> l_vertex.m_pos[0] >> l_vertex.m_pos[1] >> l_vertex.m_pos[2];
						l_ssToken.clear( std::istringstream::goodbit );
					}
				}

				l_submesh->AddPoints( l_vertices );
				// Parsing triangles
				FaceSPtr l_pFace;
				std::vector< FaceIndices > l_faces( l_iNbFaces );
				FaceIndices * l_pFaces = &l_faces[0];

				for ( int i = 0; i < l_iNbFaces; i++ )
				{
					std::getline( l_isFile, l_strLine );
					l_ssToken.str( l_strLine );
					l_ssToken >> l_iNbVertex;

					if ( l_iNbVertex >= 3 )
					{
						l_ssToken >> l_pFaces->m_index[0] >> l_pFaces->m_index[1] >> l_pFaces->m_index[2];
						l_pFaces++;
					}

					l_ssToken.clear( std::istringstream::goodbit );
				}

				l_submesh->AddFaceGroup( l_faces );
			}

			l_return = true;
		}

		l_submesh->ComputeContainers();

		if ( l_iNbProperties < 6 )
		{
			l_submesh->ComputeNormals( false );
		}
		else
		{
			l_submesh->ComputeTangentsFromNormals();
		}

		l_isFile.close();
		return l_return;
	}
}
