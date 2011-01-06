#include <Castor3D/Prerequisites.h>

using namespace Castor::Templates;

#include <Castor3D/material/MaterialManager.h>
#include <Castor3D/material/Material.h>
#include <Castor3D/material/Pass.h>
#include <Castor3D/material/TextureUnit.h>
#include <Castor3D/render_system/RenderSystem.h>
#include <Castor3D/geometry/mesh/MeshManager.h>
#include <Castor3D/geometry/mesh/Mesh.h>
#include <Castor3D/geometry/mesh/Submesh.h>
#include <Castor3D/geometry/basic/Face.h>
#include <Castor3D/geometry/primitives/Geometry.h>
#include <Castor3D/main/Version.h>
#include <Castor3D/scene/Scene.h>
#include <Castor3D/scene/SceneNode.h>
#include <Castor3D/scene/SceneManager.h>

#include "Md3Importer/Md3Importer.h"

using namespace Castor3D;

//*************************************************************************************************

extern "C" C3D_Md3_API void GetRequiredVersion( Version & p_version)
{
	p_version = Version( 0, 6);
}

extern "C" C3D_Md3_API Importer * CreateImporter( SceneManager * p_pManager)
{
	Importer * l_pReturn( new Md3Importer( p_pManager));

	return l_pReturn;
}

//*************************************************************************************************

bool _isInString( const String & p_strString, const String & p_strSubString)
{
	if (p_strString.length() <= 0 || p_strSubString.length() <= 0)
	{
		return false;
	}

	return p_strString.find( p_strSubString) != String::npos;
}

Md3Importer :: Md3Importer( SceneManager * p_pManager)
	:	Importer( p_pManager)
	,	m_skins( NULL)
	,	m_texCoords( NULL)
	,	m_triangles( NULL)
	,	m_bones( NULL)
	,	m_pFile( NULL)
{
	memset( & m_header, 0, sizeof( Md3Header));
}

bool Md3Importer :: _import()
{
	m_pFile = new File( m_fileName, File::eRead);

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

	if (m_pManager->GetMeshManager()->HasElement( l_meshName))
	{
		l_pMesh = m_pManager->GetMeshManager()->GetElementByName( l_meshName);
	}
	else
	{
		l_pMesh = m_pManager->GetMeshManager()->CreateMesh( l_meshName, l_faces, l_sizes, Mesh::eCustom);
		Logger::LogMessage( CU_T( "CreatePrimitive - Mesh %s created"), l_meshName.c_str());
	}

	m_pFile->Read<Md3Header>( m_header);

	char * l_id = m_header.m_fileID;

	if ((l_id[0] != 'I' || l_id[1] != 'D' || l_id[2] != 'P' || l_id[3] != '3') || m_header.m_version != 15)
	{
		return false;
	}
	
	_readMD3Data( l_pMesh);

	_cleanUp();

	m_pScene = m_pManager->GetElementByName( "MainScene");
	SceneNodePtr l_pNode = m_pScene->CreateSceneNode( l_name);

	GeometryPtr l_pGeometry( new Geometry( l_pMesh, l_pNode, l_name));
	Logger::LogMessage( CU_T( "PlyImporter::_import - Geometry %s created"), l_name.c_str());

	m_geometries.insert( GeometryPtrStrMap::value_type( l_name, l_pGeometry));

	_cleanUp();

	delete m_pFile;

	return true;
}

