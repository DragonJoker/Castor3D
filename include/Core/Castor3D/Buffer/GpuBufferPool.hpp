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
	/**
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		22/08/2017
	\~english
	\brief		Pool for vertex and index buffers.
	\~french
	\brief		Pool pour les tampons de sommets et d'indices.
	*/
	class GpuBufferPool
		: public castor::OwnedBy< RenderSystem >
	{
	public:
		using BufferArray = std::vector< std::unique_ptr< GpuBuffer > >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	renderSystem	The RenderSystem.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	renderSystem	Le RenderSystem.
		 */
		C3D_API explicit GpuBufferPool( RenderSystem & renderSystem
			, RenderDevice const & device
			, castor::String debugName );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~GpuBufferPool();
		/**
		 *\~english
		 *\brief		Cleans up all GPU buffers.
		 *\~french
		 *\brief		Nettoie tous les tampons GPU.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Retrieves a GPU buffer with the given size.
		 *\param[in]	target	The buffer type.
		 *\param[in]	size	The wanted buffer size.
		 *\param[in]	flags	The buffer memory flags.
		 *\return		The GPU buffer.
		 *\~french
		 *\brief		Récupère un tampon GPU avec la taille donnée.
		 *\param[in]	target	Le type de tampon.
		 *\param[in]	size	La taille voulue pour le tampon.
		 *\param[in]	flags	Les indicateurs de mémoire du tampon.
		 *\return		Le tampon GPU.
		 */
		template< typename DataT >
		GpuBufferOffsetT< DataT > getBuffer( VkBufferUsageFlagBits target
			, VkDeviceSize count
			, VkMemoryPropertyFlags flags );
		/**
		 *\~english
		 *\brief		Releases a GPU buffer.
		 *\param[in]	target			The buffer type.
		 *\param[in]	bufferOffset	The buffer offset to release.
		 *\~french
		 *\brief		Libère un tampon GPU.
		 *\param[in]	target			Le type de tampon.
		 *\param[in]	bufferOffset	Le tampon à libérer.
		 */
		template< typename DataT >
		void putBuffer( GpuBufferOffsetT< DataT > const & bufferOffset );

	private:
		C3D_API BufferArray::iterator doFindBuffer( VkDeviceSize size
			, BufferArray & array );
		C3D_API uint32_t doMakeKey( VkBufferUsageFlagBits target
			, VkMemoryPropertyFlags flags );
		C3D_API GpuBuffer & doGetBuffer( VkDeviceSize size
			, VkBufferUsageFlagBits target
			, VkMemoryPropertyFlags memory
			, MemChunk & chunk );
		C3D_API void doPutBuffer( GpuBuffer const & buffer
			, VkBufferUsageFlagBits target
			, VkMemoryPropertyFlags memory
			, MemChunk const & chunk );

	private:
		RenderDevice const & m_device;
		castor::String m_debugName;
		std::map< uint32_t, BufferArray > m_buffers;
		BufferArray m_nonSharedBuffers;
	};
}

#include "Castor3D/Buffer/GpuBufferPool.inl"

#endif
