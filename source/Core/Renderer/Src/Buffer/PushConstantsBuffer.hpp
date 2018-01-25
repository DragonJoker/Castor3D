/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_PushConstantsBuffer_HPP___
#define ___Renderer_PushConstantsBuffer_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	A variable for a push constants buffer.
	*\~french
	*\brief
	*	Une variable pour un tampon de push constants.
	*/
	struct PushConstant
	{
		/**
		*\~english
		*\brief
		*	The variable location in the shader.
		*\~french
		*\brief
		*	La position de la variable dans le shader.
		*/
		uint32_t location;
		/**
		*\~english
		*\brief
		*	The offset in the buffer.
		*\~french
		*\brief
		*	Le décalage dans le tampon.
		*/
		uint32_t offset;
		/**
		*\~english
		*\brief
		*	The variable format, the size of the variable is deduced from that.
		*\~french
		*\brief
		*	Le nom de la variable, la taille de la variable est déduite de là.
		*/
		AttributeFormat format;
	};
	/**
	*\~english
	*\brief
	*	Wraps the push constants concept.
	*\remarks
	*	With OpenGL it will become a set of uniform variables, while in Vulkan it will become a push constants bloc.
	*\~french
	*\brief
	*	Wrappe le concept de push constants.
	*\remarks
	*	En OpenGL il sera traité comme un ensemble de variables uniformes, alors q'en Vulkan ce sera un bloc de push constants.
	*/
	class PushConstantsBuffer
	{
	public:
		/**
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le périphérique logique.
		*\param[in] stageFlags
		*	Spécifie les niveaux de shaders qui vont utiliser les push constants dans l'intervalle mis à jour.
		*\param[in] variables
		*	Les variables contenues dans le tampon.
		*\~english
		*\brief
		*	Constructor.
		*\param[in] device
		*	The logical device.
		*\param[in] stageFlags
		*	Specifies the shader stages that will use the push constants in the updated range.
		*\param[in] variables
		*	The constants contained in the buffer.
		*/
		PushConstantsBuffer( ShaderStageFlags stageFlags
			, PushConstantArray const & variables );
		/**
		*\~english
		*\return
		*	The base offset.
		*\~french
		*\return
		*	Le décalage de base.
		*/
		inline uint32_t getOffset()const
		{
			return m_offset;
		}
		/**
		*\~english
		*\return
		*	The data size.
		*\~french
		*\return
		*	La taille des données.
		*/
		inline uint32_t getSize()const
		{
			return uint32_t( m_data.size() );
		}
		/**
		*\~english
		*\return
		*	The shader stages that will use the push constants in the updated range.
		*\~french
		*\return
		*	Les niveaux de shaders qui vont utiliser les push constants dans l'intervalle mis à jour.
		*/
		inline ShaderStageFlags getStageFlags()const
		{
			return m_stageFlags;
		}
		/**
		*\~english
		*\brief
		*	A pointer to the buffer data.
		*\~french
		*\brief
		*	Un pointeur sur les données du tampon.
		*/
		inline uint8_t const * getData()const
		{
			return m_data.data();
		}
		/**
		*\~english
		*\brief
		*	A pointer to the buffer data.
		*\~french
		*\brief
		*	Un pointeur sur les données du tampon.
		*/
		inline uint8_t * getData()
		{
			return m_data.data();
		}
		/**
		*\return
		*	Le début du tableau de constantes.
		*/
		inline PushConstantArray::const_iterator begin()const
		{
			return m_variables.begin();
		}
		/**
		*\return
		*	La fin du tableau de constantes.
		*/
		inline PushConstantArray::const_iterator end()const
		{
			return m_variables.end();
		}

	protected:
		ShaderStageFlags m_stageFlags;
		PushConstantArray m_variables;
		uint32_t m_offset;
		renderer::ByteArray m_data;
	};
}

#endif
