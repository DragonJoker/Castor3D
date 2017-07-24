#include "Tower.hpp"

#include "Enemy.hpp"
#include "Game.hpp"

#include <Scene/Animation/AnimatedObjectGroup.hpp>

using namespace Castor;
using namespace Castor3D;

namespace castortd
{
	//*********************************************************************************************
	
	//// Nearest enemy
	//EnemyArray DoSortNearest( EnemyArray const & p_enemies, Point3r const & p_position )
	//{
	//	EnemyArray result{ p_enemies };
	//	std::sort( std::begin( result )
	//		, std::end( result )
	//		, [&p_position]( EnemyPtr a, EnemyPtr b )
	//		{
	//			return point::distance_squared( a->GetNode().GetPosition(), p_position )
	//				< point::distance_squared( b->GetNode().GetPosition(), p_position );
	//		} );
	//	return result;
	//}

	// First enemy
	EnemyArray DoSortFirst( EnemyArray const & p_enemies, Point3r const & p_position )
	{
		return p_enemies;
	}

	//*********************************************************************************************

	Tower::Tower( CategoryPtr && p_category
		, SceneNode & p_node
		, Castor3D::AnimatedObjectGroup & p_anim
		, Cell const & p_cell )
		: m_node{ p_node }
		, m_anim{ p_anim }
		, m_cell{ p_cell }
		, m_category( std::move( p_category ) )
	{
	}

	Tower::~Tower()
	{
	}

	void Tower::Accept( Game & p_game )
	{
		DoUpdateTimes( p_game.GetElapsed() );
		m_anim.SetAnimationScale( m_category->GetAttackAnimationName(), m_animScale );

		switch ( m_state )
		{
		case Tower::State::Idle:
			DoLookForEnemy( DoSortFirst( p_game.GetEnemies(), m_node.GetPosition() ) );
			break;

		case Tower::State::Spotted:
			if ( DoCanShoot() )
			{
				DoStartAttack();
			}
			break;

		case Tower::State::Shooting:
			if ( DoAnimEnded( DoSortFirst( p_game.GetEnemies(), m_node.GetPosition() ) ) )
			{
				DoShoot( p_game );
			}
			break;
		}

		if ( m_target
			&& m_target->IsAlive()
			&& DoIsInRange( *m_target ) )
		{
			DoTurnToTarget();
		}
	}

	bool Tower::DoLookForEnemy( EnemyArray & p_enemies )
	{
		for ( auto & enemy : p_enemies )
		{
			if ( enemy->IsAlive()
				&& DoIsInRange( *enemy ) )
			{
				m_target = enemy;
				m_state = State::Spotted;
			}
		}

		return m_state == State::Spotted;
	}

	bool Tower::DoCanShoot()
	{
		if ( !m_target
			 || !m_target->IsAlive()
			 || !DoIsInRange( *m_target ) )
		{
			m_target = nullptr;
			m_state = State::Idle;
		}

		return m_state != State::Idle && m_remaining == Castor::Milliseconds{};
	}

	void Tower::DoStartAttack()
	{
		m_remaining = m_category->GetSpeed();
		m_animRemain = m_category->GetAttackAnimationTime();
		m_anim.StartAnimation( m_category->GetAttackAnimationName() );
		m_state = State::Shooting;
	}

	bool Tower::DoAnimEnded( EnemyArray & p_enemies )
	{
		bool result = m_animRemain <= Castor::Milliseconds{};

		if ( !result
			&& ( !m_target->IsAlive()
			|| !DoIsInRange( *m_target ) ) )
		{
			if ( !DoLookForEnemy( p_enemies ) )
			{
				m_anim.StopAnimation( m_category->GetAttackAnimationName() );
				m_anim.StartAnimation( m_category->GetAttackAnimationName() );
				m_anim.PauseAnimation( m_category->GetAttackAnimationName() );
				return false;
			}

			m_state = State::Shooting;
		}

		return result;
	}

	void Tower::DoUpdateTimes( Castor::Milliseconds const & p_elapsed )
	{
		static Castor::Milliseconds zeroTime;
		m_remaining -= p_elapsed;

		if ( m_remaining < zeroTime )
		{
			m_remaining = zeroTime;
		}

		if ( m_state == State::Shooting )
		{
			m_animRemain -= p_elapsed;

			if ( m_animRemain <= zeroTime )
			{
				m_animRemain = zeroTime;
				m_anim.StopAnimation( m_category->GetAttackAnimationName() );
				m_anim.StartAnimation( m_category->GetAttackAnimationName() );
				m_anim.PauseAnimation( m_category->GetAttackAnimationName() );
			}
		}
	}

	void Tower::DoShoot( Game & p_game )
	{
		m_state = State::Idle;
		p_game.EmitBullet( m_category->GetBulletSpeed(), m_category->GetDamage(), m_node.GetPosition(), *m_target );
	}

	bool Tower::DoIsInRange( Enemy const & p_enemy )const
	{
		return point::length( m_node.GetPosition() - p_enemy.GetNode().GetPosition() ) <= m_category->GetRange();
	}

	void Tower::DoTurnToTarget()
	{
		auto targetPosition = m_target->GetNode().GetDerivedPosition();
		targetPosition[1] = m_node.GetPosition()[1];
		auto direction = targetPosition - m_node.GetDerivedPosition();
		direction = point::get_normalised( direction );
		Point3r up{ 0, 1, 0 };
		auto right = direction ^ up;
		auto transform = matrix::look_at( m_node.GetDerivedPosition(), m_node.GetDerivedPosition() - direction, up );
		m_node.SetOrientation( Quaternion::from_matrix( transform ) );
	}
}
