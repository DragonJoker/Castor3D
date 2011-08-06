#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Scene.hpp"
#include "Castor3D/SceneNode.hpp"
#include "Castor3D/SceneFileParser.hpp"
#include "Castor3D/Camera.hpp"
#include "Castor3D/PointLight.hpp"
#include "Castor3D/SpotLight.hpp"
#include "Castor3D/DirectionalLight.hpp"
#include "Castor3D/Geometry.hpp"
#include "Castor3D/Mesh.hpp"
#include "Castor3D/Submesh.hpp"
#include "Castor3D/SmoothingGroup.hpp"
#include "Castor3D/Vertex.hpp"
#include "Castor3D/Face.hpp"
#include "Castor3D/Material.hpp"
#include "Castor3D/RenderSystem.hpp"
#include "Castor3D/Root.hpp"
#include "Castor3D/Pipeline.hpp"
#include "Castor3D/Ray.hpp"
#include "Castor3D/Importer.hpp"
#include "Castor3D/AnimatedObjectGroup.hpp"
#include "Castor3D/Overlay.hpp"

using namespace Castor3D;

//*************************************************************************************************

ScenePtr Factory<Scene> :: Create( String const & p_strName)
{
	Collection<Scene, String> l_collection;
	ScenePtr l_pReturn = l_collection.GetElement( p_strName);

	if ( ! l_pReturn)
	{
		l_pReturn.reset( new Scene( p_strName));
		l_collection.AddElement( p_strName, l_pReturn);
	}

	return l_pReturn;
}

//*************************************************************************************************

bool Loader<Scene> :: Read( Scene & p_scene, File & p_file)
{
	SceneFileParser l_parser;
	return l_parser.ParseFile( p_scene, p_file);
}

bool Loader<Scene> :: Write( const Scene & p_scene, File & p_file)
{
	bool l_bReturn = true;

	Logger::LogMessage( cuT( "Scene :: Write - Scene Name"));

	if (l_bReturn)
	{
		Logger::LogMessage( cuT( "Scene :: Write - Root Camera"));
		l_bReturn = Loader<Camera>::Write( * p_scene.GetRootCamera(), p_file);
	}

	if (l_bReturn)
	{
		Logger::LogMessage( cuT( "Scene :: Write - Root Node"));
		l_bReturn = Loader<SceneNode>::Write( * p_scene.GetRootNode(), p_file);
	}

	if (l_bReturn)
	{
		Logger::LogMessage( cuT( "Scene :: Write - Lights"));
		size_t l_nbLights = p_scene.GetLightsCount();

		LightPtrStrMap::const_iterator l_it = p_scene.LightsBegin();

		while (l_bReturn && l_it != p_scene.LightsEnd())
		{
			switch (l_it->second->GetLightType())
			{
			case eLIGHT_TYPE_DIRECTIONAL:
				l_bReturn = Loader<DirectionalLight>::Write( * static_pointer_cast<DirectionalLight>( l_it->second->GetLightCategory()), p_file);
				break;

			case eLIGHT_TYPE_POINT:
				l_bReturn = Loader<PointLight>::Write( * static_pointer_cast<PointLight>( l_it->second->GetLightCategory()), p_file);
				break;

			case eLIGHT_TYPE_SPOT:
				l_bReturn = Loader<SpotLight>::Write( * static_pointer_cast<SpotLight>( l_it->second->GetLightCategory()), p_file);
				break;

			default:
				l_bReturn = false;
			}
			++l_it;
		}
	}

	if (l_bReturn)
	{
		Logger::LogMessage( cuT( "Scene :: Write - Geometries"));
		size_t l_nbGeometries = p_scene.GetGeometriesCount();

		GeometryPtrStrMap::const_iterator l_it = p_scene.GeometriesBegin();

		while (l_bReturn && l_it != p_scene.GeometriesEnd())
		{
			l_bReturn = Loader<Geometry>::Write( * l_it->second, p_file);
			++l_it;
		}
	}

	return l_bReturn;
}

//*************************************************************************************************

Scene :: Scene( String const & p_name)
	:	m_strName	( p_name)
	,	m_changed	( false)
{
	m_rootNode.reset( new SceneNode( this, cuT( "RootNode")));
//	SceneNodePtr l_camNode = SceneNodePtr( new SceneNode( this, cuT( "RootCameraNode")));
//	m_rootCamera.reset( new Camera( this, cuT( "RootCamera"), 800, 600, l_camNode, Viewport::e3DView));
//	m_rootCamera->GetParent()->Translate( 0.0f, 0.0f, -5.0f);
}

