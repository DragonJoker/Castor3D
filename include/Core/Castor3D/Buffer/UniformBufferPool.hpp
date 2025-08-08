/*
See LICENSE file in root folder
*/
#ifndef ___C3D_UniformBufferPool_HPP___
#define ___C3D_UniformBufferPool_HPP___

#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Buffer/PoolUniformBuffer.hpp"
#include "Castor3D/Buffer/UniformBufferOffset.hpp"

#include <CastorUtils/Design/OwnedBy.hpp>

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <mutex>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

namespace c3d
{
	class UniformBufferPool
		: public OwnedBy< RenderSystem >
	{
	public:
		struct Buffer
		{
			uint32_t index;
			PoolUniformBufferUPtr buffer;
		};
		using BufferArray = Vector< Buffer >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	device			The GPU device.
		 *\param[in]	debugName		The buffer debug name.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	debugName		Le nom debug du tampon.
		 */
		C3D_API UniformBufferPool( RenderDevice const & device
			, crg::ResourcesCache & resources
			, String debugName );
		/**
		 *\~english
		 *\brief		Cleans up all GPU buffers.
		 *\~french
		 *\brief		Nettoie tous les tampons GPU.
		 */
		C3D_API ~UniformBufferPool()noexcept;
		/**
		 *\~english
		 *\return		The allocation statistics.
		 *\~french
		 *\return		Les statistiques d'allocation.
		 */
		C3D_API AllocationStats getAllocationStats()const noexcept;
		/**
		 *\~english
		 *\return		The allocation statistics.
		 *\~french
		 *\return		Les statistiques d'allocation.
		 */
		C3D_API Vector< Pair< MemChunk, String > > listAllocations()const;
		/**
		 *\~english
		 *\brief		Uploads all GPU buffers to VRAM.
		 *\param[in]	uploader	Receives the upload requests.
		 *\~french
		 *\brief		Met à jour tous les tampons GPU en VRAM.
		 *\param[in]	uploader	Reçoit les requêtes d'upload.
		 */
		C3D_API void upload( UploadData const & uploader )const;
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
		UniformBufferOffsetT< DataT > getBuffer( MemoryPropertyFlags flags );
		/**
		 *\~english
		 *\brief		Releases a GPU buffer.
		 *\param[in]	bufferOffset	The buffer offset to release.
		 *\~french
		 *\brief		Libère un tampon GPU.
		 *\param[in]	bufferOffset	Le tampon à libérer.
		 */
		template< typename DataT >
		void putBuffer( UniformBufferOffsetT< DataT > & bufferOffset )noexcept;
		/**
		 *\~english
		 *\return		The pool buffers count.
		 *\~french
		 *\return		Le nombre de tampons du pool.
		 */
		C3D_API uint32_t getBufferCount()const;

	private:
		C3D_API BufferArray::iterator doFindBuffer( BufferArray & array
			, DeviceSize alignedSize )const;
		C3D_API BufferArray::iterator doCreatePoolBuffer( MemoryPropertyFlags flags
			, UniformBufferPool::BufferArray & buffers );

	private:
		RenderDevice const & m_device;
		crg::ResourcesCache & m_resources;
		uint32_t m_maxUboSize{ 0u };
		uint32_t m_currentUboIndex{ 0u };
		Map< uint32_t, BufferArray > m_buffers;
		String m_debugName;
		Mutex m_mutex;
	};
}

#include "Castor3D/Buffer/UniformBufferPool.inl"

#endif
