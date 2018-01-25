/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#pragma once

#include <Enum/SampleCountFlag.hpp>

namespace gl_renderer
{
	/**
	*\brief
	*	Convertit un renderer::SampleCountFlag en VkSampleCountFlagBits.
	*\param[in] flags
	*	Le renderer::SampleCountFlag.
	*\return
	*	Le VkSampleCountFlagBits.
	*/
	GLsizei convert( renderer::SampleCountFlag const & flags );
}