void Md3Importer :: _readMD3Data( MeshPtr p_pMesh)
{
	int i = 0;
	m_bones = new Md3Bone[m_header.m_numFrames];
	m_pFile->ReadArray<Md3Bone>( m_bones, m_header.m_numFrames);
	delete [] m_bones;

	m_tags = new Md3Tag[m_header.m_numFrames * m_header.m_numTags];
	m_pFile->ReadArray<Md3Tag>( m_tags, m_header.m_numFrames * m_header.m_numTags);
	m_numOfTags = m_header.m_numTags;

	m_links = (MeshPtr *) malloc( sizeof( Mesh) * m_header.m_numTags);

	for (i = 0 ; i < m_header.m_numTags ; i++)
	{
		m_links[i].reset();
	}

	long l_meshOffset = m_pFile->Tell();
	String l_strFileName = m_pFile->GetFileFullPath();
	l_strFileName = l_strFileName.substr( 0, l_strFileName.find_last_of( '.'));

	Md3MeshInfo l_meshHeader;
	for (i = 0; i < m_header.m_numMeshes; i++)
	{
		m_pFile->Seek( l_meshOffset);
		m_pFile->Read<Md3MeshInfo>( l_meshHeader);

		m_skins     = new Md3Skin[l_meshHeader.m_numSkins];
		m_texCoords = new Md3TexCoord[l_meshHeader.m_numVertices];
		m_triangles = new Md3Face[l_meshHeader.m_numTriangles];
		m_vertices  = new Md3Triangle[l_meshHeader.m_numVertices * l_meshHeader.m_numMeshFrames];

		m_pFile->ReadArray<Md3Skin>( m_skins, l_meshHeader.m_numSkins);

		m_pFile->Seek( l_meshOffset + l_meshHeader.m_triStart);
		m_pFile->ReadArray<Md3Face>( m_triangles, l_meshHeader.m_numTriangles);

		m_pFile->Seek( l_meshOffset + l_meshHeader.m_uvStart);
		m_pFile->ReadArray<Md3TexCoord>( m_texCoords, l_meshHeader.m_numVertices);

		m_pFile->Seek( l_meshOffset + l_meshHeader.m_vertexStart);
		m_pFile->ReadArray<Md3Triangle>( m_vertices, l_meshHeader.m_numMeshFrames * l_meshHeader.m_numVertices);

		_convertDataStructures( p_pMesh, l_meshHeader);

		delete [] m_skins;    
		delete [] m_texCoords;
		delete [] m_triangles;
		delete [] m_vertices;   

		l_meshOffset += l_meshHeader.m_meshSize;
	}

	p_pMesh->ComputeContainers();

	for (size_t i = 0 ; i < p_pMesh->GetNbSubmeshes() ; i++)
	{
		p_pMesh->GetSubmesh( i)->GenerateBuffers();
	}

	_loadSkin( l_strFileName + ".skin");
	_loadShader( p_pMesh, l_strFileName + ".shader");

	p_pMesh->ComputeNormals();
}

void Md3Importer :: _convertDataStructures( MeshPtr p_pMesh, Md3MeshInfo p_meshHeader)
{
	SubmeshPtr l_pSubmesh = p_pMesh->CreateSubmesh( 1);
	m_mapSubmeshesByName.insert( SubmeshPtrStrMap::value_type( p_meshHeader.m_strName, l_pSubmesh));

	for (int i = 0 ; i < p_meshHeader.m_numVertices * p_meshHeader.m_numMeshFrames ; i++)
	{
		l_pSubmesh->AddPoint( m_vertices[i].m_vertex[0] / 64.0f, m_vertices[i].m_vertex[1] / 64.0f, m_vertices[i].m_vertex[2] / 64.0f);
	}

	Point2rArray l_texVerts;

	for (int i = 0 ; i < p_meshHeader.m_numVertices ; i++)
	{
		l_texVerts.push_back( Point2r( 2, m_texCoords[i].m_textureCoord[0], -m_texCoords[i].m_textureCoord[1]));
	}

	for (int i = 0 ; i < p_meshHeader.m_numTriangles ; i++)
	{
		FacePtr l_face = l_pSubmesh->AddFace( m_triangles[i].m_vertexIndices[0], m_triangles[i].m_vertexIndices[1], m_triangles[i].m_vertexIndices[2], 0);
		l_face->SetVertexTexCoords( 0, l_texVerts[m_triangles[i].m_vertexIndices[0]][0], l_texVerts[m_triangles[i].m_vertexIndices[0]][1]);
		l_face->SetVertexTexCoords( 1, l_texVerts[m_triangles[i].m_vertexIndices[1]][0], l_texVerts[m_triangles[i].m_vertexIndices[1]][1]);
		l_face->SetVertexTexCoords( 2, l_texVerts[m_triangles[i].m_vertexIndices[2]][0], l_texVerts[m_triangles[i].m_vertexIndices[2]][1]);
	}
}

