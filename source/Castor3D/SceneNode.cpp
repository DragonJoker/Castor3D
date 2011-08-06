#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/SceneNode.hpp"

#include "Castor3D/MovableObject.hpp"
#include "Castor3D/Ray.hpp"
#include "Castor3D/Pipeline.hpp"
#include "Castor3D/Vertex.hpp"
#include "Castor3D/Geometry.hpp"
#include "Castor3D/Scene.hpp"
#include "Castor3D/SceneFileParser.hpp"

using namespace Castor3D;
using namespace Castor::Resources;

//*************************************************************************************************

bool Loader<SceneNode> :: Write( const SceneNode & p_node, File & p_file)
{
	bool l_bReturn = false;
	Logger::LogMessage( cuT( "Writing Node ") + p_node.GetName());

	if (p_node.GetName() != cuT( "RootNode"))
	{
		l_bReturn = p_file.WriteLine( cuT( "scene_node ") + p_node.GetName() + cuT( "\n{\n")) > 0;

		if (l_bReturn && p_node.GetParent())
		{
			l_bReturn = p_file.WriteLine( cuT( "\tattach_to ") + p_node.GetParent()->GetName() + cuT( "\n")) > 0;
		}

		if (l_bReturn)
		{
			l_bReturn = p_file.Print( 256, cuT( "\torientation ")) > 0;
			Loader<Quaternion>::Write( p_node.GetOrientation(), p_file);
		}

		if (l_bReturn)
		{
			l_bReturn = p_file.Print( 256, cuT( "\n\tposition ")) > 0;
			Loader<Point3r>::Write( p_node.GetPosition(), p_file);
		}

		if (l_bReturn)
		{
			l_bReturn = p_file.Print( 256, cuT( "\n\tscale ")) > 0;
			Loader<Point3r>::Write( p_node.GetScale(), p_file);
		}

		if (l_bReturn)
		{
			l_bReturn = p_file.WriteLine( cuT( "\n}\n")) > 0;
		}
	}

	if (l_bReturn)
	{
		Logger::LogMessage( cuT( "Writing Childs"));

		for (SceneNode::node_const_iterator l_it = p_node.ChildsBegin() ; l_it != p_node.ChildsEnd() && l_bReturn ; ++l_it)
		{
			l_bReturn = Loader<SceneNode>::Write( * l_it->second, p_file);
		}
	}

	if (l_bReturn)
	{
		Logger::LogMessage( cuT( "Childs Written"));
	}

	return l_bReturn;
}

//*************************************************************************************************

unsigned long long SceneNode::Count = 0;

Matrix4x4r SceneNode :: MtxTranslate;
Matrix4x4r SceneNode :: MtxRotate;
Matrix4x4r SceneNode :: MtxScale;

SceneNode :: SceneNode()
	:	m_bVisible( true)
	,	m_ptScale( 1.0, 1.0, 1.0)
	,	m_ptPosition( 0, 0, 0)
	,	m_pParent( NULL)
	,	m_bDisplayable( false)
	,	m_pScene( NULL)
	,	m_bMtxChanged( true)
{
	m_qOrientation.ToRotationMatrix( m_mtxMatrix);

	if (m_strName.empty())
	{
		m_strName = cuT( "SceneNode_%d");
		m_strName << Count;
	}

	Count++;
}

SceneNode :: SceneNode( Scene * p_pScene, String const & p_name)
	:	m_strName( p_name)
	,	m_bVisible( true)
	,	m_ptScale( 1.0, 1.0, 1.0)
	,	m_ptPosition( 0, 0, 0)
	,	m_pParent( NULL)
	,	m_bDisplayable( p_name == cuT( "RootNode"))
	,	m_pScene( p_pScene)
	,	m_bMtxChanged( true)
{
	m_qOrientation.ToRotationMatrix( m_mtxMatrix);

	if (m_strName.empty())
	{
		m_strName = cuT( "SceneNode_%d");
		m_strName << Count;
	}

	Count++;
}

SceneNode :: ~SceneNode()
{
	if (m_pParent)
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
	if (p_pObject)
	{
		m_mapAttachedObjects[p_pObject->GetName()] = p_pObject;
		p_pObject->AttachTo( this);
	}
}

void SceneNode :: DetachObject( MovableObject * p_pObject)
{
	if (p_pObject)
	{
		MovableObjectPtrStrMap::iterator l_it = m_mapAttachedObjects.find( p_pObject->GetName());

		if (l_it != m_mapAttachedObjects.end())
		{
			m_mapAttachedObjects.erase( l_it);
			p_pObject->AttachTo( nullptr);
		}
	}
}

