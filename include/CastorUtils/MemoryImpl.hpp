/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___Castor_MemoryImpl___
#define ___Castor_MemoryImpl___

#undef CHECK_MEMORYLEAKS

#if !defined( NDEBUG )
#	define CHECK_MEMORYLEAKS	0
#	define USE_VLD				0
#else
#	define CHECK_MEMORYLEAKS	0
#	define USE_VLD				0
#endif

#include <fstream>
#include <iostream>
#include <ctime>
#include <string>
#include <vector>
#include <map>
#include <new>
#include <algorithm>
#include "MultiThreadConfig.hpp"
#include "Unique.hpp"

#if defined( _MSC_VER )
#	pragma warning( push )
#	pragma warning( disable:4290 )
#endif

#if defined( new )
#	undef new
#endif
#if defined( delete )
#	undef delete
#endif
#if defined( max )
#	undef max
#endif

namespace Castor
{
	/**
	 *\~english
	 *\brief		Allocator
	 *\param[in]	p_size	The allocation size
	 *\return		The allocated buffer
	 *\~french
	 *\brief		Allocateur
	 *\param[in]	p_size	La taille à allouer
	 *\return		Le buffer alloué
	 */
	void * Allocate( std::size_t p_size );
	/**
	 *\~english
	 *\brief		Allocator, no throw version
	 *\param[in]	p_size	The allocation size
	 *\return		The allocated buffer
	 *\~french
	 *\brief		Allocateur, version no throw
	 *\param[in]	p_size	La taille à allouer
	 *\return		Le buffer alloué
	 */
	void * Allocate( std::size_t p_size, std::nothrow_t const & )throw();
	/**
	 *\~english
	 *\brief		De-allocator
	 *\param[in]	p_pMemory	The allocated memory
	 *\~french
	 *\brief		Désallocateur
	 *\param[in]	p_pMemory	La mémoire allouée
	 */
	void Deallocate( void * p_pMemory )throw();
}

#if CHECK_MEMORYLEAKS
#	if !defined( VLD_AVAILABLE ) || !USE_VLD
#		define	DECLARE_MEMORY_MANAGER_INSTANCE( x )	Castor::MemoryManager * x
#		define	INITIALISE_MEMORY_MANAGER( x )			,	x	( NULL )
#		define	SET_MEMORY_MANAGER( x )					x = new Castor::MemoryManager
#		define	SET_MEMORY_MANAGER_FROM( x, y )			x = y;					\
														x->SetSingleton( y )
#		define	RELEASE_MEMORY_MANAGER( x )				delete x;	\
														x = NULL
namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Memory block base class
	\remark		Used by the memory manager to know what is allocated and deallocated
	\~french
	\brief		Classe de base pour les blocs de mémoire
	\remark		Utilisé par le gestionnaire de mémoire pour savoir ce qui est alloué et désalloué
	*/
	class MemoryBlock
	{
	public:
		//!\~english	The file in which allocation occurs		\~french	Le fichier dans lequel l'allocation a lieu
		char const *	file;
		//!\~english	The function in which allocation occurs	\~french	La fonction dans laquelle l'allocation a lieu
		char const *	function;
		//!\~english	The line in which allocation occurs		\~french	La ligne dans laquelle l'allocation a lieu
		uint32_t		line;
		//!\~english	The allocation size						\~french	La taille allouée
		std::size_t		size;
		//!\~english	Tells if the allocation was an array	\~french	Dit si l'allocation était un tableau
		bool			isArray;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		MemoryBlock();
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\~french
		 *\brief		Constructeur par copie
		 */
		MemoryBlock( MemoryBlock const & p_copy );
		/**
		 *\~english
		 *\brief		Move constructor
		 *\~french
		 *\brief		Constructeur par déplacement
		 */
		MemoryBlock( MemoryBlock && p_copy );
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 */
		MemoryBlock & operator =( MemoryBlock const & p_copy );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement
		 */
		MemoryBlock & operator =( MemoryBlock && p_copy );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pszFile		Allocation file
		 *\param[in]	p_pszFunction	Allocation function
		 *\param[in]	p_uiLine		Allocation line
		 *\param[in]	p_size			Allocation size
		 *\param[in]	p_array			Tells if allocation is an array
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pszFile		Fichier de l'allocation
		 *\param[in]	p_pszFunction	Fonction de l'allocation
		 *\param[in]	p_uiLine		Ligne de l'allocation
		 *\param[in]	p_size			Taille de l'allocation
		 *\param[in]	p_array			Dit si l'allocation est un tableau
		 */
		MemoryBlock( char const * p_pszFile, char const * p_pszFunction, uint32_t p_uiLine, std::size_t p_size, bool p_array );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pszFile		Allocation file
		 *\param[in]	p_pszFunction	Allocation function
		 *\param[in]	p_uiLine		Allocation line
		 *\param[in]	p_size			Allocation size
		 *\param[in]	p_array			Tells if allocation is an array
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pszFile		Fichier de l'allocation
		 *\param[in]	p_pszFunction	Fonction de l'allocation
		 *\param[in]	p_uiLine		Ligne de l'allocation
		 *\param[in]	p_size			Taille de l'allocation
		 *\param[in]	p_array			Dit si l'allocation est un tableau
		 */
		MemoryBlock( char const * p_pszFile, char const * p_pszFunction, uint32_t p_uiLine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~MemoryBlock();
		/**
		 *\~english
		 *\brief		new operator
		 *\param[in]	p_size	The allocation size
		 *\return		The allocated pointer
		 *\~french
		 *\brief		Opérateur new
		 *\param[in]	p_size	La taille d'allocation
		 *\return		Le pointeur alloué
		 */
		void * operator new( std::size_t p_size );
		/**
		 *\~english
		 *\brief		delete operator
		 *\param[in]	p_pointer	The memory allocated
		 *\~french
		 *\brief		Opérateur delete
		 *\param[in]	p_pointer	La mémoire allouée
		 */
		void operator delete( void * p_pointer );
		/**
		 *\~english
		 *\brief		Retrieves the block type name
		 *\return		The name
		 *\~french
		 *\brief		Récupère le nom du type de bloc
		 *\return		Le nom
		 */
		virtual char const * GetBlockType()const { return "Unknown class"; }
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		08/10/2010
	\~english
	\brief		Memory management class
	\remark		Used to check leaks, allocations and deallocations
	\~french
	\brief		Classe de gestion de la mémoire
	\remark		Utilisée afin de vérifier les allocations, désallocations et fuites de mémoire
	*/
	class MemoryManager : public Unique< MemoryManager >
	{
	private:
		DECLARE_MAP(	void*, MemoryBlock,	MemoryBlock );
		DECLARE_VECTOR(	MemoryBlock,		MemoryBlock );

	private:
		std::recursive_mutex 	m_mutex;
		MemoryBlock				m_lastAllocatedBlock;
		MemoryBlockMap			m_mapAllocated;
		MemoryBlockArray		m_failedNews;
		MemoryBlockArray		m_failedDeletes;
		bool 					m_initialised;
		bool					m_locked;
		uint64_t				m_currentMemoryAllocated;
		uint64_t				m_maximumMemoryAllocated;
		uint64_t				m_totalObjectsAllocated;
		uint64_t				m_totalArraysAllocated;
		uint64_t				m_totalMemoryAllocated;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		MemoryManager();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~MemoryManager();
		/**
		 *\~english
		 *\brief		Registers a successful allocation
		 *\param[in]	p_ptr			Allocated pointer
		 *\param[in]	p_pszFile		Allocation file
		 *\param[in]	p_pszFunction	Allocation function
		 *\param[in]	p_uiLine		Allocation line
		 *\param[in]	p_size			Allocation size
		 *\param[in]	p_array			Tells if allocation is an array
		 *\~french
		 *\brief		Enregistre une allocation réussie
		 *\param[in]	p_ptr			Le pointeur alloué
		 *\param[in]	p_pszFile		Fichier de l'allocation
		 *\param[in]	p_pszFunction	Fonction de l'allocation
		 *\param[in]	p_uiLine		Ligne de l'allocation
		 *\param[in]	p_size			Taille de l'allocation
		 *\param[in]	p_array			Dit si l'allocation est un tableau
		 */
		static void AddSuccessfulAllocation( void * p_ptr, char const * p_pszFile, char const * p_pszFunction, uint32_t p_uiLine, std::size_t p_size, bool p_array );
		/**
		 *\~english
		 *\brief		Registers a failed allocation
		 *\param[in]	p_size		The wanted size
		 *\param[in]	p_isArray	Tells if allocation is an array
		 *\return
		 *\~french
		 *\brief		Enregistre une allocation échouée
		 *\param[in]	p_size		La taille voulue
		 *\param[in]	p_isArray	Dit si l'allocation est un tableau
		 */
		static void AddFailedAllocation( std::size_t p_size, bool p_isArray );
		/**
		 *\~english
		 *\brief		Unregisters an allocation
		 *\param[in]	p_pointer	The allocated pointer
		 *\return		\p true if the allocation was found
		 *\~french
		 *\brief		Enlève une allocation
		 *\param[in]	p_pointer	Le pointeur alloué
		 *\return		\p true si l'allocation a été trouvée
		 */
		static bool RemoveAllocation( void * p_pointer );
		/**
		 *\~english
		 *\brief		Retrieves the instance of MemoryManager
		 *\return		A reference to the instance
		 *\~french
		 *\brief		Récupère l'instance de MemoryManager
		 *\return		Une référence ssur l'instance
		 */
		static MemoryManager & GetSingleton();
		/**
		 *\~english
		 *\brief		Retrieves a pointer to the instance of MemoryManager
		 *\return		A pointer to the instance
		 *\~french
		 *\brief		Récupère un pointeur sur l'instance de MemoryManager
		 *\return		Un pointeur sur l'instance
		 */
		static MemoryManager * GetSingletonPtr();
		/**
		 *\~english
		 *\brief		Sets the singleton to another one
		 *\param[in]	p_pInstance	The other singleton
		 *\~french
		 *\brief		Définit le singleton à un autre
		 *\param[in]	p_pInstance	L'autre singleton
		 */
		void SetSingleton( MemoryManager * p_pInstance );
		/**
		 *\~english
		 *\brief		Locks the instance
		 *\~french
		 *\brief		Lock l'instance
		 */
		static void Lock();
		/**
		 *\~english
		 *\brief		Unlocks the instance
		 *\~french
		 *\brief		Unlock l'instance
		 */
		static void Unlock();
		/**
		 *\~english
		 *\brief		Tells if the instance is locked
		 *\return		\p true if it is locked
		 *\~french
		 *\brief		Dit si l'instance est lockée
		 *\return		\p true si elle est lockée
		 */
		static bool IsLocked();
		/**
		 *\~english
		 *\brief		Tells if the instance has been initialised
		 *\return		\p true if it is initialised
		 *\~french
		 *\brief		Dit si l'instance a été initialisée
		 *\return		\p true si elle est initialisée
		 */
		static bool Exists();
		/**
		 *\~english
		 *\brief		new operator
		 *\param[in]	p_size	The allocation size
		 *\return		The allocated pointer
		 *\~french
		 *\brief		Opérateur new
		 *\param[in]	p_size	La taille d'allocation
		 *\return		Le pointeur alloué
		 */
		void * operator new( std::size_t p_size );
		/**
		 *\~english
		 *\brief		delete operator
		 *\param[in]	p_pointer	The memory allocated
		 *\~french
		 *\brief		Opérateur delete
		 *\param[in]	p_pointer	La mémoire allouée
		 */
		void operator delete( void * p_pointer );
		/**
		 *\~english
		 *\brief		Inserts an allocated memory block
		 *\param[in]	p_block	The allocated memory block
		 *\~french
		 *\brief		Ajoute un block de mémoire allouée
		 *\param[in]	p_block	Le bloc de mémoire allouée
		 */
		MemoryManager & operator <<( MemoryBlock & p_block );
		/**
		 *\~english
		 *\brief		Inserts an allocated memory block
		 *\param[in]	p_block	The allocated memory block
		 *\~french
		 *\brief		Ajoute un block de mémoire allouée
		 *\param[in]	p_block	Le bloc de mémoire allouée
		 */
		void * operator <<( void * p_pMemory );
		/**
		 *\~english
		 *\brief		Removes and deletes a memory block
		 *\param[in]	p_pMemory	The memory block
		 *\~french
		 *\brief		Retire et désalloue un block de mémoire
		 *\param[in]	p_block	Le bloc de mémoire
		 */
		void operator >>( void * p_pMemory );
		/**
		 *\~english
		 *\brief		Removes and deletes a memory block
		 *\param[in]	p_pMemory	The memory block
		 *\~french
		 *\brief		Retire et désalloue un block de mémoire
		 *\param[in]	p_block	Le bloc de mémoire
		 */
		void operator -( void * p_pMemory );

	private:
		void DoAddSuccessfulAllocation( void * p_ptr );
		void DoAddFailedAllocation( std::size_t p_size, bool p_isArray );
		bool DoRemoveAllocation( void * p_pointer );
		void DoFinalReport();
	};
 	/**
 	 *\~english
 	 *\brief		Allocation function
 	 *\param[in]	p_size			The allocation size
	 *\param[in]	p_szFile		The file name
	 *\param[in]	p_szFunction	The function name
	 *\param[in]	p_uiLine		The line number
	 *\param[in]	p_array			Tells the allocation is an array
 	 *\return		The allocated buffer
 	 *\~french
 	 *\brief		Fonction d'allocation
 	 *\param[in]	p_size			La taille à allouer
	 *\param[in]	p_szFile		Le nom du fichier
	 *\param[in]	p_szFunction	Le nom de la fonction
	 *\param[in]	p_uiLine		Le numéro de ligne
	 *\param[in]	p_array			Dit que l'allocation est un tableau
 	 *\return		Le buffer alloué
 	 */
 	void * TracedAllocate( std::size_t p_size, char const * p_szFile, char const * p_szFunction, uint32_t p_uiLine, bool p_array );
 	/**
 	 *\~english
 	 *\brief		Allocation function, no throw version
 	 *\param[in]	p_size			The allocation size
	 *\param[in]	p_szFile		The file name
	 *\param[in]	p_szFunction	The function name
	 *\param[in]	p_uiLine		The line number
	 *\param[in]	p_array			Tells the allocation is an array
 	 *\return		The allocated buffer
 	 *\~french
 	 *\brief		Fonction d'allocation, version no throw
 	 *\param[in]	p_size			La taille à allouer
	 *\param[in]	p_szFile		Le nom du fichier
	 *\param[in]	p_szFunction	Le nom de la fonction
	 *\param[in]	p_uiLine		Le numéro de ligne
	 *\param[in]	p_array			Dit que l'allocation est un tableau
 	 *\return		Le buffer alloué
 	 */
 	void * TracedAllocate( std::size_t p_size, char const * p_szFile, char const * p_szFunction, uint32_t p_uiLine, bool p_array, std::nothrow_t const & )throw();
 	/**
 	 *\~english
 	 *\brief		Deallocation function
 	 *\param[in]	p_pMemory		The allocated memory
	 *\param[in]	p_szFile		The file name
	 *\param[in]	p_szFunction	The function name
	 *\param[in]	p_uiLine		The line number
	 *\param[in]	p_array			Tells the deallocation is an array
 	 *\~french
 	 *\brief		Fonction de désallocation
 	 *\param[in]	p_pMemory		La mémoire allouée
	 *\param[in]	p_szFile		Le nom du fichier
	 *\param[in]	p_szFunction	Le nom de la fonction
	 *\param[in]	p_uiLine		Le numéro de ligne
	 *\param[in]	p_array			Dit que la désallocation est un tableau
 	 */
 	void TracedDeallocate( void * p_pMemory, char const * p_szFile, char const * p_szFunction, uint32_t p_uiLine, bool p_array );
 	/**
 	 *\~english
 	 *\brief		Deallocation function, no throw version
 	 *\param[in]	p_pMemory		The allocated memory
	 *\param[in]	p_szFile		The file name
	 *\param[in]	p_szFunction	The function name
	 *\param[in]	p_uiLine		The line number
	 *\param[in]	p_array			Tells the deallocation is an array
 	 *\~french
 	 *\brief		Fonction de désallocation, version no throw
 	 *\param[in]	p_pMemory		La mémoire allouée
	 *\param[in]	p_szFile		Le nom du fichier
	 *\param[in]	p_szFunction	Le nom de la fonction
	 *\param[in]	p_uiLine		Le numéro de ligne
	 *\param[in]	p_array			Dit que la désallocation est un tableau
 	 */
 	void TracedDeallocate( void * p_pMemory, char const * p_szFile, char const * p_szFunction, uint32_t p_uiLine, bool p_array, std::nothrow_t const & )throw();
}
#	else
#		pragma warning( push )
#		pragma warning( disable:4311 )
#		pragma warning( disable:4312 )
#		include <vld.h>
#		pragma comment( lib, "vld.lib" )
#		pragma warning( pop )
#		define	DECLARE_MEMORY_MANAGER_INSTANCE( x )
#		define	INITIALISE_MEMORY_MANAGER( x )
#		define	SET_MEMORY_MANAGER( x )
#		define	SET_MEMORY_MANAGER_FROM( x, y )
#		define	RELEASE_MEMORY_MANAGER( x )
#	endif
#else
#	define	DECLARE_MEMORY_MANAGER_INSTANCE( x )
#	define	INITIALISE_MEMORY_MANAGER( x )
#	define	SET_MEMORY_MANAGER( x )
#	define	SET_MEMORY_MANAGER_FROM( x, y )
#	define	RELEASE_MEMORY_MANAGER( x )
#endif //____CASTOR_NO_MEMORY_DEBUG____

#if defined( _MSC_VER )
#	pragma warning( pop )
#endif

#endif