/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include "GlRendererPrerequisites.hpp"

#include <Core/BackBuffer.hpp>

namespace gl_renderer
{
	/**
	*\brief
	*	Classe encapsulant une image venant d'une swap chain.
	*/
	class BackBuffer
		: public renderer::BackBuffer
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] device
		*	La connexion logique au GPU.
		*\param[in] swapchain
		*	La swapchain.
		*\param[in] imageIndex
		*	L'index de l'image, parmi les images de la swap chain.
		*/
		BackBuffer( renderer::Device const & device
			, renderer::TexturePtr && image
			, renderer::TextureViewPtr && view
			, uint32_t imageIndex );
	};
}
