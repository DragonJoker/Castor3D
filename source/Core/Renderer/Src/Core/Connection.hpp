/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_Connection_HPP___
#define ___Renderer_Connection_HPP___
#pragma once

#include "Core/WindowHandle.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	Connection between a physical device and a window.
	*\~french
	*\brief
	*	Connection entre un périphérique physique et une fenêtre.
	*/
	class Connection
	{
	protected:
		Connection( Connection const & ) = delete;
		Connection & operator=( Connection const & ) = delete;
		Connection( Connection && rhs ) = default;
		Connection & operator=( Connection && rhs ) = default;
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] renderer
		*	The Renderer instance.
		*\param[in] deviceIndex
		*	The physical device index.
		*\param[in] handle
		*	The window handle.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] renderer
		*	L'instance de Renderer.
		*\param[in] deviceIndex
		*	L'indice du périphérique physique.
		*\param[in] handle
		*	Le descripteur de la fenêtre.
		*/
		Connection( Renderer const & renderer
			, uint32_t deviceIndex
			, WindowHandle && handle );

	public:
		/**
		*\~english
		*\brief
		*	Destructor.
		*\~french
		*\brief
		*	Destructeur.
		*/
		virtual ~Connection() = default;
		/**
		*\~french
		*\return
		*	The window handle.
		*\~french
		*\return
		*	Le descripteur de la fenêtre.
		*/
		WindowHandle const & getHandle()const
		{
			return m_handle;
		}
		/**
		*\~french
		*\return
		*	Le périphérique physique.
		*\~english
		*\return
		*	The physical device.
		*/
		inline PhysicalDevice const & getGpu()const
		{
			return m_gpu;
		}

	protected:
		WindowHandle m_handle;
		PhysicalDevice const & m_gpu;
	};
}

#endif
