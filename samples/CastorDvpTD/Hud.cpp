#include "CastorDvpTD/Hud.hpp"

#include "CastorDvpTD/Game.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/OverlayCache.hpp>
#include <Castor3D/Overlay/Overlay.hpp>
#include <Castor3D/Overlay/TextOverlay.hpp>
#include <Castor3D/Scene/Scene.hpp>

using namespace castor;
using namespace castor3d;

namespace castortd
{
	namespace
	{
		TextOverlaySPtr getTextOverlay( OverlayCache & cache, String const & p_name )
		{
			TextOverlaySPtr result;
			OverlaySPtr overlay = cache.find( p_name );

			if ( overlay )
			{
				result = overlay->getTextOverlay();
			}

			return result;
		}
	}

	Hud::Hud( Game const & p_game, castor3d::Scene const & p_scene )
		: m_game{ p_game }
		, m_lives{ getTextOverlay( p_scene.getEngine()->getOverlayCache(), cuT( "LivesValue" ) ) }
		, m_ore{ getTextOverlay( p_scene.getEngine()->getOverlayCache(), cuT( "OreValue" ) ) }
		, m_level{ getTextOverlay( p_scene.getEngine()->getOverlayCache(), cuT( "LevelValue" ) ) }
		, m_kills{ getTextOverlay( p_scene.getEngine()->getOverlayCache(), cuT( "KillsValue" ) ) }
		, m_enemyLife{ getTextOverlay( p_scene.getEngine()->getOverlayCache(), cuT( "MonsterLifeValue" ) ) }
		, m_enemyBounty{ getTextOverlay( p_scene.getEngine()->getOverlayCache(), cuT( "MonsterBountyValue" ) ) }
		, m_towerSpeed{ getTextOverlay( p_scene.getEngine()->getOverlayCache(), cuT( "TowerSpeedValue" ) ) }
		, m_towerRange{ getTextOverlay( p_scene.getEngine()->getOverlayCache(), cuT( "TowerRangeValue" ) ) }
		, m_towerDamage{ getTextOverlay( p_scene.getEngine()->getOverlayCache(), cuT( "TowerDamageValue" ) ) }
	{
	}

	Hud::~Hud()
	{
	}

	void Hud::initialise()
	{
		auto & cache = m_game.getScene().getEngine()->getOverlayCache();
		cache.find( cuT( "TitlePanel" ) )->setVisible( true );
		cache.find( cuT( "HUDResources" ) )->setVisible( false );
		cache.find( cuT( "HUDScore" ) )->setVisible( false );
		cache.find( cuT( "HUDDetails" ) )->setVisible( false );
		cache.find( cuT( "HUDPause" ) )->setVisible( false );
		cache.find( cuT( "GameEndPanel" ) )->setVisible( false );
		cache.find( cuT( "HelpPanel" ) )->setVisible( false );
	}

	void Hud::start()
	{
		auto & cache = m_game.getScene().getEngine()->getOverlayCache();
		cache.find( cuT( "TitlePanel" ) )->setVisible( false );
		cache.find( cuT( "HUDResources" ) )->setVisible( true );
		cache.find( cuT( "HUDScore" ) )->setVisible( true );
		cache.find( cuT( "HUDDetails" ) )->setVisible( true );
		cache.find( cuT( "HUDPause" ) )->setVisible( false );
		cache.find( cuT( "GameEndPanel" ) )->setVisible( false );
		cache.find( cuT( "HelpPanel" ) )->setVisible( false );
		update();
	}

	void Hud::pause()
	{
		auto & cache = m_game.getScene().getEngine()->getOverlayCache();
		cache.find( cuT( "TitlePanel" ) )->setVisible( false );
		cache.find( cuT( "HUDResources" ) )->setVisible( false );
		cache.find( cuT( "HUDScore" ) )->setVisible( false );
		cache.find( cuT( "HUDDetails" ) )->setVisible( false );
		cache.find( cuT( "HUDPause" ) )->setVisible( true );
		cache.find( cuT( "GameEndPanel" ) )->setVisible( false );
		cache.find( cuT( "HelpPanel" ) )->setVisible( false );
	}

