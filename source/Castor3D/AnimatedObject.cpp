#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/AnimatedObject.hpp"
#include "Castor3D/AnimatedObjectGroup.hpp"
#include "Castor3D/MovableObject.hpp"
#include "Castor3D/Scene.hpp"
#include "Castor3D/Geometry.hpp"

using namespace Castor3D;

//*************************************************************************************************

AnimatedObject :: AnimatedObject()
	:	m_pMapAnimations( NULL)
	,	m_pObject( MovableObjectPtr())
	,	m_pScene( NULL)
{
}

AnimatedObject :: AnimatedObject( Scene * p_pScene, MovableObjectPtr p_pObject, AnimationPtrStrMap * p_pMapAnimations)
	:	m_pMapAnimations( p_pMapAnimations)
	,	m_pObject( p_pObject)
	,	m_pScene( p_pScene)
	,	m_strObjName( p_pObject->GetName())
{
}

AnimatedObject :: AnimatedObject( Scene * p_pScene, AnimationPtrStrMap * p_pMapAnimations)
	:	m_pMapAnimations( p_pMapAnimations)
	,	m_pScene( p_pScene)
{
}

AnimatedObject :: ~AnimatedObject()
{
}

void AnimatedObject :: SetObject( MovableObjectPtr p_pObject)
{
	m_pObject = p_pObject;

	if (m_pObject.use_count() > 0)
	{
		m_strObjName = m_pObject->GetName();
	}
	else
	{
		m_strObjName = cuEmptyString;
	}
}

BEGIN_SERIALISE_MAP( AnimatedObject, Serialisable)
	ADD_ELEMENT( m_strObjName)
END_SERIALISE_MAP()

BEGIN_POST_UNSERIALISE( AnimatedObject, Serialisable)
	m_pScene = static_cast<AnimatedObjectGroup *>( m_pParentSerialisable)->GetScene();
	m_pObject = m_pScene->GetGeometry( m_strObjName);
END_POST_UNSERIALISE()
