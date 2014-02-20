#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Animation.hpp"
#include "Castor3D/KeyFrame.hpp"
#include "Castor3D/AnimatedObjectGroup.hpp"
#include "Castor3D/MovableObject.hpp"
#include "Castor3D/SceneNode.hpp"
#include "Castor3D/Skeleton.hpp"
#include "Castor3D/Interpolation.hpp"

using namespace Castor3D;
using namespace Castor;

//*************************************************************************************************

MovingObjectBase :: MovingObjectBase( eMOVING_OBJECT_TYPE p_eType )
	:	m_rLength	( 0.0		)
	,	m_eType		( p_eType	)
{
	m_mtxFinalTransformation.set_identity();
}

MovingObjectBase :: ~MovingObjectBase()
{
}

void MovingObjectBase :: AddChild( MovingObjectBaseSPtr p_pObject )
{
	m_arrayChildren.push_back( p_pObject );
}

void MovingObjectBase :: Update( real p_rTime, bool p_bLooped, Matrix4x4r const & p_mtxTransformations )
{
	m_mtxTransformations = p_mtxTransformations * DoCompute( p_rTime );
	DoApply();

	std::for_each( m_arrayChildren.begin(), m_arrayChildren.end(), [&]( MovingObjectBaseSPtr p_pObject )
	{
		p_pObject->Update( p_rTime, p_bLooped, m_mtxTransformations );
	} );
}

ScaleKeyFrameSPtr MovingObjectBase :: AddScaleKeyFrame( real p_rFrom )
{
	ScaleKeyFrameSPtr l_pReturn = map::find_or_null( m_mapScales, p_rFrom );

	if( !l_pReturn )
	{
		if( p_rFrom > m_rLength )
		{
			m_rLength = p_rFrom;
		}

		l_pReturn = std::make_shared< ScaleKeyFrame >();
		l_pReturn->SetTimeIndex( p_rFrom );
		m_mapScales.insert( std::make_pair( p_rFrom, l_pReturn ) );
	}

	return l_pReturn;
}

TranslateKeyFrameSPtr MovingObjectBase :: AddTranslateKeyFrame( real p_rFrom )
{
	TranslateKeyFrameSPtr l_pReturn = map::find_or_null( m_mapTranslates, p_rFrom );

	if( !l_pReturn )
	{
		if( p_rFrom > m_rLength )
		{
			m_rLength = p_rFrom;
		}

		l_pReturn = std::make_shared< TranslateKeyFrame >();
		l_pReturn->SetTimeIndex( p_rFrom );
		m_mapTranslates.insert( std::make_pair( p_rFrom, l_pReturn ) );
	}

	return l_pReturn;
}

RotateKeyFrameSPtr MovingObjectBase :: AddRotateKeyFrame( real p_rFrom )
{
	RotateKeyFrameSPtr l_pReturn = map::find_or_null( m_mapRotates, p_rFrom );

	if( !l_pReturn )
	{
		if( p_rFrom > m_rLength )
		{
			m_rLength = p_rFrom;
		}

		l_pReturn = std::make_shared< RotateKeyFrame >();
		l_pReturn->SetTimeIndex( p_rFrom );
		m_mapRotates.insert( std::make_pair( p_rFrom, l_pReturn ) );
	}

	return l_pReturn;
}

void MovingObjectBase :: RemoveScaleKeyFrame( real p_rTime )
{
	ScaleKeyFrameSPtr l_pKeyFrame;
	map::erase_at( m_mapScales, p_rTime, l_pKeyFrame );
	l_pKeyFrame.reset();
}

void MovingObjectBase :: RemoveTranslateKeyFrame( real p_rTime )
{
	TranslateKeyFrameSPtr l_pKeyFrame;
	map::erase_at( m_mapTranslates, p_rTime, l_pKeyFrame );
	l_pKeyFrame.reset();
}

void MovingObjectBase :: RemoveRotateKeyFrame( real p_rTime )
{
	RotateKeyFrameSPtr l_pKeyFrame;
	map::erase_at( m_mapRotates, p_rTime, l_pKeyFrame );
	l_pKeyFrame.reset();
}

MovingObjectBaseSPtr MovingObjectBase :: Clone( MovingObjectPtrStrMap & p_map )
{
	MovingObjectBaseSPtr l_pReturn = DoClone();
	p_map.insert( std::make_pair( l_pReturn->GetName(), l_pReturn ) );
	l_pReturn->m_mtxNodeTransform = m_mtxNodeTransform;

	l_pReturn->m_arrayChildren.clear();

	std::for_each( m_arrayChildren.begin(), m_arrayChildren.end(), [&]( MovingObjectBaseSPtr p_pObject )
	{
		l_pReturn->m_arrayChildren.push_back( p_pObject->Clone( p_map ) );
	} );

	return l_pReturn;
}

