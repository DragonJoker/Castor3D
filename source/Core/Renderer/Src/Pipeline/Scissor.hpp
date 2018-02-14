/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_Scissor_HPP___
#define ___Renderer_Scissor_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\brief
	*	Description d'un ciseau à utiliser lors de la création d'un pipeline ou à l'exécution.
	*/
	class Scissor
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] x, y
		*	La position du ciseau.
		*\param[in] width, height
		*	Les dimensions du ciseau.
		*/
		Scissor( int32_t x
			, int32_t y
			, uint32_t width
			, uint32_t height );
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
		friend bool operator==( Scissor const & lhs, Scissor const & rhs );
	};

	inline bool operator==( Scissor const & lhs, Scissor const & rhs )
	{
		return lhs.m_offset == rhs.m_offset
			&& lhs.m_size == rhs.m_size;
	}

	inline bool operator!=( Scissor const & lhs, Scissor const & rhs )
	{
		return !( lhs == rhs );
	}
}

#endif
