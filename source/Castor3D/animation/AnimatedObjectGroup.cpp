#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/animation/AnimatedObjectGroup.h"
#include "Castor3D/animation/AnimatedObject.h"
#include "Castor3D/animation/Animation.h"
#include "Castor3D/main/MovableObject.h"
#include "Castor3D/geometry/primitives/Geometry.h"
#include "Castor3D/scene/Scene.h"

using namespace Castor3D;

AnimatedObjectGroup :: AnimatedObjectGroup( Scene * p_pScene, Manager<AnimatedObjectGroup> * p_pManager, const String & p_strName)
	:	m_strName( p_strName)
	,	m_pScene( p_pScene)
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

	if (p_pObject != NULL && m_mapObjects.find( p_pObject->GetName()) == m_mapObjects.end())
	{
		l_pReturn = AnimatedObjectPtr( new AnimatedObject( m_pScene, p_pObject, m_mapAnimations));
		m_mapObjects.insert( AnimatedObjectPtrStrMap::value_type( p_pObject->GetName(), l_pReturn));
	}

	return l_pReturn;
}

void AnimatedObjectGroup :: AddAnimation( AnimationPtr p_pAnimation)
{
	if (p_pAnimation != NULL && m_mapAnimations.find( p_pAnimation->GetName()) == m_mapAnimations.end())
	{
		m_mapAnimations.insert( AnimationPtrStrMap::value_type( p_pAnimation->GetName(), p_pAnimation));
	}
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

bool AnimatedObjectGroup :: Write( File & p_file)const
{
	bool l_bReturn = p_file.WriteLine( CU_T( "animated_object_group ") + m_strName + CU_T( "\n{\n")) > 0;

	for (AnimationPtrStrMap::const_iterator l_it = m_mapAnimations.begin() ; l_it != m_mapAnimations.end() && l_bReturn ; ++l_it)
	{
		l_bReturn = p_file.WriteLine( CU_T( "\tanimation ") + l_it->second->GetName() + CU_T( "\n")) > 0;
	}

	for (AnimatedObjectPtrStrMap::const_iterator l_it = m_mapObjects.begin() ; l_it != m_mapObjects.end() && l_bReturn ; ++l_it)
	{
		l_bReturn = p_file.WriteLine( CU_T( "\tanimated_object ") + l_it->second->GetObject()->GetName() + CU_T( "\n")) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteLine( CU_T( "}\n\n")) > 0;
	}

	return l_bReturn;
}

bool AnimatedObjectGroup :: Save( File & p_file)const
{
	bool l_bReturn = p_file.Write( m_strName);

	if (l_bReturn)
	{
		l_bReturn = p_file.Write( m_mapAnimations.size()) == sizeof( size_t);
	}

	for (AnimationPtrStrMap::const_iterator l_it = m_mapAnimations.begin() ; l_it != m_mapAnimations.end() && l_bReturn ; ++l_it)
	{
		l_bReturn = l_it->second->Save( p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Write( m_mapObjects.size()) == sizeof( size_t);
	}

	for (AnimatedObjectPtrStrMap::const_iterator l_it = m_mapObjects.begin() ; l_it != m_mapObjects.end() && l_bReturn ; ++l_it)
	{
		l_bReturn = l_it->second->Save( p_file);
	}

	return l_bReturn;
}

bool AnimatedObjectGroup :: Load( File & p_file)
{
	size_t l_uiSize;
	bool l_bReturn = p_file.Read( m_strName);

	if (l_bReturn)
	{
		l_bReturn = p_file.Read( l_uiSize) == sizeof( size_t);
	}

	for (size_t i = 0 ; i < l_uiSize && l_bReturn ; i++)
	{
		AnimationPtr l_pAnimation( new Animation( m_pScene));
		l_bReturn = l_pAnimation->Load( p_file);

		if (l_bReturn)
		{
			m_mapAnimations.insert( AnimationPtrStrMap::value_type( l_pAnimation->GetName(), l_pAnimation));
		}
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Read( l_uiSize) == sizeof( size_t);
	}

	for (size_t i = 0 ; i < l_uiSize && l_bReturn ; i++)
	{
		AnimatedObjectPtr l_pObject( new AnimatedObject( m_pScene, m_mapAnimations));
		l_bReturn = l_pObject->Load( p_file);

		if (l_bReturn)
		{
			m_mapObjects.insert( AnimatedObjectPtrStrMap::value_type( l_pObject->GetObject()->GetName(), l_pObject));
		}
	}

	return l_bReturn;
}