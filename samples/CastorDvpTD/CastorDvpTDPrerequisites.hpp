#pragma once

#include <Castor3D/Castor3DPrerequisites.hpp>
#include <GuiCommon/GuiCommonPrerequisites.hpp>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wambiguous-reversed-operator"
#pragma clang diagnostic ignored "-Watomic-implicit-seq-cst"
#pragma clang diagnostic ignored "-Wcast-function-type"
#pragma clang diagnostic ignored "-Wcomma"
#pragma clang diagnostic ignored "-Wdeprecated-copy-with-user-provided-copy"
#pragma clang diagnostic ignored "-Wdeprecated-copy-with-user-provided-dtor"
#pragma clang diagnostic ignored "-Wimplicit-int-float-conversion"
#pragma clang diagnostic ignored "-Winfinite-recursion"
#pragma clang diagnostic ignored "-Wreserved-identifier"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wdeprecated-copy"
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wredundant-decls"
#pragma GCC diagnostic ignored "-Wsuggest-destructor-override"
#pragma GCC diagnostic ignored "-Wsuggest-override"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#include <wx/wx.h>
#include <wx/windowptr.h>
#pragma GCC diagnostic pop
#pragma clang diagnostic pop

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
	using TowerWPtr = std::weak_ptr< Tower >;

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
