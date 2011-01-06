#include <Castor3D/Prerequisites.h>

using namespace Castor::Templates;

#include <Castor3D/render_system/RenderSystem.h>
#include <Castor3D/render_system/Buffer.h>
#include <Castor3D/scene/SceneNode.h>
#include <Castor3D/scene/SceneManager.h>
#include <Castor3D/scene/Scene.h>
#include <Castor3D/camera/Camera.h>
#include <Castor3D/camera/Viewport.h>
#include <Castor3D/material/MaterialManager.h>
#include <Castor3D/material/Material.h>
#include <Castor3D/material/Pass.h>
#include <Castor3D/material/TextureUnit.h>
#include <Castor3D/main/Version.h>
#include <Castor3D/geometry/primitives/Geometry.h>
#include <Castor3D/geometry/mesh/MeshManager.h>
#include <Castor3D/geometry/mesh/Mesh.h>
#include <Castor3D/geometry/mesh/Submesh.h>
#include <Castor3D/geometry/basic/SmoothingGroup.h>
#include <Castor3D/geometry/basic/Face.h>

#include "AseImporter/AseImporter.h"

using namespace Castor3D;

//*************************************************************************************************

extern "C" C3D_Ase_API void GetRequiredVersion( Version & p_version)
{
	p_version = Version( 0, 6);
}

extern "C" C3D_Ase_API Importer * CreateImporter( SceneManager * p_pManager)
{
	Importer * l_pReturn( new AseImporter( p_pManager));

	return l_pReturn;
}

//*************************************************************************************************

AseImporter :: AseImporter( SceneManager * p_pManager)
	:	Importer( p_pManager)
{
}

AseImporter :: ~AseImporter()
{
}

bool AseImporter :: _import()
{
	m_pFile = new File( m_fileName, File::eRead);

	_readAseFile();

	MeshPtr l_pMesh;

	for (GeometryPtrStrMap::iterator l_it = m_geometries.begin() ; l_it != m_geometries.end() ; ++l_it)
	{
		l_pMesh = l_it->second->GetMesh();
		l_pMesh->ComputeContainers();

		for (size_t i = 0 ; i < l_pMesh->GetNbSubmeshes() ; i++)
		{
			l_pMesh->GetSubmesh( i)->GenerateBuffers();
		}

		l_pMesh->ComputeNormals();
	}

//	vector::deleteAll( m_texCoords);
	m_texCoords.clear();

	delete m_pFile;

	return true;
}

