/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_DescriptorType_HPP___
#define ___Renderer_DescriptorType_HPP___
#pragma once

namespace renderer
{
	/**
	*\~french
	*\brief
	*	Masques de bits décrivant les propriétés pour un type de mémoire.
	*\~english
	*\brief
	*	Bitmask specifying properties for a memory type.
	*/
	enum class DescriptorType
		: uint32_t
	{
		eSampler,
		eCombinedImageSampler,
		eSampledImage,
		eStorageImage,
		eUniformTexelBuffer,
		eStorageTexelBuffer,
		eUniformBuffer,
		eStorageBuffer,
		eUniformBufferDynamic,
		eStorageBufferDynamic,
		eInputAttachment,
	};
}

#endif
