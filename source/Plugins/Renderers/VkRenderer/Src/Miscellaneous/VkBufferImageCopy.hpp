/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___VkRenderer_BufferImageCopy_HPP___
#define ___VkRenderer_BufferImageCopy_HPP___
#pragma once

#include <Miscellaneous/BufferImageCopy.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::BufferImageCopy en VkBufferImageCopy.
	*\param[in] mode
	*	Le renderer::BufferImageCopy.
	*\return
	*	Le VkBufferImageCopy.
	*/
	VkBufferImageCopy convert( renderer::BufferImageCopy const & value );
}

#endif
