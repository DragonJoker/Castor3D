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
	static const String CreateImporterFunctionABIName		= cuT( "?Create@@YAXPEAVEngine@Castor3D@@PEAVImporterPlugin@2@@Z" );
	static const String DestroyImporterFunctionABIName		= cuT( "?Destroy@@YAXPEAVImporterPlugin@Castor3D@@@Z" );
	static const String GetExtensionFunctionABIName			= cuT( "?GetExtensions@@YA?AV?$vector@U?$pair@V?$basic_string@_WU?$char_traits@_W@std@@V?$allocator@_W@2@@std@@V12@@std@@V?$allocator@U?$pair@V?$basic_string@_WU?$char_traits@_W@std@@V?$allocator@_W@2@@std@@V12@@std@@@2@@std@@XZ" );
#	else
	static const String CreateImporterFunctionABIName		= cuT( "?Create@@YAXPAVEngine@Castor3D@@PAVImporterPlugin@2@@Z" );
	static const String DestroyImporterFunctionABIName		= cuT( "?Destroy@@YAXPAVImporterPlugin@Castor3D@@@Z" );
	static const String GetExtensionFunctionABIName			= cuT( "?GetExtensions@@YA?AV?$vector@U?$pair@V?$basic_string@_WU?$char_traits@_W@std@@V?$allocator@_W@2@@std@@V12@@std@@V?$allocator@U?$pair@V?$basic_string@_WU?$char_traits@_W@std@@V?$allocator@_W@2@@std@@V12@@std@@@2@@std@@XZ" );
#	endif
#elif defined( __GNUG__)
	static const String CreateImporterFunctionABIName		= cuT( "_Z6CreatePN8Castor3D6EngineEPNS_14ImporterPluginE" );
	static const String DestroyImporterFunctionABIName		= cuT( "_Z7DestroyPN8Castor3D14ImporterPluginE" );
	static const String GetExtensionFunctionABIName			= cuT( "_Z13GetExtensionsv" );
#else
#	error "Implement ABI names for this compiler"
#endif

	ImporterPlugin::ImporterPlugin( Engine * p_pEngine, DynamicLibrarySPtr p_pLibrary )
		:	PluginBase( ePLUGIN_TYPE_IMPORTER, p_pLibrary )
	{
		if ( !p_pLibrary->GetFunction( m_pfnCreateImporter, CreateImporterFunctionABIName ) )
		{
			String l_strError = cuT( "Error encountered while loading dll [" ) + p_pLibrary->GetPath().GetFileName() + cuT( "] CreateImporter plugin function : " );
			l_strError += str_utils::to_string( dlerror() );
			CASTOR_PLUGIN_EXCEPTION( str_utils::to_str( l_strError ), false );
		}

		if ( !p_pLibrary->GetFunction( m_pfnDestroyImporter, DestroyImporterFunctionABIName ) )
		{
			String l_strError = cuT( "Error encountered while loading dll [" ) + p_pLibrary->GetPath().GetFileName() + cuT( "] DestroyImporter plugin function : " );
			l_strError += str_utils::to_string( dlerror() );
			CASTOR_PLUGIN_EXCEPTION( str_utils::to_str( l_strError ), false );
		}

		if ( !p_pLibrary->GetFunction( m_pfnGetExtension, GetExtensionFunctionABIName ) )
		{
			String l_strError = cuT( "Error encountered while loading dll [" ) + p_pLibrary->GetPath().GetFileName() + cuT( "] GetExtension plugin function : " );
			l_strError += str_utils::to_string( dlerror() );
			CASTOR_PLUGIN_EXCEPTION( str_utils::to_str( l_strError ), false );
		}

		m_pfnCreateImporter( p_pEngine, this );
	}

	ImporterPlugin::ImporterPlugin( ImporterPlugin const & p_plugin )
		:	PluginBase( p_plugin )
		,	m_pfnCreateImporter( p_plugin.m_pfnCreateImporter )
		,	m_pfnDestroyImporter( p_plugin.m_pfnDestroyImporter )
		,	m_pfnGetExtension( p_plugin.m_pfnGetExtension )
		,	m_pImporter( p_plugin.m_pImporter )
	{
	}

	ImporterPlugin::ImporterPlugin( ImporterPlugin && p_plugin )
		:	PluginBase( std::move( p_plugin ) )
		,	m_pfnCreateImporter( std::move( p_plugin.m_pfnCreateImporter ) )
		,	m_pfnDestroyImporter( std::move( p_plugin.m_pfnDestroyImporter ) )
		,	m_pfnGetExtension( std::move( p_plugin.m_pfnGetExtension ) )
		,	m_pImporter( std::move( p_plugin.m_pImporter ) )
	{
		p_plugin.m_pfnCreateImporter	= NULL;
		p_plugin.m_pfnDestroyImporter	= NULL;
		p_plugin.m_pfnGetExtension		= NULL;
		p_plugin.m_pImporter			.reset();
	}

	ImporterPlugin & ImporterPlugin::operator =( ImporterPlugin const & p_plugin )
	{
		PluginBase::operator =( p_plugin );
		m_pfnCreateImporter		= p_plugin.m_pfnCreateImporter;
		m_pfnDestroyImporter	= p_plugin.m_pfnDestroyImporter;
		m_pfnGetExtension		= p_plugin.m_pfnGetExtension;
		m_pImporter				= p_plugin.m_pImporter;
		return * this;
	}

	ImporterPlugin & ImporterPlugin::operator =( ImporterPlugin && p_plugin )
	{
		PluginBase::operator =( std::move( p_plugin ) );

		if ( this != & p_plugin )
		{
			m_pfnCreateImporter		= std::move( p_plugin.m_pfnCreateImporter );
			m_pfnDestroyImporter	= std::move( p_plugin.m_pfnDestroyImporter );
			m_pfnGetExtension		= std::move( p_plugin.m_pfnGetExtension );
			m_pImporter				= std::move( p_plugin.m_pImporter );
			p_plugin.m_pfnCreateImporter	= NULL;
			p_plugin.m_pfnDestroyImporter	= NULL;
			p_plugin.m_pfnGetExtension		= NULL;
			p_plugin.m_pImporter			.reset();
		}

		return * this;
	}

	ImporterPlugin::~ImporterPlugin()
	{
		m_pfnDestroyImporter( this );
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
