/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_MemoryPropertyFlag_HPP___
#define ___Renderer_MemoryPropertyFlag_HPP___
#pragma once

namespace renderer
{
	/**
	*\brief
	*	Masques de bits décrivant les propriétés pour un type de mémoire.
	*/
	enum class MemoryPropertyFlag
		: uint32_t
	{
		//! Non cached on host.
		eDeviceLocal = 0x00000001,
		//! Host can map memory allocated with this flag.
		eHostVisible = 0x00000002,
		//! Cache commands to flush or invalidate the host memory changes are not necessary anymore, with this flag.
		eHostCoherent = 0x00000004,
		//! The memory allocated with this flag is cached on the host (hence changes will need flush or invalidate commands).
		eHostCached = 0x00000008,
		//! Non visible to host, moreover object's memory may be provided lazily, by implementation.
		eLazilyAllocated = 0x00000010,
	};
	Utils_ImplementFlag( MemoryPropertyFlag )
}

#endif
