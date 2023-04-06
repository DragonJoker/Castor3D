#pragma once

#include <GuiCommon/GuiCommonPrerequisites.hpp>

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <wx/wx.h>
#include <wx/windowptr.h>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

#include <Castor3D/Castor3DPrerequisites.hpp>

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
	class Boulder;

	using EnemyPtr = std::shared_ptr< Enemy >;
	using TowerPtr = std::shared_ptr< Tower >;
	using TowerRPtr = Tower *;

	using TowerArray = std::vector< TowerPtr >;
	using EnemyArray = std::vector< EnemyPtr >;
	using BulletArray = std::vector< Bullet >;
	using BoulderArray = std::vector< Boulder >;

	using Clock = std::chrono::high_resolution_clock;

	wxString const ApplicationName = wxT( "CastorDvpTD" );

	template< typename T, typename ... Params >
	wxWindowPtr< T > wxMakeWindowPtr( Params && ... params )
	{
		return wxWindowPtr< T >( new T( std::forward< Params >( params )... ) );
	}
}
