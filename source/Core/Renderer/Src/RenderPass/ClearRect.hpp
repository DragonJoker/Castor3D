/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_ClearRect_HPP___
#define ___Renderer_ClearRect_HPP___
#pragma once

#include "Miscellaneous/Extent2D.hpp"
#include "Miscellaneous/Offset2D.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	Specifies a clear rectangle.
	*\remarks
	*	The layers [\p baseArrayLayer, \p baseArrayLayer + \p layerCount] counting from the base layer of the attachment image view are cleared.
	*\~french
	*\brief
	*	Définit un rectangle de nettoyage.
	*\remarks
	*	Les couches [\p baseArrayLayer, \p baseArrayLayer + \p layerCount] en comptant depuis la couche de base de la vue sur l'image de l'attache sont nettoyées.
	*/
	struct ClearRect
	{
		/**
		*\~english
		*\brief
		*	The rectangle offset.
		*\~french
		*\brief
		*	Le décalage du rectangle.
		*/
		Offset2D offset;
		/**
		*\~english
		*\brief
		*	The rectangle extent.
		*\~french
		*\brief
		*	L'étendue du rectangle.
		*/
		Extent2D extent;
		/**
		*\~english
		*\brief
		*	The first layer to be cleared.
		*\~french
		*\brief
		*	La première couche à nettoyer.
		*/
		uint32_t baseArrayLayer{ 0u };
		/**
		*\~english
		*\brief
		*	The number of layers to clear.
		*\~french
		*\brief
		*	Le nombre de couches à nettoyer.
		*/
		uint32_t layerCount{ 1u };
	};
}

#endif