Scene :: ~Scene()
{
	ClearScene();
	m_rootCamera.reset();
	m_rootNode.reset();
}

void Scene :: ClearScene()
{
	CASTOR_TRACK;
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	if (RenderSystem::GetSingletonPtr())
	{
		RenderSystem::GetSingletonPtr()->SetCurrentShaderProgram( nullptr);
	}
/*
	RemoveAllGeometries();
	RemoveAllLights();
*/
	LightPtrStrMap::iterator l_itLights = m_addedLights.begin();

	while (m_addedLights.size() > 0)
	{
		l_itLights->second->Detach();
		l_itLights->second.reset();
		m_addedLights.erase( l_itLights);
		l_itLights = m_addedLights.begin();
	}

	GeometryPtrStrMap::iterator l_itGeometries = m_addedPrimitives.begin();

	while (m_addedPrimitives.size() > 0)
	{
		l_itGeometries->second->Detach();
		l_itGeometries->second.reset();
		m_addedPrimitives.erase( l_itGeometries);
		l_itGeometries = m_addedPrimitives.begin();
	}

	_removeNodesAndUnattached( m_addedCameras);
}

void Scene :: Render( ePRIMITIVE_TYPE p_displayMode, real p_tslf)
{
	CASTOR_TRACK;
	_deleteToDelete();
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	_updateAnimations();

	RenderSystem::GetSingletonPtr()->RenderAmbientLight( m_clAmbientLight);

	LightPtrStrMap::iterator l_itEnd = m_addedLights.end();

	for (LightPtrStrMap::iterator l_it = m_addedLights.begin() ; l_it != l_itEnd ; ++l_it)
	{
		l_it->second->Render( p_displayMode);
	}

	m_rootNode->Render( p_displayMode);

	for (LightPtrStrMap::iterator l_it = m_addedLights.begin() ; l_it != l_itEnd ; ++l_it)
	{
		l_it->second->EndRender();
	}
}

SceneNodePtr Scene :: CreateSceneNode( String const & p_name, SceneNode * p_parent)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	SceneNodePtr l_pReturn;

	if (p_name != cuT( "RootNode") && m_addedNodes.find( p_name) == m_addedNodes.end())
	{
		l_pReturn = SceneNodePtr( new SceneNode( this, p_name));

		if (p_parent)
		{
			l_pReturn->AttachTo( p_parent);
		}
		else
		{
			l_pReturn->AttachTo( m_rootNode.get());
		}

		m_addedNodes[p_name] = l_pReturn;
	}
	else
	{
		Logger::LogWarning( cuT( "Scene :: CreateSceneNode - Can't create scene node [%s] - Another scene node with the same name already exists"), p_name.c_str());
		l_pReturn = m_addedNodes.find( p_name)->second;
	}

	return l_pReturn;
}

GeometryPtr Scene :: CreatePrimitive( String const & p_name, eMESH_TYPE p_type,
									 String const & p_meshName, UIntArray p_faces,
									 FloatArray p_size)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	GeometryPtr l_pReturn;

	if (m_addedPrimitives.find( p_name) == m_addedPrimitives.end())
	{
		Collection<Mesh, String> l_meshManager;
		MeshPtr l_mesh = l_meshManager.GetElement( p_meshName);

		if ( ! l_mesh)
		{
			Collection<Mesh, String> l_mshCollection;
			MeshPtr l_pMesh = l_mshCollection.GetElement( p_meshName);
			if ( ! l_pMesh)
			{
				l_pMesh.reset( new Mesh( p_meshName, p_type));
				l_pMesh->Initialise( p_faces, p_size);
				l_mshCollection.AddElement( p_meshName, l_pMesh);
			}
			Logger::LogMessage( cuT( "Scene :: CreatePrimitive - Mesh [%s] created"), p_meshName.c_str());
			l_mesh->ComputeNormals();
			Logger::LogMessage( cuT( "Scene :: CreatePrimitive - Normals setting done"));
		}

		if (l_mesh)
		{
			l_pReturn = GeometryPtr( new Geometry( this, l_mesh, SceneNodePtr(), p_name));
			Logger::LogMessage( cuT( "Scene :: CreatePrimitive - Geometry [%s] created"), p_name.c_str());
			m_addedPrimitives[p_name] = l_pReturn;
			m_newlyAddedPrimitives[p_name] = l_pReturn;
			m_changed = true;
		}
		else
		{
			Logger::LogMessage( cuT( "Scene :: CreatePrimitive - Can't create primitive [%s] - Mesh creation failed"), p_name.c_str());
		}
	}
	else
	{
		Logger::LogMessage( cuT( "Scene :: CreatePrimitive - Can't create primitive [%s] - Another primitive with the same name already exists"), p_name.c_str());
	}

	return l_pReturn;
}

