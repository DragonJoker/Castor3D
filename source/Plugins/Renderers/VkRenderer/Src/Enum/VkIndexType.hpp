/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#pragma once

#include <RendererPrerequisites.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::IndexType en VkIndexType.
	*\param[in] type
	*	Le renderer::IndexType.
	*\return
	*	Le VkIndexType.
	*/
	VkIndexType convert( renderer::IndexType const & type );
}
