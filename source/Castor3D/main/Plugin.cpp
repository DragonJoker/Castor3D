#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/main/Plugin.h"
#include "Castor3D/main/Version.h"


#ifdef _WIN32
#	define dlerror() GetLastError()
#endif

using namespace Castor3D;
using namespace Castor::Utils;

//*************************************************************************************************

PluginBase :: PluginBase( const String & p_fileName)
	:	m_pDLLRefCount			( NULL)
	,	m_pfnGetRequiredVersion	( 0)
	,	m_library				( new DynamicLibrary)
{

	Cout( CU_T( "Plugin name [") << p_fileName << CU_T( "]") << std::endl);

	if ( ! m_library->Open( p_fileName))
	{
		Cerr( CU_T( "Error encountered while loading plugin : ") << dlerror() << std::endl);
		throw false;
	}

	m_pfnGetRequiredVersion = reinterpret_cast <PGetRequiredVersionFunction> ( m_library->GetFunction( "GetRequiredVersion"));

	if (m_pfnGetRequiredVersion == NULL)
	{
		Cerr( CU_T( "Error encountered while loading plugin version function : ") << dlerror() << std::endl);
		throw false;
	}

	m_pDLLRefCount = new size_t( 1);
}

PluginBase :: PluginBase( const PluginBase & p_plugin)
	:	m_pDLLRefCount			( p_plugin.m_pDLLRefCount)
	,	m_pfnGetRequiredVersion	( p_plugin.m_pfnGetRequiredVersion)
{
	++*m_pDLLRefCount;
}

PluginBase :: ~PluginBase()
{
	if (--*m_pDLLRefCount == 0)
	{
		delete m_pDLLRefCount;
		delete m_library;
	}
}

void PluginBase :: GetRequiredVersion( Version & p_version)
{
	if (m_pfnGetRequiredVersion)
	{
		m_pfnGetRequiredVersion( p_version);
	}
}

PluginBase & PluginBase :: operator =( const PluginBase & p_plugin)
{
	m_pfnGetRequiredVersion = p_plugin.m_pfnGetRequiredVersion;
	m_pDLLRefCount = p_plugin.m_pDLLRefCount;
	++*m_pDLLRefCount;
	return *this;
}

//*************************************************************************************************

RendererPlugin :: RendererPlugin( const String & p_fileName)
	:	PluginBase( p_fileName)
{
	m_eType = PluginBase::ePluginRenderer;
	m_pfnRegisterPlugin = reinterpret_cast <PRegisterPluginFunction> ( m_library->GetFunction( "RegisterPlugin"));

	if (m_pfnRegisterPlugin == NULL)
	{
		Cerr( CU_T( "Error encountered while loading plugin register function : ") << dlerror() << std::endl);
		throw false;
	}
}

RendererPlugin :: RendererPlugin( const RendererPlugin & p_plugin)
	:	PluginBase( p_plugin),
		m_pfnRegisterPlugin( p_plugin.m_pfnRegisterPlugin)
{
}

RendererPlugin :: ~RendererPlugin()
{
}

RendererPlugin & RendererPlugin :: operator =( const RendererPlugin & p_plugin)
{
	PluginBase::operator =( p_plugin);
	m_pfnRegisterPlugin = p_plugin.m_pfnRegisterPlugin;
	return * this;
}

void RendererPlugin :: RegisterPlugin( Root & p_root)
{
	if (m_pfnRegisterPlugin)
	{
		m_pfnRegisterPlugin( p_root);
	}
}

//*************************************************************************************************

ImporterPlugin :: ImporterPlugin( const String & p_fileName)
	:	PluginBase( p_fileName)
{
	m_eType = PluginBase::ePluginImporter;
	m_pfnCreateImporterFunction = reinterpret_cast <PCreateImporterFunction> ( m_library->GetFunction( "CreateImporter"));

	if (m_pfnCreateImporterFunction == NULL)
	{
		Cerr( CU_T( "Error encountered while loading plugin importer creator function : ") << dlerror() << std::endl);
		throw false;
	}
}

ImporterPlugin :: ImporterPlugin( const ImporterPlugin & p_plugin)
	:	PluginBase( p_plugin),
		m_pfnCreateImporterFunction( p_plugin.m_pfnCreateImporterFunction)
{
}

ImporterPlugin :: ~ImporterPlugin()
{
}

ImporterPlugin & ImporterPlugin :: operator =( const ImporterPlugin & p_plugin)
{
	PluginBase::operator =( p_plugin);
	m_pfnCreateImporterFunction = p_plugin.m_pfnCreateImporterFunction;
	return * this;
}

Importer * ImporterPlugin :: CreateImporter( SceneManager * p_pManager)
{
	Importer * l_pReturn = NULL;

	if (m_pfnCreateImporterFunction)
	{
		l_pReturn = m_pfnCreateImporterFunction( p_pManager);
	}

	return l_pReturn;
}

//*************************************************************************************************

DividerPlugin :: DividerPlugin( const String & p_fileName)
	:	PluginBase( p_fileName)
{
	m_eType = PluginBase::ePluginDivider;
	m_pfnCreateDividerFunction = reinterpret_cast <PCreateDividerFunction> ( m_library->GetFunction( "CreateDivider"));

	if (m_pfnCreateDividerFunction == NULL)
	{
		Cerr( CU_T( "Error encountered while loading plugin divider creator function : ") << dlerror() << std::endl);
		throw false;
	}
}

DividerPlugin :: DividerPlugin( const DividerPlugin & p_plugin)
	:	PluginBase( p_plugin),
		m_pfnCreateDividerFunction( p_plugin.m_pfnCreateDividerFunction)
{
}

DividerPlugin :: ~DividerPlugin()
{
}

DividerPlugin & DividerPlugin :: operator =( const DividerPlugin & p_plugin)
{
	PluginBase::operator =( p_plugin);
	m_pfnCreateDividerFunction = p_plugin.m_pfnCreateDividerFunction;
	return * this;
}

Subdivider * DividerPlugin :: CreateDivider( Submesh * p_pSubmesh)
{
	Subdivider * l_pReturn = NULL;

	if (m_pfnCreateDividerFunction)
	{
		l_pReturn = m_pfnCreateDividerFunction( p_pSubmesh);
	}

	return l_pReturn;
}


//*************************************************************************************************