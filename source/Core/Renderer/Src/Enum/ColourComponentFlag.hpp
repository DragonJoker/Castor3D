/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_ColourComponentFlag_HPP___
#define ___Renderer_ColourComponentFlag_HPP___
#pragma once

namespace renderer
{
	/**
	*\brief
	*	Enumération des types d'accès.
	*/
	enum class ColourComponentFlag
		: uint32_t
	{
		eR = 0x00000001,
		eG = 0x00000002,
		eB = 0x00000004,
		eA = 0x00000008,
	};
	Utils_ImplementFlag( ColourComponentFlag )
}

#endif