void AseImporter :: _readAseFile()
{
	m_pScene = m_pManager->GetElementByName( "MainScene");

	while (m_pFile->IsOk())
	{
		m_pFile->ReadWord( m_currentWord);

		if (m_currentWord == SCENE)
		{
			_readSceneInfos();
		}
		else if (m_currentWord == MATERIAL || m_currentWord == SUBMATERIAL)
		{
			m_materials.insert( MaterialPtrUIntMap::value_type( m_materials.size(), _readMaterialInfos()));
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

MaterialPtr AseImporter :: _readMaterialInfos()
{
	MaterialPtr l_material;
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
			l_diffuse[0] = m_currentWord.ToFloat();
			m_pFile->ReadWord( m_currentWord);
			l_diffuse[1] = m_currentWord.ToFloat();
			m_pFile->ReadWord( m_currentWord);
			l_diffuse[2] = m_currentWord.ToFloat();
			l_material->GetPass( 0)->SetDiffuse( l_diffuse);
		}
		else if (m_currentWord == MATERIAL_AMBIENT)
		{
			m_pFile->ReadWord( m_currentWord);
			l_ambient[0] = m_currentWord.ToFloat();
			m_pFile->ReadWord( m_currentWord);
			l_ambient[1] = m_currentWord.ToFloat();
			m_pFile->ReadWord( m_currentWord);
			l_ambient[2] = m_currentWord.ToFloat();
			l_material->GetPass( 0)->SetAmbient( l_ambient);
		}
		else if (m_currentWord == MATERIAL_SPECULAR)
		{
			m_pFile->ReadWord( m_currentWord);
			l_specular[0] = m_currentWord.ToFloat();
			m_pFile->ReadWord( m_currentWord);
			l_specular[1] = m_currentWord.ToFloat();
			m_pFile->ReadWord( m_currentWord);
			l_specular[2] = m_currentWord.ToFloat();
			l_material->GetPass( 0)->SetSpecular( l_specular);
		}
		else if (m_currentWord == MATERIAL_SHININESS)
		{
			m_pFile->ReadWord( m_currentWord);
			l_shininess = m_currentWord.ToFloat();
			l_material->GetPass( 0)->SetShininess( l_shininess * 128.0f);
		}
		else if (m_currentWord == TEXTURE)
		{
			PassPtr l_pass = l_material->GetPass( 0);
			size_t l_i = std::min( m_fileName.find_last_of( "/"), m_fileName.find_last_of( "\\"));
			String l_filePath = m_fileName.substr( 0, l_i + 1);
			TextureUnitPtr l_unit = l_pass->AddTextureUnit();
			String l_texPath = _retrieveString();

			if (File::FileExists( l_texPath))
			{
				l_unit->SetTexture2D( m_pManager->GetImageManager()->CreateImage( l_texPath, l_texPath));
			}
			else if (File::FileExists( l_filePath + l_texPath))
			{
				l_unit->SetTexture2D( m_pManager->GetImageManager()->CreateImage( l_filePath + l_texPath, l_filePath + l_texPath));
			}
			else
			{
				l_pass->DestroyTextureUnit( l_unit->GetIndex());
			}
		}
		else if (m_currentWord == MATERIAL_NAME)
		{
			l_material = m_pManager->GetMaterialManager()->CreateMaterial( _retrieveString());
			l_material->GetPass( 0)->SetDoubleFace( true);
		}
	}

	l_material->GetPass( 0)->SetAlpha( 1.0);

	return l_material;
}

void AseImporter :: _readLightInfos()
{
}

void AseImporter :: _readCameraInfos()
{
	CameraPtr l_camera;
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
			SceneNodePtr l_node = _readNodeInfos( l_nodeName);
			l_camera->GetParent()->Rotate( l_node->GetOrientation());
			l_camera->GetParent()->Translate( l_node->GetDerivedPosition());
			l_node.reset();
		}
		else if (m_currentWord == NODE_NAME)
		{
			l_nodeName = _retrieveString();
			l_camera = m_pScene->CreateCamera( l_nodeName, 800, 600, m_pScene->CreateSceneNode( l_nodeName), Viewport::e3DView);
		}
		else if (m_currentWord == CAMERA_SETTINGS)
		{
			_readCameraSettings( l_camera);
		}
	}
}

void AseImporter :: _readCameraSettings( CameraPtr p_camera)
{
	real l_near, l_far, l_fov;

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
			l_near = ator( m_currentWord.c_str());
			p_camera->GetViewport()->SetNearView( l_near);
		}
		else if (m_currentWord == CAMERA_FAR)
		{
			m_pFile->ReadWord( m_currentWord);
			l_far = ator( m_currentWord.c_str());
			p_camera->GetViewport()->SetFarView( l_far);
		}
		else if (m_currentWord == CAMERA_FOV)
		{
			m_pFile->ReadWord( m_currentWord);
			l_fov = ator( m_currentWord.c_str());
			p_camera->GetViewport()->SetFOVY( l_fov);
		}
	}
}

SceneNodePtr AseImporter :: _readNodeInfos( const String & p_name)
{
	SceneNodePtr l_node = m_pScene->CreateSceneNode( p_name);
	real l_angle;
	real l_position[3], l_axis[3], l_scale[3];
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
			l_position[0] = ator( m_currentWord.c_str());
			m_pFile->ReadWord( m_currentWord);
			l_position[1] = ator( m_currentWord.c_str());
			m_pFile->ReadWord( m_currentWord);
			l_position[2] = ator( m_currentWord.c_str());
			l_node->SetPosition( l_position);
		}
		else if (m_currentWord == NODE_AXIS)
		{
			m_pFile->ReadWord( m_currentWord);
			l_axis[0] = ator( m_currentWord.c_str());
			m_pFile->ReadWord( m_currentWord);
			l_axis[1] = ator( m_currentWord.c_str());
			m_pFile->ReadWord( m_currentWord);
			l_axis[2] = ator( m_currentWord.c_str());
			l_gotAxis = true;

			if (l_gotAngle)
			{
				l_node->SetOrientation( Quaternion( l_axis, l_angle));
			}
		}
		else if (m_currentWord == NODE_ANGLE)
		{
			m_pFile->ReadWord( m_currentWord);
			l_angle = ator( m_currentWord.c_str());
			l_gotAngle = true;

			if (l_gotAxis)
			{
				l_node->SetOrientation( Quaternion( l_axis, l_angle));
			}
		}
		else if (m_currentWord == NODE_SCALE)
		{
			m_pFile->ReadWord( m_currentWord);
			l_scale[0] = ator( m_currentWord.c_str());
			m_pFile->ReadWord( m_currentWord);
			l_scale[1] = ator( m_currentWord.c_str());
			m_pFile->ReadWord( m_currentWord);
			l_scale[2] = ator( m_currentWord.c_str());
			l_node->SetScale( l_scale);
		}
	}

	return l_node;
}

