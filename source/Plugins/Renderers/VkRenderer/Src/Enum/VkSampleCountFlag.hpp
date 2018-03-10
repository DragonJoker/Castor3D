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
	*	Convertit un renderer::SampleCountFlag en VkSampleCountFlagBits.
	*\param[in] flags
	*	Le renderer::SampleCountFlag.
	*\return
	*	Le VkSampleCountFlagBits.
	*/
	VkSampleCountFlagBits convert( renderer::SampleCountFlag const & flags );
	/**
	*\brief
	*	Convertit un VkSampleCountFlags en renderer::SampleCountFlags.
	*\param[in] flags
	*	Le VkSampleCountFlags.
	*\return
	*	Le renderer::SampleCountFlags.
	*/
	renderer::SampleCountFlags convertSampleCountFlags( VkSampleCountFlags const & flags );
}
