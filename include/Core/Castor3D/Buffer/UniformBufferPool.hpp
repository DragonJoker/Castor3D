/*
See LICENSE file in root folder
*/
#ifndef ___C3D_UniformBufferPool_HPP___
#define ___C3D_UniformBufferPool_HPP___

#include "UniformBuffer.hpp"

#include <Design/OwnedBy.hpp>

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		22/08/2017
	\~english
	\brief		A GpuBuffer and an offset from the GpuBuffer.
	\~french
	\brief		Un GpuBuffer et un offset dans le GpuBuffer.
	*/
	template< typename T >
	struct UniformBufferOffset
	{
		UniformBuffer< T > * buffer;
		ashes::MemoryPropertyFlags flags;
		uint32_t offset;

		explicit operator bool()const
		{
			return buffer->hasBuffer();
		}

		T const & getData()const
		{
			return buffer->getBuffer().getData( offset );
		}

		T & getData()
		{
			return buffer->getBuffer().getData( offset );
		}
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.11.0
	\date		08/02/2018
	\~english
	\brief		Uniform buffer pool implementation.
	\~french
	\brief		Implémentation d'un pool de tampon d'uniformes.
	*/
	template< typename T >
	class UniformBufferPool
		: public castor::OwnedBy< RenderSystem >
	{
	public:
		using BufferArray = std::vector< std::unique_ptr< UniformBuffer< T > > >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	renderSystem	The RenderSystem.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	renderSystem	Le RenderSystem.
		 */
		explicit UniformBufferPool( RenderSystem & renderSystem );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~UniformBufferPool();
		/**
		 *\~english
		 *\brief		Cleans up all GPU buffers.
		 *\~french
		 *\brief		Nettoie tous les tampons GPU.
		 */
		void cleanup();
		/**
		 *\~english
		 *\brief		Uploads all GPU buffers to VRAM.
		 *\param[in]	timer	The render pass timer.
		 *\param[in]	index	The render pass index.
		 *\~french
		 *\brief		Met à jour tous les tampons GPU en VRAM.
		 *\param[in]	timer	Le timer de passe de rendu.
		 *\param[in]	index	L'indice de passe de rendu.
		 */
		void upload( RenderPassTimer & timer
			, uint32_t index )const;
		/**
		 *\~english
		 *\brief		Retrieves a uniform buffer.
		 *\param[in]	flags	The buffer memory flags.
		 *\return		The uniform buffer.
		 *\~french
		 *\brief		Récupère un tampon d'uniformes.
		 *\param[in]	flags	Les indicateurs de mémoire du tampon.
		 *\return		Le tampon d'uniformes.
		 */
		UniformBufferOffset< T > getBuffer( ashes::MemoryPropertyFlags flags );
		/**
		 *\~english
		 *\brief		Releases a GPU buffer.
		 *\param[in]	bufferOffset	The buffer offset to release.
		 *\~french
		 *\brief		Libère un tampon GPU.
		 *\param[in]	bufferOffset	Le tampon à libérer.
		 */
		void putBuffer( UniformBufferOffset< T > const & bufferOffset );
		/**
		 *\~english
		 *\return		The pool buffers count.
		 *\~french
		 *\return		Le nombre de tampons du pool.
		 */
		uint32_t getBufferCount()const;

	private:
		typename BufferArray::iterator doFindBuffer( BufferArray & array );

	private:
		uint32_t m_maxCount{ 0u };
		uint32_t m_maxSize{ 0u };
		ashes::StagingBufferPtr m_stagingBuffer;
		ashes::CommandBufferPtr m_uploadCommandBuffer;
		ashes::FencePtr m_uploadFence;
		std::map< uint32_t, BufferArray > m_buffers;
	};
}

#include "UniformBufferPool.inl"

#endif
