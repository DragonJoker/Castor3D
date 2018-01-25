#include "MainFrame.hpp"

#include "RenderPanel.hpp"

namespace vkapp
{
	MainFrame::MainFrame( wxString const & rendererName
		, common::RendererFactory & factory )
		: common::MainFrame{ AppName, rendererName, factory }
	{
	}

	wxPanel * MainFrame::doCreatePanel( wxSize const & size, renderer::Renderer const & renderer )
	{
		return new RenderPanel( this, size, renderer );
	}
}
