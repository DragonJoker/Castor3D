/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___GlRenderer_TextureType_HPP___
#define ___GlRenderer_TextureType_HPP___
#pragma once

#include <RendererPrerequisites.hpp>

namespace gl_renderer
{
	enum GlTextureType
		: GLenum
	{
		GL_TEXTURE_1D = 0x0DE0,
		GL_TEXTURE_2D = 0x0DE1,
		GL_TEXTURE_3D = 0x806F,
		GL_TEXTURE_1D_ARRAY = 0x8C18,
		GL_TEXTURE_2D_ARRAY = 0x8C1A,
		GL_TEXTURE_2D_MULTISAMPLE = 0x9100,
		GL_TEXTURE_2D_MULTISAMPLE_ARRAY = 0x9102,
	};
	std::string getName( GlTextureType value );
	/**
	*\brief
	*	Convertit un renderer::TextureType en GlTextureType.
	*\param[in] mode
	*	Le renderer::TextureType.
	*\return
	*	Le GlTextureType.
	*/
	GlTextureType convert( renderer::TextureType const & mode
		, uint32_t layerCount );
}

#endif
