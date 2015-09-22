#include "GenericPlugin.hpp"

#if defined( _WIN32 )
#	include <Windows.h>
#else
#	include <dlfcn.h>
#endif

#include <DynamicLibrary.hpp>

using namespace Castor;

namespace Castor3D
{
#pragma warning( disable:4290 )
#if defined( _MSC_VER)
#	if defined( _WIN64 )
	static const String AddOptionalParsersFunctionABIName = cuT( "?AddOptionalParsers@@YAXPEAVSceneFileParser@Castor3D@@@Z" );
#	else
	static const String AddOptionalParsersFunctionABIName = cuT( "?AddOptionalParsers@@YAXPAVSceneFileParser@Castor3D@@@Z" );
#	endif
#elif defined( __GNUG__)
	static const String AddOptionalParsersFunctionABIName =  cuT( "_Z18AddOptionalParsersPN8Castor3D15SceneFileParserE" );
#else
#	error "Implement ABI names for this compiler"
#endif

	GenericPlugin::GenericPlugin( DynamicLibrarySPtr p_pLibrary, Engine * p_engine )
		: PluginBase( ePLUGIN_TYPE_GENERIC, p_pLibrary, p_engine )
	{
		// Since this is an optional function, don't fail on error.
		p_pLibrary->GetFunction( m_pfnAddOptionalParsers, AddOptionalParsersFunctionABIName );

		if ( m_pfnOnLoad )
		{
			m_pfnOnLoad( m_engine );
		}
	}

	GenericPlugin::~GenericPlugin()
	{
		if ( m_pfnOnUnload )
		{
			m_pfnOnUnload( m_engine );
		}
	}

	void GenericPlugin::AddOptionalParsers( SceneFileParser * p_parser )
	{
		if ( m_pfnAddOptionalParsers )
		{
			m_pfnAddOptionalParsers( p_parser );
		}
	}
}
