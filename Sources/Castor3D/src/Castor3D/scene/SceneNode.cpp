#include "PrecompiledHeader.h"

#include "scene/Module_Scene.h"

#include "scene/SceneNode.h"

#include "geometry/primitives/Geometry.h"
#include "geometry/primitives/MovableObject.h"
#include "scene/Scene.h"
#include "main/Root.h"
#include "camera/Ray.h"
#include "Log.h"

#include "render_system/RenderSystem.h"
#include "render_system/SceneRenderer.h"

using namespace Castor3D;

int SceneNode :: s_nbSceneNodes = 0;

SceneNode :: SceneNode( SceneNodeRenderer * p_renderer, const String & p_name)
	:	m_name( p_name),
		m_visible( true),
		m_scale( Vector3f( 1.0, 1.0, 1.0)),
		m_matrix( new float[16]),
		m_parent( NULL),
		m_renderer( p_renderer),
		m_displayable( p_name == "RootNode")
{
	m_orientation.ToRotationMatrix( m_matrix);

	m_renderer->SetTarget( this);

	m_geometriesBegin = m_attachedGeometries.begin();
	m_geometriesEnd = m_attachedGeometries.end();
	m_childsBegin = m_childs.begin();
	m_childsEnd = m_childs.end();

	Vector3f::s_vertexNumber--;

	if (m_name.empty())
	{
		m_name = C3D_T( "SceneNode_%d");
		m_name << s_nbSceneNodes;
	}

	s_nbSceneNodes++;
}

SceneNode :: ~SceneNode()
{
	if (m_parent != NULL)
	{
		m_parent->DetachChild( this);
	}

	SceneNodeStrMap::iterator l_it = m_childs.begin();

	while (l_it != m_childs.end())
	{
		l_it->second->Detach();
		l_it++;
	}

	m_childs.clear();
	delete [] m_matrix;
	Vector3f::s_vertexNumber++;
}

void SceneNode :: AttachGeometry( MovableObject * p_geometry)
{
	if (p_geometry != NULL)
	{
		m_attachedGeometries[p_geometry->GetName()] = p_geometry;
		p_geometry->SetParent( this);
		m_geometriesBegin = m_attachedGeometries.begin();
		m_geometriesEnd = m_attachedGeometries.end();
	}
}

void SceneNode :: DetachGeometry( MovableObject * p_geometry)
{
	if (p_geometry != NULL)
	{
		MovableObjectStrMap::iterator l_it = m_attachedGeometries.find( p_geometry->GetName());

		if (l_it != m_attachedGeometries.end())
		{
			m_attachedGeometries.erase( l_it);
			p_geometry->SetParent( NULL);
			m_geometriesBegin = m_attachedGeometries.begin();
			m_geometriesEnd = m_attachedGeometries.end();
		}
	}
}

void SceneNode :: AttachTo( SceneNode * p_parent)
{
	if (m_parent != NULL)
	{
		m_parent->DetachChild( this);
	}

	m_parent = p_parent;

	if (m_parent != NULL)
	{
		m_displayable = m_parent->m_displayable;

		m_parent->AddChild( this);
		m_derivedOrientation = m_parent->GetDerivedOrientation() * m_orientation;
		m_derivedPosition = m_parent->GetDerivedPosition() + m_position;
		m_derivedScale = m_parent->GetDerivedScale() * m_scale;
	}
}

void SceneNode :: Detach()
{
	if (m_parent != NULL)
	{
		m_displayable = false;
		SceneNode * l_parent = m_parent;
		m_parent = NULL;
		l_parent->DetachChild( this);
	}
}

bool SceneNode :: HasChild( const String & p_name)
{
	bool l_bFound = false;

	if (m_childs.find( p_name) == m_childs.end())
	{
		SceneNodeStrMap::iterator l_it = m_childs.begin();

		while (l_it != m_childs.end() && ! l_bFound)
		{
			if (l_it->second->HasChild( p_name))
			{
				l_bFound = true;
			}

			++l_it;
		}
	}

	return l_bFound;
}

void SceneNode :: AddChild( SceneNode * p_child)
{
	String l_name = p_child->GetName();

	if (m_childs.find( l_name) == m_childs.end())
	{
		m_childs[l_name] = p_child;
	}
	else
	{
		Log::LogMessage( C3D_T( "Can't add SceneNode %s - Already in childs"), l_name.c_str());
	}

	m_childsBegin = m_childs.begin();
	m_childsEnd = m_childs.end();
}