GeometryPtr Scene :: CreatePrimitive( String const & p_strName)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	GeometryPtr l_pReturn;
	GeometryPtrStrMap::iterator l_it = m_addedPrimitives.find( p_strName);

	if (l_it == m_addedPrimitives.end())
	{
		l_pReturn = GeometryPtr( new Geometry( this, MeshPtr(), SceneNodePtr(), p_strName));
	}
	else
	{
		Logger::LogWarning( cuT( "Scene :: CreatePrimitive - Can't create primitive [%s] - Another primitive with the same name already exists"), p_strName.c_str());
		l_pReturn = l_it->second;
	}

	return l_pReturn;
}

CameraPtr Scene :: CreateCamera( String const & p_name, int p_ww, int p_wh, SceneNodePtr p_pNode,
							   eVIEWPORT_TYPE p_type)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	CameraPtr l_pReturn;

	if (m_addedCameras.find( p_name) != m_addedCameras.end())
	{
		Logger::LogMessage( cuT( "Scene :: CreateCamera - Can't create camera [%s] - A camera with that name already exists"), p_name.c_str());
	}
	else
	{
		l_pReturn = CameraPtr( new Camera( this, p_name, p_ww, p_wh, p_pNode, p_type));

		if ( ! m_rootCamera)
		{
			m_rootCamera = l_pReturn;
		}

		m_addedCameras[p_name] = l_pReturn;
		Logger::LogMessage( cuT( "Scene :: CreateCamera - Camera [") + p_name + cuT( "] created"));
	}

	return l_pReturn;
}

LightPtr Scene :: CreateLight( String const & p_name, SceneNodePtr p_pNode, eLIGHT_TYPE p_eLightType)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	LightPtr l_pReturn;
	LightPtrStrMap::iterator l_it = m_addedLights.find( p_name);

	if (l_it != m_addedLights.end())
	{
		Logger::LogWarning( cuT( "Scene :: CreateLight - Can't create light [%s] - A light with that name already exists"), p_name.c_str());
		l_pReturn = l_it->second;
	}
	else
	{
		l_pReturn = LightPtr( new Light( this, p_pNode, p_name, p_eLightType));
		m_addedLights[p_name] = l_pReturn;
		Logger::LogMessage( cuT( "Scene :: CreateLight - Light [") + p_name + cuT( "] created"));
	}

	return l_pReturn;
}

void Scene :: CreateList( eNORMALS_MODE p_nm, bool p_showNormals)
{
	CASTOR_TRACK;
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	m_normalsMode = p_nm;

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

	Logger::LogMessage( cuT( "Scene :: CreateList - [%s] - NbVertex : %d - NbFaces : %d"), m_strName.c_str(), m_nbVertex, m_nbFaces);
	m_changed = false;
}

SceneNodePtr Scene :: GetNode( String const & p_name)const
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	SceneNodePtr l_pReturn;

	if (p_name == cuT( "RootNode"))
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
		Logger::LogMessage( cuT( "Scene :: AddNode - Can't add node [%s] - A node with that name already exists"), p_node->GetName().c_str());
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
		Logger::LogMessage( cuT( "Scene :: AddLight - Can't add light [%s] - A light with that name already exists"), p_light->GetName().c_str());
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
		Logger::LogMessage( cuT( "Scene :: AddGeometry - Can't add geometry [%s] - A geometry with that name already exists"), p_geometry->GetName().c_str());
	}
	else
	{
		m_addedPrimitives[p_geometry->GetName()] = p_geometry;
		m_newlyAddedPrimitives[p_geometry->GetName()] = p_geometry;
		m_changed = true;
	}
}

GeometryPtr Scene :: GetGeometry( String const & p_name)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	GeometryPtr l_pReturn;

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

	if (p_geometry)
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

	if (p_pLight)
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

	if (p_pNode)
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

	if (p_pCamera)
	{
		CameraPtrStrMap::iterator l_it = m_addedCameras.find( p_pCamera->GetName());

		if (l_it != m_addedCameras.end())
		{
			m_addedCameras.erase( l_it);
		}

		m_arrayCamerasToDelete.push_back( p_pCamera);
	}
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

