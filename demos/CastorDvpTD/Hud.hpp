#pragma once

#include "CastorDvpTDPrerequisites.hpp"

namespace castortd
{
	class Hud
	{
	public:
		Hud( Game const & game
			, castor3d::Scene const & scene );

		void initialise();
		void start();
		void pause();
		void resume();
		void Help();
		void GameOver();
		void update();

	private:
		Game const & m_game;
		castor3d::TextOverlayRPtr m_lives{};
		castor3d::TextOverlayRPtr m_ore{};
		castor3d::TextOverlayRPtr m_level{};
		castor3d::TextOverlayRPtr m_kills{};
		castor3d::TextOverlayRPtr m_enemyLife{};
		castor3d::TextOverlayRPtr m_enemyBounty{};
		castor3d::TextOverlayRPtr m_towerSpeed{};
		castor3d::TextOverlayRPtr m_towerRange{};
		castor3d::TextOverlayRPtr m_towerDamage{};
	};
}
