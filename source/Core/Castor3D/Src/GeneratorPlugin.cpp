#include "GeneratorPlugin.hpp"

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
	static const String CreateGeneratorFunctionABIName		= cuT( "?CreateGenerator@@YAPAVGenerator@Castor3D@@PAVTextureUnit@2@@Z" );
	static const String DestroyGeneratorFunctionABIName		= cuT( "?DestroyGenerator@@YAXPAVGenerator@Castor3D@@@Z" );
#elif defined( __GNUG__)
	static const String CreateGeneratorFunctionABIName		= cuT( "_Z15CreateGeneratorPN8Castor3D11TextureUnitE" );
	static const String DestroyGeneratorFunctionABIName		= cuT( "_Z16DestroyGeneratorPN8Castor3D9GeneratorE" );
#else
#	error "Implement ABI names for this compiler"
#endif

	GeneratorPlugin::GeneratorPlugin( DynamicLibrarySPtr p_pLibrary )
		:	PluginBase( ePLUGIN_TYPE_DIVIDER, p_pLibrary )
	{
		if ( !p_pLibrary->GetFunction( m_pfnCreateGenerator, CreateGeneratorFunctionABIName ) )
		{
			String l_strError = cuT( "Error encountered while loading dll [" ) + p_pLibrary->GetPath().GetFileName() + cuT( "] CreateGenerator plugin function : " );
			l_strError += str_utils::to_string( dlerror() );
			CASTOR_PLUGIN_EXCEPTION( str_utils::to_str( l_strError ), false );
		}

		if ( !p_pLibrary->GetFunction( m_pfnDestroyGenerator, DestroyGeneratorFunctionABIName ) )
		{
			String l_strError = cuT( "Error encountered while loading dll [" ) + p_pLibrary->GetPath().GetFileName() + cuT( "] DestroyGenerator plugin function : " );
			l_strError += str_utils::to_string( dlerror() );
			CASTOR_PLUGIN_EXCEPTION( str_utils::to_str( l_strError ), false );
		}
	}

	GeneratorPlugin::~GeneratorPlugin()
	{
	}

	Subdivider * GeneratorPlugin::CreateGenerator( TextureUnit * p_pTexture )
	{
		Subdivider * l_pReturn = nullptr;

		if ( m_pfnCreateGenerator )
		{
			l_pReturn = m_pfnCreateGenerator( p_pTexture );
		}

		return l_pReturn;
	}

	void GeneratorPlugin::DestroyGenerator( Subdivider * p_pGenerator )
	{
		if ( m_pfnDestroyGenerator )
		{
			m_pfnDestroyGenerator( p_pGenerator );
		}
	}
}
