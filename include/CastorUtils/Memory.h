/*
This source file is part of Castor3D (http://dragonjoker.co.cc

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
#ifndef ___Castor_Memory___
#define ___Castor_Memory___

//! Memory management class
/*!
Every class which derivates this one can be memory traced precisely.
This class overloads new and delete operators (and array ones too)
\author Sylvain DOREMUS
\version 0.6.1.0
\date 03/01/2011
*/
class MemoryTracedBase
{
public:
	static unsigned long long TotalAllocatedSize;
	static unsigned long long TotalAllocatedObjectCount;
	static unsigned long long TotalAllocatedArrayCount;
};

template <class T>
class MemoryTraced : public MemoryTracedBase
{
public:
	void * operator new( size_t p_size)
	{
		return Alloc( p_size);
	}

	void operator delete( void * p_pointer)
	{
		Dealloc( p_pointer);
	}

	void * operator new[]( size_t p_size)
	{
		return AllocArray( p_size);
	}

	void operator delete[]( void * p_pointer)
	{
		DeallocArray( p_pointer);
	}
	static void * Alloc( size_t p_size);
	static void Dealloc( void * p_pointer);
	static void * AllocArray( size_t p_size);
	static void DeallocArray( void * p_pointer);
};

#include "Mutex.h"
#include "AutoSingleton.h"
#include "Module_Utils.h"
#include "File.h"

#ifdef _WIN32
#	define LOG_LOCATION "c:\\memoryleaks.log"
#	define ALLOG_LOCATION "c:\\memoryalloc.log"
#	define DEALLOG_LOCATION "c:\\memorydealloc.log"
#else
#	define LOG_LOCATION "~/memoryleaks.log"
#	define ALLOG_LOCATION "~/memoryalloc.log"
#	define DEALLOG_LOCATION "~/memorydealloc.log"
#endif

//NO one touches my defines
#ifdef NEW
#	undef NEW
#endif

#ifdef NEW_ARRAY
#	undef NEW_ARRAY
#endif

#ifdef DELETE
#	undef DELETE
#endif

#ifdef DELETE_ARRAY
#	undef DELETE_ARRAY
#endif

#ifdef new
#	undef new
#endif

#ifdef delete
#	undef delete
#endif

#if ! CHECK_MEMORYLEAKS
#	define ____CASTOR_NO_MEMORY_DEBUG____
#endif

/**
 * Templated allocator, to have a better trace of allocation
 */
template <class T> void * TAlloc( size_t p_size);
/**
 * Templated de-allocator, to have a better trace of de-allocation
 */
template <class T> void TDealloc( void * p_pPointer);
/**
 * Templated array allocator, to have a better trace of allocation
 */
template <class T> void * TAllocArray( size_t p_size);
/**
 * Templated array de-allocator, to have a better trace of de-allocation
 */
template <class T> void TDeallocArray( void * p_pPointer);

#ifndef ____CASTOR_NO_MEMORY_DEBUG____

#	define MEMORY_MANAGER_START() MemoryManager::GetSingleton()

	namespace Castor
	{
		//! Memory block base class
		/*!
		Used by the memory manager to know what is allocated and deallocated
		\author Sylvain DOREMUS
		\version 0.6.1.0
		\date 03/01/2011
		*/
		class MemoryBlockBase
		{
		public:
			const char * file;
			const char * function;
			unsigned int line;
			size_t size;
			bool isArray;

		public:
			/**@name Constructors */
			//@{
			MemoryBlockBase();
			MemoryBlockBase( const char * p_file, const char * p_function, unsigned int p_line);
			MemoryBlockBase( size_t p_size, bool p_array);
			virtual ~MemoryBlockBase();
			//@}

			/**@name Overloads */
			//@{
			void * operator new( size_t p_size);
			void operator delete( void * p_pointer);
			//@}


		public:
			MemoryBlockBase & operator =( const MemoryBlockBase & p_other);
			virtual void Clear();
			void Assign( const MemoryBlockBase & p_block);
			virtual const char * GetBlockType()const { return "Unknown class"; }
		};

		template <class T>
		class MemoryBlockImpl : public MemoryBlockBase
		{
		protected:
			T * ptr;

		public:
			/**@name Constructors */
			//@{
			MemoryBlockImpl();
			MemoryBlockImpl( const char * p_file, const char * p_function, unsigned int p_line);
			MemoryBlockImpl( T * p_ptr, size_t p_size, bool p_array);
			//@}

			/**@name Overloads */
			//@{
			virtual const char * GetBlockType()const;
			virtual void Clear();
			void * operator new( size_t p_size)
			{
				void * l_pReturn = malloc( p_size);
				return l_pReturn;
			}
			void operator delete( void * p_pointer)
			{
				free( p_pointer);
			}
			//@}

		};

		class MemoryManager : public Castor::Theory::AutoSingleton <MemoryManager>
		{
		private:
			static unsigned int sm_initialised;

			typedef std::map <void *, MemoryBlockBase *>	MemoryBlockMap;
			typedef std::vector <MemoryBlockBase *>			MemoryBlockArray;

		public:
			Castor::MultiThreading::RecursiveMutex * m_pMutex;
			MemoryBlockBase m_lastBlock;
			MemoryBlockMap m_memoryMap;
			MemoryBlockArray m_failedNews;
			MemoryBlockArray m_failedDeletes;

			bool m_locked;
			size_t m_currentMemoryAllocated;
			size_t m_maximumMemoryAllocated;
			size_t m_totalObjectsAllocated;
			size_t m_totalArraysAllocated;
			unsigned long long m_totalMemoryAllocated;

		public:
			MemoryManager();
			~MemoryManager();

		public:
			template <typename T> void AddLocation( size_t p_size, T * p_pointer, bool p_typeArray);
			bool RemoveLocation( void * p_pointer, bool p_isArray);
			void FailedAlloc( size_t p_size, bool p_isArray);
			void MemoryLeaksReport( const Castor::String & p_filename = Castor::String());
			void RecordAllocation( size_t p_size);
			void RecordDeallocation( size_t p_size);
			MemoryManager & operator <<( const MemoryBlockBase & p_block);
			template <typename T> T * operator <<( T * p_ptr);

		private:
			void _finalReport();
			void _addMemoryBlock( void * p_pointer, MemoryBlockBase * p_pBlock);
			void * _parse( void * p_ptr);
			void _localise( void * p_ptr);

		public:
			static inline void Lock()		{ GetSingleton().m_locked = true; }
			static inline void Unlock()		{ GetSingleton().m_locked = false; }
			static inline bool IsLocked()	{ return GetSingleton().m_locked; }
			static inline bool Exists()		{ return (sm_initialised == 1); }
		};
	}

#	define new Castor::MemoryManager::GetSingleton() << Castor::MemoryBlockBase( __FILE__, __FUNCTION__, __LINE__) << new

#else 
#	define MEMORY_MANAGER_START
#endif //____CASTOR_NO_MEMORY_DEBUG____

#include "Memory.inl"

#endif	//___Castor_Memory___
