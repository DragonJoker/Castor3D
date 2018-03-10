/**
*\file
*	VertexBuffer.h
*\author
*	Sylvain Doremus
*/
#ifndef ___VkRenderer_VertexInputAttributeDescription_HPP___
#define ___VkRenderer_VertexInputAttributeDescription_HPP___
#pragma once

#include <Pipeline/VertexInputAttributeDescription.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::VertexInputAttributeDescription en VkVertexInputAttributeDescription.
	*\param[in] desc
	*	Le renderer::VertexInputAttributeDescription.
	*\return
	*	Le VkVertexInputAttributeDescription.
	*/
	VkVertexInputAttributeDescription convert( renderer::VertexInputAttributeDescription const & desc );
}

#endif
