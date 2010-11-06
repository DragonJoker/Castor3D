#include "PrecompiledHeader.h"
#include "scene/SceneNode.h"

#include "geometry/primitives/Geometry.h"
#include "geometry/primitives/MovableObject.h"
#include "camera/Ray.h"

using namespace Castor3D;

int SceneNode :: s_nbSceneNodes = 0;

SceneNode :: SceneNode( const String & p_name)
	:	m_name( p_name),
		m_visible( true),
		m_scale( Point3r( 1.0, 1.0, 1.0)),
		m_matrix( new real[16]),
		m_parent( NULL),
		m_displayable( p_name == "RootNode")
{
	m_orientation.ToRotationMatrix( m_matrix);

	Vertex::s_vertexNumber--;

	if (m_name.empty())
	{
		m_name = CU_T( "SceneNode_%d");
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

	C3DMap( String, SceneNode *)::iterator l_it = m_childs.begin();

	while (l_it != m_childs.end())
	{
		l_it->second->Detach();
		l_it++;
	}

	m_childs.clear();
	delete [] m_matrix;
	Vertex::s_vertexNumber++;
}

void SceneNode :: AttachGeometry( MovableObject * p_geometry)
{
	if (p_geometry != NULL)
	{
		m_attachedGeometries[p_geometry->GetName()] = p_geometry;
		p_geometry->SetParent( this);
	}
}

void SceneNode :: DetachGeometry( MovableObject * p_geometry)
{
	if (p_geometry != NULL)
	{
		C3DMap( String, MovableObject *)::iterator l_it = m_attachedGeometries.find( p_geometry->GetName());

		if (l_it != m_attachedGeometries.end())
		{
			m_attachedGeometries.erase( l_it);
			p_geometry->SetParent( NULL);
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
		C3DMap( String, SceneNode *)::iterator l_it = m_childs.begin();

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
		Log::LogMessage( CU_T( "Can't add SceneNode %s - Already in childs"), l_name.c_str());
	}
}

void SceneNode :: DetachChild( SceneNode * p_child)
{
	String l_name = p_child->GetName();
	C3DMap( String, SceneNode *)::iterator l_it = m_childs.find( l_name);

	if (l_it != m_childs.end())
	{
		SceneNode * l_current = l_it->second;
		m_childs.erase( l_it);
		l_current->Detach();
	}
	else
	{
		Log::LogMessage( CU_T( "Can't remove SceneNode %s - Not in childs"), l_name.c_str());
	}
}

void SceneNode :: DetachChild( const String & p_childName)
{
	C3DMap( String, SceneNode *)::iterator l_it = m_childs.find( p_childName);

	if (l_it != m_childs.end())
	{
		SceneNode * l_current = l_it->second;
		m_childs.erase( l_it);
		l_current->Detach();
	}
	else
	{
		Log::LogMessage( CU_T( "Can't remove SceneNode %s - Not in childs"), p_childName.c_str());
	}
}

void SceneNode :: DetachAllChilds()
{
	C3DMap( String, SceneNode *)::iterator l_it = m_childs.begin();
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

void SceneNode :: Yaw( real p_angle)
{
	Quaternion l_tmp( Point3r( 0.0, 1.0, 0.0), p_angle);
	m_orientation *= l_tmp;
	m_orientation.ToRotationMatrix( m_matrix);
}

void SceneNode :: Pitch( real p_angle)
{
	Quaternion l_tmp( Point3r( 1.0, 0.0, 0.0), p_angle);
	m_orientation *= l_tmp;
	m_orientation.ToRotationMatrix( m_matrix);
}

void SceneNode :: Roll( real p_angle)
{
	Quaternion l_tmp( Point3r( 0.0, 0.0, 1.0), p_angle);
	m_orientation *= l_tmp;
	m_orientation.ToRotationMatrix( m_matrix);
}

void SceneNode :: Rotate( const Quaternion & p_quat)
{
	m_orientation *= p_quat;
	m_orientation.ToRotationMatrix( m_matrix);
}

void SceneNode :: Translate( const Point3r & p_t)
{
	m_position += p_t;
}

void SceneNode :: CreateList( NormalsMode p_nm, bool p_showNormals,
							  size_t & p_nbFaces, size_t & p_nbVertex)const
{
	C3DMap( String, MovableObject *)::const_iterator l_it = m_attachedGeometries.begin();

	while (l_it != m_attachedGeometries.end())
	{
		((Geometry *)l_it->second)->CreateBuffers( p_nm, p_nbFaces, p_nbVertex);
		++l_it;
	}

	C3DMap( String, SceneNode *)::const_iterator l_it2 = m_childs.begin();

	while (l_it2 != m_childs.end())
	{
		l_it2->second->CreateList( p_nm, p_showNormals, p_nbFaces, p_nbVertex);
		++l_it2;
	}
}

void SceneNode :: Apply( eDRAW_TYPE p_displayMode)
{
	if (m_visible && m_displayable)
	{
		m_pRenderer->ApplyTransformations();

		C3DMap( String, MovableObject *)::iterator l_geometriesIt = m_attachedGeometries.begin();

		while (l_geometriesIt != m_attachedGeometries.end())
		{
			((Geometry *) l_geometriesIt->second)->Render( p_displayMode);
			++l_geometriesIt;
		}

		C3DMap( String, SceneNode *)::iterator l_childsIt = m_childs.begin();

		while (l_childsIt != m_childs.end())
		{
			l_childsIt->second->Apply( p_displayMode);
			++l_childsIt;
		}

		m_pRenderer->RemoveTransformations();

	}
}

bool SceneNode :: Write( Castor::Utils::File & p_pFile)const
{
	bool l_bReturn = false;
	Log::LogMessage( CU_T( "Writing Node %s"), m_name.c_str());
	l_bReturn = WriteOne( p_pFile);

	if (l_bReturn)
	{
		Log::LogMessage( CU_T( "Writing Childs"));
		C3DMap( String, SceneNode *)::const_iterator l_it = m_childs.begin();

		while (l_it != m_childs.end() && l_bReturn)
		{
			l_bReturn = l_it->second->Write( p_pFile);
			++l_it;
		}
	}

	if (l_bReturn)
	{
		Log::LogMessage( CU_T( "Childs Written"));
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

real * SceneNode :: Get4x4RotationMatrix()
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

void SceneNode :: SetPosition( const Point3r & p_position)
{
	m_position = p_position;

	if (m_parent != NULL)
	{
		m_derivedPosition = m_parent->GetDerivedPosition() + m_position;
	}
}

void SceneNode :: SetPosition( real x, real y, real z)
{
	m_position = Point3r( x, y, z);

	if (m_parent != NULL)
	{
		m_derivedPosition = m_parent->GetDerivedPosition() + m_position;
	}
}

void SceneNode :: SetScale( const Point3r & p_scale)
{
	m_scale = p_scale;

	if (m_parent != NULL)
	{
		m_derivedScale = m_parent->GetDerivedScale() + m_scale;
	}
}

void SceneNode :: SetScale( real x, real y, real z)
{
	m_scale = Point3r( x, y, z);

	if (m_parent != NULL)
	{
		m_derivedScale = m_parent->GetDerivedScale() + m_scale;
	}
}

GeometryPtr SceneNode :: GetNearestGeometry( Ray * p_pRay, real & p_fDistance, FacePtr* p_ppFace, SubmeshPtr* p_ppSubmesh)
{
	GeometryPtr l_pReturn;

	C3DMap( String, MovableObject *)::iterator l_geometriesIt = m_attachedGeometries.begin();
	real l_fDistance;

	while (l_geometriesIt != m_attachedGeometries.end())
	{
		if ((l_fDistance = p_pRay->Intersects( (GeometryPtr)l_geometriesIt->second, p_ppFace, p_ppSubmesh)) >= 0.0 && l_fDistance < p_fDistance)
		{
			p_fDistance = l_fDistance;
			l_pReturn = (GeometryPtr)l_geometriesIt->second;
		}

		++l_geometriesIt;
	}

	C3DMap( String, SceneNode *)::iterator l_childsIt = m_childs.begin();
	GeometryPtr l_pTmp;

	while (l_childsIt != m_childs.end())
	{
		if ( ! (l_pTmp = l_childsIt->second->GetNearestGeometry( p_pRay, p_fDistance, p_ppFace, p_ppSubmesh)).null())
		{
			l_pReturn = l_pTmp;
		}

		++l_childsIt;
	}

	return l_pReturn;
}