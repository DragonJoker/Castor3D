#include "Animation.hpp"
#include "MovingObject.hpp"
#include "MovingNode.hpp"
#include "MovingBone.hpp"
#include "MovableObject.hpp"
#include "Skeleton.hpp"
#include "Bone.hpp"

using namespace Castor;

namespace Castor3D
{
	Animation::Animation( String const & p_name )
		:	Named( p_name )
		,	m_rCurrentTime( 0.0 )
		,	m_eState( eSTATE_STOPPED )
		,	m_rScale( 1.0 )
		,	m_bLooped( false )
		,	m_rLength( 0.0 )
	{
	}

	Animation::~Animation()
	{
	}

	void Animation::Update( real p_rTslf )
	{
		if ( m_rLength == 0 )
		{
			std::for_each( m_mapToMove.begin(), m_mapToMove.end(), [&]( std::pair< String, MovingObjectBaseSPtr > p_pair )
			{
				m_rLength = std::max( m_rLength, p_pair.second->GetLength() );
			} );
		}

		if ( m_eState == eSTATE_PLAYING )
		{
			m_rCurrentTime += ( p_rTslf * m_rScale );

			if ( m_rCurrentTime >= m_rLength )
			{
				if ( !m_bLooped )
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

		if ( m_eState != eSTATE_STOPPED )
		{
			std::for_each( m_mapToMove.begin(), m_mapToMove.end(), [&]( std::pair< String, MovingObjectBaseSPtr > p_pair )
			{
				p_pair.second->Update( m_rCurrentTime, m_bLooped, Matrix4x4r().get_identity() );
			} );
		}
	}

	MovingObjectBaseSPtr Animation::AddMovingObject()
	{
		std::shared_ptr< MovingNode > l_pObj = std::make_shared< MovingNode >();
		m_mapToMove.insert( std::make_pair( str_utils::to_string( uint32_t( m_mapToMove.size() ) ), l_pObj ) );
		return l_pObj;
	}

	MovingObjectBaseSPtr Animation::AddMovingObject( MovableObjectSPtr p_pObject )
	{
		MovingObjectBaseSPtr l_return;
		MovingObjectPtrStrMapIt l_it = m_mapToMove.find( p_pObject->GetName() );

		if ( l_it != m_mapToMove.end() )
		{
			l_return = l_it->second;
			std::shared_ptr< MovingObject > l_pObj = std::make_shared< MovingObject >();
			l_pObj->SetObject( p_pObject );
			l_return = l_pObj;
			m_mapToMove.insert( std::make_pair( p_pObject->GetName(), l_return ) );
		}
		else
		{
			CASTOR_EXCEPTION( "Can't add this movable : already added" );
		}

		return l_return;
	}

	MovingObjectBaseSPtr Animation::AddMovingObject( BoneSPtr p_pBone )
	{
		MovingObjectBaseSPtr l_return;
		MovingObjectPtrStrMapIt l_it = m_mapToMove.find( p_pBone->GetName() );

		if ( l_it != m_mapToMove.end() )
		{
			l_return = l_it->second;
			std::shared_ptr< MovingBone > l_pObj = std::make_shared< MovingBone >();
			l_pObj->SetBone( p_pBone );
			l_return = l_pObj;
			m_mapToMove.insert( std::make_pair( p_pBone->GetName(), l_return ) );
		}
		else
		{
			CASTOR_EXCEPTION( "Can't add this bone : already added" );
		}

		return l_return;
	}

	void Animation::AddMovingObject( MovingObjectBaseSPtr p_pObject )
	{
		MovingObjectPtrStrMapIt l_it = m_mapToMove.find( p_pObject->GetName() );

		if ( l_it == m_mapToMove.end() )
		{
			m_mapToMove.insert( std::make_pair( p_pObject->GetName(), p_pObject ) );
		}
		else
		{
			CASTOR_EXCEPTION( "Can't add this object : already added" );
		}
	}

	MovingObjectBaseSPtr Animation::GetMovingObject( BoneSPtr p_pBone )const
	{
		MovingObjectBaseSPtr l_return;
		MovingObjectPtrStrMapConstIt l_it = m_mapToMove.find( p_pBone->GetName() );

		if ( l_it == m_mapToMove.end() )
		{
			l_return = l_it->second;
		}

		return l_return;
	}

	MovingObjectBaseSPtr Animation::GetMovingObject( MovableObjectSPtr p_pObject )const
	{
		MovingObjectBaseSPtr l_return;
		MovingObjectPtrStrMapConstIt l_it = m_mapToMove.find( p_pObject->GetName() );

		if ( l_it == m_mapToMove.end() )
		{
			l_return = l_it->second;
		}

		return l_return;
	}

	void Animation::Play()
	{
		m_eState = eSTATE_PLAYING;
	}

	void Animation::Pause()
	{
		if ( m_eState == eSTATE_PLAYING )
		{
			m_eState = eSTATE_PAUSED;
		}
	}

	void Animation::Stop()
	{
		if ( m_eState != eSTATE_STOPPED )
		{
			m_eState = eSTATE_STOPPED;
			m_rCurrentTime = 0.0;
		}
	}
}
