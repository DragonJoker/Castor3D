#include "PrecompiledHeader.h"

#include "importer/Module_Importer.h"

#include "importer/Md3Importer.h"
#include "material/MaterialManager.h"
#include "material/Material.h"
#include "material/Pass.h"
#include "geometry/mesh/MeshManager.h"
#include "geometry/mesh/Mesh.h"
#include "geometry/mesh/Submesh.h"
#include "geometry/primitives/Geometry.h"
#include "scene/Scene.h"
#include "scene/SceneManager.h"
#include "Log.h"

using namespace Castor3D;

bool _isInString( const String & p_strString, const String & p_strSubString)
{
	if (p_strString.length() <= 0 || p_strSubString.length() <= 0)
	{
		return false;
	}

	return p_strString.find( p_strSubString) != String::npos;
}

Md3Importer :: Md3Importer( const String & p_textureName)
	:	m_textureName( p_textureName),
		m_skins( NULL),
		m_texCoords( NULL),
		m_triangles( NULL),
		m_bones( NULL),
		m_pFile( NULL)
{
	memset( & m_header, 0, sizeof( Md3Header));
}

bool Md3Importer :: _import()
{
	m_pFile = new File( m_fileName, File::eRead);

	size_t l_uiSlashIndex = 0;

	if (m_fileName.find_last_of( C3D_T( "\\")) != String::npos)
	{
		l_uiSlashIndex = m_fileName.find_last_of( C3D_T( "\\")) + 1;
	}

	if (m_fileName.find_last_of( C3D_T( "/")) != String::npos)
	{
		l_uiSlashIndex = max( l_uiSlashIndex, m_fileName.find_last_of( C3D_T( "/")) + 1);
	}

	size_t l_uiDotIndex = m_fileName.find_last_of( C3D_T( "."));

	UIntArray l_faces;
	FloatArray l_sizes;
	String l_name = m_fileName.substr( l_uiSlashIndex, l_uiDotIndex - l_uiSlashIndex);
	String l_meshName = m_fileName.substr( l_uiSlashIndex, l_uiDotIndex - l_uiSlashIndex);
	String l_materialName = m_fileName.substr( l_uiSlashIndex, l_uiDotIndex - l_uiSlashIndex);

	Mesh * l_pMesh = NULL;

	if (MeshManager::GetSingletonPtr()->HasElement( l_meshName))
	{
		l_pMesh = MeshManager::GetSingletonPtr()->GetElementByName( l_meshName);
	}
	else
	{
		l_pMesh = MeshManager::GetSingletonPtr()->CreateMesh( l_meshName, l_faces, l_sizes, Mesh::eCustom);
		Log::LogMessage( C3D_T( "CreatePrimitive - Mesh %s created"), l_meshName.c_str());
	}

	Imported3DModel l_model = {0};

	m_pFile->Read<Md3Header>( m_header);

	char * l_id = m_header.m_fileID;

	if ((l_id[0] != 'I' || l_id[1] != 'D' || l_id[2] != 'P' || l_id[3] != '3') || m_header.m_version != 15)
	{
		return false;
	}
	
	_readMD3Data( & l_model);

	_cleanUp();

	Material * l_material = MaterialManager::GetSingletonPtr()->CreateMaterial( l_materialName);
	Pass * l_pass = l_material->GetPass( 0);
	l_pass->SetAmbient( 0.0f, 0.0f, 0.0f, 1.0f);
	l_pass->SetEmissive( 0.5f, 0.5f, 0.5f, 1.0f);

	ImportedMaterialInfo l_infos;
	Strcpy( l_infos.m_strName, 256, l_material->GetName().c_str());

	if ( ! m_textureName.empty())
	{
		Strcpy( l_infos.m_strFile, 256, m_textureName.c_str());
	}

	l_model.m_numOfMaterials = 1;
	l_model.m_materials.push_back( l_infos);

	_convertToMesh( l_pMesh, & l_model);

	Submesh * l_submesh;
	for (size_t i = 0 ; i < l_pMesh->GetNbSubmeshes() ; i++)
	{
		l_submesh = l_pMesh->GetSubmesh( i);
		l_submesh->InvertNormals();
	}

	m_pScene = SceneManager::GetSingleton().GetElementByName( "MainScene");
	SceneNode * l_pNode = m_pScene->CreateSceneNode( l_name);

	Geometry * l_pGeometry = new Geometry( l_pMesh, l_pNode, l_name);
	Log::LogMessage( C3D_T( "PlyImporter::_import - Geometry %s created"), l_name.c_str());

	m_geometries.insert( GeometryStrMap::value_type( l_name, l_pGeometry));

	_cleanUp();

	delete m_pFile;

	return true;
}

void Md3Importer :: _readMD3Data( Imported3DModel * p_model)
{
	int i = 0;
	m_bones = new Md3Bone[m_header.m_numFrames];
	m_pFile->ReadArray<Md3Bone>( m_bones, m_header.m_numFrames);
	delete [] m_bones;

	m_tags = new Md3Tag[m_header.m_numFrames * m_header.m_numTags];
	m_pFile->ReadArray<Md3Tag>( m_tags, m_header.m_numFrames * m_header.m_numTags);
	m_numOfTags = m_header.m_numTags;

	m_links = (Imported3DModel **) malloc( sizeof( Imported3DModel) * m_header.m_numTags);

	for (i = 0 ; i < m_header.m_numTags ; i++)
	{
		m_links[i] = NULL;
	}

	long l_meshOffset = m_pFile->Tell();

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

		_convertDataStructures( p_model, l_meshHeader);

		delete [] m_skins;    
		delete [] m_texCoords;
		delete [] m_triangles;
		delete [] m_vertices;   

		l_meshOffset += l_meshHeader.m_meshSize;
	}
}