Matrix4x4r MovingObjectBase :: DoCompute( real p_rTime )
{
	Matrix4x4r l_mtxReturn;

	if( HasKeyFrames() )
	{
		MtxUtils::set_transform( l_mtxReturn, DoComputeTranslation( p_rTime ), DoComputeScaling( p_rTime ), DoComputeRotation( p_rTime ) );
	}
	else
	{
		l_mtxReturn = m_mtxNodeTransform;
	}

	return l_mtxReturn;
}

Point3r MovingObjectBase :: DoComputeScaling( real p_rTime )
{
	Point3r l_ptReturn( 1.0, 1.0, 1.0 );
	ScaleKeyFramePtrRealMapIt l_itCur = m_mapScales.begin();

	while( l_itCur != m_mapScales.end() && l_itCur->second->GetTimeIndex() < p_rTime )
	{
		l_itCur++;
	}

	if( l_itCur != m_mapScales.end() )
	{
		ScaleKeyFramePtrRealMapIt l_itPrv = l_itCur;

		if( l_itPrv == m_mapScales.begin() )
		{
			l_ptReturn = l_itCur->second->GetValue();
		}
		else
		{
			--l_itPrv;
			real l_rDeltaTime = l_itCur->first - l_itPrv->first;
			real l_rFactor = (p_rTime - l_itPrv->first) / l_rDeltaTime;
			l_ptReturn = LinearPointInterpolator( l_itPrv->second->GetValue(), l_itCur->second->GetValue() ).Interpolate( l_rFactor );
		}
	}

	return l_ptReturn;
}

Point3r MovingObjectBase :: DoComputeTranslation( real p_rTime )
{
	Point3r l_ptReturn;
	TranslateKeyFramePtrRealMapIt l_itCur = m_mapTranslates.begin();

	while( l_itCur != m_mapTranslates.end() && l_itCur->second->GetTimeIndex() < p_rTime )
	{
		l_itCur++;
	}

	if( l_itCur != m_mapTranslates.end() )
	{
		TranslateKeyFramePtrRealMapIt l_itPrv = l_itCur;

		if( l_itPrv == m_mapTranslates.begin() )
		{
			l_ptReturn = l_itCur->second->GetValue();
		}
		else
		{
			--l_itPrv;
			real l_rDeltaTime = l_itCur->first - l_itPrv->first;
			real l_rFactor = (p_rTime - l_itPrv->first) / l_rDeltaTime;
			l_ptReturn = LinearPointInterpolator( l_itPrv->second->GetValue(), l_itCur->second->GetValue() ).Interpolate( l_rFactor );
		}
	}

	return l_ptReturn;
}

Quaternion MovingObjectBase :: DoComputeRotation( real p_rTime )
{
	Quaternion l_qReturn;
	RotateKeyFramePtrRealMapIt l_itCur = m_mapRotates.begin();

	while( l_itCur != m_mapRotates.end() && l_itCur->second->GetTimeIndex() < p_rTime )
	{
		l_itCur++;
	}

	if( l_itCur != m_mapRotates.end() )
	{
		RotateKeyFramePtrRealMapIt l_itPrv = l_itCur;

		if( l_itPrv == m_mapRotates.begin() )
		{
			l_qReturn = l_itCur->second->GetValue();
		}
		else
		{
			--l_itPrv;
			real l_rDeltaTime = l_itCur->first - l_itPrv->first;
			real l_rFactor = (p_rTime - l_itPrv->first) / l_rDeltaTime;
			l_qReturn = LinearQuaternionInterpolator( l_itPrv->second->GetValue(), l_itCur->second->GetValue() ).Interpolate( l_rFactor );
		}
	}

	return l_qReturn;
}

//*************************************************************************************************

const String MovingNode::StrVoidString;

MovingNode :: MovingNode()
	:	MovingObjectBase	( eMOVING_OBJECT_TYPE_NODE	)
{
}

MovingNode :: ~MovingNode()
{
}

String const & MovingNode :: GetName()const
{
	return StrVoidString;
}

void MovingNode :: DoApply()
{
	m_mtxFinalTransformation = m_mtxTransformations;
}

MovingObjectBaseSPtr MovingNode :: DoClone()
{
	return std::make_shared< MovingNode >( *this );
}

//*************************************************************************************************

MovingObject :: MovingObject()
	:	MovingObjectBase	( eMOVING_OBJECT_TYPE_OBJECT	)
{
}

MovingObject :: ~MovingObject()
{
}

String const & MovingObject :: GetName()const
{
	return GetObject()->GetName();
}

void MovingObject :: DoApply()
{
	m_mtxFinalTransformation = m_mtxTransformations;
}

MovingObjectBaseSPtr MovingObject :: DoClone()
{
	return std::make_shared< MovingObject >( *this );
}

