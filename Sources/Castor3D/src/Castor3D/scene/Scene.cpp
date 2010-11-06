#include "PrecompiledHeader.h"

#include "scene/Module_Scene.h"

#include "scene/Scene.h"
#include "scene/SceneNode.h"
#include "camera/Camera.h"
#include "light/PointLight.h"
#include "light/SpotLight.h"
#include "light/DirectionalLight.h"
#include "geometry/primitives/Geometry.h"
#include "geometry/mesh/Mesh.h"
#include "geometry/mesh/Submesh.h"
#include "geometry/basic/SmoothingGroup.h"
#include "geometry/basic/Face.h"
#include "geometry/mesh/MeshManager.h"
#include "material/MaterialManager.h"
#include "material/Material.h"
#include "render_system/RenderSystem.h"
#include "main/Root.h"
#include "importer/SMaxImporter.h"
#include "importer/AseImporter.h"
#include "importer/Md2Importer.h"
#include "importer/Md3Importer.h"
#include "importer/ObjImporter.h"
#include "importer/BspImporter.h"
#include "importer/PlyImporter.h"
#include "camera/Ray.h"



using namespace Castor3D;

Scene :: Scene( const String & p_name)
	:	m_name			( p_name),
		m_rootNode		( new SceneNode( CU_T( "RootNode"))),
		m_rootCamera	( new Camera( CU_T( "RootCamera"), 800, 600, Viewport::pt3DView)),
		m_changed		( false)
{
	m_rootCamera->Translate( 0.0f, 0.0f, -5.0f);
}

Scene :: ~Scene()
{
	ClearScene();
	m_rootCamera.reset();
	m_rootNode.reset();
}

void Scene :: ClearScene()
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	RemoveAllNodes();
	RemoveAllGeometries();
	RemoveAllLights();
	RemoveAllCameras();
}

void Scene :: Render( eDRAW_TYPE p_displayMode, real p_tslf)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	if ( ! m_rootNode.null())
	{
		_deleteToDelete();

		LightPtrStrMap::iterator l_it = m_addedLights.begin();
		LightPtrStrMap::const_iterator l_end = m_addedLights.end();

		RenderSystem::GetSingletonPtr<RenderSystem>()->RenderAmbientLight( m_clAmbientLight);

		while (l_it != l_end)
		{
			l_it->second->Apply( p_displayMode);
			++l_it;
		}

		m_rootNode->Apply( p_displayMode);

		l_it = m_addedLights.begin();

		while (l_it != l_end)
		{
			l_it->second->Disable();
			++l_it;
		}
	}
}

SceneNodePtr Scene :: CreateSceneNode( const String & p_name, SceneNodePtr p_parent)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	SceneNodePtr l_pReturn = NULL;

	if (p_name != CU_T( "RootNode") && m_addedNodes.find( p_name) == m_addedNodes.end())
	{
		l_pReturn = new SceneNode( p_name);

		if ( ! p_parent.null())
		{
			l_pReturn->AttachTo( p_parent.get());
		}
		else
		{
			l_pReturn->AttachTo( m_rootNode.get());
		}

		m_addedNodes[p_name] = l_pReturn;
	}
	else
	{
		Log::LogMessage( CU_T( "CreateSceneNode - Can't create scene node %s - Another scene node with the same name already exists"), p_name.c_str());
		l_pReturn = m_addedNodes.find( p_name)->second;
	}

	return l_pReturn;
}