	void Hud::resume()
	{
		auto & cache = m_game.getScene().getEngine()->getOverlayCache();
		cache.find( cuT( "TitlePanel" ) )->setVisible( false );
		cache.find( cuT( "HUDResources" ) )->setVisible( true );
		cache.find( cuT( "HUDScore" ) )->setVisible( true );
		cache.find( cuT( "HUDDetails" ) )->setVisible( true );
		cache.find( cuT( "HUDPause" ) )->setVisible( false );
		cache.find( cuT( "GameEndPanel" ) )->setVisible( false );
		cache.find( cuT( "HelpPanel" ) )->setVisible( false );
	}

	void Hud::Help()
	{
		auto & cache = m_game.getScene().getEngine()->getOverlayCache();
		cache.find( cuT( "TitlePanel" ) )->setVisible( false );
		cache.find( cuT( "HUDResources" ) )->setVisible( false );
		cache.find( cuT( "HUDScore" ) )->setVisible( false );
		cache.find( cuT( "HUDDetails" ) )->setVisible( false );
		cache.find( cuT( "HUDPause" ) )->setVisible( false );
		cache.find( cuT( "GameEndPanel" ) )->setVisible( false );
		cache.find( cuT( "HelpPanel" ) )->setVisible( true );
	}

	void Hud::GameOver()
	{
		auto & cache = m_game.getScene().getEngine()->getOverlayCache();
		cache.find( cuT( "TitlePanel" ) )->setVisible( false );
		cache.find( cuT( "HUDResources" ) )->setVisible( false );
		cache.find( cuT( "HUDScore" ) )->setVisible( false );
		cache.find( cuT( "HUDDetails" ) )->setVisible( false );
		cache.find( cuT( "HUDPause" ) )->setVisible( false );
		cache.find( cuT( "GameEndPanel" ) )->setVisible( true );
		cache.find( cuT( "HelpPanel" ) )->setVisible( false );

		getTextOverlay( m_game.getScene().getEngine()->getOverlayCache(), cuT( "ResultLevelValue" ) )->setCaption( StringStream() << m_game.getWave() );
		getTextOverlay( m_game.getScene().getEngine()->getOverlayCache(), cuT( "ResultKillsValue" ) )->setCaption( StringStream() << m_game.getKills() );
	}

	void Hud::update()
	{
		auto text = m_lives.lock();

		if ( text )
		{
			text->setCaption( StringStream() << m_game.getLives() );
		}

		text = m_ore.lock();

		if ( text )
		{
			text->setCaption( StringStream() << m_game.getOre() );
		}

		text = m_level.lock();

		if ( text )
		{
			text->setCaption( StringStream() << m_game.getWave() );
		}

		text = m_kills.lock();

		if ( text )
		{
			text->setCaption( StringStream() << m_game.getKills() );
		}

		text = m_enemyLife.lock();

		if ( text )
		{
			text->setCaption( StringStream() << m_game.getEnemiesLife() );
		}

		text = m_enemyBounty.lock();

		if ( text )
		{
			text->setCaption( StringStream() << m_game.getEnemiesBounty() );
		}

		TowerPtr tower = m_game.getSelectedTower();

		if ( tower )
		{
			text = m_towerDamage.lock();

			if ( text )
			{
				text->setVisible( true );
				text->setCaption( StringStream() << tower->getDamage() );
			}

			text = m_towerSpeed.lock();

			if ( text )
			{
				text->setVisible( true );
				text->setCaption( StringStream() << tower->getSpeed() );
			}

			text = m_towerRange.lock();

			if ( text )
			{
				text->setVisible( true );
				text->setCaption( StringStream() << tower->getRange() );
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
