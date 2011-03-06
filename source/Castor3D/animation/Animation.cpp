#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/animation/Animation.h"
#include "Castor3D/animation/KeyFrame.h"

using namespace Castor3D;

Animation :: Animation( Scene * p_pScene, const String & p_name)
	:	m_strName			( p_name)
	,	m_rLength			( 0.0)
	,	m_itLastKeyFrame	( m_mapKeyFrames.end())
	,	m_rCurrentTime		( 0.0)
	,	m_eState			( eAnimationStopped)
	,	m_rWeight			( 1.0)
	,	m_rScale			( 1.0)
	,	m_bLooped			( false)
	,	m_pScene			( p_pScene)
{
}

Animation :: ~Animation()
{
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

		l_pReturn = KeyFramePtr( new KeyFrame( m_pScene, p_rFrom, p_rTo));
		m_mapKeyFrames.insert( KeyFramePtrRealMap::value_type( p_rFrom, l_pReturn));
		m_itLastKeyFrame = m_mapKeyFrames.begin();
	}

	return l_pReturn;
}

KeyFramePtr Animation :: AddKeyFrame( const KeyFrame & p_keyFrame)
{
	KeyFramePtr l_pReturn;

	if ( ! map::has( m_mapKeyFrames, p_keyFrame.GetFrom()))
	{
		if (p_keyFrame.GetTo() > m_rLength)
		{
			m_rLength = p_keyFrame.GetTo();
		}

		l_pReturn = KeyFramePtr( new KeyFrame( p_keyFrame));
		m_mapKeyFrames.insert( KeyFramePtrRealMap::value_type( p_keyFrame.GetFrom(), l_pReturn));
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

bool Animation :: Save( File & p_file)const
{
	bool l_bReturn = p_file.Write( m_strName);

	if (l_bReturn)
	{
		l_bReturn = p_file.Write( m_rLength) == sizeof( real);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Write( m_rWeight) == sizeof( real);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Write( m_rScale) == sizeof( real);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Write( m_bLooped) == sizeof( real);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Write( m_mapKeyFrames.size()) == sizeof( size_t);
	}

	for (KeyFramePtrRealMap::const_iterator l_it = m_mapKeyFrames.begin() ; l_it != m_mapKeyFrames.end() && l_bReturn ; ++l_it)
	{
		l_bReturn = l_it->second->Save( p_file);
	}

	return true;
}

bool Animation :: Load( File & p_file)
{
	size_t l_uiSize;
	bool l_bReturn = p_file.Read( m_strName);

	if (l_bReturn)
	{
		l_bReturn = p_file.Read( m_rLength) == sizeof( real);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Read( m_rWeight) == sizeof( real);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Read( m_rScale) == sizeof( real);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Read( m_bLooped) == sizeof( real);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Read( l_uiSize) == sizeof( size_t);
	}

	for (size_t i = 0 ; i < l_uiSize && l_bReturn ; i++)
	{
		KeyFrame l_keyFrame( m_pScene);
		l_bReturn = l_keyFrame.Load( p_file);

		if (l_bReturn)
		{
			AddKeyFrame( l_keyFrame.GetFrom(), l_keyFrame.GetTo());
		}
	}

	return true;
}