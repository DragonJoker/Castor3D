#include "PrecompiledHeader.h"

#include "animation/Module_Animation.h"


#include "geometry/Module_Geometry.h"
#include "scene/Module_Scene.h"

#include "animation/AnimationManager.h"
#include "Log.h"

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
	m_timeSinceLastFrame = m_timer.Time();
	map::cycle( m_objectMap, & AnimatedObjectGroup::Update,
				static_cast <float>( m_timeSinceLastFrame));
}



AnimatedObjectGroup * AnimationManager :: CreateAnimatedObjectGroup( const String & p_name)
{
	AnimatedObjectGroup * l_group = new AnimatedObjectGroup( p_name);

	if ( ! AddElement( l_group))
	{
		Log::LogMessage( C3D_T( "Can't add Material %s"), p_name.c_str());
		delete l_group;
		return NULL;
	}
	Log::LogMessage( C3D_T( "Material %s added"), p_name.c_str());
	return l_group;
}



bool AnimationManager::Write( File & p_file)const
{
	size_t l_nbMaterials = m_objectMap.size();
	if ( ! p_file.Write<size_t>( l_nbMaterials))
	{
		return false;
	}

	TypeMap::const_iterator l_it = m_objectMap.begin();
	while (l_it != m_objectMap.end())
	{
		if ( ! l_it->second->Write( p_file))
		{
			return false;
		}
		++l_it;
	}
	return true;
}



bool AnimationManager::Read( File & p_file)
{
	size_t l_nbMaterials = 0;
	if ( ! p_file.Read<size_t>( l_nbMaterials))
	{
		return false;
	}

	AnimatedObjectGroup * l_group;
	for (size_t i = 0 ; i < l_nbMaterials ; i++)
	{
		l_group = NULL;
		l_group = new AnimatedObjectGroup( C3DEmptyString);
		if ( ! l_group->Read( p_file))
		{
			return false;
		}
		if (l_group != NULL)
		{
			if ( ! AddElement( l_group))
			{
				delete l_group;
			}
		}
	}
	return true;
}


