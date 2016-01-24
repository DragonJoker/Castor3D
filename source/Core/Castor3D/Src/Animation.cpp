#include "Animation.hpp"

#include "Bone.hpp"
#include "MovableObject.hpp"
#include "MovingBone.hpp"
#include "MovingNode.hpp"
#include "MovingObject.hpp"

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
		std::shared_ptr< MovingNode > l_return = std::make_shared< MovingNode >();
		m_toMove.insert( std::make_pair( cuT( "Node_" ) + string::to_string( uint32_t( m_toMove.size() ) ), l_return ) );
		return l_return;
	}

	MovingObjectBaseSPtr Animation::AddMovingObject( MovableObjectSPtr p_object )
	{
		MovingObjectBaseSPtr l_return;
		auto l_it = m_toMove.find( cuT( "Movable_" ) + p_object->GetName() );

		if ( l_it == m_toMove.end() )
		{
			std::shared_ptr< MovingObject > l_moving = std::make_shared< MovingObject >();
			l_moving->SetObject( p_object );
			l_return = l_moving;
			m_toMove.insert( std::make_pair( cuT( "Movable_" ) + p_object->GetName(), l_return ) );
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
		auto l_it = m_toMove.find( cuT( "Bone_" ) + p_bone->GetName() );

		if ( l_it == m_toMove.end() )
		{
			std::shared_ptr< MovingBone > l_moving = std::make_shared< MovingBone >();
			l_moving->SetBone( p_bone );
			l_return = l_moving;
			m_toMove.insert( std::make_pair( cuT( "Bone_" ) + p_bone->GetName(), l_return ) );
		}
		else
		{
			CASTOR_EXCEPTION( "Can't add this bone : already added" );
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
