#include "CastorUtils/PrecompiledHeader.hpp"

#include "CastorUtils/Memory.hpp"

#ifndef ____CASTOR_NO_MEMORY_DEBUG____

#	undef new
#	undef delete

using namespace Castor;
using namespace Castor::Utils;

//*************************************************************************************************

unsigned long long	MemoryTracedBase::TotalAllocatedSize = 0;
unsigned long long	MemoryTracedBase::TotalAllocatedObjectCount = 0;
unsigned long long	MemoryTracedBase::TotalAllocatedArrayCount = 0;

//*************************************************************************************************

MemoryBlockBase :: MemoryBlockBase()
	:	file		(nullptr)
	,	function	(nullptr)
	,	line		(0)
	,	size		(0)
	,	isArray		(false)
{
}

MemoryBlockBase :: MemoryBlockBase(	char const * p_file, char const * p_function, unsigned int p_line)
	:	file		(p_file)
	,	function	(p_function)
	,	line		(p_line)
	,	size		(0)
	,	isArray		(false)
{
}

MemoryBlockBase :: MemoryBlockBase( size_t p_size, bool p_array)
	:	file		(nullptr)
	,	function	(nullptr)
	,	line		(0)
	,	size		(p_size)
	,	isArray		(p_array)
{
}

MemoryBlockBase :: ~MemoryBlockBase()
{
}

void * MemoryBlockBase :: operator new( size_t p_size)
{
	void * l_pReturn = malloc( p_size);
	return l_pReturn;
}

void MemoryBlockBase :: operator delete( void * p_pointer)
{
	free( p_pointer);
}

MemoryBlockBase & MemoryBlockBase :: operator =( MemoryBlockBase const & p_other)
{
	Assign( p_other);
	return * this;
}

void MemoryBlockBase :: Clear()
{
	file = nullptr;
	function = nullptr;
	line = 0;
	size = 0;
	isArray = false;
}

void MemoryBlockBase :: Assign( MemoryBlockBase const & p_block)
{
	file = p_block.file;
	function = p_block.function;
	line = p_block.line;
}

//*************************************************************************************************

#ifdef _MSC_VER
#	define _CRTDBG_MAP_ALLOC
#	include <stdlib.h>
#	include <crtdbg.h>
#endif

#include <fstream>

unsigned int MemoryManager :: sm_initialised = 0;

MemoryManager :: MemoryManager()
	:	m_locked					( false),
		m_currentMemoryAllocated	( 0),
		m_maximumMemoryAllocated	( 0),
		m_totalObjectsAllocated		( 0),
		m_totalArraysAllocated		( 0),
		m_pMutex					( new Castor::MultiThreading::RecursiveMutex),
		m_totalMemoryAllocated		( 0)
{
	File::FileDelete( LOG_LOCATION);
	File l_file( LOG_LOCATION, File::eOPEN_MODE_WRITE);
	sm_initialised = 1;
}

MemoryManager :: ~MemoryManager()
{
	m_pMutex->Lock();
	sm_initialised = 0;
	_finalReport();

	for (MemoryBlockMap::iterator l_it = m_memoryMap.begin() ; l_it != m_memoryMap.end() ; ++l_it)
	{
		delete l_it->second;
	}

	m_memoryMap.clear();
	m_pMutex->Unlock();
	delete m_pMutex;
/*
#ifdef _MSC_VER
	_CrtDumpMemoryLeaks();
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif
*/
}

MemoryManager & MemoryManager :: operator <<( MemoryBlockBase const & p_block)
{
	CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( * m_pMutex);
	m_lastBlock = p_block;
	return * this;
}

void MemoryManager :: _addMemoryBlock( void * p_pointer, MemoryBlockBase * p_pBlock)
{
	CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( * m_pMutex);

	if (p_pBlock->isArray)
	{
		m_totalArraysAllocated ++;
	}
	else
	{
		m_totalObjectsAllocated ++;
	}

	m_memoryMap.insert( MemoryBlockMap::value_type( p_pointer, p_pBlock));
}

void * MemoryManager :: _parse( void * p_ptr)
{
	CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( * m_pMutex);
	_localise( p_ptr);
	return p_ptr;
}

void MemoryManager :: _localise( void * p_ptr)
{
	CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( * m_pMutex);
	MemoryBlockMap::iterator ifind = m_memoryMap.find( p_ptr);

	if (ifind != m_memoryMap.end())
	{
		ifind->second->Assign( m_lastBlock);
	}

	m_lastBlock.Clear();
}

bool MemoryManager :: RemoveLocation( void * p_pointer, bool p_isArray)
{
	CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( * m_pMutex);
	bool l_bReturn = false;
	MemoryBlockMap::iterator ifind = m_memoryMap.find( p_pointer);

	if (ifind != m_memoryMap.end())
	{
		MemoryBlockBase * p_block = ifind->second;
		RecordDeallocation( p_block->size);
		m_memoryMap.erase( ifind);
		delete p_block;
		l_bReturn = true;
	}
	else
	{
		m_failedDeletes.push_back( new MemoryBlockBase( "", "", 0));
	}

	return false;
}

void MemoryManager :: FailedAlloc( size_t p_size, bool p_isArray)
{
	CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( * m_pMutex);
	m_failedNews.push_back( new MemoryBlockBase( "", "", 0));
}

void MemoryManager :: RecordAllocation( size_t p_size)
{
	CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( * m_pMutex);
	m_currentMemoryAllocated += p_size;
	m_totalMemoryAllocated += p_size;

	if (m_currentMemoryAllocated > m_maximumMemoryAllocated)
	{
		m_maximumMemoryAllocated = m_currentMemoryAllocated;
	}
}