GeometryPtr Scene :: CreatePrimitive( const String & p_name, Mesh::eTYPE p_type, 
									const String & p_meshName, UIntArray p_faces,
									FloatArray p_size)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	GeometryPtr l_pReturn = NULL;

	if (m_addedPrimitives.find( p_name) == m_addedPrimitives.end())
	{
		MeshPtr l_mesh;

		if (MeshManager::HasElement( p_meshName))
		{
			l_mesh = MeshManager::GetElementByName( p_meshName);
		}
		else
		{
			l_mesh = MeshManager::CreateMesh( p_meshName, p_faces, p_size, p_type);
			Log::LogMessage( CU_T( "CreatePrimitive - Mesh %s created"), p_meshName.c_str());
			l_mesh->SetNormals();
			Log::LogMessage( CU_T( "CreatePrimitive - Normals setting finished"));
		}

		if ( ! l_mesh.null())
		{
			l_pReturn = new Geometry( l_mesh, NULL, p_name);
			Log::LogMessage( CU_T( "CreatePrimitive - Geometry %s created"), p_name.c_str());
			m_addedPrimitives[p_name] = l_pReturn;
			m_newlyAddedPrimitives[p_name] = l_pReturn;
			m_changed = true;
		}
		else
		{
			Log::LogMessage( CU_T( "CreatePrimitive - Can't create primitive %s - Mesh creation failed"), p_name.c_str());
		}
	}
	else
	{
		Log::LogMessage( CU_T( "CreatePrimitive - Can't create primitive %s - Another primitive with the same name already exists"), p_name.c_str());
	}

	return l_pReturn;
}



Camera * Scene :: CreateCamera( const String & p_name, int p_ww, int p_wh,
							   Viewport::eTYPE p_type)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	Camera * l_pReturn = NULL;

	if (m_addedCameras.find( p_name) != m_addedCameras.end())
	{
		Log::LogMessage( CU_T( "CreateCamera - Can't create camera %s - A camera with that name already exists"), p_name.c_str());
	}
	else
	{
		Log::LogMessage( CU_T( "CreateCamera - Creating Camera %s"), p_name.c_str());
		l_pReturn = new Camera( p_name, p_ww, p_wh, p_type);

		if ( ! m_rootCamera.null())
		{
			m_rootCamera = l_pReturn;
		}
		else
		{
			m_addedCameras[p_name] = l_pReturn;
		}
	}

	return l_pReturn;
}



LightPtr Scene :: CreateLight( Light::eTYPE p_type, const String & p_name)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	LightPtr l_pReturn = NULL;

	if (m_addedLights.find( p_name) != m_addedLights.end())
	{
		Log::LogMessage( CU_T( "CreateLight - Can't create light %s - A light with that name already exists"), p_name.c_str());
	}
	else if (m_addedLights.size() >= 8)
	{
		Log::LogMessage( CU_T( "CreateLight - Can't create light %s - max light number reached"), p_name.c_str());
	}
	else
	{
		if (p_type == Light::eSpot)
		{
			l_pReturn = new SpotLight( p_name);
			m_addedLights[p_name] = l_pReturn;
		}
		else if (p_type == Light::ePoint)
		{
			l_pReturn = new PointLight( p_name);
			m_addedLights[p_name] = l_pReturn;
		}
		else if (p_type == Light::eDirectional)
		{
			l_pReturn = new DirectionalLight( p_name);
			m_addedLights[p_name] = l_pReturn;
		}
		else
		{
			Log::LogMessage( CU_T( "CreateLight - Can't create light %s - unknown light type"), p_name.c_str());
		}
	}

	return l_pReturn;
}



void Scene :: CreateList( NormalsMode p_nm, bool p_showNormals)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	m_normalsMode = p_nm;

	if ( ! m_rootNode.null())
	{
		m_nbFaces = 0;
		m_nbVertex = 0;

		if (m_newlyAddedPrimitives.size() > 0)
		{
			GeometryPtrStrMap::iterator l_it = m_newlyAddedPrimitives.begin();

			for ( ; l_it != m_newlyAddedPrimitives.end() ; ++l_it)
			{
				l_it->second->CreateBuffers( m_normalsMode, m_nbFaces, m_nbVertex);
			}

			m_newlyAddedPrimitives.clear();
		}
		else
		{
			m_rootNode->CreateList( m_normalsMode, p_showNormals, m_nbFaces, m_nbVertex);
		}

		Log::LogMessage( CU_T( "CreateList - %s - NbVertex : %d - NbFaces : %d"), m_name.c_str(), m_nbVertex, m_nbFaces);
		m_changed = false;
	}
}



