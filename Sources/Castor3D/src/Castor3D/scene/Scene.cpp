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

#include "Log.h"

using namespace Castor3D;

Scene :: Scene( const String & p_name)
	:	m_name			( p_name),
		m_rootNode		( new SceneNode( Root::GetRenderSystem()->CreateSceneNodeRenderer(), C3D_T( "RootNode"))),
		m_rootCamera	( new Camera( Root::GetRenderSystem()->CreateCameraRenderer(), C3D_T( "RootCamera"), 800, 600, pt3DView)),
		m_changed		( false)
{
	m_rootCamera->Translate( 0.0f, 0.0f, -5.0f);
}

Scene :: ~Scene()
{
	ClearScene();
	delete m_rootCamera;
	delete m_rootNode;
}

void Scene :: ClearScene()
{
	RemoveAllNodes();
	RemoveAllGeometries();
	RemoveAllLights();
	RemoveAllCameras();
}

void Scene :: Render( DrawType p_displayMode, float p_tslf)
{
	if (m_rootNode != NULL)
	{
		_deleteToDelete();

		LightStrMap::iterator l_it = m_addedLights.begin();
		LightStrMap::const_iterator l_end = m_addedLights.end();

		while (l_it != l_end)
		{
			l_it->second->Render();
			++l_it;
		}

		m_rootNode->Draw( p_displayMode);

		l_it = m_addedLights.begin();

		while (l_it != l_end)
		{
			l_it->second->Disable();
			++l_it;
		}
	}
}

SceneNode * Scene :: CreateSceneNode( const String & p_name, SceneNode * p_parent)
{
	SceneNode * l_pReturn = NULL;

	if (p_name != C3D_T( "RootNode") && m_addedNodes.find( p_name) == m_addedNodes.end())
	{
		l_pReturn = new SceneNode( Root::GetRenderSystem()->CreateSceneNodeRenderer(), p_name);

		if (p_parent != NULL)
		{
			l_pReturn->AttachTo( p_parent);
		}
		else
		{
			l_pReturn->AttachTo( m_rootNode);
		}

		m_addedNodes[p_name] = l_pReturn;
	}
	else
	{
		Log::LogMessage( C3D_T( "CreateSceneNode - Can't create scene node %s - Another scene node with the same name already exists"), p_name.c_str());
		l_pReturn = m_addedNodes.find( p_name)->second;
	}

	return l_pReturn;
}



Geometry * Scene :: CreatePrimitive( const String & p_name, Mesh::eTYPE p_type, 
									const String & p_meshName, UIntArray p_faces,
									FloatArray p_size)
{
	Geometry * l_pReturn = NULL;

	if (m_addedPrimitives.find( p_name) == m_addedPrimitives.end())
	{
		Mesh * l_mesh = NULL;

		if (MeshManager::GetSingletonPtr()->HasElement( p_meshName))
		{
			l_mesh = MeshManager::GetSingletonPtr()->GetElementByName( p_meshName);
		}
		else
		{
			l_mesh = MeshManager::GetSingletonPtr()->CreateMesh( p_meshName, p_faces, p_size, p_type);
			Log::LogMessage( C3D_T( "CreatePrimitive - Mesh %s created"), p_meshName.c_str());
			l_mesh->SetNormals();
			Log::LogMessage( C3D_T( "CreatePrimitive - Normals setting finished"));
		}

		if (l_mesh != NULL)
		{
			l_pReturn = new Geometry( l_mesh, NULL, p_name);
			Log::LogMessage( C3D_T( "CreatePrimitive - Geometry %s created"), p_name.c_str());
			m_addedPrimitives[p_name] = l_pReturn;
			m_newlyAddedPrimitives[p_name] = l_pReturn;
			m_changed = true;
		}
		else
		{
			Log::LogMessage( C3D_T( "CreatePrimitive - Can't create primitive %s - Mesh creation failed"), p_name.c_str());
		}
	}
	else
	{
		Log::LogMessage( C3D_T( "CreatePrimitive - Can't create primitive %s - Another primitive with the same name already exists"), p_name.c_str());
	}

	return l_pReturn;
}



