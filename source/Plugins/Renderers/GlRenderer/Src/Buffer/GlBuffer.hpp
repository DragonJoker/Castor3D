/**
*\file
*	RenderingResources.h
*\author
*	Sylvain Doremus
*/
#ifndef ___VkRenderer_Buffer_HPP___
#define ___VkRenderer_Buffer_HPP___
#pragma once

#include "GlRendererPrerequisites.hpp"

#include <Buffer/Buffer.hpp>

namespace gl_renderer
{
	/**
	*\brief
	*	Classe regroupant les ressources de rendu nécessaires au dessin d'une image.
	*/
	class Buffer
		: public renderer::BufferBase
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le périphérique logique.
		*\param[in] count
		*	Le nombre d'éléments du tampon.
		*\param[in] target
		*	Les indicateurs d'utilisation du tampon.
		*\param[in] flags
		*	Les indicateurs de mémoire du tampon.
		*/
		Buffer( renderer::Device const & device
			, uint32_t size
			, renderer::BufferTargets target
			, renderer::MemoryPropertyFlags flags );
		/**
		*\brief
		*	Destructeur.
		*/
		~Buffer();
		/**
		*\brief
		*	Mappe la mémoire du tampon en RAM.
		*\param[in] offset
		*	L'offset à partir duquel la mémoire du tampon est mappée.
		*\param[in] size
		*	La taille en octets de la mémoire à mapper.
		*\param[in] flags
		*	Indicateurs de configuration du mapping.
		*\return
		*	\p nullptr si le mapping a échoué.
		*/
		uint8_t * lock( uint32_t offset
			, uint32_t size
			, renderer::MemoryMapFlags flags )const override;
		/**
		*\brief
		*	Unmappe la mémoire du tampon de la RAM.
		*\param[in] size
		*	La taille en octets de la mémoire mappée.
		*\param[in] modified
		*	Dit si le tampon a été modifié, et donc si la VRAM doit être mise à jour.
		*/
		void unlock( uint32_t size
			, bool modified )const override;
		/**
		*\brief
		*	Prépare une barrière mémoire de transition vers un layout de destination de transfert.
		*\return
		*	La barrière mémoire.
		*/
		renderer::BufferMemoryBarrier makeTransferDestination()const override;
		/**
		*\brief
		*	Prépare une barrière mémoire de transition vers un layout de source de transfert.
		*\return
		*	La barrière mémoire.
		*/
		renderer::BufferMemoryBarrier makeTransferSource()const override;
		/**
		*\brief
		*	Prépare une barrière mémoire de transition vers un layout de ressource d'entrée (lecture seule) d'un vertex shader.
		*\return
		*	La barrière mémoire.
		*/
		renderer::BufferMemoryBarrier makeVertexShaderInputResource()const override;
		/**
		*\brief
		*	Prépare une barrière mémoire de transition vers un layout de ressource d'entrée (lecture seule) d'un shader.
		*\return
		*	La barrière mémoire.
		*/
		renderer::BufferMemoryBarrier makeUniformBufferInput()const override;
		/**
		*\brief
		*	Prépare une barrière mémoire de transition vers un layout mémoire donné.
		*\param[in] dstAccess
		*	Les indicateurs d'accès voulus après la transition.
		*\return
		*	La barrière mémoire.
		*/
		renderer::BufferMemoryBarrier makeMemoryTransitionBarrier( renderer::AccessFlags dstAccess )const override;
		/**
		*\return
		*	Le tampon.
		*/
		inline GLuint getBuffer()const
		{
			assert( m_name != GL_INVALID_INDEX );
			return m_name;
		}
		/**
		*\return
		*	La cible du tampon.
		*/
		inline GlBufferTarget getTarget()const
		{
			return m_target;
		}

	private:
		GLuint m_name{ GL_INVALID_INDEX };
		GlBufferTarget m_target;
		mutable GlBufferTarget m_copyTarget;
	};
}

#endif
