/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_ImageLayout_HPP___
#define ___Renderer_ImageLayout_HPP___
#pragma once

namespace renderer
{
	/**
	*\brief
	*	Enumération des layouts d'image.
	*/
	enum class ImageLayout
	{
		eUndefined = 0,
		eGeneral = 1,
		eColourAttachmentOptimal = 2,
		eDepthStencilAttachmentOptimal = 3,
		eDepthStencilReadOnlyOptimal = 4,
		eShaderReadOnlyOptimal = 5,
		eTransferSrcOptimal = 6,
		eTransferDstOptimal = 7,
		ePreinitialised = 8,
		Utils_EnumBounds( eUndefined ),
		ePresentSrc = 1000001002,
	};
}

#endif
