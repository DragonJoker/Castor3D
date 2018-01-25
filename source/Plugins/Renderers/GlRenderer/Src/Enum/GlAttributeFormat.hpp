/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#pragma once

#include <Enum/AttributeFormat.hpp>

namespace gl_renderer
{
	enum GlAttributeFormatBaseType
		: GLenum
	{
		GL_ATTRIBUTE_FORMAT_BASE_TYPE_INT = 0x1404,
		GL_ATTRIBUTE_FORMAT_BASE_TYPE_UNSIGNED_INT = 0x1405,
		GL_ATTRIBUTE_FORMAT_BASE_TYPE_FLOAT = 0x1406,
	};

	/**
	*\brief
	*	Convertit un renderer::AttributeFormat en GlAttributeFormatBaseType.
	*\param[in] flags
	*	Le renderer::AttributeFormat.
	*\return
	*	Le GlAttributeFormatBaseType.
	*/
	GlAttributeFormatBaseType getType( renderer::AttributeFormat format );
	/**
	*\brief
	*	R�cup�re le nombre d'�l�ments du format donn�.
	*\param[in] flags
	*	Le renderer::AttributeFormat.
	*\return
	*	Le compte.
	*/
	uint32_t getCount( renderer::AttributeFormat format );
}
