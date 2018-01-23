/*
This file belongs to Renderer.
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
	*	D�finit une op�ration de copie de tampon.
	*/
	struct BufferCopy
	{
		/**
		*\~english
		*\brief
		*	The starting offset in bytes from the start of \p srcBuffer.
		*\~french
		*\brief
		*	Le d�calage de d�part en octets, depuis le d�but de \p srcBuffer.
		*/
		uint32_t srcOffset;
		/**
		*\~english
		*\brief
		*	The starting offset in bytes from the start of \p dstBuffer.
		*\~french
		*\brief
		*	Le d�calage de d�part en octets, depuis le d�but de \p dstBuffer.
		*/
		uint32_t dstOffset;
		/**
		*\~english
		*\brief
		*	The number of bytes to copy.
		*\~french
		*\brief
		*	Le nombre d'octets � copier.
		*/
		uint32_t size;
	};
}

#endif
