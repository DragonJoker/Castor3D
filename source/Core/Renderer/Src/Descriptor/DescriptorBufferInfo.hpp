/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_DescriptorBufferInfo_HPP___
#define ___Renderer_DescriptorBufferInfo_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	struct DescriptorBufferInfo
	{
		BufferCRef buffer;
		uint64_t offset;
		uint64_t range;
	};
}

#endif
