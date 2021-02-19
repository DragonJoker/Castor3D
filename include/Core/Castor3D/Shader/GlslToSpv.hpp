/*
This file belongs to Ashes.
See LICENSE file in root folder.
*/
#pragma once

#include "ShaderModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"

namespace castor3d
{
	/**
	*\~english
	*\brief
	*	Initialises glslang globals.
	*\~french
	*\brief
	*	Initialise les globales de glslang.
	*/
	C3D_API void initialiseGlslang();
	/**
	*\~english
	*\brief
	*	Cleans up glslang globals.
	*\~french
	*\brief
	*	Nettoie les globales de glslang.
	*/
	C3D_API void cleanupGlslang();
	/**
	*\~english
	*\brief
	*	Transpiles a GLSL shader to SPIR-V.
	*\param device
	*	The GPU device.
	*\param stage
	*	The shader stage.
	*\param shader
	*	The GLSL shader.
	*\~french
	*\brief
	*	Transpile un shader GLSL en SPIR-V.
	*\param device
	*	Le device GPU.
	*\param stage
	*	Le shader stage.
	*\param shader
	*	Le shader GLSL.
	*/
	C3D_API UInt32Array compileGlslToSpv( RenderDevice const & device
		, VkShaderStageFlagBits stage
		, std::string const & shader );
}
