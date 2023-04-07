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
		castor3d::TextOverlayRPtr getTextOverlay( castor3d::Scene const & scene
			, castor::String const & name )
		{
			castor3d::TextOverlayRPtr result{};

			if ( auto overlay = scene.findOverlay( name ) )
			{
				result = overlay->getTextOverlay();
			}

			return result;
		}
	}

	Hud::Hud( Game const & game, castor3d::Scene const & scene )
		: m_game{ game }
		, m_lives{ getTextOverlay( scene, cuT( "LivesValue" ) ) }
		, m_ore{ getTextOverlay( scene, cuT( "OreValue" ) ) }
		, m_level{ getTextOverlay( scene, cuT( "LevelValue" ) ) }
		, m_kills{ getTextOverlay( scene, cuT( "KillsValue" ) ) }
		, m_enemyLife{ getTextOverlay( scene, cuT( "MonsterLifeValue" ) ) }
		, m_enemyBounty{ getTextOverlay( scene, cuT( "MonsterBountyValue" ) ) }
		, m_towerSpeed{ getTextOverlay( scene, cuT( "TowerSpeedValue" ) ) }
		, m_towerRange{ getTextOverlay( scene, cuT( "TowerRangeValue" ) ) }
		, m_towerDamage{ getTextOverlay( scene, cuT( "TowerDamageValue" ) ) }
	{
	}

	void Hud::initialise()
	{
		auto & scene = m_game.getScene();
		scene.findOverlay( cuT( "TitlePanel" ) )->setVisible( true );
		scene.findOverlay( cuT( "HUDResources" ) )->setVisible( false );
		scene.findOverlay( cuT( "HUDScore" ) )->setVisible( false );
		scene.findOverlay( cuT( "HUDDetails" ) )->setVisible( false );
		scene.findOverlay( cuT( "HUDPause" ) )->setVisible( false );
		scene.findOverlay( cuT( "GameEndPanel" ) )->setVisible( false );
		scene.findOverlay( cuT( "HelpPanel" ) )->setVisible( false );
	}

	void Hud::start()
	{
		auto & scene = m_game.getScene();
		scene.findOverlay( cuT( "TitlePanel" ) )->setVisible( false );
		scene.findOverlay( cuT( "HUDResources" ) )->setVisible( true );
		scene.findOverlay( cuT( "HUDScore" ) )->setVisible( true );
		scene.findOverlay( cuT( "HUDDetails" ) )->setVisible( true );
		scene.findOverlay( cuT( "HUDPause" ) )->setVisible( false );
		scene.findOverlay( cuT( "GameEndPanel" ) )->setVisible( false );
		scene.findOverlay( cuT( "HelpPanel" ) )->setVisible( false );
		update();
	}

	void Hud::pause()
	{
		auto & scene = m_game.getScene();
		scene.findOverlay( cuT( "TitlePanel" ) )->setVisible( false );
		scene.findOverlay( cuT( "HUDResources" ) )->setVisible( false );
		scene.findOverlay( cuT( "HUDScore" ) )->setVisible( false );
		scene.findOverlay( cuT( "HUDDetails" ) )->setVisible( false );
		scene.findOverlay( cuT( "HUDPause" ) )->setVisible( true );
		scene.findOverlay( cuT( "GameEndPanel" ) )->setVisible( false );
		scene.findOverlay( cuT( "HelpPanel" ) )->setVisible( false );
	}

	void Hud::resume()
	{
		auto & scene = m_game.getScene();
		scene.findOverlay( cuT( "TitlePanel" ) )->setVisible( false );
		scene.findOverlay( cuT( "HUDResources" ) )->setVisible( true );
		scene.findOverlay( cuT( "HUDScore" ) )->setVisible( true );
		scene.findOverlay( cuT( "HUDDetails" ) )->setVisible( true );
		scene.findOverlay( cuT( "HUDPause" ) )->setVisible( false );
		scene.findOverlay( cuT( "GameEndPanel" ) )->setVisible( false );
		scene.findOverlay( cuT( "HelpPanel" ) )->setVisible( false );
	}

	void Hud::Help()
	{
		auto & scene = m_game.getScene();
		scene.findOverlay( cuT( "TitlePanel" ) )->setVisible( false );
		scene.findOverlay( cuT( "HUDResources" ) )->setVisible( false );
		scene.findOverlay( cuT( "HUDScore" ) )->setVisible( false );
		scene.findOverlay( cuT( "HUDDetails" ) )->setVisible( false );
		scene.findOverlay( cuT( "HUDPause" ) )->setVisible( false );
		scene.findOverlay( cuT( "GameEndPanel" ) )->setVisible( false );
		scene.findOverlay( cuT( "HelpPanel" ) )->setVisible( true );
	}

	void Hud::GameOver()
	{
		auto & scene = m_game.getScene();
		scene.findOverlay( cuT( "TitlePanel" ) )->setVisible( false );
		scene.findOverlay( cuT( "HUDResources" ) )->setVisible( false );
		scene.findOverlay( cuT( "HUDScore" ) )->setVisible( false );
		scene.findOverlay( cuT( "HUDDetails" ) )->setVisible( false );
		scene.findOverlay( cuT( "HUDPause" ) )->setVisible( false );
		scene.findOverlay( cuT( "GameEndPanel" ) )->setVisible( true );
		scene.findOverlay( cuT( "HelpPanel" ) )->setVisible( false );

		getTextOverlay( m_game.getScene(), cuT( "ResultLevelValue" ) )->setCaption( castor::string::toU32String( m_game.getWave() ) );
		getTextOverlay( m_game.getScene(), cuT( "ResultKillsValue" ) )->setCaption( castor::string::toU32String( m_game.getKills() ) );
	}

	void Hud::update()
	{
		auto text = m_lives;

		if ( text )
		{
			text->setCaption( castor::string::toU32String( m_game.getLives() ) );
		}

		text = m_ore;

		if ( text )
		{
			text->setCaption( castor::string::toU32String( m_game.getOre() ) );
		}

		text = m_level;

		if ( text )
		{
			text->setCaption( castor::string::toU32String( m_game.getWave() ) );
		}

		text = m_kills;

		if ( text )
		{
			text->setCaption( castor::string::toU32String( m_game.getKills() ) );
		}

		text = m_enemyLife;

		if ( text )
		{
			text->setCaption( castor::string::toU32String( m_game.getEnemiesLife() ) );
		}

		text = m_enemyBounty;

		if ( text )
		{
			text->setCaption( castor::string::toU32String( m_game.getEnemiesBounty() ) );
		}

		if ( auto tower = m_game.getSelectedTower() )
		{
			text = m_towerDamage;

			if ( text )
			{
				text->setVisible( true );
				text->setCaption( castor::string::toU32String( tower->getDamage() ) );
			}

			text = m_towerSpeed;

			if ( text )
			{
				text->setVisible( true );
				text->setCaption( castor::string::toU32String( tower->getSpeed() ) );
			}

			text = m_towerRange;

			if ( text )
			{
				text->setVisible( true );
				text->setCaption( castor::string::toU32String( tower->getRange() ) );
			}
		}
		else
		{
			text = m_towerDamage;

			if ( text )
			{
				text->setVisible( false );
			}

			text = m_towerSpeed;

			if ( text )
			{
				text->setVisible( false );
			}

			text = m_towerRange;

			if ( text )
			{
				text->setVisible( false );
			}
		}
	}
}
