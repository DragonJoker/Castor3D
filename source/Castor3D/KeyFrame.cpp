#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/SceneNode.hpp"
#include "Castor3D/KeyFrame.hpp"
#include "Castor3D/MovableObject.hpp"
#include "Castor3D/Geometry.hpp"
#include "Castor3D/Scene.hpp"
#include "Castor3D/Animation.hpp"

using namespace Castor3D;

//*************************************************************************************************

MovingObject :: MovingObject()
	:	m_pScene( NULL)
{
}

MovingObject :: MovingObject( Scene * p_pScene)
	:	m_pScene( p_pScene)
{
}

MovingObject :: MovingObject( Scene * p_pScene, MovableObjectPtr p_pObject,
							  Point3r const & p_ptTranslate, Point3r const & p_ptScale,
							  Quaternion const & p_qRotate)
	:	m_pScene( p_pScene)
	,	m_qRotate( p_qRotate)
	,	m_pObject( p_pObject)
	,	m_strObjName( p_pObject->GetName())
{
	m_ptTranslate.copy( p_ptTranslate);
	m_ptScale.copy( p_ptScale);
}

MovingObject :: ~MovingObject()
{
}

void MovingObject :: Update( real p_rPercent, real p_rWeight)
{
	CASTOR_TRACK;
	m_pObject->GetParent()->SetOrientation( (m_pObject->GetParent()->GetOrientation() * m_qRotate.Slerp( Quaternion::Identity, 1.0f - p_rPercent, true)) * p_rWeight);
	m_pObject->GetParent()->SetPosition( m_pObject->GetParent()->GetPosition() + (m_ptTranslate * p_rPercent));
}

BEGIN_SERIALISE_MAP( MovingObject, Serialisable)
	ADD_ELEMENT( m_strObjName)
	ADD_ELEMENT( m_ptTranslate)
	ADD_ELEMENT( m_ptScale)
	ADD_ELEMENT( m_qRotate)
END_SERIALISE_MAP()

BEGIN_POST_UNSERIALISE( MovingObject, Serialisable)
	m_pScene = static_cast<KeyFrame *>( m_pParentSerialisable)->GetScene();
	m_pObject = m_pScene->GetGeometry( m_strObjName);
END_POST_UNSERIALISE()

//*************************************************************************************************

KeyFrame :: KeyFrame()
	:	m_rTo	( 0)
	,	m_rFrom	( 0)
	,	m_pScene( NULL)
{
}

KeyFrame :: KeyFrame( Scene * p_pScene, real p_rFrom, real p_rTo)
	:	m_rTo	( p_rTo)
	,	m_rFrom	( p_rFrom)
	,	m_pScene( p_pScene)
{
}

KeyFrame :: KeyFrame( const KeyFrame & p_keyFrame)
	:	m_rTo	( p_keyFrame.m_rTo)
	,	m_rFrom	( p_keyFrame.m_rFrom)
	,	m_pScene( p_keyFrame.m_pScene)
{
	for (MovingObjectPtrStrMap::const_iterator l_it = p_keyFrame.m_mapToMove.begin() ; l_it != p_keyFrame.m_mapToMove.end() ; ++l_it)
	{
		AddMovingObject( l_it->second->GetObject(), l_it->second->GetTranslate(), l_it->second->GetScale(), l_it->second->GetRotate());
	}
}

KeyFrame :: ~KeyFrame()
{
}

void KeyFrame :: Update( real p_rTime, real p_rWeight)
{
	CASTOR_TRACK;
	real l_rPercent = (p_rTime - m_rFrom) / (m_rTo - m_rFrom);
	MovingObjectPtrStrMap::const_iterator l_endIt = m_mapToMove.end();

	for (MovingObjectPtrStrMap::iterator l_it = m_mapToMove.begin() ; l_it != l_endIt ; ++l_it)
	{
		l_it->second->Update( l_rPercent, p_rWeight);
	}
}

void KeyFrame :: AddMovingObject( MovableObjectPtr p_pObject, Point3r const & p_ptTranslate, Point3r const & p_ptScale, Quaternion const & p_qRotate)
{
	if ( ! map::has( m_mapToMove, p_pObject->GetName()))
	{
		MovingObjectPtr l_pObject( new MovingObject( m_pScene, p_pObject, p_ptTranslate,
													p_ptScale, p_qRotate));
		m_mapToMove.insert( MovingObjectPtrStrMap::value_type( p_pObject->GetName(), l_pObject));
	}
}

void KeyFrame :: AddMovingObject( MovingObjectPtr p_pObject)
{
	if (p_pObject && p_pObject->GetObject()&&  ! map::has( m_mapToMove, p_pObject->GetObject()->GetName()))
	{
		m_mapToMove.insert( MovingObjectPtrStrMap::value_type( p_pObject->GetObject()->GetName(), p_pObject));
	}
}

void KeyFrame :: RemoveMovingObject( String const & p_strName)
{
	MovingObjectPtr l_pMovable;
	map::eraseValue( m_mapToMove, p_strName, l_pMovable);
	l_pMovable.reset();
}

BEGIN_SERIALISE_MAP( KeyFrame, Serialisable)
	ADD_ELEMENT( m_mapToMove)
	ADD_ELEMENT( m_rTo)
	ADD_ELEMENT( m_rFrom)
END_SERIALISE_MAP()

BEGIN_PRE_UNSERIALISE( KeyFrame, Serialisable)
	m_pScene = static_cast<Animation *>( m_pParentSerialisable)->GetScene();
END_PRE_UNSERIALISE()

//*************************************************************************************************
