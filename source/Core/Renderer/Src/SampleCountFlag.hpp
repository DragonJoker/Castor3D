/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_SampleCountFlag_HPP___
#define ___Renderer_SampleCountFlag_HPP___
#pragma once

#include <Design/FlagCombination.hpp>

namespace renderer
{
	enum class SampleCountFlag
		: uint32_t
	{
		e1 = 0x00000001,
		e2 = 0x00000002,
		e4 = 0x00000004,
		e8 = 0x00000008,
		e16 = 0x00000010,
		e32 = 0x00000020,
		e64 = 0x00000040,
	};
	IMPLEMENT_FLAGS( SampleCountFlag )
}

#endif