SceneNodePtr Scene :: GetNode( const String & p_name)const
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	SceneNodePtr l_pReturn = NULL;

	if (p_name == CU_T( "RootNode"))
	{
		l_pReturn = m_rootNode;
	}
	else
	{
		SceneNodePtrStrMap::const_iterator l_it = m_addedNodes.find( p_name);

		if (l_it != m_addedNodes.end())
		{
			l_pReturn = l_it->second;
		}
	}

	return l_pReturn;
}



void Scene :: AddNode( SceneNodePtr p_node)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	SceneNodePtrStrMap::const_iterator l_it = m_addedNodes.find( p_node->GetName());

	if (l_it != m_addedNodes.end())
	{
		Log::LogMessage( CU_T( "AddNode - Can't add node %s - A node with that name already exists"), p_node->GetName().c_str());
	}
	else
	{
		m_addedNodes[p_node->GetName()] = p_node;
	}
}



void Scene :: AddLight( LightPtr p_light)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	LightPtrStrMap::const_iterator l_it = m_addedLights.find( p_light->GetName());

	if (l_it != m_addedLights.end())
	{
		Log::LogMessage( CU_T( "AddLight - Can't add light %s - A light with that name already exists"), p_light->GetName().c_str());
	}
	else
	{
		m_addedLights[p_light->GetName()] = p_light;
	}
}



void Scene :: AddGeometry( GeometryPtr p_geometry)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	GeometryPtrStrMap::const_iterator l_it = m_addedPrimitives.find( p_geometry->GetName());

	if (l_it != m_addedPrimitives.end())
	{
		Log::LogMessage( CU_T( "AddGeometry - Can't add geometry %s - A geometry with that name already exists"), p_geometry->GetName().c_str());
	}
	else
	{
		m_addedPrimitives[p_geometry->GetName()] = p_geometry;
		m_newlyAddedPrimitives[p_geometry->GetName()] = p_geometry;
		m_changed = true;
	}
}



GeometryPtr Scene :: GetGeometry( const String & p_name)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	GeometryPtr l_pReturn = NULL;

	GeometryPtrStrMap::iterator l_it = m_addedPrimitives.find( p_name);

	if (l_it != m_addedPrimitives.end())
	{
		l_pReturn = l_it->second;
	}

	return l_pReturn;
}

void Scene :: RemoveGeometry( GeometryPtr p_geometry)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	if ( ! p_geometry.null())
	{
		GeometryPtrStrMap::iterator l_it = m_addedPrimitives.find( p_geometry->GetName());

		if (l_it != m_addedPrimitives.end())
		{
			m_addedPrimitives.erase( l_it);
		}

		m_arrayPrimitivesToDelete.push_back( p_geometry);
	}
}

void Scene :: RemoveLight( LightPtr p_pLight)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	if ( ! p_pLight.null())
	{
		LightPtrStrMap::iterator l_it = m_addedLights.find( p_pLight->GetName());

		if (l_it != m_addedLights.end())
		{
			m_addedLights.erase( l_it);
		}

		m_arrayLightsToDelete.push_back( p_pLight);
	}
}

void Scene :: RemoveNode( SceneNodePtr p_pNode)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	if ( ! p_pNode.null())
	{
		SceneNodePtrStrMap::iterator l_it = m_addedNodes.find( p_pNode->GetName());

		if (l_it != m_addedNodes.end())
		{
			m_addedNodes.erase( l_it);
		}

		m_arrayNodesToDelete.push_back( p_pNode);
	}
}

void Scene :: RemoveCamera( CameraPtr p_pCamera)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	if ( ! p_pCamera.null())
	{
		CameraPtrStrMap::iterator l_it = m_addedCameras.find( p_pCamera->GetName());

		if (l_it != m_addedCameras.end())
		{
			m_addedCameras.erase( l_it);
		}

		m_arrayCamerasToDelete.push_back( p_pCamera);
	}
}

