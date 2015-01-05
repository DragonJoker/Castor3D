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
#		if ( _MSC_VER < 1700 )
	static const String CreateTechniqueFunctionABIName		= cuT( "?CreateTechnique@@YA?AV?$shared_ptr@VRenderTechnique@Castor3D@@@tr1@std@@PEAVRenderSystem@Castor3D@@@Z" );
#		else
	static const String CreateTechniqueFunctionABIName		= cuT( "?CreateTechnique@@YA?AV?$shared_ptr@VRenderTechnique@Castor3D@@@std@@PEAVRenderSystem@Castor3D@@@Z" );
#		endif
#	else
#		if ( _MSC_VER < 1700 )
	static const String CreateTechniqueFunctionABIName		= cuT( "?CreateTechnique@@YA?AV?$shared_ptr@VRenderTechnique@Castor3D@@@tr1@std@@PAVRenderSystem@Castor3D@@@Z" );
#		else
	static const String CreateTechniqueFunctionABIName		= cuT( "?CreateTechnique@@YA?AV?$shared_ptr@VRenderTechnique@Castor3D@@@std@@PAVRenderSystem@Castor3D@@@Z" );
#		endif
#	endif
#elif defined( __GNUG__)
	static const String CreateTechniqueFunctionABIName		= cuT( "_Z12CreateTechniquePN8Castor3D12RenderSystemE" );
#else
#	error "Implement ABI names for this compiler"
#endif

	TechniquePlugin::TechniquePlugin( DynamicLibrarySPtr p_pLibrary )
		:	PluginBase( ePLUGIN_TYPE_TECHNIQUE, p_pLibrary )
	{
		if ( !p_pLibrary->GetFunction( m_pfnCreateTechnique, CreateTechniqueFunctionABIName ) )
		{
			String l_strError = cuT( "Error encountered while loading dll [" ) + p_pLibrary->GetPath().GetFileName() + cuT( "] CreateTechnique plugin function : " );
			l_strError += str_utils::to_string( dlerror() );
			CASTOR_PLUGIN_EXCEPTION( str_utils::to_str( l_strError ), false );
		}
	}

	TechniquePlugin::~TechniquePlugin()
	{
	}

	RenderTechniqueBaseSPtr TechniquePlugin::CreateTechnique( RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & p_params )
	{
		RenderTechniqueBaseSPtr l_pReturn;

		if ( m_pfnCreateTechnique )
		{
			l_pReturn = m_pfnCreateTechnique( p_renderTarget, p_pRenderSystem, p_params );
		}

		return l_pReturn;
	}
}
