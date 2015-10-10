#include "TechniquePlugin.hpp"

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
	static const String RegisterTechniqueFunctionABIName = cuT( "?RegisterTechnique@@YAXAEAVTechniqueFactory@Castor3D@@@Z" );
	static const String UnregisterTechniqueFunctionABIName = cuT( "?UnregisterTechnique@@YAXAEAVTechniqueFactory@Castor3D@@@Z" );
#	else
	static const String RegisterTechniqueFunctionABIName = cuT( "?RegisterTechnique@@YAXAAVTechniqueFactory@Castor3D@@@Z" );
	static const String UnregisterTechniqueFunctionABIName = cuT( "?UnregisterTechnique@@YAXAAVTechniqueFactory@Castor3D@@@Z" );
#	endif
#elif defined( __GNUG__)
	static const String RegisterTechniqueFunctionABIName = cuT( "_Z17RegisterTechniqueN8Castor3D17TechniqueFactoryE" );
	static const String UnregisterTechniqueFunctionABIName = cuT( "_Z19UnregisterTechniqueN8Castor3D17TechniqueFactoryE" );
#else
#	error "Implement ABI names for this compiler"
#endif

	TechniquePlugin::TechniquePlugin( DynamicLibrarySPtr p_pLibrary, Engine * p_engine )
		: PluginBase( ePLUGIN_TYPE_TECHNIQUE, p_pLibrary, *p_engine )
	{
		if ( m_pfnOnLoad )
		{
			m_pfnOnLoad( GetOwner() );
		}
	}

	TechniquePlugin::~TechniquePlugin()
	{
		if ( m_pfnOnUnload )
		{
			m_pfnOnUnload( GetOwner() );
		}
	}
}
