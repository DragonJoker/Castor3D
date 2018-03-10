/**
*\file
*	VkBufferView.h
*\author
*	Sylvain Doremus
*/
#ifndef ___VkRenderer_BufferView_HPP___
#define ___VkRenderer_BufferView_HPP___
#pragma once

#include "VkRendererPrerequisites.hpp"

#include <Buffer/BufferView.hpp>

namespace vk_renderer
{
	/**
	*\~french
	*\brief
	*	Description d'une vue sur une image Vulkan.
	*\~english
	*\brief
	*	Vulkan image view wrapper.
	*/
	class BufferView
		: public renderer::BufferView
	{
	public:
		/**
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] device
		*	La connexion logique au GPU.
		*\param[in] buffer
		*	Le tampon sur lequel la vue est créée.
		*\param[in] format
		*	Le format des pixels de la vue.
		*\param[in] offset
		*	Le décalage dans le tampon.
		*\param[in] range
		*	Le nombre d'éléments dans le tampon.
		*\~english
		*\brief
		*	Constructor.
		*\param[in] device
		*	The logical connection to the GPU.
		*\param[in] buffer
		*	The buffer from which the view is created.
		*\param[in] format
		*	The view's pixels format.
		*\param[in] offset
		*	The offset in the buffer.
		*\param[in] range
		*	The number of elements from the buffer.
		*/
		BufferView( Device const & device
			, Buffer const & buffer
			, renderer::Format format
			, uint32_t offset
			, uint32_t range );
		/**
		*\~french
		*\brief
		*	Destructeur.
		*\~english
		*\brief
		*	Destructor.
		*/
		~BufferView();
		/**
		*\~french
		*\brief
		*	Conversion implicite vers VkBufferView.
		*\~english
		*\brief
		*	VkBufferView implicit cast operator.
		*/
		inline operator VkBufferView const &( )const
		{
			return m_view;
		}

	private:
		Device const & m_device;
		VkBufferView m_view{};
	};
}

#endif
