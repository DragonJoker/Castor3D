/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_PixelFormat_HPP___
#define ___Renderer_PixelFormat_HPP___
#pragma once

#include "Enum/ImageAspectFlag.hpp"

namespace renderer
{
	/**
	*\~french
	*\brief
	*	Dit si le renderer::PixelFormat donné est un format utilisable pour les tampons de profondeur et stencil.
	*\param[in] format
	*	Le renderer::PixelFormat à tester.
	*\return
	*	\p true s'il l'est...
	*\~english
	*\brief
	*	Tells if the given renderer::PixelFormat is usable in depth and stencil buffers.
	*\param[in] format
	*	The renderer::PixelFormat.
	*\return
	*	\p true if it is usable in depth and stencil buffers.
	*/
	bool isDepthStencilFormat( renderer::PixelFormat format );
	/**
	*\~french
	*\brief
	*	Dit si le renderer::PixelFormat donné est un format utilisable pour les tampons de stencil.
	*\param[in] format
	*	Le renderer::PixelFormat à tester.
	*\return
	*	\p true s'il l'est...
	*\~english
	*\brief
	*	Tells if the given renderer::PixelFormat is usable in stencil buffers.
	*\param[in] format
	*	The renderer::PixelFormat.
	*\return
	*	\p true if it is usable in stencil buffers.
	*/
	bool isStencilFormat( renderer::PixelFormat format );
	/**
	*\~french
	*\brief
	*	Dit si le renderer::PixelFormat donné est un format utilisable pour les tampons de profondeur.
	*\param[in] format
	*	Le renderer::PixelFormat à tester.
	*\return
	*	\p true s'il l'est...
	*\~english
	*\brief
	*	Tells if the given renderer::PixelFormat is usable in depth buffers.
	*\param[in] format
	*	The renderer::PixelFormat.
	*\return
	*	\p true if it is usable in depth buffers.
	*/
	bool isDepthFormat( renderer::PixelFormat format );
	/**
	*\~french
	*\brief
	*	Récupère le masque d'aspects correspondant au renderer::PixelFormat donné.
	*\param[in] format
	*	Le renderer::PixelFormat.
	*\return
	*	Les aspects.
	*\~english
	*\brief
	*	Retrieves the aspects mask matching given renderer::PixelFormat.
	*\param[in] format
	*	The renderer::PixelFormat.
	*\return
	*	The aspects.
	*/
	ImageAspectFlags getAspectMask( renderer::PixelFormat format );
}

#endif
