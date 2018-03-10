/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_DescriptorPoolCreateFlag_HPP___
#define ___Renderer_DescriptorPoolCreateFlag_HPP___
#pragma once

namespace renderer
{
	/**
	*\~english
	*\brief
	*	Bitmask specifying certain supported operations on a descriptor pool.
	*\~french
	*\brief
	*	Masque de bits définissant les opérations supportées sur un pool de descripteurs.
	*/
	enum class DescriptorPoolCreateFlag
		: uint32_t
	{
		eFreeDescriptorSet = 0x00000001,
	};
	Utils_ImplementFlag( DescriptorPoolCreateFlag )
}

#endif
