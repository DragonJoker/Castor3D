﻿#include "GeneratorPlugin.hpp"

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
	static const String CreateGeneratorFunctionABIName = cuT( "?CreateGenerator@@YAPAVGenerator@Castor3D@@PAVTextureUnit@2@@Z" );
	static const String DestroyGeneratorFunctionABIName = cuT( "?DestroyGenerator@@YAXPAVGenerator@Castor3D@@@Z" );
#elif defined( __GNUG__)
	static const String CreateGeneratorFunctionABIName = cuT( "_Z15CreateGeneratorPN8Castor3D11TextureUnitE" );
	static const String DestroyGeneratorFunctionABIName = cuT( "_Z16DestroyGeneratorPN8Castor3D9GeneratorE" );
#else
#	error "Implement ABI names for this compiler"
#endif

	GeneratorPlugin::GeneratorPlugin( DynamicLibrarySPtr p_library, Engine * p_engine )
		: PluginBase( ePLUGIN_TYPE_DIVIDER, p_library, *p_engine )
	{
		if ( !p_library->GetFunction( m_pfnCreateGenerator, CreateGeneratorFunctionABIName ) )
		{
			String l_strError = cuT( "Error encountered while loading dll [" ) + p_library->GetPath().GetFileName() + cuT( "] CreateGenerator plug-in function : " );
			l_strError += System::GetLastErrorText();
			CASTOR_PLUGIN_EXCEPTION( string::string_cast< char >( l_strError ), false );
		}

		if ( !p_library->GetFunction( m_pfnDestroyGenerator, DestroyGeneratorFunctionABIName ) )
		{
			String l_strError = cuT( "Error encountered while loading dll [" ) + p_library->GetPath().GetFileName() + cuT( "] DestroyGenerator plug-in function : " );
			l_strError += System::GetLastErrorText();
			CASTOR_PLUGIN_EXCEPTION( string::string_cast< char >( l_strError ), false );
		}

		if ( m_pfnOnLoad )
		{
			m_pfnOnLoad( GetEngine() );
		}
	}

	GeneratorPlugin::~GeneratorPlugin()
	{
		if ( m_pfnOnUnload )
		{
			m_pfnOnUnload( GetEngine() );
		}
	}

	Subdivider * GeneratorPlugin::CreateGenerator( TextureUnit * p_texture )
	{
		Subdivider * l_return = nullptr;

		if ( m_pfnCreateGenerator )
		{
			l_return = m_pfnCreateGenerator( p_texture );
		}

		return l_return;
	}

	void GeneratorPlugin::DestroyGenerator( Subdivider * p_pGenerator )
	{
		if ( m_pfnDestroyGenerator )
		{
			m_pfnDestroyGenerator( p_pGenerator );
		}
	}
}
