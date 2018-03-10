/**
*\file
*	RenderingResources.h
*\author
*	Sylvain Doremus
*/
#ifndef ___VkRenderer_Buffer_HPP___
#define ___VkRenderer_Buffer_HPP___
#pragma once

#include "VkRendererPrerequisites.hpp"

#include <Buffer/Buffer.hpp>

namespace vk_renderer
{
	/**
	*\~french
	*\brief
	*	Classe regroupant les ressources de rendu nécessaires au dessin d'une image.
	*/
	class Buffer
		: public renderer::BufferBase
	{
	public:
		/**
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le périphérique logique.
		*\param[in] count
		*	Le nombre d'éléments du tampon.
		*\param[in] target
		*	Les indicateurs d'utilisation du tampon.
		*\~english
		*\brief
		*	Constructor.
		*\param[in] device
		*	The logical connection to the GPU.
		*\param[in] size
		*	The buffer data size.
		*\param[in] target
		*	The buffer usage flags.
		*/
		Buffer( Device const & device
			, uint32_t size
			, renderer::BufferTargets target );
		/**
		*\~french
		*\brief
		*	Destructeur.
		*\~english
		*\brief
		*	Destructor.
		*/
		~Buffer();
		/**
		*\copydoc	renderer::BufferBase::getMemoryRequirements
		*/
		renderer::MemoryRequirements getMemoryRequirements()const override;
		/**
		*\copydoc	renderer::BufferBase::makeTransferDestination
		*/
		renderer::BufferMemoryBarrier makeTransferDestination()const override;
		/**
		*\copydoc	renderer::BufferBase::makeTransferSource
		*/
		renderer::BufferMemoryBarrier makeTransferSource()const override;
		/**
		*\copydoc	renderer::BufferBase::makeVertexShaderInputResource
		*/
		renderer::BufferMemoryBarrier makeVertexShaderInputResource()const override;
		/**
		*\copydoc	renderer::BufferBase::makeUniformBufferInput
		*/
		renderer::BufferMemoryBarrier makeUniformBufferInput()const override;
		/**
		*\copydoc	renderer::BufferBase::makeMemoryTransitionBarrier
		*/
		renderer::BufferMemoryBarrier makeMemoryTransitionBarrier( renderer::AccessFlags dstAccess )const override;
		/**
		*\~french
		*\brief
		*	Opérateur de conversion implicite vers VkBuffer.
		*\~english
		*\brief
		*	VkBuffer implicit cast operator.
		*/
		inline operator VkBuffer const &( )const
		{
			return m_buffer;
		}

	private:
		void doBindMemory()override;

	private:
		Device const & m_device;
		uint32_t m_size{ 0u };
		VkBuffer m_buffer{ VK_NULL_HANDLE };
		mutable VkAccessFlags m_currentAccessMask{ VK_ACCESS_MEMORY_WRITE_BIT };
	};
}

#endif
