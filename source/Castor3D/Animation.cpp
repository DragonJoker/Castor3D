#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Animation.hpp"
#include "Castor3D/KeyFrame.hpp"
#include "Castor3D/AnimatedObjectGroup.hpp"

using namespace Castor3D;

//*************************************************************************************************

Animation :: Animation()
	:	m_rLength			( 0.0)
	,	m_itLastKeyFrame	( m_mapKeyFrames.end())
	,	m_rCurrentTime		( 0.0)
	,	m_eState			( eSTATE_STOPPED)
	,	m_rWeight			( 1.0)
	,	m_rScale			( 1.0)
	,	m_bLooped			( false)
	,	m_pScene			( NULL)
{
}

Animation :: Animation( Scene * p_pScene, String const & p_name)
	:	m_strName			( p_name)
	,	m_rLength			( 0.0)
	,	m_itLastKeyFrame	( m_mapKeyFrames.end())
	,	m_rCurrentTime		( 0.0)
	,	m_eState			( eSTATE_STOPPED)
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
	CASTOR_TRACK;
	if (m_eState == eSTATE_PLAYING)
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
	CASTOR_TRACK;
	m_eState = eSTATE_PLAYING;
}

void Animation :: Pause()
{
	CASTOR_TRACK;
	if (m_eState == eSTATE_PLAYING)
	{
		m_eState = eSTATE_PAUSED;
	}
}

void Animation :: Stop()
{
	CASTOR_TRACK;
	if (m_eState != eSTATE_STOPPED)
	{
		m_eState = eSTATE_STOPPED;
		m_rCurrentTime = 0.0;
		m_itLastKeyFrame = m_mapKeyFrames.begin();
	}
}

BEGIN_SERIALISE_MAP( Animation, Serialisable)
	ADD_ELEMENT( m_strName)
	ADD_ELEMENT( m_rLength)
	ADD_ELEMENT( m_rWeight)
	ADD_ELEMENT( m_rScale)
	ADD_ELEMENT( m_bLooped)
	ADD_ELEMENT( m_arrayKeyFrames)
END_SERIALISE_MAP()

BEGIN_PRE_SERIALISE( Animation, Serialisable)
	for (KeyFramePtrRealMap::const_iterator l_it = m_mapKeyFrames.begin() ; l_it != m_mapKeyFrames.end() ; ++l_it)
	{
		m_arrayKeyFrames.push_back( l_it->second);
	}
END_PRE_SERIALISE()

BEGIN_POST_SERIALISE( Animation, Serialisable)
	m_arrayKeyFrames.clear();
END_POST_SERIALISE()

BEGIN_PRE_UNSERIALISE( Animation, Serialisable)
	m_pScene = static_cast<AnimatedObjectGroup *>( m_pParentSerialisable)->GetScene();
END_PRE_UNSERIALISE()

BEGIN_POST_UNSERIALISE( Animation, Serialisable)
	for (size_t i = 0 ; i < m_arrayKeyFrames.size() ; i++)
	{
		AddKeyFrame( m_arrayKeyFrames[i]->GetFrom(), m_arrayKeyFrames[i]->GetTo());
	}
	m_arrayKeyFrames.clear();
END_POST_UNSERIALISE()

//*************************************************************************************************