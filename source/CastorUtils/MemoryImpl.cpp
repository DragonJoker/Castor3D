#include "CastorUtils/PrecompiledHeader.hpp"

#include "CastorUtils/MemoryImpl.hpp"

using namespace Castor;

#pragma warning( push )
#pragma warning( disable:4290 )

//*************************************************************************************************

void * Castor::Allocate( std::size_t p_size )
{
	p_size = std::max< std::size_t >( p_size, 1 );
	void * l_pMemory = std::malloc( p_size );

	while( !l_pMemory )
	{
		std::new_handler l_newHandler = std::set_new_handler( 0 );
		std::set_new_handler( l_newHandler );

		if( !l_newHandler )
		{
			throw std::bad_alloc();
		}

		std::new_handler();
		l_pMemory = std::malloc( p_size );
	}

	return l_pMemory;
}

void * Castor::Allocate( std::size_t p_size, std::nothrow_t const & )throw()
{
	p_size = std::max< std::size_t >( p_size, 1 );
	return std::malloc( p_size );
}

void Castor::Deallocate( void * p_pMemory )throw()
{
	std::free( p_pMemory );
}

//*************************************************************************************************

#if CHECK_MEMORYLEAKS && (!defined( VLD_AVAILABLE ) || !USE_VLD)
#	undef new
#	undef delete
#	undef delete_array

MemoryBlock :: MemoryBlock()
	:	file		( NULL	)
	,	function	( NULL 	)
	,	line		( 0 	)
	,	size		( 0 	)
	,	isArray		( false	)
{
}

MemoryBlock :: MemoryBlock( MemoryBlock const & p_copy )
	:	file		( p_copy.file		)
	,	function	( p_copy.function	)
	,	line		( p_copy.line		)
	,	size		( p_copy.size		)
	,	isArray		( p_copy.isArray	)
{
}

MemoryBlock :: MemoryBlock( MemoryBlock && p_copy )
	:	file		( std::move( p_copy.file		) )
	,	function	( std::move( p_copy.function	) )
	,	line		( std::move( p_copy.line		) )
	,	size		( std::move( p_copy.size		) )
	,	isArray		( std::move( p_copy.isArray		) )
{
	p_copy.file		= NULL	;
	p_copy.function	= NULL 	;
	p_copy.line		= 0 	;
	p_copy.size		= 0 	;
	p_copy.isArray	= false	;
}

MemoryBlock & MemoryBlock :: operator =( MemoryBlock const & p_copy )
{
	file		= p_copy.file		;
	function	= p_copy.function	;
	line		= p_copy.line		;
	size		= p_copy.size		;
	isArray		= p_copy.isArray	;

	return *this;
}

MemoryBlock & MemoryBlock :: operator =( MemoryBlock && p_copy )
{
	if( this != &p_copy )
	{
		file		= std::move( p_copy.file		);
		function	= std::move( p_copy.function	);
		line		= std::move( p_copy.line		);
		size		= std::move( p_copy.size		);
		isArray		= std::move( p_copy.isArray		);
		p_copy.file		= NULL	;
		p_copy.function	= NULL 	;
		p_copy.line		= 0 	;
		p_copy.size		= 0 	;
		p_copy.isArray	= false	;
	}

	return *this;
}

MemoryBlock :: MemoryBlock( char const * p_pszFile, char const * p_pszFunction, uint32_t p_uiLine, std::size_t p_size, bool p_array )
	:	file		( p_pszFile		)
	,	function	( p_pszFunction	)
	,	line		( p_uiLine 		)
	,	size		( p_size 		)
	,	isArray		( p_array		)
{
}

MemoryBlock :: ~MemoryBlock()
{
}

void * MemoryBlock :: operator new( std::size_t p_size )
{
	void * l_pReturn = Allocate( p_size );
	return l_pReturn;
}

void MemoryBlock :: operator delete( void * p_pointer )
{
	Deallocate( p_pointer );
}

//*************************************************************************************************

CASTOR_DECLARE_UNIQUE_INSTANCE( MemoryManager );

MemoryManager :: MemoryManager()
	:	m_locked					( false )
	,	m_currentMemoryAllocated	( 0		)
	,	m_maximumMemoryAllocated	( 0		)
	,	m_totalObjectsAllocated		( 0		)
	,	m_totalArraysAllocated		( 0		)
	,	m_totalMemoryAllocated		( 0		)
	,	m_initialised				( false	)
{
	CASTOR_INIT_UNIQUE_INSTANCE();
	m_initialised = true;
}

MemoryManager :: ~MemoryManager()
{
	m_mutex.lock();
	m_initialised = false;
	DoFinalReport();
	m_mapAllocated.clear();
	m_mutex.unlock();
	CASTOR_CLEANUP_UNIQUE_INSTANCE();
}

