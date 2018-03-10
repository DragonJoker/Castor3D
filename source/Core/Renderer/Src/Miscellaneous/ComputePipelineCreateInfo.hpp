/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_ComputePipelineCreateInfo_HPP___
#define ___Renderer_ComputePipelineCreateInfo_HPP___
#pragma once

#include "Pipeline/ShaderStageState.hpp"
#include "Pipeline/SpecialisationInfo.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	Holds the data used to create a compute pipeline.
	*\~french
	*\brief
	*	Contient les données utilisées pour créer un pipeline de calcul.
	*/
	struct ComputePipelineCreateInfo
	{
		ShaderStageState stage;
	};
}

#endif
