#include "PrecompiledHeader.h"

#include "importer/Module_Importer.h"

#include "importer/SMaxImporter.h"
#include "geometry/mesh/MeshManager.h"
#include "geometry/mesh/Mesh.h"
#include "geometry/primitives/Geometry.h"
#include "scene/Scene.h"
#include "scene/SceneManager.h"
#include "Log.h"

using namespace Castor3D;

char g_buffer[50000] = {0};

SMaxImporter :: SMaxImporter()
	:	ExternalImporter(),
		m_pFile( NULL)
{
}

bool SMaxImporter :: _import()
{
	m_pFile = new File( m_fileName, File::eRead);

	if (m_pFile->IsOk())
	{
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
		SMaxChunk l_currentChunk = {0};

		_readChunk( & l_currentChunk);

		if (l_currentChunk.m_id != SMAX_PRIMARY)
		{
			delete m_pFile;
			return false;
		}

		_processNextChunk( & l_model, & l_currentChunk);
		_cleanUp();

		Imported3DObject l_object;
		int l_tmp;
		for (int i = 0 ; i < l_model.m_numOfObjects ; i++)
		{
			l_object = l_model.m_objects[i];
			for (int j = 0 ; j < l_object.m_numOfFaces ; j++)
			{
				l_tmp = l_object.m_faces[j].m_vertIndex[0];
				l_object.m_faces[j].m_vertIndex[0] = l_object.m_faces[j].m_vertIndex[1];
				l_object.m_faces[j].m_vertIndex[1] = l_tmp;
				l_object.m_faces[j].m_coordIndex[0] = l_object.m_faces[j].m_vertIndex[0];
				l_object.m_faces[j].m_coordIndex[1] = l_object.m_faces[j].m_vertIndex[1];
				l_object.m_faces[j].m_coordIndex[2] = l_object.m_faces[j].m_vertIndex[2];
			}
		}

		_convertToMesh( l_pMesh, & l_model);

		m_pScene = SceneManager::GetSingleton().GetElementByName( "MainScene");
		SceneNode * l_pNode = m_pScene->CreateSceneNode( l_name);

		Geometry * l_pGeometry = new Geometry( l_pMesh, l_pNode, l_name);
		Log::LogMessage( C3D_T( "PlyImporter::_import - Geometry %s created"), l_name.c_str());

		m_geometries.insert( GeometryStrMap::value_type( l_name, l_pGeometry));
	}

	delete m_pFile;

	return true;
}

void SMaxImporter :: _cleanUp()
{
}

void SMaxImporter :: _processNextChunk( Imported3DModel * p_model, SMaxChunk * p_previousChunk)
{
	Imported3DObject l_newObject = {0};
	ImportedMaterialInfo l_newTexture;

	SMaxChunk l_currentChunk = {0};
	SMaxChunk l_tempChunk = {0};

	while (m_pFile->IsOk() && p_previousChunk->m_bytesRead < p_previousChunk->m_length)
	{
		_readChunk( & l_currentChunk);

		switch (l_currentChunk.m_id)
		{
		case SMAX_VERSION:
			l_currentChunk.m_bytesRead += m_pFile->ReadArray<char>( g_buffer, l_currentChunk.m_length - l_currentChunk.m_bytesRead);
			if ((l_currentChunk.m_length - l_currentChunk.m_bytesRead == 4) && (((int *)g_buffer)[0] > 0x03))
			{
				Log::LogMessage( "File version is over version 3 and may load incorrectly");
			}
		break;

		case SMAX_OBJECTINFO:
			_readChunk( & l_tempChunk);
			l_tempChunk.m_bytesRead += m_pFile->ReadArray<char>( g_buffer, l_tempChunk.m_length - l_tempChunk.m_bytesRead);
			l_currentChunk.m_bytesRead += l_tempChunk.m_bytesRead;
			_processNextChunk( p_model, & l_currentChunk);
		break;

		case SMAX_MATERIAL:
			p_model->m_numOfMaterials++;
			p_model->m_materials.push_back( l_newTexture);
			_processNextMaterialChunk( p_model, & l_currentChunk);
		break;

		case SMAX_OBJECT:
		{
			p_model->m_numOfObjects++;
			p_model->m_objects.push_back( l_newObject);
			memset( & (p_model->m_objects[p_model->m_numOfObjects - 1]), 0, sizeof( Imported3DObject));
			l_currentChunk.m_bytesRead += _getString( p_model->m_objects[p_model->m_numOfObjects - 1].m_strName);
			_processNextObjectChunk( p_model, &(p_model->m_objects[p_model->m_numOfObjects - 1]), & l_currentChunk);
		}
		break;

		case SMAX_EDITKEYFRAME:
			l_currentChunk.m_bytesRead += m_pFile->ReadArray<char>( g_buffer, l_currentChunk.m_length - l_currentChunk.m_bytesRead);
		break;

		default: 
			l_currentChunk.m_bytesRead += m_pFile->ReadArray<char>( g_buffer, l_currentChunk.m_length - l_currentChunk.m_bytesRead);
		break;
		}

		p_previousChunk->m_bytesRead += l_currentChunk.m_bytesRead;
	}
}