void SceneNode :: DetachChild( SceneNode * p_child)
{
	String l_name = p_child->GetName();
	SceneNodeStrMap::iterator l_it = m_childs.find( l_name);
	SceneNode * l_current;

	if (l_it != m_childs.end())
	{
		l_current = l_it->second;
		m_childs.erase( l_it);
		l_current->Detach();
	}
	else
	{
		Log::LogMessage( C3D_T( "Can't remove SceneNode %s - Not in childs"), l_name.c_str());
	}

	m_childsBegin = m_childs.begin();
	m_childsEnd = m_childs.end();
}

void SceneNode :: DetachChild( const String & p_childName)
{
	SceneNodeStrMap::iterator l_it = m_childs.find( p_childName);
	SceneNode * l_current;

	if (l_it != m_childs.end())
	{
		l_current = l_it->second;
		m_childs.erase( l_it);
		l_current->Detach();
	}
	else
	{
		Log::LogMessage( C3D_T( "Can't remove SceneNode %s - Not in childs"), p_childName.c_str());
	}

	m_childsBegin = m_childs.begin();
	m_childsEnd = m_childs.end();
}

void SceneNode :: DetachAllChilds()
{
	SceneNodeStrMap::iterator l_it = m_childs.begin();
	SceneNode * l_current;

	while (l_it != m_childs.end())
	{
		l_current = l_it->second;
		m_childs.erase( l_it);
		l_current->Detach();
		l_it = m_childs.begin();
	}

	m_childs.clear();
	m_childsBegin = m_childs.begin();
	m_childsEnd = m_childs.end();
}

void SceneNode :: DestroyChild( SceneNode * p_child)
{
	String l_name = p_child->GetName();
	SceneNodeStrMap::iterator l_it = m_childs.find( l_name);
	SceneNode * l_current;

	if (l_it != m_childs.end())
	{
		l_current = l_it->second;
		m_childs.erase( l_it);
		l_current->Detach();
		delete l_current;
	}
	else
	{
		Log::LogMessage( C3D_T( "Can't remove SceneNode %s  - Not in childs"), l_name.c_str());
	}

	m_childsBegin = m_childs.begin();
	m_childsEnd = m_childs.end();
}

void SceneNode :: DestroyChild( const String & p_childName)
{
	SceneNodeStrMap::iterator l_it = m_childs.find( p_childName);
	SceneNode * l_current;

	if (l_it != m_childs.end())
	{
		l_current = l_it->second;
		m_childs.erase( l_it);
		l_current->Detach();
		delete l_current;
	}
	else
	{
		Log::LogMessage( C3D_T( "Can't remove SceneNode %s - Not in childs"), p_childName.c_str());
	}

	m_childsBegin = m_childs.begin();
	m_childsEnd = m_childs.end();
}

void SceneNode :: DestroyAllChilds()
{
	SceneNodeStrMap::iterator l_it = m_childs.begin();
	SceneNode * l_current;

	while (l_it != m_childs.end())
	{
		l_current = l_it->second;
		m_childs.erase( l_it);
		l_current->Detach();
		delete l_current;
		l_it = m_childs.begin();
	}

	m_childs.clear();
	m_childsBegin = m_childs.begin();
	m_childsEnd = m_childs.end();
}

void SceneNode :: DestroyAllChildsRecursive()
{
	SceneNodeStrMap::iterator l_it = m_childs.begin();
	SceneNode * l_current;

	while (l_it != m_childs.end())
	{
		l_current = l_it->second;
		m_childs.erase( l_it);
		l_current->Detach();
		l_it->second->DestroyAllChildsRecursive();
		l_it = m_childs.begin();
	}

	m_childs.clear();
	m_childsBegin = m_childs.begin();
	m_childsEnd = m_childs.end();
}

void SceneNode :: Yaw( float p_angle)
{
	Quaternion l_tmp( Vector3f( 0.0, 1.0, 0.0), p_angle);
	m_orientation *= l_tmp;
	m_orientation.ToRotationMatrix( m_matrix);
}

void SceneNode :: Pitch( float p_angle)
{
	Quaternion l_tmp( Vector3f( 1.0, 0.0, 0.0), p_angle);
	m_orientation *= l_tmp;
	m_orientation.ToRotationMatrix( m_matrix);
}

