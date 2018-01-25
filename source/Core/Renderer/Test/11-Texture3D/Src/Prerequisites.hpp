#pragma once

#include <Prerequisites.hpp>

namespace vkapp
{
	/**
	*\~french
	*\brief
	*	Les données d'un sommet texturé.
	*\~english
	*\brief
	*	The data for one textured vertex.
	*/
	struct UVVertexData
	{
		renderer::Vec4 position;
		renderer::Vec2 uv;
	};
	/**
	*\~french
	*\brief
	*	Les données d'un sommet texturé.
	*\~english
	*\brief
	*	The data for one textured vertex.
	*/
	struct UVWVertexData
	{
		renderer::Vec4 position;
		renderer::Vec3 uvw;
	};

	static wxString const AppName = wxT( "11-Texture3D" );

	class RenderPanel;
	class MainFrame;
	class Application;
}
