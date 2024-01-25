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

	using EnemyPtr = castor::SharedPtr< Enemy >;
	using TowerPtr = castor::SharedPtr< Tower >;
	using TowerRPtr = Tower *;

	using TowerArray = castor::Vector< TowerPtr >;
	using EnemyArray = castor::Vector< EnemyPtr >;
	using BulletArray = castor::Vector< Bullet >;
	using BoulderArray = castor::Vector< Boulder >;

	using Clock = std::chrono::high_resolution_clock;

	wxString const ApplicationName = wxT( "CastorDvpTD" );

	template< typename T, typename ... Params >
	wxWindowPtr< T > wxMakeWindowPtr( Params && ... params )
	{
		return wxWindowPtr< T >( new T( castor::forward< Params >( params )... ) );
	}
}
