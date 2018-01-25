#pragma once

#include "Prerequisites.hpp"

#include <Application.hpp>

namespace vkapp
{
	/**
	*\~french
	*\brief
	*	L'application.
	*\~english
	*\brief
	*	The application.
	*/
	class Application
		: public common::App
	{
	public:
		Application();

	private:
		common::MainFrame * doCreateMainFrame( wxString const & rendererName )override;
	};
}
/**
*\~french
*\brief
*	D�clare la fonction wxGetApp() permettant d'acc�der � l'application
*	depuis partout.
*\~english
*\brief
*	Declares wxGetApp() function, which gives access to the application
*	from everywhere.
*/
wxDECLARE_APP( vkapp::Application );
