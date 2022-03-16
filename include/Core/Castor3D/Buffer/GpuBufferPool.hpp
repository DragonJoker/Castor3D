/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GpuBufferPool_HPP___
#define ___C3D_GpuBufferPool_HPP___

#include "Castor3D/Buffer/GpuBufferOffset.hpp"

#include <CastorUtils/Design/OwnedBy.hpp>

#include <ashespp/Buffer/StagingBuffer.hpp>

namespace castor3d
{
	class GpuBufferPool
		: public castor::OwnedBy< RenderSystem >
	{
	public:
		using BufferArray = std::vector< std::unique_ptr< GpuBuddyBuffer > >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	renderSystem	The RenderSystem.
		 *\param[in]	device			The GPU device.
		 *\param[in]	debugName		The debug name.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	renderSystem	Le RenderSystem.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	debugName		Le nom debug.
		 */
		C3D_API GpuBufferPool( RenderDevice const & device
			, castor::String debugName );
		/**
		 *\~english
		 *\brief		Cleans up all GPU buffers.
		 *\~french
		 *\brief		Nettoie tous les tampons GPU.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Uploads all ready memory ranges to VRAM.
		 *\param[in]	cb	The command buffer on which transfer commands are recorded.
		 *\~french
		 *\brief		Met à jour tous les intervalles mémoire prêts en VRAM.
		 *\param[in]	cb	Le command buffer sur lequel les commandes de transfert sont enregistrées.
		 */
		C3D_API void upload( ashes::CommandBuffer const & cb );
		/**
		 *\~english
		 *\brief		Retrieves a GPU buffer with the given size.
		 *\param[in]	target	The buffer type.
		 *\param[in]	count	The wanted buffer element count.
		 *\param[in]	flags	The buffer memory flags.
		 *\return		The GPU buffer.
		 *\~french
		 *\brief		Récupère un tampon GPU avec la taille donnée.
		 *\param[in]	target	Le type de tampon.
		 *\param[in]	count	Le nombre d'éléments voulu pour le tampon.
		 *\param[in]	flags	Les indicateurs de mémoire du tampon.
		 *\return		Le tampon GPU.
		 */
		template< typename DataT >
		GpuBufferOffsetT< DataT > getBuffer( VkBufferUsageFlags target
			, VkDeviceSize count
			, VkMemoryPropertyFlags flags );
		/**
		 *\~english
		 *\brief		Releases a GPU buffer.
		 *\param[in]	bufferOffset	The buffer offset to release.
		 *\~french
		 *\brief		Libère un tampon GPU.
		 *\param[in]	bufferOffset	Le tampon à libérer.
		 */
		template< typename DataT >
		void putBuffer( GpuBufferOffsetT< DataT > const & bufferOffset );

	private:
		C3D_API BufferArray::iterator doFindBuffer( VkDeviceSize size
			, BufferArray & array );
		C3D_API uint32_t doMakeKey( VkBufferUsageFlags target
			, VkMemoryPropertyFlags flags );
		C3D_API GpuBufferBase & doGetBuffer( VkDeviceSize size
			, VkBufferUsageFlags target
			, VkMemoryPropertyFlags memory
			, MemChunk & chunk );
		C3D_API void doPutBuffer( GpuBufferBase const & buffer
			, VkBufferUsageFlags target
			, VkMemoryPropertyFlags memory
			, MemChunk const & chunk );

	private:
		RenderDevice const & m_device;
		castor::String m_debugName;
		std::map< uint32_t, BufferArray > m_buffers;
		BufferArray m_nonSharedBuffers;
		uint32_t m_minBlockSize{};
	};
}

#include "Castor3D/Buffer/GpuBufferPool.inl"

#endif
