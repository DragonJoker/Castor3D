/*
See LICENSE file in root folder
*/
#ifndef ___C3D_UniformBuffer_H___
#define ___C3D_UniformBuffer_H___

#include "UniformBufferBase.hpp"

#include <vector>

namespace castor3d
{
	template< typename DataT >
	class UniformBufferT
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
		 *\param[in]	sharingMode		The buffer sharing mode.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	renderSystem	Le RenderSystem.
		 *\param[in]	count			Le nombre d'éléments.
		 *\param[in]	usage			Les indicateurs d'utilisation du tampon.
		 *\param[in]	flags			Les indicateurs de mémoire du tampon.
		 *\param[in]	debugName		Le nom debug du tampon.
		 *\param[in]	sharingMode		Le mode de partage du tampon.
		 */
		inline UniformBufferT( RenderSystem const & renderSystem
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
		 *\param[in]	queue			The queue used to execute the transfer.
		 *\param[in]	commandPool		The command pool on which the command buffer will be created.
		 *\param[in]	offset			The start offset.
		 *\param[in]	flags			The pipeline stage flags for the out memory barrier.
		 *\~french
		 *\brief		Transfère des données au tampon GPU à partir de la RAM.
		 *\remarks		Transfère les données de tampon[offset*sizeof( T )] à tampon[(offset+count-1) * sizeof( T )].
		 *\param[in]	stagingBuffer	Le staging buffer utilisé pour effectuer le transfer.
		 *\param[in]	queue			La file sur laquelle le transfert sera effectué.
		 *\param[in]	commandPool		Le command pool depuis lequel le command buffer sera créé.
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
		 *\param[in]	queue			The queue used to execute the transfer.
		 *\param[in]	commandPool		The command pool on which the command buffer will be created.
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
			, FramePassTimer const & timer
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
			, FramePassTimer const & timer
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
		 *\param[in]	queue			The queue used to execute the transfer.
		 *\param[in]	commandPool		The command pool on which the command buffer will be created.
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
		 *\param[in]	offset			L'offset de départ.
		 *\param[in]	flags			Les indicateurs de pipeline stage pour la barrière mémoire de sortie.
		 *\param[in]	timer			Le timer de passe de rendu.
		 *\param[in]	index			L'indice de passe de rendu.
		 */
		inline void download( ashes::BufferBase const & stagingBuffer
			, ashes::Queue const & queue
			, ashes::CommandPool const & commandPool
			, uint32_t offset
			, VkPipelineStageFlags flags
			, FramePassTimer const & timer
			, uint32_t index );
		/**
		*\~english
		*\return
		*	The N-th instance of the data.
		*\param[in] index
		*	The instance index.
		*\~french
		*\return
		*	La n-ème instance des données.
		*\param[in] index
		*	L'indice de l'instance.
		*/
		DataT const & getData( VkDeviceSize index = 0 )const
		{
			return m_data[index];
		}
		/**
		*\~english
		*\return
		*	The N-th instance of the data.
		*\param[in] index
		*	The instance index.
		*\~french
		*\return
		*	La n-ème instance des données.
		*\param[in] index
		*	L'indice de l'instance.
		*/
		DataT & getData( VkDeviceSize index = 0 )
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
		std::vector< DataT > const & getDatas()const
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
		std::vector< DataT > & getDatas()
		{
			return m_data;
		}

	private:
		std::vector< DataT > m_data;
	};

	template< typename DataT >
	inline UniformBufferUPtrT< DataT > makeUniformBuffer( RenderSystem const & renderSystem
		, VkDeviceSize count
		, VkBufferUsageFlags usage
		, VkMemoryPropertyFlags flags
		, std::string name
		, ashes::QueueShare sharingMode = {} )
	{
		return std::make_unique< UniformBufferT< DataT > >( renderSystem
			, uint32_t( count )
			, usage
			, flags
			, std::move( name )
			, std::move( sharingMode ) );
	}
}

#include "Castor3D/Buffer/UniformBuffer.inl"

#endif
