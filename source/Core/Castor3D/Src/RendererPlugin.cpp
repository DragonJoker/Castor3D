#include "RendererPlugin.hpp"

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
	static const String GetRendererTypeFunctionABIName		= cuT( "?GetRendererType@@YA?AW4eRENDERER_TYPE@Castor3D@@XZ" );
#	if defined( _WIN64 )
	static const String CreateRenderSystemFunctionABIName	= cuT( "?CreateRenderSystem@@YAPEAVRenderSystem@Castor3D@@PEAVEngine@2@PEAVLogger@Castor@@@Z" );
	static const String DestroyRenderSystemFunctionABIName	= cuT( "?DestroyRenderSystem@@YAXPEAVRenderSystem@Castor3D@@@Z" );
#	else
	static const String CreateRenderSystemFunctionABIName	= cuT( "?CreateRenderSystem@@YAPAVRenderSystem@Castor3D@@PAVEngine@2@PAVLogger@Castor@@@Z" );
	static const String DestroyRenderSystemFunctionABIName	= cuT( "?DestroyRenderSystem@@YAXPAVRenderSystem@Castor3D@@@Z" );
#	endif
#elif defined( __GNUG__)
	static const String GetRendererTypeFunctionABIName		= cuT( "_Z15GetRendererTypev" );
	static const String CreateRenderSystemFunctionABIName	= cuT( "_Z18CreateRenderSystemPN8Castor3D6EngineEPN6Castor6LoggerE" );
	static const String DestroyRenderSystemFunctionABIName	= cuT( "_Z19DestroyRenderSystemPN8Castor3D12RenderSystemE" );
#else
#	error "Implement ABI names for this compiler"
#endif

	RendererPlugin::RendererPlugin( DynamicLibrarySPtr p_pLibrary )
		:	PluginBase( ePLUGIN_TYPE_RENDERER, p_pLibrary )
	{
		if ( !p_pLibrary->GetFunction( m_pfnGetRendererType, GetRendererTypeFunctionABIName ) )
		{
			String l_strError = cuT( "Error encountered while loading dll [" ) + p_pLibrary->GetPath().GetFileName() + cuT( "] GetRendererType plugin function : " );
			l_strError += str_utils::to_string( dlerror() );
			CASTOR_PLUGIN_EXCEPTION( str_utils::to_str( l_strError ), false );
		}

		if ( !p_pLibrary->GetFunction( m_pfnCreateRenderSystem, CreateRenderSystemFunctionABIName ) )
		{
			String l_strError = cuT( "Error encountered while loading dll [" ) + p_pLibrary->GetPath().GetFileName() + cuT( "] CreateRenderSystem plugin function : " );
			l_strError += str_utils::to_string( dlerror() );
			CASTOR_PLUGIN_EXCEPTION( str_utils::to_str( l_strError ), false );
		}

		if ( !p_pLibrary->GetFunction( m_pfnDestroyRenderSystem, DestroyRenderSystemFunctionABIName ) )
		{
			String l_strError = cuT( "Error encountered while loading dll [" ) + p_pLibrary->GetPath().GetFileName() + cuT( "] DestroyRenderSystem plugin function : " );
			l_strError += str_utils::to_string( dlerror() );
			CASTOR_PLUGIN_EXCEPTION( str_utils::to_str( l_strError ), false );
		}
	}

	RendererPlugin::RendererPlugin( RendererPlugin const & p_plugin )
		:	PluginBase( p_plugin )
		,	m_pfnCreateRenderSystem( p_plugin.m_pfnCreateRenderSystem )
		,	m_pfnDestroyRenderSystem( p_plugin.m_pfnDestroyRenderSystem )
		,	m_pfnGetRendererType( p_plugin.m_pfnGetRendererType )
	{
	}

	RendererPlugin::RendererPlugin( RendererPlugin && p_plugin )
		:	PluginBase( std::move( p_plugin ) )
		,	m_pfnCreateRenderSystem( std::move( p_plugin.m_pfnCreateRenderSystem ) )
		,	m_pfnDestroyRenderSystem( std::move( p_plugin.m_pfnDestroyRenderSystem ) )
		,	m_pfnGetRendererType( std::move( p_plugin.m_pfnGetRendererType ) )
	{
		p_plugin.m_pfnCreateRenderSystem	= NULL;
		p_plugin.m_pfnDestroyRenderSystem	= NULL;
		p_plugin.m_pfnGetRendererType		= NULL;
	}

	RendererPlugin::~RendererPlugin()
	{
	}

	RendererPlugin & RendererPlugin::operator =( RendererPlugin const & p_plugin )
	{
		PluginBase::operator =( p_plugin );
		m_pfnCreateRenderSystem		= p_plugin.m_pfnCreateRenderSystem;
		m_pfnDestroyRenderSystem	= p_plugin.m_pfnDestroyRenderSystem;
		m_pfnGetRendererType		= p_plugin.m_pfnGetRendererType;
		return * this;
	}

	RendererPlugin & RendererPlugin::operator =( RendererPlugin && p_plugin )
	{
		PluginBase::operator =( std::move( p_plugin ) );

		if ( this != &p_plugin )
		{
			m_pfnCreateRenderSystem		= std::move( p_plugin.m_pfnCreateRenderSystem );
			m_pfnDestroyRenderSystem	= std::move( p_plugin.m_pfnDestroyRenderSystem );
			m_pfnGetRendererType		= std::move( p_plugin.m_pfnGetRendererType );
			p_plugin.m_pfnCreateRenderSystem	= NULL;
			p_plugin.m_pfnDestroyRenderSystem	= NULL;
			p_plugin.m_pfnGetRendererType		= NULL;
		}

		return * this;
	}

	RenderSystem * RendererPlugin::CreateRenderSystem( Engine * p_pEngine, Logger * p_pLogger )
	{
		RenderSystem * l_pReturn = NULL;

		if ( m_pfnCreateRenderSystem )
		{
			l_pReturn = m_pfnCreateRenderSystem( p_pEngine, p_pLogger );
		}

		return l_pReturn;
	}

	void RendererPlugin::DestroyRenderSystem( RenderSystem * p_pRenderSystem )
	{
		if ( m_pfnDestroyRenderSystem )
		{
			m_pfnDestroyRenderSystem( p_pRenderSystem );
		}
	}

	eRENDERER_TYPE RendererPlugin::GetRendererType()
	{
		eRENDERER_TYPE l_eReturn = eRENDERER_TYPE_UNDEFINED;

		if ( m_pfnGetRendererType )
		{
			l_eReturn = m_pfnGetRendererType();
		}

		return l_eReturn;
	}
}
