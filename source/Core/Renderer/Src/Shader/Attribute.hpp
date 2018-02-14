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
	*\~french
	*\brief
	*	Classe encapsulant le concept d'attribut de sommet.
	*/
	class Attribute
	{
	public:
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] layout
		*	The parent vertex layout.
		*\param[in] format
		*	The attribute format, in the shader.
		*\param[in] location
		*	The attribute location in the shader.
		*\param[in] offset
		*	The attribute location in the buffer.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] layout
		*	Le layout de sommets parent.
		*\param[in] format
		*	Le format de l'attribut dans le tampon.
		*\param[in] location
		*	La position de l'attribut dans le shader.
		*\param[in] offset
		*	La position de l'attribut dans le tampon.
		*/
		Attribute( VertexLayout const & layout
			, AttributeFormat format
			, uint32_t location
			, uint32_t offset );
		/**
		*\~english
		*\brief
		*	Destructor.
		*\~french
		*\brief
		*	Destructeur.
		*/
		virtual ~Attribute() = default;
		/**
		*\~french
		*\return
		*	Le format de l'attribut dans le tampon.
		*\~french
		*\return
		*	Le format de l'attribut dans le tampon.
		*/
		inline AttributeFormat getFormat()const
		{
			return m_format;
		}
		/**
		*\~french
		*\return
		*	The attribute location in the shader.
		*\~french
		*\return
		*	La position de l'attribut dans le shader.
		*/
		inline uint32_t getLocation()const
		{
			return m_location;
		}
		/**
		*\~french
		*\return
		*	The attribute location in the buffer.
		*\~french
		*\return
		*	La position de l'attribut dans le tampon.
		*/
		inline uint32_t getOffset()const
		{
			return m_offset;
		}
		/**
		*\~french
		*\return
		*	The parent vertex layout.
		*\~french
		*\return
		*	Le layout de sommets parent.
		*/
		inline VertexLayout const & getLayout()const
		{
			return m_layout;
		}

	private:
		VertexLayout const & m_layout;
		AttributeFormat m_format;
		uint32_t m_location;
		uint32_t m_offset;
	};
}

#endif
