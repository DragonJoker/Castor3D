#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/scene/SceneNode.h"
#include "Castor3D/animation/KeyFrame.h"
#include "Castor3D/main/MovableObject.h"

using namespace Castor3D;

//**************************************************************************************************************************************

MovingObject :: MovingObject()
{
}

MovingObject :: MovingObject( MovableObjectPtr p_pObject,
							  const Point3r & p_ptTranslate, const Point3r & p_ptScale,
							  const Quaternion & p_qRotate)
	:	m_ptTranslate( p_ptTranslate),
		m_ptScale( p_ptScale),
		m_qRotate( p_qRotate),
		m_pObject( p_pObject)
{
}

MovingObject :: ~MovingObject()
{
}

void MovingObject :: Update( real p_rPercent, real p_rWeight)
{
	m_pObject->GetParent()->SetOrientation( (m_pObject->GetParent()->GetOrientation() * m_qRotate.Slerp( Quaternion::Identity, 1.0f - p_rPercent, true)) * p_rWeight);
	m_pObject->GetParent()->SetPosition( m_pObject->GetParent()->GetPosition() + (m_ptTranslate * p_rPercent));
}

//**************************************************************************************************************************************

KeyFrame :: KeyFrame( real p_rFrom, real p_rTo)
	:	m_rTo	( p_rTo),
		m_rFrom	( p_rFrom)
{
}

KeyFrame :: ~KeyFrame()
{
}

void KeyFrame :: Update( real p_rTime, real p_rWeight)
{
	real l_rPercent = (p_rTime - m_rFrom) / (m_rTo - m_rFrom);
	MovingObjectPtrStrMap::const_iterator l_endIt = m_mapToMove.end();

	for (MovingObjectPtrStrMap::iterator l_it = m_mapToMove.begin() ; l_it != l_endIt ; ++l_it)
	{
		l_it->second->Update( l_rPercent, p_rWeight);
	}
}

bool KeyFrame :: Write( File & p_file)const
{
	return true;
}

bool KeyFrame :: Read( File & p_file)
{
	return true;
}

void KeyFrame :: AddMovingObject( MovableObjectPtr p_pObject,
								  const Point3r & p_ptTranslate,
								  const Point3r & p_ptScale,
								  const Quaternion & p_qRotate)
{
	if ( ! map::has( m_mapToMove, p_pObject->GetName()))
	{
		MovingObjectPtr l_pObject( new MovingObject( p_pObject, p_ptTranslate,
													p_ptScale, p_qRotate));
		m_mapToMove.insert( MovingObjectPtrStrMap::value_type( p_pObject->GetName(), l_pObject));
	}
}

void KeyFrame :: RemoveMovingObject( const String & p_strName)
{
	MovingObjectPtr l_pMovable;
	map::eraseValue( m_mapToMove, p_strName, l_pMovable);
	l_pMovable.reset();
}

//**************************************************************************************************************************************