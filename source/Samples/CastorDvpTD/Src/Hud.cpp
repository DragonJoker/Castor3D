#include "Hud.hpp"

#include "Game.hpp"

#include <OverlayCache.hpp>
#include <Overlay/Overlay.hpp>
#include <Overlay/TextOverlay.hpp>
#include <Scene/Scene.hpp>

using namespace Castor;
using namespace Castor3D;

namespace castortd
{
	namespace
	{
		TextOverlaySPtr GetTextOverlay( OverlayCache & p_cache, String const & p_name )
		{
			TextOverlaySPtr l_return;
			OverlaySPtr l_overlay = p_cache.Find( p_name );

			if ( l_overlay )
			{
				l_return = l_overlay->GetTextOverlay();
			}

			return l_return;
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
		auto & l_cache = m_game.GetScene().GetEngine()->GetOverlayCache();
		l_cache.Find( cuT( "TitlePanel" ) )->SetVisible( true );
		l_cache.Find( cuT( "HUDResources" ) )->SetVisible( false );
		l_cache.Find( cuT( "HUDScore" ) )->SetVisible( false );
		l_cache.Find( cuT( "HUDDetails" ) )->SetVisible( false );
		l_cache.Find( cuT( "HUDPause" ) )->SetVisible( false );
		l_cache.Find( cuT( "GameEndPanel" ) )->SetVisible( false );
		l_cache.Find( cuT( "HelpPanel" ) )->SetVisible( false );
	}
	
	void Hud::Start()
	{
		auto & l_cache = m_game.GetScene().GetEngine()->GetOverlayCache();
		l_cache.Find( cuT( "TitlePanel" ) )->SetVisible( false );
		l_cache.Find( cuT( "HUDResources" ) )->SetVisible( true );
		l_cache.Find( cuT( "HUDScore" ) )->SetVisible( true );
		l_cache.Find( cuT( "HUDDetails" ) )->SetVisible( true );
		l_cache.Find( cuT( "HUDPause" ) )->SetVisible( false );
		l_cache.Find( cuT( "GameEndPanel" ) )->SetVisible( false );
		l_cache.Find( cuT( "HelpPanel" ) )->SetVisible( false );
		Update();
	}

	void Hud::Pause()
	{
		auto & l_cache = m_game.GetScene().GetEngine()->GetOverlayCache();
		l_cache.Find( cuT( "TitlePanel" ) )->SetVisible( false );
		l_cache.Find( cuT( "HUDResources" ) )->SetVisible( false );
		l_cache.Find( cuT( "HUDScore" ) )->SetVisible( false );
		l_cache.Find( cuT( "HUDDetails" ) )->SetVisible( false );
		l_cache.Find( cuT( "HUDPause" ) )->SetVisible( true );
		l_cache.Find( cuT( "GameEndPanel" ) )->SetVisible( false );
		l_cache.Find( cuT( "HelpPanel" ) )->SetVisible( false );
	}

	void Hud::Resume()
	{
		auto & l_cache = m_game.GetScene().GetEngine()->GetOverlayCache();
		l_cache.Find( cuT( "TitlePanel" ) )->SetVisible( false );
		l_cache.Find( cuT( "HUDResources" ) )->SetVisible( true );
		l_cache.Find( cuT( "HUDScore" ) )->SetVisible( true );
		l_cache.Find( cuT( "HUDDetails" ) )->SetVisible( true );
		l_cache.Find( cuT( "HUDPause" ) )->SetVisible( false );
		l_cache.Find( cuT( "GameEndPanel" ) )->SetVisible( false );
		l_cache.Find( cuT( "HelpPanel" ) )->SetVisible( false );
	}

	void Hud::Help()
	{
		auto & l_cache = m_game.GetScene().GetEngine()->GetOverlayCache();
		l_cache.Find( cuT( "TitlePanel" ) )->SetVisible( false );
		l_cache.Find( cuT( "HUDResources" ) )->SetVisible( false );
		l_cache.Find( cuT( "HUDScore" ) )->SetVisible( false );
		l_cache.Find( cuT( "HUDDetails" ) )->SetVisible( false );
		l_cache.Find( cuT( "HUDPause" ) )->SetVisible( false );
		l_cache.Find( cuT( "GameEndPanel" ) )->SetVisible( false );
		l_cache.Find( cuT( "HelpPanel" ) )->SetVisible( true );
	}

	void Hud::GameOver()
	{
		auto & l_cache = m_game.GetScene().GetEngine()->GetOverlayCache();
		l_cache.Find( cuT( "TitlePanel" ) )->SetVisible( false );
		l_cache.Find( cuT( "HUDResources" ) )->SetVisible( false );
		l_cache.Find( cuT( "HUDScore" ) )->SetVisible( false );
		l_cache.Find( cuT( "HUDDetails" ) )->SetVisible( false );
		l_cache.Find( cuT( "HUDPause" ) )->SetVisible( false );
		l_cache.Find( cuT( "GameEndPanel" ) )->SetVisible( true );
		l_cache.Find( cuT( "HelpPanel" ) )->SetVisible( false );

		GetTextOverlay( m_game.GetScene().GetEngine()->GetOverlayCache(), cuT( "ResultLevelValue" ) )->SetCaption( StringStream() << m_game.GetWave() );
		GetTextOverlay( m_game.GetScene().GetEngine()->GetOverlayCache(), cuT( "ResultKillsValue" ) )->SetCaption( StringStream() << m_game.GetKills() );
	}

	void Hud::Update()
	{
		auto l_text = m_lives.lock();

		if ( l_text )
		{
			l_text->SetCaption( StringStream() << m_game.GetLives() );
		}

		l_text = m_ore.lock();

		if ( l_text )
		{
			l_text->SetCaption( StringStream() << m_game.GetOre() );
		}

		l_text = m_level.lock();

		if ( l_text )
		{
			l_text->SetCaption( StringStream() << m_game.GetWave() );
		}

		l_text = m_kills.lock();

		if ( l_text )
		{
			l_text->SetCaption( StringStream() << m_game.GetKills() );
		}

		l_text = m_enemyLife.lock();

		if ( l_text )
		{
			l_text->SetCaption( StringStream() << m_game.GetEnemiesLife() );
		}

		l_text = m_enemyBounty.lock();

		if ( l_text )
		{
			l_text->SetCaption( StringStream() << m_game.GetEnemiesBounty() );
		}

		TowerPtr l_tower = m_game.GetSelectedTower();

		if ( l_tower )
		{
			l_text = m_towerDamage.lock();

			if ( l_text )
			{
				l_text->SetVisible( true );
				l_text->SetCaption( StringStream() << l_tower->GetDamage() );
			}

			l_text = m_towerSpeed.lock();

			if ( l_text )
			{
				l_text->SetVisible( true );
				l_text->SetCaption( StringStream() << l_tower->GetSpeed() );
			}

			l_text = m_towerRange.lock();

			if ( l_text )
			{
				l_text->SetVisible( true );
				l_text->SetCaption( StringStream() << l_tower->GetRange() );
			}
		}
		else
		{
			l_text = m_towerDamage.lock();

			if ( l_text )
			{
				l_text->SetVisible( false );
			}

			l_text = m_towerSpeed.lock();

			if ( l_text )
			{
				l_text->SetVisible( false );
			}

			l_text = m_towerRange.lock();

			if ( l_text )
			{
				l_text->SetVisible( false );
			}
		}
	}
}
