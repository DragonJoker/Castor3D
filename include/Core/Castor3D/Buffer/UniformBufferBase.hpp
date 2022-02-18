/*
See LICENSE file in root folder
*/
#ifndef ___C3D_UniformBufferBase_H___
#define ___C3D_UniformBufferBase_H___

#include "BufferModule.hpp"

#include "Castor3D/Render/RenderDevice.hpp"

#include <ashespp/Buffer/UniformBuffer.hpp>
#include <ashespp/Sync/Fence.hpp>

#include <set>

namespace castor3d
{
	class UniformBufferBase
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	renderSystem	The RenderSystem.
		 *\param[in]	elemCount		The elements count.
		 *\param[in]	elemSize		The elements size.
		 *\param[in]	usage			The buffer usage flags.
		 *\param[in]	flags			The buffer memory flags.
		 *\param[in]	debugName		The buffer debug name.
		 *\param[in]	sharingMode		The buffer sharing mode.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	renderSystem	Le RenderSystem.
		 *\param[in]	elemCount		Le nombre d'éléments.
		 *\param[in]	elemSize		La taille d'un élément.
		 *\param[in]	usage			Les indicateurs d'utilisation du tampon.
		 *\param[in]	flags			Les indicateurs de mémoire du tampon.
		 *\param[in]	debugName		Le nom debug du tampon.
		 *\param[in]	sharingMode		Le mode de partage du tampon.
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
		 *\param[in]	device	The GPU device.
		 *\~french
		 *\brief		Initialise le tampon GPU.
		 *\param[in]	device	Le device GPU.
		 */
		C3D_API uint32_t initialise( RenderDevice const & device );
		/**
		 *\~english
		 *\brief		Initialises the GPU buffer.
		 *\param[in]	device		The GPU device.
		 *\param[in]	sharingMode	The buffer sharing mode.
		 *\~french
		 *\brief		Initialise le tampon GPU.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	sharingMode	Le mode de partage du tampon.
		 */
		C3D_API uint32_t initialise( RenderDevice const & device
			, ashes::QueueShare sharingMode );
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
		 *\param[in]	queue			The queue used to execute the transfer.
		 *\param[in]	commandPool		The command pool on which the command buffer will be created.
		 *\param[in]	data			The data to upload, packed (will be aligned on upload).
		 *\param[in]	size			The data size.
		 *\param[in]	offset			The start offset.
		 *\param[in]	flags			The pipeline stage flags for the out memory barrier.
		 *\~french
		 *\brief		Transfère des données au tampon GPU à partir de la RAM.
		 *\remarks		Transfère les données de tampon[offset*sizeof( T )] à tampon[(offset+count-1) * sizeof( T )].
		 *\param[in]	stagingBuffer	Le staging buffer utilisé pour effectuer le transfer.
		 *\param[in]	queue			La file sur laquelle le transfert sera effectué.
		 *\param[in]	commandPool		Le command pool depuis lequel le command buffer sera créé.
		 *\param[in]	data			Les données à transférer, packed (elles seront alignées lors de l'upload).
		 *\param[in]	size			La taille des données.
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
		/**
		 *\~english
		 *\brief		Transfers data to the GPU buffer from RAM.
		 *\remarks		Transfers data from buffer[offset*sizeof( T )] to buffer[(offset+count-1)*sizeof( T )].
		 *\param[in]	stagingBuffer	The staging buffer used to transfer the data.
		 *\param[in]	queue			The queue used to execute the transfer.
		 *\param[in]	commandPool		The command pool on which the command buffer will be created.
		 *\param[in]	data			The data to upload, packed (will be aligned on upload).
		 *\param[in]	offset			The start offset.
		 *\param[in]	flags			The pipeline stage flags for the out memory barrier.
		 *\~french
		 *\brief		Transfère des données au tampon GPU à partir de la RAM.
		 *\remarks		Transfère les données de tampon[offset*sizeof( T )] à tampon[(offset+count-1) * sizeof( T )].
		 *\param[in]	stagingBuffer	Le staging buffer utilisé pour effectuer le transfer.
		 *\param[in]	queue			La file sur laquelle le transfert sera effectué.
		 *\param[in]	commandPool		Le command pool depuis lequel le command buffer sera créé.
		 *\param[in]	data			Les données à transférer, packed (elles seront alignées lors de l'upload).
		 *\param[in]	offset			L'offset de départ.
		 *\param[in]	flags			Les indicateurs de pipeline stage pour la barrière mémoire de sortie.
		 */
		void upload( ashes::BufferBase const & stagingBuffer
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
		 *\param[in]	size			The data size.
		 *\param[in]	offset			The start offset.
		 *\param[in]	flags			The pipeline stage flags for the out memory barrier.
		 *\~french
		 *\brief		Transfère des données au tampon GPU à partir de la RAM.
		 *\remarks		Transfère les données de tampon[offset*sizeof( T )] à tampon[(offset+count-1) * sizeof( T )].
		 *\param[in]	stagingBuffer	Le staging buffer utilisé pour effectuer le transfer.
		 *\param[in]	commandBuffer	Le command buffer sur lequel les commandes de transfert sont enregistrées.
		 *\param[in]	data			Les données à transférer, packed (elles seront alignées lors de l'upload).
		 *\param[in]	size			La taille des données.
		 *\param[in]	offset			L'offset de départ.
		 *\param[in]	flags			Les indicateurs de pipeline stage pour la barrière mémoire de sortie.
		 */
		C3D_API void upload( ashes::BufferBase const & stagingBuffer
			, ashes::CommandBuffer const & commandBuffer
			, const void * data
			, size_t size
			, uint32_t offset
			, VkPipelineStageFlags flags )const;
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
		void upload( ashes::BufferBase const & stagingBuffer
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
		 *\param[in]	queue			The queue used to execute the transfer.
		 *\param[in]	commandPool		The command pool on which the command buffer will be created.
		 *\param[in]	data			The data to upload, packed (will be aligned on upload).
		 *\param[in]	size			The data size.
		 *\param[in]	offset			The start offset.
		 *\param[in]	flags			The pipeline stage flags for the out memory barrier.
		 *\param[in]	timer			The render pass timer.
		 *\param[in]	index			The render pass index.
		 *\~french
		 *\brief		Transfère des données au tampon GPU à partir de la RAM.
		 *\remarks		Transfère les données de tampon[offset*sizeof( T )] à tampon[(offset+count-1) * sizeof( T )].
		 *\param[in]	stagingBuffer	Le staging buffer utilisé pour effectuer le transfer.
		 *\param[in]	queue			La file sur laquelle le transfert sera effectué.
		 *\param[in]	commandPool		Le command pool depuis lequel le command buffer sera créé.
		 *\param[in]	data			Les données à transférer, packed (elles seront alignées lors de l'upload).
		 *\param[in]	size			La taille des données.
		 *\param[in]	offset			L'offset de départ.
		 *\param[in]	flags			Les indicateurs de pipeline stage pour la barrière mémoire de sortie.
		 *\param[in]	timer			Le timer de passe de rendu.
		 *\param[in]	index			L'indice de passe de rendu.
		 */
		C3D_API void upload( ashes::BufferBase const & stagingBuffer
			, ashes::Queue const & queue
			, ashes::CommandPool const & commandPool
			, const void * data
			, size_t size
			, uint32_t offset
			, VkPipelineStageFlags flags
			, FramePassTimer const & timer
			, uint32_t index )const;
		/**
		 *\~english
		 *\brief		Transfers data to the GPU buffer from RAM.
		 *\remarks		Transfers data from buffer[offset*sizeof( T )] to buffer[(offset+count-1)*sizeof( T )].
		 *\param[in]	stagingBuffer	The staging buffer used to transfer the data.
		 *\param[in]	queue			The queue used to execute the transfer.
		 *\param[in]	commandPool		The command pool on which the command buffer will be created.
		 *\param[in]	data			The data to upload, packed (will be aligned on upload).
		 *\param[in]	offset			The start offset.
		 *\param[in]	flags			The pipeline stage flags for the out memory barrier.
		 *\param[in]	timer			The render pass timer.
		 *\param[in]	index			The render pass index.
		 *\~french
		 *\brief		Transfère des données au tampon GPU à partir de la RAM.
		 *\remarks		Transfère les données de tampon[offset*sizeof( T )] à tampon[(offset+count-1) * sizeof( T )].
		 *\param[in]	stagingBuffer	Le staging buffer utilisé pour effectuer le transfer.
		 *\param[in]	queue			La file sur laquelle le transfert sera effectué.
		 *\param[in]	commandPool		Le command pool depuis lequel le command buffer sera créé.
		 *\param[in]	data			Les données à transférer, packed (elles seront alignées lors de l'upload).
		 *\param[in]	offset			L'offset de départ.
		 *\param[in]	flags			Les indicateurs de pipeline stage pour la barrière mémoire de sortie.
		 *\param[in]	timer			Le timer de passe de rendu.
		 *\param[in]	index			L'indice de passe de rendu.
		 */
		void upload( ashes::BufferBase const & stagingBuffer
			, ashes::Queue const & queue
			, ashes::CommandPool const & commandPool
			, ashes::ByteArray const & data
			, uint32_t offset
			, VkPipelineStageFlags flags
			, FramePassTimer const & timer
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
		 *\param[in]	size			The data size.
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
		 *\param[in]	size			La taille des données.
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
			, FramePassTimer const & timer
			, uint32_t index )const;
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
		void upload( ashes::BufferBase const & stagingBuffer
			, ashes::CommandBuffer const & commandBuffer
			, ashes::ByteArray const & data
			, uint32_t offset
			, VkPipelineStageFlags flags
			, FramePassTimer const & timer
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
		 *\param[in]	queue			The queue used to execute the transfer.
		 *\param[in]	commandPool		The command pool on which the command buffer will be created.
		 *\param[in]	data			Receives the data to download, packed.
		 *\param[in]	size			The data size.
		 *\param[in]	offset			The start offset.
		 *\param[in]	flags			The pipeline stage flags for the out memory barrier.
		 *\param[in]	timer			The render pass timer.
		 *\param[in]	index			The render pass index.
		 *\~french
		 *\brief		Transfère des données du tampon GPU vers la RAM.
		 *\remarks		Transfère les données de tampon[offset*sizeof( T )] à tampon[(offset+count-1) * sizeof( T )].
		 *\param[in]	stagingBuffer	Le staging buffer utilisé pour effectuer le transfer.
		 *\param[in]	queue			La file sur laquelle le transfert sera effectué.
		 *\param[in]	commandPool		Le command pool depuis lequel le command buffer sera créé.
		 *\param[in]	data			Reçoit les données transférées, packed.
		 *\param[in]	size			La taille des données.
		 *\param[in]	offset			L'offset de départ.
		 *\param[in]	flags			Les indicateurs de pipeline stage pour la barrière mémoire de sortie.
		 *\param[in]	timer			Le timer de passe de rendu.
		 *\param[in]	index			L'indice de passe de rendu.
		 */
		C3D_API void download( ashes::BufferBase const & stagingBuffer
			, ashes::Queue const & queue
			, ashes::CommandPool const & commandPool
			, void * data
			, size_t size
			, uint32_t offset
			, VkPipelineStageFlags flags
			, FramePassTimer const & timer
			, uint32_t index )const;
		/**
		 *\~english
		 *\brief		Transfers data from the GPU buffer to RAM.
		 *\remarks		Transfers data from buffer[offset*sizeof( T )] to buffer[(offset+count-1)*sizeof( T )].
		 *\param[in]	stagingBuffer	The staging buffer used to transfer the data.
		 *\param[in]	queue			The queue used to execute the transfer.
		 *\param[in]	commandPool		The command pool on which the command buffer will be created.
		 *\param[in]	data			Receives the data to download, packed.
		 *\param[in]	offset			The start offset.
		 *\param[in]	flags			The pipeline stage flags for the out memory barrier.
		 *\param[in]	timer			The render pass timer.
		 *\param[in]	index			The render pass index.
		 *\~french
		 *\brief		Transfère des données du tampon GPU vers la RAM.
		 *\remarks		Transfère les données de tampon[offset*sizeof( T )] à tampon[(offset+count-1) * sizeof( T )].
		 *\param[in]	stagingBuffer	Le staging buffer utilisé pour effectuer le transfer.
		 *\param[in]	queue			La file sur laquelle le transfert sera effectué.
		 *\param[in]	commandPool		Le command pool depuis lequel le command buffer sera créé.
		 *\param[in]	data			Reçoit les données transférées, packed.
		 *\param[in]	offset			L'offset de départ.
		 *\param[in]	flags			Les indicateurs de pipeline stage pour la barrière mémoire de sortie.
		 *\param[in]	timer			Le timer de passe de rendu.
		 *\param[in]	index			L'indice de passe de rendu.
		 */
		void download( ashes::BufferBase const & stagingBuffer
			, ashes::Queue const & queue
			, ashes::CommandPool const & commandPool
			, ashes::ByteArray & data
			, uint32_t offset
			, VkPipelineStageFlags flags
			, FramePassTimer const & timer
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
		ashes::UniformBuffer const & getBuffer()const
		{
			return *m_buffer;
		}
		operator ashes::UniformBuffer const &()const
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
		ashes::UniformBuffer & getBuffer()
		{
			return *m_buffer;
		}
		operator ashes::UniformBuffer & ()
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
		uint32_t getElementSize()const
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
		uint32_t getAlignedSize( uint32_t size )const
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
		uint32_t getAlignedSize()const
		{
			return getAlignedSize( getElementSize() );
		}

	private:
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
		return castor::makeUnique< UniformBufferBase >( renderSystem
			, uint32_t( count )
			, uint32_t( size )
			, usage
			, flags
			, std::move( name )
			, std::move( sharingMode ) );
	}
}

#endif
