/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___VkRenderer_BufferCopy_HPP___
#define ___VkRenderer_BufferCopy_HPP___
#pragma once

#include <Miscellaneous/BufferCopy.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::BufferCopy en VkBufferCopy.
	*\param[in] mode
	*	Le renderer::BufferCopy.
	*\return
	*	Le VkBufferCopy.
	*/
	VkBufferCopy convert( renderer::BufferCopy const & value );
}

#endif
