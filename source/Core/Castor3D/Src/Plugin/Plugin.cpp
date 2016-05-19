#include "Plugin.hpp"

#include "Miscellaneous/Version.hpp"

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
	static const String GetNameFunctionABIName = cuT( "?GetName@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ" );
#	if defined( _WIN64 )
	static const String GetRequiredVersionFunctionABIName = cuT( "?GetRequiredVersion@@YAXAEAVVersion@Castor3D@@@Z" );
	static const String GetOnLoadFunctionABIName = cuT( "?OnLoad@@YAXPEAVEngine@Castor3D@@@Z" );
	static const String GetOnUnloadFunctionABIName = cuT( "?OnUnload@@YAXPEAVEngine@Castor3D@@@Z" );
#	else
	static const String GetRequiredVersionFunctionABIName = cuT( "?GetRequiredVersion@@YAXAAVVersion@Castor3D@@@Z" );
	static const String GetOnLoadFunctionABIName = cuT( "?OnLoad@@YAXPAVEngine@Castor3D@@@Z" );
	static const String GetOnUnloadFunctionABIName = cuT( "?OnUnload@@YAXPAVEngine@Castor3D@@@Z" );
#	endif
#elif defined( __GNUG__)
#	if GCC_VERSION >= 50300
	static const String GetNameFunctionABIName = cuT( "_Z7GetNameB5cxx11v" );
#	else
	static const String GetNameFunctionABIName = cuT( "_Z7GetNamev" );
#	endif
	static const String GetRequiredVersionFunctionABIName = cuT( "_Z18GetRequiredVersionRN8Castor3D7VersionE" );
	static const String GetOnLoadFunctionABIName = cuT( "_Z6OnLoadPN8Castor3D6EngineE" );
	static const String GetOnUnloadFunctionABIName = cuT( "_Z8OnUnloadPN8Castor3D6EngineE" );
#else
#	error "Implement ABI names for this compiler"
#endif

	PluginBase::PluginBase( ePLUGIN_TYPE p_type, DynamicLibrarySPtr p_library, Engine & p_engine )
		: OwnedBy< Engine >( p_engine )
		, m_pfnGetRequiredVersion( 0 )
		, m_pfnGetName( 0 )
		, m_type( p_type )
	{
		if ( !p_library->GetFunction( m_pfnGetName, GetNameFunctionABIName ) )
		{
			String l_strError = cuT( "Error encountered while loading dll [" ) + p_library->GetPath().GetFileName() + cuT( "] plug-in GetName function : " );
			l_strError += System::GetLastErrorText();
			CASTOR_PLUGIN_EXCEPTION( string::string_cast< char >( l_strError ), true );
		}

		if ( !p_library->GetFunction( m_pfnGetRequiredVersion, GetRequiredVersionFunctionABIName ) )
		{
			String l_strError = cuT( "Error encountered while loading dll [" ) + p_library->GetPath().GetFileName() + cuT( "] plug-in GetRequiredVersion function : " );
			l_strError += System::GetLastErrorText();
			CASTOR_PLUGIN_EXCEPTION( string::string_cast< char >( l_strError ), true );
		}

		p_library->GetFunction( m_pfnOnLoad, GetOnLoadFunctionABIName );
		p_library->GetFunction( m_pfnOnUnload, GetOnUnloadFunctionABIName );
	}

	PluginBase::~PluginBase()
	{
	}

	void PluginBase::GetRequiredVersion( Version & p_version )const
	{
		if ( m_pfnGetRequiredVersion )
		{
			m_pfnGetRequiredVersion( p_version );
		}
	}

	String PluginBase::GetName()const
	{
		String l_strReturn;

		if ( m_pfnGetName )
		{
			l_strReturn = m_pfnGetName();
		}

		return l_strReturn;
	}
}
