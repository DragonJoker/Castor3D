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

template <class T>
class MemoryTraced
{
private:
	size_t m_uiSize;

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

	static void * Alloc( size_t p_size)
	{
		return TAlloc<T>( p_size);
	}
	static void Dealloc( void * p_pointer)
	{
		TDeallocArray<T>( p_pointer);
	}
	static void * AllocArray( size_t p_size)
	{
		return TAllocArray<T>( p_size);
	}
	static void DeallocArray( void * p_pointer)
	{
		TDeallocArray<T>( p_pointer);
	}
};

#include "Mutex.h"
#include "AutoSingleton.h"
#include "Module_Utils.h"
#include "Memory_Impl.h"
#include "File.h"

#ifndef __GNUG__
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

#ifndef ____CASTOR_NO_MEMORY_DEBUG____

#	define MEMORY_MANAGER_START() Castor::Utils::MemoryManager::GetSingleton()

	template <class T>
	void * TAlloc( size_t p_size)
	{
		T * l_ptr = (T *)malloc( p_size);
		Castor::Utils::File l_file( ALLOG_LOCATION, Castor::Utils::File::eAdd);
		l_file << "TAlloc" << " - " << l_ptr << " - " << p_size << " -\t" << typeid( T).name() << "\n";

		if (Castor::Utils::MemoryManager::Exists())
		{
			CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( Castor::Utils::MemoryManager::GetSingleton().m_mutex);

			if ( ! Castor::Utils::MemoryManager::IsLocked())
			{
				Castor::Utils::MemoryManager::Lock();

				if (l_ptr != NULL)
				{
					Castor::Utils::MemoryManager::GetSingleton().AddLocation( p_size, l_ptr, false);
				}
				else
				{
					Castor::Utils::MemoryManager::GetSingleton().FailedAlloc( p_size, false);
				}

				Castor::Utils::MemoryManager::Unlock();
			}
		}

		return l_ptr;
	}

	template <class T>
	void TDealloc( void * p_pointer)
	{
		Castor::Utils::File l_file( DEALLOG_LOCATION, Castor::Utils::File::eAdd);
		l_file << "TDealloc" << " - " << p_pointer << " - " << sizeof( T) << " -\t" << typeid( T).name() << "\n";

		if (MemoryManager::Exists())
		{
			CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( Castor::Utils::MemoryManager::GetSingleton().m_mutex);

			if ( ! Castor::Utils::MemoryManager::IsLocked())
			{
				Castor::Utils::MemoryManager::Lock();

				if (Castor::Utils::MemoryManager::GetSingleton().RemoveLocation( p_pointer, false))
				{
					free( p_pointer);
				}
				else
				{
				}

				Castor::Utils::MemoryManager::Unlock();
			}
			else
			{
				free( p_pointer);
			}
		}
		else
		{
			free( p_pointer);
		}
	}

	template <class T>
	void * TAllocArray( size_t p_size)
	{
		T * l_ptr = (T *)malloc( p_size);
		Castor::Utils::File l_file( ALLOG_LOCATION, Castor::Utils::File::eAdd);
		l_file << "TAllocArray" << " - " << l_ptr << " - " << p_size << " - " << typeid( T).name() << "\n";

		if (MemoryManager::Exists())
		{
			CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( MemoryManager::GetSingleton().m_mutex);

			if ( ! MemoryManager::IsLocked())
			{
				MemoryManager::Lock();

				if (l_ptr != NULL)
				{
					MemoryManager::GetSingleton().AddLocation( p_size, l_ptr, true);
				}
				else
				{
					MemoryManager::GetSingleton().FailedAlloc( p_size, true);
				}

				MemoryManager::Unlock();
			}
		}

		return l_ptr;
	}

	template <class T>
	void TDeallocArray( void * p_pointer)
	{
		Castor::Utils::File l_file( DEALLOG_LOCATION, Castor::Utils::File::eAdd);
		l_file << "TDeallocArray" << " - " << p_pointer << " - " << typeid( T).name() << "\n";

		if (MemoryManager::Exists())
		{
			CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( MemoryManager::GetSingleton().m_mutex);

			if ( ! MemoryManager::IsLocked())
			{
				MemoryManager::Lock();

				if (MemoryManager::GetSingleton().RemoveLocation( p_pointer, true))
				{
					free( p_pointer);
				}
				else
				{
				}

				MemoryManager::Unlock();
			}
			else
			{
				free( p_pointer);
			}
		}
		else
		{
			free( p_pointer);
		}
	}
