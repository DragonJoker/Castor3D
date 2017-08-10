#include "Tower.hpp"

#include "Enemy.hpp"
#include "Game.hpp"

#include <Scene/Animation/AnimatedObjectGroup.hpp>

using namespace castor;
using namespace castor3d;

namespace castortd
{
	//*********************************************************************************************
	
	//// Nearest enemy
	//EnemyArray doSortNearest( EnemyArray const & p_enemies, Point3r const & p_position )
	//{
	//	EnemyArray result{ p_enemies };
	//	std::sort( std::begin( result )
	//		, std::end( result )
	//		, [&p_position]( EnemyPtr a, EnemyPtr b )
	//		{
	//			return point::distanceSquared( a->getNode().getPosition(), p_position )
	//				< point::distanceSquared( b->getNode().getPosition(), p_position );
	//		} );
	//	return result;
	//}

	// First enemy
	EnemyArray doSortFirst( EnemyArray const & p_enemies, Point3r const & p_position )
	{
		return p_enemies;
	}

	//*********************************************************************************************

	Tower::Tower( CategoryPtr && p_category
		, SceneNode & p_node
		, castor3d::AnimatedObjectGroup & p_anim
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

	void Tower::accept( Game & p_game )
	{
		doUpdateTimes( p_game.getElapsed() );
		m_anim.setAnimationscale( m_category->getAttackAnimationName(), m_animScale );
		EnemyArray sorted;

		switch ( m_state )
		{
		case Tower::State::Idle:
			sorted = doSortFirst( p_game.getEnemies(), m_node.getPosition() );
			doLookForEnemy( sorted );
			break;

		case Tower::State::Spotted:
			if ( doCanShoot() )
			{
				doStartAttack();
			}
			break;

		case Tower::State::Shooting:
			sorted = doSortFirst( p_game.getEnemies(), m_node.getPosition() );
			if ( doAnimEnded( sorted ) )
			{
				doShoot( p_game );
			}
			break;
		}

		if ( m_target
			&& m_target->IsAlive()
			&& doIsInRange( *m_target ) )
		{
			doTurnToTarget();
		}
	}

	bool Tower::doLookForEnemy( EnemyArray & p_enemies )
	{
		for ( auto & enemy : p_enemies )
		{
			if ( enemy->IsAlive()
				&& doIsInRange( *enemy ) )
			{
				m_target = enemy;
				m_state = State::Spotted;
			}
		}

		return m_state == State::Spotted;
	}

	bool Tower::doCanShoot()
	{
		if ( !m_target
			 || !m_target->IsAlive()
			 || !doIsInRange( *m_target ) )
		{
			m_target = nullptr;
			m_state = State::Idle;
		}

		return m_state != State::Idle && m_remaining == Milliseconds{};
	}

	void Tower::doStartAttack()
	{
		m_remaining = m_category->getSpeed();
		m_animRemain = m_category->getAttackAnimationTime();
		m_anim.startAnimation( m_category->getAttackAnimationName() );
		m_state = State::Shooting;
	}

	bool Tower::doAnimEnded( EnemyArray & p_enemies )
	{
		bool result = m_animRemain <= Milliseconds{};

		if ( !result
			&& ( !m_target->IsAlive()
			|| !doIsInRange( *m_target ) ) )
		{
			if ( !doLookForEnemy( p_enemies ) )
			{
				m_anim.stopAnimation( m_category->getAttackAnimationName() );
				m_anim.startAnimation( m_category->getAttackAnimationName() );
				m_anim.pauseAnimation( m_category->getAttackAnimationName() );
				return false;
			}

			m_state = State::Shooting;
		}

		return result;
	}

	void Tower::doUpdateTimes( Milliseconds const & p_elapsed )
	{
		static Milliseconds zeroTime;
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
				m_anim.stopAnimation( m_category->getAttackAnimationName() );
				m_anim.startAnimation( m_category->getAttackAnimationName() );
				m_anim.pauseAnimation( m_category->getAttackAnimationName() );
			}
		}
	}

	void Tower::doShoot( Game & p_game )
	{
		m_state = State::Idle;
		p_game.EmitBullet( m_category->getBulletSpeed(), m_category->getDamage(), m_node.getPosition(), *m_target );
	}

	bool Tower::doIsInRange( Enemy const & p_enemy )const
	{
		return point::length( m_node.getPosition() - p_enemy.getNode().getPosition() ) <= m_category->getRange();
	}

	void Tower::doTurnToTarget()
	{
		auto targetPosition = m_target->getNode().getDerivedPosition();
		targetPosition[1] = m_node.getPosition()[1];
		auto direction = targetPosition - m_node.getDerivedPosition();
		direction = point::getNormalised( direction );
		Point3r up{ 0, 1, 0 };
		auto right = direction ^ up;
		auto transform = matrix::lookAt( m_node.getDerivedPosition(), m_node.getDerivedPosition() - direction, up );
		m_node.setOrientation( Quaternion::fromMatrix( transform ) );
	}
}
