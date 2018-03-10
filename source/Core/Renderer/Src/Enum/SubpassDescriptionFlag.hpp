/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_SubpassDescriptionFlag_HPP___
#define ___Renderer_SubpassDescriptionFlag_HPP___
#pragma once

namespace renderer
{
	/**
	*\~english
	*\brief
	*	Bitmask specifying usage of a subpass.
	*\~french
	*\brief
	*	Masque de bits définissant l'utilisation d'une sous-passe.
	*/
	enum class SubpassDescriptionFlag
		: uint32_t
	{
	};
	Utils_ImplementFlag( SubpassDescriptionFlag )
}

#endif
