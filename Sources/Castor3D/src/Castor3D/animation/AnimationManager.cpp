#include "PrecompiledHeader.h"

#include "animation/Module_Animation.h"
#include "geometry/Module_Geometry.h"
#include "scene/Module_Scene.h"
#include "animation/AnimationManager.h"

using namespace Castor3D;

AnimationManager :: AnimationManager()
{
	m_timer.Time();
}

AnimationManager :: ~AnimationManager()
{
	Clear();
}

void AnimationManager :: Update()
{
	AnimationManager & l_pThis = GetSingleton();
	l_pThis.m_timeSinceLastFrame = l_pThis.m_timer.Time();

	for (C3DMap( String, AnimatedObjectGroupPtr)::iterator l_it = l_pThis.m_objectMap.begin() ; l_it != l_pThis.m_objectMap.end() ; ++l_it)
	{
		l_it->second->Update( static_cast <real>( l_pThis.m_timeSinceLastFrame));
	}

//	map::cycle( m_objectMap, & AnimatedObjectGroup::Update,
//				static_cast <real>( m_timeSinceLastFrame));
}

AnimatedObjectGroupPtr AnimationManager :: CreateAnimatedObjectGroup( const String & p_name)
{
	AnimationManager & l_pThis = GetSingleton();
	AnimatedObjectGroupPtr l_pReturn( new AnimatedObjectGroup( p_name));

	if ( ! l_pThis.AddElement( l_pReturn))
	{
		Logger::LogMessage( CU_T( "Can't add AnimatedObjectGroup %s"), p_name.char_str());
		l_pReturn.reset();
	}
	else
	{
		Logger::LogMessage( CU_T( "AnimatedObjectGroup %s added"), p_name.char_str());
	}

	return l_pReturn;
}

bool AnimationManager::Write( File & p_file)
{
	AnimationManager & l_pThis = GetSingleton();
	size_t l_nbMaterials = l_pThis.m_objectMap.size();
	bool l_bReturn = (p_file.Write( l_nbMaterials) == sizeof( size_t));

	if (l_bReturn)
	{
		TypeMap::const_iterator l_it = l_pThis.m_objectMap.begin();

		while (l_it != l_pThis.m_objectMap.end() && l_bReturn)
		{
			l_bReturn = l_it->second->Write( p_file);
			++l_it;
		}
	}

	return l_bReturn;
}

bool AnimationManager::Read( File & p_file)
{
	AnimationManager & l_pThis = GetSingleton();
	size_t l_nbMaterials = 0;
	bool l_bReturn = (p_file.Read( l_nbMaterials) == sizeof( size_t));

	if (l_bReturn)
	{
		for (size_t i = 0 ; i < l_nbMaterials && l_bReturn ; i++)
		{
			AnimatedObjectGroupPtr l_group( new AnimatedObjectGroup( C3DEmptyString));

			if ( ! l_group.null())
			{
				l_bReturn = l_group->Read( p_file);

				if ( ! l_pThis.AddElement( l_group))
				{
					l_group.reset();
				}
			}
		}
	}

	return l_bReturn;
}
