/*
See LICENSE file in root folder
*/
#ifndef ___C3D_UniformBuffer_H___
#define ___C3D_UniformBuffer_H___

#include "BufferModule.hpp"

#include "Castor3D/Miscellaneous/DebugName.hpp"
#include "Castor3D/Render/RenderDevice.hpp"

#include <CastorUtils/Design/ArrayView.hpp>

#include <ashespp/Buffer/UniformBuffer.hpp>
#include <ashespp/Sync/Fence.hpp>

namespace castor3d
{
	class UniformBufferBase
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	renderSystem	The RenderSystem.
		 *\param[in]	count			The elements count.
		 *\param[in]	usage			The buffer usage flags.
		 *\param[in]	flags			The buffer memory flags.
		 *\param[in]	debugName		The buffer debug name.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	renderSystem	Le RenderSystem.
		 *\param[in]	count			Le nombre d'éléments.
		 *\param[in]	usage			Les indicateurs d'utilisation du tampon.
		 *\param[in]	flags			Les indicateurs de mémoire du tampon.
		 *\param[in]	debugName		Le nom debug du tampon.
		 */
		C3D_API UniformBufferBase( RenderSystem const & renderSystem
			, VkDeviceSize elemCount
			, VkDeviceSize elemSize
			, VkBufferUsageFlags usage
			, VkMemoryPropertyFlags flags
			, castor::String debugName
			, ashes::QueueShare sharingMode = {} );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~UniformBufferBase() = default;
		/**
		 *\~english
		 *\brief		Initialises the GPU buffer.
		 *\~french
		 *\brief		Initialise le tampon GPU.
		 */
		C3D_API uint32_t initialise();
		/**
		 *\~english
		 *\brief		Initialises the GPU buffer.
		 *\~french
		 *\brief		Initialise le tampon GPU.
		 */
		C3D_API uint32_t initialise( ashes::QueueShare sharingMode );
		/**
		 *\~english
		 *\brief		Cleans up the GPU buffer.
		 *\~french
		 *\brief		Nettoie le tampon GPU.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\return		\p true if there is enough remaining memory for a new element.
		 *\~french
		 *\return		\p true s'il y a assez de mémoire restante pour un nouvel élément.
		 */
		C3D_API bool hasAvailable()const;
		/**
		 *\~english
		 *\brief		Allocates a memory chunk for a CPU buffer.
		 *\return		The memory chunk offset.
		 *\~french
		 *\brief		Alloue une zone mémoire pour un CPU buffer.
		 *\return		L'offset de la zone mémoire.
		 */
		C3D_API uint32_t allocate();
		/**
		 *\~english
		 *\brief		Deallocates memory.
		 *\param[in]	offset	The memory chunk offset.
		 *\~french
		 *\brief		Désalloue de la mémoire.
		 *\param[in]	offset	L'offset de la zone mémoire.
		 */
		C3D_API void deallocate( uint32_t offset );
		/**
		 *\~english
		 *\brief		Transfers data to the GPU buffer from RAM.
		 *\remarks		Transfers data from buffer[offset*sizeof( T )] to buffer[(offset+count-1)*sizeof( T )].
		 *\param[in]	stagingBuffer	The staging buffer used to transfer the data.
		 *\param[in]	commandBuffer	The command buffer on which the transfer commands are recorded.
		 *\param[in]	data			The data to upload, packed (will be aligned on upload).
		 *\param[in]	offset			The start offset.
		 *\param[in]	flags			The pipeline stage flags for the out memory barrier.
		 *\~french
		 *\brief		Transfère des données au tampon GPU à partir de la RAM.
		 *\remarks		Transfère les données de tampon[offset*sizeof( T )] à tampon[(offset+count-1) * sizeof( T )].
		 *\param[in]	stagingBuffer	Le staging buffer utilisé pour effectuer le transfer.
		 *\param[in]	commandBuffer	Le command buffer sur lequel les commandes de transfert sont enregistrées.
		 *\param[in]	data			Les données à transférer, packed (elles seront alignées lors de l'upload).
		 *\param[in]	offset			L'offset de départ.
		 *\param[in]	flags			Les indicateurs de pipeline stage pour la barrière mémoire de sortie.
		 */
		C3D_API void upload( ashes::BufferBase const & stagingBuffer
			, ashes::Queue const & queue
			, ashes::CommandPool const & commandPool
			, const void * data
			, size_t size
			, uint32_t offset
			, VkPipelineStageFlags flags )const;
		inline void upload( ashes::BufferBase const & stagingBuffer
			, ashes::Queue const & queue
			, ashes::CommandPool const & commandPool
			, ashes::ByteArray const & data
			, uint32_t offset
			, VkPipelineStageFlags flags )const
		{
			return upload( stagingBuffer
				, queue
				, commandPool
				, data.data()
				, data.size()
				, offset
				, flags );
		}
		/**
		 *\~english
		 *\brief		Transfers data to the GPU buffer from RAM.
		 *\remarks		Transfers data from buffer[offset*sizeof( T )] to buffer[(offset+count-1)*sizeof( T )].
		 *\param[in]	stagingBuffer	The staging buffer used to transfer the data.
		 *\param[in]	commandBuffer	The command buffer on which the transfer commands are recorded.
		 *\param[in]	data			The data to upload, packed (will be aligned on upload).
		 *\param[in]	offset			The start offset.
		 *\param[in]	flags			The pipeline stage flags for the out memory barrier.
		 *\~french
		 *\brief		Transfère des données au tampon GPU à partir de la RAM.
		 *\remarks		Transfère les données de tampon[offset*sizeof( T )] à tampon[(offset+count-1) * sizeof( T )].
		 *\param[in]	stagingBuffer	Le staging buffer utilisé pour effectuer le transfer.
		 *\param[in]	commandBuffer	Le command buffer sur lequel les commandes de transfert sont enregistrées.
		 *\param[in]	data			Les données à transférer, packed (elles seront alignées lors de l'upload).
		 *\param[in]	offset			L'offset de départ.
		 *\param[in]	flags			Les indicateurs de pipeline stage pour la barrière mémoire de sortie.
		 */
		C3D_API void upload( ashes::BufferBase const & stagingBuffer
			, ashes::CommandBuffer const & commandBuffer
			, const void * data
			, size_t size
			, uint32_t offset
			, VkPipelineStageFlags flags )const;
		inline void upload( ashes::BufferBase const & stagingBuffer
			, ashes::CommandBuffer const & commandBuffer
			, ashes::ByteArray const & data
			, uint32_t offset
			, VkPipelineStageFlags flags )const
		{
			return upload( stagingBuffer
				, commandBuffer
				, data.data()
				, data.size()
				, offset
				, flags );
		}
		/**
		 *\~english
		 *\brief		Transfers data to the GPU buffer from RAM.
		 *\remarks		Transfers data from buffer[offset*sizeof( T )] to buffer[(offset+count-1)*sizeof( T )].
		 *\param[in]	stagingBuffer	The staging buffer used to transfer the data.
		 *\param[in]	commandBuffer	The command buffer on which the transfer commands are recorded.
		 *\param[in]	data			The data to upload, packed (will be aligned on upload).
		 *\param[in]	offset			The start offset.
		 *\param[in]	flags			The pipeline stage flags for the out memory barrier.
		 *\~french
		 *\brief		Transfère des données au tampon GPU à partir de la RAM.
		 *\remarks		Transfère les données de tampon[offset*sizeof( T )] à tampon[(offset+count-1) * sizeof( T )].
		 *\param[in]	stagingBuffer	Le staging buffer utilisé pour effectuer le transfer.
		 *\param[in]	commandBuffer	Le command buffer sur lequel les commandes de transfert sont enregistrées.
		 *\param[in]	data			Les données à transférer, packed (elles seront alignées lors de l'upload).
		 *\param[in]	offset			L'offset de départ.
		 *\param[in]	flags			Les indicateurs de pipeline stage pour la barrière mémoire de sortie.
		 */
		C3D_API void upload( ashes::BufferBase const & stagingBuffer
			, ashes::Queue const & queue
			, ashes::CommandPool const & commandPool
			, const void * data
			, size_t size
			, uint32_t offset
			, VkPipelineStageFlags flags
			, RenderPassTimer const & timer
			, uint32_t index )const;
		inline void upload( ashes::BufferBase const & stagingBuffer
			, ashes::Queue const & queue
			, ashes::CommandPool const & commandPool
			, ashes::ByteArray const & data
			, uint32_t offset
			, VkPipelineStageFlags flags
			, RenderPassTimer const & timer
			, uint32_t index )const
		{
			return upload( stagingBuffer
				, queue
				, commandPool
				, data.data()
				, data.size()
				, offset
				, flags
				, timer
				, index );
		}
		/**
		 *\~english
		 *\brief		Transfers data to the GPU buffer from RAM.
		 *\remarks		Transfers data from buffer[offset*sizeof( T )] to buffer[(offset+count-1)*sizeof( T )].
		 *\param[in]	stagingBuffer	The staging buffer used to transfer the data.
		 *\param[in]	commandBuffer	The command buffer on which the transfer commands are recorded.
		 *\param[in]	data			The data to upload, packed (will be aligned on upload).
		 *\param[in]	offset			The start offset.
		 *\param[in]	flags			The pipeline stage flags for the out memory barrier.
		 *\param[in]	timer			The render pass timer.
		 *\param[in]	index			The render pass index.
		 *\~french
		 *\brief		Transfère des données au tampon GPU à partir de la RAM.
		 *\remarks		Transfère les données de tampon[offset*sizeof( T )] à tampon[(offset+count-1) * sizeof( T )].
		 *\param[in]	stagingBuffer	Le staging buffer utilisé pour effectuer le transfer.
		 *\param[in]	commandBuffer	Le command buffer sur lequel les commandes de transfert sont enregistrées.
		 *\param[in]	data			Les données à transférer, packed (elles seront alignées lors de l'upload).
		 *\param[in]	offset			L'offset de départ.
		 *\param[in]	flags			Les indicateurs de pipeline stage pour la barrière mémoire de sortie.
		 *\param[in]	timer			Le timer de passe de rendu.
		 *\param[in]	index			L'indice de passe de rendu.
		 */
		C3D_API void upload( ashes::BufferBase const & stagingBuffer
			, ashes::CommandBuffer const & commandBuffer
			, const void * data
			, size_t size
			, uint32_t offset
			, VkPipelineStageFlags flags
			, RenderPassTimer const & timer
			, uint32_t index )const;
		inline void upload( ashes::BufferBase const & stagingBuffer
			, ashes::CommandBuffer const & commandBuffer
			, ashes::ByteArray const & data
			, uint32_t offset
			, VkPipelineStageFlags flags
			, RenderPassTimer const & timer
			, uint32_t index )const
		{
			return upload( stagingBuffer
				, commandBuffer
				, data.data()
				, data.size()
				, offset
				, flags
				, timer
				, index );
		}
		/**
		 *\~english
		 *\brief		Transfers data from the GPU buffer to RAM.
		 *\remarks		Transfers data from buffer[offset*sizeof( T )] to buffer[(offset+count-1)*sizeof( T )].
		 *\param[in]	stagingBuffer	The staging buffer used to transfer the data.
		 *\param[in]	commandBuffer	The command buffer on which the transfer commands are recorded.
		 *\param[in]	data			Receives the data to download, packed.
		 *\param[in]	offset			The start offset.
		 *\param[in]	flags			The pipeline stage flags for the out memory barrier.
		 *\~french
		 *\brief		Transfère des données du tampon GPU vers la RAM.
		 *\remarks		Transfère les données de tampon[offset*sizeof( T )] à tampon[(offset+count-1) * sizeof( T )].
		 *\param[in]	stagingBuffer	Le staging buffer utilisé pour effectuer le transfer.
		 *\param[in]	commandBuffer	Le command buffer sur lequel les commandes de transfert sont enregistrées.
		 *\param[in]	data			Reçoit les données transférées, packed.
		 *\param[in]	offset			L'offset de départ.
		 *\param[in]	flags			Les indicateurs de pipeline stage pour la barrière mémoire de sortie.
		 */
		C3D_API void download( ashes::BufferBase const & stagingBuffer
			, ashes::Queue const & queue
			, ashes::CommandPool const & commandPool
			, void * data
			, size_t size
			, uint32_t offset
			, VkPipelineStageFlags flags
			, RenderPassTimer const & timer
			, uint32_t index )const;
		inline void download( ashes::BufferBase const & stagingBuffer
			, ashes::Queue const & queue
			, ashes::CommandPool const & commandPool
			, ashes::ByteArray & data
			, uint32_t offset
			, VkPipelineStageFlags flags
			, RenderPassTimer const & timer
			, uint32_t index )const
		{
			data.resize( size_t( m_elemCount ) * m_elemSize );
			return download( stagingBuffer
				, queue
				, commandPool
				, data.data()
				, data.size()
				, offset
				, flags
				, timer
				, index );
		}
		/**
		*\~english
		*\return
		*	\p false if the internal buffer doesn't exist.
		*\~french
		*\return
		*	\p false si le tampon interne n'existe pas.
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
		inline ashes::UniformBuffer const & getBuffer()const
		{
			return *m_buffer;
		}
		inline operator ashes::UniformBuffer const &()const
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
		inline ashes::UniformBuffer & getBuffer()
		{
			return *m_buffer;
		}
		inline operator ashes::UniformBuffer & ()
		{
			return *m_buffer;
		}
		/**
		*\~english
		*\return
		*	The size of one element in the buffer.
		*\~french
		*\return
		*	La taille d'un élément du tampon.
		*/
		inline uint32_t getElementSize()const
		{
			return uint32_t( getBuffer().getElementSize() );
		}
		/**
		*\~english
		*\brief
		*	Retrieves the aligned size for an element.
		*\param[in] size
		*	The size of an element.
		*\return
		*	The aligned size.
		*\~french
		*\brief
		*	Récupère la taille alignée d'un élément.
		*\param[in] size
		*	La taille d'un élément.
		*\return
		*	La taille alignée.
		*/
		inline uint32_t getAlignedSize( uint32_t size )const
		{
			return uint32_t( getBuffer().getAlignedSize( size ) );
		}
		/**
		*\~english
		*\return
		*	The aligned size for an element.
		*\~french
		*\return
		*	La taille alignée d'un élément.
		*/
		inline uint32_t getAlignedSize()const
		{
			return getAlignedSize( getElementSize() );
		}

