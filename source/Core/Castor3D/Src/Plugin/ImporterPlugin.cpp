#include "ImporterPlugin.hpp"

#if defined( _WIN32 )
#	include <Windows.h>
#else
#	include <dlfcn.h>
#endif

#include <Miscellaneous/DynamicLibrary.hpp>

using namespace Castor;

namespace Castor3D
{
#if defined( _MSC_VER)
#	if defined( _WIN64 )
	static const String GetExtensionFunctionABIName = cuT( "?GetExtensions@@YA?AV?$vector@U?$pair@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V12@@std@@V?$allocator@U?$pair@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V12@@std@@@2@@std@@PEAVEngine@Castor3D@@@Z" );
#	else
	static const String GetExtensionFunctionABIName = cuT( "?GetExtensions@@YA?AV?$vector@U?$pair@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V12@@std@@V?$allocator@U?$pair@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V12@@std@@@2@@std@@PAVEngine@Castor3D@@@Z" );
#	endif
#elif defined( __GNUG__)
#	if GCC_VERSION >= 50300
	static const String GetExtensionFunctionABIName = cuT( "_Z13GetExtensionsB5cxx11PN8Castor3D6EngineE" );
#	else
	static const String GetExtensionFunctionABIName = cuT( "_Z13GetExtensionsPN8Castor3D6EngineE" );
#	endif
#else
#	error "Implement ABI names for this compiler"
#endif

	ImporterPlugin::ImporterPlugin( DynamicLibrarySPtr p_library, Engine * p_engine )
		:	Plugin( ePLUGIN_TYPE_IMPORTER, p_library, *p_engine )
	{
		if ( !p_library->GetFunction( m_pfnGetExtension, GetExtensionFunctionABIName ) )
		{
			String l_strError = cuT( "Error encountered while loading dll [" ) + p_library->GetPath().GetFileName() + cuT( "] GetExtension plug-in function : " );
			l_strError += System::GetLastErrorText();
			CASTOR_PLUGIN_EXCEPTION( string::string_cast< char >( l_strError ), false );
		}

		if ( m_pfnOnLoad )
		{
			m_pfnOnLoad( GetEngine() );
		}
	}

	ImporterPlugin::~ImporterPlugin()
	{
		if ( m_pfnOnUnload )
		{
			m_pfnOnUnload( GetEngine() );
		}
	}

	ImporterPlugin::ExtensionArray ImporterPlugin::GetExtensions()
	{
		ExtensionArray l_arrayReturn;

		if ( m_pfnGetExtension )
		{
			l_arrayReturn = m_pfnGetExtension( GetEngine() );
		}

		return l_arrayReturn;
	}
}
