#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/scene/SceneNode.h"

#include "Castor3D/main/MovableObject.h"
#include "Castor3D/camera/Ray.h"
#include "Castor3D/main/Pipeline.h"
#include "Castor3D/geometry/basic/Vertex.h"
#include "Castor3D/geometry/primitives/Geometry.h"
#include "Castor3D/scene/Scene.h"
#include "Castor3D/scene/SceneFileParser.h"

using namespace Castor3D;

unsigned long long SceneNode::Count = 0;

SceneNode :: SceneNode( Scene * p_pScene, const String & p_name)
	:	m_strName( p_name)
	,	m_bVisible( true)
	,	m_ptScale( 1.0, 1.0, 1.0)
	,	m_ptPosition( 0, 0, 0)
	,	m_pParent( NULL)
	,	m_bDisplayable( p_name == "RootNode" || p_name == "RootNode" || p_name == "RootNode")
	,	m_pScene( p_pScene)
{
	m_qOrientation.ToRotationMatrix( m_mtxMatrix);

	if (m_strName.empty())
	{
		m_strName = CU_T( "SceneNode_%d");
		m_strName << Count;
	}

	Count++;
}

SceneNode :: ~SceneNode()
{
	if (m_pParent != NULL)
	{
		m_pParent->DetachChild( this);
	}

	if (m_mapChilds.size() > 0)
	{
		SceneNodePtrStrMap::iterator l_it = m_mapChilds.begin();

		while (l_it != m_mapChilds.end())
		{
			l_it->second->Detach();
			l_it = m_mapChilds.begin();
		}

		m_mapChilds.clear();
	}

	Count--;
}

void SceneNode :: AttachObject( MovableObject * p_pObject)
{
	if (p_pObject != NULL)
	{
		m_mapAttachedObjects[p_pObject->GetName()] = p_pObject;
		p_pObject->AttachTo( this);
	}
}

void SceneNode :: DetachObject( MovableObject * p_pObject)
{
	if (p_pObject != NULL)
	{
		MovableObjectPtrStrMap::iterator l_it = m_mapAttachedObjects.find( p_pObject->GetName());

		if (l_it != m_mapAttachedObjects.end())
		{
			m_mapAttachedObjects.erase( l_it);
			p_pObject->AttachTo( NULL);
		}
	}
}

void SceneNode :: AttachTo( SceneNode * p_parent)
{
	if (m_pParent != NULL)
	{
		m_pParent->DetachChild( this);
	}

	m_pParent = p_parent;

	if (m_pParent != NULL)
	{
		m_bDisplayable = m_pParent->m_bDisplayable;

		m_pParent->AddChild( this);
		m_qDerivedOrientation = m_pParent->GetDerivedOrientation() * m_qOrientation;
		m_ptDerivedPosition = m_pParent->GetDerivedPosition() + m_ptPosition;
		m_ptDerivedScale = m_pParent->GetDerivedScale() * m_ptScale;
	}
}

void SceneNode :: Detach()
{
	if (m_pParent != NULL)
	{
		m_bDisplayable = false;
		SceneNode * l_parent = m_pParent;
		m_pParent = NULL;
		l_parent->DetachChild( this);
	}
}

