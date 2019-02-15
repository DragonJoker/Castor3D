/*
This file belongs to Ashes.
See LICENSE file in root folder.
*/
#pragma once

#include "Castor3D/Castor3DPrerequisites.hpp"

namespace castor3d
{
	/**
	*\~french
	*\brief
	*	Initialise les globales de glslang.
	*\~english
	*\brief
	*	Initialises glslang globals.
	*/
	C3D_API void initialiseGlslang();
	/**
	*\~french
	*\brief
	*	Nettoie les globales de glslang.
	*\~english
	*\brief
	*	Cleans up glslang globals.
	*/
	C3D_API void cleanupGlslang();
	/**
	*\~french
	*\brief
	*	Transpile un shader GLSL en SPIR-V.
	*\~english
	*\brief
	*	Transpiles a GLSL shader to SPIR-V.
	*/
	C3D_API UInt32Array compileGlslToSpv( ashes::Device const & device
		, ashes::ShaderStageFlag stage
		, std::string const & shader );
}
