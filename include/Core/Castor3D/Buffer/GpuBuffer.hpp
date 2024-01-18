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

#include <unordered_map>

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
		 *\param[in]	allocatedSize	The buffer size.
		 *\param[in]	smallData		Tells if the memory chunks allocated through this buffer are less than 65536 bytes.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	renderSystem	Le device sur lequel le stockage est alloué.
		 *\param[in]	usage			Les cibles du tampon.
		 *\param[in]	memoryFlags		Les propriétés mémoire du tampon.
		 *\param[in]	debugName		Le nom debug.
		 *\param[in]	sharingMode		Le mode de partage.
		 *\param[in]	allocatedSize	La taille du buffer.
		 *\param[in]	smallData		Dit si les chunks mémoire alloués via ce buffer font moins de 65536 bytes.
		 */
		C3D_API GpuBufferBase( RenderSystem const & renderSystem
			, VkBufferUsageFlags usage
			, VkMemoryPropertyFlags memoryFlags
			, castor::String debugName
			, ashes::QueueShare sharingMode
			, VkDeviceSize allocatedSize
			, bool smallData = false );
		/**
		 *\~english
		 *\brief		Uploads all ready memory ranges to VRAM.
		 *\param[in]	cb	The command buffer on which transfer commands are recorded.
		 *\~french
		 *\brief		Met à jour tous les intervalles mémoire prêts en VRAM.
		 *\param[in]	cb	Le command buffer sur lequel les commandes de transfert sont enregistrées.
		 */
		C3D_API void upload( UploadData & uploader );
		/**
		 *\~english
		 *\brief		Uploads a memory range.
		 *\param[in]	queue				The queue to which the upload commands will be submitted to.
		 *\param[in]	commandPool			The command pool from which the command buffer will be created.
		 *\param[in]	offset, size		The memory range.
		 *\param[in]	dstAccessFlags		The access flags wanted after upload.
		 *\param[in]	dstPipelineFlags	The pipeline stage flags wanted after upload.
		 *\~french
		 *\brief		Met à jour un intervalle mémoire en VRAM.
		 *\param[in]	queue				La queue à laquelle les commandes d'upload seront soumises.
		 *\param[in]	commandPool			Le command pool depuis lequel le command buffer sera créé.
		 *\param[in]	offset, size		L'intervalle mémoire.
		 *\param[in]	dstAccessFlags		Les flags d'accès voulus après l'upload.
		 *\param[in]	dstPipelineFlags	Les flags d'étape de pipeline voulus après l'upload.
		 */
		C3D_API void upload( UploadData & uploader
			, VkDeviceSize offset
			, VkDeviceSize size
			, VkAccessFlags dstAccessFlags
			, VkPipelineStageFlags dstPipelineFlags );
		/**
		 *\~english
		 *\brief		Marks a memory range to be ready for upload.
		 *\param[in]	offset, size		The memory range.
		 *\param[in]	dstAccessFlags		The access flags wanted after upload.
		 *\param[in]	dstPipelineFlags	The pipeline stage flags wanted after upload.
		 *\~french
		 *\brief		Marque un intervalle mémoire comme prêt à l'upload.
		 *\param[in]	offset, size		L'intervalle mémoire.
		 *\param[in]	dstAccessFlags		Les flags d'accès voulus après l'upload.
		 *\param[in]	dstPipelineFlags	Les flags d'étape de pipeline voulus après l'upload.
		 */
		C3D_API void markDirty( VkDeviceSize offset
			, VkDeviceSize size
			, VkAccessFlags dstAccessFlags
			, VkPipelineStageFlags dstPipelineFlags );
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
		/**
		*\~english
		*\return
		*	The N-th instance of the data.
		 *\param[in] offset
		 *	The memory chunk offset.
		*\~french
		*\return
		*	La n-ème instance des données.
		 *\param[in] offset
		*	L'offset de la zone mémoire.
		*/
		template< typename DataT >
		DataT const & getData( VkDeviceSize offset )const
		{
			using DataCPtr = DataT const *;
			return *DataCPtr( m_data.data() + offset );
		}
		/**
		*\~english
		*\return
		*	The N-th instance of the data.
		 *\param[in] offset
		 *	The memory chunk offset.
		*\~french
		*\return
		*	La n-ème instance des données.
		 *\param[in] offset
		*	L'offset de la zone mémoire.
		*/
		template< typename DataT >
		DataT & getData( VkDeviceSize offset )
		{
			using DataPtr = DataT *;
			return *DataPtr( m_data.data() + offset );
		}
		/**
		*\~english
		*\return
		*	The data.
		*\~french
		*\return
		*	Les données.
		*/
		castor::ByteArrayView const & getDatas()const
		{
			return m_data;
		}
		/**
		*\~english
		*\return
		*	The data.
		*\~french
		*\return
		*	Les données.
		*/
		castor::ByteArrayView & getDatas()
		{
			return m_data;
		}

	private:
		RenderSystem const & m_renderSystem;
		VkBufferUsageFlags m_usage;
		VkMemoryPropertyFlags m_memoryFlags;
		ashes::QueueShare m_sharingMode;
		VkDeviceSize m_allocatedSize;
		ashes::BufferPtr< uint8_t > m_buffer;
		castor::ByteArray m_ownData;
		castor::ByteArrayView m_data;
		struct MemoryRange
		{
			VkDeviceSize offset;
			VkDeviceSize size;
			VkAccessFlags dstAccessFlags;
			VkPipelineStageFlags dstPipelineFlags;
		};
		using MemoryRangeArray = std::vector< MemoryRange >;
		std::unordered_map< size_t, MemoryRangeArray > m_ranges;
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
		 *\param[in]	smallData		Tells if the memory chunks allocated through this buffer are less than 65536 bytes.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	renderSystem	Le device sur lequel le stockage est alloué.
		 *\param[in]	usage			Les cibles du tampon.
		 *\param[in]	memoryFlags		Les propriétés mémoire du tampon.
		 *\param[in]	debugName		Le nom debug.
		 *\param[in]	sharingMode		Le mode de partage.
		 *\param[in]	allocator		L'allocateur.
		 *\param[in]	smallData		Dit si les chunks mémoire alloués via ce buffer font moins de 65536 bytes.
		 */
		GpuBufferT( RenderSystem const & renderSystem
			, VkBufferUsageFlags usage
			, VkMemoryPropertyFlags memoryFlags
			, castor::String debugName
			, ashes::QueueShare sharingMode
			, AllocatorT allocator
			, bool smallData = false );
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
		AllocatorT m_allocator;
	};

	template< typename AllocatorT >
	class GpuBaseBufferT
	{
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
		 *\param[in]	smallData		Tells if the memory chunks allocated through this buffer are less than 65536 bytes.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	renderSystem	Le device sur lequel le stockage est alloué.
		 *\param[in]	usage			Les cibles du tampon.
		 *\param[in]	memoryFlags		Les propriétés mémoire du tampon.
		 *\param[in]	debugName		Le nom debug.
		 *\param[in]	sharingMode		Le mode de partage.
		 *\param[in]	allocator		L'allocateur.
		 *\param[in]	smallData		Dit si les chunks mémoire alloués via ce buffer font moins de 65536 bytes.
		 */
		GpuBaseBufferT( RenderDevice const & device
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
		/**
		*\~english
		*\return
		*	The internal buffer.
		*\~french
		*\return
		*	Le tampon interne.
		*/
		ashes::BufferBase const & getBuffer()const
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
		ashes::BufferBase & getBuffer()
		{
			return *m_buffer;
		}

	private:
		RenderDevice const & m_device;
		VkBufferUsageFlags m_usage;
		VkMemoryPropertyFlags m_memoryFlags;
		ashes::QueueShare m_sharingMode;
		VkDeviceSize m_allocatedSize;
		ashes::BufferBasePtr m_buffer;
		AllocatorT m_allocator;
	};

	C3D_API std::pair< VkDeviceSize, VkDeviceSize > adaptRange( VkDeviceSize offset
		, VkDeviceSize size
		, VkDeviceSize align );

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
