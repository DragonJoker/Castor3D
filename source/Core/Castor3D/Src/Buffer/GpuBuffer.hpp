/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GpuBuffer_H___
#define ___C3D_GpuBuffer_H___

#include "Buffer/GpuBufferBuddyAllocator.hpp"

#include <Buffer/Buffer.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.5.0
	\date		22/10/2011
	\~english
	\brief		A GPU buffer, that uses a buddy allocator to allocate sub-buffers.
	\~french
	\brief		Un tampon GPU, utilisant un buddy allocator pour allouer des sous-tampons.
	\remark
	*/
	class GpuBuffer
	{
		friend class GpuBufferPool;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructeur.
		 */
		C3D_API GpuBuffer();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~GpuBuffer();
		/**
		 *\~english
		 *\brief		Initialises the GPU buffer storage.
		 *\param[in]	numLevels		The allocator maximum tree size.
		 *\param[in]	minBlockSize	The minimum size for a block.
		 *\param[in]	targets			The buffer targets.
		 *\param[in]	memoryFlags		The buffer memory properties.
		 *\~french
		 *\brief		Initialise le stockage GPU du tampon.
		 *\param[in]	numLevels		La taille maximale de l'arbre de l'allocateur.
		 *\param[in]	minBlockSize	La taille minimale d'un bloc.
		 *\param[in]	targets			Les cibles du tampon.
		 *\param[in]	memoryFlags		Les propriétés mémoire du tampon.
		 */
		C3D_API void initialiseStorage( renderer::Device const & device
			, uint32_t numLevels
			, uint32_t minBlockSize
			, renderer::BufferTargets targets
			, renderer::MemoryPropertyFlags memoryFlags );
		/**
		 *\~english
		 *\param[in]	size	The requested memory size.
		 *\return		\p true if there is enough remaining memory for given size.
		 *\~french
		 *\param[in]	size	La taille requise pour la mémoire.
		 *\return		\p true s'il y a assez de mémoire restante pour la taille donnée.
		 */
		C3D_API bool hasAvailable( size_t size )const;
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
		C3D_API uint32_t allocate( size_t size );
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
		 *\brief		Locks the buffer, id est maps it into memory so we can modify it.
		 *\remarks		Maps from m_buffer[offset] to m_buffer[offset + count - 1].
		 *\param[in]	offset	The start offset in the buffer.
		 *\param[in]	size	The mapped memory size.
		 *\param[in]	flags	The lock flags.
		 *\return		The mapped buffer address.
		 *\~french
		 *\brief		Locke le tampon, càd le mappe en mémoire ram afin d'y autoriser des modifications.
		 *\remarks		Mappe de m_buffer[offset] à m_buffer[offset + count - 1].
		 *\param[in]	offset	L'offset de départ.
		 *\param[in]	size	La taille de la mémoire à mapper.
		 *\param[in]	flags	Les flags de lock.
		 *\return		L'adresse du tampon mappé.
		 */
		C3D_API uint8_t * lock( uint32_t offset
			, uint32_t size
			, renderer::MemoryMapFlags const & flags )const;
		/**
		 *\~english
		 *\brief		Locks the buffer, id est maps it into memory so we can modify it.
		 *\remarks		Maps from m_buffer[offset] to m_buffer[offset + count - 1].
		 *\param[in]	offset	The start offset in the buffer.
		 *\param[in]	size	The mapped memory size.
		 *\param[in]	flags	The lock flags.
		 *\return		The mapped buffer address.
		 *\~french
		 *\brief		Locke le tampon, càd le mappe en mémoire ram afin d'y autoriser des modifications.
		 *\remarks		Mappe de m_buffer[offset] à m_buffer[offset + count - 1].
		 *\param[in]	offset	L'offset de départ.
		 *\param[in]	size	La taille de la mémoire à mapper.
		 *\param[in]	flags	Les flags de lock.
		 *\return		L'adresse du tampon mappé.
		 */
		C3D_API void flush( uint32_t offset
			, uint32_t size )const;
		/**
		 *\~english
		 *\brief		Locks the buffer, id est maps it into memory so we can modify it.
		 *\remarks		Maps from m_buffer[offset] to m_buffer[offset + count - 1].
		 *\param[in]	offset	The start offset in the buffer.
		 *\param[in]	size	The mapped memory size.
		 *\param[in]	flags	The lock flags.
		 *\return		The mapped buffer address.
		 *\~french
		 *\brief		Locke le tampon, càd le mappe en mémoire ram afin d'y autoriser des modifications.
		 *\remarks		Mappe de m_buffer[offset] à m_buffer[offset + count - 1].
		 *\param[in]	offset	L'offset de départ.
		 *\param[in]	size	La taille de la mémoire à mapper.
		 *\param[in]	flags	Les flags de lock.
		 *\return		L'adresse du tampon mappé.
		 */
		C3D_API void invalidate( uint32_t offset
			, uint32_t size )const;
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
		 *\param[in]	src			The source buffer.
		 *\param[in]	srcOffset	The start offset in the source buffer.
		 *\param[in]	dstOffset	The start offset in this buffer.
		 *\param[in]	size		The number of elements to copy.
		 *\~french
		 *\brief		Copie les données du tampon donné dans celui-ci.
		 *\param[in]	src			Le tampon source.
		 *\param[in]	srcOffset	L'offset de départ dans le tampon source.
		 *\param[in]	dstOffset	L'offset de départ dans ce tampon.
		 *\param[in]	size		Le nombre d'éléments à copier.
		 */
		C3D_API void copy( renderer::CommandBuffer const & commandBuffer
			, GpuBuffer const & src
			, uint32_t srcOffset
			, uint32_t dstOffset
			, uint32_t size )const;
		/**
		 *\~english
		 *\brief		Transfers data to the GPU buffer from RAM.
		 *\remarks		Transfers data from buffer[offset*sizeof( T )] to buffer[(offset+count-1)*sizeof( T )].
		 *\param[in]	offset	The start offset.
		 *\param[in]	count	Elements count.
		 *\param[in]	buffer	The data.
		 *\~french
		 *\brief		Transfère des données au tampon GPU à partir de la RAM.
		 *\remarks		Transfère les données de tampon[offset*sizeof( T )] à tampon[(offset+count-1) * sizeof( T )].
		 *\param[in]	offset	L'offset de départ.
		 *\param[in]	count	Nombre d'éléments.
		 *\param[in]	buffer	Les données.
		 */
		C3D_API void upload( renderer::StagingBuffer & stagingBuffer
			, renderer::CommandBuffer const & commandBuffer
			, uint32_t offset
			, uint32_t count
			, uint8_t const * buffer )const;
		/**
		 *\~english
		 *\brief		Transfers data from the GPU buffer to RAM.
		 *\remarks		Transfers data from buffer[offset*sizeof( T )] to buffer[(offset+count-1)*sizeof( T )].
		 *\param[in]	offset	The start offset.
		 *\param[in]	count	Elements count.
		 *\param[out]	buffer	The data.
		 *\~french
		 *\brief		Transfère des données du tampon GPU vers la RAM.
		 *\remarks		Transfère les données de tampon[offset*sizeof( T )] à tampon[(offset+count-1) * sizeof( T )].
		 *\param[in]	offset	L'offset de départ.
		 *\param[in]	count	Nombre d'éléments.
		 *\param[out]	buffer	Les données.
		 */
		C3D_API void download( renderer::StagingBuffer & stagingBuffer
			, renderer::CommandBuffer const & commandBuffer
			, uint32_t offset
			, uint32_t count
			, uint8_t * buffer )const;
		/**
		*\~english
		*\return
		*	The internal buffer.
		*\~french
		*\return
		*	Le tampon interne.
		*/
		inline renderer::Buffer< uint8_t > const & getBuffer()const
		{
			return *m_buffer;
		}

	private:
		void doInitialiseStorage( renderer::Device const & device
			, uint32_t size
			, renderer::BufferTargets targets
			, renderer::MemoryPropertyFlags memoryFlags );

	private:
		GpuBufferBuddyAllocatorUPtr m_allocator;
		renderer::BufferPtr< uint8_t > m_buffer;
	};
}

#endif
