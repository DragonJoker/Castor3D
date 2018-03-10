/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_RenderSubpassState_HPP___
#define ___Renderer_RenderSubpassState_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	Specifies the wanted state at the beginning of a subpass.
	*\~french
	*\brief
	*	Description d'un état voulu au début d'une sous-passe de rendu.
	*/
	struct RenderSubpassState
	{
		/**
		*\~english
		*\brief
		*	The pipeline stage flags.
		*\~french
		*\brief
		*	Les indicateurs d'état du pipeline.
		*/
		PipelineStageFlags pipelineStage;
		/**
		*\~english
		*\brief
		*	The access flags.
		*\~french
		*\brief
		*	Les indicateurs d'accès.
		*/
		AccessFlags access;
	};
}

#endif