Camera * Scene :: CreateCamera( const String & p_name, int p_ww, int p_wh,
								ProjectionType p_type)
{
	Camera * l_pReturn = NULL;

	if (m_addedCameras.find( p_name) != m_addedCameras.end())
	{
		Log::LogMessage( C3D_T( "CreateCamera - Can't create camera %s - A camera with this name already exists"), p_name.c_str());
	}
	else
	{
		Log::LogMessage( C3D_T( "CreateCamera - Creating Camera %s"), p_name.c_str());
		l_pReturn = new Camera( Root::GetRenderSystem()->CreateCameraRenderer(), p_name, p_ww, p_wh, p_type);

		if (m_rootCamera == NULL)
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



Light * Scene :: CreateLight( Light::eTYPE p_type, const String & p_name)
{
	Light * l_pReturn = NULL;

	if (m_addedLights.find( p_name) != m_addedLights.end())
	{
		Log::LogMessage( C3D_T( "CreateLight - Can't create light %s - A light with this name already exists"), p_name.c_str());
	}
	else if (m_addedLights.size() >= 8)
	{
		Log::LogMessage( C3D_T( "CreateLight - Can't create light %s - max light number reached"), p_name.c_str());
	}
	else
	{
		LightRenderer * l_renderer = Root::GetRenderSystem()->CreateLightRenderer();

		if (p_type == Light::eSpot)
		{
			l_pReturn = new SpotLight( l_renderer, p_name);
			m_addedLights[p_name] = l_pReturn;
		}
		else if (p_type == Light::ePoint)
		{
			l_pReturn = new PointLight( l_renderer, p_name);
			m_addedLights[p_name] = l_pReturn;
		}
		else if (p_type == Light::eDirectional)
		{
			l_pReturn = new DirectionalLight( l_renderer, p_name);
			m_addedLights[p_name] = l_pReturn;
		}
		else
		{
			Log::LogMessage( C3D_T( "CreateLight - Can't create light %s - unknown light type"), p_name.c_str());
		}
	}

	return l_pReturn;
}



void Scene :: CreateList( NormalsMode p_nm, bool p_showNormals)
{
	m_normalsMode = p_nm;

	if (m_rootNode != NULL)
	{
		m_nbFaces = 0;
		m_nbVertex = 0;

		if (m_newlyAddedPrimitives.size() > 0)
		{
			GeometryStrMap::iterator l_it = m_newlyAddedPrimitives.begin();

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

		Log::LogMessage( C3D_T( "CreateList - %s - NbVertex : %d - NbFaces : %d"), m_name.c_str(), m_nbVertex, m_nbFaces);
		m_changed = false;
	}
}



SceneNode * Scene :: GetNode( const String & p_name)const
{
	SceneNode * l_pReturn = NULL;

	if (p_name == C3D_T( "RootNode"))
	{
		l_pReturn = m_rootNode;
	}
	else
	{
		SceneNodeStrMap::const_iterator l_it = m_addedNodes.find( p_name);

		if (l_it != m_addedNodes.end())
		{
			l_pReturn = l_it->second;
		}
	}

	return l_pReturn;
}



void Scene :: AddNode( SceneNode * p_node)
{
	SceneNodeStrMap::const_iterator l_it = m_addedNodes.find( p_node->GetName());

	if (l_it != m_addedNodes.end())
	{
		Log::LogMessage( C3D_T( "AddNode - Can't add node %s - A node with that name already exists"), p_node->GetName().c_str());
	}
	else
	{
		m_addedNodes[p_node->GetName()] = p_node;
	}
}



void Scene :: AddLight( Light * p_light)
{
	LightStrMap::const_iterator l_it = m_addedLights.find( p_light->GetName());

	if (l_it != m_addedLights.end())
	{
		Log::LogMessage( C3D_T( "AddLight - Can't add light %s - A light with that name already exists"), p_light->GetName().c_str());
	}
	else
	{
		m_addedLights[p_light->GetName()] = p_light;
	}
}



void Scene :: AddGeometry( Geometry * p_geometry)
{
	GeometryStrMap::const_iterator l_it = m_addedPrimitives.find( p_geometry->GetName());

	if (l_it != m_addedPrimitives.end())
	{
		Log::LogMessage( C3D_T( "AddGeometry - Can't add geometry %s - A geometry with that name already exists"), p_geometry->GetName().c_str());
	}
	else
	{
		m_addedPrimitives[p_geometry->GetName()] = p_geometry;
		m_newlyAddedPrimitives[p_geometry->GetName()] = p_geometry;
		m_changed = true;
	}
}



Geometry * Scene :: GetGeometry( String p_name)
{
	Geometry * l_pReturn = NULL;

	GeometryStrMap::iterator l_it = m_addedPrimitives.find( p_name);

	if (l_it != m_addedPrimitives.end())
	{
		l_pReturn = l_it->second;
	}

	return l_pReturn;
}

void Scene :: RemoveGeometry( Geometry * p_geometry)
{
	if (p_geometry != NULL)
	{
		GeometryStrMap::iterator l_it = m_addedPrimitives.find( p_geometry->GetName());

		if (l_it != m_addedPrimitives.end())
		{
			m_addedPrimitives.erase( l_it);
		}

		m_arrayPrimitivesToDelete.push_back( p_geometry);
	}
}

void Scene :: RemoveLight( Light * p_pLight)
{
	if (p_pLight != NULL)
	{
		LightStrMap::iterator l_it = m_addedLights.find( p_pLight->GetName());

		if (l_it != m_addedLights.end())
		{
			m_addedLights.erase( l_it);
		}

		m_arrayLightsToDelete.push_back( p_pLight);
	}
}

void Scene :: RemoveNode( SceneNode * p_pNode)
{
	if (p_pNode != NULL)
	{
		SceneNodeStrMap::iterator l_it = m_addedNodes.find( p_pNode->GetName());

		if (l_it != m_addedNodes.end())
		{
			m_addedNodes.erase( l_it);
		}

		m_arrayNodesToDelete.push_back( p_pNode);
	}
}

void Scene :: RemoveCamera( Camera * p_pCamera)
{
	if (p_pCamera != NULL)
	{
		CameraStrMap::iterator l_it = m_addedCameras.find( p_pCamera->GetName());

		if (l_it != m_addedCameras.end())
		{
			m_addedCameras.erase( l_it);
		}

		m_arrayCamerasToDelete.push_back( p_pCamera);
	}
}

void Scene :: RemoveAllLights()
{
	LightStrMap::iterator l_it = m_addedLights.begin();

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
	SceneNodeStrMap::iterator l_it = m_addedNodes.begin();

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
	GeometryStrMap::iterator l_it = m_addedPrimitives.begin();

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
	CameraStrMap::iterator l_it = m_addedCameras.begin();

	while (m_addedCameras.size() > 0)
	{
		m_arrayCamerasToDelete.push_back( l_it->second);
		m_addedCameras.erase( l_it);
		l_it = m_addedCameras.begin();
	}

	m_addedCameras.clear();
}

std::map <String, bool> Scene :: GetGeometriesVisibility()
{
	std::map <String, bool> l_mapReturn;
	GeometryStrMap::iterator l_it = m_addedPrimitives.begin();

	while (l_it != m_addedPrimitives.end())
	{
		l_mapReturn[l_it->first] = l_it->second->GetParent()->IsVisible();
		++l_it;
	}
	return l_mapReturn;
}

bool Scene :: Write( General::Utils::FileIO * p_pFile)const
{
	bool l_bReturn = true;

	Log::LogMessage( C3D_T( "Writing Scene Name"));

	if (l_bReturn)
	{
		Log::LogMessage( C3D_T( "Writing Root Camera"));

		if ( ! m_rootCamera->Write( p_pFile))
		{
			l_bReturn = false;
		}
	}

	if (l_bReturn)
	{
		Log::LogMessage( C3D_T( "Writing Root Scene Node"));

		if ( ! m_rootNode->Write( p_pFile))
		{
			l_bReturn = false;
		}
	}

	if (l_bReturn)
	{
		Log::LogMessage( C3D_T( "Writing Lights"));

		if ( ! _writeLights( p_pFile))
		{
			l_bReturn = false;
		}
	}

	if (l_bReturn)
	{
		Log::LogMessage( C3D_T( "Writing Geometries"));

		if ( ! _writeGeometries( p_pFile))
		{
			l_bReturn = false;
		}
	}

	return l_bReturn;
}

bool Scene :: Import3DS( const String & p_file)
{
	SMaxImporter l_importer;
	return _importExternal( p_file, & l_importer);
}

bool Scene :: ImportObj( const String & p_file)
{
	ObjImporter l_importer;
	return _importExternal( p_file, & l_importer);
}

bool Scene :: ImportMD2( const String & p_file, const String & p_texName)
{
	Md2Importer l_importer( p_texName);
	return _importExternal( p_file, & l_importer);
}

bool Scene :: ImportMD3( const String & p_file)
{
	Md3Importer l_importer;
	return _importExternal( p_file, & l_importer);
}

bool Scene :: ImportPLY( const String & p_file)
{
	PlyImporter l_importer;
	return _importExternal( p_file, & l_importer);
}

bool Scene :: ImportASE( const String & p_file)
{
	AseImporter l_importer;
	return _importExternal( p_file, & l_importer);
}

bool Scene :: ImportBSP( const String & p_file)
{
//	BspImporter l_importer;
//	return _importExternal( p_file, & l_importer);
	return false;
}

bool Scene :: _writeLights( FileIO * p_pFile)const
{
	bool l_bReturn = true;
	size_t l_nbLights = m_addedLights.size();

	Log::LogMessage( C3D_T( "NbLights : %d"), l_nbLights);
	Light::eTYPE l_type;
	LightStrMap::const_iterator l_it = m_addedLights.begin();

	while (l_bReturn && l_it != m_addedLights.end())
	{
		l_type = l_it->second->GetLightType();

		if (l_type == Light::eDirectional)
		{
			l_bReturn = static_cast <DirectionalLight *>( l_it->second)->Write( p_pFile);
		}
		else if (l_type == Light::ePoint)
		{
			l_bReturn = static_cast <PointLight *>( l_it->second)->Write( p_pFile);
		}
		else if (l_type == Light::eSpot)
		{
			l_bReturn = static_cast <SpotLight *>( l_it->second)->Write( p_pFile);
		}

		++l_it;
	}

	return l_bReturn;
}

bool Scene :: _writeGeometries( General::Utils::FileIO * p_pFile)const
{
	bool l_bReturn = true;
	size_t l_nbGeometries = m_addedPrimitives.size();

	Log::LogMessage( C3D_T( "NbGeometries : %d"), l_nbGeometries);
	GeometryStrMap::const_iterator l_it = m_addedPrimitives.begin();

	while (l_bReturn && l_it != m_addedPrimitives.end())
	{
		l_bReturn = l_it->second->Write( p_pFile);
		++l_it;
	}

	return l_bReturn;
}

bool Scene :: _importExternal( const String & p_fileName, ExternalImporter * p_importer)
{
	bool l_bReturn = true;

	if (p_importer->Import( p_fileName))
	{
		for (GeometryStrMap::iterator l_it = p_importer->m_geometries.begin() ; l_it != p_importer->m_geometries.end() ; ++l_it)
		{
			if (m_addedPrimitives.find( l_it->first) == m_addedPrimitives.end())
			{
				m_addedPrimitives.insert( GeometryStrMap::value_type( l_it->first, l_it->second));
				m_newlyAddedPrimitives.insert( GeometryStrMap::value_type( l_it->first, l_it->second));
				Log::LogMessage( C3D_T( "_importExternal - Geometry %s added"), l_it->first.c_str());
			}
			else
			{
				delete l_it->second;
			}
		}

		m_changed = true;
		l_bReturn = true;
	}

	return l_bReturn;
}

void Scene :: Select( Ray * p_ray, Geometry ** p_geo, Submesh ** p_submesh, Face ** p_face, Vector3f ** p_vertex)
{
	Vector3f l_min;
	Vector3f l_max;
	Geometry * l_geo, *l_selectedGeo=NULL;
	Mesh * l_mesh;
	Submesh * l_submesh, *l_selectedSubmesh=NULL;
	unsigned int l_nbSubmeshes;
	SmoothGroupPtrMap::const_iterator l_itGroupsEnd;
	Face * l_face, *l_selectedFace=NULL;
	Vector3f * l_selectedVertex=NULL;
	float l_geoDist = 10e6, l_faceDist = 10e6, l_vertexDist = 10e6;
	float l_curgeoDist, l_curfaceDist, l_curvertexDist;
	ComboBox * l_box;
	Sphere * l_sphere;
	SmoothingGroup * l_group;

	for (GeometryStrMap::iterator l_it = m_addedPrimitives.begin() ; l_it != m_addedPrimitives.end() ; ++l_it)
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

						if( l_sphere != NULL)
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

	if (*p_geo != NULL)
	{
		*p_geo = l_selectedGeo;
	}

	if (*p_submesh != NULL)
	{
		*p_submesh = l_selectedSubmesh;
	}

	if (*p_face != NULL)
	{
		*p_face = l_selectedFace;
	}

	if (*p_vertex != NULL)
	{
		*p_vertex = l_selectedVertex;
	}
}

void Scene :: _deleteToDelete()
{
	for (size_t i = 0 ; i < m_arrayLightsToDelete.size() ; i++)
	{
		delete m_arrayLightsToDelete[i];
	}

	m_arrayLightsToDelete.clear();

	for (size_t i = 0 ; i < m_arrayPrimitivesToDelete.size() ; i++)
	{
		m_arrayPrimitivesToDelete[i]->Detach();
		delete m_arrayPrimitivesToDelete[i];
	}
	
	for (size_t i = 0 ; i < m_arrayNodesToDelete.size() ; i++)
	{
		delete m_arrayNodesToDelete[i];
	}

	m_arrayNodesToDelete.clear();

	m_arrayPrimitivesToDelete.clear();
	
	for (size_t i = 0 ; i < m_arrayCamerasToDelete.size() ; i++)
	{
		delete m_arrayCamerasToDelete[i];
	}

	m_arrayCamerasToDelete.clear();
}