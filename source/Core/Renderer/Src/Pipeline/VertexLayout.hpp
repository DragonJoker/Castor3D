/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_VertexLayout_HPP___
#define ___Renderer_VertexLayout_HPP___
#pragma once

#include "Core/Device.hpp"
#include "Shader/Attribute.hpp"
#include "Shader/FormatGetter.hpp"

#include <vector>

namespace renderer
{
	/**
	*\~french
	*\brief
	*	The layout of a vertex inside a vertex buffer.
	*\~french
	*\brief
	*	Layout d'un sommet dans un tampon de sommets.
	*/
	class VertexLayout
	{
	protected:
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] bindingSlot
		*	The binding slot of the associated buffer.
		*\param[in] inputRate
		*	The layout input rate.
		*\param[in] stride
		*	The byte size to go from an element to the next one, in the buffer.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] bindingSlot
		*	Le point d'attache du tampon associé.
		*\param[in] inputRate
		*	La cadence d'entrée.
		*\param[in] stride
		*	La taille en octets séparant un élément du suivant, dans le tampon.
		*/
		VertexLayout( uint32_t bindingSlot
			, uint32_t stride
			, VertexInputRate inputRate );

	public:
		/**
		*\~english
		*\brief
		*	Destructor.
		*\~french
		*\brief
		*	Destructeur.
		*/
		virtual ~VertexLayout() = default;
		/**
		*\~english
		*\brief
		*	Creates a vertex attribute.
		*\param[in] location
		*	The attribute location, in the shader.
		*\param[in] format
		*	The attribute format, in the shader.
		*\param[in] offset
		*	The attribute location, in the buffer.
		*\~french
		*\brief
		*	Crée un attribut de sommet.
		*\param[in] location
		*	La position de l'attribut, dans le shader.
		*\param[in] format
		*	Le format de l'attribut, dans le shader.
		*\param[in] offset
		*	La position de l'attribut, dans le tampon.
		*/
		Attribute createAttribute( uint32_t location
			, AttributeFormat format
			, uint32_t offset );
		/**
		*\~english
		*\brief
		*	Creates a vertex attribute.
		*\param[in] location
		*	The attribute location in the shader.
		*\param[in] offset
		*	The attribute location in the buffer.
		*\~french
		*\brief
		*	Crée un attribut de sommet.
		*\param[in] location
		*	La position de l'attribut dans le shader.
		*\param[in] offset
		*	La position de l'attribut dans le tampon.
		*/
		template< typename T >
		inline Attribute createAttribute( uint32_t location
			, uint32_t offset )
		{
			return createAttribute( location
				, details::FormatGetter< T >::value
				, offset );
		}
		/**
		*\~french
		*\return
		*	The binding slot of the associated buffer.
		*\~french
		*\return
		*	Le point d'attache du tampon associé.
		*/
		inline uint32_t getBindingSlot()const
		{
			return m_bindingSlot;
		}
		/**
		*\~english
		*\return
		*	The byte size to go from an element to the next one, in the buffer.
		*\~french
		*\return
		*	La taille en octets séparant un élément du suivant, dans le tampon.
		*/
		inline uint32_t getStride()const
		{
			return m_stride;
		}
		/**
		*\~english
		*\return
		*	The input rate.
		*\~french
		*\return
		*	La cadence d'entrée.
		*/
		inline VertexInputRate getInputRate()const
		{
			return m_inputRate;
		}
		/**
		*\~french
		*\return
		*	Le début du tableau d'attributs.
		*\~english
		*\return
		*	The attributes array beginning.
		*/
		inline auto begin()const
		{
			return m_attributes.begin();
		}
		/**
		*\~french
		*\return
		*	Le début du tableau d'attributs.
		*\~english
		*\return
		*	The attributes array beginning.
		*/
		inline auto begin()
		{
			return m_attributes.begin();
		}
		/**
		*\~french
		*\return
		*	La fin du tableau d'attributs.
		*\~english
		*\return
		*	The attributes array end.
		*/
		inline auto end()const
		{
			return m_attributes.end();
		}
		/**
		*\~french
		*\return
		*	La fin du tableau d'attributs.
		*\~english
		*\return
		*	The attributes array end.
		*/
		inline auto end()
		{
			return m_attributes.end();
		}

	private:

	private:
		AttributeArray m_attributes;
		uint32_t m_bindingSlot;
		uint32_t m_stride;
		VertexInputRate m_inputRate;
	};
	/**
	*\~english
	*\brief
	*	Creates a vertex layout.
	*\param[in] device
	*	The logical device, that will effectively create the vertex layout.
	*\param[in] bindingSlot
	*	The binding slot of the associated buffer.
	*\param[in] inputRate
	*	The layout input rate.
	*\~french
	*\brief
	*	Crée un layout de sommet.
	*\param[in] device
	*	Le périphérique logique, qui va effectivement créer le layout de sommet.
	*\param[in] bindingSlot
	*	Le point d'attache du tampon associé.
	*\param[in] inputRate
	*	La cadence d'entrée.
	*/
	template< typename T >
	VertexLayoutPtr makeLayout( Device const & device
		, uint32_t bindingSlot
		, VertexInputRate inputRate = VertexInputRate::eVertex )
	{
		return device.createVertexLayout( bindingSlot
			, uint32_t( sizeof( T ) )
			, inputRate );
	}
}

#endif
