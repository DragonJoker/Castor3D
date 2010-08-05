#include "PrecompiledHeader.h"

#include "animation/Module_Animation.h"


#include "geometry/Module_Geometry.h"
#include "scene/Module_Scene.h"

#include "animation/AnimatedObjectGroup.h"
#include "animation/AnimatedObject.h"
#include "animation/Animation.h"
#include "geometry/primitives/MovableObject.h"

using namespace Castor3D;

AnimatedObjectGroup :: AnimatedObjectGroup( const String & p_name)
{
}

AnimatedObjectGroup :: ~AnimatedObjectGroup()
{
	map::deleteAll( m_objects);
	map::deleteAll( m_animations);
}

AnimatedObject * AnimatedObjectGroup :: AddObject( MovableObject * p_object)
{
	if ( ! map::has( m_objects, p_object->GetName()))
	{
		AnimatedObject * l_object = new AnimatedObject( p_object, & m_animations);
		m_objects.insert( AnimatedObjectStrMap::value_type( p_object->GetName(), l_object));
	}
	return NULL;
}

void AnimatedObjectGroup :: AddAnimation( Animation * p_animation)
{
	if ( ! map::has( m_animations, p_animation->GetName()))
	{
		m_animations.insert( AnimationStrMap::value_type( p_animation->GetName(), p_animation));
	}
}

bool AnimatedObjectGroup :: Write( File & p_file)const
{
	return true;
}

bool AnimatedObjectGroup :: Read( File & p_file)
{
	return true;
}

void AnimatedObjectGroup :: Update( float p_tslf)
{
	AnimationStrMap::const_iterator l_endit = m_animations.end();
	for (AnimationStrMap::iterator l_it = m_animations.begin() ; l_it != l_endit ; ++l_it)
	{
		l_it->second->Update( p_tslf);
	}
}

void AnimatedObjectGroup :: StartAnimation( const String & p_name)
{
	Animation * l_anim = map::findOrNull( m_animations, p_name);
	if (l_anim == NULL)
	{
		return;
	}
	l_anim->Play();
}

void AnimatedObjectGroup :: StopAnimation( const String & p_name)
{
	Animation * l_anim = map::findOrNull( m_animations, p_name);
	if (l_anim == NULL)
	{
		return;
	}
	l_anim->Stop();
}

void AnimatedObjectGroup :: PauseAnimation(  const String & p_name)
{
	Animation * l_anim = map::findOrNull( m_animations, p_name);
	if (l_anim == NULL)
	{
		return;
	}
	l_anim->Pause();
}

void AnimatedObjectGroup :: StartAllAnimations()
{
	map::cycle( m_animations, & Animation::Play);
}

void AnimatedObjectGroup :: StopAllAnimations()
{
	map::cycle( m_animations, & Animation::Stop);
}

void AnimatedObjectGroup :: PauseAllAnimations()
{
	map::cycle( m_animations, & Animation::Pause);
}