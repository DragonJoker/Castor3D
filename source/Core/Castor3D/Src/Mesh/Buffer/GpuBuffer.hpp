/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GPU_BUFFER_H___
#define ___C3D_GPU_BUFFER_H___

#include "Castor3DPrerequisites.hpp"

#include <Design/OwnedBy.hpp>
#include <Pool/BuddyAllocator.hpp>

#include <cstddef>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.10.0
	\~english
	\brief		Buddy allocator traits, for GPU buffers.
	\~french
	\brief		Traits de buddy allocator, pour les tampons GPU.
	\remark
	*/
	struct GpuBufferBuddyAllocatorTraits
	{
		using PointerType = uint32_t;
		struct Block
		{
			PointerType data;
		};
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	size	The allocator size.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	size	La taille de l'allocateur.
		 */
		inline explicit GpuBufferBuddyAllocatorTraits( size_t size )
			: m_allocatedSize{ size }
		{
		}
		/**
		 *\~english
		 *\return		The allocator size.
		 *\~french
		 *\return		La taille de l'allocateur.
		 */
		inline size_t getSize()const
		{
			return m_allocatedSize;
		}
		/**
		 *\~english
		 *\brief		Creates a memory block.
		 *\param[in]	offset	The block memory offset.
		 *\return		The block.
		 *\~french
		 *\brief		Crée un bloc de mémoire.
		 *\param[in]	offset	Le décalage en mémoire du bloc.
		 *\return		Le block.
		 */
		inline PointerType getPointer( uint32_t offset )
		{
			return offset;
		}
		/**
		 *\~english
		 *\brief		Retrieves the offset from a memory block.
		 *\param[in]	pointer	The memory block.
		 *\return		The offset.
		 *\~french
		 *\brief		Récupère le décalage en mémoire d'un block.
		 *\param[in]	pointer	Le bloc mémoire.
		 *\return		Le décalage.
		 */
		inline size_t getOffset( PointerType pointer )const
		{
			return size_t( pointer );
		}
		/**
		 *\~english
		 *\return		The null memory block.
		 *\~french
		 *\return		Le block mémoire nul.
		 */
		inline Block getNull()const
		{
			static Block result{ ~0u };
			return result;
		}
		/**
		 *\~english
		 *\return		\p true if given pointer is null.
		 *\~french
		 *\return		\p true si le pointeur donné est nul.
		 */
		inline bool isNull( PointerType pointer )const
		{
			return pointer == getNull().data;
		}

	private:
		size_t m_allocatedSize;
	};
	//!\~english	The allocator for GPU buffers.
	//!\~french		L'allocateur pour les tampons GPU.
	using GpuBufferAllocator = castor::BuddyAllocatorT< GpuBufferBuddyAllocatorTraits >;
	//!\~english	A pointer to an allocator for GPU buffers.
	//!\~french		Un pointeur sur un allocateur pour les tampons GPU.
	using GpuBufferAllocatorUPtr = std::unique_ptr< GpuBufferAllocator >;
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.5.0
	\date		22/10/2011
	\~english
	\brief		Base class for renderer dependant buffers
	\remarks	Contains data for multiple CPU buffers.
	\~french
	\brief		Classe de base pour les tampons dépendants du renderer
	\remarks	Contient les données pour de multiples tampons CPU.
	\remark
	*/
	class GpuBuffer
		: public castor::OwnedBy< RenderSystem >
	{
		friend class GpuBufferPool;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	renderSystem	The RenderSystem.
		 *\param[in]	target			The buffer target.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	renderSystem	Le RenderSystem.
		 *\param[in]	target			La cible du tampon.
		 */
		C3D_API explicit GpuBuffer( RenderSystem & renderSystem
			, renderer::BufferTarget target );
		/**
		 *\~english
		 *\brief		Cleans up the GPU buffer storage.
		 *\remarks		Used for buffer not in pools.
		 *\~french
		 *\brief		Initialise le stockage GPU du tampon.
		 *\remarks		Utilisé pour les tampons qui ne sont pas dans un pool.
		 */
		C3D_API void cleanupStorage();
		/**
		 *\~english
		 *\brief		Initialises the GPU buffer storage.
		 *\remarks		Used for buffer pools, through allocator.
		 *\param[in]	numLevels		The allocator maximum tree size.
		 *\param[in]	minBlockSize	The minimum size for a block.
		 *\param[in]	flags			The buffer memory flags.
		 *\~french
		 *\brief		Initialise le stockage GPU du tampon.
		 *\remarks		Utilisé pour les pools de tampons, au travers de l'allocateur.
		 *\param[in]	numLevels		La taille maximale de l'arbre de l'allocateur.
		 *\param[in]	minBlockSize	La taille minimale d'un bloc.
		 *\param[in]	flags			Les indicateurs de mémoire du tampon.
		 */
		C3D_API void initialiseStorage( uint32_t numLevels
			, uint32_t minBlockSize
			, renderer::MemoryPropertyFlags flags );
		/**
		 *\~english
		 *\brief		Initialises the GPU buffer storage.
		 *\param[in]	size	The buffer size.
		 *\param[in]	flags	The buffer memory flags.
		 *\~french
		 *\brief		Initialise le stockage GPU du tampon.
		 *\param[in]	size	La taille du tampon.
		 *\param[in]	flags	Les indicateurs de mémoire du tampon.
		 */
		C3D_API void initialiseStorage( uint32_t size
			, renderer::MemoryPropertyFlags flags );
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
		 *\remarks		Maps from buffer[offset*sizeof( T )] to buffer[(offset+count-1)*sizeof( T )].
		 *\param[in]	offset	The start offset in the buffer.
		 *\param[in]	size	The mapped memory size.
		 *\param[in]	flags	The lock flags.
		 *\return		The mapped buffer address.
		 *\~french
		 *\brief		Locke le tampon, càd le mappe en mémoire ram afin d'y autoriser des modifications.
		 *\remarks		Mappe de tampon[offset*sizeof( T )] à tampon[(offset+count-1) * sizeof( T )].
		 *\param[in]	offset	L'offset de départ.
		 *\param[in]	size	La taille mémoire à mapper.
		 *\param[in]	flags	Les flags de lock.
		 *\return		L'adresse du tampon mappé.
		 */
		C3D_API uint8_t * lock( uint32_t offset
			, uint32_t size
			, renderer::MemoryMapFlags const & flags )const;
		/**
		 *\~english
		 *\brief		Unlocks the buffer, id est unmaps it from memory so no modification can be made after that.
		 *\remarks		All modifications made in the mapped buffer are put into GPU memory.
		 *\param[in]	size		The mapped elements count.
		 *\param[in]	modified	Tells if the buffer has ben modified, and if the VRAM must be updated.
		 *\~french
		 *\brief		Un locke le tampon, càd l'unmappe de la mémoire ram afin de ne plus autoriser de modifications dessus.
		 *\remarks		Toutes les modifications qui avaient été effectuées sur le tampon mappé sont rapatriées dans la mémoire GPU.
		 *\param[in]	size		Le nombre d'éléments mappés.
		 *\param[in]	modified	Dit si le tampon a été modifié, et donc si la VRAM doit être mise à jour.
		 */
		C3D_API void unlock( uint32_t size, bool modified )const;
		/**
		 *\~english
		 *\return		The GPU storage.
		 *\~french
		 *\return		Le stockage GPU.
		 */
		inline renderer::BufferBase const & getStorage()const
		{
			REQUIRE( m_storage );
			return *m_storage;
		}

	private:
		//!\~english	The allocation policy.
		//!\~french		La politique d'allocation.
		GpuBufferAllocatorUPtr m_allocator;
		//!\~english	The GPU storage.
		//!\~french		Le stockage GPU.
		renderer::BufferBasePtr m_storage;
		//!\~english	The buffer target.
		//!\~french		La cible du tampon.
		renderer::BufferTarget m_target;
		//!\~english	The buffer memory flags.
		//!\~french		Les indcateurs de mémoire du tampon.
		renderer::MemoryPropertyFlags m_flags;
	};
}

#endif
