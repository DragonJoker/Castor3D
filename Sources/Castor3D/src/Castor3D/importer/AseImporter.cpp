#include "PrecompiledHeader.h"

#include "importer/Module_Importer.h"

#include "importer/AseImporter.h"
#include "render_system/RenderSystem.h"
#include "render_system/Buffer.h"
#include "render_system/MeshRenderer.h"
#include "scene/SceneNode.h"
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
#include "Log.h"

using namespace Castor3D;

bool AseImporter :: _import()
{
	m_pFile = new File( m_fileName, File::eRead);

	_readAseFile();

	Mesh * l_pMesh;

	for (std::map <String, Geometry *>::iterator l_it = m_geometries.begin() ; l_it != m_geometries.end() ; ++l_it)
	{
		l_pMesh = l_it->second->GetMesh();
		l_pMesh->ComputeContainers();

		for (size_t i = 0 ; i < l_pMesh->GetNbSubmeshes() ; i++)
		{
			l_pMesh->GetSubmesh( i)->GenerateBuffers();
		}

		l_pMesh->SetNormals();
	}

	vector::deleteAll( m_texCoords);

	delete m_pFile;

	return true;
}

void AseImporter :: _readAseFile()
{
	m_pScene = SceneManager::GetSingletonPtr()->GetElementByName( "MainScene");

	while (m_pFile->IsOk())
	{
		m_pFile->ReadWord( m_currentWord);

		if (m_currentWord == SCENE)
		{
			_readSceneInfos();
		}
		else if (m_currentWord == MATERIAL || m_currentWord == SUBMATERIAL)
		{
			m_materials.insert( std::map <size_t, Material *>::value_type( m_materials.size(), _readMaterialInfos()));
		}
		else if (m_currentWord == OBJECT)
		{
			_readObjectInfos();
		}
		else if (m_currentWord == LIGHT)
		{
			_readLightInfos();
		}
		else if (m_currentWord == CAMERA)
		{
			_readCameraInfos();
		}
	}
}

void AseImporter :: _readSceneInfos()
{
	while (m_pFile->IsOk())
	{
		m_pFile->ReadWord( m_currentWord);

		if (m_currentWord == ENDER)
		{
			return;
		}
	}
}

Material * AseImporter :: _readMaterialInfos()
{
	Material * l_material;
	Pass * l_pass;
	float l_ambient[3];
	float l_diffuse[3];
	float l_specular[3];
	float l_shininess;
	while (m_pFile->IsOk())
	{
		m_pFile->ReadWord( m_currentWord);

		if (m_currentWord == ENDER)
		{
			MaterialManager::GetSingletonPtr()->AddElement( l_material);
			return l_material;
		}
		else if (m_currentWord == MATERIAL ||m_currentWord == SUBMATERIAL)
		{
			_readMaterialInfos();
		}
		else if (m_currentWord == MATERIAL_DIFFUSE)
		{
			m_pFile->ReadArray<float>( l_diffuse, 3);
			l_pass->SetDiffuse( l_diffuse);
		}
		else if (m_currentWord == MATERIAL_AMBIENT)
		{
			m_pFile->ReadArray<float>( l_ambient, 3);
			l_pass->SetAmbient( l_ambient);
		}
		else if (m_currentWord == MATERIAL_SPECULAR)
		{
			m_pFile->ReadArray<float>( l_specular, 3);
			l_pass->SetSpecular( l_specular);
		}
		else if (m_currentWord == MATERIAL_SHININESS)
		{
			m_pFile->Read<float>( l_shininess);
			l_pass->SetShininess( l_shininess);
		}
		else if (m_currentWord == TEXTURE)
		{
			size_t l_i = min( m_fileName.find_last_of( C3D_T( "/")), m_fileName.find_last_of( C3D_T( "\\")));
			String l_filePath = m_fileName.substr( 0, l_i + 1);
			TextureUnit * l_unit = new TextureUnit( RenderSystem::GetSingletonPtr()->CreateTextureRenderer());
			String l_texPath = _retrieveString();

			if (File::FileExists( l_texPath))
			{
				l_unit->SetTexture2D( l_texPath);
				l_pass->AddTextureUnit( l_unit);
			}
			else if (File::FileExists( l_filePath + l_texPath))
			{
				l_unit->SetTexture2D( l_filePath + l_texPath);
				l_pass->AddTextureUnit( l_unit);
			}
			else
			{
				delete l_unit;
			}
		}
		else if (m_currentWord == MATERIAL_NAME)
		{
			l_material = new Material( _retrieveString());
			l_pass = l_material->GetPass( 0);
			l_pass->SetDoubleFace( true);
		}
	}
	return l_material;
}

