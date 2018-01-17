/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_Attribute_HPP___
#define ___Renderer_Attribute_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\brief
	*	Classe encapsulant le concept d'attribut de sommet.
	*/
	class AttributeBase
	{
	protected:
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
		AttributeBase( VertexLayout & layout
			, AttributeFormat format
			, uint32_t location
			, uint32_t offset );

	public:
		/**
		*\brief
		*	Destructeur.
		*/
		virtual ~AttributeBase() = default;
		/**
		*\return
		*	Le format de l'attribut dans le tampon.
		*/
		inline AttributeFormat getFormat()const
		{
			return m_format;
		}
		/**
		*\return
		*	La position de l'attribut dans le shader.
		*/
		inline uint32_t getLocation()const
		{
			return m_location;
		}
		/**
		*\return
		*	La position de l'attribut dans le tampon.
		*/
		inline uint32_t getOffset()const
		{
			return m_offset;
		}

	private:
		AttributeFormat m_format;
		uint32_t m_location;
		uint32_t m_offset;
	};
}

#endif
