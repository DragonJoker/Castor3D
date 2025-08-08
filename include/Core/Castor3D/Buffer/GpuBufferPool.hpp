/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GpuBufferPool_HPP___
#define ___C3D_GpuBufferPool_HPP___

#include "Castor3D/Buffer/GpuBufferOffset.hpp"

#include <CastorUtils/Design/OwnedBy.hpp>

namespace c3d
{
	class GpuBufferPool
		: public OwnedBy< RenderSystem >
	{
	public:
		using BufferArray = Vector< RawUniquePtr< GpuBuddyBuffer > >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	device			The GPU device.
		 *\param[in]	debugName		The debug name.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	debugName		Le nom debug.
		 */
		C3D_API GpuBufferPool( RenderDevice const & device
			, crg::ResourcesCache & resources
			, String debugName );
		/**
		 *\~english
		 *\brief			Uploads all ready memory ranges to VRAM.
		 *\param[in,out]	uploader	Receives the upload requests.
		 *\~french
		 *\brief			Met à jour tous les intervalles mémoire prêts en VRAM.
		 *\param[in,out]	uploader	Reçoit les requêtes d'upload.
		 */
		C3D_API void upload( UploadData & uploader )const;
		/**
		 *\~english
		 *\return		The allocation statistics.
		 *\~french
		 *\return		Les statistiques d'allocation.
		 */
		C3D_API AllocationStats getAllocationStats()const noexcept;
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
		GpuBufferOffsetT< DataT > getBuffer( BufferUsageFlags target
			, DeviceSize count
			, MemoryPropertyFlags flags );
		/**
		 *\~english
		 *\brief		Releases a GPU buffer.
		 *\param[in]	bufferOffset	The buffer offset to release.
		 *\~french
		 *\brief		Libère un tampon GPU.
		 *\param[in]	bufferOffset	Le tampon à libérer.
		 */
		template< typename DataT >
		void putBuffer( GpuBufferOffsetT< DataT > const & bufferOffset )noexcept;

	private:
		C3D_API BufferArray::iterator doFindBuffer( DeviceSize size
			, BufferArray & array )const;
		C3D_API uint32_t doMakeKey( BufferUsageFlags target
			, MemoryPropertyFlags flags )const noexcept;
		C3D_API GpuBufferBase & doGetBuffer( DeviceSize size
			, BufferUsageFlags target
			, MemoryPropertyFlags memory
			, MemChunk & chunk );
		C3D_API void doPutBuffer( GpuBufferBase const & buffer
			, BufferUsageFlags target
			, MemoryPropertyFlags memory
			, MemChunk const & chunk )noexcept;

	private:
		RenderDevice const & m_device;
		crg::ResourcesCache & m_resources;
		String m_debugName;
		Map< uint32_t, BufferArray > m_buffers;
		BufferArray m_nonSharedBuffers;
		uint32_t m_minBlockSize{};
	};
}

#include "Castor3D/Buffer/GpuBufferPool.inl"

#endif