void SMaxImporter :: _processNextObjectChunk( Imported3DModel * p_model, Imported3DObject * p_object, SMaxChunk * p_previousChunk)
{
	SMaxChunk l_currentChunk = {0};

	while (m_pFile->IsOk() && p_previousChunk->m_bytesRead < p_previousChunk->m_length)
	{
		_readChunk( & l_currentChunk);

		switch (l_currentChunk.m_id)
		{
		case SMAX_OBJECT_MESH:
			_processNextObjectChunk( p_model, p_object, & l_currentChunk);
		break;

		case SMAX_OBJECT_VERTICES:
			_readVertices( p_object, & l_currentChunk);
		break;

		case SMAX_OBJECT_FACES:
			_readVertexIndices( p_object, & l_currentChunk);
		break;

		case SMAX_OBJECT_MATERIAL:
			_readObjectMaterial( p_model, p_object, & l_currentChunk);			
		break;

		case SMAX_OBJECT_UV:
			_readUVCoordinates( p_object, & l_currentChunk);
		break;

		default:  
			l_currentChunk.m_bytesRead += m_pFile->ReadArray<char>( g_buffer, l_currentChunk.m_length - l_currentChunk.m_bytesRead);
		break;
		}

		p_previousChunk->m_bytesRead += l_currentChunk.m_bytesRead;
	}
}

void SMaxImporter :: _processNextMaterialChunk( Imported3DModel * p_model, SMaxChunk * p_previousChunk)
{
	SMaxChunk l_currentChunk = {0};

	while (m_pFile->IsOk() && p_previousChunk->m_bytesRead < p_previousChunk->m_length)
	{
		_readChunk( & l_currentChunk);

		switch (l_currentChunk.m_id)
		{
		case SMAX_MATNAME:
			l_currentChunk.m_bytesRead += _getString( p_model->m_materials[p_model->m_numOfMaterials - 1].m_strName);
			l_currentChunk.m_bytesRead = l_currentChunk.m_length;
		break;

		case SMAX_MATDIFFUSE:
			_readColorChunk( & (p_model->m_materials[p_model->m_numOfMaterials - 1]), & l_currentChunk);
		break;

		case SMAX_MATMAP:
			_processNextMaterialChunk( p_model, & l_currentChunk);
		break;

		case SMAX_MATMAPFILE:
			l_currentChunk.m_bytesRead += _getString( p_model->m_materials[p_model->m_numOfMaterials - 1].m_strFile);
			l_currentChunk.m_bytesRead = l_currentChunk.m_length;
		break;

		default:  
			l_currentChunk.m_bytesRead += m_pFile->ReadArray<char>( g_buffer, l_currentChunk.m_length - l_currentChunk.m_bytesRead);
		break;
		}

		p_previousChunk->m_bytesRead += l_currentChunk.m_bytesRead;
	}
}

void SMaxImporter :: _readChunk( SMaxChunk * p_chunk)
{
	p_chunk->m_bytesRead = m_pFile->Read<unsigned short>( p_chunk->m_id);
	p_chunk->m_bytesRead += m_pFile->Read<unsigned int>( p_chunk->m_length);
}

int SMaxImporter :: _getString( Char * p_pBuffer)
{
	char l_pBuffer[255];
	int index = 0;
	m_pFile->Read<char>( *l_pBuffer);

	while (m_pFile->IsOk() && *(l_pBuffer + index++) != 0)
	{
		m_pFile->Read<char>( *(l_pBuffer + index));
	}

	Strcpy( p_pBuffer, 255, String( l_pBuffer).c_str());

	return strlen( l_pBuffer) + 1;
}

