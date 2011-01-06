#include "Castor3D/PrecompiledHeader.h"
#include "Castor3D/scene/SceneNode.h"

#include "Castor3D/main/MovableObject.h"
#include "Castor3D/camera/Ray.h"
#include "Castor3D/main/Pipeline.h"
#include "Castor3D/geometry/basic/Vertex.h"
#include "Castor3D/geometry/primitives/Geometry.h"

using namespace Castor3D;

unsigned long long SceneNode::Count = 0;

SceneNode :: SceneNode( const String & p_name)
	:	m_name( p_name)
	,	m_visible( true)
	,	m_scale( 1.0, 1.0, 1.0)
	,	m_position( 0, 0, 0)
	,	m_parent( NULL)
	,	m_displayable( p_name == "RootNode" || p_name == "RootNode" || p_name == "RootNode")
{
	m_orientation.ToRotationMatrix( m_matrix);

	if (m_name.empty())
	{
		m_name = CU_T( "SceneNode_%d");
		m_name << Count;
	}

	Count++;
}

SceneNode :: ~SceneNode()
{
	if (m_parent != NULL)
	{
		m_parent->DetachChild( this);
	}

	if (m_childs.size() > 0)
	{
		SceneNodePtrStrMap::iterator l_it = m_childs.begin();

		while (l_it != m_childs.end())
		{
			l_it->second->Detach();
			l_it = m_childs.begin();
		}

		m_childs.clear();
	}

	Count--;
}

void SceneNode :: AttachObject( MovableObject * p_pObject)
{
	if (p_pObject != NULL)
	{
		m_attachedObjects[p_pObject->GetName()] = p_pObject;
		p_pObject->AttachTo( this);
	}
}

