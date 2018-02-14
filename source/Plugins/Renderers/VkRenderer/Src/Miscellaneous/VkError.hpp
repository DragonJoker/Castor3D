/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include <string>

namespace vk_renderer
{
	/**
	*\~french
	*\brief
	*	Vérifie que le code donné ne représente pas une erreur.
	*\param[in] result
	*	Le code résultat Vulkan.
	*\return
	*	\p false si le code représente une erreur.
	*\~english
	*\brief
	*	Checks if the given code is an error code.
	*\param[in] result
	*	The Vulkan result code.
	*\return
	*	\p false if the code is an error.
	*/
	bool checkError( VkResult result );
	/**
	*\~french
	*\return
	*	Le texte de la dernière erreur rencontrée.
	*\~english
	*\return
	*	The last encountered error text.
	*/
	std::string const & getLastError();
}