void Scene :: RemoveAllLights()
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	LightPtrStrMap::iterator l_it = m_addedLights.begin();

	while (m_addedLights.size() > 0)
	{
		m_arrayLightsToDelete.push_back( l_it->second);
		m_addedLights.erase( l_it);
		l_it = m_addedLights.begin();
	}

	m_addedLights.clear();
}

void Scene :: RemoveAllNodes()
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	SceneNodePtrStrMap::iterator l_it = m_addedNodes.begin();

	while (m_addedNodes.size() > 0)
	{
		l_it->second->Detach();
		m_arrayNodesToDelete.push_back( l_it->second);
		m_addedNodes.erase( l_it);
		l_it = m_addedNodes.begin();
	}

	m_addedNodes.clear();
}

void Scene :: RemoveAllGeometries()
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	GeometryPtrStrMap::iterator l_it = m_addedPrimitives.begin();

	while (m_addedPrimitives.size() > 0)
	{
		l_it->second->Detach();
		m_arrayPrimitivesToDelete.push_back( l_it->second);
		m_addedPrimitives.erase( l_it);
		l_it = m_addedPrimitives.begin();
	}

	m_addedPrimitives.clear();
}

void Scene :: RemoveAllCameras()
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	CameraPtrStrMap::iterator l_it = m_addedCameras.begin();

	while (m_addedCameras.size() > 0)
	{
		m_arrayCamerasToDelete.push_back( l_it->second);
		m_addedCameras.erase( l_it);
		l_it = m_addedCameras.begin();
	}

	m_addedCameras.clear();
}

BoolStrMap Scene :: GetGeometriesVisibility()
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	BoolStrMap l_mapReturn;
	GeometryPtrStrMap::iterator l_it = m_addedPrimitives.begin();

	while (l_it != m_addedPrimitives.end())
	{
		l_mapReturn[l_it->first] = l_it->second->GetParent()->IsVisible();
		++l_it;
	}

	return l_mapReturn;
}

bool Scene :: Write( Castor::Utils::File & p_pFile)const
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	bool l_bReturn = true;

	Log::LogMessage( CU_T( "Writing Scene Name"));

	if (l_bReturn)
	{
		Log::LogMessage( CU_T( "Writing Root Camera"));

		if ( ! m_rootCamera->Write( p_pFile))
		{
			l_bReturn = false;
		}
	}

	if (l_bReturn)
	{
		Log::LogMessage( CU_T( "Writing Root Scene Node"));

		if ( ! m_rootNode->Write( p_pFile))
		{
			l_bReturn = false;
		}
	}

	if (l_bReturn)
	{
		Log::LogMessage( CU_T( "Writing Lights"));

		if ( ! _writeLights( p_pFile))
		{
			l_bReturn = false;
		}
	}

	if (l_bReturn)
	{
		Log::LogMessage( CU_T( "Writing Geometries"));

		if ( ! _writeGeometries( p_pFile))
		{
			l_bReturn = false;
		}
	}

	return l_bReturn;
}

bool Scene :: Import3DS( const String & p_file)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	SMaxImporter l_importer;
	return _importExternal( p_file, & l_importer);
}

bool Scene :: ImportObj( const String & p_file)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	ObjImporter l_importer;
	return _importExternal( p_file, & l_importer);
}

bool Scene :: ImportMD2( const String & p_file, const String & p_texName)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	Md2Importer l_importer( p_texName);
	return _importExternal( p_file, & l_importer);
}

bool Scene :: ImportMD3( const String & p_file)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	Md3Importer l_importer;
	return _importExternal( p_file, & l_importer);
}

bool Scene :: ImportPLY( const String & p_file)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	PlyImporter l_importer;
	return _importExternal( p_file, & l_importer);
}

bool Scene :: ImportASE( const String & p_file)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	AseImporter l_importer;
	return _importExternal( p_file, & l_importer);
}

