/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#pragma once

#include <RendererPrerequisites.hpp>

namespace gl_renderer
{
	enum GlConstantFormatBaseType
		: GLenum
	{
		GL_CONSTANT_FORMAT_BASE_TYPE_UNSIGNED_BYTE = 0x1401,
		GL_CONSTANT_FORMAT_BASE_TYPE_INT = 0x1404,
		GL_CONSTANT_FORMAT_BASE_TYPE_UNSIGNED_INT = 0x1405,
		GL_CONSTANT_FORMAT_BASE_TYPE_FLOAT = 0x1406,
	};
	std::string getName( GlConstantFormatBaseType value );

	/**
	*\brief
	*	Convertit un renderer::ConstantFormat en GlConstantFormat.
	*\param[in] flags
	*	Le renderer::ConstantFormat.
	*\return
	*	Le GlConstantFormat.
	*/
	GlConstantFormatBaseType getType( renderer::ConstantFormat format );
	/**
	*\brief
	*	R�cup�re le nombre d'�l�ments du format donn�.
	*\param[in] flags
	*	Le renderer::ConstantFormat.
	*\return
	*	Le compte.
	*/
	uint32_t getCount( renderer::ConstantFormat format );
}
