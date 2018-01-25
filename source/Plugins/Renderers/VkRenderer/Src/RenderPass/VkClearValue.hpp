/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

#include <RenderPass/ClearValue.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::ClearValue en VkClearValue.
	*\param[in] value
	*	Le renderer::ClearValue.
	*\return
	*	Le VkClearValue.
	*/
	VkClearValue convert( renderer::ClearValue const & value );
	/**
	*\brief
	*	Convertit une renderer::RgbaColour en VkClearColorValue.
	*\param[in] value
	*	La renderer::RgbaColour.
	*\return
	*	La VkClearColorValue.
	*/
	VkClearColorValue convert( renderer::RgbaColour const & colour );
	/**
	*\brief
	*	Convertit une VkClearColorValue en renderer::RgbaColour.
	*\param[in] value
	*	La VkClearColorValue.
	*\return
	*	La renderer::RgbaColour.
	*/
	renderer::RgbaColour convert( VkClearColorValue const & colour );
	/**
	*\brief
	*	Convertit une renderer::DepthStencilClearValue en VkClearDepthStencilValue.
	*\param[in] value
	*	La renderer::DepthStencilClearValue.
	*\return
	*	La VkClearDepthStencilValue.
	*/
	VkClearDepthStencilValue convert( renderer::DepthStencilClearValue const & value );
	/**
	*\brief
	*	Convertit une VkClearDepthStencilValue en renderer::DepthStencilClearValue.
	*\param[in] value
	*	La VkClearDepthStencilValue.
	*\return
	*	La renderer::DepthStencilClearValue.
	*/
	renderer::RgbaColour convert( VkClearColorValue const & colour );
}
