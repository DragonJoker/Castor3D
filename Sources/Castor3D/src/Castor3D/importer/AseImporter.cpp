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
	m_pFile = new FileIO( m_fileName, FileIO::eRead);

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
			return l_material;
		}
		else if (m_currentWord == MATERIAL ||m_currentWord == SUBMATERIAL)
		{
			_readMaterialInfos();
		}
		else if (m_currentWord == MATERIAL_DIFFUSE)
		{
			m_pFile->ReadWord( m_currentWord);
			l_diffuse[0] = (float)atof( m_currentWord.c_str());
			m_pFile->ReadWord( m_currentWord);
			l_diffuse[1] = (float)atof( m_currentWord.c_str());
			m_pFile->ReadWord( m_currentWord);
			l_diffuse[2] = (float)atof( m_currentWord.c_str());
			l_pass->SetDiffuse( l_diffuse);
		}
		else if (m_currentWord == MATERIAL_AMBIENT)
		{
			m_pFile->ReadWord( m_currentWord);
			l_ambient[0] = (float)atof( m_currentWord.c_str());
			m_pFile->ReadWord( m_currentWord);
			l_ambient[1] = (float)atof( m_currentWord.c_str());
			m_pFile->ReadWord( m_currentWord);
			l_ambient[2] = (float)atof( m_currentWord.c_str());
			l_pass->SetAmbient( l_ambient);
		}
		else if (m_currentWord == MATERIAL_SPECULAR)
		{
			m_pFile->ReadWord( m_currentWord);
			l_specular[0] = (float)atof( m_currentWord.c_str());
			m_pFile->ReadWord( m_currentWord);
			l_specular[1] = (float)atof( m_currentWord.c_str());
			m_pFile->ReadWord( m_currentWord);
			l_specular[2] = (float)atof( m_currentWord.c_str());
			l_pass->SetSpecular( l_specular);
		}
		else if (m_currentWord == MATERIAL_SHININESS)
		{
			m_pFile->ReadWord( m_currentWord);
			l_shininess = (float)atof( m_currentWord.c_str());
			l_pass->SetShininess( l_shininess * 128.0f);
		}
		else if (m_currentWord == TEXTURE)
		{
			size_t l_i = min( m_fileName.find_last_of( C3D_T( "/")), m_fileName.find_last_of( C3D_T( "\\")));
			String l_filePath = m_fileName.substr( 0, l_i + 1);
			TextureUnit * l_unit = new TextureUnit( RenderSystem::GetSingletonPtr()->CreateTextureRenderer());
			String l_texPath = _retrieveString();

			if (FileBase::FileExists( l_texPath))
			{
				l_unit->SetTexture2D( l_texPath);
				l_pass->AddTextureUnit( l_unit);
			}
			else if (FileBase::FileExists( l_filePath + l_texPath))
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
			l_material = MaterialManager::GetSingletonPtr()->CreateMaterial( _retrieveString());
			l_pass = l_material->GetPass( 0);
			l_pass->SetDoubleFace( true);
		}
	}

	l_pass->SetAlpha( 1.0);

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
			m_pFile->ReadWord( m_currentWord);
			l_near = (float)atof( m_currentWord.c_str());
			p_camera->GetViewport()->SetNearView( l_near);
		}
		else if (m_currentWord == CAMERA_FAR)
		{
			m_pFile->ReadWord( m_currentWord);
			l_far = (float)atof( m_currentWord.c_str());
			p_camera->GetViewport()->SetFarView( l_far);
		}
		else if (m_currentWord == CAMERA_FOV)
		{
			m_pFile->ReadWord( m_currentWord);
			l_fov = (float)atof( m_currentWord.c_str());
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
			m_pFile->ReadWord( m_currentWord);
			l_position[0] = (float)atof( m_currentWord.c_str());
			m_pFile->ReadWord( m_currentWord);
			l_position[1] = (float)atof( m_currentWord.c_str());
			m_pFile->ReadWord( m_currentWord);
			l_position[2] = (float)atof( m_currentWord.c_str());
			l_node->SetPosition( l_position);
		}
		else if (m_currentWord == NODE_AXIS)
		{
			m_pFile->ReadWord( m_currentWord);
			l_axis[0] = (float)atof( m_currentWord.c_str());
			m_pFile->ReadWord( m_currentWord);
			l_axis[1] = (float)atof( m_currentWord.c_str());
			m_pFile->ReadWord( m_currentWord);
			l_axis[2] = (float)atof( m_currentWord.c_str());
			l_gotAxis = true;

			if (l_gotAngle)
			{
				l_node->SetOrientation( Quaternion( l_axis, l_angle));
			}
		}
		else if (m_currentWord == NODE_ANGLE)
		{
			m_pFile->ReadWord( m_currentWord);
			l_angle = (float)atof( m_currentWord.c_str());
			l_gotAngle = true;

			if (l_gotAxis)
			{
				l_node->SetOrientation( Quaternion( l_axis, l_angle));
			}
		}
		else if (m_currentWord == NODE_SCALE)
		{
			m_pFile->ReadWord( m_currentWord);
			l_scale[0] = (float)atof( m_currentWord.c_str());
			m_pFile->ReadWord( m_currentWord);
			l_scale[1] = (float)atof( m_currentWord.c_str());
			m_pFile->ReadWord( m_currentWord);
			l_scale[2] = (float)atof( m_currentWord.c_str());
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
		else if (m_currentWord == MATERIAL_ID)
		{
			m_pFile->ReadWord( m_currentWord);
			int l_id = atoi( m_currentWord.c_str());
			Mesh * l_mesh = l_geometry->GetMesh();

			for (size_t i = 0 ; i < l_mesh->GetNbSubmeshes() ; i++)
			{
				l_mesh->GetSubmesh( i)->SetMaterial( m_materials.find( (size_t)l_id)->second);
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
			m_pFile->ReadWord( m_currentWord);
			l_id = atoi( m_currentWord.c_str());
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