bool Scene :: ImportBSP( const String & p_file)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
//	BspImporter l_importer;
//	return _importExternal( p_file, & l_importer);
	return false;
}

void Scene :: Merge( ScenePtr p_pScene)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_pScene->m_mutex);

	for (LightPtrStrMap::iterator l_it = p_pScene->m_addedLights.begin() ; l_it != p_pScene->m_addedLights.end() ; ++l_it)
	{
		if (m_addedLights.find( l_it->first) == m_addedLights.end())
		{
			m_addedLights.insert( LightPtrStrMap::value_type( l_it->first, l_it->second));
		}
		else
		{
			m_addedLights.insert( LightPtrStrMap::value_type( p_pScene->GetName() + "_" + l_it->first, l_it->second));
		}

		l_it->second.reset();
	}

	p_pScene->m_addedLights.clear();

	for (SceneNodePtrStrMap::iterator l_it = p_pScene->m_addedNodes.begin() ; l_it != p_pScene->m_addedNodes.end() ; ++l_it)
	{
		if (m_addedNodes.find( l_it->first) == m_addedNodes.end())
		{
			m_addedNodes.insert( SceneNodePtrStrMap::value_type( l_it->first, l_it->second));
		}
		else
		{
			m_addedNodes.insert( SceneNodePtrStrMap::value_type( p_pScene->GetName() + "_" + l_it->first, l_it->second));
		}

		if (l_it->second->GetParent()->GetName() == "RootNode")
		{
			l_it->second->Detach();
			l_it->second->AttachTo( m_rootNode.get());
		}

		l_it->second.reset();
	}

	p_pScene->m_addedNodes.clear();

	for (GeometryPtrStrMap::iterator l_it = p_pScene->m_addedPrimitives.begin() ; l_it != p_pScene->m_addedPrimitives.end() ; ++l_it)
	{
		if (m_addedPrimitives.find( l_it->first) == m_addedPrimitives.end())
		{
			m_addedPrimitives.insert( GeometryPtrStrMap::value_type( l_it->first, l_it->second));
		}
		else
		{
			m_addedPrimitives.insert( GeometryPtrStrMap::value_type( p_pScene->GetName() + "_" + l_it->first, l_it->second));
		}

		if (l_it->second->GetParent()->GetName() == "RootNode")
		{
			l_it->second->Detach();
			l_it->second->SetParent( m_rootNode.get());
		}

		l_it->second.reset();
	}

	p_pScene->m_addedPrimitives.clear();

	for (GeometryPtrStrMap::iterator l_it = p_pScene->m_newlyAddedPrimitives.begin() ; l_it != p_pScene->m_newlyAddedPrimitives.end() ; ++l_it)
	{
		if (m_newlyAddedPrimitives.find( l_it->first) == m_newlyAddedPrimitives.end())
		{
			m_newlyAddedPrimitives.insert( GeometryPtrStrMap::value_type( l_it->first, l_it->second));
		}
		else
		{
			m_newlyAddedPrimitives.insert( GeometryPtrStrMap::value_type( p_pScene->GetName() + "_" + l_it->first, l_it->second));
		}

		if (l_it->second->GetParent()->GetName() == "RootNode")
		{
			l_it->second->Detach();
			l_it->second->SetParent( m_rootNode.get());
		}

		l_it->second.reset();
		m_changed = true;
	}

	p_pScene->m_newlyAddedPrimitives.clear();

	for (CameraPtrStrMap::iterator l_it = p_pScene->m_addedCameras.begin() ; l_it != p_pScene->m_addedCameras.end() ; ++l_it)
	{
		if (m_addedCameras.find( l_it->first) == m_addedCameras.end())
		{
			m_addedCameras.insert( CameraPtrStrMap::value_type( l_it->first, l_it->second));
		}
		else
		{
			m_addedCameras.insert( CameraPtrStrMap::value_type( p_pScene->GetName() + "_" + l_it->first, l_it->second));
		}

		l_it->second.reset();
	}

	p_pScene->m_addedCameras.clear();

	p_pScene->ClearScene();
}

