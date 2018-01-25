#include "Application.hpp"

#include "MainFrame.hpp"

wxIMPLEMENT_APP( vkapp::Application );

namespace vkapp
{
	Application::Application()
		: common::App{ AppName }
	{
	}

	common::MainFrame * Application::doCreateMainFrame( wxString const & rendererName )
	{
		return new MainFrame{ rendererName, m_factory };
	}
};
