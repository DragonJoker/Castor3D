#pragma once

#include <Castor3D/Castor3DPrerequisites.hpp>
#include <GuiCommon/GuiCommonPrerequisites.hpp>

#include <wx/wx.h>
#include <wx/windowptr.h>

#include <chrono>
#include <vector>

namespace castortd
{
	class CastorDvpTD;
	class MainFrame;
	class RenderPanel;

	struct Cell;
	class Grid;
	class Game;
	class Tower;
	class Enemy;
	class Bullet;

	using EnemyPtr = std::shared_ptr< Enemy >;
	using TowerPtr = std::shared_ptr< Tower >;
	using TowerWPtr = std::weak_ptr< Tower >;

	using TowerArray = std::vector< TowerPtr >;
	using EnemyArray = std::vector< EnemyPtr >;
	using BulletArray = std::vector< Bullet >;

	using Clock = std::chrono::high_resolution_clock;

	wxString const ApplicationName = wxT( "CastorDvpTD" );

	template< typename T, typename ... Params >
	inline wxWindowPtr< T > wxMakeWindowPtr( Params && ... p_params )
	{
		return wxWindowPtr< T >( new T( std::forward< Params >( p_params )... ) );
	}
}
