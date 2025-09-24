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
		c3d::TextOverlayRPtr getTextOverlay( c3d::Scene const & scene
			, c3d::String const & name )
		{
			c3d::TextOverlayRPtr result{};

			if ( auto overlay = scene.findOverlay( name ) )
			{
				result = overlay->getTextOverlay();
			}

			return result;
		}
	}

	Hud::Hud( Game const & game, c3d::Scene const & scene )
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

	void Hud::initialise()const
	{
		auto const & scene = m_game.getScene();
		scene.findOverlay( cuT( "TitlePanel" ) )->setVisible( true );
		scene.findOverlay( cuT( "HUDResources" ) )->setVisible( false );
		scene.findOverlay( cuT( "HUDScore" ) )->setVisible( false );
		scene.findOverlay( cuT( "HUDDetails" ) )->setVisible( false );
		scene.findOverlay( cuT( "HUDPause" ) )->setVisible( false );
		scene.findOverlay( cuT( "GameEndPanel" ) )->setVisible( false );
		scene.findOverlay( cuT( "HelpPanel" ) )->setVisible( false );
	}

	void Hud::start()const
	{
		auto const & scene = m_game.getScene();
		scene.findOverlay( cuT( "TitlePanel" ) )->setVisible( false );
		scene.findOverlay( cuT( "HUDResources" ) )->setVisible( true );
		scene.findOverlay( cuT( "HUDScore" ) )->setVisible( true );
		scene.findOverlay( cuT( "HUDDetails" ) )->setVisible( true );
		scene.findOverlay( cuT( "HUDPause" ) )->setVisible( false );
		scene.findOverlay( cuT( "GameEndPanel" ) )->setVisible( false );
		scene.findOverlay( cuT( "HelpPanel" ) )->setVisible( false );
		update();
	}

	void Hud::pause()const
	{
		auto const & scene = m_game.getScene();
		scene.findOverlay( cuT( "TitlePanel" ) )->setVisible( false );
		scene.findOverlay( cuT( "HUDResources" ) )->setVisible( false );
		scene.findOverlay( cuT( "HUDScore" ) )->setVisible( false );
		scene.findOverlay( cuT( "HUDDetails" ) )->setVisible( false );
		scene.findOverlay( cuT( "HUDPause" ) )->setVisible( true );
		scene.findOverlay( cuT( "GameEndPanel" ) )->setVisible( false );
		scene.findOverlay( cuT( "HelpPanel" ) )->setVisible( false );
	}

	void Hud::resume()const
	{
		auto const & scene = m_game.getScene();
		scene.findOverlay( cuT( "TitlePanel" ) )->setVisible( false );
		scene.findOverlay( cuT( "HUDResources" ) )->setVisible( true );
		scene.findOverlay( cuT( "HUDScore" ) )->setVisible( true );
		scene.findOverlay( cuT( "HUDDetails" ) )->setVisible( true );
		scene.findOverlay( cuT( "HUDPause" ) )->setVisible( false );
		scene.findOverlay( cuT( "GameEndPanel" ) )->setVisible( false );
		scene.findOverlay( cuT( "HelpPanel" ) )->setVisible( false );
	}

	void Hud::Help()const
	{
		auto const & scene = m_game.getScene();
		scene.findOverlay( cuT( "TitlePanel" ) )->setVisible( false );
		scene.findOverlay( cuT( "HUDResources" ) )->setVisible( false );
		scene.findOverlay( cuT( "HUDScore" ) )->setVisible( false );
		scene.findOverlay( cuT( "HUDDetails" ) )->setVisible( false );
		scene.findOverlay( cuT( "HUDPause" ) )->setVisible( false );
		scene.findOverlay( cuT( "GameEndPanel" ) )->setVisible( false );
		scene.findOverlay( cuT( "HelpPanel" ) )->setVisible( true );
	}

	void Hud::GameOver()const
	{
		auto const & scene = m_game.getScene();
		scene.findOverlay( cuT( "TitlePanel" ) )->setVisible( false );
		scene.findOverlay( cuT( "HUDResources" ) )->setVisible( false );
		scene.findOverlay( cuT( "HUDScore" ) )->setVisible( false );
		scene.findOverlay( cuT( "HUDDetails" ) )->setVisible( false );
		scene.findOverlay( cuT( "HUDPause" ) )->setVisible( false );
		scene.findOverlay( cuT( "GameEndPanel" ) )->setVisible( true );
		scene.findOverlay( cuT( "HelpPanel" ) )->setVisible( false );

		getTextOverlay( m_game.getScene(), cuT( "ResultLevelValue" ) )->setCaption( c3d::toUtf8U32String( c3d::string::toString( m_game.getWave() ) ) );
		getTextOverlay( m_game.getScene(), cuT( "ResultKillsValue" ) )->setCaption( c3d::toUtf8U32String( c3d::string::toString( m_game.getKills() ) ) );
	}

	void Hud::update()const
	{
		if ( auto text = m_lives )
			text->setCaption( c3d::toUtf8U32String( c3d::string::toString( m_game.getLives() ) ) );

		if ( auto text = m_ore )
			text->setCaption( c3d::toUtf8U32String( c3d::string::toString( m_game.getOre() ) ) );

		if ( auto text = m_level )
			text->setCaption( c3d::toUtf8U32String( c3d::string::toString( m_game.getWave() ) ) );

		if ( auto text = m_kills )
			text->setCaption( c3d::toUtf8U32String( c3d::string::toString( m_game.getKills() ) ) );

		if ( auto text = m_enemyLife )
			text->setCaption( c3d::toUtf8U32String( c3d::string::toString( m_game.getEnemiesLife() ) ) );

		if ( auto text = m_enemyBounty )
			text->setCaption( c3d::toUtf8U32String( c3d::string::toString( m_game.getEnemiesBounty() ) ) );

		if ( auto tower = m_game.getSelectedTower() )
		{
			if ( auto text = m_towerDamage )
			{
				text->setVisible( true );
				text->setCaption( c3d::toUtf8U32String( c3d::string::toString( tower->getDamage() ) ) );
			}

			if ( auto text = m_towerSpeed )
			{
				text->setVisible( true );
				text->setCaption( c3d::toUtf8U32String( c3d::string::toString( tower->getSpeed() ) ) );
			}

			if ( auto text = m_towerRange )
			{
				text->setVisible( true );
				text->setCaption( c3d::toUtf8U32String( c3d::string::toString( tower->getRange() ) ) );
			}
		}
		else
		{
			if ( auto text = m_towerDamage )
				text->setVisible( false );

			if ( auto text = m_towerSpeed )
				text->setVisible( false );

			if ( auto text = m_towerRange )
				text->setVisible( false );
		}
	}
}
