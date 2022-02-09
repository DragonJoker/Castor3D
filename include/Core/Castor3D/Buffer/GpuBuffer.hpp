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
	class GpuBufferBase
	{
		friend class GpuBufferPool;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	renderSystem	The device on which the storage is allocated.
		 *\param[in]	usage			The buffer targets.
		 *\param[in]	memoryFlags		The buffer memory properties.
		 *\param[in]	debugName		The debug name.
		 *\param[in]	sharingMode		The sharing mode.
		 *\param[in]	numLevels		The allocator maximum tree size.
		 *\param[in]	minBlockSize	The minimum size for a block.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	renderSystem	Le device sur lequel le stockage est alloué.
		 *\param[in]	usage			Les cibles du tampon.
		 *\param[in]	memoryFlags		Les propriétés mémoire du tampon.
		 *\param[in]	debugName		Le nom debug.
		 *\param[in]	sharingMode		Le mode de partage.
		 *\param[in]	numLevels		La taille maximale de l'arbre de l'allocateur.
		 *\param[in]	minBlockSize	La taille minimale d'un bloc.
		 */
		C3D_API GpuBufferBase( RenderSystem const & renderSystem
			, VkBufferUsageFlags usage
			, VkMemoryPropertyFlags memoryFlags
			, castor::String debugName
			, ashes::QueueShare sharingMode
			, VkDeviceSize allocatedSize );
		/**
		 *\~english
		 *\brief		Initialises the GPU buffer storage.
		 *\param[in]	device	The GPU device.
		 *\~french
		 *\brief		Initialise le stockage GPU du tampon.
		 *\param[in]	device	Le device GPU.
		 */
		C3D_API uint32_t initialise( RenderDevice const & device );
		/**
		 *\~english
		 *\brief		Cleans up the GPU buffer.
		 *\param[in]	device	The GPU device.
		 *\~french
		 *\brief		Nettoie le tampon GPU.
		 *\param[in]	device	Le device GPU.
		 */
		C3D_API void cleanup( RenderDevice const & device );
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
		 *\param[in]	chunk	The memory chunk.
		 *\return		The mapped buffer address.
		 *\~french
		 *\brief		Locke le tampon, càd le mappe en mémoire ram afin d'y autoriser des modifications.
		 *\param[in]	chunk	La zone mémoire.
		 *\return		L'adresse du tampon mappé.
		 */
		C3D_API void flush( MemChunk const & chunk )const;
		/**
		 *\~english
		 *\brief		Marks the given memory chunk as dirty.
		 *\param[in]	chunk	The memory chunk.
		 *\~french
		 *\brief		Marque la zone mémoire donnée comme "sale".
		 *\param[in]	chunk	La zone mémoire.
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
		 *\param[in]	srcChunk		The memory chunk.
		 *\param[in]	dstOffset		The start offset in this buffer.
		 *\~french
		 *\brief		Copie les données du tampon donné dans celui-ci.
		 *\remarks		Le command buffer doit être en état d'enregistrement.
		 *\param[in]	commandBuffer	Le command buffer sur lequel les commandes de copie sont enregistrées.
		 *\param[in]	src				Le tampon source.
		 *\param[in]	srcChunk		La zone mémoire.
		 *\param[in]	dstOffset		L'offset de départ dans ce tampon.
		 */
		C3D_API void copy( ashes::CommandBuffer const & commandBuffer
			, GpuBufferBase const & src
			, MemChunk const & srcChunk
			, VkDeviceSize dstOffset )const;
		/**
		 *\~english
		 *\brief		Transfers data to the GPU buffer from RAM.
		 *\remarks		Transfers data from buffer[offset*sizeof( T )] to buffer[(offset+count-1)*sizeof( T )].
		 *\param[in]	stagingBuffer	The staging buffer used to transfer the data.
		 *\param[in]	queue			The queue used to execute the transfer.
		 *\param[in]	commandPool		The command pool on which the command buffer will be created.
		 *\param[in]	chunk			The memory chunk.
		 *\param[in]	buffer			The data.
		 *\~french
		 *\brief		Transfère des données au tampon GPU à partir de la RAM.
		 *\remarks		Transfère les données de tampon[offset*sizeof( T )] à tampon[(offset+count-1) * sizeof( T )].
		 *\param[in]	stagingBuffer	Le staging buffer utilisé pour effectuer le transfer.
		 *\param[in]	queue			La file sur laquelle le transfert sera effectué.
		 *\param[in]	commandPool		Le command pool depuis lequel le command buffer sera créé.
		 *\param[in]	chunk			La zone mémoire.
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
		 *\param[in]	chunk			The memory chunk.
		 *\param[in]	buffer			The data.
		 *\~french
		 *\brief		Transfère des données au tampon GPU à partir de la RAM.
		 *\remarks		Transfère les données de tampon[offset*sizeof( T )] à tampon[(offset+count-1) * sizeof( T )].
		 *\param[in]	stagingBuffer	Le staging buffer utilisé pour effectuer le transfer.
		 *\param[in]	commandBuffer	Le command buffer sur lequel les commandes de transfert sont enregistrées.
		 *\param[in]	chunk			La zone mémoire.
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
		 *\param[in]	queue			The queue used to execute the transfer.
		 *\param[in]	commandPool		The command pool on which the command buffer will be created.
		 *\param[in]	chunk			The memory chunk.
		 *\param[out]	buffer			The data.
		 *\~french
		 *\brief		Transfère des données du tampon GPU vers la RAM.
		 *\remarks		Transfère les données de tampon[offset*sizeof( T )] à tampon[(offset+count-1) * sizeof( T )].
		 *\param[in]	stagingBuffer	Le staging buffer utilisé pour effectuer le transfer.
		 *\param[in]	queue			La file sur laquelle le transfert sera effectué.
		 *\param[in]	commandPool		Le command pool depuis lequel le command buffer sera créé.
		 *\param[in]	chunk			La zone mémoire.
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
		bool hasBuffer()const
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
		ashes::Buffer< uint8_t > const & getBuffer()const
		{
			return *m_buffer;
		}
		operator ashes::Buffer< uint8_t > const & ()const
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
		ashes::Buffer< uint8_t > & getBuffer()
		{
			return *m_buffer;
		}
		operator ashes::Buffer< uint8_t > & ()
		{
			return *m_buffer;
		}

	private:
		RenderSystem const & m_renderSystem;
		VkBufferUsageFlags m_usage;
		VkMemoryPropertyFlags m_memoryFlags;
		ashes::QueueShare m_sharingMode;
		VkDeviceSize m_allocatedSize;
		ashes::BufferPtr< uint8_t > m_buffer;
		castor::String m_debugName;
	};

	template< typename AllocatorT >
	class GpuBufferT
		: public GpuBufferBase
	{
		friend class GpuBufferPool;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	renderSystem	The device on which the storage is allocated.
		 *\param[in]	usage			The buffer targets.
		 *\param[in]	memoryFlags		The buffer memory properties.
		 *\param[in]	debugName		The debug name.
		 *\param[in]	sharingMode		The sharing mode.
		 *\param[in]	allocator		The allocator.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	renderSystem	Le device sur lequel le stockage est alloué.
		 *\param[in]	usage			Les cibles du tampon.
		 *\param[in]	memoryFlags		Les propriétés mémoire du tampon.
		 *\param[in]	debugName		Le nom debug.
		 *\param[in]	sharingMode		Le mode de partage.
		 *\param[in]	allocator		L'allocateur.
		 */
		GpuBufferT( RenderSystem const & renderSystem
			, VkBufferUsageFlags usage
			, VkMemoryPropertyFlags memoryFlags
			, castor::String debugName
			, ashes::QueueShare sharingMode
			, AllocatorT allocator );
		/**
		 *\~english
		 *\param[in]	size	The requested memory size.
		 *\return		\p true if there is enough remaining memory for given size.
		 *\~french
		 *\param[in]	size	La taille requise pour la mémoire.
		 *\return		\p true s'il y a assez de mémoire restante pour la taille donnée.
		 */
		bool hasAvailable( VkDeviceSize size )const;
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
		MemChunk allocate( VkDeviceSize size );
		/**
		 *\~english
		 *\brief		Deallocates memory.
		 *\param[in]	mem	The memory chunk.
		 *\~french
		 *\brief		Désalloue de la mémoire.
		 *\param[in]	mem	La zone mémoire.
		 */
		void deallocate( MemChunk const & mem );
		/**
		 *\~english
		 *\return		The element aligned size.
		 *\~french
		 *\return		La taille  alignée d'un élément.
		 */
		size_t getMinAlignment()const;

	private:
		std::set< MemChunk > m_allocated;
		AllocatorT m_allocator;
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

#include "GpuBuffer.inl"

#endif
