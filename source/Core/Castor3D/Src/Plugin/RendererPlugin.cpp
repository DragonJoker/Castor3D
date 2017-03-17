#include "RendererPlugin.hpp"

#include "Render/RenderSystem.hpp"

#if defined( CASTOR_PLATFORM_WINDOWS )
#	include <Windows.h>
#else
#	include <dlfcn.h>
#endif

#include <Miscellaneous/DynamicLibrary.hpp>

using namespace Castor;

namespace Castor3D
{
#if defined( CASTOR_COMPILER_MSVC )
	static const String GetRendererTypeFunctionABIName = cuT( "?GetRendererType@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ" );
#elif defined( CASTOR_COMPILER_GNUC )
#	if CASTOR_COMPILER_VERSION >= 50300
	static const String GetRendererTypeFunctionABIName = cuT( "_Z15GetRendererTypeB5cxx11v" );
#	else
	static const String GetRendererTypeFunctionABIName = cuT( "_Z15GetRendererTypev" );
#	endif
#else
#	error "Implement ABI names for this compiler"
#endif

	RendererPlugin::RendererPlugin( DynamicLibrarySPtr p_library, Engine * p_engine )
		: Plugin( PluginType::eRenderer, p_library, *p_engine )
	{
		if ( !p_library->GetFunction( m_pfnGetRendererType, GetRendererTypeFunctionABIName ) )
		{
			String l_strError = cuT( "Error encountered while loading dll [" ) + p_library->GetPath().GetFileName() + cuT( "] GetRendererType plug-in function : " );
			l_strError += System::GetLastErrorText();
			CASTOR_PLUGIN_EXCEPTION( string::string_cast< char >( l_strError ), false );
		}

		if ( m_pfnOnLoad )
		{
			m_pfnOnLoad( GetEngine() );
		}
	}

	RendererPlugin::~RendererPlugin()
	{
		if ( m_pfnOnUnload )
		{
			m_pfnOnUnload( GetEngine() );
		}
	}

	Castor::String RendererPlugin::GetRendererType()
	{
		Castor::String l_return = RENDERER_TYPE_UNDEFINED;

		if ( m_pfnGetRendererType )
		{
			l_return = m_pfnGetRendererType();
		}

		return l_return;
	}
}