bool SceneNode :: HasChild( const String & p_name)
{
	bool l_bFound = false;

	if (m_mapChilds.find( p_name) == m_mapChilds.end())
	{
		SceneNodePtrStrMap::iterator l_it = m_mapChilds.begin();

		while (l_it != m_mapChilds.end() && ! l_bFound)
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

	if (m_mapChilds.find( l_name) == m_mapChilds.end())
	{
		m_mapChilds.insert( std::pair <String, SceneNode *>( l_name, p_child));
	}
	else
	{
//		Logger::LogMessage( m_strName + CU_T( " - Can't add SceneNode ") + l_name + CU_T( " - Already in childs"));
	}
}

void SceneNode :: DetachChild( SceneNode * p_child)
{
	String l_name = p_child->GetName();
	SceneNodePtrStrMap::iterator l_it = m_mapChilds.find( l_name);

	if (l_it != m_mapChilds.end())
	{
		SceneNode * l_current = l_it->second;
		m_mapChilds.erase( l_it);
		l_current->Detach();
	}
	else
	{
// 		Logger::LogMessage( m_strName + CU_T( " - Can't remove SceneNode ") + l_name + CU_T( " - Not in childs"));
	}
}

void SceneNode :: DetachChild( const String & p_childName)
{
	SceneNodePtrStrMap::iterator l_it = m_mapChilds.find( p_childName);

	if (l_it != m_mapChilds.end())
	{
		SceneNode * l_current = l_it->second;
		m_mapChilds.erase( l_it);
		l_current->Detach();
	}
	else
	{
		Logger::LogMessage( m_strName + CU_T( " - Can't remove SceneNode ") + p_childName + CU_T( " - Not in childs"));
	}
}

void SceneNode :: DetachAllChilds()
{
	SceneNodePtrStrMap::iterator l_it = m_mapChilds.begin();
	SceneNode * l_current;

	while (l_it != m_mapChilds.end())
	{
		l_current = l_it->second;
		m_mapChilds.erase( l_it);
		l_current->Detach();
		l_it = m_mapChilds.begin();
	}

	m_mapChilds.clear();
}

void SceneNode :: Yaw( const Angle & p_angle)
{
	Quaternion l_tmp( Point3r( 0.0, 1.0, 0.0), p_angle);
	m_qOrientation *= l_tmp;
	m_mtxMatrix.Identity();
	MtxUtils::translate( m_mtxMatrix, m_ptPosition);
	MtxUtils::rotate( m_mtxMatrix, m_qOrientation);
	MtxUtils::scale( m_mtxMatrix, m_ptScale);
}

void SceneNode :: Pitch( const Angle & p_angle)
{
	Quaternion l_tmp( Point3r( 1.0, 0.0, 0.0), p_angle);
	m_qOrientation *= l_tmp;
	m_mtxMatrix.Identity();
	MtxUtils::translate( m_mtxMatrix, m_ptPosition);
	MtxUtils::rotate( m_mtxMatrix, m_qOrientation);
	MtxUtils::scale( m_mtxMatrix, m_ptScale);
}

void SceneNode :: Roll( const Angle & p_angle)
{
	Quaternion l_tmp( Point3r( 0.0, 0.0, 1.0), p_angle);
	m_qOrientation *= l_tmp;
	m_mtxMatrix.Identity();
	MtxUtils::translate( m_mtxMatrix, m_ptPosition);
	MtxUtils::rotate( m_mtxMatrix, m_qOrientation);
	MtxUtils::scale( m_mtxMatrix, m_ptScale);
}

void SceneNode :: Rotate( real x, real y, real z, real w)
{
	m_qOrientation *= Quaternion( x, y, z, w);
	m_mtxMatrix.Identity();
	MtxUtils::translate( m_mtxMatrix, m_ptPosition);
	MtxUtils::rotate( m_mtxMatrix, m_qOrientation);
	MtxUtils::scale( m_mtxMatrix, m_ptScale);
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
	m_ptPosition[0] += x;
	m_ptPosition[1] += y;
	m_ptPosition[2] += z;
	m_mtxMatrix.Identity();
	MtxUtils::translate( m_mtxMatrix, m_ptPosition);
	MtxUtils::rotate( m_mtxMatrix, m_qOrientation);
	MtxUtils::scale( m_mtxMatrix, m_ptScale);
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
	m_ptScale[0] += x;
	m_ptScale[1] += y;
	m_ptScale[2] += z;
	m_mtxMatrix.Identity();
	MtxUtils::translate( m_mtxMatrix, m_ptPosition);
	MtxUtils::rotate( m_mtxMatrix, m_qOrientation);
	MtxUtils::scale( m_mtxMatrix, m_ptScale);
}

void SceneNode :: Scale( const Point3r & p_scale)
{
	Scale( p_scale[0], p_scale[1], p_scale[2]);
}

void SceneNode :: Scale( real * p_pCoords)
{
	Scale( p_pCoords[0], p_pCoords[1], p_pCoords[2]);
}

void SceneNode :: SetOrientation( real x, real y, real z, real w)
{
	m_qOrientation = Quaternion( x, y, z, w);

	if (m_pParent != NULL)
	{
		m_qDerivedOrientation = m_pParent->GetDerivedOrientation() * m_qOrientation;
	}

	m_mtxMatrix.Identity();
	MtxUtils::translate( m_mtxMatrix, m_ptPosition);
	MtxUtils::rotate( m_mtxMatrix, m_qOrientation);
	MtxUtils::scale( m_mtxMatrix, m_ptScale);
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
	m_ptPosition = Point3r( x, y, z);

	if (m_pParent != NULL)
	{
		m_ptDerivedPosition = m_pParent->GetDerivedPosition() + m_ptPosition;
	}

	m_mtxMatrix.Identity();
	MtxUtils::translate( m_mtxMatrix, m_ptPosition);
	MtxUtils::rotate( m_mtxMatrix, m_qOrientation);
	MtxUtils::scale( m_mtxMatrix, m_ptScale);
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
	m_ptScale = Point3r( x, y, z);

	if (m_pParent != NULL)
	{
		m_ptDerivedScale = m_pParent->GetDerivedScale() + m_ptScale;
	}

	m_mtxMatrix.Identity();
	MtxUtils::translate( m_mtxMatrix, m_ptPosition);
	MtxUtils::rotate( m_mtxMatrix, m_qOrientation);
	MtxUtils::scale( m_mtxMatrix, m_ptScale);
}

void SceneNode :: SetScale( const Point3r & p_scale)
{
	SetScale( p_scale[0], p_scale[1], p_scale[2]);
}

void SceneNode :: SetScale( real * p_pCoords)
{
	SetScale( p_pCoords[0], p_pCoords[1], p_pCoords[2]);
}

void SceneNode :: Render( ePRIMITIVE_TYPE p_displayMode)
{
	if (m_bVisible && m_bDisplayable)
	{
		Pipeline::PushMatrix();
		Pipeline::MultMatrix( m_mtxMatrix);

		for (MovableObjectPtrStrMap::iterator l_it = m_mapAttachedObjects.begin() ; l_it != m_mapAttachedObjects.end() ; ++l_it)
		{
			if (l_it->second->GetType() == MovableObject::eGeometry)
			{
				l_it->second->Render( p_displayMode);
			}
		}

		for (SceneNodePtrStrMap::iterator l_it = m_mapChilds.begin() ; l_it != m_mapChilds.end() ; ++l_it)
		{
			l_it->second->Render( p_displayMode);
		}

		Pipeline::PopMatrix();
	}
}

void SceneNode :: EndRender()
{
	if (m_bVisible && m_bDisplayable)
	{
		for (MovableObjectPtrStrMap::iterator l_it = m_mapAttachedObjects.begin() ; l_it != m_mapAttachedObjects.end() ; ++l_it)
		{
			l_it->second->EndRender();
		}

		for (SceneNodePtrStrMap::iterator l_it = m_mapChilds.begin() ; l_it != m_mapChilds.end() ; ++l_it)
		{
			l_it->second->EndRender();
		}
	}
}

void SceneNode :: CreateList( eNORMALS_MODE p_nm, bool p_showNormals, size_t & p_nbFaces, size_t & p_nbVertex)const
{
	for (MovableObjectPtrStrMap::const_iterator l_it = m_mapAttachedObjects.begin() ; l_it != m_mapAttachedObjects.end() ; ++l_it)
	{
		if (l_it->second->GetType() == MovableObject::eGeometry)
		{
			((Geometry *)l_it->second)->CreateBuffers( p_nm, p_nbFaces, p_nbVertex);
		}
	}

	for (SceneNodePtrStrMap::const_iterator l_it = m_mapChilds.begin() ; l_it != m_mapChilds.end() ; ++l_it)
	{
		l_it->second->CreateList( p_nm, p_showNormals, p_nbFaces, p_nbVertex);
	}
}

Geometry * SceneNode :: GetNearestGeometry( Ray * p_pRay, real & p_fDistance, FacePtr * p_ppFace, SubmeshPtr * p_ppSubmesh)
{
	Geometry * l_pReturn = NULL;
	real l_fDistance;

	for (MovableObjectPtrStrMap::iterator l_it = m_mapAttachedObjects.begin() ; l_it != m_mapAttachedObjects.end() ; ++l_it)
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

	for (SceneNodePtrStrMap::iterator l_it = m_mapChilds.begin() ; l_it != m_mapChilds.end() ; ++l_it)
	{
		if ((l_pTmp = l_it->second->GetNearestGeometry( p_pRay, p_fDistance, p_ppFace, p_ppSubmesh)) != NULL)
		{
			l_pReturn = l_pTmp;
		}
	}

	return l_pReturn;
}

bool SceneNode :: Write( File & p_file)const
{
	bool l_bReturn = false;
	Logger::LogMessage( CU_T( "Writing Node %s"), m_strName.c_str());
	l_bReturn = _writeOne( p_file);

	if (l_bReturn)
	{
		Logger::LogMessage( CU_T( "Writing Childs"));

		for (SceneNodePtrStrMap::const_iterator l_it = m_mapChilds.begin() ; l_it != m_mapChilds.end() && l_bReturn ; ++l_it)
		{
			l_bReturn = l_it->second->Write( p_file);
		}
	}

	if (l_bReturn)
	{
		Logger::LogMessage( CU_T( "Childs Written"));
	}

	return l_bReturn;
}

bool SceneNode :: Save( File & p_file)const
{
	bool l_bReturn = true;

	if (m_strName != "RootNode")
	{
		l_bReturn = p_file.Write( m_strName);

		if (l_bReturn)
		{
			l_bReturn = m_qOrientation.Save( p_file);
		}

		if (l_bReturn)
		{
			l_bReturn = m_ptPosition.Save( p_file);
		}

		if (l_bReturn)
		{
			l_bReturn = m_ptScale.Save( p_file);
		}
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Write( m_mapChilds.size()) == sizeof( size_t);
	}

	if (l_bReturn)
	{
		for (SceneNodePtrStrMap::const_iterator l_it = m_mapChilds.begin() ; l_it != m_mapChilds.end() && l_bReturn ; ++l_it)
		{
			l_bReturn = l_it->second->Save( p_file);
		}
	}

	return l_bReturn;
}

bool SceneNode :: Load( File & p_file)
{
	bool l_bReturn = true;

	if (m_strName != "RootNode")
	{
		if (l_bReturn)
		{
			l_bReturn = m_qOrientation.Load( p_file);
		}

		if (l_bReturn)
		{
			l_bReturn = m_ptPosition.Load( p_file);
		}

		if (l_bReturn)
		{
			l_bReturn = m_ptScale.Load( p_file);
		}
	}

	size_t l_uiSize;

	if (l_bReturn)
	{
		l_bReturn = p_file.Read( l_uiSize) == sizeof( size_t);
	}

	if (l_bReturn)
	{
		SceneNodePtr l_pChild;
		String l_strName;

		for (size_t i = 0 ; i < l_uiSize && l_bReturn ; i++)
		{
			l_bReturn = p_file.Read( l_strName);

			if (l_bReturn)
			{
				l_pChild = m_pScene->CreateSceneNode( l_strName, this);
				l_bReturn = l_pChild != NULL;
			}

			if (l_bReturn)
			{
				l_bReturn = l_pChild->Load( p_file);
			}

			if (l_bReturn)
			{
				m_mapChilds.insert( SceneNodePtrStrMap::value_type( l_strName, l_pChild.get()));
			}
		}
	}

	return l_bReturn;
}

bool SceneNode :: _writeOne( File & p_file)const
{
	bool l_bReturn = true;

	if (m_strName != "RootNode")
	{
		l_bReturn = p_file.WriteLine( "scene_node " + m_strName + "\n{\n") > 0;

		if (l_bReturn && m_pParent != NULL)
		{
			l_bReturn = p_file.WriteLine( "\tattach_to " + m_pParent->m_strName + "\n") > 0;
		}

		if (l_bReturn)
		{
			l_bReturn = p_file.Print( 256, "\torientation %f %f %f %f\n", m_qOrientation[0], m_qOrientation[1], m_qOrientation[2], m_qOrientation[3]) > 0;
		}

		if (l_bReturn)
		{
			l_bReturn = p_file.Print( 256, "\tposition %f %f %f\n", m_ptPosition[0], m_ptPosition[1], m_ptPosition[2]) > 0;
		}

		if (l_bReturn)
		{
			l_bReturn = p_file.Print( 256, "\tscale %f %f %f\n", m_ptScale[0], m_ptScale[1], m_ptScale[2]) > 0;
		}

		if (l_bReturn)
		{
			l_bReturn = p_file.WriteLine( "}\n") > 0;
		}
	}

	return l_bReturn;
}