/*
	void * operator new( size_t p_size);
	void operator delete( void * p_pointer);
/**/
/*
	void * operator new[]( size_t p_size);
	void operator delete[]( void * p_pointer);
/**/
	namespace Castor
	{ namespace Utils
	{
		class MemoryBlock
		{
		public:
			const char * file;
			const char * function;
			unsigned int line;
			size_t size;
			bool isArray;

		public:
			MemoryBlock()
				:	file		(NULL),
					function	(NULL),
					line		(0),
					size		(0),
					isArray		(false)
			{}
			MemoryBlock(	const char * p_file, const char * p_function,
				unsigned int p_line)
				:	file		(p_file),
					function	(p_function),
					line		(p_line),
					size		(0),
					isArray		(false)
			{}
			MemoryBlock( size_t p_size, bool p_array)
				:	file		(NULL),
					function	(NULL),
					line		(0),
					size		(p_size),
					isArray		(p_array)
			{}
			virtual ~MemoryBlock(){}
			void * operator new( size_t p_size)
			{
				void * l_pReturn = malloc( p_size);
				return l_pReturn;
			}
			void operator delete( void * p_pointer)
			{
				free( p_pointer);
			}

		public:
			MemoryBlock & operator =( const MemoryBlock & p_other)
			{
				Assign( p_other);
				return * this;
			}

			virtual void Clear()
			{
				file = NULL;
				function = NULL;
				line = 0;
				size = 0;
				isArray = false;
			}

			void Assign( const MemoryBlock & p_block)
			{
				file = p_block.file;
				function = p_block.function;
				line = p_block.line;
			}

			virtual const char * GetBlockType()const { return "Unknown class"; }
		};

		template <class T>
		class MemoryBlockImpl : public MemoryBlock
		{
		protected:
			T * ptr;

		public:
			MemoryBlockImpl()
				:	ptr( NULL)
			{}
			MemoryBlockImpl( const char * p_file, const char * p_function, unsigned int p_line)
				:	MemoryBlock( p_file, p_function, p_line)
					ptr( NULL)
			{}
			MemoryBlockImpl( T * p_ptr, size_t p_size, bool p_array)
				:	MemoryBlock( p_size, p_array),
					ptr( p_ptr)
			{}
			virtual const char * GetBlockType()const
			{
				return typeid( T).name();
			}
			virtual void Clear()
			{
				MemoryBlock::Clear();
				ptr = NULL;
			}
			void * operator new( size_t p_size)
			{
				void * l_pReturn = malloc( p_size);
				return l_pReturn;
			}
			void operator delete( void * p_pointer)
			{
				free( p_pointer);
			}
		};

		class MemoryManager : public Castor::Theory::AutoSingleton <MemoryManager>
		{
		private:
			static unsigned int sm_initialised;

			typedef std::map <void *, MemoryBlock *>	MemoryBlockMap;
			typedef std::vector <MemoryBlock *>			MemoryBlockArray;

		public:
			Castor::MultiThreading::RecursiveMutex m_mutex;
			MemoryBlock m_lastBlock;
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
			template <typename T>
			void AddLocation( size_t p_size, T * p_pointer, bool p_typeArray)
			{
				RecordAllocation( p_size);

				if (p_typeArray)
				{
					m_totalArraysAllocated ++;
				}
				else
				{
					m_totalObjectsAllocated ++;
				}

				m_memoryMap.insert( MemoryBlockMap::value_type( p_pointer, new MemoryBlockImpl<T>( p_pointer, p_size, p_typeArray)));
			}
			bool RemoveLocation( void * p_pointer, bool p_isArray);
			void FailedAlloc( size_t p_size, bool p_isArray);
			void MemoryLeaksReport( const String & p_filename = String());
			void RecordAllocation( size_t p_size);
			void RecordDeallocation( size_t p_size);

			MemoryManager & operator <<( const MemoryBlock & p_block);

			template <typename T>
			T * operator <<( T * p_ptr)
			{
				_localise( p_ptr);
				return p_ptr;
			}

		private:
			void _finalReport();
			void _localise( void * p_ptr);

		public:
			static inline void Lock()		{ GetSingleton().m_locked = true; }
			static inline void Unlock()		{ GetSingleton().m_locked = false; }
			static inline bool IsLocked()	{ return GetSingleton().m_locked; }
			static inline bool Exists()		{ return (sm_initialised == 1); }
		};
	}
	}

#	define new Castor::Utils::MemoryManager::GetSingleton() << Castor::Utils::MemoryBlock( __FILE__, __FUNCTION__, __LINE__) << new

#else 

#	define MEMORY_MANAGER_START

	template <class T>
	void * TAlloc( size_t p_size)
	{
		void * l_pointer = malloc( p_size);

		if (l_pointer == NULL)
		{
			throw std::bad_alloc();
		}

		return l_pointer;
	}

	template <class T>
	void TDealloc( void * p_pointer)
	{
		free( p_pointer);
	}

	template <class T>
	void * TAllocArray( size_t p_size)
	{
		void * l_pointer = malloc( p_size);

		if (l_pointer == NULL)
		{
			throw std::bad_alloc();
		}

		return l_pointer;
	}

	template <class T>
	void TDeallocArray( void * p_pointer)
	{
		free( p_pointer);
	}

#endif //____CASTOR_NO_MEMORY_DEBUG____

#endif	//___Castor_Memory___
