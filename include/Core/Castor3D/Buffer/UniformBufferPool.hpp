/*
See LICENSE file in root folder
*/
#ifndef ___C3D_UniformBufferPool_HPP___
#define ___C3D_UniformBufferPool_HPP___

#include "Castor3D/Buffer/UniformBuffer.hpp"

#include <ashespp/Buffer/StagingBuffer.hpp>

namespace castor3d
{
	template< typename T >
	struct UniformBufferOffset
	{
		PoolUniformBuffer< T > * buffer{ nullptr };
		VkMemoryPropertyFlags flags{ 0u };
		uint32_t offset{ 0u };

		explicit operator bool()const
		{
			return buffer
				&& buffer->hasBuffer();
		}

		T const & getData()const
		{
			return buffer->getData( offset );
		}

		T & getData()
		{
			return buffer->getData( offset );
		}

		uint32_t getAlignedSize()const
		{
			return buffer->getAlignedSize();
		}
	};

	template< typename T >
	class UniformBufferPool
		: public castor::OwnedBy< RenderSystem >
	{
	public:
		struct Buffer
		{
			uint32_t index;
			PoolUniformBufferUPtr< T > buffer;
		};
		using BufferArray = std::vector< Buffer >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	renderSystem	The RenderSystem.
		 *\param[in]	debugName		The buffer debug name.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	renderSystem	Le RenderSystem.
		 *\param[in]	debugName		Le nom debug du tampon.
		 */
		UniformBufferPool( RenderSystem & renderSystem
			, castor::String debugName );
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
		 *\~french
		 *\brief		Met à jour tous les tampons GPU en VRAM.
		 */
		void upload( ashes::CommandBuffer const & cb )const;
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
		void upload( ashes::CommandBuffer const & cb
			, RenderPassTimer & timer
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
		UniformBufferOffset< T > getBuffer( VkMemoryPropertyFlags flags );
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
		uint32_t m_maxUboElemCount{ 0u };
		uint32_t m_maxUboSize{ 0u };
		uint32_t m_maxPoolUboCount{ 10u };
		uint32_t m_currentUboIndex{ 0u };
		ashes::StagingBufferPtr m_stagingBuffer;
		T * m_stagingData;
		std::map< uint32_t, BufferArray > m_buffers;
		castor::String m_debugName;
	};
}

#include "Castor3D/Buffer/UniformBufferPool.inl"

#endif
