#pragma once

#include <Castor3DPrerequisites.hpp>
#include <GuiCommonPrerequisites.hpp>

#include <wx/wx.h>
#include <wx/windowptr.h>

namespace castortd
{
	class CastorDvpTD;
	class MainFrame;
	class RenderPanel;

	wxString const ApplicationName = wxT( "CastorDvpTD" );

	template< typename T, typename ... Params >
	inline wxWindowPtr< T > wxMakeWindowPtr( Params && ... p_params )
	{
		return wxWindowPtr< T >( new T( std::forward< Params >( p_params )... ) );
	}
}
