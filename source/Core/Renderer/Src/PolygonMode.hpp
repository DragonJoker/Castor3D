/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_PolygonMode_HPP___
#define ___Renderer_PolygonMode_HPP___
#pragma once

#include <Design/FlagCombination.hpp>

namespace renderer
{
	/**
	*\brief
	*	Modes d'affichage des polygones.
	*/
	enum class PolygonMode
		: uint32_t
	{
		eFill,
		eLine,
		ePoint,
	};
}

#endif
