/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GpuBuffer_H___
#define ___C3D_GpuBuffer_H___

#include "Castor3D/Buffer/BufferModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"

#include <ashespp/Miscellaneous/QueueShare.hpp>

namespace c3d
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
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	renderSystem	Le device sur lequel le stockage est alloué.
		 *\param[in]	usage			Les cibles du tampon.
		 *\param[in]	memoryFlags		Les propriétés mémoire du tampon.
		 *\param[in]	debugName		Le nom debug.
		 *\param[in]	sharingMode		Le mode de partage.
		 *\param[in]	allocatedSize	La taille du buffer.
		 */
		C3D_API GpuBufferBase( RenderSystem const & renderSystem
			, crg::ResourcesCache & resources
			, BufferUsageFlags usage
			, MemoryPropertyFlags memoryFlags
			, String const & debugName
			, ashes::QueueShare sharingMode
			, DeviceSize allocatedSize );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~GpuBufferBase()noexcept;
		/**
		 *\~english
		 *\brief			Uploads all ready memory ranges to VRAM.
		 *\param[in,out]	uploader	Receives the upload requests.
		 *\~french
		 *\brief			Met à jour tous les intervalles mémoire prêts en VRAM.
		 *\param[in,out]	uploader	Reçoit les requêtes d'upload.
		 */
		C3D_API void upload( UploadData & uploader );
		/**
		 *\~english
		 *\brief			Uploads a memory range.
		 *\param[in,out]	uploader		Receives the upload requests.
		 *\param[in]		offset, size	The memory range.
		 *\param[in]		dstAccessState	The pipeline stage flags wanted after upload.
		 *\~french
		 *\brief			Met à jour un intervalle mémoire en VRAM.
		 *\param[in,out]	uploader		Reçoit les requêtes d'upload.
		 *\param[in]		offset, size	L'intervalle mémoire.
		 *\param[in]		dstAccessState	L'état d'accès voulu après l'upload.
		 */
		C3D_API void upload( UploadData & uploader
			, DeviceSize offset
			, DeviceSize size
			, AccessState dstAccessState );
		/**
		 *\~english
		 *\brief			Uploads a memory range.
		 *\param[in,out]	uploader			Receives the upload requests.
		 *\param[in]		offset, size		The memory range.
		 *\param[in]		dstAccessFlags		The access flags wanted after upload.
		 *\param[in]		dstPipelineFlags	The pipeline stage flags wanted after upload.
		 *\~french
		 *\brief			Met à jour un intervalle mémoire en VRAM.
		 *\param[in,out]	uploader			Reçoit les requêtes d'upload.
		 *\param[in]		offset, size		L'intervalle mémoire.
		 *\param[in]		dstAccessFlags		Les flags d'accès voulus après l'upload.
		 *\param[in]		dstPipelineFlags	Les flags d'étape de pipeline voulus après l'upload.
		 */
		void upload( UploadData & uploader
			, DeviceSize offset
			, DeviceSize size
			, AccessFlags dstAccessFlags
			, PipelineStageFlags dstPipelineFlags )
		{
			upload( uploader, offset, size
				, AccessState{ dstAccessFlags, dstPipelineFlags } );
		}
		/**
		 *\~english
		 *\brief		Marks a memory range to be ready for upload.
		 *\param[in]	offset, size	The memory range.
		 *\param[in]	dstAccessState	The access and state wanted after upload.
		 *\~french
		 *\brief		Marque un intervalle mémoire comme prêt à l'upload.
		 *\param[in]	offset, size	L'intervalle mémoire.
		 *\param[in]	dstAccessState	L'état d'accès voulu après l'upload.
		 */
		C3D_API void markDirty( DeviceSize offset
			, DeviceSize size
			, AccessState dstAccessState );
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
		void markDirty( DeviceSize offset
			, DeviceSize size
			, AccessFlags dstAccessFlags
			, PipelineStageFlags dstPipelineFlags )
		{
			markDirty( offset, size
				, AccessState{ dstAccessFlags, dstPipelineFlags } );
		}
		/**
		*\~english
		*\return
		*	The internal buffer.
		*\~french
		*\return
		*	Le tampon interne.
		*/
		bool hasBuffer()const noexcept
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
		Buffer const & getBuffer()const noexcept
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
		Buffer & getBuffer()noexcept
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
		DataT const & getData( DeviceSize offset )const noexcept
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
		DataT & getData( DeviceSize offset )noexcept
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
		ByteArrayView const & getDatas()const noexcept
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
		ByteArrayView & getDatas()noexcept
		{
			return m_data;
		}

	protected:
		C3D_API crg::BufferViewId getSubView( DeviceSize offset, DeviceSize size )const;

	private:
		RenderSystem const & m_renderSystem;
		crg::ResourcesCache & m_resources;
		BufferUsageFlags m_usage;
		MemoryPropertyFlags m_memoryFlags;
		ashes::QueueShare m_sharingMode;
		DeviceSize m_allocatedSize;
		BufferUPtr m_buffer;
		ByteArray m_ownData;
		ByteArrayView m_data;
		struct MemoryRange
		{
			MemoryRange() = default;
			MemoryRange( DeviceSize offset
				, DeviceSize size
				, AccessState dstAccessState )
				: offset{ offset }
				, size{ size }
				, dstAccessState{ std::move( dstAccessState ) }
			{
			}

			DeviceSize offset{};
			DeviceSize size{};
			AccessState dstAccessState;
		};
		using MemoryRangeArray = Vector< MemoryRange >;
		HashMap< size_t, MemoryRangeArray > m_ranges;
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
			, crg::ResourcesCache & resources
			, BufferUsageFlags usage
			, MemoryPropertyFlags memoryFlags
			, String const & debugName
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
		bool hasAvailable( DeviceSize size )const noexcept;
		/**
		 *\~english
		 *\return		The remaining memory.
		 *\~french
		 *\return		La mémoire restante.
		 */
		DeviceSize getAvailable()const noexcept;
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
		MemChunk allocate( DeviceSize size );
		/**
		 *\~english
		 *\brief		Deallocates memory.
		 *\param[in]	mem	The memory chunk.
		 *\~french
		 *\brief		Désalloue de la mémoire.
		 *\param[in]	mem	La zone mémoire.
		 */
		void deallocate( MemChunk const & mem )noexcept;
		/**
		 *\~english
		 *\return		The element aligned size.
		 *\~french
		 *\return		La taille  alignée d'un élément.
		 */
		size_t getMinAlignment()const noexcept;

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
		 *\param[in]	device			The device on which the storage is allocated.
		 *\param[in]	usage			The buffer targets.
		 *\param[in]	memoryFlags		The buffer memory properties.
		 *\param[in]	debugName		The debug name.
		 *\param[in]	sharingMode		The sharing mode.
		 *\param[in]	allocator		The allocator.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	device			Le device sur lequel le stockage est alloué.
		 *\param[in]	usage			Les cibles du tampon.
		 *\param[in]	memoryFlags		Les propriétés mémoire du tampon.
		 *\param[in]	debugName		Le nom debug.
		 *\param[in]	sharingMode		Le mode de partage.
		 *\param[in]	allocator		L'allocateur.
		 */
		GpuBaseBufferT( RenderDevice const & device
			, crg::ResourcesCache & resources
			, BufferUsageFlags usage
			, MemoryPropertyFlags memoryFlags
			, String const & debugName
			, ashes::QueueShare sharingMode
			, AllocatorT allocator );
		~GpuBaseBufferT()noexcept;
		/**
		 *\~english
		 *\return		The remaining memory.
		 *\~french
		 *\return		La mémoire restante.
		 */
		DeviceSize getAvailable()const noexcept;
		/**
		 *\~english
		 *\param[in]	size	The requested memory size.
		 *\return		\p true if there is enough remaining memory for given size.
		 *\~french
		 *\param[in]	size	La taille requise pour la mémoire.
		 *\return		\p true s'il y a assez de mémoire restante pour la taille donnée.
		 */
		bool hasAvailable( DeviceSize size )const noexcept;
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
		MemChunk allocate( DeviceSize size )noexcept;
		/**
		 *\~english
		 *\brief		Deallocates memory.
		 *\param[in]	mem	The memory chunk.
		 *\~french
		 *\brief		Désalloue de la mémoire.
		 *\param[in]	mem	La zone mémoire.
		 */
		void deallocate( MemChunk const & mem )noexcept;
		/**
		 *\~english
		 *\return		The element aligned size.
		 *\~french
		 *\return		La taille  alignée d'un élément.
		 */
		size_t getMinAlignment()const noexcept;
		/**
		*\~english
		*\return
		*	The internal buffer.
		*\~french
		*\return
		*	Le tampon interne.
		*/
		Buffer const & getBuffer()const noexcept
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
		Buffer & getBuffer()noexcept
		{
			return *m_buffer;
		}

	private:
		RenderDevice const & m_device;
		BufferUsageFlags m_usage;
		MemoryPropertyFlags m_memoryFlags;
		ashes::QueueShare m_sharingMode;
		DeviceSize m_allocatedSize;
		BufferUPtr m_buffer;
		AllocatorT m_allocator;
	};

	C3D_API Pair< DeviceSize, DeviceSize > adaptRange( DeviceSize offset
		, DeviceSize size
		, DeviceSize align );
}

#include "GpuBuffer.inl"

#endif