void AseImporter :: _readLightInfos()
{
}

void AseImporter :: _readCameraInfos()
{
	Camera * l_camera;
	String l_nodeName;

	while (m_pFile->IsOk())
	{
		m_pFile->ReadWord( m_currentWord);

		if (m_currentWord == ENDER)
		{
			return;
		}
		else if (m_currentWord == CAMERA)
		{
			_readCameraInfos();
		}
		else if (m_currentWord == NODE)
		{
			SceneNode * l_node = _readNodeInfos( l_nodeName);
			l_camera->Rotate( l_node->GetOrientation());
			l_camera->Translate( l_node->GetDerivedPosition());
			delete l_node;
		}
		else if (m_currentWord == NODE_NAME)
		{
			l_nodeName = _retrieveString();
			l_camera = m_pScene->CreateCamera( l_nodeName, 800, 600, pt3DView);
		}
		else if (m_currentWord == CAMERA_SETTINGS)
		{
			_readCameraSettings( l_camera);
		}
	}
}

void AseImporter :: _readCameraSettings( Camera * p_camera)
{
	float l_near, l_far, l_fov;

	while (m_pFile->IsOk())
	{
		m_pFile->ReadWord( m_currentWord);

		if (m_currentWord == ENDER)
		{
			return;
		}
		else if (m_currentWord == CAMERA_NEAR)
		{
			m_pFile->Read<float>( l_near);
			p_camera->GetViewport()->SetNearView( l_near);
		}
		else if (m_currentWord == CAMERA_FAR)
		{
			m_pFile->Read<float>( l_far);
			p_camera->GetViewport()->SetFarView( l_far);
		}
		else if (m_currentWord == CAMERA_FOV)
		{
			m_pFile->Read<float>( l_fov);
			p_camera->GetViewport()->SetFOVY( l_fov);
		}
	}
}

SceneNode * AseImporter :: _readNodeInfos( const String & p_name)
{
	SceneNode * l_node = m_pScene->CreateSceneNode( p_name);
	float l_angle;
	float l_position[3], l_axis[3], l_scale[3];
	bool l_gotAngle = false;
	bool l_gotAxis = false;

	while (m_pFile->IsOk())
	{
		m_pFile->ReadWord( m_currentWord);

		if (m_currentWord == ENDER)
		{
			return l_node;
		}
		else if (m_currentWord == NODE_POSITION)
		{
			m_pFile->ReadArray<float>( l_position, 3);
			l_node->SetPosition( l_position);
		}
		else if (m_currentWord == NODE_AXIS)
		{
			m_pFile->ReadArray<float>( l_axis, 3);
			l_gotAxis = true;
			if (l_gotAngle)
			{
				l_node->SetOrientation( Quaternion( l_axis, l_angle));
			}
		}
		else if (m_currentWord == NODE_ANGLE)
		{
			m_pFile->Read<float>( l_angle);
			l_gotAngle = true;
			if (l_gotAxis)
			{
				l_node->SetOrientation( Quaternion( l_axis, l_angle));
			}
		}
		else if (m_currentWord == NODE_SCALE)
		{
			m_pFile->ReadArray<float>( l_scale, 3);
			l_node->SetScale( l_scale);
		}
	}
	return l_node;
}