bool Md3Importer :: _loadSkin( const String & p_strSkin)
{
	bool l_bReturn = false;

	if (File::FileExists( p_strSkin))
	{
		File l_fileIO( p_strSkin, File::eRead);
		String l_strLine, l_strSection, l_strImage;

		while (l_fileIO.IsOk())
		{
			l_fileIO.ReadLine( l_strLine, 255);
			l_strSection.clear();

			if ( ! l_strLine.empty() && l_strLine.find_last_of( ',') != String::npos)
			{
				l_strSection = l_strLine.substr( 0, l_strLine.find_last_of( ','));
			}

			if (m_mapSubmeshesByName.find( l_strSection) != m_mapSubmeshesByName.end())
			{
				l_strImage = l_strLine.substr( l_strLine.find_last_of( ',') + 1);
				l_strImage = l_strImage.substr( l_strImage.find_last_of( '/') + 1);

				MaterialPtr l_material = m_pManager->GetMaterialManager()->CreateMaterial( l_strSection);
				PassPtr l_pass = l_material->GetPass( 0);
				l_pass->SetAmbient( 0.0f, 0.0f, 0.0f, 1.0f);
				l_pass->SetEmissive( 0.5f, 0.5f, 0.5f, 1.0f);

				if ( ! l_strImage.empty())
				{
					TextureUnitPtr l_unit = l_pass->AddTextureUnit();

					if (File::FileExists( l_strImage))
					{
						l_unit->SetTexture2D( m_pManager->GetImageManager()->CreateImage( l_strImage, l_strImage));
					}
					else if (File::FileExists( l_fileIO.GetFilePath() + "/" + l_strImage))
					{
						l_unit->SetTexture2D( m_pManager->GetImageManager()->CreateImage( l_fileIO.GetFilePath() + "/" + l_strImage, l_fileIO.GetFilePath() + "/" + l_strImage));
					}
					else
					{
						l_pass->DestroyTextureUnit( l_unit->GetIndex());
					}
				}

				m_mapSubmeshesByName.find( l_strSection)->second->SetMaterial( l_material);
				m_pManager->GetMaterialManager()->SetToInitialise( l_material);
			}
		}

		l_bReturn = true;
	}

	return l_bReturn;
}

bool Md3Importer :: _loadShader( MeshPtr p_pMesh, const String & p_strShader)
{
	bool l_bReturn = false;

	if (File::FileExists( p_strShader))
	{
		File l_fileIO( p_strShader, File::eRead);
		String l_strName = l_fileIO.GetFileName().substr( 0, l_fileIO.GetFileName().find_last_of( '.'));
		String l_strLine;
		size_t l_uiIndex = 0;
		String l_strMatName;

		while (l_fileIO.IsOk() && l_uiIndex < p_pMesh->GetNbSubmeshes())
		{
			l_fileIO.ReadLine( l_strLine, 255);

			l_strMatName = l_strName;
			l_strMatName << "_" << l_uiIndex;

			MaterialPtr l_material = m_pManager->GetMaterialManager()->CreateMaterial( l_strMatName);
			PassPtr l_pass = l_material->GetPass( 0);
			l_pass->SetAmbient( 0.0f, 0.0f, 0.0f, 1.0f);
			l_pass->SetEmissive( 0.5f, 0.5f, 0.5f, 1.0f);
			l_pass->SetDoubleFace( true);

			if ( ! l_strLine.empty())
			{
				TextureUnitPtr l_unit = l_pass->AddTextureUnit();

				if (File::FileExists( l_strLine))
				{
					l_unit->SetTexture2D( m_pManager->GetImageManager()->CreateImage( l_strLine, l_strLine));
				}
				else if (File::FileExists( l_fileIO.GetFilePath() + "/" + l_strLine))
				{
					l_unit->SetTexture2D( m_pManager->GetImageManager()->CreateImage( l_fileIO.GetFilePath() + "/" + l_strLine, l_fileIO.GetFilePath() + "/" + l_strLine));
				}
				else
				{
					l_pass->DestroyTextureUnit( l_unit->GetIndex());
				}
			}

			p_pMesh->GetSubmesh( l_uiIndex)->SetMaterial( l_material);
			m_pManager->GetMaterialManager()->SetToInitialise( l_material);

			l_uiIndex++;
		}

		l_bReturn = true;
	}

	return l_bReturn;
}

void Md3Importer :: _cleanUp()
{
}