void SceneNode :: DetachObject( MovableObject * p_pObject)
{
	if (p_pObject != NULL)
	{
		MovableObjectPtrStrMap::iterator l_it = m_attachedObjects.find( p_pObject->GetName());

		if (l_it != m_attachedObjects.end())
		{
			m_attachedObjects.erase( l_it);
			p_pObject->AttachTo( NULL);
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
		SceneNodePtrStrMap::iterator l_it = m_childs.begin();

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
		m_childs.insert( std::pair <String, SceneNode *>( l_name, p_child));
	}
	else
	{
		Logger::LogMessage( CU_T( "Can't add SceneNode %s - Already in childs"), l_name.c_str());
	}
}

void SceneNode :: DetachChild( SceneNode * p_child)
{
	String l_name = p_child->GetName();
	SceneNodePtrStrMap::iterator l_it = m_childs.find( l_name);

	if (l_it != m_childs.end())
	{
		SceneNode * l_current = l_it->second;
		m_childs.erase( l_it);
		l_current->Detach();
	}
	else
	{
		Logger::LogMessage( CU_T( "Can't remove SceneNode %s - Not in childs"), l_name.c_str());
	}
}

void SceneNode :: DetachChild( const String & p_childName)
{
	SceneNodePtrStrMap::iterator l_it = m_childs.find( p_childName);

	if (l_it != m_childs.end())
	{
		SceneNode * l_current = l_it->second;
		m_childs.erase( l_it);
		l_current->Detach();
	}
	else
	{
		Logger::LogMessage( CU_T( "Can't remove SceneNode %s - Not in childs"), p_childName.c_str());
	}
}

void SceneNode :: DetachAllChilds()
{
	SceneNodePtrStrMap::iterator l_it = m_childs.begin();
	SceneNode * l_current;

	while (l_it != m_childs.end())
	{
		l_current = l_it->second;
		m_childs.erase( l_it);
		l_current->Detach();
		l_it = m_childs.begin();
	}

	m_childs.clear();
}

void SceneNode :: Yaw( const Angle & p_angle)
{
	Quaternion l_tmp( Point3r( 0.0, 1.0, 0.0), p_angle);
	m_orientation *= l_tmp;
	m_matrix.Identity();
	translate( m_matrix, m_position);
	rotate( m_matrix, m_orientation);
	scale( m_matrix, m_scale);
}

void SceneNode :: Pitch( const Angle & p_angle)
{
	Quaternion l_tmp( Point3r( 1.0, 0.0, 0.0), p_angle);
	m_orientation *= l_tmp;
	m_matrix.Identity();
	translate( m_matrix, m_position);
	rotate( m_matrix, m_orientation);
	scale( m_matrix, m_scale);
}

void SceneNode :: Roll( const Angle & p_angle)
{
	Quaternion l_tmp( Point3r( 0.0, 0.0, 1.0), p_angle);
	m_orientation *= l_tmp;
	m_matrix.Identity();
	translate( m_matrix, m_position);
	rotate( m_matrix, m_orientation);
	scale( m_matrix, m_scale);
}

void SceneNode :: Rotate( real x, real y, real z, real w)
{
	m_orientation *= Quaternion( x, y, z, w);
	m_matrix.Identity();
	translate( m_matrix, m_position);
	rotate( m_matrix, m_orientation);
	scale( m_matrix, m_scale);
}

void SceneNode :: Rotate( const Quaternion & p_orientation)
{
	Rotate( p_orientation[0], p_orientation[1], p_orientation[2], p_orientation[3]);
}

void SceneNode :: Rotate	( const Point3r & p_axis, const Angle & p_angle)
{
	Rotate( p_axis[0], p_axis[1], p_axis[2], p_angle.Radians());
}

void SceneNode :: Rotate	( real * p_quat)
{
	Rotate( p_quat[0], p_quat[1], p_quat[2], p_quat[3]);
}

void SceneNode :: Translate( real x, real y, real z)
{
	m_position[0] += x;
	m_position[1] += y;
	m_position[2] += z;
	m_matrix.Identity();
	translate( m_matrix, m_position);
	rotate( m_matrix, m_orientation);
	scale( m_matrix, m_scale);
}

void SceneNode :: Translate( const Point3r & p_position)
{
	Translate( p_position[0], p_position[1], p_position[2]);
}

void SceneNode :: Translate( real * p_pCoords)
{
	Translate( p_pCoords[0], p_pCoords[1], p_pCoords[2]);
}

void SceneNode :: Scale( real x, real y, real z)
{
	m_scale[0] += x;
	m_scale[1] += y;
	m_scale[2] += z;
	m_matrix.Identity();
	translate( m_matrix, m_position);
	rotate( m_matrix, m_orientation);
	scale( m_matrix, m_scale);
}

void SceneNode :: Scale( const Point3r & p_scale)
{
	Scale( p_scale[0], p_scale[1], p_scale[2]);
}

void SceneNode :: Scale( real * p_pCoords)
{
	Scale( p_pCoords[0], p_pCoords[1], p_pCoords[2]);
}

void SceneNode :: SetOrientation	( real x, real y, real z, real w)
{
	m_orientation = Quaternion( x, y, z, w);

	if (m_parent != NULL)
	{
		m_derivedOrientation = m_parent->GetDerivedOrientation() * m_orientation;
	}

	m_matrix.Identity();
	translate( m_matrix, m_position);
	rotate( m_matrix, m_orientation);
	scale( m_matrix, m_scale);
}

void SceneNode :: SetOrientation( const Quaternion & p_orientation)
{
	SetOrientation( p_orientation[0], p_orientation[1], p_orientation[2], p_orientation[3]);
}

void SceneNode :: SetOrientation	( const Point3r & p_axis, const Angle & p_angle)
{
	SetOrientation( p_axis[0], p_axis[1], p_axis[2], p_angle.Radians());
}

void SceneNode :: SetOrientation	( real * p_quat)
{
	SetOrientation( p_quat[0], p_quat[1], p_quat[2], p_quat[3]);
}

void SceneNode :: SetPosition( real x, real y, real z)
{
	m_position = Point3r( x, y, z);

	if (m_parent != NULL)
	{
		m_derivedPosition = m_parent->GetDerivedPosition() + m_position;
	}

	m_matrix.Identity();
	translate( m_matrix, m_position);
	rotate( m_matrix, m_orientation);
	scale( m_matrix, m_scale);
}

void SceneNode :: SetPosition( const Point3r & p_position)
{
	SetPosition( p_position[0], p_position[1], p_position[2]);
}

void SceneNode :: SetPosition( real * p_pCoords)
{
	SetPosition( p_pCoords[0], p_pCoords[1], p_pCoords[2]);
}

void SceneNode :: SetScale( real x, real y, real z)
{
	m_scale = Point3r( x, y, z);

	if (m_parent != NULL)
	{
		m_derivedScale = m_parent->GetDerivedScale() + m_scale;
	}

	m_matrix.Identity();
	translate( m_matrix, m_position);
	rotate( m_matrix, m_orientation);
	scale( m_matrix, m_scale);
}

void SceneNode :: SetScale( const Point3r & p_scale)
{
	SetScale( p_scale[0], p_scale[1], p_scale[2]);
}

void SceneNode :: SetScale( real * p_pCoords)
{
	SetScale( p_pCoords[0], p_pCoords[1], p_pCoords[2]);
}

bool SceneNode :: Write( Castor::Utils::File & p_pFile)const
{
	bool l_bReturn = false;
	Logger::LogMessage( CU_T( "Writing Node %s"), m_name.c_str());
	l_bReturn = WriteOne( p_pFile);

	if (l_bReturn)
	{
		Logger::LogMessage( CU_T( "Writing Childs"));
		SceneNodePtrStrMap::const_iterator l_it = m_childs.begin();

		while (l_it != m_childs.end() && l_bReturn)
		{
			l_bReturn = l_it->second->Write( p_pFile);
			++l_it;
		}
	}

	if (l_bReturn)
	{
		Logger::LogMessage( CU_T( "Childs Written"));
	}

	return l_bReturn;
}

bool SceneNode :: WriteOne( Castor::Utils::File & p_pFile)const
{
	bool l_bReturn = true;

	if (m_name != "RootNode")
	{
		l_bReturn = p_pFile.WriteLine( "scene_node " + m_name + "\n{\n");

		if (l_bReturn && m_parent != NULL && m_parent->GetName() != "RootNode")
		{
			l_bReturn = p_pFile.WriteLine( "\tattach_to " + m_parent->m_name + "\n");
		}

		if (l_bReturn)
		{
			l_bReturn = p_pFile.Print( 256, "\torientation %f %f %f %f\n", m_orientation[0], m_orientation[1], m_orientation[2], m_orientation[3]);
		}

		if (l_bReturn)
		{
			l_bReturn = p_pFile.Print( 256, "\tposition %f %f %f\n", m_position[0], m_position[1], m_position[2]);
		}

		if (l_bReturn)
		{
			l_bReturn = p_pFile.Print( 256, "\tscale %f %f %f\n", m_scale[0], m_scale[1], m_scale[2]);
		}

		if (l_bReturn)
		{
			l_bReturn = p_pFile.WriteLine( "}\n");
		}
	}

	return l_bReturn;
}

void SceneNode :: Render( eDRAW_TYPE p_displayMode)
{
	if (m_visible && m_displayable)
	{
		Pipeline::PushMatrix();
		Pipeline::MultMatrix( m_matrix);

		for (MovableObjectPtrStrMap::iterator l_it = m_attachedObjects.begin() ; l_it != m_attachedObjects.end() ; ++l_it)
		{
			if (l_it->second->GetType() == MovableObject::eGeometry)
			{
				l_it->second->Render( p_displayMode);
			}
		}

		for (SceneNodePtrStrMap::iterator l_it = m_childs.begin() ; l_it != m_childs.end() ; ++l_it)
		{
			l_it->second->Render( p_displayMode);
		}

		Pipeline::PopMatrix();
	}
}

void SceneNode :: EndRender()
{
	if (m_visible && m_displayable)
	{
		for (MovableObjectPtrStrMap::iterator l_it = m_attachedObjects.begin() ; l_it != m_attachedObjects.end() ; ++l_it)
		{
			l_it->second->EndRender();
		}

		for (SceneNodePtrStrMap::iterator l_it = m_childs.begin() ; l_it != m_childs.end() ; ++l_it)
		{
			l_it->second->EndRender();
		}
	}
}

void SceneNode :: CreateList( eNORMALS_MODE p_nm, bool p_showNormals, size_t & p_nbFaces, size_t & p_nbVertex)const
{
	for (MovableObjectPtrStrMap::const_iterator l_it = m_attachedObjects.begin() ; l_it != m_attachedObjects.end() ; ++l_it)
	{
		if (l_it->second->GetType() == MovableObject::eGeometry)
		{
			((Geometry *)l_it->second)->CreateBuffers( p_nm, p_nbFaces, p_nbVertex);
		}
	}

	for (SceneNodePtrStrMap::const_iterator l_it = m_childs.begin() ; l_it != m_childs.end() ; ++l_it)
	{
		l_it->second->CreateList( p_nm, p_showNormals, p_nbFaces, p_nbVertex);
	}
}

Geometry * SceneNode :: GetNearestGeometry( Ray * p_pRay, real & p_fDistance, FacePtr * p_ppFace, SubmeshPtr * p_ppSubmesh)
{
	Geometry * l_pReturn = NULL;
	real l_fDistance;

	for (MovableObjectPtrStrMap::iterator l_it = m_attachedObjects.begin() ; l_it != m_attachedObjects.end() ; ++l_it)
	{
		if (l_it->second->GetType() == MovableObject::eGeometry)
		{
			if ((l_fDistance = p_pRay->Intersects( (Geometry *)l_it->second, p_ppFace, p_ppSubmesh)) >= 0.0 && l_fDistance < p_fDistance)
			{
				p_fDistance = l_fDistance;
				l_pReturn = (Geometry *)l_it->second;
			}
		}
	}

	Geometry * l_pTmp;

	for (SceneNodePtrStrMap::iterator l_it = m_childs.begin() ; l_it != m_childs.end() ; ++l_it)
	{
		if ( ! (l_pTmp = l_it->second->GetNearestGeometry( p_pRay, p_fDistance, p_ppFace, p_ppSubmesh)) == NULL)
		{
			l_pReturn = l_pTmp;
		}
	}

	return l_pReturn;
}