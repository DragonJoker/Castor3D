#include "PrecompiledHeader.h"

#include "importer/Module_Importer.h"

#include "importer/PlyImporter.h"
#include "render_system/RenderSystem.h"
#include "render_system/Buffer.h"
#include "scene/NodeBase.h"
#include "scene/SceneManager.h"
#include "scene/Scene.h"
#include "camera/Camera.h"
#include "camera/Viewport.h"
#include "material/MaterialManager.h"
#include "material/Material.h"
#include "material/Pass.h"
#include "material/TextureUnit.h"
#include "geometry/primitives/Geometry.h"
#include "geometry/mesh/MeshManager.h"
#include "geometry/mesh/Mesh.h"
#include "geometry/mesh/Submesh.h"
#include "geometry/basic/SmoothingGroup.h"
#include "geometry/basic/Face.h"


using namespace Castor3D;

PlyImporter :: PlyImporter()
	:	ExternalImporter( ePLY)
{
}

bool PlyImporter :: _import()
{
	m_pScene = SceneManager::GetSingleton().GetElementByName( "MainScene");

	size_t l_uiSlashIndex = 0;

	if (m_fileName.find_last_of( CU_T( "\\")) != String::npos)
	{
		l_uiSlashIndex = m_fileName.find_last_of( CU_T( "\\")) + 1;
	}

	if (m_fileName.find_last_of( CU_T( "/")) != String::npos)
	{
		l_uiSlashIndex = std::max( l_uiSlashIndex, m_fileName.find_last_of( CU_T( "/")) + 1);
	}

	size_t l_uiDotIndex = m_fileName.find_last_of( CU_T( "."));

	UIntArray l_faces;
	FloatArray l_sizes;
	String l_name = m_fileName.substr( l_uiSlashIndex, l_uiDotIndex - l_uiSlashIndex);
	String l_meshName = m_fileName.substr( l_uiSlashIndex, l_uiDotIndex - l_uiSlashIndex);
	String l_materialName = m_fileName.substr( l_uiSlashIndex, l_uiDotIndex - l_uiSlashIndex);

	MeshPtr l_pMesh;

	if (MeshManager::HasElement( l_meshName))
	{
		l_pMesh = MeshManager::GetElementByName( l_meshName);
	}
	else
	{
		l_pMesh = MeshManager::CreateMesh( l_meshName, l_faces, l_sizes, Mesh::eCustom);
		Logger::LogMessage( CU_T( "CreatePrimitive - Mesh %s created"), l_meshName.c_str());
	}

	std::ifstream l_isFile;
	l_isFile.open( m_fileName.c_str(), std::ios::in);

	std::string l_strLine;
	std::istringstream l_ssToken;
	String::size_type l_stIndex;
	real * l_pfNormals = NULL;
	real * l_pfTexcoords = NULL;

	SubmeshPtr l_pSubmesh = l_pMesh->CreateSubmesh( 1);
	MaterialPtr l_pMaterial = MaterialManager::CreateMaterial( l_materialName);
	l_pSubmesh->SetMaterial( l_pMaterial);

	// Parsing the ply identification line
	std::getline( l_isFile, l_strLine);

	if (l_strLine == "ply")
	{
		// Parsing the format specification line
		std::getline( l_isFile, l_strLine);

		if (l_strLine == "format ascii 1.0")
		{
			int l_iNbProperties = 0;
			int l_iNbVertex;
			int l_iNbFaces;

			// Parsing number of vertices
			while (l_isFile.good())
			{
				std::getline( l_isFile, l_strLine);

				if ( (l_stIndex = l_strLine.find( "element vertex ")) == String::npos )
				{
					continue;
				}
				else 
				{
					l_ssToken.str( l_strLine.substr( String( "element vertex ").length()) );
					l_ssToken >> l_iNbVertex;
					l_ssToken.clear( std::istringstream::goodbit);

//					std::cout << "Vertices: " << l_iNbVertex << std::endl;
					break;
				}
			}

			// Parsing number of vertex properties
			while (l_isFile.good())
			{
				std::getline( l_isFile, l_strLine);

				if ( (l_stIndex = l_strLine.find( "property ")) != String::npos )
				{
					l_iNbProperties++;
				}
				else
				{
					l_isFile.seekg( -l_isFile.gcount()); // Unget last line
//					std::cout << "Properties: " << l_iNbProperties << std::endl;
					break;
				}
			}


			// Parsing number of triangles
			while (l_isFile.good())
			{
				std::getline( l_isFile, l_strLine);

				if ( (l_stIndex = l_strLine.find( "element face ")) == String::npos )
				{
					continue;
				}
				else
				{
					l_ssToken.str( l_strLine.substr( String("element face ").size()) );
					l_ssToken >> l_iNbFaces;
					l_ssToken.clear( std::istringstream::goodbit);

//					std::cout << "Triangles: " << l_iNbFaces << std::endl;
					break;
				}

			}

			// Parsing end of the header
			while (l_isFile.good())
			{
				std::getline( l_isFile, l_strLine);

				if ( (l_stIndex = l_strLine.find( "end_header")) == String::npos )
				{
					continue;
				}
				else
				{
					break;
				}
			}

			if (l_iNbProperties >= 6)
			{
				l_pfNormals = new real[l_iNbVertex * 3];
			}

			if (l_iNbProperties >= 8)
			{
				l_pfTexcoords = new real[l_iNbVertex * 2];
			}

			real l_fX, l_fY, l_fZ;

			// Parsing vertices
			for (int i = 0 ; i < l_iNbVertex ; i++)
			{
				std::getline( l_isFile, l_strLine);

				l_ssToken.str( l_strLine);
				l_ssToken >> l_fX >> l_fY >> l_fZ;
				l_pSubmesh->AddVertex( l_fX, l_fY, l_fZ);

				if (l_iNbProperties >= 6)
				{
					l_ssToken >> l_pfNormals[i*3] >> l_pfNormals[i*3 + 1] >> l_pfNormals[i*3 + 2];
				}

				if (l_iNbProperties >= 8)
				{
					l_ssToken >> l_pfTexcoords[i*2] >> l_pfTexcoords[i*2 + 1];
				}

//				std::cout << "Coords: " << l_fX << ", " << l_fY << ", " << l_fZ << std::endl;
//				std::cout << "Normals: " << l_pfNormals[i*3] << ", " << l_pfNormals[i*3 + 1] << ", " << l_pfNormals[i*3 + 2] << std::endl;
//				std::cout << "TexCoords: " << l_pfTexcoords[i*2] << ", " << l_pfTexcoords[i*2 + 1] << std::endl;

				l_ssToken.clear( std::istringstream::goodbit);
			}

			// Parsing triangles
			int l_iV1, l_iV2, l_iV3;

			for (int i = 0 ; i < l_iNbFaces ; i++)
			{
				std::getline( l_isFile, l_strLine);

				l_ssToken.str( l_strLine);
				l_ssToken >> l_iNbVertex;

				if (l_iNbVertex == 3)
				{
					l_ssToken >> l_iV1 >> l_iV2 >> l_iV3;
//					std::cout << "Indices: " << l_iV1 << ", " << l_iV2 << ", " << l_iV3 << std::endl;
					Face & l_face = l_pSubmesh->AddFace( l_iV2, l_iV1, l_iV3, 0);
					l_face.SetVertexTexCoords( 0, l_pfTexcoords[(l_iV2 * 2)], l_pfTexcoords[(l_iV2 * 2) + 1]);
					l_face.SetVertexTexCoords( 1, l_pfTexcoords[(l_iV1 * 2)], l_pfTexcoords[(l_iV1 * 2) + 1]);
					l_face.SetVertexTexCoords( 2, l_pfTexcoords[(l_iV3 * 2)], l_pfTexcoords[(l_iV3 * 2) + 1]);
				}

				l_ssToken.clear( std::istringstream::goodbit);
			}
		}
	}

	delete [] l_pfNormals;
	delete [] l_pfTexcoords;

	l_pSubmesh->ComputeContainers();
	l_pSubmesh->GenerateBuffers();
	l_pMesh->ComputeNormals();

	l_isFile.close();

	GeometryNodePtr l_pNode = m_pScene->CreateGeometryNode( l_name);

	GeometryPtr l_pGeometry( new Geometry( l_pMesh, l_pNode, l_name));
	Logger::LogMessage( CU_T( "PlyImporter::_import - Geometry %s created"), l_name.c_str());

	m_geometries.insert( GeometryPtrStrMap::value_type( l_name, l_pGeometry));

	return true;
}