void Md3Importer :: _convertDataStructures( Imported3DModel * p_model, Md3MeshInfo p_meshHeader)
{
	int i = 0;

	p_model->m_numOfObjects++;

	Imported3DObject l_currentMesh = {0};
	Strcpy( l_currentMesh.m_strName, 256, String( p_meshHeader.m_strName).c_str());

	l_currentMesh.m_numOfVerts   = p_meshHeader.m_numVertices;
	l_currentMesh.m_numTexVertex = p_meshHeader.m_numVertices;
	l_currentMesh.m_numOfFaces   = p_meshHeader.m_numTriangles;

	l_currentMesh.m_vertex   = new ImportedVertex3[l_currentMesh.m_numOfVerts * p_meshHeader.m_numMeshFrames];
	l_currentMesh.m_texVerts = new ImportedVertex2[l_currentMesh.m_numOfVerts];
	l_currentMesh.m_faces    = new ImportedFace[l_currentMesh.m_numOfFaces];

	for (i = 0 ; i < l_currentMesh.m_numOfVerts * p_meshHeader.m_numMeshFrames ; i++)
	{
		l_currentMesh.m_vertex[i].x =  m_vertices[i].m_vertex[0] / 64.0f;
		l_currentMesh.m_vertex[i].y =  m_vertices[i].m_vertex[1] / 64.0f;
		l_currentMesh.m_vertex[i].z =  m_vertices[i].m_vertex[2] / 64.0f;
	}

	for (i = 0 ; i < l_currentMesh.m_numTexVertex ; i++)
	{
		l_currentMesh.m_texVerts[i].x =  m_texCoords[i].m_textureCoord[0];
		l_currentMesh.m_texVerts[i].y = -m_texCoords[i].m_textureCoord[1];
	}

	for (i = 0 ; i < l_currentMesh.m_numOfFaces ; i++)
	{
		l_currentMesh.m_faces[i].m_vertIndex[0] = m_triangles[i].m_vertexIndices[0];
		l_currentMesh.m_faces[i].m_vertIndex[1] = m_triangles[i].m_vertexIndices[2];
		l_currentMesh.m_faces[i].m_vertIndex[2] = m_triangles[i].m_vertexIndices[1];

		l_currentMesh.m_faces[i].m_coordIndex[0] = m_triangles[i].m_vertexIndices[0];
		l_currentMesh.m_faces[i].m_coordIndex[1] = m_triangles[i].m_vertexIndices[2];
		l_currentMesh.m_faces[i].m_coordIndex[2] = m_triangles[i].m_vertexIndices[1];
	}

	p_model->m_objects.push_back( l_currentMesh);
}

bool Md3Importer :: _loadSkin( Imported3DModel * p_model, const String & p_strSkin)
{
	if ( ! p_model || p_strSkin.empty())
	{
		return false;
	}

	std::ifstream l_inFile( p_strSkin.char_str());

	if (l_inFile.fail())
	{
		return false;
	}

	std::string l_line;
	int l_textureNameStart = 0;

	while (getline( l_inFile, l_line))
	{
		for (int i = 0; i < p_model->m_numOfObjects; i++)
		{
			if (_isInString( l_line, p_model->m_objects[i].m_strName) )			
			{			
				for (int j = l_line.size() - 1; j > 0; j--)
				{
					if (l_line[j] == '/')
					{
						l_textureNameStart = j + 1;
						break;
					}	
				}

				ImportedMaterialInfo l_texture;

				Strcpy( l_texture.m_strFile, 256, String( & l_line[l_textureNameStart]).c_str());

				l_texture.m_uTile = l_texture.m_vTile = 1;

				p_model->m_objects[i].m_materialID = p_model->m_numOfMaterials;
				p_model->m_objects[i].m_hasTexture = true;

				p_model->m_numOfMaterials++;

				p_model->m_materials.push_back( l_texture);
			}
		}
	}

	l_inFile.close();
	return true;
}

bool Md3Importer :: _loadShader( Imported3DModel *p_model, const String & p_strShader)
{
	if ( ! p_model || p_strShader.empty())
	{
		return false;
	}

	std::ifstream l_inFile( p_strShader.char_str());

	if (l_inFile.fail())
	{
		return false;
	}

	std::string l_line;
	int l_currentIndex = 0;
	
	while (getline( l_inFile, l_line))
	{
		ImportedMaterialInfo l_texture;

		Strcpy( l_texture.m_strFile, 256, String( l_line).c_str());
				
		l_texture.m_uTile = l_texture.m_uTile = 1;

		p_model->m_objects[l_currentIndex].m_materialID = p_model->m_numOfMaterials;
		p_model->m_objects[l_currentIndex].m_hasTexture = true;

		p_model->m_numOfMaterials++;

		p_model->m_materials.push_back( l_texture);

		l_currentIndex++;
	}

	l_inFile.close();
	return true;
}

void Md3Importer :: _cleanUp()
{
}