void SceneNode :: AttachTo( SceneNode * p_parent)
{
	if (m_pParent)
	{
		m_pParent->DetachChild( this);
	}

	m_pParent = p_parent;

	if (m_pParent)
	{
		m_bDisplayable = m_pParent->m_bDisplayable;

		m_pParent->AddChild( this);
		m_qDerivedOrientation = m_pParent->GetDerivedOrientation() * m_qOrientation;
		m_ptDerivedPosition.copy( m_pParent->GetDerivedPosition() + m_ptPosition);
		m_ptDerivedScale.copy( m_pParent->GetDerivedScale() * m_ptScale);
		m_bMtxChanged = true;
	}
}

void SceneNode :: Detach()
{
	if (m_pParent)
	{
		m_bDisplayable = false;
		SceneNode * l_parent = m_pParent;
		m_pParent = nullptr;
		l_parent->DetachChild( this);
		m_bMtxChanged = true;
	}
}

bool SceneNode :: HasChild( String const & p_name)
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
//		Logger::LogMessage( m_strName + cuT( " - Can't add SceneNode ") + l_name + cuT( " - Already in childs"));
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
// 		Logger::LogMessage( m_strName + cuT( " - Can't remove SceneNode ") + l_name + cuT( " - Not in childs"));
	}
}

void SceneNode :: DetachChild( String const & p_childName)
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
		Logger::LogMessage( m_strName + cuT( " - Can't remove SceneNode ") + p_childName + cuT( " - Not in childs"));
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

void SceneNode :: Yaw( Angle const & p_angle)
{
	Quaternion l_tmp( Point3r( 0.0, 1.0, 0.0), p_angle);
	m_qOrientation *= l_tmp;
	m_bMtxChanged = true;
	m_mtxMatrix.SetIdentity();
	MtxUtils::translate( m_mtxMatrix, m_ptPosition);
	MtxUtils::rotate( m_mtxMatrix, m_qOrientation);
	MtxUtils::scale( m_mtxMatrix, m_ptScale);
}

void SceneNode :: Pitch( Angle const & p_angle)
{
	Quaternion l_tmp( Point3r( 1.0, 0.0, 0.0), p_angle);
	m_qOrientation *= l_tmp;
	m_bMtxChanged = true;
	m_mtxMatrix.SetIdentity();
	MtxUtils::translate( m_mtxMatrix, m_ptPosition);
	MtxUtils::rotate( m_mtxMatrix, m_qOrientation);
	MtxUtils::scale( m_mtxMatrix, m_ptScale);
}

void SceneNode :: Roll( Angle const & p_angle)
{
	Quaternion l_tmp( Point3r( 0.0, 0.0, 1.0), p_angle);
	m_qOrientation *= l_tmp;
	m_bMtxChanged = true;
	m_mtxMatrix.SetIdentity();
	MtxUtils::translate( m_mtxMatrix, m_ptPosition);
	MtxUtils::rotate( m_mtxMatrix, m_qOrientation);
	MtxUtils::scale( m_mtxMatrix, m_ptScale);
}

void SceneNode :: Rotate( real x, real y, real z, real w)
{
	m_qOrientation *= Quaternion( x, y, z, w);
	m_bMtxChanged = true;
	m_mtxMatrix.SetIdentity();
	MtxUtils::translate( m_mtxMatrix, m_ptPosition);
	MtxUtils::rotate( m_mtxMatrix, m_qOrientation);
	MtxUtils::scale( m_mtxMatrix, m_ptScale);
}

void SceneNode :: Rotate( Quaternion const & p_orientation)
{
	Rotate( p_orientation[0], p_orientation[1], p_orientation[2], p_orientation[3]);
}

void SceneNode :: Rotate	( Point3r const & p_axis, Angle const & p_angle)
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
	m_bMtxChanged = true;
	m_mtxMatrix.SetIdentity();
	MtxUtils::translate( m_mtxMatrix, m_ptPosition);
	MtxUtils::rotate( m_mtxMatrix, m_qOrientation);
	MtxUtils::scale( m_mtxMatrix, m_ptScale);
}

void SceneNode :: Translate( Point3r const & p_position)
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
	m_bMtxChanged = true;
	m_mtxMatrix.SetIdentity();
	MtxUtils::translate( m_mtxMatrix, m_ptPosition);
	MtxUtils::rotate( m_mtxMatrix, m_qOrientation);
	MtxUtils::scale( m_mtxMatrix, m_ptScale);
}

void SceneNode :: Scale( Point3r const & p_scale)
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

	if (m_pParent)
	{
		m_qDerivedOrientation = m_pParent->GetDerivedOrientation() * m_qOrientation;
	}

	m_bMtxChanged = true;
	m_mtxMatrix.SetIdentity();
	MtxUtils::translate( m_mtxMatrix, m_ptPosition);
	MtxUtils::rotate( m_mtxMatrix, m_qOrientation);
	MtxUtils::scale( m_mtxMatrix, m_ptScale);
}

