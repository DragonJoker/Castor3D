/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#pragma once

#include "VkRendererPrerequisites.hpp"

#include <Shader/AttributeBase.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Classe encapsulant le concept d'attribut de sommet.
	*/
	class Attribute
		: public renderer::AttributeBase
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] layout
		*	Le layout de sommets associé.
		*\param[in] location
		*	La position de l'attribut dans le shader.
		*\param[in] offset
		*	La position de l'attribut dans le tampon.
		*/
		Attribute( VertexLayout & layout
			, renderer::AttributeFormat format
			, uint32_t location
			, uint32_t offset );
		/**
		*\~french
		*\brief
		*	Opérateur de conversion implicite vers VkVertexInputAttributeDescription.
		*\~english
		*\brief
		*	VkVertexInputAttributeDescription implicit cast operator.
		*/
		inline operator VkVertexInputAttributeDescription const &( )const
		{
			return m_description;
		}

	private:
		VkVertexInputAttributeDescription m_description;
	};
}
