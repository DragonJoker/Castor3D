#include "Tower.hpp"

#include "Enemy.hpp"
#include "Game.hpp"

#include <Castor3D/Scene/Animation/AnimatedObjectGroup.hpp>

namespace castortd
{
	//*********************************************************************************************

	namespace
	{
		static castor::Milliseconds constexpr zeroTime{ 0_ms };

		// Nearest enemy
		EnemyArray doSortNearest( EnemyArray const & enemies, castor::Point3f const & position )
		{
			EnemyArray result{ enemies };
			std::sort( std::begin( result )
				, std::end( result )
				, [&position]( EnemyPtr a, EnemyPtr b )
				{
					return castor::point::distanceSquared( a->getNode().getPosition(), position )
						< castor::point::distanceSquared( b->getNode().getPosition(), position );
				} );
			return result;
		}

		//// First enemy
		//EnemyArray doSortFirst( EnemyArray const & enemies, castor::Point3f const & position )
		//{
		//	return enemies;
		//}
	}

	//*********************************************************************************************

	Tower::Tower( CategoryPtr && category
		, castor3d::SceneNode & node
		, castor3d::AnimatedObjectGroup & anim
		, Cell const & cell )
		: m_node{ node }
		, m_anim{ anim }
		, m_cell{ cell }
		, m_category( castor::move( category ) )
	{
	}

	void Tower::accept( Game & game )
	{
		doUpdateTimes( game.getElapsed() );
		m_anim.setAnimationScale( m_category->getAttackAnimationName(), m_animScale );
		EnemyArray sorted;

		switch ( m_state )
		{
		case Tower::State::Idle:
			sorted = doSortNearest( game.getEnemies(), m_node.getPosition() );
			doLookForEnemy( sorted );
			break;

		case Tower::State::Spotted:
			if ( doCanShoot() )
			{
				doStartAttack();
			}
			break;

		case Tower::State::Shooting:
			sorted = doSortNearest( game.getEnemies(), m_node.getPosition() );
			if ( doAnimEnded( sorted ) )
			{
				doShoot( game );
			}
			break;
		}

		if ( m_target
			&& m_target->isAlive()
			&& doIsInRange( *m_target ) )
		{
			doTurnToTarget();
		}
	}

	bool Tower::doLookForEnemy( EnemyArray & enemies )
	{
		for ( auto & enemy : enemies )
		{
			if ( enemy->isAlive()
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
			 || !m_target->isAlive()
			 || !doIsInRange( *m_target ) )
		{
			m_target = nullptr;
			m_state = State::Idle;
		}

		return m_state != State::Idle && m_remaining == 0_ms;
	}

	void Tower::doStartAttack()
	{
		m_animRemain = castor::Milliseconds{ int64_t( float( m_category->getAttackAnimationTime().count() ) / m_category->getSpeed() ) };
		m_anim.startAnimation( m_category->getAttackAnimationName() );
		m_state = State::Shooting;
	}

	bool Tower::doAnimEnded( EnemyArray & enemies )
	{
		bool result = m_animRemain <= zeroTime;

		if ( !result
			&& ( !m_target->isAlive()
			|| !doIsInRange( *m_target ) ) )
		{
			if ( !doLookForEnemy( enemies ) )
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

	void Tower::doUpdateTimes( castor::Milliseconds const & elapsed )
	{
		m_remaining -= elapsed;

		if ( m_remaining < zeroTime )
		{
			m_remaining = zeroTime;
		}

		if ( m_state == State::Shooting )
		{
			m_animRemain -= elapsed;

			if ( m_animRemain <= zeroTime )
			{
				m_animRemain = zeroTime;
				m_anim.stopAnimation( m_category->getAttackAnimationName() );
				m_anim.startAnimation( m_category->getAttackAnimationName() );
				m_anim.pauseAnimation( m_category->getAttackAnimationName() );
			}
		}
	}

	void Tower::doShoot( Game & game )
	{
		m_state = State::Idle;

		switch ( m_category->getKind() )
		{
		case Category::Kind::eShortRange:
			game.emitBullet( m_category->getBulletSpeed()
				, m_category->getDamage()
				, m_node.getPosition()
				, *m_target );
			break;
		case Category::Kind::eLongRange:
			game.emitBoulder( m_category->getBulletSpeed()
				, m_category->getDamage()
				, m_node.getPosition()
				, m_target->getNode().getDerivedPosition() );
			break;
		}
	}

	bool Tower::doIsInRange( Enemy const & enemy )const
	{
		return castor::point::length( m_node.getPosition() - enemy.getNode().getPosition() ) <= m_category->getRange();
	}

	void Tower::doTurnToTarget()
	{
		auto targetPosition = m_target->getNode().getDerivedPosition();
		targetPosition[1] = m_node.getPosition()[1];
		auto direction = targetPosition - m_node.getDerivedPosition();
		direction = castor::point::getNormalised( direction );
		castor::Point3f up{ 0, 1, 0 };
		auto transform = castor::matrix::lookAt( m_node.getDerivedPosition(), m_node.getDerivedPosition() - direction, up );
		m_node.setOrientation( castor::Quaternion::fromMatrix( transform ) );
	}
}
