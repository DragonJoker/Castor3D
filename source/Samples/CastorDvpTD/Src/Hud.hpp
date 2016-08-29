#pragma once

#include "CastorDvpTDPrerequisites.hpp"

namespace castortd
{
	class Hud
	{
	public:
		Hud( Game const & p_game, Castor3D::Scene const & p_scene );
		~Hud();

		void Initialise();
		void Start();
		void Pause();
		void Resume();
		void Help();
		void GameOver();
		void Update();

	private:
		Game const & m_game;
		Castor3D::TextOverlayWPtr m_lives;
		Castor3D::TextOverlayWPtr m_ore;
		Castor3D::TextOverlayWPtr m_level;
		Castor3D::TextOverlayWPtr m_kills;
		Castor3D::TextOverlayWPtr m_enemyLife;
		Castor3D::TextOverlayWPtr m_enemyBounty;
		Castor3D::TextOverlayWPtr m_towerSpeed;
		Castor3D::TextOverlayWPtr m_towerRange;
		Castor3D::TextOverlayWPtr m_towerDamage;
	};
}
