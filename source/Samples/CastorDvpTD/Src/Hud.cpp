#include "Hud.hpp"

#include "Game.hpp"

#include <Cache/OverlayCache.hpp>
#include <Overlay/Overlay.hpp>
#include <Overlay/TextOverlay.hpp>
#include <Scene/Scene.hpp>

using namespace Castor;
using namespace Castor3D;

namespace castortd
{
	namespace
	{
		TextOverlaySPtr GetTextOverlay( OverlayCache & cache, String const & p_name )
		{
			TextOverlaySPtr result;
			OverlaySPtr overlay = cache.Find( p_name );

			if ( overlay )
			{
				result = overlay->GetTextOverlay();
			}

			return result;
		}
	}

	Hud::Hud( Game const & p_game, Castor3D::Scene const & p_scene )
		: m_game{ p_game }
		, m_lives{ GetTextOverlay( p_scene.GetEngine()->GetOverlayCache(), cuT( "LivesValue" ) ) }
		, m_ore{ GetTextOverlay( p_scene.GetEngine()->GetOverlayCache(), cuT( "OreValue" ) ) }
		, m_level{ GetTextOverlay( p_scene.GetEngine()->GetOverlayCache(), cuT( "LevelValue" ) ) }
		, m_kills{ GetTextOverlay( p_scene.GetEngine()->GetOverlayCache(), cuT( "KillsValue" ) ) }
		, m_enemyLife{ GetTextOverlay( p_scene.GetEngine()->GetOverlayCache(), cuT( "MonsterLifeValue" ) ) }
		, m_enemyBounty{ GetTextOverlay( p_scene.GetEngine()->GetOverlayCache(), cuT( "MonsterBountyValue" ) ) }
		, m_towerSpeed{ GetTextOverlay( p_scene.GetEngine()->GetOverlayCache(), cuT( "TowerSpeedValue" ) ) }
		, m_towerRange{ GetTextOverlay( p_scene.GetEngine()->GetOverlayCache(), cuT( "TowerRangeValue" ) ) }
		, m_towerDamage{ GetTextOverlay( p_scene.GetEngine()->GetOverlayCache(), cuT( "TowerDamageValue" ) ) }
	{
	}

	Hud::~Hud()
	{
	}

	void Hud::Initialise()
	{
		auto & cache = m_game.GetScene().GetEngine()->GetOverlayCache();
		cache.Find( cuT( "TitlePanel" ) )->SetVisible( true );
		cache.Find( cuT( "HUDResources" ) )->SetVisible( false );
		cache.Find( cuT( "HUDScore" ) )->SetVisible( false );
		cache.Find( cuT( "HUDDetails" ) )->SetVisible( false );
		cache.Find( cuT( "HUDPause" ) )->SetVisible( false );
		cache.Find( cuT( "GameEndPanel" ) )->SetVisible( false );
		cache.Find( cuT( "HelpPanel" ) )->SetVisible( false );
	}

	void Hud::Start()
	{
		auto & cache = m_game.GetScene().GetEngine()->GetOverlayCache();
		cache.Find( cuT( "TitlePanel" ) )->SetVisible( false );
		cache.Find( cuT( "HUDResources" ) )->SetVisible( true );
		cache.Find( cuT( "HUDScore" ) )->SetVisible( true );
		cache.Find( cuT( "HUDDetails" ) )->SetVisible( true );
		cache.Find( cuT( "HUDPause" ) )->SetVisible( false );
		cache.Find( cuT( "GameEndPanel" ) )->SetVisible( false );
		cache.Find( cuT( "HelpPanel" ) )->SetVisible( false );
		Update();
	}

	void Hud::Pause()
	{
		auto & cache = m_game.GetScene().GetEngine()->GetOverlayCache();
		cache.Find( cuT( "TitlePanel" ) )->SetVisible( false );
		cache.Find( cuT( "HUDResources" ) )->SetVisible( false );
		cache.Find( cuT( "HUDScore" ) )->SetVisible( false );
		cache.Find( cuT( "HUDDetails" ) )->SetVisible( false );
		cache.Find( cuT( "HUDPause" ) )->SetVisible( true );
		cache.Find( cuT( "GameEndPanel" ) )->SetVisible( false );
		cache.Find( cuT( "HelpPanel" ) )->SetVisible( false );
	}

