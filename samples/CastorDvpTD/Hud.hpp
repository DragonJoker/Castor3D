#pragma once

#include "CastorDvpTDPrerequisites.hpp"

namespace castortd
{
	class Hud
	{
	public:
		Hud( Game const & p_game, castor3d::Scene const & p_scene );

		void initialise();
		void start();
		void pause();
		void resume();
		void Help();
		void GameOver();
		void update();

	private:
		Game const & m_game;
		castor3d::TextOverlayWPtr m_lives;
		castor3d::TextOverlayWPtr m_ore;
		castor3d::TextOverlayWPtr m_level;
		castor3d::TextOverlayWPtr m_kills;
		castor3d::TextOverlayWPtr m_enemyLife;
		castor3d::TextOverlayWPtr m_enemyBounty;
		castor3d::TextOverlayWPtr m_towerSpeed;
		castor3d::TextOverlayWPtr m_towerRange;
		castor3d::TextOverlayWPtr m_towerDamage;
	};
}
