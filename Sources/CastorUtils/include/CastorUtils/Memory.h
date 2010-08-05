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
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___MEMORY_MANAGER_H___
#define ___MEMORY_MANAGER_H___

#include "Module_Utils.h"

#ifndef __GNUG__
#	define LOG_LOCATION "c:\\memoryleaks.log"
#else
#	define LOG_LOCATION "~/memoryleaks.log"
#endif

#include "Mutex.h"
#include "AutoSingleton.h"

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
/*
#define NEW(X) new X
#define NEW_ARRAY( X, Y) new X[Y]
#define DELETE( X) delete X
#define DELETE_ARRAY( X) delete [] X
*/
#ifndef ____GENLIB_NO_MEMORY_DEBUG____

#	define MEMORY_MANAGER_START() General::Utils::MemoryManager::GetSingleton()

/*
#	if defined(_MT) || defined(__MT__)
#		pragma comment( lib, "GeneralMemory_mt.lib")
#		pragma comment( lib, "libcpmt.lib")
#	else
#		pragma comment( lib, "GeneralMemory_md.lib")
#		pragma comment( lib, "msvcprt.lib")
#	endif
*/

	#include "Memory_Impl.h"

	void operator delete[]( void * p_pointer);
	void operator delete( void * p_pointer);
	void * operator new[]( size_t p_size);
	void * operator new( size_t p_size);

	namespace General
	{ namespace Utils
	{
		class MemoryBlock
		{
		public:
			const char * file;
			const char * function;
			void * ptr;
			unsigned int line;
			size_t size;
			bool isArray;

		public:
			MemoryBlock()
				:	file		(NULL),
					function	(NULL),
					ptr			(NULL),
					line		(0),
					size		(0),
					isArray		(false)
			{}
			MemoryBlock(	const char * p_file, const char * p_function,
							unsigned int p_line)
				:	file		(p_file),
					function	(p_function),
					ptr			(NULL),
					line		(p_line),
					size		(0),
					isArray		(false)
			{}
			MemoryBlock( void * p_ptr, size_t p_size, bool p_array)
				:	file		(NULL),
					function	(NULL),
					ptr			(p_ptr),
					line		(0),
					size		(p_size),
					isArray		(p_array)
			{}
			~MemoryBlock(){}

		public:
			MemoryBlock & operator =( const MemoryBlock & p_other)
			{
				file = p_other.file;
				function = p_other.function;
				line = p_other.line;
				return * this;
			}

			void Clear()
			{
				file = NULL;
				function = NULL;
				ptr = NULL;
				line = 0;
				size = 0;
				isArray = false;
			}
		};

		class MemoryManager : public General::Theory::AutoSingleton <MemoryManager>
		{
		private:
			static unsigned int sm_initialised;

			typedef std::map	<void *, MemoryBlock>	MemoryBlockMap;
			typedef std::vector	<MemoryBlock>			MemoryBlockArray;
			
		public:
			General::MultiThreading::RecursiveMutex m_mutex;
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
			void AddLocation( size_t p_size, void * p_pointer, bool p_typeArray);
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
	
#	define new General::Utils::MemoryManager::GetSingleton() << MemoryBlock( __FILE__, __FUNCTION__, __LINE__) << new

#else 

#	define MEMORY_MANAGER_START

#endif //____GENLIB_NO_MEMORY_DEBUG____

#endif	//___MEMORY_MANAGER_H___
