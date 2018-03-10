/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___GlRenderer_TextureViewType_HPP___
#define ___GlRenderer_TextureViewType_HPP___
#pragma once

#include <RendererPrerequisites.hpp>

namespace gl_renderer
{
	enum GlTextureViewType
		: GLenum
	{
		GL_TEXTURE_VIEW_1D = 0x0DE0,
		GL_TEXTURE_VIEW_2D = 0x0DE1,
		GL_TEXTURE_VIEW_3D = 0x806F,
		GL_TEXTURE_VIEW_CUBE_MAP = 0x8513,
		GL_TEXTURE_VIEW_1D_ARRAY = 0x8C18,
		GL_TEXTURE_VIEW_2D_ARRAY = 0x8C1A,
		GL_TEXTURE_VIEW_CUBE_MAP_ARRAY = 0x9009,
		GL_TEXTURE_VIEW_2D_MULTISAMPLE = 0x9100,
		GL_TEXTURE_VIEW_2D_MULTISAMPLE_ARRAY = 0x9102,
		GL_TEXTURE_VIEW_CUBE_MAP_POSITIVE_X = 0x8515,
		GL_TEXTURE_VIEW_CUBE_MAP_NEGATIVE_X = 0x8516,
		GL_TEXTURE_VIEW_CUBE_MAP_POSITIVE_Y = 0x8517,
		GL_TEXTURE_VIEW_CUBE_MAP_NEGATIVE_Y = 0x8518,
		GL_TEXTURE_VIEW_CUBE_MAP_POSITIVE_Z = 0x8519,
		GL_TEXTURE_VIEW_CUBE_MAP_NEGATIVE_Z = 0x851A,
	};
	std::string getName( GlTextureViewType value );
	/**
	*\brief
	*	Convertit un renderer::WrapMode en GlTextureType.
	*\param[in] mode
	*	Le renderer::WrapMode.
	*\return
	*	Le GlTextureType.
	*/
	GlTextureViewType convert( renderer::TextureViewType const & mode );
}

#endif