void SceneNode :: SetOrientation( Quaternion const & p_orientation)
{
	SetOrientation( p_orientation[0], p_orientation[1], p_orientation[2], p_orientation[3]);
}

void SceneNode :: SetOrientation	( Point3r const & p_axis, Angle const & p_angle)
{
	SetOrientation( p_axis[0], p_axis[1], p_axis[2], p_angle.Radians());
}

void SceneNode :: SetOrientation	( real * p_quat)
{
	SetOrientation( p_quat[0], p_quat[1], p_quat[2], p_quat[3]);
}

void SceneNode :: SetPosition( real x, real y, real z)
{
	m_ptPosition.copy( Point3r( x, y, z));

	if (m_pParent)
	{
		m_ptDerivedPosition.copy( m_pParent->GetDerivedPosition() + m_ptPosition);
	}

	m_bMtxChanged = true;
	m_mtxMatrix.SetIdentity();
	MtxUtils::translate( m_mtxMatrix, m_ptPosition);
	MtxUtils::rotate( m_mtxMatrix, m_qOrientation);
	MtxUtils::scale( m_mtxMatrix, m_ptScale);
}

void SceneNode :: SetPosition( Point3r const & p_position)
{
	SetPosition( p_position[0], p_position[1], p_position[2]);
}

void SceneNode :: SetPosition( real * p_pCoords)
{
	SetPosition( p_pCoords[0], p_pCoords[1], p_pCoords[2]);
}

void SceneNode :: SetScale( real x, real y, real z)
{
	m_ptScale.copy( Point3r( x, y, z));

	if (m_pParent)
	{
		m_ptDerivedScale.copy( m_pParent->GetDerivedScale() + m_ptScale);
	}

	m_bMtxChanged = true;
	m_mtxMatrix.SetIdentity();
	MtxUtils::translate( m_mtxMatrix, m_ptPosition);
	MtxUtils::rotate( m_mtxMatrix, m_qOrientation);
	MtxUtils::scale( m_mtxMatrix, m_ptScale);
}

void SceneNode :: SetScale( Point3r const & p_scale)
{
	SetScale( p_scale[0], p_scale[1], p_scale[2]);
}

void SceneNode :: SetScale( real * p_pCoords)
{
	SetScale( p_pCoords[0], p_pCoords[1], p_pCoords[2]);
}

void SceneNode :: Render( ePRIMITIVE_TYPE p_displayMode)
{
	CASTOR_TRACK;
	if (m_bVisible && m_bDisplayable)
	{
		Pipeline::PushMatrix();
		Pipeline::MultMatrix( m_mtxMatrix);

		for (MovableObjectPtrStrMap::iterator l_it = m_mapAttachedObjects.begin() ; l_it != m_mapAttachedObjects.end() ; ++l_it)
		{
			if (l_it->second->GetType() == eMOVABLE_TYPE_GEOMETRY)
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
	CASTOR_TRACK;
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
		if (l_it->second->GetType() == eMOVABLE_TYPE_GEOMETRY)
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
	Geometry * l_pReturn = nullptr;
	real l_fDistance;

	for (MovableObjectPtrStrMap::iterator l_it = m_mapAttachedObjects.begin() ; l_it != m_mapAttachedObjects.end() ; ++l_it)
	{
		if (l_it->second->GetType() == eMOVABLE_TYPE_GEOMETRY)
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
		if ((l_pTmp = l_it->second->GetNearestGeometry( p_pRay, p_fDistance, p_ppFace, p_ppSubmesh)))
		{
			l_pReturn = l_pTmp;
		}
	}

	return l_pReturn;
}

void SceneNode :: _computeMatrix()
{
// 	if (m_bMtxChanged)
// 	{
// 		MtxUtils::set_rotate( MtxRotate, m_qOrientation);
// 		MtxUtils::set_scale( MtxScale, m_ptScale);
// 		MtxUtils::set_translate( MtxTranslate, m_ptPosition);
// 		m_mtxMatrix = MtxTranslate * MtxRotate * MtxScale;
// 		m_bMtxChanged = false;
// 	}
}

BEGIN_SERIALISE_MAP( SceneNode, Serialisable)
	ADD_ELEMENT(					m_strName)
	ADD_ELEMENT(					m_qOrientation)
	ADD_ELEMENT(					m_ptPosition)
	ADD_ELEMENT(					m_ptScale)
	ADD_ELEMENT(					m_mapChilds)
END_SERIALISE_MAP()