void SceneNode :: Roll( float p_angle)
{
	Quaternion l_tmp( Vector3f( 0.0, 0.0, 1.0), p_angle);
	m_orientation *= l_tmp;
	m_orientation.ToRotationMatrix( m_matrix);
}

void SceneNode :: Rotate( const Quaternion & p_quat)
{
	m_orientation *= p_quat;
	m_orientation.ToRotationMatrix( m_matrix);
}

void SceneNode :: Translate( const Vector3f & p_t)
{
	m_position += p_t;
}

void SceneNode :: CreateList( NormalsMode p_nm, bool p_showNormals,
							  size_t & p_nbFaces, size_t & p_nbVertex)const
{
	MovableObjectStrMap::const_iterator l_it = m_attachedGeometries.begin();

	while (l_it != m_attachedGeometries.end())
	{
		static_cast <Geometry *>( l_it->second)->CreateBuffers( p_nm, p_nbFaces, p_nbVertex);
		++l_it;
	}

	SceneNodeStrMap::const_iterator l_it2 = m_childs.begin();

	while (l_it2 != m_childs.end())
	{
		l_it2->second->CreateList( p_nm, p_showNormals, p_nbFaces, p_nbVertex);
		++l_it2;
	}
}

void SceneNode :: Draw( DrawType p_displayMode)
{
	if (m_visible && m_displayable)
	{
		m_renderer->ApplyTransformations();

		MovableObjectStrMap::iterator l_geometriesIt = m_geometriesBegin;

		while (l_geometriesIt != m_geometriesEnd)
		{
			static_cast <Geometry *>( l_geometriesIt->second)->Render( p_displayMode);
			++l_geometriesIt;
		}

		SceneNodeStrMap::iterator l_childsIt = m_childsBegin;

		while (l_childsIt != m_childsEnd)
		{
			l_childsIt->second->Draw( p_displayMode);
			++l_childsIt;
		}

		m_renderer->RemoveTransformations();

	}
}

void SceneNode :: ApplyTransformations()
{
	m_renderer->ApplyTransformations();
}

void SceneNode :: RemoveTransformations()
{
	m_renderer->RemoveTransformations();
}

bool SceneNode :: Write( General::Utils::File & p_file)const
{
	bool l_bReturn = false;
	Log::LogMessage( C3D_T( "Writing Node %s"), m_name.c_str());
	l_bReturn = WriteOne( p_file);

	if (l_bReturn)
	{
		size_t l_nbChilds = m_childs.size();
		l_bReturn = (p_file.Write<size_t>( l_nbChilds) == sizeof( size_t));
	}

	if (l_bReturn)
	{
		Log::LogMessage( C3D_T( "Writing Childs"));
		SceneNodeStrMap::const_iterator l_it = m_childs.begin();

		while (l_it != m_childs.end() && l_bReturn)
		{
			l_bReturn = l_it->second->Write( p_file);
			++l_it;
		}
	}

	if (l_bReturn)
	{
		Log::LogMessage( C3D_T( "Childs Written"));
	}
	return l_bReturn;
}

bool SceneNode :: Read( General::Utils::File & p_file, Scene * p_scene)
{
	bool l_bReturn = true;
	DestroyAllChilds();
	l_bReturn = ReadOne( p_file);
	size_t l_nbChilds = 0;

	if (l_bReturn)
	{
		if (m_name != C3D_T( "RootNode"))
		{
			p_scene->AddNode( this);
		}

		Log::LogMessage( C3D_T( "Reading Node %s"), m_name.c_str());
		l_bReturn = (p_file.Read<size_t>( l_nbChilds) == sizeof( size_t));
	}

	if (l_bReturn)
	{
		Log::LogMessage( C3D_T( "Reading Childs"));
		SceneNode * l_node;
		for (size_t i = 0 ; i < l_nbChilds && l_bReturn ; i++)
		{
			l_node = new SceneNode( Root::GetRenderSystem()->CreateSceneNodeRenderer());
			l_bReturn = l_node->Read( p_file, p_scene);

			if (l_bReturn)
			{
				l_node->AttachTo( this);
			}
		}
	}

	if (l_bReturn)
	{
		Log::LogMessage( C3D_T( "Childs Read"));
	}

	return true;
}

