/*
See LICENSE file in root folder
*/
#ifndef ___C3D_UniformBufferPool_HPP___
#define ___C3D_UniformBufferPool_HPP___

#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Buffer/PoolUniformBuffer.hpp"
#include "Castor3D/Buffer/UniformBufferOffset.hpp"

#include <CastorUtils/Design/OwnedBy.hpp>

#include <ashespp/Buffer/StagingBuffer.hpp>

namespace castor3d
{
	class UniformBufferPool
		: public castor::OwnedBy< RenderSystem >
	{
	public:
		struct Buffer
		{
			uint32_t index;
			PoolUniformBufferUPtr buffer;
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
		C3D_API UniformBufferPool( RenderSystem & renderSystem
			, RenderDevice const & device
			, castor::String debugName );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~UniformBufferPool();
		/**
		 *\~english
		 *\brief		Cleans up all GPU buffers.
		 *\~french
		 *\brief		Nettoie tous les tampons GPU.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Uploads all GPU buffers to VRAM.
		 *\~french
		 *\brief		Met à jour tous les tampons GPU en VRAM.
		 */
		C3D_API void upload( ashes::CommandBuffer const & cb )const;
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
		template< typename DataT >
		UniformBufferOffsetT< DataT > getBuffer( VkMemoryPropertyFlags flags );
		/**
		 *\~english
		 *\brief		Releases a GPU buffer.
		 *\param[in]	bufferOffset	The buffer offset to release.
		 *\~french
		 *\brief		Libère un tampon GPU.
		 *\param[in]	bufferOffset	Le tampon à libérer.
		 */
		template< typename DataT >
		void putBuffer( UniformBufferOffsetT< DataT > const & bufferOffset );
		/**
		 *\~english
		 *\return		The pool buffers count.
		 *\~french
		 *\return		Le nombre de tampons du pool.
		 */
		C3D_API uint32_t getBufferCount()const;

	private:
		C3D_API BufferArray::iterator doFindBuffer( BufferArray & array
			, VkDeviceSize alignedSize );
		C3D_API void doCreateStagingBuffer();
		C3D_API BufferArray::iterator doCreatePoolBuffer( VkMemoryPropertyFlags flags
			, UniformBufferPool::BufferArray & buffers );

	private:
		RenderDevice const & m_device;
		uint32_t m_maxUboElemCount{ 0u };
		uint32_t m_maxUboSize{ 0u };
		uint32_t m_maxPoolUboCount{ 10u };
		uint32_t m_currentUboIndex{ 0u };
		ashes::StagingBufferPtr m_stagingBuffer;
		uint8_t * m_stagingData;
		std::map< uint32_t, BufferArray > m_buffers;
		castor::String m_debugName;
	};
}

#include "Castor3D/Buffer/UniformBufferPool.inl"

#endif
