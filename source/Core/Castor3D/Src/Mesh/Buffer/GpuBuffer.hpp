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
		inline GpuBufferBuddyAllocatorTraits( size_t size )
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
		 *\return		The null memory block.
		 *\~french
		 *\return		Le block mémoire nul.
		 */
		inline bool isNull( PointerType pointer )const
		{
			return pointer == getNull().data;
		}

	private:
		size_t m_allocatedSize;
	};
	using GpuBufferAllocator = castor::BuddyAllocatorT< GpuBufferBuddyAllocatorTraits >;
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
		 *\~french
		 *\brief		Constructeur.
		 */
		C3D_API explicit GpuBuffer( RenderSystem & renderSystem );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~GpuBuffer();
		/**
		 *\~english
		 *\brief		Creation function.
		 *\return		\p true if OK.
		 *\~french
		 *\brief		Fonction de création.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API virtual bool create() = 0;
		/**
		 *\~english
		 *\brief		Destruction function.
		 *\~french
		 *\brief		Fonction de destruction.
		 */
		C3D_API virtual void destroy() = 0;
		/**
		 *\~english
		 *\brief		Initialises the GPU buffer storage.
		 *\param[in]	numLevels		The allocator maximum tree size.
		 *\param[in]	minBlockSize	The minimum size for a block.
		 *\param[in]	type			Buffer access type.
		 *\param[in]	nature			Buffer access nature.
		 *\~french
		 *\brief		Initialise le stockage GPU du tampon.
		 *\param[in]	numLevels		La taille maximale de l'arbre de l'allocateur.
		 *\param[in]	minBlockSize	La taille minimale d'un bloc.
		 *\param[in]	type			Type d'accès du tampon.
		 *\param[in]	nature			Nature d'accès du tampon.
		 */
		C3D_API void initialiseStorage( uint32_t numLevels
			, uint32_t minBlockSize
			, BufferAccessType type
			, BufferAccessNature nature );
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
		 *\brief		sets the buffer's binding point.
		 *\param[in]	index	The binding point.
		 *\~french
		 *\brief		Définit le point d'attache du tampon.
		 *\param[in]	index	Le point d'attache.
		 */
		C3D_API virtual void setBindingPoint( uint32_t index )const = 0;
		/**
		 *\~english
		 *\return		The buffer's binding point.
		 *\~french
		 *\return		Le point d'attache du tampon.
		 */
		C3D_API virtual uint32_t getBindingPoint()const = 0;
		/**
		 *\~english
		 *\brief		Locks the buffer, id est maps it into memory so we can modify it.
		 *\remarks		Maps from buffer[offset*sizeof( T )] to buffer[(offset+count-1)*sizeof( T )].
		 *\param[in]	offset	The start offset in the buffer.
		 *\param[in]	count	The mapped elements count.
		 *\param[in]	flags	The lock flags.
		 *\return		The mapped buffer address.
		 *\~french
		 *\brief		Locke le tampon, càd le mappe en mémoire ram afin d'y autoriser des modifications.
		 *\remarks		Mappe de tampon[offset*sizeof( T )] à tampon[(offset+count-1) * sizeof( T )].
		 *\param[in]	offset	L'offset de départ.
		 *\param[in]	count	Le nombre d'éléments à mapper.
		 *\param[in]	flags	Les flags de lock.
		 *\return		L'adresse du tampon mappé.
		 */
		C3D_API virtual uint8_t * lock( uint32_t offset
			, uint32_t count
			, AccessTypes const & flags )const = 0;
		/**
		 *\~english
		 *\brief		Unlocks the buffer, id est unmaps it from memory so no modification can be made after that.
		 *\remarks		All modifications made in the mapped buffer are put into GPU memory.
		 *\~french
		 *\brief		Un locke le tampon, càd l'unmappe de la mémoire ram afin de ne plus autoriser de modifications dessus.
		 *\remarks		Toutes les modifications qui avaient été effectuées sur le tampon mappé sont rapatriées dans la mémoire GPU.
		 */
		C3D_API virtual void unlock()const = 0;
		/**
		 *\~english
		 *\brief		Activation function, to tell the GPU it is active.
		 *\~french
		 *\brief		Fonction d'activation, pour dire au GPU qu'il est activé.
		 */
		C3D_API virtual void bind()const = 0;
		/**
		 *\~english
		 *\brief		Activation function, to tell the GPU it is active.
		 *\remarks		Used for instanciation.
		 *\param[in]	instantiated	Tells if the buffer is instantiated.
		 *\~french
		 *\brief		Fonction d'activation, pour dire au GPU qu'il est activé.
		 *\remarks		Utilisé pour l'instanciation.
		 *\param[in]	instantiated	Dit si le tampon est instantié.
		 */
		C3D_API virtual void bind( bool instantiated )const
		{
		}
		/**
		 *\~english
		 *\brief		Deactivation function, to tell the GPU it is inactive.
		 *\~french
		 *\brief		Fonction de désactivation, pour dire au GPU qu'il est désactivé.
		 */
		C3D_API virtual void unbind()const = 0;
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
		C3D_API virtual void copy( GpuBuffer const & src
			, uint32_t srcOffset
			, uint32_t dstOffset
			, uint32_t size )const = 0;
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
		C3D_API virtual void upload( uint32_t offset
			, uint32_t count
			, uint8_t const * buffer )const = 0;
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
		C3D_API virtual void download( uint32_t offset
			, uint32_t count
			, uint8_t * buffer )const = 0;

	private:
		/**
		 *\~english
		 *\brief		Initialises the GPU buffer storage.
		 *\param[in]	count	The buffer elements count.
		 *\param[in]	type	Buffer access type.
		 *\param[in]	nature	Buffer access nature.
		 *\~french
		 *\brief		Initialise le stockage GPU du tampon.
		 *\param[in]	count	Le nombre d'éléments du tampon.
		 *\param[in]	type	Type d'accès du tampon.
		 *\param[in]	nature	Nature d'accès du tampon.
		 */
		C3D_API virtual void doInitialiseStorage( uint32_t size
			, BufferAccessType type
			, BufferAccessNature nature )const = 0;

	private:
		GpuBufferAllocatorUPtr m_allocator;
	};
}

#endif
