/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_FormatFeatureFlag_HPP___
#define ___Renderer_FormatFeatureFlag_HPP___
#pragma once

namespace renderer
{
	/**
	*\brief
	*	Enumération des capacités mémoire.
	*/
	enum class FormatFeatureFlag
		: int32_t
	{
		eSampledImage = 0x00000001,
		eStorageImage = 0x00000002,
		eStorageImageAtomic = 0x00000004,
		eUniformTexelBuffer = 0x00000008,
		eStorageTexelBuffer = 0x00000010,
		eStorageTexelBufferAtomic = 0x00000020,
		eVertexBuffer = 0x00000040,
		eColourAttachment = 0x00000080,
		eColourAttachmentBlend = 0x00000100,
		eDepthStencilAttachment = 0x00000200,
		eBlitSrc = 0x00000400,
		eBlitDst = 0x00000800,
		eSampledImageFilterLinear = 0x00001000,
		eSampledImageFilterCubic = 0x00002000,
	};
	Utils_ImplementFlag( FormatFeatureFlag )
}

#endif
