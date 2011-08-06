#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Plugin.hpp"
#include "Castor3D/Version.hpp"

#ifdef _WIN32
#	define dlerror() GetLastError()
#endif

using namespace Castor3D;
using namespace Castor::Utils;

//*************************************************************************************************

PluginException :: PluginException( String const & p_description, String const & p_file, String const & p_function, unsigned int p_line)
	:	Exception( cuT( ""), p_file, p_function, p_line)
{
	m_description = cuT( "Plugin loading error : ") + p_description;
}

PluginException :: ~PluginException()throw()
{
}

//*************************************************************************************************

#if defined( _MSC_VER)
#	define GetNameFunctionABIName				cuT( "?GetName@@YA?AVString@Castor@@XZ")
#	define GetRequiredVersionABIName			cuT( "?GetRequiredVersion@@YAXAAVVersion@Castor3D@@@Z")
#	define CreateRenderSystemFunctionABIName	cuT( "?CreateRenderSystem@@YAPAVRenderSystem@Castor3D@@XZ")
#	define GetRendererTypeFunctionABIName		cuT( "?GetRendererType@@YA?AW4eRENDERER_TYPE@Castor3D@@XZ")
#	define CreateImporterABIName				cuT( "?CreateImporter@@YAPAVImporter@Castor3D@@XZ")
#	define GetExtensionFunctionABIName			cuT( "?GetExtension@@YA?AVString@Castor@@XZ")
#	define CreateDividerABIName					cuT( "?CreateDivider@@YAPAVSubdivider@Castor3D@@PAVSubmesh@2@@Z")
#	define CreateGeneratorABIName				cuT( "?CreateGenerator@@YAPAVGenerator@Castor3D@@PAVTextureUnit@2@@Z")
#elif defined( __GNUG__)
#	define GetNameFunctionABIName				cuT( "_Z7GetNamev")
#	define GetRequiredVersionABIName			cuT( "_Z18GetRequiredVersionRN8Castor3D7VersionE")
#	define CreateRenderSystemFunctionABIName	cuT( "_Z18CreateRenderSystemv")
#	define GetRendererTypeFunctionABIName		cuT( "_Z15GetRendererTypev")
#	define CreateImporterABIName				cuT( "_Z14CreateImporterv")
#	define GetExtensionFunctionABIName			cuT( "_Z12GetExtensionv")
#	define CreateDividerABIName					cuT( "_Z13CreateDividerPN8Castor3D7SubmeshE")
#	define CreateGeneratorABIName				cuT( "_Z15CreateGeneratorPN8Castor3D11TextureUnitE")
#endif

//*************************************************************************************************

PluginBase :: PluginBase( ePLUGIN_TYPE p_eType, Utils::DynamicLibraryPtr p_pLibrary)throw( PluginException)
	:	m_pfnGetRequiredVersion	( 0)
	,	m_pfnGetName			( 0)
	,	m_eType					( p_eType)
{
	try
	{
		m_pfnGetName = reinterpret_cast <PGetNameFunction> ( p_pLibrary->GetFunction( GetNameFunctionABIName));

		if (m_pfnGetName == NULL)
		{
			String l_strError = String( cuT( "Error encountered while loading dll [")) << p_pLibrary->GetPath().GetFileName() << cuT( "] plugin name function : ");
			l_strError << dlerror();
			Logger::LogError( l_strError, false);
			CASTOR_PLUGIN_EXCEPTION( "Retrieving GetName function");

		}

		m_pfnGetRequiredVersion = reinterpret_cast <PGetRequiredVersionFunction> ( p_pLibrary->GetFunction( GetRequiredVersionABIName));

		if (m_pfnGetRequiredVersion == NULL)
		{
			String l_strError = String( cuT( "Error encountered while loading dll [")) << p_pLibrary->GetPath().GetFileName() << cuT( "] plugin version function : ");
			l_strError << dlerror();
			Logger::LogError( l_strError, false);
			CASTOR_PLUGIN_EXCEPTION( "Retrieving GetRequiredVersion function");
		}
	}
	catch (...)
	{
	}
}

PluginBase :: PluginBase( const PluginBase & p_plugin)
	:	m_pfnGetRequiredVersion	( p_plugin.m_pfnGetRequiredVersion)
	,	m_pfnGetName			( p_plugin.m_pfnGetName)
	,	m_eType					( m_eType)
{
}

PluginBase :: ~PluginBase()
{
}

PluginBase & PluginBase :: operator =( const PluginBase & p_plugin)
{
	m_pfnGetRequiredVersion = p_plugin.m_pfnGetRequiredVersion;
	m_pfnGetName = p_plugin.m_pfnGetName;
	m_eType = p_plugin.m_eType;
	return *this;
}

