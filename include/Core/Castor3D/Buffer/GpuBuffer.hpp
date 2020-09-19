/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GpuBuffer_H___
#define ___C3D_GpuBuffer_H___

#include "BufferModule.hpp"

#include "Castor3D/Buffer/GpuBufferBuddyAllocator.hpp"
#include "Castor3D/Miscellaneous/DebugName.hpp"
#include "Castor3D/Render/RenderDevice.hpp"

#include <CastorUtils/Design/ArrayView.hpp>

#include <ashespp/Buffer/Buffer.hpp>
#include <ashespp/Buffer/VertexBuffer.hpp>

namespace castor3d
{
	class GpuBuffer
	{
		friend class GpuBufferPool;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	device			The device on which the storage is allocated.
		 *\param[in]	numLevels		The allocator maximum tree size.
		 *\param[in]	minBlockSize	The minimum size for a block.
		 *\param[in]	usage			The buffer targets.
		 *\param[in]	memoryFlags		The buffer memory properties.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	device			Le device sur lequel le stockage est alloué.
		 *\param[in]	numLevels		La taille maximale de l'arbre de l'allocateur.
		 *\param[in]	minBlockSize	La taille minimale d'un bloc.
		 *\param[in]	usage			Les cibles du tampon.
		 *\param[in]	memoryFlags		Les propriétés mémoire du tampon.
		 */
		C3D_API GpuBuffer( RenderSystem const & renderSystem
			, VkBufferUsageFlags usage
			, VkMemoryPropertyFlags memoryFlags
			, castor::String debugName
			, ashes::QueueShare sharingMode
			, uint32_t numLevels
			, uint32_t minBlockSize );
		/**
		 *\~english
		 *\brief		Initialises the GPU buffer storage.
		 *\~french
		 *\brief		Initialise le stockage GPU du tampon.
		 */
		C3D_API uint32_t initialise();
		/**
		 *\~english
		 *\brief		Cleans up the GPU buffer.
		 *\~french
		 *\brief		Nettoie le tampon GPU.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\param[in]	size	The requested memory size.
		 *\return		\p true if there is enough remaining memory for given size.
		 *\~french
		 *\param[in]	size	La taille requise pour la mémoire.
		 *\return		\p true s'il y a assez de mémoire restante pour la taille donnée.
		 */
		C3D_API bool hasAvailable( VkDeviceSize size )const;
		/**
		 *\~english
		 *\brief		Allocates a memory chunk for a CPU buffer.
		 *\param[in]	size	The requested memory size.
		 *\return		The memory chunk offset.
		 *\~french
		 *\brief		Alloue une zone mémoire pour un CPU buffer.
		 *\param[in]	size	La taille requise pour la mémoire.
		 *\return		L'offset de la zone mémoire.
		 */
		C3D_API MemChunk allocate( VkDeviceSize size );
		/**
		 *\~english
		 *\brief		Deallocates memory.
		 *\param[in]	offset	The memory chunk offset.
		 *\~french
		 *\brief		Désalloue de la mémoire.
		 *\param[in]	offset	L'offset de la zone mémoire.
		 */
		C3D_API void deallocate( MemChunk const & mem );
		/**
		 *\~english
		 *\brief		Locks the buffer, id est maps it into memory so we can modify it.
		 *\remarks		Maps from m_buffer[offset] to m_buffer[offset + count - 1].
		 *\param[in]	chunk	The mapped memory range.
		 *\return		The mapped buffer address.
		 *\~french
		 *\brief		Locke le tampon, càd le mappe en mémoire ram afin d'y autoriser des modifications.
		 *\remarks		Mappe de m_buffer[offset] à m_buffer[offset + count - 1].
		 *\param[in]	chunk	L'intervalle de mémoire à mapper.
		 *\return		L'adresse du tampon mappé.
		 */
		C3D_API uint8_t * lock( MemChunk const & chunk )const;
		/**
		 *\~english
		 *\brief		Validates a memory range in VRAM.
		 *\param[in]	offset	The start offset in the buffer.
		 *\param[in]	size	The mapped memory size.
		 *\return		The mapped buffer address.
		 *\~french
		 *\brief		Locke le tampon, càd le mappe en mémoire ram afin d'y autoriser des modifications.
		 *\param[in]	chunk	L'intervalle de mémoire à valider.
		 *\return		L'adresse du tampon mappé.
		 */
		C3D_API void flush( MemChunk const & chunk )const;
		/**
		 *\~english
		 *\brief		Locks the buffer, id est maps it into memory so we can modify it.
		 *\remarks		Maps from m_buffer[offset] to m_buffer[offset + count - 1].
		 *\param[in]	offset	The start offset in the buffer.
		 *\param[in]	size	The mapped memory size.
		 *\return		The mapped buffer address.
		 *\~french
		 *\brief		Locke le tampon, càd le mappe en mémoire ram afin d'y autoriser des modifications.
		 *\remarks		Mappe de m_buffer[offset] à m_buffer[offset + count - 1].
		 *\param[in]	offset	L'offset de départ.
		 *\param[in]	size	La taille de la mémoire à mapper.
		 *\return		L'adresse du tampon mappé.
		 */
		C3D_API void invalidate( MemChunk const & chunk )const;
		/**
		 *\~english
		 *\brief		Unlocks the buffer, id est unmaps it from memory so no modification can be made after that.
		 *\remarks		All modifications made in the mapped buffer are put into GPU memory.
		 *\~french
		 *\brief		Un locke le tampon, càd l'unmappe de la mémoire ram afin de ne plus autoriser de modifications dessus.
		 *\remarks		Toutes les modifications qui avaient été effectuées sur le tampon mappé sont rapatriées dans la mémoire GPU.
		 */
		C3D_API void unlock()const;
		/**
		 *\~english
		 *\brief		Copies data from given buffer to this one.
		 *\remarks		The command buffer must be in recording state.
		 *\param[in]	commandBuffer	The command buffer on which the copy commands are recorded.
		 *\param[in]	src				The source buffer.
		 *\param[in]	srcOffset		The start offset in the source buffer.
		 *\param[in]	dstOffset		The start offset in this buffer.
		 *\param[in]	size			The number of elements to copy.
		 *\~french
		 *\brief		Copie les données du tampon donné dans celui-ci.
		 *\remarks		Le command buffer doit être en état d'enregistrement.
		 *\param[in]	commandBuffer	Le command buffer sur lequel les commandes de copie sont enregistrées.
		 *\param[in]	src				Le tampon source.
		 *\param[in]	srcOffset		L'offset de départ dans le tampon source.
		 *\param[in]	dstOffset		L'offset de départ dans ce tampon.
		 *\param[in]	size			Le nombre d'éléments à copier.
		 */
		C3D_API void copy( ashes::CommandBuffer const & commandBuffer
			, GpuBuffer const & src
			, MemChunk const & srcChunk
			, VkDeviceSize dstOffset )const;
		/**
		 *\~english
		 *\brief		Transfers data to the GPU buffer from RAM.
		 *\remarks		Transfers data from buffer[offset*sizeof( T )] to buffer[(offset+count-1)*sizeof( T )].
		 *\param[in]	stagingBuffer	The staging buffer used to transfer the data.
		 *\param[in]	commandBuffer	The command buffer on which the transfer commands are recorded.
		 *\param[in]	offset			The start offset.
		 *\param[in]	count			Elements count.
		 *\param[in]	buffer			The data.
		 *\~french
		 *\brief		Transfère des données au tampon GPU à partir de la RAM.
		 *\remarks		Transfère les données de tampon[offset*sizeof( T )] à tampon[(offset+count-1) * sizeof( T )].
		 *\param[in]	stagingBuffer	Le staging buffer utilisé pour effectuer le transfer.
		 *\param[in]	commandBuffer	Le command buffer sur lequel les commandes de transfert sont enregistrées.
		 *\param[in]	offset			L'offset de départ.
		 *\param[in]	count			Nombre d'éléments.
		 *\param[in]	buffer			Les données.
		 */
		C3D_API void upload( ashes::StagingBuffer & stagingBuffer
			, ashes::Queue const & queue
			, ashes::CommandPool const & commandPool
			, MemChunk const & chunk
			, uint8_t const * buffer )const;
		/**
		 *\~english
		 *\brief		Transfers data to the GPU buffer from RAM.
		 *\remarks		Transfers data from buffer[offset*sizeof( T )] to buffer[(offset+count-1)*sizeof( T )].
		 *\param[in]	stagingBuffer	The staging buffer used to transfer the data.
		 *\param[in]	commandBuffer	The command buffer on which the transfer commands are recorded.
		 *\param[in]	offset			The start offset.
		 *\param[in]	count			Elements count.
		 *\param[in]	buffer			The data.
		 *\~french
		 *\brief		Transfère des données au tampon GPU à partir de la RAM.
		 *\remarks		Transfère les données de tampon[offset*sizeof( T )] à tampon[(offset+count-1) * sizeof( T )].
		 *\param[in]	stagingBuffer	Le staging buffer utilisé pour effectuer le transfer.
		 *\param[in]	commandBuffer	Le command buffer sur lequel les commandes de transfert sont enregistrées.
		 *\param[in]	offset			L'offset de départ.
		 *\param[in]	count			Nombre d'éléments.
		 *\param[in]	buffer			Les données.
		 */
		C3D_API void upload( ashes::StagingBuffer & stagingBuffer
			, ashes::CommandBuffer const & commandBuffer
			, MemChunk const & chunk
			, uint8_t const * buffer )const;
		/**
		 *\~english
		 *\brief		Transfers data from the GPU buffer to RAM.
		 *\remarks		Transfers data from buffer[offset*sizeof( T )] to buffer[(offset+count-1)*sizeof( T )].
		 *\param[in]	stagingBuffer	The staging buffer used to transfer the data.
		 *\param[in]	commandBuffer	The command buffer on which the transfer commands are recorded.
		 *\param[in]	offset			The start offset.
		 *\param[in]	count			Elements count.
		 *\param[out]	buffer			The data.
		 *\~french
		 *\brief		Transfère des données du tampon GPU vers la RAM.
		 *\remarks		Transfère les données de tampon[offset*sizeof( T )] à tampon[(offset+count-1) * sizeof( T )].
		 *\param[in]	stagingBuffer	Le staging buffer utilisé pour effectuer le transfer.
		 *\param[in]	commandBuffer	Le command buffer sur lequel les commandes de transfert sont enregistrées.
		 *\param[in]	offset			L'offset de départ.
		 *\param[in]	count			Nombre d'éléments.
		 *\param[out]	buffer			Les données.
		 */
		C3D_API void download( ashes::StagingBuffer & stagingBuffer
			, ashes::Queue const & queue
			, ashes::CommandPool const & commandPool
			, MemChunk const & chunk
			, uint8_t * buffer )const;
		/**
		*\~english
		*\return
		*	The internal buffer.
		*\~french
		*\return
		*	Le tampon interne.
		*/
		inline bool hasBuffer()const
		{
			return m_buffer != nullptr;
		}
		/**
		*\~english
		*\return
		*	The internal buffer.
		*\~french
		*\return
		*	Le tampon interne.
		*/
		inline ashes::Buffer< uint8_t > const & getBuffer()const
		{
			return *m_buffer;
		}
		inline operator ashes::Buffer< uint8_t > const & ()const
		{
			return *m_buffer;
		}
		/**
		*\~english
		*\return
		*	The internal buffer.
		*\~french
		*\return
		*	Le tampon interne.
		*/
		inline ashes::Buffer< uint8_t > & getBuffer()
		{
			return *m_buffer;
		}
		inline operator ashes::Buffer< uint8_t > & ()
		{
			return *m_buffer;
		}

