#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/animation/AnimatedObjectGroup.h"
#include "Castor3D/animation/AnimatedObject.h"
#include "Castor3D/animation/Animation.h"
#include "Castor3D/main/MovableObject.h"

using namespace Castor3D;

AnimatedObjectGroup :: AnimatedObjectGroup( AnimationManager * p_pManager, const String & p_strName)
	:	Managed<String, AnimatedObjectGroup, AnimationManager>( p_pManager, p_strName)
{
}

AnimatedObjectGroup :: ~AnimatedObjectGroup()
{
	m_mapObjects.clear();
	m_mapPlayingAnimations.clear();
	m_mapAnimations.clear();
}

AnimatedObjectPtr AnimatedObjectGroup :: AddObject( MovableObjectPtr p_pObject)
{
	AnimatedObjectPtr l_pReturn;

	if (m_mapObjects.find( p_pObject->GetName()) == m_mapObjects.end())
	{
		l_pReturn = AnimatedObjectPtr( new AnimatedObject( p_pObject, m_mapAnimations));
		m_mapObjects.insert( AnimatedObjectPtrStrMap::value_type( p_pObject->GetName(), l_pReturn));
	}

	return l_pReturn;
}

void AnimatedObjectGroup :: AddAnimation( AnimationPtr p_pAnimation)
{
	if (m_mapAnimations.find( p_pAnimation->GetName()) == m_mapAnimations.end())
	{
		m_mapAnimations.insert( AnimationPtrStrMap::value_type( p_pAnimation->GetName(), p_pAnimation));
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

void AnimatedObjectGroup :: Update( real p_rTslf)
{
	AnimationPtrStrMap::const_iterator l_endit = m_mapAnimations.end();

	for (AnimationPtrStrMap::iterator l_it = m_mapAnimations.begin() ; l_it != l_endit ; ++l_it)
	{
		l_it->second->Update( p_rTslf);
	}
}

void AnimatedObjectGroup :: StartAnimation( const String & p_strName)
{
	if (m_mapAnimations.find( p_strName) != m_mapAnimations.end())
	{
		m_mapAnimations.find( p_strName)->second->Play();
	}
}

void AnimatedObjectGroup :: StopAnimation( const String & p_strName)
{
	if (m_mapAnimations.find( p_strName) != m_mapAnimations.end())
	{
		m_mapAnimations.find( p_strName)->second->Stop();
	}
}

void AnimatedObjectGroup :: PauseAnimation(  const String & p_strName)
{
	if (m_mapAnimations.find( p_strName) != m_mapAnimations.end())
	{
		m_mapAnimations.find( p_strName)->second->Pause();
	}
}

void AnimatedObjectGroup :: StartAllAnimations()
{
	for (AnimationPtrStrMap::iterator l_it = m_mapAnimations.begin() ; l_it != m_mapAnimations.end() ; ++l_it)
	{
		l_it->second->Play();
	}
}

void AnimatedObjectGroup :: StopAllAnimations()
{
	for (AnimationPtrStrMap::iterator l_it = m_mapAnimations.begin() ; l_it != m_mapAnimations.end() ; ++l_it)
	{
		l_it->second->Stop();
	}
}

void AnimatedObjectGroup :: PauseAllAnimations()
{
	for (AnimationPtrStrMap::iterator l_it = m_mapAnimations.begin() ; l_it != m_mapAnimations.end() ; ++l_it)
	{
		l_it->second->Pause();
	}
}