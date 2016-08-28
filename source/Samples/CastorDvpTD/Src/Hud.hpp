#pragma once

#include "CastorDvpTDPrerequisites.hpp"

namespace castortd
{
	class Hud
	{
	public:
		Hud( Game const & p_game );
		~Hud();

		void Initialise();
		void Start();
		void Update();
		void GameOver();

	private:
		Game const & m_game;
		Castor3D::TextOverlayWPtr m_lives;
		Castor3D::TextOverlayWPtr m_ore;
		Castor3D::TextOverlayWPtr m_level;
		Castor3D::TextOverlayWPtr m_kills;
		Castor3D::TextOverlayWPtr m_enemyLife;
		Castor3D::TextOverlayWPtr m_towerSpeed;
		Castor3D::TextOverlayWPtr m_towerRange;
		Castor3D::TextOverlayWPtr m_towerDamage;
	};
}
