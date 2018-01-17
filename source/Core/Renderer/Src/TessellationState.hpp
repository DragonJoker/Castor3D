/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_TessellationState_HPP___
#define ___Renderer_TessellationState_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\~french
	*\brief
	*	Un état de tessellation.
	*/
	class TessellationState
	{
	public:
		TessellationState( TessellationStateFlags flags = 0u
			, uint32_t patchControlPoints = 0u );
		/**
		*\return
		*	Les indicateurs de l'état.
		*/
		inline TessellationStateFlags const & getFlags()const
		{
			return m_flags;
		}
		/**
		*\return
		*	Le nombre de points de contrôle par patch.
		*/
		inline uint32_t const & getControlPoints()const
		{
			return m_patchControlPoints;
		}

	private:
		TessellationStateFlags m_flags;
		uint32_t m_patchControlPoints;
	};
}

#endif