void MemoryManager :: AddSuccessfulAllocation( void * p_ptr, char const * p_pszFile, char const * p_pszFunction, uint32_t p_uiLine, std::size_t p_size, bool p_array )
{
	GetSingleton().m_lastAllocatedBlock = MemoryBlock( p_pszFile, p_pszFunction, p_uiLine, p_size, p_array );
	GetSingleton().DoAddSuccessfulAllocation( p_ptr );
}

void MemoryManager :: AddFailedAllocation( std::size_t p_size, bool p_isArray )
{
	GetSingleton().DoAddFailedAllocation( p_size, p_isArray );
}

bool MemoryManager :: RemoveAllocation( void * p_pointer )
{
	return GetSingleton().DoRemoveAllocation( p_pointer );
}

MemoryManager & MemoryManager :: GetSingleton()
{
	return *Unique< MemoryManager >::m_pInstance;
}

MemoryManager * MemoryManager :: GetSingletonPtr()
{
	return Unique< MemoryManager >::m_pInstance;
}

void MemoryManager :: SetSingleton( MemoryManager * p_pInstance )
{
	Unique< MemoryManager >::SetInstance( p_pInstance );
}

void MemoryManager :: Lock()
{
	GetSingleton().m_locked = true;
}

void MemoryManager :: Unlock()
{
	GetSingleton().m_locked = false;
}

bool MemoryManager :: IsLocked()
{
	return GetSingleton().m_locked;
}

bool MemoryManager :: Exists()
{
	return GetSingletonPtr() && GetSingleton().m_initialised;
}

void * MemoryManager :: operator new( std::size_t p_size )
{
	void * l_pReturn = Allocate( p_size );
	return l_pReturn;
}

void MemoryManager :: operator delete( void * p_pointer )
{
	Deallocate( p_pointer );
}

MemoryManager & MemoryManager :: operator <<( MemoryBlock & p_block )
{
	if( MemoryManager::Exists() )
	{
		m_lastAllocatedBlock = p_block;
	}

	return *this;
}

void * MemoryManager :: operator <<( void * p_pMemory )
{
	if( MemoryManager::Exists() )
	{
		DoAddSuccessfulAllocation( p_pMemory );
	}

	return p_pMemory;
}

void MemoryManager :: operator >>( void * p_pMemory )
{
	if( MemoryManager::Exists() )
	{
		DoRemoveAllocation( p_pMemory );
	}

	delete p_pMemory;
}

void MemoryManager :: operator -( void * p_pMemory )
{
	if( MemoryManager::Exists() )
	{
		DoRemoveAllocation( p_pMemory );
	}

	delete [] p_pMemory;
}

void MemoryManager :: DoAddSuccessfulAllocation( void * p_ptr )
{
	std::unique_lock< std::recursive_mutex > l_lock( m_mutex );
	m_totalMemoryAllocated += m_lastAllocatedBlock.size;

	if( m_lastAllocatedBlock.isArray )
	{
		m_totalObjectsAllocated++;
	}
	else
	{
		m_totalArraysAllocated++;
	}

	m_currentMemoryAllocated += m_lastAllocatedBlock.size;
	std::max( m_maximumMemoryAllocated, m_currentMemoryAllocated );
	m_mapAllocated.insert( std::make_pair( p_ptr, m_lastAllocatedBlock ) );
}

void MemoryManager :: DoAddFailedAllocation( std::size_t p_size, bool p_isArray )
{
	std::unique_lock< std::recursive_mutex > l_lock( m_mutex );
	m_failedNews.push_back( MemoryBlock( NULL, NULL, 0, p_size, p_isArray ) );
}

bool MemoryManager :: DoRemoveAllocation( void * p_pointer )
{
	std::unique_lock< std::recursive_mutex > l_lock( m_mutex );
	bool l_bReturn = false;
	MemoryBlockMapIt ifind = m_mapAllocated.find( p_pointer );

	if( ifind != m_mapAllocated.end() )
	{
		m_currentMemoryAllocated -= ifind->second.size;
		m_mapAllocated.erase( ifind );
		l_bReturn = true;
	}
	else
	{
		m_failedDeletes.push_back( MemoryBlock( NULL, NULL, 0, 0, false ) );
	}

	return l_bReturn;
}

