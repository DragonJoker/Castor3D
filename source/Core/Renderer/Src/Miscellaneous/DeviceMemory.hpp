/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_DeviceMemory_HPP___
#define ___Renderer_DeviceMemory_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	A device memory object.
	*\~french
	*\brief
	*	Un objet mémoire.
	*/
	class DeviceMemory
	{
	protected:
		DeviceMemory( Device const & device
			, MemoryPropertyFlags flags )
			: m_device{ device }
			, m_flags{ flags }
		{
		}

	public:
		virtual ~DeviceMemory() = default;
		/**
		*\~english
		*\brief
		*	Maps a range of the memory in RAM.
		*\param[in] offset
		*	The range beginning offset.
		*\param[in] size
		*	The range size.
		*\param[in] flags
		*	The mapping flags.
		*\return
		*	\p nullptr if mapping failed.
		*\~french
		*\brief
		*	Mappe la mémoire en RAM.
		*\param[in] offset
		*	L'offset à partir duquel la mémoire est mappée.
		*\param[in] size
		*	La taille en octets de la mémoire à mapper.
		*\param[in] flags
		*	Indicateurs de configuration du mapping.
		*\return
		*	\p nullptr si le mapping a échoué.
		*/
		virtual uint8_t * lock( uint32_t offset
			, uint32_t size
			, renderer::MemoryMapFlags flags )const = 0;
		/**
		*\~english
		*\brief
		*	Invalidates the memory content.
		*\param[in] offset
		*	The mapped memory starting offset.
		*\param[in] size
		*	The range size.
		*\~french
		*\brief
		*	Invalide le contenu de la mémoire.
		*\param[in] offset
		*	L'offset de la mémoire mappée.
		*\param[in] size
		*	La taille en octets de la mémoire mappée.
		*/
		virtual void invalidate( uint32_t offset
			, uint32_t size )const = 0;
		/**
		*\~english
		*\brief
		*	Updates the VRAM.
		*\param[in] offset
		*	The mapped memory starting offset.
		*\param[in] size
		*	The range size.
		*\~french
		*\brief
		*	Met à jour la VRAM.
		*\param[in] offset
		*	L'offset de la mémoire mappée.
		*\param[in] size
		*	La taille en octets de la mémoire mappée.
		*/
		virtual void flush( uint32_t offset
			, uint32_t size )const = 0;
		/**
		*\~english
		*\brief
		*	Unmaps the memory from RAM.
		*\~french
		*\brief
		*	Unmappe la mémoire de la RAM.
		*/
		virtual void unlock()const = 0;

	protected:
		Device const & m_device;
		MemoryPropertyFlags m_flags;
	};
}

#endif