bool Scene :: _writeLights( File & p_pFile)const
{
	bool l_bReturn = true;
	size_t l_nbLights = m_addedLights.size();

	Log::LogMessage( CU_T( "NbLights : %d"), l_nbLights);
	Light::eTYPE l_type;
	LightPtrStrMap::const_iterator l_it = m_addedLights.begin();

	while (l_bReturn && l_it != m_addedLights.end())
	{
		l_type = l_it->second->GetLightType();

		if (l_type == Light::eDirectional)
		{
			l_bReturn = static_cast <DirectionalLightPtr>( l_it->second)->Write( p_pFile);
		}
		else if (l_type == Light::ePoint)
		{
			l_bReturn = static_cast <PointLightPtr>( l_it->second)->Write( p_pFile);
		}
		else if (l_type == Light::eSpot)
		{
			l_bReturn = static_cast <SpotLightPtr>( l_it->second)->Write( p_pFile);
		}

		++l_it;
	}

	return l_bReturn;
}

bool Scene :: _writeGeometries( Castor::Utils::File & p_pFile)const
{
	bool l_bReturn = true;
	size_t l_nbGeometries = m_addedPrimitives.size();

	Log::LogMessage( CU_T( "NbGeometries : %d"), l_nbGeometries);
	GeometryPtrStrMap::const_iterator l_it = m_addedPrimitives.begin();

	while (l_bReturn && l_it != m_addedPrimitives.end())
	{
		l_bReturn = l_it->second->Write( p_pFile);
		++l_it;
	}

	return l_bReturn;
}

bool Scene :: _importExternal( const String & p_fileName, ExternalImporterPtr p_importer)
{
	bool l_bReturn = true;

	if (p_importer->Import( p_fileName))
	{
		for (GeometryPtrStrMap::iterator l_it = p_importer->m_geometries.begin() ; l_it != p_importer->m_geometries.end() ; ++l_it)
		{
			if (m_addedPrimitives.find( l_it->first) == m_addedPrimitives.end())
			{
				m_addedPrimitives.insert( GeometryPtrStrMap::value_type( l_it->first, l_it->second));
				m_newlyAddedPrimitives.insert( GeometryPtrStrMap::value_type( l_it->first, l_it->second));
				Log::LogMessage( CU_T( "_importExternal - Geometry %s added"), l_it->first.c_str());
			}
			else
			{
				l_it->second.reset();
//				delete l_it->second;
			}
		}

		m_changed = true;
		l_bReturn = true;
	}

	return l_bReturn;
}

