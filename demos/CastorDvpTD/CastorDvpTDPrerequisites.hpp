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

	using EnemyPtr = c3d::SharedPtr< Enemy >;
	using TowerPtr = c3d::SharedPtr< Tower >;
	using TowerRPtr = Tower *;

	using TowerArray = c3d::Vector< TowerPtr >;
	using EnemyArray = c3d::Vector< EnemyPtr >;
	using BulletArray = c3d::Vector< Bullet >;
	using BoulderArray = c3d::Vector< Boulder >;

	using Clock = std::chrono::high_resolution_clock;

	wxString const ApplicationName = wxT( "CastorDvpTD" );

	template< typename T, typename ... Params >
	wxWindowPtr< T > wxMakeWindowPtr( Params && ... params )
	{
		return wxWindowPtr< T >( new T( c3d::forward< Params >( params )... ) );
	}
}
