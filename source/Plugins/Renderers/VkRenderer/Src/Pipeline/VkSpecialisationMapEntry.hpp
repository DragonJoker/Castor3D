/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include <Pipeline/SpecialisationMapEntry.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::SpecialisationMapEntry en VkSpecializationMapEntry.
	*\param[in] state
	*	Le renderer::SpecialisationMapEntry.
	*\return
	*	Le VkSpecializationMapEntry.
	*/
	VkSpecializationMapEntry convert( renderer::SpecialisationMapEntry const & state );
}
