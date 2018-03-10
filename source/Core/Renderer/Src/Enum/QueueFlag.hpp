/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_QueueFlag_HPP___
#define ___Renderer_QueueFlag_HPP___
#pragma once

namespace renderer
{
	/**
	*\brief
	*	Enumération des types de file.
	*/
	enum class QueueFlag
		: uint32_t
	{
		eGraphics = 0x00000001,
		eCompute = 0x00000002,
		eTransfer = 0x00000004,
		eSparseBinding = 0x00000008,
	};
	Utils_ImplementFlag( QueueFlag )
}

#endif
