#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/animation/AnimatedObject.h"
#include "Castor3D/main/MovableObject.h"

using namespace Castor3D;

AnimatedObject :: AnimatedObject( MovableObjectPtr p_pObject,
								  AnimationPtrStrMap & p_mapAnimations)
	:	m_mapAnimations( p_mapAnimations),
		m_pObject( p_pObject)
{
}

AnimatedObject :: ~AnimatedObject()
{
}

bool AnimatedObject :: Write( File & p_file)const
{
	return true;
}

bool AnimatedObject :: Read( File & p_file)
{
	return true;
}