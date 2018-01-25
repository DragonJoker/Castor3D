/**
*\file
*	UniformBuffer.h
*\author
*	Sylvain Doremus
*/
#ifndef ___VkRenderer_UniformBuffer_HPP___
#define ___VkRenderer_UniformBuffer_HPP___
#pragma once

#include "Core/GlDevice.hpp"

#include <Buffer/UniformBuffer.hpp>

namespace gl_renderer
{
	/**
	*\brief
	*	Tampon de variables uniformes.
	*/
	class UniformBuffer
		: public renderer::UniformBufferBase
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le périphérique logique.
		*\param[in] count
		*	Le nombre d'instance des données.
		*\param[in] target
		*	Les indicateurs d'utilisation du tampon.
		*\param[in] flags
		*	Les indicateurs de mémoire du tampon.
		*/
		UniformBuffer( renderer::Device const & device
			, uint32_t count
			, uint32_t size
			, renderer::BufferTargets target
			, renderer::MemoryPropertyFlags flags );
		/**
		*\brief
		*	Récupère l'offset dans le buffer pour un nombre d'éléments donné.
		*\param[in] count
		*	Le nombre d'éléments.
		*\return
		*	L'offset réel.
		*/
		uint32_t getOffset( uint32_t count )const
		{
			return count * getSize();
		}

	protected:
		/**
		*\brief
		*	Crée le tampon GPU.
		*\param[in] count
		*	Le nombre d'instance des données.
		*\param[in] target
		*	Les indicateurs d'utilisation du tampon.
		*\param[in] flags
		*	Les indicateurs de mémoire du tampon.
		*/
		void doCreateBuffer( uint32_t count
			, renderer::BufferTargets target
			, renderer::MemoryPropertyFlags flags )override;
	};
}

#endif
