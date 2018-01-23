/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_Viewport_HPP___
#define ___Renderer_Viewport_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\brief
	*	Description d'un viewport à utiliser lors de la création d'un pipeline ou à l'exécution.
	*/
	class Viewport
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] width, height
		*	Les dimensions du viewport.
		*\param[in] x, y
		*	La position du viewport.
		*/
		Viewport( uint32_t width
			, uint32_t height
			, int32_t x
			, int32_t y );
		/**
		*\return
		*	L'offset du ciseau.
		*/
		inline IVec2 const & getOffset()const
		{
			return m_offset;
		}
		/**
		*\return
		*	Les dimensions du ciseau.
		*/
		inline IVec2 const & getSize()const
		{
			return m_size;
		}

	private:
		IVec2 m_offset;
		IVec2 m_size;
	};
}

#endif
