/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_ShaderStageFlag_HPP___
#define ___Renderer_ShaderStageFlag_HPP___
#pragma once

#include <Design/FlagCombination.hpp>

namespace renderer
{
	/**
	*\brief
	*	Masques de bits décrivant les niveaux de shader.
	*/
	enum class ShaderStageFlag
		: uint32_t
	{
		eVertex = 0x00000001,
		eTessellationControl = 0x00000002,
		eTessellationEvaluation = 0x00000004,
		eGeometry = 0x00000008,
		eFragment = 0x00000010,
		eCompute = 0x00000020,
		eAll = 0x0000001F,
	};
	IMPLEMENT_FLAGS( ShaderStageFlag )
}

#endif