void AseImporter :: _readObjectInfos()
{
	Geometry * l_geometry = NULL;
	SceneNode * l_node = NULL;
	String l_nodeName;

	while (m_pFile->IsOk())
	{
		m_pFile->ReadWord( m_currentWord);

		if (m_currentWord == ENDER)
		{
			return;
		}
		else if (m_currentWord == OBJECT)
		{
			_readObjectInfos();
		}
		else if (m_currentWord == NODE)
		{
			l_node = _readNodeInfos( l_nodeName);
		}
		else if (m_currentWord == NODE_NAME)
		{
			l_nodeName = _retrieveString();
		}
		else if (m_currentWord == MESH)
		{
			UIntArray l_faces;
			FloatArray l_sizes;
			Mesh * l_mesh = NULL;

			if ( ! MeshManager::GetSingletonPtr()->HasElement( l_nodeName))
			{
				l_mesh = MeshManager::GetSingleton().CreateMesh( l_nodeName, l_faces, l_sizes, Mesh::eCustom);
				l_geometry = new Geometry( l_mesh, l_node, l_nodeName);
				Log::LogMessage( C3D_T( "CreatePrimitive - Mesh %s created"), l_nodeName.c_str());
				_readMeshInfos( l_mesh);
				m_geometries.insert( std::map <String, Geometry *>::value_type( l_nodeName, l_geometry));
				m_nodes.insert( std::map <String, SceneNode *>::value_type( l_nodeName, l_node));
			}
		}
	}
}

void AseImporter :: _readMeshInfos( Mesh * p_mesh)
{
	Submesh * l_submesh;
	int l_id;

	while (m_pFile->IsOk())
	{
		m_pFile->ReadWord( m_currentWord);

		if (m_currentWord == ENDER)
		{
			return;
		}
		else if (m_currentWord == MESH_VERTEX_LIST)
		{
			l_submesh = p_mesh->CreateSubmesh( 1);
			l_submesh->GetRenderer()->GetTriangles()->Cleanup();
			l_submesh->GetRenderer()->GetTrianglesTexCoords()->Cleanup();
			l_submesh->GetRenderer()->GetLines()->Cleanup();
			l_submesh->GetRenderer()->GetLinesTexCoords()->Cleanup();
			_readVertexList( l_submesh);
		}
		else if (m_currentWord == MESH_FACE_LIST)
		{
			_readFaceList( l_submesh);
		}
		else if (m_currentWord == MESH_TVERTLIST)
		{
			_readTexCoordsList( l_submesh);
		}
		else if (m_currentWord == MESH_TFACELIST)
		{
			_readTexFaceList( l_submesh);
			vector::deleteAll( m_texCoords);
		}
		else if (m_currentWord == MESH_MAPPINGCHANNEL)
		{
			_readMeshMappingChannel( l_submesh);
		}
		else if (m_currentWord == NORMALS)
		{
			_readMeshNormals( l_submesh);
		}
		else if (m_currentWord == MATERIAL_ID)
		{
			m_pFile->Read<int>( l_id);
			l_submesh->SetMaterial( m_materials.find( (size_t)l_id)->second);
		}
	}
}

void AseImporter :: _readVertexList( Submesh * p_submesh)
{
	float l_position[3];

	while (m_pFile->IsOk())
	{
		m_pFile->ReadWord( m_currentWord);

		if (m_currentWord == ENDER)
		{
			return;
		}
		else if (m_currentWord == VERTEX)
		{
			_retrieveVertex( l_position);
			p_submesh->AddVertex( l_position[0], l_position[1], l_position[2]);
		}
	}
}

void AseImporter :: _readFaceList( Submesh * p_submesh)
{
	int l_indices[3];

	while (m_pFile->IsOk())
	{
		m_pFile->ReadWord( m_currentWord);

		if (m_currentWord == ENDER)
		{
			return;
		}
		else if (m_currentWord == FACE)
		{
			_retrieveFace( l_indices);
			p_submesh->AddFace( l_indices[0], l_indices[1], l_indices[2], 0);
		}
	}
}

void AseImporter :: _readTexCoordsList( Submesh * p_submesh)
{
	float l_position[3];

	while (m_pFile->IsOk())
	{
		m_pFile->ReadWord( m_currentWord);

		if (m_currentWord == ENDER)
		{
			return;
		}
		else if (m_currentWord == TVERTEX)
		{
			_retrieveTextureVertex( l_position);
			m_texCoords.push_back( new Vector3f( l_position));
		}
	}
}

