#pragma once

#include <Prerequisites.hpp>

namespace vkapp
{
	/**
	*\~french
	*\brief
	*	Les donn�es d'un sommet.
	*\~english
	*\brief
	*	The data for one vertex.
	*/
	struct VertexData
	{
		renderer::Vec4 position;
		renderer::Vec4 colour;
	};

	static wxString const AppName = wxT( "03-VertexLayout" );

	class RenderPanel;
	class MainFrame;
	class Application;
}