void Scene :: RemoveAllLights()
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	LightPtrStrMap::iterator l_it = m_addedLights.begin();
	SceneNodePtrStrMap::iterator l_itNodes;

	while (m_addedLights.size() > 0)
	{
		l_it->second->Detach();
		m_arrayLightsToDelete.push_back( l_it->second);
		m_addedLights.erase( l_it);
		l_it = m_addedLights.begin();
	}

	m_addedLights.clear();
}

void Scene :: RemoveAllGeometries()
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	GeometryPtrStrMap::iterator l_it = m_addedPrimitives.begin();
	SceneNodePtrStrMap::iterator l_itNodes;

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
		l_it->second->Detach();
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

void Scene :: Merge( ScenePtr p_pScene)
{
	{
		CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
		CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_pScene->m_mutex);

		String l_strName;

		_merge<SceneNodePtrStrMap>( p_pScene, p_pScene->m_addedNodes, m_addedNodes);
		_merge<LightPtrStrMap>( p_pScene, p_pScene->m_addedLights, m_addedLights);
		_merge<CameraPtrStrMap>( p_pScene, p_pScene->m_addedCameras, m_addedCameras);
		_merge<GeometryPtrStrMap>( p_pScene, p_pScene->m_addedPrimitives, m_addedPrimitives);
		_merge<GeometryPtrStrMap>( p_pScene, p_pScene->m_newlyAddedPrimitives, m_newlyAddedPrimitives);
		m_changed = true;

		m_clAmbientLight = p_pScene->GetAmbientLight();
	}

	p_pScene->ClearScene();
}

bool Scene :: ImportExternal( String const & p_fileName, Importer * p_importer)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	bool l_bReturn = true;

	if (p_importer->Import( p_fileName))
	{
		for (GeometryPtrStrMap::iterator l_it = p_importer->m_geometries.begin() ; l_it != p_importer->m_geometries.end() ; ++l_it)
		{
			if (m_addedPrimitives.find( l_it->first) == m_addedPrimitives.end())
			{
				m_addedPrimitives.insert( GeometryPtrStrMap::value_type( l_it->first, l_it->second));
				m_newlyAddedPrimitives.insert( GeometryPtrStrMap::value_type( l_it->first, l_it->second));
				Logger::LogMessage( cuT( "Scene :: ImportExternal - Geometry [%s] added"), l_it->first.c_str());
			}
			else
			{
				l_it->second.reset();
			}
		}

		m_changed = true;
		l_bReturn = true;
	}

	return l_bReturn;
}