void PluginBase :: GetRequiredVersion( Version & p_version)const
{
	if (m_pfnGetRequiredVersion)
	{
		m_pfnGetRequiredVersion( p_version);
	}
}

String PluginBase :: GetName()const
{
	String l_strReturn;

	if (m_pfnGetName)
	{
		l_strReturn = m_pfnGetName();
	}

	return l_strReturn;
}

//*************************************************************************************************

RendererPlugin :: RendererPlugin( Utils::DynamicLibraryPtr p_pLibrary)throw( PluginException)
	:	PluginBase( ePLUGIN_TYPE_RENDERER, p_pLibrary)
{
	m_pfnCreateRenderSystem = reinterpret_cast <PCreateRenderSystemFunction> ( p_pLibrary->GetFunction( CreateRenderSystemFunctionABIName));

	if (m_pfnCreateRenderSystem == NULL)
	{
		String l_strError = String( cuT( "Error encountered while loading dll [")) << p_pLibrary->GetPath().GetFileName() << cuT( "] CreateRenderSystem plugin function : ");
		l_strError << dlerror();
		Logger::LogError( l_strError, false);
		CASTOR_PLUGIN_EXCEPTION( "Retrieving RegisterPlugin function");
	}

	m_pfnGetRendererType = reinterpret_cast <PGetRendererTypeFunction> ( p_pLibrary->GetFunction( GetRendererTypeFunctionABIName));

	if (m_pfnGetRendererType == NULL)
	{
		String l_strError = String( cuT( "Error encountered while loading dll [")) << p_pLibrary->GetPath().GetFileName() << cuT( "] GetRendererType plugin function : ");
		l_strError << dlerror();
		Logger::LogError( l_strError, false);
		CASTOR_PLUGIN_EXCEPTION( "Retrieving RegisterPlugin function");
	}
}

RendererPlugin :: RendererPlugin( const RendererPlugin & p_plugin)
	:	PluginBase( p_plugin)
	,	m_pfnCreateRenderSystem( p_plugin.m_pfnCreateRenderSystem)
	,	m_pfnGetRendererType( p_plugin.m_pfnGetRendererType)
{
}

RendererPlugin :: ~RendererPlugin()
{
}

RendererPlugin & RendererPlugin :: operator =( const RendererPlugin & p_plugin)
{
	PluginBase::operator =( p_plugin);
	m_pfnCreateRenderSystem = p_plugin.m_pfnCreateRenderSystem;
	m_pfnGetRendererType = p_plugin.m_pfnGetRendererType;
	return * this;
}

RenderSystem * RendererPlugin :: CreateRenderSystem()
{
	RenderSystem * l_pReturn = NULL;

	if (m_pfnCreateRenderSystem)
	{
		l_pReturn = m_pfnCreateRenderSystem();
	}

	return l_pReturn;
}

eRENDERER_TYPE RendererPlugin :: GetRendererType()
{
	eRENDERER_TYPE l_eReturn = eRENDERER_TYPE_UNDEFINED;

	if (m_pfnGetRendererType)
	{
		l_eReturn = m_pfnGetRendererType();
	}

	return l_eReturn;
}

//*************************************************************************************************

ImporterPlugin :: ImporterPlugin( Utils::DynamicLibraryPtr p_pLibrary)throw( PluginException)
	:	PluginBase( ePLUGIN_TYPE_IMPORTER, p_pLibrary)
{
	m_pfnCreateImporter = reinterpret_cast <PCreateImporterFunction> ( p_pLibrary->GetFunction( CreateImporterABIName));

	if (m_pfnCreateImporter == NULL)
	{
		String l_strError = String( cuT( "Error encountered while loading dll [")) << p_pLibrary->GetPath().GetFileName() << cuT( "] CreateImporter plugin function : ");
		l_strError << dlerror();
		Logger::LogError( l_strError, false);
		CASTOR_PLUGIN_EXCEPTION( "Retrieving CreateImporter function");
	}

	m_pfnGetExtension = reinterpret_cast <PGetExtensionFunction> ( p_pLibrary->GetFunction( GetExtensionFunctionABIName));

	if (m_pfnGetExtension == NULL)
	{
		String l_strError = String( cuT( "Error encountered while loading dll [")) << p_pLibrary->GetPath().GetFileName() << cuT( "] GetExtension plugin function : ");
		l_strError << dlerror();
		Logger::LogError( l_strError, false);
		CASTOR_PLUGIN_EXCEPTION( "Retrieving GetExtension function");
	}
}

