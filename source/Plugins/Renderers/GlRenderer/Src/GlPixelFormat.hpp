/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#pragma once

#include <UtilsMapping.hpp>

namespace gl_renderer
{
	enum GlInternal
		: GLenum
	{
		GL_INTERNAL_R8 = 0x8229,
		GL_INTERNAL_R8G8 = 0x822B,
		GL_INTERNAL_R8G8B8 = 0x8051,
		GL_INTERNAL_R5G6B5 = 0x8D62,
		GL_INTERNAL_R8G8B8A8 = 0x8058,
		GL_INTERNAL_B8G8R8A8 = 0x93A1,
		GL_INTERNAL_R5G5B5A1 = 0x8057,
		GL_INTERNAL_R4G4B4A4 = 0x8056,
		GL_INTERNAL_D16 = 0x81A5,
		GL_INTERNAL_D24S8 = 0x88F0,
		GL_INTERNAL_D32F = 0x8CAC,
		GL_INTERNAL_S8 = 0x8D48,
	};
	enum GlFormat
		: GLenum
	{
		GL_FORMAT_S = 0x1802,
		GL_FORMAT_D = 0x1902,
		GL_FORMAT_R = 0x1903,
		GL_FORMAT_RGB = 0x1907,
		GL_FORMAT_RGBA = 0x1908,
		GL_FORMAT_ABGR = 0x8000,
		GL_FORMAT_BGR = 0x80E0,
		GL_FORMAT_BGRA = 0x80E1,
		GL_FORMAT_RG = 0x8227,
		GL_FORMAT_DS = 0x84F9,
	};
	enum GlType
		: GLenum
	{
		GL_TYPE_UI8 = 0x1401,
		GL_TYPE_UI16 = 0x1403,
		GL_TYPE_F = 0x1406,
		GL_TYPE_US4444 = 0x8033,
		GL_TYPE_US5551 = 0x8034,
		GL_TYPE_UI8888 = 0x8035,
		GL_TYPE_UI565 = 0x8363,
		GL_TYPE_UI24_8 = 0x84FA,
	};
	/**
	*\brief
	*	Convertit un renderer::PixelFormat en GlInternal.
	*\param[in] format
	*	Le renderer::PixelFormat.
	*\return
	*	Le GlInternal.
	*/
	GlInternal getInternal( renderer::PixelFormat const & format )noexcept;
	/**
	*\brief
	*	Convertit un renderer::PixelFormat en GlFormat.
	*\param[in] format
	*	Le renderer::PixelFormat.
	*\return
	*	Le GlFormat.
	*/
	GlFormat getFormat( renderer::PixelFormat format )noexcept;
	/**
	*\brief
	*	Convertit un renderer::PixelFormat en GlType.
	*\param[in] format
	*	Le renderer::PixelFormat.
	*\return
	*	Le GlType.
	*/
	GlType getType( renderer::PixelFormat format )noexcept;
	/**
	*\brief
	*	Récupère la taille en octets du renderer::PixelFormat donné.
	*\param[in] format
	*	Le renderer::PixelFormat.
	*\return
	*	La taille des données.
	*/
	uint32_t getSize( renderer::PixelFormat format )noexcept;
	/**
	*\brief
	*	Convertit un GlInternal en renderer::PixelFormat.
	*\param[in] format
	*	Le GlInternal.
	*\return
	*	Le renderer::PixelFormat.
	*/
	renderer::PixelFormat convert( GlInternal format )noexcept;
}
