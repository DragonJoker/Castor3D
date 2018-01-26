/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

#include <Enum/ShaderStageFlag.hpp>

namespace gl_renderer
{
	enum GlShaderStageFlag
	{
		GL_SHADER_STAGE_FRAGMENT = 0x8B30,
		GL_SHADER_STAGE_VERTEX = 0x8B31,
		GL_SHADER_STAGE_GEOMETRY = 0x8DD9,
		GL_SHADER_STAGE_TESS_CONTROL = 0x8E88,
		GL_SHADER_STAGE_TESS_EVALUATION = 0x8E87,
		GL_SHADER_STAGE_COMPUTE = 0x91B9,
	};
	Utils_ImplementFlag( GlShaderStageFlag );
	std::string getName( GlShaderStageFlags value );
	/**
	*\brief
	*	Convertit un renderer::ShaderStageFlags en VkShaderStageFlags.
	*\param[in] flags
	*	Le renderer::ShaderStageFlags.
	*\return
	*	Le VkShaderStageFlags.
	*/
	GlShaderStageFlags convert( renderer::ShaderStageFlags const & flags );
	/**
	*\brief
	*	Convertit un renderer::ShaderStageFlag en VkShaderStageFlagBits.
	*\param[in] flags
	*	Le renderer::ShaderStageFlag.
	*\return
	*	Le VkShaderStageFlagBits.
	*/
	GlShaderStageFlag convert( renderer::ShaderStageFlag const & flag );
}