void Scene :: Select( Ray * p_ray, GeometryPtr * p_geo, SubmeshPtr * p_submesh, FacePtr * p_face, Point3rPtr * p_vertex)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	Point3r l_min;
	Point3r l_max;
	GeometryPtr l_geo, l_selectedGeo;
	MeshPtr l_mesh;
	SubmeshPtr l_submesh, l_selectedSubmesh;
	unsigned int l_nbSubmeshes;
	SmoothGroupPtrUIntMap::const_iterator l_itGroupsEnd;
	FacePtr l_face, l_selectedFace=NULL;
	Point3rPtr l_selectedVertex=NULL;
	real l_geoDist = 10e6, l_faceDist = 10e6, l_vertexDist = 10e6;
	real l_curgeoDist, l_curfaceDist, l_curvertexDist;
	ComboBoxPtr l_box;
	SpherePtr l_sphere;
	SmoothingGroupPtr l_group;

	for (GeometryPtrStrMap::iterator l_it = m_addedPrimitives.begin() ; l_it != m_addedPrimitives.end() ; ++l_it)
	{
		l_geo = l_it->second;

		if (l_geo->IsVisible())
		{
			l_mesh = l_geo->GetMesh();
			l_sphere = l_mesh->GetSphere();

			if ((l_curgeoDist = p_ray->Intersects( *l_sphere)) > 0.0f)
			{
				l_box = l_mesh->GetComboBox();

				if (p_ray->Intersects( *l_box) > 0.0f)
				{
					l_geoDist = l_curgeoDist;
					l_nbSubmeshes = l_mesh->GetNbSubmeshes();

					for (unsigned int i = 0 ; i < l_nbSubmeshes ; i++)
					{
						l_submesh = l_mesh->GetSubmesh( i);
						l_sphere = l_submesh->GetSphere();

						if ( ! l_sphere.null())
						{
							if (p_ray->Intersects( *l_sphere) >= 0.0f)
							{
								l_box = l_submesh->GetComboBox();

								if (p_ray->Intersects( *l_box) >= 0.0f)
								{
									for (size_t i = 0 ; i < l_submesh->GetNbSmoothGroups() ; i++)
									{
										l_group = l_submesh->GetSmoothGroup( i);

										for (size_t k = 0 ; k < l_group->m_faces.size() ; k++)
										{
											l_face = l_group->m_faces[k];

											if ((l_curfaceDist = p_ray->Intersects( *l_face)) < l_faceDist)
											{
												if ((l_curvertexDist = p_ray->Intersects( *l_face->m_vertex1)) < l_vertexDist)
												{
													l_vertexDist = l_curvertexDist;
													l_geoDist = l_curgeoDist;
													l_faceDist = l_curfaceDist;
													l_selectedGeo = l_geo;
													l_selectedSubmesh = l_submesh;
													l_selectedFace = l_face;
													l_selectedVertex = l_face->m_vertex1;
												}

												if ((l_curvertexDist = p_ray->Intersects( *l_face->m_vertex2)) < l_vertexDist)
												{
													l_vertexDist = l_curvertexDist;
													l_geoDist = l_curgeoDist;
													l_faceDist = l_curfaceDist;
													l_selectedGeo = l_geo;
													l_selectedSubmesh = l_submesh;
													l_selectedFace = l_face;
													l_selectedVertex = l_face->m_vertex2;
												}

												if ((l_curvertexDist = p_ray->Intersects( *l_face->m_vertex2)) < l_vertexDist)
												{
													l_vertexDist = l_curvertexDist;
													l_geoDist = l_curgeoDist;
													l_faceDist = l_curfaceDist;
													l_selectedGeo = l_geo;
													l_selectedSubmesh = l_submesh;
													l_selectedFace = l_face;
													l_selectedVertex = l_face->m_vertex3;
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	if ( ! (*p_geo).null())
	{
		*p_geo = l_selectedGeo;
	}

	if ( ! (*p_submesh).null())
	{
		*p_submesh = l_selectedSubmesh;
	}

	if ( ! (* p_face).null())
	{
		*p_face = l_selectedFace;
	}

	if ( ! (* p_vertex).null())
	{
		*p_vertex = l_selectedVertex;
	}
}

void Scene :: _deleteToDelete()
{
	for (size_t i = 0 ; i < m_arrayLightsToDelete.size() ; i++)
	{
//		delete m_arrayLightsToDelete[i];
		m_arrayLightsToDelete[i].reset();
	}

	m_arrayLightsToDelete.clear();

	for (size_t i = 0 ; i < m_arrayPrimitivesToDelete.size() ; i++)
	{
		m_arrayPrimitivesToDelete[i]->Detach();
//		delete m_arrayPrimitivesToDelete[i];
		m_arrayPrimitivesToDelete[i].reset();
	}

	m_arrayPrimitivesToDelete.clear();
	
	for (size_t i = 0 ; i < m_arrayNodesToDelete.size() ; i++)
	{
		m_arrayNodesToDelete[i].reset();
//		delete m_arrayNodesToDelete[i];
	}

	m_arrayNodesToDelete.clear();

	for (size_t i = 0 ; i < m_arrayCamerasToDelete.size() ; i++)
	{
//		delete m_arrayCamerasToDelete[i];
		m_arrayCamerasToDelete[i].reset();
	}

	m_arrayCamerasToDelete.clear();
}