	void Hud::Resume()
	{
		auto & cache = m_game.GetScene().GetEngine()->GetOverlayCache();
		cache.Find( cuT( "TitlePanel" ) )->SetVisible( false );
		cache.Find( cuT( "HUDResources" ) )->SetVisible( true );
		cache.Find( cuT( "HUDScore" ) )->SetVisible( true );
		cache.Find( cuT( "HUDDetails" ) )->SetVisible( true );
		cache.Find( cuT( "HUDPause" ) )->SetVisible( false );
		cache.Find( cuT( "GameEndPanel" ) )->SetVisible( false );
		cache.Find( cuT( "HelpPanel" ) )->SetVisible( false );
	}

	void Hud::Help()
	{
		auto & cache = m_game.GetScene().GetEngine()->GetOverlayCache();
		cache.Find( cuT( "TitlePanel" ) )->SetVisible( false );
		cache.Find( cuT( "HUDResources" ) )->SetVisible( false );
		cache.Find( cuT( "HUDScore" ) )->SetVisible( false );
		cache.Find( cuT( "HUDDetails" ) )->SetVisible( false );
		cache.Find( cuT( "HUDPause" ) )->SetVisible( false );
		cache.Find( cuT( "GameEndPanel" ) )->SetVisible( false );
		cache.Find( cuT( "HelpPanel" ) )->SetVisible( true );
	}

	void Hud::GameOver()
	{
		auto & cache = m_game.GetScene().GetEngine()->GetOverlayCache();
		cache.Find( cuT( "TitlePanel" ) )->SetVisible( false );
		cache.Find( cuT( "HUDResources" ) )->SetVisible( false );
		cache.Find( cuT( "HUDScore" ) )->SetVisible( false );
		cache.Find( cuT( "HUDDetails" ) )->SetVisible( false );
		cache.Find( cuT( "HUDPause" ) )->SetVisible( false );
		cache.Find( cuT( "GameEndPanel" ) )->SetVisible( true );
		cache.Find( cuT( "HelpPanel" ) )->SetVisible( false );

		GetTextOverlay( m_game.GetScene().GetEngine()->GetOverlayCache(), cuT( "ResultLevelValue" ) )->SetCaption( StringStream() << m_game.GetWave() );
		GetTextOverlay( m_game.GetScene().GetEngine()->GetOverlayCache(), cuT( "ResultKillsValue" ) )->SetCaption( StringStream() << m_game.GetKills() );
	}

	void Hud::Update()
	{
		auto text = m_lives.lock();

		if ( text )
		{
			text->SetCaption( StringStream() << m_game.GetLives() );
		}

		text = m_ore.lock();

		if ( text )
		{
			text->SetCaption( StringStream() << m_game.GetOre() );
		}

		text = m_level.lock();

		if ( text )
		{
			text->SetCaption( StringStream() << m_game.GetWave() );
		}

		text = m_kills.lock();

		if ( text )
		{
			text->SetCaption( StringStream() << m_game.GetKills() );
		}

		text = m_enemyLife.lock();

		if ( text )
		{
			text->SetCaption( StringStream() << m_game.GetEnemiesLife() );
		}

		text = m_enemyBounty.lock();

		if ( text )
		{
			text->SetCaption( StringStream() << m_game.GetEnemiesBounty() );
		}

		TowerPtr tower = m_game.GetSelectedTower();

		if ( tower )
		{
			text = m_towerDamage.lock();

			if ( text )
			{
				text->SetVisible( true );
				text->SetCaption( StringStream() << tower->GetDamage() );
			}

			text = m_towerSpeed.lock();

			if ( text )
			{
				text->SetVisible( true );
				text->SetCaption( StringStream() << tower->GetSpeed() );
			}

			text = m_towerRange.lock();

			if ( text )
			{
				text->SetVisible( true );
				text->SetCaption( StringStream() << tower->GetRange() );
			}
		}
		else
		{
			text = m_towerDamage.lock();

			if ( text )
			{
				text->SetVisible( false );
			}

			text = m_towerSpeed.lock();

			if ( text )
			{
				text->SetVisible( false );
			}

			text = m_towerRange.lock();

			if ( text )
			{
				text->SetVisible( false );
			}
		}
	}
}
