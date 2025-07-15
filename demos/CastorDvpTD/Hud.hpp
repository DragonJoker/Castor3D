#pragma once

#include "CastorDvpTDPrerequisites.hpp"

namespace castortd
{
	class Hud
	{
	public:
		Hud( Game const & game
			, c3d::Scene const & scene );

		void initialise();
		void start();
		void pause();
		void resume();
		void Help();
		void GameOver();
		void update();

	private:
		Game const & m_game;
		c3d::TextOverlayRPtr m_lives{};
		c3d::TextOverlayRPtr m_ore{};
		c3d::TextOverlayRPtr m_level{};
		c3d::TextOverlayRPtr m_kills{};
		c3d::TextOverlayRPtr m_enemyLife{};
		c3d::TextOverlayRPtr m_enemyBounty{};
		c3d::TextOverlayRPtr m_towerSpeed{};
		c3d::TextOverlayRPtr m_towerRange{};
		c3d::TextOverlayRPtr m_towerDamage{};
	};
}
