#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/animation/Animation.h"
#include "Castor3D/animation/KeyFrame.h"

using namespace Castor3D;

Animation :: Animation( const String & p_name)
	:	m_strName			( p_name)
	,	m_rLength			( 0.0)
	,	m_itLastKeyFrame	( m_mapKeyFrames.end())
	,	m_rCurrentTime		( 0.0)
	,	m_eState			( eAnimationStopped)
	,	m_rWeight			( 1.0)
	,	m_rScale			( 1.0)
	,	m_bLooped			( false)
{
}

Animation :: ~Animation()
{
}

bool Animation :: Write( File & p_file)const
{
	return true;
}

bool Animation :: Read( File & p_file)
{
	return true;
}

void Animation :: Update( real p_rTslf)
{
	if (m_eState == eAnimationPlaying)
	{
		m_rCurrentTime += (p_rTslf * m_rScale);

		while (m_itLastKeyFrame != m_mapKeyFrames.end()
			   && m_itLastKeyFrame->second->GetTo() < m_rCurrentTime)
		{
			m_itLastKeyFrame->second->Update( m_itLastKeyFrame->second->GetTo(), m_rWeight);
			m_itLastKeyFrame++;
		}

		if (m_itLastKeyFrame != m_mapKeyFrames.end())
		{
			m_itLastKeyFrame->second->Update( m_rCurrentTime, m_rWeight);
		}
		else if (m_bLooped)
		{
			m_rCurrentTime = 0.0;
			m_itLastKeyFrame = m_mapKeyFrames.begin();
			m_itLastKeyFrame->second->Update( m_rCurrentTime, m_rWeight);
		}
		else
		{
			Stop();
		}
	}
}

KeyFramePtr Animation :: AddKeyFrame( real p_rFrom, real p_rTo)
{
	KeyFramePtr l_pReturn;

	if ( ! map::has( m_mapKeyFrames, p_rFrom))
	{
		if (p_rTo > m_rLength)
		{
			m_rLength = p_rTo;
		}

		l_pReturn = KeyFramePtr( new KeyFrame( p_rFrom, p_rTo));
		m_mapKeyFrames.insert( KeyFramePtrRealMap::value_type( p_rFrom, l_pReturn));
		m_itLastKeyFrame = m_mapKeyFrames.begin();
	}

	return l_pReturn;
}

void Animation :: RemoveKeyFrame( real p_rTime)
{
	KeyFramePtr l_pKeyFrame;
	map::eraseValue( m_mapKeyFrames, p_rTime, l_pKeyFrame);
	l_pKeyFrame.reset();
}

void Animation :: Play()
{
	m_eState = eAnimationPlaying;
}

void Animation :: Pause()
{
	if (m_eState == eAnimationPlaying)
	{
		m_eState = eAnimationPaused;
	}
}

void Animation :: Stop()
{
	if (m_eState != eAnimationStopped)
	{
		m_eState = eAnimationStopped;
		m_rCurrentTime = 0.0;
		m_itLastKeyFrame = m_mapKeyFrames.begin();
	}
}