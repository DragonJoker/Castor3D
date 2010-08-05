#include "PrecompiledHeader.h"

#include "animation/Module_Animation.h"


#include "geometry/Module_Geometry.h"
#include "scene/Module_Scene.h"

#include "animation/AnimatedObject.h"
#include "geometry/primitives/MovableObject.h"

using namespace Castor3D;

AnimatedObject :: AnimatedObject( MovableObject * p_object,
								  AnimationStrMap * p_animations)
	:	m_animations( p_animations),
		m_object( p_object)
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


