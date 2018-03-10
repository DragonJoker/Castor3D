/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_ShaderStageState_HPP___
#define ___Renderer_ShaderStageState_HPP___
#pragma once

#include "Pipeline/SpecialisationInfo.hpp"
#include "Shader/ShaderModule.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	Holds the data describing a shader stage within a pipeline.
	*\~french
	*\brief
	*	Contient les données décrivant un niveau de shader dans un pipeline.
	*/
	struct ShaderStageState
	{
		ShaderModulePtr module;
		SpecialisationInfoBasePtr specialisationInfo = nullptr;
		std::string entryPoint = "main";
	};
}

#endif