void AseImporter :: _readTexFaceList( Submesh * p_submesh)
{
	int l_indices[4];
	Face * l_face;

	while (m_pFile->IsOk())
	{
		m_pFile->ReadWord( m_currentWord);

		if (m_currentWord == ENDER)
		{
			return;
		}
		else if (m_currentWord == TFACE)
		{
			_retrieveTextureFace( l_indices);

			l_face = p_submesh->GetSmoothGroup( 0)->m_faces[ l_indices[0] ];
			SetTexCoordV1( l_face, m_texCoords[ l_indices[1] ]->x, m_texCoords[ l_indices[1] ]->y);
			SetTexCoordV2( l_face, m_texCoords[ l_indices[2] ]->x, m_texCoords[ l_indices[2] ]->y);
			SetTexCoordV3( l_face, m_texCoords[ l_indices[3] ]->x, m_texCoords[ l_indices[3] ]->y);
		}
	}
}

void AseImporter :: _readMeshMappingChannel( Submesh * p_submesh)
{
	while (m_pFile->IsOk())
	{
		m_pFile->ReadWord( m_currentWord);

		if (m_currentWord == ENDER)
		{
			return;
		}
		else if (m_currentWord == MESH_TVERTLIST)
		{
			while (m_pFile->IsOk() && m_currentWord != ENDER)
			{
				m_pFile->ReadWord( m_currentWord);
			}
		}
		else if (m_currentWord == MESH_TFACELIST)
		{
			while (m_pFile->IsOk() && m_currentWord != ENDER)
			{
				m_pFile->ReadWord( m_currentWord);
			}
		}
	}
}

void AseImporter :: _readMeshNormals( Submesh * p_submesh)
{
	while (m_pFile->IsOk())
	{
		m_pFile->ReadWord( m_currentWord);

		if (m_currentWord == ENDER)
		{
			return;
		}
	}
}

String AseImporter :: _retrieveString()
{
	m_pFile->ReadLine( m_currentWord, 256);

	if (m_pFile->IsOk())
	{
		String l_string( m_currentWord);
		size_t l_index = l_string.find_first_of( C3D_T( "\""));
		l_string = l_string.substr( l_index+1, String::npos);
		l_index = l_string.find_last_of( C3D_T( "\""));
		l_string = l_string.substr( 0, l_index);
		return l_string;
	}
	return C3DEmptyString;
}

void AseImporter :: _retrieveVertex( float * p_position)
{
	int l_index = 0;
	String l_tmp;

	if (m_pFile->ReadLine( l_tmp, 256))
	{
		sscanf_s( l_tmp.char_str(), "%d %f %f %f", & l_index, & p_position[0], & p_position[1], & p_position[2]);
	}
}

void AseImporter :: _retrieveFace( int * p_indices)
{
	int l_index = 0;
	String l_tmp;

	if (m_pFile->ReadLine( l_tmp, 256))
	{
		sscanf_s( l_tmp.char_str(), "%d:\tA:\t%d B:\t%d C:\t%d", & l_index, & p_indices[0], & p_indices[1], & p_indices[2]);
	}
}

void AseImporter :: _retrieveTextureVertex( float * p_position)
{
	int l_index = 0;
	String l_tmp;

	if (m_pFile->ReadLine( l_tmp, 256))
	{
		sscanf_s( l_tmp.char_str(), "%d %f %f %f", & l_index, & p_position[0], & p_position[1], & p_position[2]);
	}
}

