#include "Animation.hpp"

#include "Bone.hpp"
#include "Geometry.hpp"
#include "MovingBone.hpp"
#include "MovingNode.hpp"
#include "MovingObject.hpp"

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		static String MovingName[] =
		{
			cuT( "Node_" ),
			cuT( "Object_" ),
			cuT( "Bone_" ),
		};
	}

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
		if ( m_state != eSTATE_STOPPED )
		{
			if ( m_length == 0 )
			{
				for ( auto l_moving : m_arrayMoving )
				{
					m_length = std::max( m_length, l_moving->GetLength() );
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

			for ( auto l_moving : m_arrayMoving )
			{
				l_moving->Update( m_currentTime, m_looped, Matrix4x4r().get_identity() );
			}
		}
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

	MovingObjectBaseSPtr Animation::AddMovingObject( MovingObjectBaseSPtr p_parent )
	{
		std::shared_ptr< MovingNode > l_return = std::make_shared< MovingNode >();
		String l_name = MovingName[eMOVING_OBJECT_TYPE_NODE] + string::to_string( uint32_t( m_toMove.size() ) );
		auto l_it = m_toMove.find( l_name );

		if ( l_it == m_toMove.end() )
		{
			m_toMove.insert( std::make_pair( l_name, l_return ) );

			if ( !p_parent )
			{
				m_arrayMoving.push_back( l_return );
			}
		}
		else
		{
			CASTOR_EXCEPTION( "Can't add this node : already added" );
		}

		return l_return;
	}

	MovingObjectBaseSPtr Animation::AddMovingObject( GeometrySPtr p_object, MovingObjectBaseSPtr p_parent )
	{
		MovingObjectBaseSPtr l_return;
		String l_name = MovingName[eMOVING_OBJECT_TYPE_OBJECT] + p_object->GetName();
		auto l_it = m_toMove.find( l_name );

		if ( l_it == m_toMove.end() )
		{
			std::shared_ptr< MovingObject > l_moving = std::make_shared< MovingObject >();
			l_moving->SetObject( p_object );
			l_return = l_moving;
			m_toMove.insert( std::make_pair( l_name, l_return ) );

			if ( !p_parent )
			{
				m_arrayMoving.push_back( l_return );
			}
		}
		else
		{
			CASTOR_EXCEPTION( "Can't add this movable : already added" );
		}

		return l_return;
	}

	MovingObjectBaseSPtr Animation::AddMovingObject( BoneSPtr p_bone, MovingObjectBaseSPtr p_parent )
	{
		MovingObjectBaseSPtr l_return;
		String l_name = MovingName[eMOVING_OBJECT_TYPE_BONE] + p_bone->GetName();
		auto l_it = m_toMove.find( l_name );

		if ( l_it == m_toMove.end() )
		{
			std::shared_ptr< MovingBone > l_moving = std::make_shared< MovingBone >();
			l_moving->SetBone( p_bone );
			l_return = l_moving;
			m_toMove.insert( std::make_pair( l_name, l_return ) );

			if ( !p_parent )
			{
				m_arrayMoving.push_back( l_return );
			}
		}
		else
		{
			CASTOR_EXCEPTION( "Can't add this bone : already added" );
		}

		return l_return;
	}

	void Animation::AddMovingObject( MovingObjectBaseSPtr p_object, MovingObjectBaseSPtr p_parent )
	{
		String l_name = MovingName[p_object->GetType()] + p_object->GetName();
		MovingObjectPtrStrMapIt l_it = m_toMove.find( l_name );

		if ( l_it == m_toMove.end() )
		{
			m_toMove.insert( std::make_pair( l_name, p_object ) );

			if ( !p_parent )
			{
				m_arrayMoving.push_back( p_object );
			}
		}
		else
		{
			CASTOR_EXCEPTION( "Can't add this object : already added" );
		}
	}

	bool Animation::HasMovingObject( eMOVING_OBJECT_TYPE p_type, Castor::String const & p_name )const
	{
		return m_toMove.find( MovingName[p_type] + p_name ) != m_toMove.end();
	}

	MovingObjectBaseSPtr Animation::GetMovingObject( MovableObjectSPtr p_object )const
	{
		MovingObjectBaseSPtr l_return;
		auto l_it = m_toMove.find( p_object->GetName() );

		if ( l_it != m_toMove.end() )
		{
			l_return = l_it->second;
		}

		return l_return;
	}

	MovingObjectBaseSPtr Animation::GetMovingObject( BoneSPtr p_bone )const
	{
		MovingObjectBaseSPtr l_return;
		auto l_it = m_toMove.find( p_bone->GetName() );

		if ( l_it != m_toMove.end() )
		{
			l_return = l_it->second;
		}

		return l_return;
	}

	AnimationSPtr Animation::Clone()const
	{
		auto l_clone = std::make_shared< Animation >( GetName() );
		MovingObjectPtrStrMap l_toMove;

		for ( auto l_moving : m_arrayMoving )
		{
			auto l_mcln = l_moving->Clone( *l_clone );
			ENSURE( l_mcln );

			if ( l_mcln )
			{
				l_clone->m_arrayMoving.push_back( l_mcln );
				l_clone->m_toMove.insert( std::make_pair( MovingName[l_mcln->GetType()] + l_mcln->GetName(), l_mcln ) );
			}
		}

		return l_clone;
	}
}
