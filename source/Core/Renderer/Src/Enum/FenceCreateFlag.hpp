/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_FenceCreateFlag_HPP___
#define ___Renderer_FenceCreateFlag_HPP___
#pragma once

namespace renderer
{
	/**
	*\brief
	*	Enumération des types d'accès.
	*/
	enum class FenceCreateFlag
		: uint32_t
	{
		eSignaled = 0x00000001,
	};
	Utils_ImplementFlag( FenceCreateFlag )
}

#endif