bool SceneNode :: WriteOne( General::Utils::File & p_file)const
{
	bool l_bReturn = false;
	size_t l_nameLength = m_name.size();
	l_bReturn = (p_file.Write<size_t>( l_nameLength) == sizeof( size_t));

	if (l_bReturn)
	{
		l_bReturn = (p_file.WriteArray<Char>( m_name.c_str(), l_nameLength) == l_nameLength);
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Write<bool>( m_displayable) == sizeof( bool));
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Write<bool>( m_visible) == sizeof( bool));
	}

	if (l_bReturn)
	{
		l_bReturn = m_orientation.Write( p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = WriteVertex( m_position, p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = WriteVertex( m_scale, p_file);
	}

	return l_bReturn;
}

bool SceneNode :: ReadOne( General::Utils::File & p_file)
{
	bool l_bReturn = false;
	size_t l_nameLength = 0;

	l_bReturn = (p_file.Read<size_t>( l_nameLength) == sizeof( size_t));

	if (l_bReturn)
	{
		Char * l_name = new Char[l_nameLength+1];
		l_bReturn = (p_file.ReadArray<Char>( l_name, l_nameLength) == l_nameLength);

		if (l_bReturn)
		{
			l_name[l_nameLength] = 0;
			m_name = l_name;
			delete [] l_name;
		}
		else
		{
			delete [] l_name;
		}
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Read<bool>( m_displayable) == sizeof( bool));
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Read<bool>( m_visible) == sizeof( bool));
	}

	if (l_bReturn)
	{
		l_bReturn = m_orientation.Read( p_file);
	}

	if (l_bReturn)
	{
		m_orientation.ToRotationMatrix( m_matrix);
		l_bReturn = ReadVertex( & m_position, p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = ReadVertex( & m_scale, p_file);
	}

	return l_bReturn;
}

float * SceneNode :: Get4x4RotationMatrix()
{
	return m_matrix;
}

void SceneNode :: SetOrientation( const Quaternion & p_orientation)
{
	m_orientation = p_orientation;

	if (m_parent != NULL)
	{
		m_derivedOrientation = m_parent->GetDerivedOrientation() * m_orientation;
	}
}

void SceneNode :: SetPosition( const Vector3f & p_position)
{
	m_position = p_position;

	if (m_parent != NULL)
	{
		m_derivedPosition = m_parent->GetDerivedPosition() + m_position;
	}
}

void SceneNode :: SetPosition( float x, float y, float z)
{
	m_position = Vector3f( x, y, z);

	if (m_parent != NULL)
	{
		m_derivedPosition = m_parent->GetDerivedPosition() + m_position;
	}
}

void SceneNode :: SetScale( const Vector3f & p_scale)
{
	m_scale = p_scale;

	if (m_parent != NULL)
	{
		m_derivedScale = m_parent->GetDerivedScale() + m_scale;
	}
}

void SceneNode :: SetScale( float x, float y, float z)
{
	m_scale = Vector3f( x, y, z);

	if (m_parent != NULL)
	{
		m_derivedScale = m_parent->GetDerivedScale() + m_scale;
	}
}

Geometry * SceneNode :: GetNearestGeometry( Ray * p_pRay, float & p_fDistance, Face ** p_ppFace, Submesh ** p_ppSubmesh)
{
	Geometry * l_pReturn = NULL;

	MovableObjectStrMap::iterator l_geometriesIt = m_geometriesBegin;
	float l_fDistance;

	while (l_geometriesIt != m_geometriesEnd)
	{
		if ((l_fDistance = p_pRay->Intersects( (Geometry *)l_geometriesIt->second, p_ppFace, p_ppSubmesh)) >= 0.0 && l_fDistance < p_fDistance)
		{
			p_fDistance = l_fDistance;
			l_pReturn = (Geometry *)l_geometriesIt->second;
		}

		++l_geometriesIt;
	}

	SceneNodeStrMap::iterator l_childsIt = m_childsBegin;
	Geometry * l_pTmp = NULL;

	while (l_childsIt != m_childsEnd)
	{
		if ((l_pTmp = l_childsIt->second->GetNearestGeometry( p_pRay, p_fDistance, p_ppFace, p_ppSubmesh)) != NULL)
		{
			l_pReturn = l_pTmp;
		}

		++l_childsIt;
	}

	return l_pReturn;
}