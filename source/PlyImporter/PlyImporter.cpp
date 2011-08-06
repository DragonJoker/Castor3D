#include <Castor3D/Prerequisites.hpp>

using namespace Castor::Templates;

#include <Castor3D/RenderSystem.hpp>
#include <Castor3D/Buffer.hpp>
#include <Castor3D/SceneNode.hpp>
#include <Castor3D/Scene.hpp>
#include <Castor3D/Camera.hpp>
#include <Castor3D/Viewport.hpp>
#include <Castor3D/Material.hpp>
#include <Castor3D/Pass.hpp>
#include <Castor3D/TextureUnit.hpp>
#include <Castor3D/Geometry.hpp>
#include <Castor3D/Mesh.hpp>
#include <Castor3D/Submesh.hpp>
#include <Castor3D/SmoothingGroup.hpp>
#include <Castor3D/Face.hpp>
#include <Castor3D/Version.hpp>
#include <Castor3D/Plugin.hpp>
#include <Castor3D/Root.hpp>

#include "PlyImporter/PlyImporter.hpp"

using namespace Castor3D;

//*************************************************************************************************

C3D_Ply_API void GetRequiredVersion( Version & p_version)
{
	p_version = Version();
}

C3D_Ply_API ePLUGIN_TYPE GetType()
{
	return ePLUGIN_TYPE_IMPORTER;
}

C3D_Ply_API String GetName()
{
	return cuT( "PLY Importer Plugin");
}

C3D_Ply_API String GetExtension()
{
	return cuT( "PLY");
}

C3D_Ply_API Importer * CreateImporter()
{
	Importer * l_pReturn( new PlyImporter());

	return l_pReturn;
}

//*************************************************************************************************

PlyImporter :: PlyImporter()
	:	Importer()
{
}

bool PlyImporter :: _import()
{
	Collection<Scene, String> l_scnCollection;

	m_pScene = l_scnCollection.GetElement( "MainScene");

	size_t l_uiSlashIndex = 0;

	if (m_fileName.find_last_of( cuT( "\\")) != String::npos)
	{
		l_uiSlashIndex = m_fileName.find_last_of( cuT( "\\")) + 1;
	}

	if (m_fileName.find_last_of( cuT( "/")) != String::npos)
	{
		l_uiSlashIndex = std::max<size_t>( l_uiSlashIndex, m_fileName.find_last_of( cuT( "/")) + 1);
	}

	size_t l_uiDotIndex = m_fileName.find_last_of( cuT( "."));

	UIntArray l_faces;
	FloatArray l_sizes;
	String l_name = m_fileName.substr( l_uiSlashIndex, l_uiDotIndex - l_uiSlashIndex);
	String l_meshName = m_fileName.substr( l_uiSlashIndex, l_uiDotIndex - l_uiSlashIndex);
	String l_materialName = m_fileName.substr( l_uiSlashIndex, l_uiDotIndex - l_uiSlashIndex);

	Collection<Mesh, String> l_mshCollection;
	MeshPtr l_pMesh = l_mshCollection.GetElement( l_meshName);
	if ( ! l_pMesh)
	{
		l_pMesh = MeshPtr( new Mesh( l_meshName, eMESH_TYPE_CUSTOM));
		l_pMesh->Initialise( l_faces, l_sizes);
		l_mshCollection.AddElement( l_meshName, l_pMesh);
		Logger::LogMessage( cuT( "CreatePrimitive - Mesh ") + l_meshName + cuT( " created"));
	}

	std::ifstream l_isFile;
	l_isFile.open( m_fileName.char_str(), std::ios::in);

	std::string l_strLine;
	std::istringstream l_ssToken;
	String::size_type l_stIndex;
	real * l_pfNormals = nullptr;
	real * l_pfTexcoords = nullptr;

	SubmeshPtr l_pSubmesh = l_pMesh->CreateSubmesh( 1);
	Collection<Material, String> l_mtlCollection;
	MaterialPtr l_pMaterial = l_mtlCollection.GetElement( l_materialName);
	if ( ! l_pMaterial)
	{
		l_pMaterial = MaterialPtr( new Material( l_materialName, 1));
		l_mtlCollection.AddElement( l_materialName, l_pMaterial);
	}

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
				l_pSubmesh->AddPoint( l_fX, l_fY, l_fZ);

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
					FacePtr l_face = l_pSubmesh->AddFace( l_iV2, l_iV1, l_iV3, 0);
					l_face->SetVertexTexCoords( 0, l_pfTexcoords[(l_iV2 * 2)], l_pfTexcoords[(l_iV2 * 2) + 1]);
					l_face->SetVertexTexCoords( 1, l_pfTexcoords[(l_iV1 * 2)], l_pfTexcoords[(l_iV1 * 2) + 1]);
					l_face->SetVertexTexCoords( 2, l_pfTexcoords[(l_iV3 * 2)], l_pfTexcoords[(l_iV3 * 2) + 1]);
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

	SceneNodePtr l_pNode = m_pScene->CreateSceneNode( l_name);

	GeometryPtr l_pGeometry( new Geometry( m_pScene.get(), l_pMesh, l_pNode, l_name));
	Logger::LogMessage( cuT( "PlyImporter::_import - Geometry %s created"), l_name.c_str());

	m_geometries.insert( GeometryPtrStrMap::value_type( l_name, l_pGeometry));

	return true;
}

//*************************************************************************************************
