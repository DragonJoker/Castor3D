#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/scene/SceneNode.h"
#include "Castor3D/animation/KeyFrame.h"
#include "Castor3D/main/MovableObject.h"
#include "Castor3D/geometry/primitives/Geometry.h"
#include "Castor3D/scene/Scene.h"

using namespace Castor3D;

//*************************************************************************************************

MovingObject :: MovingObject( Scene * p_pScene)
	:	m_pScene( p_pScene)
{
}

MovingObject :: MovingObject( MovableObjectPtr p_pObject,
							  const Point3r & p_ptTranslate, const Point3r & p_ptScale,
							  const Quaternion & p_qRotate)
	:	m_ptTranslate( p_ptTranslate)
	,	m_ptScale( p_ptScale)
	,	m_qRotate( p_qRotate)
	,	m_pObject( p_pObject)
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

bool MovingObject :: Save( File & p_file)const
{
	bool l_bReturn = p_file.Write( m_pObject->GetName().size()) == sizeof( size_t);

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteArray<xchar>( m_pObject->GetName().c_str(), m_pObject->GetName().size()) == sizeof( xchar) * m_pObject->GetName().size();
	}

	if (l_bReturn)
	{
		l_bReturn = m_ptTranslate.Save( p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = m_ptScale.Save( p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = m_qRotate.Save( p_file);
	}

	return l_bReturn;
}

bool MovingObject :: Load( File & p_file)
{
	size_t l_uiSize;
	bool l_bReturn = p_file.Read( l_uiSize) == sizeof( size_t);

	if (l_bReturn)
	{
		xchar * l_pTmp = new xchar[l_uiSize + 1];
		memset( l_pTmp, 0, l_uiSize);
		l_bReturn = p_file.ReadArray<xchar>( l_pTmp, l_uiSize) == sizeof( xchar) * l_uiSize;

		if (l_bReturn)
		{
			m_pObject=  static_pointer_cast<MovableObject>( m_pScene->GetGeometry( l_pTmp));
			l_bReturn = m_pObject != NULL;
		}

		delete [] l_pTmp;
	}

	if (l_bReturn)
	{
		l_bReturn = m_ptTranslate.Load( p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = m_ptScale.Load( p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = m_qRotate.Load( p_file);
	}

	return l_bReturn;
}

//*************************************************************************************************

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
	real l_rPercent = (p_rTime - m_rFrom) / (m_rTo - m_rFrom);
	MovingObjectPtrStrMap::const_iterator l_endIt = m_mapToMove.end();

	for (MovingObjectPtrStrMap::iterator l_it = m_mapToMove.begin() ; l_it != l_endIt ; ++l_it)
	{
		l_it->second->Update( l_rPercent, p_rWeight);
	}
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

bool KeyFrame :: Save( File & p_file)const
{
	bool l_bReturn = p_file.Write( m_mapToMove.size()) == sizeof( size_t);

	for (MovingObjectPtrStrMap::const_iterator l_it = m_mapToMove.begin() ; l_it != m_mapToMove.end() && l_bReturn ; ++l_it)
	{
		l_bReturn = l_it->second->Save( p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Write( m_rTo) == sizeof( real);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Write( m_rFrom) == sizeof( real);
	}

	return l_bReturn;
}

bool KeyFrame :: Load( File & p_file)
{
	size_t l_uiSize;
	bool l_bReturn = p_file.Read( l_uiSize) == sizeof( size_t);

	for (size_t i = 0 ; i < l_uiSize && l_bReturn ; i++)
	{
		MovingObjectPtr l_pObject( new MovingObject( m_pScene));
		l_bReturn = l_pObject->Load( p_file);

		if (l_bReturn)
		{
			m_mapToMove.insert( MovingObjectPtrStrMap::value_type( l_pObject->GetObject()->GetName(), l_pObject));
		}
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Read( m_rTo) == sizeof( real);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Read( m_rFrom) == sizeof( real);
	}

	return l_bReturn;
}

//*************************************************************************************************