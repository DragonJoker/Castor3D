/**
*\file
*	VertexBuffer.h
*\author
*	Sylvain Doremus
*/
#ifndef ___VkRenderer_VertexInputBindingDescription_HPP___
#define ___VkRenderer_VertexInputBindingDescription_HPP___
#pragma once

#include <Pipeline/VertexInputBindingDescription.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::VertexInputBindingDescription en VkVertexInputBindingDescription.
	*\param[in] state
	*	Le renderer::VertexInputBindingDescription.
	*\return
	*	Le VkVertexInputBindingDescription.
	*/
	VkVertexInputBindingDescription convert( renderer::VertexInputBindingDescription const & desc );
}

#endif
