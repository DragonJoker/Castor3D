/*
See LICENSE file in root folder
*/
#ifndef ___C3D_UniformBuffer_H___
#define ___C3D_UniformBuffer_H___

#include "Castor3D/Castor3DPrerequisites.hpp"

#include <Ashes/Buffer/UniformBuffer.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.5.0
	\date		22/10/2011
	\~english
	\brief		A uniform buffer, than can contain multiple sub-buffers.
	\~french
	\brief		Un tampon d'uniformes, puovant contenir de multiples sous-tampons.
	\remark
	*/
	template< typename T >
	class UniformBuffer
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	renderSystem	The RenderSystem.
		 *\param[in]	count			The elements count.
		 *\param[in]	flags			The buffer memory flags.
		 *\param[in]	debugName		The buffer debug name.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	renderSystem	Le RenderSystem.
		 *\param[in]	count			Le nombre d'éléments.
		 *\param[in]	flags			Les indicateurs de mémoire du tampon.
		 *\param[in]	debugName		Le nom debug du tampon.
		 */
		inline UniformBuffer( RenderSystem const & renderSystem
			, uint32_t count
			, ashes::MemoryPropertyFlags flags
			, castor::String debugName );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		inline ~UniformBuffer();
		/**
		 *\~english
		 *\brief		Initialises the GPU buffer.
		 *\~french
		 *\brief		Initialise le tampon GPU.
		 */
		inline void initialise();
		/**
		 *\~english
		 *\brief		Cleans up the GPU buffer.
		 *\~french
		 *\brief		Nettoie le tampon GPU.
		 */
		inline void cleanup();
		/**
		 *\~english
		 *\return		\p true if there is enough remaining memory for a new element.
		 *\~french
		 *\return		\p true s'il y a assez de mémoire restante pour un nouvel élément.
		 */
		inline bool hasAvailable()const;
		/**
		 *\~english
		 *\brief		Allocates a memory chunk for a CPU buffer.
		 *\return		The memory chunk offset.
		 *\~french
		 *\brief		Alloue une zone mémoire pour un CPU buffer.
		 *\return		L'offset de la zone mémoire.
		 */
		inline uint32_t allocate();
		/**
		 *\~english
		 *\brief		Deallocates memory.
		 *\param[in]	offset	The memory chunk offset.
		 *\~french
		 *\brief		Désalloue de la mémoire.
		 *\param[in]	offset	L'offset de la zone mémoire.
		 */
		inline void deallocate( uint32_t offset );
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
		inline void upload( ashes::StagingBuffer & stagingBuffer
			, ashes::CommandBuffer const & commandBuffer
			, uint32_t offset
			, ashes::PipelineStageFlags flags
			, RenderPassTimer const & timer
			, uint32_t index )const;
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
		inline void download( ashes::StagingBuffer & stagingBuffer
			, ashes::CommandBuffer const & commandBuffer
			, uint32_t offset
			, ashes::PipelineStageFlags flags )const;
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
		inline ashes::UniformBuffer< T > const & getBuffer()const
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
		inline ashes::UniformBuffer< T > & getBuffer()
		{
			return *m_buffer;
		}

	private:
		RenderSystem const & m_renderSystem;
		uint32_t m_count;
		ashes::UniformBufferPtr< T > m_buffer;
		std::set< uint32_t > m_available;
		ashes::MemoryPropertyFlags m_flags;
		castor::String m_debugName;
	};
}

#include "Castor3D/Buffer/UniformBuffer.inl"

#endif
