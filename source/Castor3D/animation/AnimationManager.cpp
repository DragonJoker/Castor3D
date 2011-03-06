#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/animation/AnimationManager.h"
#include "Castor3D/render_system/RenderSystem.h"
#include "Castor3D/scene/SceneFileParser.h"

using namespace Castor3D;

AnimationManager :: AnimationManager( Scene * p_pScene)
	:	m_pScene( p_pScene)
{
	m_timer.Time();
}

AnimationManager :: ~AnimationManager()
{
	Clear();
}

void AnimationManager :: Update()
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	m_dTimeSinceLastFrame = m_timer.Time();

	for (TypeMap::iterator l_it = m_objectMap.begin() ; l_it != m_objectMap.end() ; ++l_it)
	{
		l_it->second->Update( static_cast <real>( m_dTimeSinceLastFrame));
	}

//	map::cycle( m_objectMap, & AnimatedObjectGroup::Update,
//				static_cast <real>( m_timeSinceLastFrame));
}

AnimatedObjectGroupPtr AnimationManager :: CreateAnimatedObjectGroup( const String & p_strName)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	AnimatedObjectGroupPtr l_pReturn( new AnimatedObjectGroup( m_pScene, this, p_strName));

	if ( ! AddElement( l_pReturn))
	{
		Logger::LogMessage( CU_T( "Can't add AnimatedObjectGroup %s"), p_strName.char_str());
		l_pReturn.reset();
	}
	else
	{
		Logger::LogMessage( CU_T( "AnimatedObjectGroup %s added"), p_strName.char_str());
	}

	return l_pReturn;
}

bool AnimationManager :: Save( File & p_file)const
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	size_t l_nbMaterials = m_objectMap.size();
	bool l_bReturn = (p_file.Write( l_nbMaterials) == sizeof( size_t));

	if (l_bReturn)
	{
		TypeMap::const_iterator l_it = m_objectMap.begin();

		while (l_it != m_objectMap.end() && l_bReturn)
		{
			l_bReturn = l_it->second->Save( p_file);
			++l_it;
		}
	}

	return l_bReturn;
}

bool AnimationManager :: Load( File & p_file)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	size_t l_nbAnimated = 0;
	bool l_bReturn = (p_file.Read( l_nbAnimated) == sizeof( size_t));

	if (l_bReturn)
	{
		for (size_t i = 0 ; i < l_nbAnimated && l_bReturn ; i++)
		{
			AnimatedObjectGroupPtr l_group( new AnimatedObjectGroup( m_pScene, this, C3DEmptyString));

			if (l_group != NULL)
			{
				l_bReturn = l_group->Load( p_file);

				if ( ! l_bReturn || ! AddElement( l_group))
				{
					l_group.reset();
				}
			}
		}
	}

	return l_bReturn;
}

bool AnimationManager :: Write( File & p_file)const
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	bool l_bReturn = true;

	if (l_bReturn)
	{
		TypeMap::const_iterator l_it = m_objectMap.begin();

		while (l_it != m_objectMap.end() && l_bReturn)
		{
			l_bReturn = l_it->second->Write( p_file);
			++l_it;
		}
	}

	return l_bReturn;
}

bool AnimationManager :: Read( File & p_file)
{
	SceneFileParser l_parser( RenderSystem::GetSingletonPtr()->GetSceneManager());
	return l_parser.ParseFile( p_file);
}