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
		: Named( p_name )
		, m_currentTime( 0.0 )
		, m_state( eSTATE_STOPPED )
		, m_scale( 1.0 )
		, m_looped( false )
		, m_length( 0.0 )
	{
	}

	Animation::~Animation()
	{
	}

	void Animation::Update( real p_tslf )
	{
		if ( m_length == 0 )
		{
			for ( auto l_it : m_toMove )
			{
				m_length = std::max( m_length, l_it.second->GetLength() );
			}
		}

		if ( m_state == eSTATE_PLAYING )
		{
			m_currentTime += ( p_tslf * m_scale );

			if ( m_currentTime >= m_length )
			{
				if ( !m_looped )
				{
					m_state = eSTATE_PAUSED;
					m_currentTime = m_length;
				}
				else
				{
					m_currentTime = fmod( m_currentTime, m_length );
				}
			}
		}

		if ( m_state != eSTATE_STOPPED )
		{
			for ( auto l_it : m_toMove )
			{
				l_it.second->Update( m_currentTime, m_looped, Matrix4x4r().get_identity() );
			}
		}
	}

	MovingObjectBaseSPtr Animation::AddMovingObject()
	{
		std::shared_ptr< MovingNode > l_node = std::make_shared< MovingNode >();
		m_toMove.insert( std::make_pair( string::to_string( uint32_t( m_toMove.size() ) ), l_node ) );
		return l_node;
	}

	MovingObjectBaseSPtr Animation::AddMovingObject( MovableObjectSPtr p_object )
	{
		MovingObjectBaseSPtr l_return;
		MovingObjectPtrStrMapIt l_it = m_toMove.find( p_object->GetName() );

		if ( l_it == m_toMove.end() )
		{
			std::shared_ptr< MovingObject > l_pObj = std::make_shared< MovingObject >();
			l_pObj->SetObject( p_object );
			l_return = l_pObj;
			m_toMove.insert( std::make_pair( p_object->GetName(), l_return ) );
		}
		else
		{
			CASTOR_EXCEPTION( "Can't add this movable : already added" );
		}

		return l_return;
	}

	MovingObjectBaseSPtr Animation::AddMovingObject( BoneSPtr p_bone )
	{
		MovingObjectBaseSPtr l_return;
		MovingObjectPtrStrMapIt l_it = m_toMove.find( p_bone->GetName() );

		if ( l_it == m_toMove.end() )
		{
			std::shared_ptr< MovingBone > l_pObj = std::make_shared< MovingBone >();
			l_pObj->SetBone( p_bone );
			l_return = l_pObj;
			m_toMove.insert( std::make_pair( p_bone->GetName(), l_return ) );
		}
		else
		{
			CASTOR_EXCEPTION( "Can't add this bone : already added" );
		}

		return l_return;
	}

	void Animation::AddMovingObject( MovingObjectBaseSPtr p_object )
	{
		MovingObjectPtrStrMapIt l_it = m_toMove.find( p_object->GetName() );

		if ( l_it == m_toMove.end() )
		{
			m_toMove.insert( std::make_pair( p_object->GetName(), p_object ) );
		}
		else
		{
			CASTOR_EXCEPTION( "Can't add this object : already added" );
		}
	}

	MovingObjectBaseSPtr Animation::GetMovingObject( BoneSPtr p_bone )const
	{
		MovingObjectBaseSPtr l_return;
		MovingObjectPtrStrMapConstIt l_it = m_toMove.find( p_bone->GetName() );

		if ( l_it != m_toMove.end() )
		{
			l_return = l_it->second;
		}

		return l_return;
	}

	MovingObjectBaseSPtr Animation::GetMovingObject( MovableObjectSPtr p_object )const
	{
		MovingObjectBaseSPtr l_return;
		MovingObjectPtrStrMapConstIt l_it = m_toMove.find( p_object->GetName() );

		if ( l_it != m_toMove.end() )
		{
			l_return = l_it->second;
		}

		return l_return;
	}

	void Animation::Play()
	{
		m_state = eSTATE_PLAYING;
	}

	void Animation::Pause()
	{
		if ( m_state == eSTATE_PLAYING )
		{
			m_state = eSTATE_PAUSED;
		}
	}

	void Animation::Stop()
	{
		if ( m_state != eSTATE_STOPPED )
		{
			m_state = eSTATE_STOPPED;
			m_currentTime = 0.0;
		}
	}
}