void Scene :: Select( Ray * p_ray, GeometryPtr & p_geo, SubmeshPtr & p_submesh, FacePtr * p_face, Vertex * p_vertex)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	Point3r l_min;
	Point3r l_max;
	GeometryPtr l_geo, l_selectedGeo;
	MeshPtr l_mesh;
	SubmeshPtr l_submesh, l_selectedSubmesh;
	unsigned int l_nbSubmeshes;
	SmoothGroupPtrUIntMap::const_iterator l_itGroupsEnd;
	FacePtr l_face, l_selectedFace;
	Vertex l_selectedVertex;
	real l_geoDist = 10e6, l_faceDist = 10e6, l_vertexDist = 10e6;
	real l_curgeoDist, l_curfaceDist, l_curvertexDist;
	CubeBox l_box;
	SphereBox l_sphere;
	SmoothingGroupPtr l_group;

	for (GeometryPtrStrMap::iterator l_it = m_addedPrimitives.begin() ; l_it != m_addedPrimitives.end() ; ++l_it)
	{
		l_geo = l_it->second;

		if (l_geo->IsVisible())
		{
			l_mesh = l_geo->GetMesh();
			l_sphere = l_mesh->GetSphere();

			if ((l_curgeoDist = p_ray->Intersects( l_sphere)) > 0.0f)
			{
				l_box = l_mesh->GetCubeBox();

				if (p_ray->Intersects( l_box) > 0.0f)
				{
					l_geoDist = l_curgeoDist;
					l_nbSubmeshes = l_mesh->GetNbSubmeshes();

					for (unsigned int i = 0 ; i < l_nbSubmeshes ; i++)
					{
						l_submesh = l_mesh->GetSubmesh( i);
						l_sphere = l_submesh->GetSphere();

						if (p_ray->Intersects( l_sphere) >= 0.0f)
						{
							l_box = l_submesh->GetCubeBox();

							if (p_ray->Intersects( l_box) >= 0.0f)
							{
								for (size_t i = 0 ; i < l_submesh->GetNbSmoothGroups() ; i++)
								{
									l_group = l_submesh->GetSmoothGroup( i);

									for (size_t k = 0 ; k < l_group->GetNbFaces() ; k++)
									{
										l_face = l_group->GetFace( k);

										if ((l_curfaceDist = p_ray->Intersects( *l_face)) < l_faceDist)
										{
											if ((l_curvertexDist = p_ray->Intersects( l_face->GetVertex( 0).GetCoords())) < l_vertexDist)
											{
												l_vertexDist = l_curvertexDist;
												l_geoDist = l_curgeoDist;
												l_faceDist = l_curfaceDist;
												l_selectedGeo = l_geo;
												l_selectedSubmesh = l_submesh;
												l_selectedFace = l_face;
												l_selectedVertex = l_face->GetVertex( 0);
											}

											if ((l_curvertexDist = p_ray->Intersects( l_face->GetVertex( 1).GetCoords())) < l_vertexDist)
											{
												l_vertexDist = l_curvertexDist;
												l_geoDist = l_curgeoDist;
												l_faceDist = l_curfaceDist;
												l_selectedGeo = l_geo;
												l_selectedSubmesh = l_submesh;
												l_selectedFace = l_face;
												l_selectedVertex = l_face->GetVertex( 1);
											}

											if ((l_curvertexDist = p_ray->Intersects( l_face->GetVertex( 2).GetCoords())) < l_vertexDist)
											{
												l_vertexDist = l_curvertexDist;
												l_geoDist = l_curgeoDist;
												l_faceDist = l_curfaceDist;
												l_selectedGeo = l_geo;
												l_selectedSubmesh = l_submesh;
												l_selectedFace = l_face;
												l_selectedVertex = l_face->GetVertex( 2);
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

	if (p_geo)
	{
		p_geo = l_selectedGeo;
	}

	if (p_submesh)
	{
		p_submesh = l_selectedSubmesh;
	}

	if (p_face)
	{
		p_face = & l_selectedFace;
	}

	if (p_vertex)
	{
		p_vertex = & l_selectedVertex;
	}
}

void Scene :: ClearOverlays()
{
	m_ovyCollection.Clear();
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	m_mapOverlaysByZIndex.clear();
	m_iCurrentZIndex = 1;
}

void Scene :: AddOverlay( String const & p_strName, OverlayPtr p_pOverlay)
{
	m_ovyCollection.AddElement( p_strName,p_pOverlay);
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	while (m_mapOverlaysByZIndex.find( m_iCurrentZIndex) != m_mapOverlaysByZIndex.end())
	{
		m_iCurrentZIndex++;
	}
}

void Scene :: AddOverlay( int p_iZIndex, OverlayPtr p_pOverlay)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	m_mapOverlaysByZIndex.insert( OverlayPtrIntMap::value_type( p_iZIndex, p_pOverlay));
}

bool Scene :: HasOverlay( int p_iZIndex)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	return m_mapOverlaysByZIndex.find( p_iZIndex) != m_mapOverlaysByZIndex.end();
}

void Scene :: RenderOverlays()
{
	CASTOR_TRACK;
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	RenderSystem::GetSingletonPtr()->BeginOverlaysRendering();

	for (OverlayPtrIntMap::iterator l_it = m_mapOverlaysByZIndex.begin() ; l_it != m_mapOverlaysByZIndex.end() ; ++l_it)
	{
		l_it->second->Render( ePRIMITIVE_TYPE_TRIANGLES);
	}

	RenderSystem::GetSingletonPtr()->EndOverlaysRendering();
}

bool Scene :: WriteOverlays( File & p_file)const
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	bool l_bReturn = true;
	OverlayPtrIntMap::const_iterator l_it = m_mapOverlaysByZIndex.begin();

	bool l_bFirst = true;

	while (l_bReturn && l_it != m_mapOverlaysByZIndex.end())
	{
		if (l_bFirst)
		{
			l_bFirst = false;
		}
		else
		{
			p_file.WriteLine( "\n");
		}

		switch (l_it->second->GetType())
		{
		case eOVERLAY_TYPE_PANEL:
			l_bReturn = Loader<PanelOverlay>::Write( * static_pointer_cast<PanelOverlay>( l_it->second->GetOverlayCategory()), p_file);
			break;

		case eOVERLAY_TYPE_BORDER_PANEL:
			l_bReturn = Loader<BorderPanelOverlay>::Write( * static_pointer_cast<BorderPanelOverlay>( l_it->second->GetOverlayCategory()), p_file);
			break;

		case eOVERLAY_TYPE_TEXT:
			l_bReturn = Loader<TextOverlay>::Write( * static_pointer_cast<TextOverlay>( l_it->second->GetOverlayCategory()), p_file);
			break;

		default:
			l_bReturn = false;
		}
		++l_it;
	}

	return l_bReturn;
}

bool Scene :: ReadOverlays( File & p_file)
{
	SceneFileParser l_parser;
	return l_parser.ParseFile( p_file);
}

bool Scene :: SaveOverlays( File & p_file)const
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	bool l_bReturn = p_file.Write( m_mapOverlaysByZIndex.size()) == sizeof( size_t);
	OverlayPtrIntMap::const_iterator l_it = m_mapOverlaysByZIndex.begin();

	while (l_bReturn && l_it != m_mapOverlaysByZIndex.end())
	{
		l_bReturn = l_it->second->Serialise( p_file);
		++l_it;
	}

	return l_bReturn;
}

bool Scene :: LoadOverlays( File & p_file)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	size_t l_uiSize;
	bool l_bReturn = p_file.Write( l_uiSize) == sizeof( size_t);
	String l_strName;
	eOVERLAY_TYPE l_eType;
	OverlayPtr l_pOverlay;

	for (size_t i = 0 ; i < l_uiSize && l_bReturn ; i++)
	{
		l_bReturn = p_file.Read( l_strName);

		if (l_bReturn)
		{
			l_bReturn = p_file.Read( l_eType) == sizeof( eOVERLAY_TYPE);
		}

		if (l_bReturn)
		{
			Collection<Overlay, String> l_ovlCollection;
			l_pOverlay = l_ovlCollection.GetElement( l_strName);
			if ( ! l_pOverlay)
			{
				l_pOverlay.reset( new Overlay( this, l_strName, NULL, l_eType));
				l_ovlCollection.AddElement( l_strName, l_pOverlay);
			}
			l_bReturn = l_pOverlay.use_count() > 0;
		}

		if (l_bReturn)
		{
			l_bReturn = l_pOverlay->Unserialise( p_file, NULL);
		}
	}

	return l_bReturn;
}

void Scene :: _deleteToDelete()
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
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

void Scene :: _updateAnimations()
{
	CASTOR_TRACK;
	Collection<AnimatedObjectGroup, String> l_anmCollection;
	l_anmCollection.Lock();

	for (Collection<AnimatedObjectGroup, String>::iterator l_it = l_anmCollection.Begin() ; l_it != l_anmCollection.End() ; ++l_it)
	{
		l_it->second->Update();
	}

	l_anmCollection.Unlock();
}

BEGIN_SERIALISE_MAP( Scene, Serialisable)
	ADD_ELEMENT( * m_rootCamera)
	ADD_ELEMENT( * m_rootNode)
	ADD_ELEMENT( m_addedPrimitives)
END_SERIALISE_MAP()

BEGIN_POST_SERIALISE( Scene, Serialisable)
	bool l_bReturn = m_pFile->Write( m_addedLights.size()) == sizeof( size_t);

	for (LightPtrStrMap::const_iterator l_it = m_addedLights.begin() ; l_it != m_addedLights.end() && l_bReturn ; ++l_it)
	{
		l_bReturn = l_it->second->Serialise( * m_pFile);
	}
END_POST_SERIALISE()

BEGIN_POST_UNSERIALISE( Scene, Serialisable)
	size_t l_uiSize;
	bool l_bReturn = m_pFile->Read( l_uiSize) == sizeof( size_t);

	for (size_t i = 0 ; i < l_uiSize ; i++)
	{
		eLIGHT_TYPE l_eType;
		l_bReturn = m_pFile->Read<eLIGHT_TYPE>( l_eType) == sizeof( eLIGHT_TYPE);
		LightPtr l_pLight;

		if (l_bReturn)
		{
			l_pLight = CreateLight( cuEmptyString, SceneNodePtr(), l_eType);
		}

		if (l_bReturn)
		{
			l_pLight->Unserialise( * m_pFile, this);
		}
	}
END_POST_UNSERIALISE()

//*************************************************************************************************
