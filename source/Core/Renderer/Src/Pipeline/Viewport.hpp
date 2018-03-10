/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_Viewport_HPP___
#define ___Renderer_Viewport_HPP___
#pragma once

#include "Miscellaneous/Extent2D.hpp"
#include "Miscellaneous/Offset2D.hpp"

namespace renderer
{
	/**
	*\~french
	*\brief
	*	Description d'un viewport à utiliser lors de la création d'un pipeline ou à l'exécution.
	*\~english
	*\brief
	*	Speifies a viewport to use when creating a pipeline or executing a command buffer.
	*/
	class Viewport
	{
	public:
		/**
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] width, height
		*	Les dimensions du viewport.
		*\param[in] x, y
		*	La position du viewport.
		*\param[in] minZ, maxZ
		*	Les bornes profondeur du viewport.
		*\~english
		*\brief
		*	Constructor.
		*\param[in] width, height
		*	The viewport dimensions.
		*\param[in] x, y
		*	The viewport position.
		*\param[in] minZ, maxZ
		*	The viewport depth bounds.
		*/
		Viewport( uint32_t width
			, uint32_t height
			, int32_t x
			, int32_t y
			, float minZ = 0.0f
			, float maxZ = 1.0f );
		/**
		*\~french
		*\return
		*	La position du viewport.
		*\~french
		*\return
		*	The viewport position.
		*/
		inline Offset2D const & getOffset()const
		{
			return m_offset;
		}
		/**
		*\~french
		*\return
		*	Les dimensions du viewport.
		*\~french
		*\return
		*	The viewport dimensions.
		*/
		inline Extent2D const & getSize()const
		{
			return m_size;
		}
		/**
		*\~french
		*\return
		*	Les bornes profondeur du viewport.
		*\~french
		*\return
		*	The viewport depth bounds.
		*/
		inline Vec2 const & getDepthBounds()const
		{
			return m_depthBounds;
		}

	private:
		Offset2D m_offset;
		Extent2D m_size;
		Vec2 m_depthBounds;
		friend bool operator==( Viewport const & lhs, Viewport const & rhs );
	};

	inline bool operator==( Viewport const & lhs, Viewport const & rhs )
	{
		return lhs.m_offset.x == rhs.m_offset.x
			&& lhs.m_offset.y == rhs.m_offset.y
			&& lhs.m_size.width == rhs.m_size.width
			&& lhs.m_size.height == rhs.m_size.height
			&& lhs.m_depthBounds == rhs.m_depthBounds;
	}

	inline bool operator!=( Viewport const & lhs, Viewport const & rhs )
	{
		return !( lhs == rhs );
	}
}

#endif