	private:
		RenderSystem const & m_renderSystem;
		VkBufferUsageFlags m_usage;
		VkMemoryPropertyFlags m_flags;
		uint32_t m_elemCount;
		uint32_t m_elemSize;
		ashes::QueueShare m_sharingMode;
		std::set< uint32_t > m_available;
		ashes::UniformBufferPtr m_buffer;
		castor::String m_debugName;
		ashes::FencePtr m_transferFence;
	};

	inline UniformBufferBaseUPtr makeUniformBufferBase( RenderSystem const & renderSystem
		, VkDeviceSize count
		, VkDeviceSize size
		, VkBufferUsageFlags usage
		, VkMemoryPropertyFlags flags
		, std::string name
		, ashes::QueueShare sharingMode = {} )
	{
		return std::make_unique< UniformBufferBase >( renderSystem
			, uint32_t( count )
			, uint32_t( size )
			, usage
			, flags
			, std::move( name )
			, std::move( sharingMode ) );
	}

	template< typename T >
	class UniformBuffer
		: public UniformBufferBase
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	renderSystem	The RenderSystem.
		 *\param[in]	count			The elements count.
		 *\param[in]	usage			The buffer usage flags.
		 *\param[in]	flags			The buffer memory flags.
		 *\param[in]	debugName		The buffer debug name.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	renderSystem	Le RenderSystem.
		 *\param[in]	count			Le nombre d'éléments.
		 *\param[in]	usage			Les indicateurs d'utilisation du tampon.
		 *\param[in]	flags			Les indicateurs de mémoire du tampon.
		 *\param[in]	debugName		Le nom debug du tampon.
		 */
		inline UniformBuffer( RenderSystem const & renderSystem
			, VkDeviceSize count
			, VkBufferUsageFlags usage
			, VkMemoryPropertyFlags flags
			, castor::String debugName
			, ashes::QueueShare sharingMode = {} );
		/**
		 *\~english
		 *\brief		Transfers data to the GPU buffer from RAM.
		 *\remarks		Transfers data from buffer[offset*sizeof( T )] to buffer[(offset+count-1)*sizeof( T )].
		 *\param[in]	stagingBuffer	The staging buffer used to transfer the data.
		 *\param[in]	commandBuffer	The command buffer on which the transfer commands are recorded.
		 *\param[in]	offset			The start offset.
		 *\param[in]	flags			The pipeline stage flags for the out memory barrier.
		 *\~french
		 *\brief		Transfère des données au tampon GPU à partir de la RAM.
		 *\remarks		Transfère les données de tampon[offset*sizeof( T )] à tampon[(offset+count-1) * sizeof( T )].
		 *\param[in]	stagingBuffer	Le staging buffer utilisé pour effectuer le transfer.
		 *\param[in]	commandBuffer	Le command buffer sur lequel les commandes de transfert sont enregistrées.
		 *\param[in]	offset			L'offset de départ.
		 *\param[in]	flags			Les indicateurs de pipeline stage pour la barrière mémoire de sortie.
		 */
		inline void upload( ashes::BufferBase const & stagingBuffer
			, ashes::Queue const & queue
			, ashes::CommandPool const & commandPool
			, uint32_t offset
			, VkPipelineStageFlags flags )const;
		/**
		 *\~english
		 *\brief		Transfers data to the GPU buffer from RAM.
		 *\remarks		Transfers data from buffer[offset*sizeof( T )] to buffer[(offset+count-1)*sizeof( T )].
		 *\param[in]	stagingBuffer	The staging buffer used to transfer the data.
		 *\param[in]	commandBuffer	The command buffer on which the transfer commands are recorded.
		 *\param[in]	offset			The start offset.
		 *\param[in]	flags			The pipeline stage flags for the out memory barrier.
		 *\~french
		 *\brief		Transfère des données au tampon GPU à partir de la RAM.
		 *\remarks		Transfère les données de tampon[offset*sizeof( T )] à tampon[(offset+count-1) * sizeof( T )].
		 *\param[in]	stagingBuffer	Le staging buffer utilisé pour effectuer le transfer.
		 *\param[in]	commandBuffer	Le command buffer sur lequel les commandes de transfert sont enregistrées.
		 *\param[in]	offset			L'offset de départ.
		 *\param[in]	flags			Les indicateurs de pipeline stage pour la barrière mémoire de sortie.
		 */
		inline void upload( ashes::BufferBase const & stagingBuffer
			, ashes::CommandBuffer const & commandBuffer
			, uint32_t offset
			, VkPipelineStageFlags flags )const;
		/**
		 *\~english
		 *\brief		Transfers data to the GPU buffer from RAM.
		 *\remarks		Transfers data from buffer[offset*sizeof( T )] to buffer[(offset+count-1)*sizeof( T )].
		 *\param[in]	stagingBuffer	The staging buffer used to transfer the data.
		 *\param[in]	commandBuffer	The command buffer on which the transfer commands are recorded.
		 *\param[in]	offset			The start offset.
		 *\param[in]	flags			The pipeline stage flags for the out memory barrier.
		 *\param[in]	timer			The render pass timer.
		 *\param[in]	index			The render pass index.
		 *\~french
		 *\brief		Transfère des données au tampon GPU à partir de la RAM.
		 *\remarks		Transfère les données de tampon[offset*sizeof( T )] à tampon[(offset+count-1) * sizeof( T )].
		 *\param[in]	stagingBuffer	Le staging buffer utilisé pour effectuer le transfer.
		 *\param[in]	commandBuffer	Le command buffer sur lequel les commandes de transfert sont enregistrées.
		 *\param[in]	offset			L'offset de départ.
		 *\param[in]	flags			Les indicateurs de pipeline stage pour la barrière mémoire de sortie.
		 *\param[in]	timer			Le timer de passe de rendu.
		 *\param[in]	index			L'indice de passe de rendu.
		 */
		inline void upload( ashes::BufferBase const & stagingBuffer
			, ashes::Queue const & queue
			, ashes::CommandPool const & commandPool
			, uint32_t offset
			, VkPipelineStageFlags flags
			, RenderPassTimer const & timer
			, uint32_t index )const;
		/**
		 *\~english
		 *\brief		Transfers data to the GPU buffer from RAM.
		 *\remarks		Transfers data from buffer[offset*sizeof( T )] to buffer[(offset+count-1)*sizeof( T )].
		 *\param[in]	stagingBuffer	The staging buffer used to transfer the data.
		 *\param[in]	commandBuffer	The command buffer on which the transfer commands are recorded.
		 *\param[in]	offset			The start offset.
		 *\param[in]	flags			The pipeline stage flags for the out memory barrier.
		 *\param[in]	timer			The render pass timer.
		 *\param[in]	index			The render pass index.
		 *\~french
		 *\brief		Transfère des données au tampon GPU à partir de la RAM.
		 *\remarks		Transfère les données de tampon[offset*sizeof( T )] à tampon[(offset+count-1) * sizeof( T )].
		 *\param[in]	stagingBuffer	Le staging buffer utilisé pour effectuer le transfer.
		 *\param[in]	commandBuffer	Le command buffer sur lequel les commandes de transfert sont enregistrées.
		 *\param[in]	offset			L'offset de départ.
		 *\param[in]	flags			Les indicateurs de pipeline stage pour la barrière mémoire de sortie.
		 *\param[in]	timer			Le timer de passe de rendu.
		 *\param[in]	index			L'indice de passe de rendu.
		 */
		inline void upload( ashes::BufferBase const & stagingBuffer
			, ashes::CommandBuffer const & commandBuffer
			, uint32_t offset
			, VkPipelineStageFlags flags
			, RenderPassTimer const & timer
			, uint32_t index )const;
		/**
		*\~english
		*\brief
		*	Uploads the buffer data to VRAM
		*\param[in] offset
		*	The offset in elements from which buffer memory is mapped.
		*\param[in] range
		*	The number of elements to map.
		*\~french
		*\brief
		*	Met en VRAM les données du tampon.
		*\param[in] offset
		*	L'offset à partir duquel la mémoire du tampon est mappée.
		*\param[in] range
		*	Le nombre d'éléments à mapper.
		*/
		inline void upload( VkDeviceSize offset = 0u
			, VkDeviceSize range = 1u )const;
		/**
		 *\~english
		 *\brief		Transfers data from the GPU buffer to RAM.
		 *\remarks		Transfers data from buffer[offset*sizeof( T )] to buffer[(offset+count-1)*sizeof( T )].
		 *\param[in]	stagingBuffer	The staging buffer used to transfer the data.
		 *\param[in]	commandBuffer	The command buffer on which the transfer commands are recorded.
		 *\param[in]	offset			The start offset.
		 *\param[in]	flags			The pipeline stage flags for the out memory barrier.
		 *\~french
		 *\brief		Transfère des données du tampon GPU vers la RAM.
		 *\remarks		Transfère les données de tampon[offset*sizeof( T )] à tampon[(offset+count-1) * sizeof( T )].
		 *\param[in]	stagingBuffer	Le staging buffer utilisé pour effectuer le transfer.
		 *\param[in]	commandBuffer	Le command buffer sur lequel les commandes de transfert sont enregistrées.
		 *\param[in]	offset			L'offset de départ.
		 *\param[in]	flags			Les indicateurs de pipeline stage pour la barrière mémoire de sortie.
		 */
		inline void download( ashes::BufferBase const & stagingBuffer
			, ashes::Queue const & queue
			, ashes::CommandPool const & commandPool
			, uint32_t offset
			, VkPipelineStageFlags flags
			, RenderPassTimer const & timer
			, uint32_t index );
		/**
		*\~english
		*\return
		*	The N-th instance of the data.
		*\~french
		*\return
		*	La n-ème instance des données.
		*/
		inline T const & getData( VkDeviceSize index = 0 )const
		{
			return m_data[index];
		}
		/**
		*\~english
		*\return
		*	The N-th instance of the data.
		*\~french
		*\return
		*	La n-ème instance des données.
		*/
		inline T & getData( VkDeviceSize index = 0 )
		{
			return m_data[index];
		}
		/**
		*\~english
		*\return
		*	The data.
		*\~french
		*\return
		*	Les données.
		*/
		inline std::vector< T > const & getDatas( VkDeviceSize index = 0 )const
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
		inline std::vector< T > & getDatas( VkDeviceSize index = 0 )
		{
			return m_data;
		}

	private:
		std::vector< T > m_data;
	};

	template< typename T >
	inline UniformBufferUPtr< T > makeUniformBuffer( RenderSystem const & renderSystem
		, VkDeviceSize count
		, VkBufferUsageFlags usage
		, VkMemoryPropertyFlags flags
		, std::string name
		, ashes::QueueShare sharingMode = {} )
	{
		return std::make_unique< UniformBuffer< T > >( renderSystem
			, uint32_t( count )
			, usage
			, flags
			, std::move( name )
			, std::move( sharingMode ) );
	}

	template< typename T >
	class PoolUniformBuffer
		: public UniformBufferBase
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	renderSystem	The RenderSystem.
		 *\param[in]	count			The elements count.
		 *\param[in]	usage			The buffer usage flags.
		 *\param[in]	flags			The buffer memory flags.
		 *\param[in]	debugName		The buffer debug name.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	renderSystem	Le RenderSystem.
		 *\param[in]	count			Le nombre d'éléments.
		 *\param[in]	usage			Les indicateurs d'utilisation du tampon.
		 *\param[in]	flags			Les indicateurs de mémoire du tampon.
		 *\param[in]	debugName		Le nom debug du tampon.
		 */
		inline PoolUniformBuffer( RenderSystem const & renderSystem
			, castor::ArrayView< T > data
			, VkBufferUsageFlags usage
			, VkMemoryPropertyFlags flags
			, castor::String debugName
			, ashes::QueueShare sharingMode = {} );
		/**
		*\~english
		*\return
		*	The N-th instance of the data.
		*\~french
		*\return
		*	La n-ème instance des données.
		*/
		inline T const & getData( VkDeviceSize index = 0 )const
		{
			return m_data[index];
		}
		/**
		*\~english
		*\return
		*	The N-th instance of the data.
		*\~french
		*\return
		*	La n-ème instance des données.
		*/
		inline T & getData( VkDeviceSize index = 0 )
		{
			return m_data[index];
		}
		/**
		*\~english
		*\return
		*	The data.
		*\~french
		*\return
		*	Les données.
		*/
		inline castor::ArrayView< T > const & getDatas( VkDeviceSize index = 0 )const
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
		inline castor::ArrayView< T > & getDatas( VkDeviceSize index = 0 )
		{
			return m_data;
		}

	private:
		castor::ArrayView< T > m_data;
	};

	template< typename T >
	inline PoolUniformBufferUPtr< T > makePoolUniformBuffer( RenderSystem const & renderSystem
		, castor::ArrayView< T > data
		, VkBufferUsageFlags usage
		, VkMemoryPropertyFlags flags
		, std::string name
		, ashes::QueueShare sharingMode = {} )
	{
		return std::make_unique< PoolUniformBuffer< T > >( renderSystem
			, std::move( data )
			, usage
			, flags
			, std::move( name )
			, std::move( sharingMode ) );
	}
}

#include "Castor3D/Buffer/UniformBuffer.inl"

#endif
