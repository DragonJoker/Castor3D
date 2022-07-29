#include "Hud.hpp"

#include "Game.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/OverlayCache.hpp>
#include <Castor3D/Overlay/Overlay.hpp>
#include <Castor3D/Overlay/TextOverlay.hpp>
#include <Castor3D/Scene/Scene.hpp>

namespace castortd
{
	namespace
	{
		castor3d::TextOverlaySPtr getTextOverlay( castor3d::Scene const & scene
			, castor::String const & p_name )
		{
			castor3d::TextOverlaySPtr result;
			auto overlay = scene.findOverlay( p_name ).lock();

			if ( overlay )
			{
				result = overlay->getTextOverlay();
			}

			return result;
		}
	}

	Hud::Hud( Game const & p_game, castor3d::Scene const & p_scene )
		: m_game{ p_game }
		, m_lives{ getTextOverlay( p_scene, cuT( "LivesValue" ) ) }
		, m_ore{ getTextOverlay( p_scene, cuT( "OreValue" ) ) }
		, m_level{ getTextOverlay( p_scene, cuT( "LevelValue" ) ) }
		, m_kills{ getTextOverlay( p_scene, cuT( "KillsValue" ) ) }
		, m_enemyLife{ getTextOverlay( p_scene, cuT( "MonsterLifeValue" ) ) }
		, m_enemyBounty{ getTextOverlay( p_scene, cuT( "MonsterBountyValue" ) ) }
		, m_towerSpeed{ getTextOverlay( p_scene, cuT( "TowerSpeedValue" ) ) }
		, m_towerRange{ getTextOverlay( p_scene, cuT( "TowerRangeValue" ) ) }
		, m_towerDamage{ getTextOverlay( p_scene, cuT( "TowerDamageValue" ) ) }
	{
	}

	void Hud::initialise()
	{
		auto & scene = m_game.getScene();
		scene.findOverlay( cuT( "TitlePanel" ) ).lock()->setVisible( true );
		scene.findOverlay( cuT( "HUDResources" ) ).lock()->setVisible( false );
		scene.findOverlay( cuT( "HUDScore" ) ).lock()->setVisible( false );
		scene.findOverlay( cuT( "HUDDetails" ) ).lock()->setVisible( false );
		scene.findOverlay( cuT( "HUDPause" ) ).lock()->setVisible( false );
		scene.findOverlay( cuT( "GameEndPanel" ) ).lock()->setVisible( false );
		scene.findOverlay( cuT( "HelpPanel" ) ).lock()->setVisible( false );
	}

	void Hud::start()
	{
		auto & scene = m_game.getScene();
		scene.findOverlay( cuT( "TitlePanel" ) ).lock()->setVisible( false );
		scene.findOverlay( cuT( "HUDResources" ) ).lock()->setVisible( true );
		scene.findOverlay( cuT( "HUDScore" ) ).lock()->setVisible( true );
		scene.findOverlay( cuT( "HUDDetails" ) ).lock()->setVisible( true );
		scene.findOverlay( cuT( "HUDPause" ) ).lock()->setVisible( false );
		scene.findOverlay( cuT( "GameEndPanel" ) ).lock()->setVisible( false );
		scene.findOverlay( cuT( "HelpPanel" ) ).lock()->setVisible( false );
		update();
	}

	void Hud::pause()
	{
		auto & scene = m_game.getScene();
		scene.findOverlay( cuT( "TitlePanel" ) ).lock()->setVisible( false );
		scene.findOverlay( cuT( "HUDResources" ) ).lock()->setVisible( false );
		scene.findOverlay( cuT( "HUDScore" ) ).lock()->setVisible( false );
		scene.findOverlay( cuT( "HUDDetails" ) ).lock()->setVisible( false );
		scene.findOverlay( cuT( "HUDPause" ) ).lock()->setVisible( true );
		scene.findOverlay( cuT( "GameEndPanel" ) ).lock()->setVisible( false );
		scene.findOverlay( cuT( "HelpPanel" ) ).lock()->setVisible( false );
	}

