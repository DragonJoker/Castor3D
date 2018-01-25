/**
*\file
*	VertexBuffer.h
*\author
*	Sylvain Doremus
*/
#ifndef ___VkRenderer_VertexLayout_HPP___
#define ___VkRenderer_VertexLayout_HPP___
#pragma once

#include "Shader/GlAttribute.hpp"

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
			, uint32_t stride );
		/**
		*\brief
		*	Crée un attribut de sommet.
		*\param[in] location
		*	La position de l'attribut dans le shader.
		*\param[in] offset
		*	La position de l'attribut dans le tampon.
		*/
		renderer::AttributeBase createAttribute( uint32_t location
			, renderer::AttributeFormat format
			, uint32_t offset )override;
		/**
		*\return
		*	Le début du tableau d'attributs.
		*/
		inline std::vector< AttributeBase >::const_iterator begin()const
		{
			return m_attributes.begin();
		}
		/**
		*\return
		*	La fin du tableau d'attributs.
		*/
		inline std::vector< AttributeBase >::const_iterator end()const
		{
			return m_attributes.end();
		}

	private:
		std::vector< AttributeBase > m_attributes;
	};
}

#endif
