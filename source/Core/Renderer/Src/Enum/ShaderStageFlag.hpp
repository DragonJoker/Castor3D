/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_ShaderStageFlag_HPP___
#define ___Renderer_ShaderStageFlag_HPP___
#pragma once

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
	Utils_ImplementFlag( ShaderStageFlag )
	/**
	*\~english
	*\brief
	*	Gets the name of the given element type.
	*\param[in] value
	*	The element type.
	*\return
	*	The name.
	*\~french
	*\brief
	*	Récupère le nom du type d'élément donné.
	*\param[in] value
	*	Le type d'élément.
	*\return
	*	Le nom.
	*/
	inline std::string getName( ShaderStageFlag value )
	{
		switch ( value )
		{
		case ShaderStageFlag::eVertex:
			return "vertex";

		case ShaderStageFlag::eTessellationControl:
			return "tess_control";

		case ShaderStageFlag::eTessellationEvaluation:
			return "tess_eval";

		case ShaderStageFlag::eGeometry:
			return "geometry";

		case ShaderStageFlag::eFragment:
			return "fragment";

		case ShaderStageFlag::eCompute:
			return "compute";

		default:
			assert( false && "Unsupported ShaderStageFlag." );
			throw std::runtime_error{ "Unsupported ShaderStageFlag" };
		}

		return 0;
	}
}

#endif
