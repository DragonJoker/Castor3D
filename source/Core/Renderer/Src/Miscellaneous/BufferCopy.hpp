/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_BufferCopy_HPP___
#define ___Renderer_BufferCopy_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	Specifies a buffer copy operation.
	*\~french
	*\brief
	*	Définit une opération de copie de tampon.
	*/
	struct BufferCopy
	{
		/**
		*\~english
		*\brief
		*	The starting offset in bytes from the start of \p srcBuffer.
		*\~french
		*\brief
		*	Le décalage de départ en octets, depuis le début de \p srcBuffer.
		*/
		uint32_t srcOffset;
		/**
		*\~english
		*\brief
		*	The starting offset in bytes from the start of \p dstBuffer.
		*\~french
		*\brief
		*	Le décalage de départ en octets, depuis le début de \p dstBuffer.
		*/
		uint32_t dstOffset;
		/**
		*\~english
		*\brief
		*	The number of bytes to copy.
		*\~french
		*\brief
		*	Le nombre d'octets à copier.
		*/
		uint32_t size;
	};
}

#endif
