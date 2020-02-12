/*
See LICENSE file in root folder
*/
#ifndef ___CU_BuddyAllocator_HPP___
#define ___CU_BuddyAllocator_HPP___

#include "CastorUtils/CastorUtilsPrerequisites.hpp"

namespace castor
{
	struct BuddyAllocatorTraits
	{
		using PointerType = uint8_t *;
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
		inline explicit BuddyAllocatorTraits( size_t size )
		{
			m_memory.resize( size );
		}
		/**
		 *\~english
		 *\return		The allocator size.
		 *\~french
		 *\return		La taille de l'allocateur.
		 */
		inline size_t getSize()const
		{
			return m_memory.size();
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
			return m_memory.data() + offset;
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
			return size_t( pointer - m_memory.data() );
		}
		/**
		 *\~english
		 *\return		The null memory block.
		 *\~french
		 *\return		Le block mémoire nul.
		 */
		inline Block getNull()const
		{
			static Block result{ nullptr };
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
		std::vector< uint8_t > m_memory;
	};
	/**
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		22/08/2017
	\~english
	\brief		Buddy allocator implementation.
	\~french
	\brief		Implémentation d'un buddy allocator.
	*/
	template< typename Traits >
	class BuddyAllocatorT
		: public Traits
	{
	private:
		using PointerType = typename Traits::PointerType;
		using Block = typename Traits::Block;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	numLevels		The allocator maximum tree size.
		 *\param[in]	minBlockSize	The minimum size for a block.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	numLevels		La taille maximale de l'arbre de l'allocateur.
		 *\param[in]	minBlockSize	La taille minimale d'un bloc.
		 */
		inline BuddyAllocatorT( uint32_t numLevels
			, uint32_t minBlockSize );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		inline ~BuddyAllocatorT();
		/**
		 *\~english
		 *\param[in]	size	The requested memory size.
		 *\return		\p true if there is enough remaining memory for given size.
		 *\~french
		 *\param[in]	size	La taille requise pour la mémoire.
		 *\return		\p true s'il y a assez de mémoire restante pour la taille donnée.
		 */
		inline bool hasAvailable( size_t size )const;
		/**
		 *\~english
		 *\brief		Allocates memory.
		 *\param[in]	size	The requested memory size.
		 *\return		The memory chunk.
		 *\~french
		 *\brief		Alloue de la mémoire.
		 *\param[in]	size	La taille requiese pour la mémoire.
		 *\return		La zone mémoire.
		 */
		inline PointerType allocate( size_t size );
		/**
		 *\~english
		 *\brief		Deallocates memory.
		 *\param[in]	pointer	The memory chunk.
		 *\~french
		 *\brief		Désalloue de la mémoire.
		 *\param[in]	pointer	La zone mémoire.
		 */
		inline void deallocate( PointerType pointer );

	private:
		inline uint32_t doGetLevel( size_t size )const;
		inline size_t doGetLevelSize( uint32_t level )const;
		inline Block doAllocate( uint32_t order );
		inline void doMergeLevel( Block const & block
			, uint32_t index
			, uint32_t level );

	private:
		using FreeList = std::list< Block >;
		using PointerLevel = std::pair< size_t, uint32_t >;

	private:
		uint32_t m_numLevels;
		uint32_t m_minBlockSize;
		std::vector< FreeList > m_freeLists;
		std::vector< PointerLevel > m_allocated;
	};

	using BuddyAllocator = BuddyAllocatorT< BuddyAllocatorTraits >;
}

#include "BuddyAllocator.inl"

#endif