	void Hud::resume()
	{
		auto & scene = m_game.getScene();
		scene.findOverlay( cuT( "TitlePanel" ) ).lock()->setVisible( false );
		scene.findOverlay( cuT( "HUDResources" ) ).lock()->setVisible( true );
		scene.findOverlay( cuT( "HUDScore" ) ).lock()->setVisible( true );
		scene.findOverlay( cuT( "HUDDetails" ) ).lock()->setVisible( true );
		scene.findOverlay( cuT( "HUDPause" ) ).lock()->setVisible( false );
		scene.findOverlay( cuT( "GameEndPanel" ) ).lock()->setVisible( false );
		scene.findOverlay( cuT( "HelpPanel" ) ).lock()->setVisible( false );
	}

	void Hud::Help()
	{
		auto & scene = m_game.getScene();
		scene.findOverlay( cuT( "TitlePanel" ) ).lock()->setVisible( false );
		scene.findOverlay( cuT( "HUDResources" ) ).lock()->setVisible( false );
		scene.findOverlay( cuT( "HUDScore" ) ).lock()->setVisible( false );
		scene.findOverlay( cuT( "HUDDetails" ) ).lock()->setVisible( false );
		scene.findOverlay( cuT( "HUDPause" ) ).lock()->setVisible( false );
		scene.findOverlay( cuT( "GameEndPanel" ) ).lock()->setVisible( false );
		scene.findOverlay( cuT( "HelpPanel" ) ).lock()->setVisible( true );
	}

	void Hud::GameOver()
	{
		auto & scene = m_game.getScene();
		scene.findOverlay( cuT( "TitlePanel" ) ).lock()->setVisible( false );
		scene.findOverlay( cuT( "HUDResources" ) ).lock()->setVisible( false );
		scene.findOverlay( cuT( "HUDScore" ) ).lock()->setVisible( false );
		scene.findOverlay( cuT( "HUDDetails" ) ).lock()->setVisible( false );
		scene.findOverlay( cuT( "HUDPause" ) ).lock()->setVisible( false );
		scene.findOverlay( cuT( "GameEndPanel" ) ).lock()->setVisible( true );
		scene.findOverlay( cuT( "HelpPanel" ) ).lock()->setVisible( false );

		getTextOverlay( m_game.getScene(), cuT( "ResultLevelValue" ) )->setCaption( castor::string::toString( m_game.getWave() ) );
		getTextOverlay( m_game.getScene(), cuT( "ResultKillsValue" ) )->setCaption( castor::string::toString( m_game.getKills() ) );
	}

	void Hud::update()
	{
		auto text = m_lives.lock();

		if ( text )
		{
			text->setCaption( castor::string::toString( m_game.getLives() ) );
		}

		text = m_ore.lock();

		if ( text )
		{
			text->setCaption( castor::string::toString( m_game.getOre() ) );
		}

		text = m_level.lock();

		if ( text )
		{
			text->setCaption( castor::string::toString( m_game.getWave() ) );
		}

		text = m_kills.lock();

		if ( text )
		{
			text->setCaption( castor::string::toString( m_game.getKills() ) );
		}

		text = m_enemyLife.lock();

		if ( text )
		{
			text->setCaption( castor::string::toString( m_game.getEnemiesLife() ) );
		}

		text = m_enemyBounty.lock();

		if ( text )
		{
			text->setCaption( castor::string::toString( m_game.getEnemiesBounty() ) );
		}

		TowerPtr tower = m_game.getSelectedTower();

		if ( tower )
		{
			text = m_towerDamage.lock();

			if ( text )
			{
				text->setVisible( true );
				text->setCaption( castor::string::toString( tower->getDamage() ) );
			}

			text = m_towerSpeed.lock();

			if ( text )
			{
				text->setVisible( true );
				text->setCaption( castor::string::toString( tower->getSpeed() ) );
			}

			text = m_towerRange.lock();

			if ( text )
			{
				text->setVisible( true );
				text->setCaption( castor::string::toString( tower->getRange() ) );
			}
		}
		else
		{
			text = m_towerDamage.lock();

			if ( text )
			{
				text->setVisible( false );
			}

			text = m_towerSpeed.lock();

			if ( text )
			{
				text->setVisible( false );
			}

			text = m_towerRange.lock();

			if ( text )
			{
				text->setVisible( false );
			}
		}
	}
}
