/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_Scissor_HPP___
#define ___Renderer_Scissor_HPP___
#pragma once

#include "Miscellaneous/Extent2D.hpp"
#include "Miscellaneous/Offset2D.hpp"

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
		inline Offset2D const & getOffset()const
		{
			return m_offset;
		}
		/**
		*\return
		*	Les dimensions du ciseau.
		*/
		inline Extent2D const & getSize()const
		{
			return m_size;
		}

	private:
		Offset2D m_offset;
		Extent2D m_size;
		friend bool operator==( Scissor const & lhs, Scissor const & rhs );
	};

	inline bool operator==( Scissor const & lhs, Scissor const & rhs )
	{
		return lhs.m_offset.x == rhs.m_offset.x
			&& lhs.m_offset.y == rhs.m_offset.y
			&& lhs.m_size.width == rhs.m_size.width
			&& lhs.m_size.height == rhs.m_size.height;
	}

	inline bool operator!=( Scissor const & lhs, Scissor const & rhs )
	{
		return !( lhs == rhs );
	}
}

#endif