void AseImporter :: _retrieveTextureFace( int * p_indices)
{
	String l_tmp;

	if (m_pFile->ReadLine( l_tmp, 256))
	{
		sscanf_s( l_tmp.char_str(), "%d %d %d %d", & p_indices[0], & p_indices[1], & p_indices[2], & p_indices[3]);
	}
}
/*
void AseImporter :: _readAseFile( Imported3DModel * p_model)
{
	ImportedMaterialInfo l_newMaterial = {0};
	Imported3DObject     l_newObject = {0};
	int i = 0;

	p_model->m_numOfObjects = _getObjectCount();
	p_model->m_numOfMaterials = _getMaterialCount();
	int l_nbNodes = _getNodeCount();

	for (i = 0 ; i < p_model->m_numOfMaterials ; i++)
	{
		p_model->m_materials.push_back( l_newMaterial);
		_getTextureInfo( & (p_model->m_materials[i]), i + 1);
	}

	SceneNode * l_node;
	for (i = 0 ; i < l_nbNodes ; i++)
	{
		l_node = new SceneNode( RenderSystem::GetSingletonPtr()->CreateSceneNodeRenderer());
		m_nodes.push_back( l_node);
		_getNodeInfo( l_node, i + 1);
	}

	for (i = 0 ; i < p_model->m_numOfObjects ; i++)
	{	
		p_model->m_objects.push_back( l_newObject);
		p_model->m_objects[i].m_materialID = -1;
		_moveToObject( i + 1);
		_readObjectInfo( & (p_model->m_objects[i]), i + 1);
		_readObjectData( p_model, & (p_model->m_objects[i]), i + 1);
	}
}

int AseImporter :: _getObjectCount()
{
	char l_strWord[255] = {0};
	int l_objectCount = 0;
	m_fileStream->seekg( 0);

	while ( ! m_fileStream->fail())
	{
		*m_fileStream >> l_strWord;

		if ( ! strcmp( l_strWord, OBJECT))
		{
			l_objectCount++;
		}
	}

	return l_objectCount;
}

int AseImporter :: _getNodeCount()
{
	char l_strWord[255] = {0};
	int l_nodeCount = 0;
	m_fileStream->seekg( 0);

	while ( ! m_fileStream->fail())
	{
		*m_fileStream >> l_strWord;

		if ( ! strcmp( l_strWord, NODE))
		{
			l_nodeCount++;
		}
	}

	return l_nodeCount;
}

int AseImporter :: _getMaterialCount()
{
	char l_strWord[255] = {0};
	int l_materialCount = 0;
	m_fileStream->clear();
	m_fileStream->seekg( 0);

	while ( ! m_fileStream->fail())
	{
		*m_fileStream >> l_strWord;

		if ( ! strcmp( l_strWord, MATERIAL_COUNT))
		{
			*m_fileStream >> l_materialCount;

			return l_materialCount;
		}
	}

	return 0;
}

void AseImporter :: _getTextureInfo ( ImportedMaterialInfo * p_texture, int p_desiredMaterial)
{
	char l_strWord[255] = {0};
	int l_materialCount= 0;
	m_fileStream->clear();
	m_fileStream->seekg( 0);

	while ( ! m_fileStream->fail())
	{
		*m_fileStream >> l_strWord;

		if ( ! strcmp( l_strWord, MATERIAL))
		{
			l_materialCount++;

			if(l_materialCount == p_desiredMaterial)
			{
				break;
			}
		}
	}

	while ( ! m_fileStream->fail())
	{
		*m_fileStream >> l_strWord;

		if ( ! strcmp( l_strWord, MATERIAL))
		{
			return;
		}
		else if ( ! strcmp( l_strWord, MATERIAL_DIFFUSE))
		{
			*m_fileStream >> (p_texture->m_fDiffuse[2]) >> (p_texture->m_fDiffuse[1]) >> (p_texture->m_fDiffuse[0]);
		}
		else if ( ! strcmp( l_strWord, MATERIAL_AMBIENT))
		{
			*m_fileStream >> (p_texture->m_fAmbient[2]) >> (p_texture->m_fAmbient[1]) >> (p_texture->m_fAmbient[0]);
		}
		else if ( ! strcmp( l_strWord, MATERIAL_SPECULAR))
		{
			*m_fileStream >> (p_texture->m_fSpecular[0]) >> (p_texture->m_fSpecular[1]) >> (p_texture->m_fSpecular[2]);
		}
		else if ( ! strcmp( l_strWord, MATERIAL_SHININESS))
		{
			*m_fileStream >> (p_texture->m_fShininess);
		}
		else if ( ! strcmp( l_strWord, TEXTURE))
		{
			_getTextureName( p_texture);
		}
		else if ( ! strcmp( l_strWord, MATERIAL_NAME))
		{
			_getMaterialName( p_texture);
		}
		else if( ! strcmp( l_strWord, UTILE))
		{
			p_texture->m_uTile = _readFloat();
		}
		else if( ! strcmp( l_strWord, VTILE))
		{
			p_texture->m_vTile = _readFloat();
		}
	}
}
void AseImporter :: _getNodeInfo( SceneNode * p_node, int p_desiredNode)
{
	char l_strWord[255] = {0};
	m_fileStream->clear();
	m_fileStream->seekg( 0);
	int l_nodeCount = 0;

	while ( ! m_fileStream->fail())
	{
		*m_fileStream >> l_strWord;

		if ( ! strcmp( l_strWord, NODE))
		{
			l_nodeCount++;

			if (l_nodeCount == p_desiredNode)
			{
				break;
			}
		}
	}

	float x, y, z;
	float l_axis[3];
	float l_angle;
	bool l_foundAxis = false, l_foundAngle = false;
	while ( ! m_fileStream->fail())
	{
		*m_fileStream >> l_strWord;

		if ( ! strcmp( l_strWord, NODE) || ! strcmp( l_strWord, "}"))
		{
			return;
		}
		else if ( ! strcmp( l_strWord, NODE_NAME))
		{
			m_fileStream->getline( l_strWord, 256);
			memcpy( l_strWord, & l_strWord[2], strlen( l_strWord) - 2);
			l_strWord[strlen( l_strWord) - 3] = 0;
			p_node->SetName( l_strWord);
		}
		else if ( ! strcmp( l_strWord, NODE_POSITION))
		{
			*m_fileStream >> x >> y >> z;
			p_node->SetPosition( x, y, z);
		}
		else if ( ! strcmp( l_strWord, NODE_AXIS))
		{
			l_foundAxis = true;
			*m_fileStream >> l_axis[0] >> l_axis[1] >> l_axis[2];
		}
		else if ( ! strcmp( l_strWord, NODE_ANGLE))
		{
			l_foundAngle = true;
			*m_fileStream >> l_angle;
		}
		else if ( ! strcmp( l_strWord, NODE_SCALE))
		{
			*m_fileStream >> x >> y >> z;
			p_node->SetScale( x, y, z);
		}
	}
	if (l_foundAxis && l_foundAngle)
	{
		p_node->SetOrientation( Quaternion( l_angle, l_axis[0], l_axis[1], l_axis[2]));
	}
}

void AseImporter :: _moveToObject ( int p_desiredObject)
{
	char l_strWord[255] = {0};
	int l_objectCount = 0;
	m_fileStream->clear();
	m_fileStream->seekg( 0);

	while( ! m_fileStream->fail())
	{
		*m_fileStream >> l_strWord;

		if ( ! strcmp( l_strWord, OBJECT))
		{
			l_objectCount++;

			if (l_objectCount == p_desiredObject)
			{
				return;
			}
		}
	}
}

float AseImporter :: _readFloat()
{
	float v = 0.0f;
	*m_fileStream >> v;
	return v;
}

void AseImporter :: _readObjectInfo( Imported3DObject * p_object, int p_desiredObject)
{
	char l_strWord[255] = {0};
	_moveToObject( p_desiredObject);
	
	while ( ! m_fileStream->fail())
	{
		*m_fileStream >> l_strWord;

		if ( ! strcmp( l_strWord, NUM_VERTEX))
		{
			*m_fileStream >> p_object->m_numOfVerts;
			p_object->m_vertex = new ImportedVertex3[p_object->m_numOfVerts];
		}
		else if ( ! strcmp( l_strWord, NUM_FACES))
		{
			*m_fileStream >> p_object->m_numOfFaces;
			p_object->m_faces = new ImportedFace [p_object->m_numOfFaces];
		}
		else if ( ! strcmp( l_strWord, NUM_TVERTEX))
		{
			*m_fileStream >> p_object->m_numTexVertex;
			p_object->m_texVerts = new ImportedVertex2[p_object->m_numTexVertex];
		}
		else if ( ! strcmp( l_strWord, OBJECT))	
		{
			return;
		}
	}	
}

void AseImporter :: _getTextureName( ImportedMaterialInfo * p_texture)
{
	*m_fileStream >> (p_texture->m_strFile);
	memcpy( p_texture->m_strFile, & p_texture->m_strFile[1], strlen( p_texture->m_strFile) - 1);
	p_texture->m_strFile[strlen( p_texture->m_strFile) - 2] = '\0';
}

void AseImporter :: _getMaterialName( ImportedMaterialInfo * p_texture)
{
	*m_fileStream >> (p_texture->m_strName);
	memcpy( p_texture->m_strName, & p_texture->m_strName[1], strlen( p_texture->m_strName) - 1);
	p_texture->m_strName[strlen( p_texture->m_strName) - 1] = '\0';
}

void AseImporter :: _readObjectData( Imported3DModel * p_model, Imported3DObject * p_object, int p_desiredObject)
{
	_getData( p_model, p_object, MATERIAL_ID,p_desiredObject);
	_getData( p_model, p_object, VERTEX,	 p_desiredObject);
	_getData( p_model, p_object, TVERTEX,	 p_desiredObject);
	_getData( p_model, p_object, FACE,		 p_desiredObject);
	_getData( p_model, p_object, TFACE,		 p_desiredObject);
	_getData( p_model, p_object, TEXTURE,	 p_desiredObject);
	_getData( p_model, p_object, UTILE,		 p_desiredObject);
	_getData( p_model, p_object, VTILE,		 p_desiredObject);
}

void AseImporter :: _getData( Imported3DModel * p_model, Imported3DObject * p_object, char * p_strDesiredData, int p_desiredObject)
{
	char l_strWord[255] = {0};
	_moveToObject( p_desiredObject);

	while ( ! m_fileStream->fail())
	{
		*m_fileStream >> l_strWord;

		if ( ! strcmp( l_strWord, OBJECT))	
		{
			return;
		}
		else if ( ! strcmp( l_strWord, VERTEX))
		{
			if ( ! strcmp( p_strDesiredData, VERTEX)) 
			{
				_readVertex( p_object);
			}
		}
		else if ( ! strcmp( l_strWord, TVERTEX))
		{
			if ( ! strcmp( p_strDesiredData, TVERTEX)) 
			{
				_readTextureVertex( p_object, p_model->m_materials[p_object->m_materialID]);
			}
		}
		else if ( ! strcmp( l_strWord, FACE))
		{
			if ( ! strcmp( p_strDesiredData, FACE)) 
			{
				_readFace( p_object);
			}
		}
		else if( ! strcmp( l_strWord, TFACE))
		{
			if( ! strcmp( p_strDesiredData, TFACE))
			{
				_readTextureFace(p_object);
			}
		}
		else if( ! strcmp( l_strWord, MATERIAL_ID))
		{
			if( ! strcmp( p_strDesiredData, MATERIAL_ID))
			{
				p_object->m_materialID = (int)_readFloat();
				return;
			}				
		}
	}
}

void AseImporter :: _readVertex( Imported3DObject * p_object)
{
	int l_index = 0;
	float x, y, z;
	char l_tmp[256];

	m_fileStream->getline( l_tmp, 256);
	sscanf_s( l_tmp, "%d %f %f %f", & l_index, & x, & y, & z);
	p_object->m_vertex[l_index].x = x;
	p_object->m_vertex[l_index].y = y;
	p_object->m_vertex[l_index].z = -z;
}

void AseImporter :: _readTextureVertex( Imported3DObject * p_object, ImportedMaterialInfo p_texture)
{
	int l_index = 0;
	float x, y;
	char l_tmp[256];

	m_fileStream->getline( l_tmp, 256);
	sscanf_s( l_tmp, "%d %f %f", & l_index, & x, & y);
	p_object->m_texVerts[l_index].x = x * p_texture.m_uTile;
	p_object->m_texVerts[l_index].y = y * p_texture.m_vTile;

	p_object->m_hasTexture = true;
}

void AseImporter :: _readFace( Imported3DObject * p_object)
{
	int l_index = 0, a, b, c;
	char l_tmp[256];

	m_fileStream->getline( l_tmp, 256);
	sscanf_s( l_tmp, "%d:\tA:\t%d B:\t%d C:\t%d", & l_index, & a, & b, & c);
	p_object->m_faces[l_index].m_vertIndex[0] = a;
	p_object->m_faces[l_index].m_vertIndex[1] = c;
	p_object->m_faces[l_index].m_vertIndex[2] = b;
}

void AseImporter :: _readTextureFace( Imported3DObject * p_object)
{
	int l_index = 0, a, b, c;
	char l_tmp[256];

	m_fileStream->getline( l_tmp, 256);
	sscanf_s( l_tmp, "%d %d %d %d", & l_index, & a, & b, & c);
	p_object->m_faces[l_index].m_coordIndex[0] = a;
	p_object->m_faces[l_index].m_coordIndex[1] = c;
	p_object->m_faces[l_index].m_coordIndex[2] = b;
}
*/