//*************************************************************************************************

MovingBone :: MovingBone()
	:	MovingObjectBase	( eMOVING_OBJECT_TYPE_BONE	)
{
}

MovingBone :: ~MovingBone()
{
}

String const & MovingBone :: GetName()const
{
	return GetBone()->GetName();
}

void MovingBone :: DoApply()
{
	m_mtxFinalTransformation = m_mtxTransformations * GetBone()->GetOffsetMatrix();
}

MovingObjectBaseSPtr MovingBone :: DoClone()
{
	return std::make_shared< MovingBone >( *this );
}

//*************************************************************************************************

Animation :: Animation( String const & p_name )
	:	Named				( p_name			)
	,	m_rCurrentTime		( 0.0				)
	,	m_eState			( eSTATE_STOPPED	)
	,	m_rScale			( 1.0				)
	,	m_bLooped			( false				)
	,	m_rLength			( 0.0				)
{
}

Animation :: ~Animation()
{
}

void Animation :: Update( real p_rTslf )
{
	if( m_rLength == 0 )
	{
		std::for_each( m_mapToMove.begin(), m_mapToMove.end(), [&]( std::pair< String, MovingObjectBaseSPtr > p_pair )
		{
			m_rLength = std::max( m_rLength, p_pair.second->GetLength() );
		} );
	}

	if( m_eState == eSTATE_PLAYING )
	{
		m_rCurrentTime += (p_rTslf * m_rScale);

		if( m_rCurrentTime >= m_rLength )
		{
			if( !m_bLooped )
			{
				m_eState = eSTATE_PAUSED;
				m_rCurrentTime = m_rLength;
			}
			else
			{
				m_rCurrentTime = fmod( m_rCurrentTime, m_rLength );
			}
		}
	}

	if( m_eState != eSTATE_STOPPED )
	{
	 	std::for_each( m_mapToMove.begin(), m_mapToMove.end(), [&]( std::pair< String, MovingObjectBaseSPtr > p_pair )
	 	{
			p_pair.second->Update( m_rCurrentTime, m_bLooped, Matrix4x4r().get_identity() );
		} );
	}
}

MovingObjectBaseSPtr Animation :: AddMovingObject()
{
	std::shared_ptr< MovingNode > l_pObj = std::make_shared< MovingNode >();
	m_mapToMove.insert( std::make_pair( str_utils::to_string( uint32_t( m_mapToMove.size() ) ), l_pObj ) );
	return l_pObj;
}

MovingObjectBaseSPtr Animation :: AddMovingObject( MovableObjectSPtr p_pObject )
{
	MovingObjectBaseSPtr l_pReturn = map::find_or_null( m_mapToMove, p_pObject->GetName() );

	if( !l_pReturn )
	{
		std::shared_ptr< MovingObject > l_pObj = std::make_shared< MovingObject >();
		l_pObj->SetObject( p_pObject );
		l_pReturn = l_pObj;
		m_mapToMove.insert( std::make_pair( p_pObject->GetName(), l_pReturn ) );
	}

	return l_pReturn;
}

MovingObjectBaseSPtr Animation :: AddMovingObject( BoneSPtr p_pBone )
{
	MovingObjectBaseSPtr l_pReturn = map::find_or_null( m_mapToMove, p_pBone->GetName() );

	if( !l_pReturn )
	{
		std::shared_ptr< MovingBone > l_pObj = std::make_shared< MovingBone >();
		l_pObj->SetBone( p_pBone );
		l_pReturn = l_pObj;
		m_mapToMove.insert( std::make_pair( p_pBone->GetName(), l_pReturn ) );
	}

	return l_pReturn;
}

void Animation :: AddMovingObject( MovingObjectBaseSPtr p_pObject )
{
	if( !map::find_or_null( m_mapToMove, p_pObject->GetName() ) )
	{
		m_mapToMove.insert( std::make_pair( p_pObject->GetName(), p_pObject ) );
	}
}

MovingObjectBaseSPtr Animation :: GetMovingObject( BoneSPtr p_pBone )const
{
	return map::find_or_null( m_mapToMove, p_pBone->GetName() );
}

MovingObjectBaseSPtr Animation :: GetMovingObject( MovableObjectSPtr p_pBone )const
{
	return map::find_or_null( m_mapToMove, p_pBone->GetName() );
}

void Animation :: Play()
{
	m_eState = eSTATE_PLAYING;
}

void Animation :: Pause()
{
	if( m_eState == eSTATE_PLAYING )
	{
		m_eState = eSTATE_PAUSED;
	}
}

void Animation :: Stop()
{
	if( m_eState != eSTATE_STOPPED )
	{
		m_eState = eSTATE_STOPPED;
		m_rCurrentTime = 0.0;
	}
}

//*************************************************************************************************
