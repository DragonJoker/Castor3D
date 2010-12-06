#include "PrecompiledHeader.h"

#include "animation/Module_Animation.h"

#include "geometry/Module_Geometry.h"
#include "scene/Module_Scene.h"
#include "animation/AnimatedObjectGroup.h"
#include "animation/AnimatedObject.h"
#include "animation/Animation.h"
#include "main/MovableObject.h"

using namespace Castor3D;

AnimatedObjectGroup :: AnimatedObjectGroup( const String & p_name)
	:	Managed<String, AnimatedObjectGroup>( p_name)
{
}

AnimatedObjectGroup :: ~AnimatedObjectGroup()
{
	m_objects.clear();
	m_playingAnimations.clear();
	m_animations.clear();
}

AnimatedObjectPtr AnimatedObjectGroup :: AddObject( MovableObjectPtr p_object)
{
	AnimatedObjectPtr l_pReturn;

	if (m_objects.find( p_object->GetName()) == m_objects.end())
	{
		l_pReturn = AnimatedObjectPtr( new AnimatedObject( p_object, m_animations));
		m_objects.insert( AnimatedObjectPtrStrMap::value_type( p_object->GetName(), l_pReturn));
	}

	return l_pReturn;
}

void AnimatedObjectGroup :: AddAnimation( AnimationPtr p_animation)
{
	if (m_animations.find( p_animation->GetName()) == m_animations.end())
	{
		m_animations.insert( AnimationPtrStrMap::value_type( p_animation->GetName(), p_animation));
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

void AnimatedObjectGroup :: Update( real p_tslf)
{
	AnimationPtrStrMap::const_iterator l_endit = m_animations.end();

	for (AnimationPtrStrMap::iterator l_it = m_animations.begin() ; l_it != l_endit ; ++l_it)
	{
		l_it->second->Update( p_tslf);
	}
}

void AnimatedObjectGroup :: StartAnimation( const String & p_name)
{
	if (m_animations.find( p_name) != m_animations.end())
	{
		m_animations.find( p_name)->second->Play();
	}
}

void AnimatedObjectGroup :: StopAnimation( const String & p_name)
{
	if (m_animations.find( p_name) != m_animations.end())
	{
		m_animations.find( p_name)->second->Stop();
	}
}

void AnimatedObjectGroup :: PauseAnimation(  const String & p_name)
{
	if (m_animations.find( p_name) != m_animations.end())
	{
		m_animations.find( p_name)->second->Pause();
	}
}

void AnimatedObjectGroup :: StartAllAnimations()
{
	for (AnimationPtrStrMap::iterator l_it = m_animations.begin() ; l_it != m_animations.end() ; ++l_it)
	{
		l_it->second->Play();
	}
}

void AnimatedObjectGroup :: StopAllAnimations()
{
	for (AnimationPtrStrMap::iterator l_it = m_animations.begin() ; l_it != m_animations.end() ; ++l_it)
	{
		l_it->second->Stop();
	}
}

void AnimatedObjectGroup :: PauseAllAnimations()
{
	for (AnimationPtrStrMap::iterator l_it = m_animations.begin() ; l_it != m_animations.end() ; ++l_it)
	{
		l_it->second->Pause();
	}
}