	private:
		RenderSystem const & m_renderSystem;
		VkBufferUsageFlags m_usage;
		VkMemoryPropertyFlags m_memoryFlags;
		ashes::QueueShare m_sharingMode;
		std::set< MemChunk > m_allocated;
		GpuBufferBuddyAllocator m_allocator;
		ashes::BufferPtr< uint8_t > m_buffer;
		castor::String m_debugName;
		VkDeviceSize m_align{ 0u };
	};

	template< typename T >
	inline ashes::BufferPtr< T > makeBuffer( RenderDevice const & device
		, VkDeviceSize count
		, VkBufferUsageFlags usage
		, VkMemoryPropertyFlags flags
		, std::string const & name
		, ashes::QueueShare sharingMode = {} )
	{
		ashes::BufferPtr< T > result = ashes::makeBuffer< T >( *device.device
			, name + "Buf"
			, count
			, usage
			, std::move( sharingMode ) );
		ashes::BufferBase & buffer = result->getBuffer();
		result->bindMemory( setupMemory( device, buffer, flags, name + "Buf" ) );
		return result;
	}

	inline ashes::BufferBasePtr makeBufferBase( RenderDevice const & device
		, VkDeviceSize size
		, VkBufferUsageFlags usage
		, VkMemoryPropertyFlags flags
		, std::string const & name
		, ashes::QueueShare sharingMode = {} )
	{
		auto result = ashes::makeBufferBase( *device.device
			, name + "Buf"
			, size
			, usage
			, std::move( sharingMode ) );
		result->bindMemory( setupMemory( device, *result, flags, name + "Buf" ) );
		return result;
	}

	template< typename T >
	inline ashes::VertexBufferPtr< T > makeVertexBuffer( RenderDevice const & device
		, VkDeviceSize count
		, VkBufferUsageFlags usage
		, VkMemoryPropertyFlags flags
		, std::string const & name
		, ashes::QueueShare sharingMode = {} )
	{
		ashes::VertexBufferPtr< T > result = ashes::makeVertexBuffer< T >( *device.device
			, name + "Vbo"
			, count
			, usage
			, std::move( sharingMode ) );
		ashes::BufferBase & buffer = result->getBuffer();
		result->bindMemory( setupMemory( device, buffer, flags, name + "Vbo" ) );
		return result;
	}

	inline ashes::VertexBufferBasePtr makeVertexBufferBase( RenderDevice const & device
		, VkDeviceSize size
		, VkBufferUsageFlags usage
		, VkMemoryPropertyFlags flags
		, std::string const & name
		, ashes::QueueShare sharingMode = {} )
	{
		auto result = ashes::makeVertexBufferBase( *device.device
			, name + "Vbo"
			, size
			, usage
			, std::move( sharingMode ) );
		result->bindMemory( setupMemory( device, *result, flags, name + "Vbo" ) );
		return result;
	}
}

#endif
