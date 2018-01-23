/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_PipelineBindPoint_HPP___
#define ___Renderer_PipelineBindPoint_HPP___
#pragma once

namespace renderer
{
	/**
	*\brief
	*	Enumération des points d'attache de tampon de commandes.
	*/
	enum class PipelineBindPoint
		: uint32_t
	{
		eGraphics,
		eCompute,
	};
}

#endif