void SMaxImporter :: _readColorChunk( ImportedMaterialInfo * p_material, SMaxChunk * p_chunk)
{
	SMaxChunk l_tempChunk = {0};
	_readChunk( & l_tempChunk);
	l_tempChunk.m_bytesRead += m_pFile->ReadArray<char>( (char *)p_material->m_color, l_tempChunk.m_length - l_tempChunk.m_bytesRead);
	p_material->m_fDiffuse[0] = float( p_material->m_color[0]) / 255.0f;
	p_material->m_fDiffuse[1] = float( p_material->m_color[1]) / 255.0f;
	p_material->m_fDiffuse[2] = float( p_material->m_color[2]) / 255.0f;
	p_chunk->m_bytesRead += l_tempChunk.m_bytesRead;
}

void SMaxImporter :: _readVertexIndices( Imported3DObject * p_object, SMaxChunk * p_chunk)
{
	unsigned short l_index = 0;					// This is used to read in the current face index
	p_object->m_numOfFaces = 0;
	p_chunk->m_bytesRead += m_pFile->Read<short>( (short &)p_object->m_numOfFaces);
	p_object->m_faces = new ImportedFace [p_object->m_numOfFaces];
	memset( p_object->m_faces, 0, sizeof( ImportedFace) * p_object->m_numOfFaces);

	for(int i = 0; m_pFile->IsOk() && i < p_object->m_numOfFaces; i++)
	{
		for(int j = 0; m_pFile->IsOk() && j < 4; j++)
		{
			p_chunk->m_bytesRead += m_pFile->Read<unsigned short>( l_index);

			if(j < 3)
			{
				p_object->m_faces[i].m_vertIndex[j] = l_index;
			}
		}
	}
}

void SMaxImporter :: _readUVCoordinates( Imported3DObject * p_object, SMaxChunk * p_chunk)
{
	p_object->m_numTexVertex = 0;
	p_chunk->m_bytesRead += m_pFile->Read<short>( (short &)p_object->m_numTexVertex);
	p_object->m_texVerts = new ImportedVertex2[p_object->m_numTexVertex];
	p_chunk->m_bytesRead += m_pFile->ReadArray<char>( (char *)p_object->m_texVerts, p_chunk->m_length - p_chunk->m_bytesRead);
}

void SMaxImporter :: _readVertices( Imported3DObject * p_object, SMaxChunk * p_chunk)
{
	p_object->m_numOfVerts = 0;
	p_chunk->m_bytesRead += m_pFile->Read<short>( (short &)p_object->m_numOfVerts);
	p_object->m_vertex = new ImportedVertex3[p_object->m_numOfVerts];
	memset( p_object->m_vertex, 0, sizeof( ImportedVertex3) * p_object->m_numOfVerts);
	p_chunk->m_bytesRead += m_pFile->ReadArray<char>( (char *)p_object->m_vertex, p_chunk->m_length - p_chunk->m_bytesRead);

	for(int i = 0; i < p_object->m_numOfVerts; i++)
	{
		float l_tmp = p_object->m_vertex[i].y;
		p_object->m_vertex[i].y = p_object->m_vertex[i].z;
		p_object->m_vertex[i].z = -l_tmp;
	}
}

void SMaxImporter :: _readObjectMaterial(Imported3DModel * p_model, Imported3DObject * p_object, SMaxChunk * p_chunk)
{
	Char l_materialName[255] = {0};
	p_chunk->m_bytesRead += _getString( l_materialName);

	for (int i = 0; i < p_model->m_numOfMaterials; i++)
	{
		if (String( l_materialName) == p_model->m_materials[i].m_strName)
		{
			if (String( p_model->m_materials[i].m_strFile).size() > 0) 
			{				
				p_object->m_materialID = i;

				p_object->m_hasTexture = true;
			}	
			break;
		}
		else
		{
			if (p_object->m_hasTexture != true)
			{
				p_object->m_materialID = -1;
			}
		}
	}

	p_chunk->m_bytesRead += m_pFile->ReadArray<char>( g_buffer, p_chunk->m_length - p_chunk->m_bytesRead);
}