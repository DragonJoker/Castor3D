#include "PrecompiledHeader.h"

#include "animation/Module_Animation.h"


#include "geometry/Module_Geometry.h"
#include "scene/Module_Scene.h"

#include "animation/Animation.h"
#include "animation/KeyFrame.h"

using namespace Castor3D;


Animation :: Animation( const String & p_name)
	:	m_name				( p_name),
		m_length			( 0.0),
		m_lastKeyFrameIt	( m_keyFrames.end()),
		m_currentTime		( 0.0),
		m_state				( AnimationStopped),
		m_weight			( 1.0),
		m_scale				( 1.0),
		m_looped			( false)
{
}



Animation :: ~Animation()
{
	map::deleteAll( m_keyFrames);
}



bool Animation :: Write( FileIO & p_file)const
{
	return true;
}



bool Animation :: Read( FileIO & p_file)
{
	return true;
}



void Animation :: Update( float p_tslf)
{
	if (m_state != AnimationPlaying)
	{
		return;
	}

	m_currentTime += (p_tslf * m_scale);
	while (m_lastKeyFrameIt != m_keyFrames.end()
		   && m_lastKeyFrameIt->second->GetTo() < m_currentTime)
	{
		m_lastKeyFrameIt->second->Update( m_lastKeyFrameIt->second->GetTo(), m_weight);
		m_lastKeyFrameIt++;
	}

	if (m_lastKeyFrameIt != m_keyFrames.end())
	{
		m_lastKeyFrameIt->second->Update( m_currentTime, m_weight);
	}
	else if (m_looped)
	{
		m_currentTime = 0.0;
		m_lastKeyFrameIt = m_keyFrames.begin();
		m_lastKeyFrameIt->second->Update( m_currentTime, m_weight);
	}
	else
	{
		Stop();
	}
}



KeyFrame * Animation :: AddKeyFrame( float p_from, float p_to)
{
	if ( ! map::has( m_keyFrames, p_from))
	{
		if (p_to > m_length)
		{
			m_length = p_to;
		}
		KeyFrame * l_keyframe = new KeyFrame( p_from, p_to);
		m_keyFrames.insert( KeyFrameMap::value_type( p_from, l_keyframe));
		m_lastKeyFrameIt = m_keyFrames.begin();
		return l_keyframe;
	}
	return NULL;
}



void Animation :: RemoveKeyFrame( float p_time)
{
	map::deleteValue( m_keyFrames, p_time);
}



void Animation :: Play()
{
	m_state = AnimationPlaying;
}



void Animation :: Pause()
{
	if (m_state == AnimationPlaying)
	{
		m_state = AnimationPaused;
	}
}



void Animation :: Stop()
{
	if (m_state != AnimationStopped)
	{
		m_state = AnimationStopped;
		m_currentTime = 0.0;
		m_lastKeyFrameIt = m_keyFrames.begin();
	}
}


