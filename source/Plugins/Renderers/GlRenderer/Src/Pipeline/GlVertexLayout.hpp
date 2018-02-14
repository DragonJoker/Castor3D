/**
*\file
*	VertexBuffer.h
*\author
*	Sylvain Doremus
*/
#ifndef ___GlRenderer_VertexLayout_HPP___
#define ___GlRenderer_VertexLayout_HPP___
#pragma once

#include "GlRendererPrerequisites.hpp"

#include <Pipeline/VertexLayout.hpp>

#include <vector>

namespace gl_renderer
{
	/**
	*\brief
	*	Layout de sommet.
	*/
	class VertexLayout
		: public renderer::VertexLayout
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] bindingSlot
		*	Le point d'attache du tampon associé.
		*\param[in] stride
		*	La taille en octets séparant un élément du suivant, dans le tampon.
		*/
		VertexLayout( uint32_t bindingSlot
			, uint32_t stride
			, renderer::VertexInputRate inputRate );
	};
}

#endif
