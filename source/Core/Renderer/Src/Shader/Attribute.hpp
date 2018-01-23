/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_Attribute_HPP___
#define ___Renderer_Attribute_HPP___
#pragma once

#include "Shader/AttributeBase.hpp"

namespace renderer
{
	/**
	*\brief
	*	Classe encapsulant le concept d'attribut de sommet.
	*/
	template< typename T >
	class Attribute
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] layout
		*	Le layout de sommets associé.
		*\param[in] format
		*	Le format de l'attribut dans le tampon.
		*\param[in] location
		*	La position de l'attribut dans le shader.
		*\param[in] offset
		*	La position de l'attribut dans le tampon.
		*/
		inline Attribute( VertexLayout & layout
			, uint32_t location
			, uint32_t offset );
		/**
		*\return
		*	L'attribut.
		*/
		inline AttributeBase const & getAttribute()const
		{
			return *m_attribute;
		}

	private:
		AttributeBasePtr m_attribute;
	};
}

#include "Attribute.inl"

#endif
