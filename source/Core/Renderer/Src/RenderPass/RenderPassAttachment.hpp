/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_TextureAttachment_HPP___
#define ___Renderer_TextureAttachment_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	An attach to a render pass.
	*\~french
	*\brief
	*	Une attache à une passe de rendu.
	*/
	struct RenderPassAttachment
	{
		/**
		*\~english
		*\brief
		*	The attach pixel format.
		*\~french
		*\brief
		*	Le format des pixels de l'attache.
		*/
		PixelFormat pixelFormat;
		/**
		*\~english
		*\brief
		*	Tells if the attach is cleared when the render pass is started.
		*\~french
		*\brief
		*	Dit si l'attache est vidée lors du démarrage de la passe de rendu.
		*/
		bool clear;
	};
}

#endif