void MemoryManager :: RecordDeallocation( size_t p_size)
{
	CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( * m_pMutex);
	m_currentMemoryAllocated -= p_size;
}

void MemoryManager :: _finalReport()
{
	CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( * m_pMutex);
	std::fstream l_file( LOG_LOCATION, std::ios_base::in | std::ios_base::app);

	if (l_file)
	{
		time_t rawtime;
		struct tm * ti = nullptr;

		time( & rawtime);
		Localtime( ti, & rawtime);

		ti->tm_year += 1900;
		ti->tm_mon ++;

		l_file.precision( 2);
		l_file << "Memoryleak.log : " << ti->tm_mday << "/" << ti->tm_mon << "/" << ti->tm_year << "@" << ti->tm_hour << ":" << ti->tm_min << ":" << ti->tm_sec << std::endl;

		l_file << "Total memory leaked : " << m_currentMemoryAllocated << " bytes" << std::endl;
		l_file << "Maximum memory usage : " << m_maximumMemoryAllocated << " bytes" << std::endl;
		l_file << "Total memory allocated : " << m_totalMemoryAllocated << " bytes on " << m_totalObjectsAllocated << " objects and " << m_totalArraysAllocated << " arrays" << std::endl;
		l_file << "Total memory allocated through MemoryTraced : " << MemoryTracedBase::TotalAllocatedSize << " bytes on " << MemoryTracedBase::TotalAllocatedObjectCount << " objects and " << MemoryTracedBase::TotalAllocatedArrayCount << " arrays" << std::endl;


		if ( ! m_memoryMap.empty())
		{
			MemoryBlockMap::iterator iter = m_memoryMap.begin();
			MemoryBlockMap::const_iterator iend = m_memoryMap.end();

			for ( ; iter != iend ; ++ iter)
			{
				MemoryBlockBase * l_block = iter->second;

				if (l_block->file == NULL)
				{
					l_file << "Outside object leaked : " << l_block->GetBlockType() << ", " << l_block->size << " bytes" << std::endl;
				}
				else
				{
					l_file << (l_block->isArray ? "Array" : "Object") << " leaked : " << l_block->size << " bytes, created in " << l_block->function << "(), line " << l_block->line << " of file " << l_block->file << std::endl;
				}
			}
		}
		else
		{
			l_file << "No memory leaks ;) You're too strong :D !" << std::endl;
		}
	}
}

void MemoryManager :: MemoryLeaksReport( String const & p_filename)
{
	CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( * m_pMutex);
	MemoryManager::Lock();
	std::ostream * out;
	std::ofstream * fout;

	if ( ! p_filename.empty())
	{
		fout = new std::ofstream;

		if (fout == NULL)
		{
			return;
		}

		fout->open( p_filename.char_str());

		if (fout->bad() || fout->eof())
		{
			return;
		}

		out = fout;
	}
	else
	{
		out = & std::cerr;
	}

	time_t rawtime;
	struct tm * ti = nullptr;

	time( & rawtime);
	Localtime( ti, & rawtime);

	* out << "Memoryleak.log : " << ti->tm_mday << "/" << (ti->tm_mon + 1) << "/" << (ti->tm_year + 1900) << " @ " << ti->tm_hour << ":" << ti->tm_min << ":" << ti->tm_sec << std::endl << std::endl;

	* out << "Total memory leaked : " << m_currentMemoryAllocated << " bytes" << std::endl;
	* out << "Maximum memory usage : " << m_maximumMemoryAllocated << " bytes" << std::endl;
	* out << "Total memory allocated : " << m_totalMemoryAllocated << " bytes on " << m_totalObjectsAllocated << " objects and " << m_totalArraysAllocated << " arrays" << std::endl;

	* out << std::endl;

	if ( ! m_memoryMap.empty())
	{
		MemoryBlockMap::iterator iter = m_memoryMap.begin();
		MemoryBlockMap::const_iterator iend = m_memoryMap.end();

		for ( ; iter != iend ; ++ iter)
		{
			MemoryBlockBase * l_block = iter->second;
			* out << (l_block->isArray ? "Array" : "Object") << " leaked : " << l_block->size << " bytes, created in " << l_block->function << "() , line " << l_block->line << " of file " << l_block->file << std::endl;
		}
	}
	else
	{
		* out << "No memory leaked" << std::endl;
	}

	* out << std::endl;

	if ( ! m_failedDeletes.empty())
	{
		size_t imax = m_failedDeletes.size();

		for (size_t i = 0 ; i < imax ; i++)
		{
			MemoryBlockBase * l_block = m_failedDeletes[i];
			* out << "Deletion of an invalid " << (l_block->isArray ? "array" : "object") << " pointer @ " << l_block->function << "() , line " << l_block->line << " of file " << l_block->file << std::endl;
		}
	}
	else
	{
		* out << "No invalid pointers" << std::endl;
	}

	* out << std::endl;

	if ( ! m_failedNews.empty())
	{
		size_t imax = m_failedNews.size();

		for (size_t i = 0 ; i < imax ; i++)
		{
			MemoryBlockBase * l_block = m_failedNews[i];
			* out << "Failed allocation of an " << (l_block->isArray ? "array" : "object") << " pointer @ " << l_block->function << "() , line " << l_block->line << " of file " << l_block->file << std::endl;
		}
	}
	else
	{
		* out << "No failed allocation" << std::endl;
	}

	if ( ! p_filename.empty())
	{
		fout->close();
		delete fout;
	}

	MemoryManager::Unlock();
}

//*************************************************************************************************
#endif
