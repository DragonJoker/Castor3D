#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/animation/AnimatedObject.h"
#include "Castor3D/main/MovableObject.h"
#include "Castor3D/scene/Scene.h"
#include "Castor3D/geometry/primitives/Geometry.h"

using namespace Castor3D;

AnimatedObject :: AnimatedObject( Scene * p_pScene, MovableObjectPtr p_pObject,
								  AnimationPtrStrMap & p_mapAnimations)
	:	m_mapAnimations( p_mapAnimations)
	,	m_pObject( p_pObject)
	,	m_pScene( p_pScene)
{
}

AnimatedObject :: AnimatedObject( Scene * p_pScene, AnimationPtrStrMap & p_mapAnimations)
	:	m_mapAnimations( p_mapAnimations)
	,	m_pScene( p_pScene)
{
}

AnimatedObject :: ~AnimatedObject()
{
}

bool AnimatedObject :: Save( File & p_file)const
{
	bool l_bReturn = p_file.Write( m_pObject->GetName());

	return l_bReturn;
}

bool AnimatedObject :: Load( File & p_file)
{
	String l_strName;
	bool l_bReturn = p_file.Read( l_strName);

	if (l_bReturn)
	{
		m_pObject=  static_pointer_cast<MovableObject>( m_pScene->GetGeometry( l_strName));
		l_bReturn = m_pObject != NULL;
	}

	return l_bReturn;
}
