#include "DynamicLibrary.hpp"

namespace castor
{
	DynamicLibrary::DynamicLibrary()throw()
		: m_pLibrary( nullptr )
		, m_pathLibrary( )
	{
	}

	DynamicLibrary::DynamicLibrary( DynamicLibrary const & p_lib )throw()
		: m_pLibrary( nullptr )
		, m_pathLibrary( )
	{
		if ( p_lib.m_pLibrary )
		{
			open( p_lib.m_pathLibrary );
		}
	}

	DynamicLibrary::DynamicLibrary( DynamicLibrary && p_lib )throw()
		: m_pLibrary( std::move( p_lib.m_pLibrary ) )
		, m_pathLibrary( std::move( p_lib.m_pathLibrary ) )
	{
		p_lib.m_pLibrary = nullptr;
		p_lib.m_pathLibrary.clear();
	}

	DynamicLibrary::~DynamicLibrary()throw()
	{
		doClose();
	}

	DynamicLibrary & DynamicLibrary::operator =( DynamicLibrary const & p_lib )
	{
		doClose();

		if ( p_lib.m_pLibrary )
		{
			open( p_lib.m_pathLibrary );
		}

		return *this;
	}

	DynamicLibrary & DynamicLibrary::operator =( DynamicLibrary && p_lib )
	{
		if ( this != &p_lib )
		{
			m_pLibrary = std::move( p_lib.m_pLibrary );
			m_pathLibrary = std::move( p_lib.m_pathLibrary );
			p_lib.m_pLibrary = nullptr;
			p_lib.m_pathLibrary.clear();
		}

		return *this;
	}

	bool DynamicLibrary::open( xchar const * p_name )throw()
	{
		return open( Path( p_name ) );
	}

	bool DynamicLibrary::open( String const & p_name )throw()
	{
		return open( Path( p_name ) );
	}
}
