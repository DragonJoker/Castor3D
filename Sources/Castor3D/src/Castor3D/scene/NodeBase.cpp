#include "PrecompiledHeader.h"
#include "scene/NodeBase.h"

#include "main/MovableObject.h"
#include "camera/Ray.h"
#include "main/Pipeline.h"
#include "geometry/basic/Vertex.h"

using namespace Castor3D;

int NodeBase :: s_nbNodes = 0;

NodeBase :: NodeBase( const String & p_name)
	:	m_name( p_name),
		m_visible( true),
		m_scale( 1.0, 1.0, 1.0),
		m_position( 0, 0, 0),
		m_parent( NULL),
		m_displayable( p_name == "GeometryRootNode" || p_name == "LightRootNode" || p_name == "CameraRootNode")
{
	m_orientation.ToRotationMatrix( m_matrix);

	Vertex::s_vertexNumber--;

	if (m_name.empty())
	{
		m_name = CU_T( "NodeBase_%d");
		m_name << s_nbNodes;
	}

	s_nbNodes++;
}

NodeBase :: ~NodeBase()
{
	if (m_parent != NULL)
	{
		m_parent->DetachChild( this);
	}

	if (m_childs.size() > 0)
	{
		C3DMap( String, NodeBase *)::iterator l_it = m_childs.begin();

		while (l_it != m_childs.end())
		{
			l_it->second->Detach();
			l_it = m_childs.begin();
		}

		m_childs.clear();
	}

	Vertex::s_vertexNumber++;
	s_nbNodes--;
}

void NodeBase :: AttachObject( MovableObject * p_pObject)
{
	if (p_pObject != NULL)
	{
		m_attachedObjects[p_pObject->GetName()] = p_pObject;
		p_pObject->AttachTo( this);
	}
}

void NodeBase :: DetachObject( MovableObject * p_pObject)
{
	if (p_pObject != NULL)
	{
		C3DMap( String, MovableObject *)::iterator l_it = m_attachedObjects.find( p_pObject->GetName());

		if (l_it != m_attachedObjects.end())
		{
			m_attachedObjects.erase( l_it);
			p_pObject->AttachTo( NULL);
		}
	}
}

void NodeBase :: AttachTo( NodeBase * p_parent)
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

void NodeBase :: Detach()
{
	if (m_parent != NULL)
	{
		m_displayable = false;
		NodeBase * l_parent = m_parent;
		m_parent = NULL;
		l_parent->DetachChild( this);
	}
}

