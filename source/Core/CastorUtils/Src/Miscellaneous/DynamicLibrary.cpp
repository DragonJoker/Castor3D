#include "DynamicLibrary.hpp"

namespace Castor
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
			Open( p_lib.m_pathLibrary );
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
		DoClose();
	}

	DynamicLibrary & DynamicLibrary::operator =( DynamicLibrary const & p_lib )
	{
		DoClose();

		if ( p_lib.m_pLibrary )
		{
			Open( p_lib.m_pathLibrary );
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

	bool DynamicLibrary::Open( xchar const * p_name )throw()
	{
		return Open( Path( p_name ) );
	}

	bool DynamicLibrary::Open( String const & p_name )throw()
	{
		return Open( Path( p_name ) );
	}
}
