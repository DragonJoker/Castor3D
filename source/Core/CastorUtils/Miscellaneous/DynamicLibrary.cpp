#include "CastorUtils/Miscellaneous/DynamicLibrary.hpp"

CU_ImplementSmartPtr( castor, DynamicLibrary )

namespace castor
{
	DynamicLibrary::DynamicLibrary( Path const & pathFile )noexcept
		: m_pathLibrary( pathFile )
	{
		doOpen();
	}

	DynamicLibrary::DynamicLibrary( String const & pathFile )noexcept
		: DynamicLibrary( Path{ pathFile } )
	{
		doOpen();
	}

	DynamicLibrary::DynamicLibrary( DynamicLibrary const & lib )noexcept
		: m_pathLibrary( lib.m_pathLibrary )
	{
		if ( lib.m_library )
		{
			doOpen();
		}
	}

	DynamicLibrary::DynamicLibrary( DynamicLibrary && lib )noexcept
		: m_library( castor::move( lib.m_library ) )
		, m_pathLibrary( castor::move( lib.m_pathLibrary ) )
	{
		lib.m_library = nullptr;
		lib.m_pathLibrary.clear();
	}

	DynamicLibrary::~DynamicLibrary()noexcept
	{
		doClose();
	}

	DynamicLibrary & DynamicLibrary::operator=( DynamicLibrary const & lib )
	{
		doClose();

		if ( lib.m_library )
		{
			m_pathLibrary = lib.m_pathLibrary;
			doOpen();
		}

		return *this;
	}

	DynamicLibrary & DynamicLibrary::operator=( DynamicLibrary && lib )noexcept
	{
		if ( this != &lib )
		{
			m_library = castor::move( lib.m_library );
			m_pathLibrary = castor::move( lib.m_pathLibrary );
			lib.m_library = nullptr;
			lib.m_pathLibrary.clear();
		}

		return *this;
	}
}