bool NodeBase :: HasChild( const String & p_name)
{
	bool l_bFound = false;

	if (m_childs.find( p_name) == m_childs.end())
	{
		C3DMap( String, NodeBase *)::iterator l_it = m_childs.begin();

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

void NodeBase :: AddChild( NodeBase * p_child)
{
	String l_name = p_child->GetName();

	if (m_childs.find( l_name) == m_childs.end())
	{
		m_childs.insert( std::pair <String, NodeBase *>( l_name, p_child));
	}
	else
	{
		Logger::LogMessage( CU_T( "Can't add NodeBase %s - Already in childs"), l_name.c_str());
	}
}

void NodeBase :: DetachChild( NodeBase * p_child)
{
	String l_name = p_child->GetName();
	C3DMap( String, NodeBase *)::iterator l_it = m_childs.find( l_name);

	if (l_it != m_childs.end())
	{
		NodeBase * l_current = l_it->second;
		m_childs.erase( l_it);
		l_current->Detach();
	}
	else
	{
		Logger::LogMessage( CU_T( "Can't remove NodeBase %s - Not in childs"), l_name.c_str());
	}
}

void NodeBase :: DetachChild( const String & p_childName)
{
	C3DMap( String, NodeBase *)::iterator l_it = m_childs.find( p_childName);

	if (l_it != m_childs.end())
	{
		NodeBase * l_current = l_it->second;
		m_childs.erase( l_it);
		l_current->Detach();
	}
	else
	{
		Logger::LogMessage( CU_T( "Can't remove NodeBase %s - Not in childs"), p_childName.c_str());
	}
}

void NodeBase :: DetachAllChilds()
{
	C3DMap( String, NodeBase *)::iterator l_it = m_childs.begin();
	NodeBase * l_current;

	while (l_it != m_childs.end())
	{
		l_current = l_it->second;
		m_childs.erase( l_it);
		l_current->Detach();
		l_it = m_childs.begin();
	}

	m_childs.clear();
}

void NodeBase :: Yaw( const Angle & p_angle)
{
	Quaternion l_tmp( Point3r( 0.0, 1.0, 0.0), p_angle);
	m_orientation *= l_tmp;
	m_matrix.Identity();
	translate( m_matrix, m_position);
	rotate( m_matrix, m_orientation);
	scale( m_matrix, m_scale);
}

void NodeBase :: Pitch( const Angle & p_angle)
{
	Quaternion l_tmp( Point3r( 1.0, 0.0, 0.0), p_angle);
	m_orientation *= l_tmp;
	m_matrix.Identity();
	translate( m_matrix, m_position);
	rotate( m_matrix, m_orientation);
	scale( m_matrix, m_scale);
}

void NodeBase :: Roll( const Angle & p_angle)
{
	Quaternion l_tmp( Point3r( 0.0, 0.0, 1.0), p_angle);
	m_orientation *= l_tmp;
	m_matrix.Identity();
	translate( m_matrix, m_position);
	rotate( m_matrix, m_orientation);
	scale( m_matrix, m_scale);
}

void NodeBase :: Rotate( real x, real y, real z, real w)
{
	m_orientation *= Quaternion( x, y, z, w);
	m_matrix.Identity();
	translate( m_matrix, m_position);
	rotate( m_matrix, m_orientation);
	scale( m_matrix, m_scale);
}

void NodeBase :: Rotate( const Quaternion & p_orientation)
{
	Rotate( p_orientation[0], p_orientation[1], p_orientation[2], p_orientation[3]);
}

void NodeBase :: Rotate	( const Point3r & p_axis, const Angle & p_angle)
{
	Rotate( p_axis[0], p_axis[1], p_axis[2], p_angle.Radians());
}

void NodeBase :: Rotate	( real * p_quat)
{
	Rotate( p_quat[0], p_quat[1], p_quat[2], p_quat[3]);
}

void NodeBase :: Translate( real x, real y, real z)
{
	m_position[0] += x;
	m_position[1] += y;
	m_position[2] += z;
	m_matrix.Identity();
	translate( m_matrix, m_position);
	rotate( m_matrix, m_orientation);
	scale( m_matrix, m_scale);
}

void NodeBase :: Translate( const Point3r & p_position)
{
	Translate( p_position[0], p_position[1], p_position[2]);
}

void NodeBase :: Translate( real * p_pCoords)
{
	Translate( p_pCoords[0], p_pCoords[1], p_pCoords[2]);
}

void NodeBase :: Scale( real x, real y, real z)
{
	m_scale[0] += x;
	m_scale[1] += y;
	m_scale[2] += z;
	m_matrix.Identity();
	translate( m_matrix, m_position);
	rotate( m_matrix, m_orientation);
	scale( m_matrix, m_scale);
}

void NodeBase :: Scale( const Point3r & p_scale)
{
	Scale( p_scale[0], p_scale[1], p_scale[2]);
}

void NodeBase :: Scale( real * p_pCoords)
{
	Scale( p_pCoords[0], p_pCoords[1], p_pCoords[2]);
}

void NodeBase :: SetOrientation	( real x, real y, real z, real w)
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

void NodeBase :: SetOrientation( const Quaternion & p_orientation)
{
	SetOrientation( p_orientation[0], p_orientation[1], p_orientation[2], p_orientation[3]);
}

void NodeBase :: SetOrientation	( const Point3r & p_axis, const Angle & p_angle)
{
	SetOrientation( p_axis[0], p_axis[1], p_axis[2], p_angle.Radians());
}

void NodeBase :: SetOrientation	( real * p_quat)
{
	SetOrientation( p_quat[0], p_quat[1], p_quat[2], p_quat[3]);
}

void NodeBase :: SetPosition( real x, real y, real z)
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

void NodeBase :: SetPosition( const Point3r & p_position)
{
	SetPosition( p_position[0], p_position[1], p_position[2]);
}

void NodeBase :: SetPosition( real * p_pCoords)
{
	SetPosition( p_pCoords[0], p_pCoords[1], p_pCoords[2]);
}

void NodeBase :: SetScale( real x, real y, real z)
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

void NodeBase :: SetScale( const Point3r & p_scale)
{
	SetScale( p_scale[0], p_scale[1], p_scale[2]);
}

void NodeBase :: SetScale( real * p_pCoords)
{
	SetScale( p_pCoords[0], p_pCoords[1], p_pCoords[2]);
}

bool NodeBase :: Write( Castor::Utils::File & p_pFile)const
{
	bool l_bReturn = false;
	Logger::LogMessage( CU_T( "Writing Node %s"), m_name.c_str());
	l_bReturn = WriteOne( p_pFile);

	if (l_bReturn)
	{
		Logger::LogMessage( CU_T( "Writing Childs"));
		C3DMap( String, NodeBase *)::const_iterator l_it = m_childs.begin();

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

bool NodeBase :: WriteOne( Castor::Utils::File & p_pFile)const
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

void NodeBase :: Render( eDRAW_TYPE p_displayMode)
{
	if (m_visible && m_displayable)
	{
		Pipeline::PushMatrix();
		Pipeline::MultMatrix( m_matrix);

		for (C3DMap( String, MovableObject *)::iterator l_it = m_attachedObjects.begin() ; l_it != m_attachedObjects.end() ; ++l_it)
		{
			l_it->second->Render( p_displayMode);
		}

		for (C3DMap( String, NodeBase *)::iterator l_it = m_childs.begin() ; l_it != m_childs.end() ; ++l_it)
		{
			l_it->second->Render( p_displayMode);
		}

		Pipeline::PopMatrix();
	}
}