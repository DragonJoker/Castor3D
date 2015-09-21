#include "ImporterPlugin.hpp"

#if defined( _WIN32 )
#	include <Windows.h>
#else
#	include <dlfcn.h>
#endif

#include <DynamicLibrary.hpp>

using namespace Castor;

namespace Castor3D
{
#if defined( _MSC_VER)
#	if defined( _WIN64 )
	static const String CreateImporterFunctionABIName = cuT( "?Create@@YAXPEAVEngine@Castor3D@@PEAVImporterPlugin@2@@Z" );
	static const String DestroyImporterFunctionABIName = cuT( "?Destroy@@YAXPEAVImporterPlugin@Castor3D@@@Z" );
#		if CASTOR_USE_UNICODE
	static const String GetExtensionFunctionABIName = cuT( "?GetExtensions@@YA?AV?$vector@U?$pair@V?$basic_string@_WU?$char_traits@_W@std@@V?$allocator@_W@2@@std@@V12@@std@@V?$allocator@U?$pair@V?$basic_string@_WU?$char_traits@_W@std@@V?$allocator@_W@2@@std@@V12@@std@@@2@@std@@XZ" );
#		else
	static const String GetExtensionFunctionABIName = cuT( "?GetExtensions@@YA?AV?$vector@U?$pair@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V12@@std@@V?$allocator@U?$pair@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V12@@std@@@2@@std@@XZ" );
#		endif
#	else
	static const String CreateImporterFunctionABIName = cuT( "?Create@@YAXPAVEngine@Castor3D@@PAVImporterPlugin@2@@Z" );
	static const String DestroyImporterFunctionABIName = cuT( "?Destroy@@YAXPAVImporterPlugin@Castor3D@@@Z" );
#		if CASTOR_USE_UNICODE
	static const String GetExtensionFunctionABIName = cuT( "?GetExtensions@@YA?AV?$vector@U?$pair@V?$basic_string@_WU?$char_traits@_W@std@@V?$allocator@_W@2@@std@@V12@@std@@V?$allocator@U?$pair@V?$basic_string@_WU?$char_traits@_W@std@@V?$allocator@_W@2@@std@@V12@@std@@@2@@std@@XZ" );
#else
	static const String GetExtensionFunctionABIName = cuT( "?GetExtensions@@YA?AV?$vector@U?$pair@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V12@@std@@V?$allocator@U?$pair@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V12@@std@@@2@@std@@XZ" );
#endif
#	endif
#elif defined( __GNUG__)
	static const String CreateImporterFunctionABIName = cuT( "_Z6CreatePN8Castor3D6EngineEPNS_14ImporterPluginE" );
	static const String DestroyImporterFunctionABIName = cuT( "_Z7DestroyPN8Castor3D14ImporterPluginE" );
	static const String GetExtensionFunctionABIName = cuT( "_Z13GetExtensionsv" );
#else
#	error "Implement ABI names for this compiler"
#endif

	ImporterPlugin::ImporterPlugin( DynamicLibrarySPtr p_pLibrary, Engine * p_engine )
		:	PluginBase( ePLUGIN_TYPE_IMPORTER, p_pLibrary, p_engine )
	{
		if ( !p_pLibrary->GetFunction( m_pfnCreateImporter, CreateImporterFunctionABIName ) )
		{
			String l_strError = cuT( "Error encountered while loading dll [" ) + p_pLibrary->GetPath().GetFileName() + cuT( "] CreateImporter plugin function : " );
			l_strError += System::GetLastErrorText();
			CASTOR_PLUGIN_EXCEPTION( string::string_cast< char >( l_strError ), false );
		}

		if ( !p_pLibrary->GetFunction( m_pfnDestroyImporter, DestroyImporterFunctionABIName ) )
		{
			String l_strError = cuT( "Error encountered while loading dll [" ) + p_pLibrary->GetPath().GetFileName() + cuT( "] DestroyImporter plugin function : " );
			l_strError += System::GetLastErrorText();
			CASTOR_PLUGIN_EXCEPTION( string::string_cast< char >( l_strError ), false );
		}

		if ( !p_pLibrary->GetFunction( m_pfnGetExtension, GetExtensionFunctionABIName ) )
		{
			String l_strError = cuT( "Error encountered while loading dll [" ) + p_pLibrary->GetPath().GetFileName() + cuT( "] GetExtension plugin function : " );
			l_strError += System::GetLastErrorText();
			CASTOR_PLUGIN_EXCEPTION( string::string_cast< char >( l_strError ), false );
		}

		if ( m_pfnOnLoad )
		{
			m_pfnOnLoad( m_engine );
		}

		m_pfnCreateImporter( p_engine, this );
	}

	ImporterPlugin::~ImporterPlugin()
	{
		m_pfnDestroyImporter( this );

		if ( m_pfnOnUnload )
		{
			m_pfnOnUnload( m_engine );
		}
	}

	ImporterPlugin::ExtensionArray ImporterPlugin::GetExtensions()
	{
		ExtensionArray l_arrayReturn;

		if ( m_pfnGetExtension )
		{
			l_arrayReturn = m_pfnGetExtension();
		}

		return l_arrayReturn;
	}
}