ImporterPlugin :: ImporterPlugin( const ImporterPlugin & p_plugin)
	:	PluginBase( p_plugin)
	,	m_pfnCreateImporter( p_plugin.m_pfnCreateImporter)
	,	m_pfnGetExtension( p_plugin.m_pfnGetExtension)
{
}

ImporterPlugin :: ~ImporterPlugin()
{
}

ImporterPlugin & ImporterPlugin :: operator =( const ImporterPlugin & p_plugin)
{
	PluginBase::operator =( p_plugin);
	m_pfnCreateImporter = p_plugin.m_pfnCreateImporter;
	m_pfnGetExtension = p_plugin.m_pfnGetExtension;
	return * this;
}

Importer * ImporterPlugin :: CreateImporter()
{
	Importer * l_pReturn = nullptr;

	if (m_pfnCreateImporter)
	{
		l_pReturn = m_pfnCreateImporter();
	}

	return l_pReturn;
}

String ImporterPlugin :: GetExtension()
{
	String l_strReturn;

	if (m_pfnGetExtension)
	{
		l_strReturn = m_pfnGetExtension();
	}

	return l_strReturn;
}

//*************************************************************************************************

DividerPlugin :: DividerPlugin( Utils::DynamicLibraryPtr p_pLibrary)throw( PluginException)
	:	PluginBase( ePLUGIN_TYPE_DIVIDER, p_pLibrary)
{
	m_pfnCreateDivider = reinterpret_cast <PCreateDividerFunction> ( p_pLibrary->GetFunction( CreateDividerABIName));

	if (m_pfnCreateDivider == NULL)
	{
		String l_strError = String( cuT( "Error encountered while loading dll [")) << p_pLibrary->GetPath().GetFileName() << cuT( "] CreateDivider plugin function : ");
		l_strError << dlerror();
		Logger::LogError( l_strError, false);
		CASTOR_PLUGIN_EXCEPTION( "Retrieving CreateDivider function");
	}
}

DividerPlugin :: DividerPlugin( const DividerPlugin & p_plugin)
	:	PluginBase( p_plugin)
	,	m_pfnCreateDivider( p_plugin.m_pfnCreateDivider)
{
}

DividerPlugin :: ~DividerPlugin()
{
}

DividerPlugin & DividerPlugin :: operator =( const DividerPlugin & p_plugin)
{
	PluginBase::operator =( p_plugin);
	m_pfnCreateDivider = p_plugin.m_pfnCreateDivider;
	return * this;
}

Subdivider * DividerPlugin :: CreateDivider( Submesh * p_pSubmesh)
{
	Subdivider * l_pReturn = nullptr;

	if (m_pfnCreateDivider)
	{
		l_pReturn = m_pfnCreateDivider( p_pSubmesh);
	}

	return l_pReturn;
}

//*************************************************************************************************

GeneratorPlugin :: GeneratorPlugin( Utils::DynamicLibraryPtr p_pLibrary)throw( PluginException)
	:	PluginBase( ePLUGIN_TYPE_DIVIDER, p_pLibrary)
{
	m_pfnCreateGenerator = reinterpret_cast <PCreateGeneratorFunction> ( p_pLibrary->GetFunction( CreateGeneratorABIName));

	if (m_pfnCreateGenerator == NULL)
	{
		String l_strError = String( cuT( "Error encountered while loading dll [")) << p_pLibrary->GetPath().GetFileName() << cuT( "] CreateGenerator plugin function : ");
		l_strError << dlerror();
		Logger::LogError( l_strError, false);
		CASTOR_PLUGIN_EXCEPTION( "Retrieving CreateGenerator function");
	}
}

GeneratorPlugin :: GeneratorPlugin( const GeneratorPlugin & p_plugin)
	:	PluginBase( p_plugin)
	,	m_pfnCreateGenerator( p_plugin.m_pfnCreateGenerator)
{
}

GeneratorPlugin :: ~GeneratorPlugin()
{
}

GeneratorPlugin & GeneratorPlugin :: operator =( const GeneratorPlugin & p_plugin)
{
	PluginBase::operator =( p_plugin);
	m_pfnCreateGenerator = p_plugin.m_pfnCreateGenerator;
	return * this;
}

Subdivider * GeneratorPlugin :: CreateGenerator( TextureUnit * p_pTexture)
{
	Subdivider * l_pReturn = nullptr;

	if (m_pfnCreateGenerator)
	{
		l_pReturn = m_pfnCreateGenerator( p_pTexture);
	}

	return l_pReturn;
}

//*************************************************************************************************