void MemoryManager :: DoFinalReport()
{
	Lock();
	std::unique_lock< std::recursive_mutex > l_lock( m_mutex );
#if defined( _WIN32 )
	std::fstream l_file( ".\\MemoryLeaks.log", std::ios_base::in | std::ios_base::app );
#else
	std::fstream l_file( "./MemoryLeaks.log", std::ios_base::in | std::ios_base::app );
#endif

	if( l_file )
	{
		time_t rawtime;
		std::tm * l_pTm = NULL;

		time( &rawtime );
		Castor::Localtime( l_pTm, &rawtime );

		l_pTm->tm_year += 1900;
		l_pTm->tm_mon ++;

		l_file.precision( 2 );
		l_file << "Memoryleak.log : " << l_pTm->tm_mday << "/" << l_pTm->tm_mon << "/" << l_pTm->tm_year << "@" << l_pTm->tm_hour << ":" << l_pTm->tm_min << ":" << l_pTm->tm_sec << std::endl;

		l_file << "*	Total memory leaked    : " << m_currentMemoryAllocated << " bytes" << std::endl;
		l_file << "*	Maximum memory usage   : " << m_maximumMemoryAllocated << " bytes" << std::endl;
		l_file << "*	Total memory allocated : " << m_totalMemoryAllocated << " bytes on " << m_totalObjectsAllocated << " objects and " << m_totalArraysAllocated << " arrays" << std::endl;

		if( ! m_mapAllocated.empty() )
		{
			for( MemoryBlockMapIt it = m_mapAllocated.begin(); it != m_mapAllocated.end(); ++it )
			{
				MemoryBlock & l_block = it->second;

				if( !l_block.file )
				{
					l_file << "*	Outside object leaked : " << l_block.GetBlockType() << ", " << uint64_t( l_block.size ) << " bytes" << std::endl;
				}
				else
				{
					l_file << (l_block.isArray ? "*	Array" : "*	Object") << " leaked : " << uint64_t( l_block.size ) << " bytes, created in " << l_block.function << "(), line " << l_block.line << " of file " << l_block.file << std::endl;
				}
			}
		}
		else
		{
			l_file << "*	No memory leaks detected ;) You're too strong :D !" << std::endl;
		}
	}

	Unlock();
}

//*************************************************************************************************

void * Castor::TracedAllocate( std::size_t p_size, char const * p_szFile, char const * p_szFunction, uint32_t p_uiLine, bool p_array )
{
	void * l_pReturn = NULL;

	try
	{
		l_pReturn = Castor::Allocate( p_size );

		if( MemoryManager::Exists() && !MemoryManager::IsLocked() )
		{
			MemoryManager::Lock();
			MemoryManager::AddSuccessfulAllocation( l_pReturn, p_szFile, p_szFunction, p_uiLine, p_size, p_array );
			MemoryManager::Unlock();
		}
	}
	catch( std::bad_alloc & p_exc )
	{
		if( MemoryManager::Exists() && !MemoryManager::IsLocked() )
		{
			MemoryManager::Lock();
			MemoryManager::AddFailedAllocation( p_size, p_array );
			MemoryManager::Unlock();
		}

		throw p_exc;
	}

	return l_pReturn;
}

void * Castor::TracedAllocate( std::size_t p_size, char const * p_szFile, char const * p_szFunction, uint32_t p_uiLine, bool p_array, std::nothrow_t const & )throw()
{
	void * l_pReturn = Castor::Allocate( p_size );

	if( l_pReturn )
	{
		if( MemoryManager::Exists() && !MemoryManager::IsLocked() )
		{
			MemoryManager::Lock();
			MemoryManager::AddSuccessfulAllocation( l_pReturn, p_szFile, p_szFunction, p_uiLine, p_size, p_array );
			MemoryManager::Unlock();
		}
	}
	else if( MemoryManager::Exists() && !MemoryManager::IsLocked() )
	{
		MemoryManager::Lock();
		MemoryManager::AddFailedAllocation( p_size, p_array );
		MemoryManager::Unlock();
	}

	return l_pReturn;
}

void Castor::TracedDeallocate( void * p_pMemory, char const * CU_PARAM_UNUSED( p_szFile ), char const * CU_PARAM_UNUSED( p_szFunction ), uint32_t CU_PARAM_UNUSED( p_uiLine ), bool CU_PARAM_UNUSED( p_array ) )
{
	if( p_pMemory )
	{
		if( MemoryManager::Exists() && !MemoryManager::IsLocked() )
		{
			MemoryManager::Lock();
			MemoryManager::RemoveAllocation( p_pMemory );
			MemoryManager::Unlock();
		}

		Castor::Deallocate( p_pMemory );
	}
}

void Castor::TracedDeallocate( void * p_pMemory, char const * CU_PARAM_UNUSED( p_szFile ), char const * CU_PARAM_UNUSED( p_szFunction ), uint32_t CU_PARAM_UNUSED( p_uiLine ), bool CU_PARAM_UNUSED( p_array ), std::nothrow_t const & )throw()
{
	if( p_pMemory )
	{
		if( MemoryManager::Exists() && !MemoryManager::IsLocked() )
		{
			MemoryManager::Lock();
			MemoryManager::RemoveAllocation( p_pMemory );
			MemoryManager::Unlock();
		}

		Castor::Deallocate( p_pMemory );
	}
}
#endif
//*************************************************************************************************

#pragma warning( pop )
