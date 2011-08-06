#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/AnimatedObjectGroup.hpp"
#include "Castor3D/AnimatedObject.hpp"
#include "Castor3D/Animation.hpp"
#include "Castor3D/MovableObject.hpp"
#include "Castor3D/Geometry.hpp"
#include "Castor3D/Scene.hpp"

using namespace Castor3D;

//*************************************************************************************************

Scene * Loader<AnimatedObjectGroup> :: m_pScene;

bool Loader<AnimatedObjectGroup> :: Load( AnimatedObjectGroup & p_group, File & p_file, Scene * p_pScene)
{
	m_pScene = p_pScene;
	return Load( p_group, p_file);
}

bool Loader<AnimatedObjectGroup> :: Write( const AnimatedObjectGroup & p_group, File & p_file)
{
	bool l_bReturn = p_file.WriteLine( cuT( "animated_object_group ") + p_group.GetName() + cuT( "\n{\n")) > 0;

	for (AnimationPtrStrMap::const_iterator l_it = p_group.AnimationsBegin() ; l_it != p_group.AnimationsEnd() && l_bReturn ; ++l_it)
	{
		l_bReturn = p_file.WriteLine( cuT( "\tanimation ") + l_it->second->GetName() + cuT( "\n")) > 0;
	}

	for (AnimatedObjectPtrStrMap::const_iterator l_it = p_group.ObjectsBegin() ; l_it != p_group.ObjectsEnd() && l_bReturn ; ++l_it)
	{
		l_bReturn = p_file.WriteLine( cuT( "\tanimated_object ") + l_it->second->GetObject()->GetName() + cuT( "\n")) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteLine( cuT( "}\n\n")) > 0;
	}

	return l_bReturn;
}

bool Loader<AnimatedObjectGroup> :: Load( AnimatedObjectGroup & p_group, File & p_file)
{
	Collection<AnimatedObjectGroup, String> l_collection;
	bool l_bResult = p_group.Unserialise( p_file);

	if ( ! l_bResult || l_collection.HasElement( p_group.GetName()))
	{
		Logger::LogMessage( cuT( "Can't add AnimatedObjectGroup [%s]"), p_group.GetName().c_str());
	}
	else
	{
		Logger::LogMessage( cuT( "AnimatedObjectGroup [%s] added"), p_group.GetName().c_str());
	}

	return l_bResult;
}

//*************************************************************************************************

AnimatedObjectGroup :: AnimatedObjectGroup()
	:	m_pScene( NULL)
{
	m_timer.Time();
}

AnimatedObjectGroup :: AnimatedObjectGroup( Scene * p_pScene, String const & p_strName)
	:	m_strName( p_strName)
	,	m_pScene( p_pScene)
{
	m_timer.Time();
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

	if (p_pObject && m_mapObjects.find( p_pObject->GetName()) == m_mapObjects.end())
	{
		l_pReturn = AnimatedObjectPtr( new AnimatedObject( m_pScene, p_pObject, & m_mapAnimations));
		m_mapObjects.insert( AnimatedObjectPtrStrMap::value_type( p_pObject->GetName(), l_pReturn));
	}

	return l_pReturn;
}

bool AnimatedObjectGroup :: AddObject( AnimatedObjectPtr p_pObject)
{
	bool l_bReturn = p_pObject != NULL && p_pObject->GetObject() != NULL && m_mapObjects.find( p_pObject->GetObject()->GetName()) == m_mapObjects.end();

	if (l_bReturn)
	{
		m_mapObjects.insert( AnimatedObjectPtrStrMap::value_type( p_pObject->GetObject()->GetName(), p_pObject));
	}

	return l_bReturn;
}

void AnimatedObjectGroup :: AddAnimation( AnimationPtr p_pAnimation)
{
	if (p_pAnimation && m_mapAnimations.find( p_pAnimation->GetName()) == m_mapAnimations.end())
	{
		m_mapAnimations.insert( AnimationPtrStrMap::value_type( p_pAnimation->GetName(), p_pAnimation));
	}
}

void AnimatedObjectGroup :: Update()
{
	CASTOR_TRACK;
	real l_rTslf = real( m_timer.Time());
	AnimationPtrStrMap::const_iterator l_endit = m_mapAnimations.end();

	for (AnimationPtrStrMap::iterator l_it = m_mapAnimations.begin() ; l_it != l_endit ; ++l_it)
	{
		l_it->second->Update( l_rTslf);
	}
}

void AnimatedObjectGroup :: StartAnimation( String const & p_strName)
{
	CASTOR_TRACK;
	if (m_mapAnimations.find( p_strName) != m_mapAnimations.end())
	{
		m_mapAnimations.find( p_strName)->second->Play();
	}
}

void AnimatedObjectGroup :: StopAnimation( String const & p_strName)
{
	CASTOR_TRACK;
	if (m_mapAnimations.find( p_strName) != m_mapAnimations.end())
	{
		m_mapAnimations.find( p_strName)->second->Stop();
	}
}

void AnimatedObjectGroup :: PauseAnimation(  String const & p_strName)
{
	CASTOR_TRACK;
	if (m_mapAnimations.find( p_strName) != m_mapAnimations.end())
	{
		m_mapAnimations.find( p_strName)->second->Pause();
	}
}

void AnimatedObjectGroup :: StartAllAnimations()
{
	CASTOR_TRACK;
	for (AnimationPtrStrMap::iterator l_it = m_mapAnimations.begin() ; l_it != m_mapAnimations.end() ; ++l_it)
	{
		l_it->second->Play();
	}
}

void AnimatedObjectGroup :: StopAllAnimations()
{
	CASTOR_TRACK;
	for (AnimationPtrStrMap::iterator l_it = m_mapAnimations.begin() ; l_it != m_mapAnimations.end() ; ++l_it)
	{
		l_it->second->Stop();
	}
}

void AnimatedObjectGroup :: PauseAllAnimations()
{
	CASTOR_TRACK;
	for (AnimationPtrStrMap::iterator l_it = m_mapAnimations.begin() ; l_it != m_mapAnimations.end() ; ++l_it)
	{
		l_it->second->Pause();
	}
}

BEGIN_SERIALISE_MAP( AnimatedObjectGroup, Serialisable)
	ADD_ELEMENT( m_strName)
	ADD_ELEMENT( m_mapAnimations)
	ADD_ELEMENT( m_mapObjects)
END_SERIALISE_MAP()

//*************************************************************************************************