#include "PrecompiledHeader.h"

#include "Memory.h"

#ifndef ____CASTOR_NO_MEMORY_DEBUG____
#	undef new
#	undef delete

using namespace Castor;
using namespace Castor::Utils;

unsigned int MemoryManager :: sm_initialised = 0;

MemoryManager :: MemoryManager()
	:	m_locked					(false),
		m_currentMemoryAllocated	(0),
		m_maximumMemoryAllocated	(0),
		m_totalObjectsAllocated		(0),
		m_totalArraysAllocated		(0),
		m_totalMemoryAllocated		(0)
{
	sm_initialised = 1;
	FILE * l_file;
	fopen_s( & l_file, ALLOG_LOCATION, "w");
	fclose( l_file);

	fopen_s( & l_file, DEALLOG_LOCATION, "w");
	fclose( l_file);
}

MemoryManager :: ~MemoryManager()
{
	sm_initialised = 0;
	_finalReport();

	for (MemoryBlockMap::iterator l_it = m_memoryMap.begin() ; l_it != m_memoryMap.end() ; ++l_it)
	{
		delete l_it->second;
	}
}

MemoryManager & MemoryManager :: operator <<( const MemoryBlock & p_block)
{
	m_lastBlock = p_block;
	return * this;
}

void MemoryManager :: _localise( void * p_ptr)
{
	const MemoryBlockMap::iterator & ifind = m_memoryMap.find( p_ptr);

	if (ifind != m_memoryMap.end())
	{
		ifind->second->Assign( m_lastBlock);
	}

	m_lastBlock.Clear();
}

bool MemoryManager :: RemoveLocation( void * p_pointer, bool p_isArray)
{
	bool l_bReturn = false;
	const MemoryBlockMap::iterator & ifind = m_memoryMap.find( p_pointer);

	if (ifind != m_memoryMap.end())
	{
		MemoryBlock * p_block = ifind->second;
		RecordDeallocation( p_block->size);
		m_memoryMap.erase( ifind);
		delete p_block;
		l_bReturn = true;
	}
	else
	{
		m_failedDeletes.push_back( new MemoryBlock( "", "", 0));
	}

	return false;
}

void MemoryManager :: FailedAlloc( unsigned int p_size, bool p_isArray)
{
	m_failedNews.push_back( new MemoryBlock( "", "", 0));
}

void MemoryManager :: RecordAllocation( unsigned int p_size)
{
	m_currentMemoryAllocated += p_size;
	m_totalMemoryAllocated += p_size;

	if (m_currentMemoryAllocated > m_maximumMemoryAllocated)
	{
		m_maximumMemoryAllocated = m_currentMemoryAllocated;
	}
}

void MemoryManager :: RecordDeallocation( unsigned int p_size)
{
	m_currentMemoryAllocated -= p_size;
}

void MemoryManager :: _finalReport()
{
	FILE * l_file;
	fopen_s( & l_file, LOG_LOCATION, "w");

	if (l_file != NULL)
	{
		time_t rawtime;
		tm ti;

		time( & rawtime);
		localtime_s( & ti, & rawtime);

		ti.tm_year += 1900;
		ti.tm_mon ++;

		fprintf( l_file, "Memoryleak.log : %.2d/%.2d/%.4d @ %.2d:%.2d:%.2d\n", ti.tm_mday, ti.tm_mon, ti.tm_year, ti.tm_hour, ti.tm_min, ti.tm_sec);

		fprintf( l_file, "Total memory leaked : %d bytes\n", m_currentMemoryAllocated);
		fprintf( l_file, "Maximum memory usage : %d bytes\n", m_maximumMemoryAllocated);
		fprintf( l_file, "Total memory allocated : %lld bytes on %d objects and %d arrays\n", m_totalMemoryAllocated, m_totalObjectsAllocated, m_totalArraysAllocated);


		if ( ! m_memoryMap.empty())
		{
			MemoryBlockMap::iterator iter = m_memoryMap.begin();
			const MemoryBlockMap::iterator & iend = m_memoryMap.end();

			for ( ; iter != iend ; ++ iter)
			{
				MemoryBlock * l_block = iter->second;

				if (l_block->file == NULL)
				{
					fprintf( l_file, "Outside object leaked : %s, %d bytes\n", l_block->GetBlockType(), l_block->size);
				}
				else
				{
					fprintf( l_file, "%s leaked : %d bytes, created in %s(), line %d of file %s\n", (l_block->isArray ? "Array" : "Object"), l_block->size, l_block->function, l_block->line, l_block->file);
				}
			}
		}
		else
		{
			fprintf( l_file, "No memory leaks\n");
		}

		fclose( l_file);
	}
}

void MemoryManager :: MemoryLeaksReport( const String & p_filename)
{
	MemoryManager::Lock();
	std::ostream  * out;
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
	tm ti;

	time( & rawtime);
	localtime_s( & ti, & rawtime);
	
	* out << "Memoryleak.log : " << ti.tm_mday << "/" << (ti.tm_mon + 1) << "/" << (ti.tm_year + 1900) << " @ " << ti.tm_hour << ":" << ti.tm_min << ":" << ti.tm_sec << std::endl << std::endl;

	* out << "Total memory leaked : " << m_currentMemoryAllocated << " bytes" << std::endl;
	* out << "Maximum memory usage : " << m_maximumMemoryAllocated << " bytes" << std::endl;
	* out << "Total memory allocated : " << m_totalMemoryAllocated << " bytes on " << m_totalObjectsAllocated << " objects and " << m_totalArraysAllocated << " arrays" << std::endl;

	* out << std::endl;

	if ( ! m_memoryMap.empty())
	{
		MemoryBlockMap::iterator iter = m_memoryMap.begin();
		const MemoryBlockMap::iterator & iend = m_memoryMap.end();

		for ( ; iter != iend ; ++ iter)
		{
			MemoryBlock * l_block = iter->second;
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
			MemoryBlock * l_block = m_failedDeletes[i];
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
			MemoryBlock * l_block = m_failedNews[i];
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
#endif