/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_DependencyFlag_HPP___
#define ___Renderer_DependencyFlag_HPP___
#pragma once

namespace renderer
{
	/**
	*\~english
	*\brief
	*	Bitmask specifying how execution and memory dependencies are formed.
	*\~french
	*\brief
	*	Masque de bits définissant comment l'exécution et les dépendances mémoire sont constituées.
	*/
	enum class DependencyFlag
		: uint32_t
	{
		eByRegion = 0x00000001
	};
	Utils_ImplementFlag( DependencyFlag )
}

#endif