void AseImporter :: _readObjectInfos()
{
	GeometryPtr l_geometry;// = NULL;
	SceneNodePtr l_node;// = NULL;
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
			MeshPtr l_mesh;

			if ( ! m_pManager->GetMeshManager()->HasElement( l_nodeName))
			{
				l_mesh = m_pManager->GetMeshManager()->CreateMesh( l_nodeName, l_faces, l_sizes, Mesh::eCustom);
				l_geometry = GeometryPtr( new Geometry( l_mesh, l_node, l_nodeName));
				Logger::LogMessage( CU_T( "CreatePrimitive - Mesh %s created"), l_nodeName.c_str());
				_readMeshInfos( l_mesh);
				m_geometries.insert( GeometryPtrStrMap::value_type( l_nodeName, l_geometry));
				m_nodes.insert( SceneNodePtrStrMap::value_type( l_nodeName, l_node));
			}
		}
		else if (m_currentWord == MATERIAL_ID)
		{
			m_pFile->ReadWord( m_currentWord);
			int l_id = m_currentWord.ToInt();
			MeshPtr l_mesh( l_geometry->GetMesh());

			for (size_t i = 0 ; i < l_mesh->GetNbSubmeshes() ; i++)
			{
				l_mesh->GetSubmesh( i)->SetMaterial( m_materials.find( (size_t)l_id)->second);
			}
		}
	}
}

void AseImporter :: _readMeshInfos( MeshPtr p_mesh)
{
	SubmeshPtr l_submesh;
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
//			vector::deleteAll( m_texCoords);
			m_texCoords.clear();
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
			l_id = m_currentWord.ToInt();
			l_submesh->SetMaterial( m_materials.find( (size_t)l_id)->second);
		}
	}
}

void AseImporter :: _readVertexList( SubmeshPtr p_submesh)
{
	real l_position[3];

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
			p_submesh->AddPoint( l_position[0], l_position[1], l_position[2]);
		}
	}
}

void AseImporter :: _readFaceList( SubmeshPtr p_submesh)
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

void AseImporter :: _readTexCoordsList( SubmeshPtr p_submesh)
{
	real l_position[3];

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
			m_texCoords.push_back( Point3rPtr( new Point3r( l_position)));
		}
	}
}

void AseImporter :: _readTexFaceList( SubmeshPtr p_submesh)
{
	int l_indices[4];

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

			FacePtr l_face = p_submesh->GetSmoothGroup( 0)->GetFace( l_indices[0]);
			l_face->SetVertexTexCoords( 0, m_texCoords[ l_indices[1] ]->m_coords[0], m_texCoords[ l_indices[1] ]->m_coords[1]);
			l_face->SetVertexTexCoords( 1, m_texCoords[ l_indices[2] ]->m_coords[0], m_texCoords[ l_indices[2] ]->m_coords[1]);
			l_face->SetVertexTexCoords( 2, m_texCoords[ l_indices[3] ]->m_coords[0], m_texCoords[ l_indices[3] ]->m_coords[1]);
		}
	}
}

void AseImporter :: _readMeshMappingChannel( SubmeshPtr p_submesh)
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

void AseImporter :: _readMeshNormals( SubmeshPtr p_submesh)
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
		size_t l_index = l_string.find_first_of( CU_T( "\""));
		l_string = l_string.substr( l_index+1, String::npos);
		l_index = l_string.find_last_of( CU_T( "\""));
		l_string = l_string.substr( 0, l_index);
		return l_string;
	}
	return C3DEmptyString;
}

void AseImporter :: _retrieveVertex( real * p_position)
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

void AseImporter :: _retrieveTextureVertex( real * p_position)
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