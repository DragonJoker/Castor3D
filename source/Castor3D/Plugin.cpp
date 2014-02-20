#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Plugin.hpp"
#include "Castor3D/Version.hpp"

using namespace Castor3D;
using namespace Castor;

#pragma warning( disable:4290 )

//*************************************************************************************************

PluginException :: PluginException( std::string const & p_description, char const * p_file, char const * p_function, uint32_t p_line)
	:	Exception( "Plugin loading error : " + p_description, p_file, p_function, p_line)
{
}

PluginException :: ~PluginException()throw()
{
}

//*************************************************************************************************

#if defined( _MSC_VER)
static const String GetNameFunctionABIName				= cuT( "?GetName@@YA?AV?$basic_string@_WU?$char_traits@_W@std@@V?$allocator@_W@2@@std@@XZ");
static const String GetRequiredVersionFunctionABIName	= cuT( "?GetRequiredVersion@@YAXAAVVersion@Castor3D@@@Z");
static const String CreateRenderSystemFunctionABIName	= cuT( "?CreateRenderSystem@@YAPAVRenderSystem@Castor3D@@PAVEngine@2@PAVLogger@Castor@@@Z");
static const String DestroyRenderSystemFunctionABIName	= cuT( "?DestroyRenderSystem@@YAXPAVRenderSystem@Castor3D@@@Z");
static const String GetRendererTypeFunctionABIName		= cuT( "?GetRendererType@@YA?AW4eRENDERER_TYPE@Castor3D@@XZ");
static const String CreateImporterFunctionABIName		= cuT( "?Create@@YAXPAVEngine@Castor3D@@PAVImporterPlugin@2@@Z");
static const String DestroyImporterFunctionABIName		= cuT( "?Destroy@@YAXPAVImporterPlugin@Castor3D@@@Z");
static const String GetExtensionFunctionABIName			= cuT( "?GetExtensions@@YA?AV?$vector@U?$pair@V?$basic_string@_WU?$char_traits@_W@std@@V?$allocator@_W@2@@std@@V12@@std@@V?$allocator@U?$pair@V?$basic_string@_WU?$char_traits@_W@std@@V?$allocator@_W@2@@std@@V12@@std@@@2@@std@@XZ");
static const String GetDividerTypeFunctionABIName		= cuT( "?GetDividerType@@YA?AV?$basic_string@_WU?$char_traits@_W@std@@V?$allocator@_W@2@@std@@XZ");
static const String CreateDividerFunctionABIName		= cuT( "?CreateDivider@@YAPAVSubdivider@Castor3D@@XZ");
static const String DestroyDividerFunctionABIName		= cuT( "?DestroyDivider@@YAXPAVSubdivider@Castor3D@@@Z");
static const String CreateGeneratorFunctionABIName		= cuT( "?CreateGenerator@@YAPAVGenerator@Castor3D@@PAVTextureUnit@2@@Z");
static const String DestroyGeneratorFunctionABIName		= cuT( "?DestroyGenerator@@YAXPAVGenerator@Castor3D@@@Z");
static const String GetShaderLanguageFunctionABIName	= cuT( "?GetShaderLanguage@@YA?AW4eSHADER_LANGUAGE@Castor3D@@XZ");
#if ( _MSC_VER < 1700 )
static const String CreateShaderFunctionABIName			= cuT( "?CreateShader@@YA?AV?$shared_ptr@VShaderProgramBase@Castor3D@@@tr1@std@@PAVRenderSystem@Castor3D@@@Z");
static const String CreateEffectFunctionABIName			= cuT( "?CreateEffect@@YA?AV?$shared_ptr@VPostEffect@Castor3D@@@tr1@std@@PAVRenderSystem@Castor3D@@@Z");
static const String CreateTechniqueFunctionABIName		= cuT( "?CreateTechnique@@YA?AV?$shared_ptr@VRenderTechnique@Castor3D@@@tr1@std@@PAVRenderSystem@Castor3D@@@Z");
#else
static const String CreateShaderFunctionABIName			= cuT( "?CreateShader@@YA?AV?$shared_ptr@VShaderProgramBase@Castor3D@@@std@@PAVRenderSystem@Castor3D@@@Z");
static const String CreateEffectFunctionABIName			= cuT( "?CreateEffect@@YA?AV?$shared_ptr@VPostEffect@Castor3D@@@std@@PAVRenderSystem@Castor3D@@@Z");
static const String CreateTechniqueFunctionABIName		= cuT( "?CreateTechnique@@YA?AV?$shared_ptr@VRenderTechnique@Castor3D@@@std@@PAVRenderSystem@Castor3D@@@Z");
#endif
static const String CreatePipelineFunctionABIName		= cuT( "?CreatePipeline@@YAPAVIPipelineImpl@Castor3D@@PAVPipeline@2@PAVRenderSystem@2@@Z");
static const String DestroyPipelineFunctionABIName		= cuT( "?DestroyPipeline@@YAXPAVIPipelineImpl@Castor3D@@@Z");
#elif defined( __GNUG__)
static const String GetNameFunctionABIName				= cuT( "_Z7GetNamev");
static const String GetRequiredVersionFunctionABIName	= cuT( "_Z18GetRequiredVersionRN8Castor3D7VersionE");
static const String CreateRenderSystemFunctionABIName	= cuT( "_Z18CreateRenderSystemPN8Castor3D6EngineEPN6Castor6LoggerE");
static const String DestroyRenderSystemFunctionABIName	= cuT( "_Z19DestroyRenderSystemPN8Castor3D12RenderSystemE");
static const String GetRendererTypeFunctionABIName		= cuT( "_Z15GetRendererTypev");
static const String CreateImporterFunctionABIName		= cuT( "_Z6CreatePN8Castor3D6EngineEPNS_14ImporterPluginE");
static const String DestroyImporterFunctionABIName		= cuT( "_Z7DestroyPN8Castor3D14ImporterPluginE");
static const String GetExtensionFunctionABIName			= cuT( "_Z13GetExtensionsv");
static const String GetDividerTypeFunctionABIName		= cuT( "_Z14GetDividerTypev");
static const String CreateDividerFunctionABIName		= cuT( "_Z13CreateDividerv");
static const String DestroyDividerFunctionABIName		= cuT( "_Z14DestroyDividerPN8Castor3D10SubdividerE");
static const String CreateGeneratorFunctionABIName		= cuT( "_Z15CreateGeneratorPN8Castor3D11TextureUnitE");
static const String DestroyGeneratorFunctionABIName		= cuT( "_Z16DestroyGeneratorPN8Castor3D9GeneratorE");
static const String GetShaderLanguageFunctionABIName	= cuT( "_Z17GetShaderLanguagev");
static const String CreateShaderFunctionABIName			= cuT( "_Z12CreateShaderPN8Castor3D12RenderSystemE");
static const String CreatePipelineFunctionABIName		= cuT( "_Z14CreatePipelinePN8Castor3D12RenderSystemE");
static const String DestroyPipelineFunctionABIName		= cuT( "_Z15DestroyPipelinePN8Castor3D13IPipelineImplE");
static const String CreateEffectFunctionABIName			= cuT( "_Z12CreateEffectPN8Castor3D12RenderSystemE");
static const String CreateTechniqueFunctionABIName		= cuT( "_Z12CreateTechniquePN8Castor3D12RenderSystemE");
#else
#	error "Implement ABI names for this compiler"
#endif

//*************************************************************************************************

PluginBase :: PluginBase( ePLUGIN_TYPE p_eType, DynamicLibrarySPtr p_pLibrary)throw( PluginException)
	:	m_pfnGetRequiredVersion	( 0			)
	,	m_pfnGetName			( 0			)
	,	m_eType					( p_eType	)
{
	if( !p_pLibrary->GetFunction( m_pfnGetName, GetNameFunctionABIName ) )
	{
		String l_strError = cuT( "Error encountered while loading dll [" ) + p_pLibrary->GetPath().GetFileName() + cuT( "] plugin name function : " );
		l_strError += str_utils::to_string( dlerror() );
		CASTOR_PLUGIN_EXCEPTION( str_utils::to_str( l_strError ) );
	}

	if( !p_pLibrary->GetFunction( m_pfnGetRequiredVersion, GetRequiredVersionFunctionABIName ) )
	{
		String l_strError = cuT( "Error encountered while loading dll [" ) + p_pLibrary->GetPath().GetFileName() + cuT( "] plugin version function : " );
		l_strError += str_utils::to_string( dlerror() );
		CASTOR_PLUGIN_EXCEPTION( str_utils::to_str( l_strError ) );
	}
}

PluginBase :: PluginBase( PluginBase const & p_plugin )
	:	m_pfnGetRequiredVersion	( p_plugin.m_pfnGetRequiredVersion	)
	,	m_pfnGetName			( p_plugin.m_pfnGetName				)
	,	m_eType					( p_plugin.m_eType					)
{
}

PluginBase :: PluginBase( PluginBase && p_plugin)
	:	m_pfnGetRequiredVersion	( std::move( p_plugin.m_pfnGetRequiredVersion	) )
	,	m_pfnGetName			( std::move( p_plugin.m_pfnGetName				) )
	,	m_eType					( std::move( p_plugin.m_eType					) )
{
	p_plugin.m_pfnGetRequiredVersion	= NULL;
	p_plugin.m_pfnGetName				= NULL;
	p_plugin.m_eType					= ePLUGIN_TYPE_COUNT;
}

PluginBase :: ~PluginBase()
{
}

PluginBase & PluginBase :: operator =( PluginBase const & p_plugin)
{
	m_pfnGetRequiredVersion	= p_plugin.m_pfnGetRequiredVersion;
	m_pfnGetName			= p_plugin.m_pfnGetName;
	m_eType					= p_plugin.m_eType;
	return *this;
}

PluginBase & PluginBase :: operator =( PluginBase && p_plugin)
{
	if( this != & p_plugin )
	{
		m_pfnGetRequiredVersion	= std::move( p_plugin.m_pfnGetRequiredVersion	);
		m_pfnGetName			= std::move( p_plugin.m_pfnGetName				);
		m_eType					= std::move( p_plugin.m_eType					);
		p_plugin.m_pfnGetRequiredVersion	= NULL;
		p_plugin.m_pfnGetName				= NULL;
		p_plugin.m_eType					= ePLUGIN_TYPE_COUNT;
	}

	return *this;
}

void PluginBase :: GetRequiredVersion( Version & p_version )const
{
	if( m_pfnGetRequiredVersion )
	{
		m_pfnGetRequiredVersion( p_version );
	}
}

String PluginBase :: GetName()const
{
	String l_strReturn;

	if( m_pfnGetName )
	{
		l_strReturn = m_pfnGetName();
	}

	return l_strReturn;
}

//*************************************************************************************************

RendererPlugin :: RendererPlugin( DynamicLibrarySPtr p_pLibrary )throw( PluginException )
	:	PluginBase	( ePLUGIN_TYPE_RENDERER, p_pLibrary	)
{
	if( !p_pLibrary->GetFunction( m_pfnCreateRenderSystem, CreateRenderSystemFunctionABIName ) )
	{
		String l_strError = cuT( "Error encountered while loading dll [" ) + p_pLibrary->GetPath().GetFileName() + cuT( "] CreateRenderSystem plugin function : " );
		l_strError += str_utils::to_string( dlerror() );
		CASTOR_PLUGIN_EXCEPTION( str_utils::to_str( l_strError ) );
	}

	if( !p_pLibrary->GetFunction( m_pfnDestroyRenderSystem, DestroyRenderSystemFunctionABIName ) )
	{
		String l_strError = cuT( "Error encountered while loading dll [" ) + p_pLibrary->GetPath().GetFileName() + cuT( "] DestroyRenderSystem plugin function : " );
		l_strError += str_utils::to_string( dlerror() );
		CASTOR_PLUGIN_EXCEPTION( str_utils::to_str( l_strError ) );
	}

	if( !p_pLibrary->GetFunction( m_pfnGetRendererType, GetRendererTypeFunctionABIName ) )
	{
		String l_strError = cuT( "Error encountered while loading dll [" ) + p_pLibrary->GetPath().GetFileName() + cuT( "] GetRendererType plugin function : " );
		l_strError += str_utils::to_string( dlerror() );
		CASTOR_PLUGIN_EXCEPTION( str_utils::to_str( l_strError ) );
	}
}

RendererPlugin :: RendererPlugin( RendererPlugin const & p_plugin )
	:	PluginBase					( p_plugin							)
	,	m_pfnCreateRenderSystem		( p_plugin.m_pfnCreateRenderSystem	)
	,	m_pfnDestroyRenderSystem	( p_plugin.m_pfnDestroyRenderSystem	)
	,	m_pfnGetRendererType		( p_plugin.m_pfnGetRendererType		)
{
}

RendererPlugin :: RendererPlugin( RendererPlugin && p_plugin )
	:	PluginBase					( std::move( p_plugin							) )
	,	m_pfnCreateRenderSystem		( std::move( p_plugin.m_pfnCreateRenderSystem	) )
	,	m_pfnDestroyRenderSystem	( std::move( p_plugin.m_pfnDestroyRenderSystem	) )
	,	m_pfnGetRendererType		( std::move( p_plugin.m_pfnGetRendererType		) )
{
	p_plugin.m_pfnCreateRenderSystem	= NULL;
	p_plugin.m_pfnDestroyRenderSystem	= NULL;
	p_plugin.m_pfnGetRendererType		= NULL;
}

RendererPlugin :: ~RendererPlugin()
{
}

RendererPlugin & RendererPlugin :: operator =( RendererPlugin const & p_plugin )
{
	PluginBase::operator =( p_plugin );
	m_pfnCreateRenderSystem		= p_plugin.m_pfnCreateRenderSystem;
	m_pfnDestroyRenderSystem	= p_plugin.m_pfnDestroyRenderSystem;
	m_pfnGetRendererType		= p_plugin.m_pfnGetRendererType;
	return * this;
}

RendererPlugin & RendererPlugin :: operator =( RendererPlugin && p_plugin )
{
	PluginBase::operator =( std::move( p_plugin ) );

	if( this != &p_plugin )
	{
		m_pfnCreateRenderSystem		= std::move( p_plugin.m_pfnCreateRenderSystem	);
		m_pfnDestroyRenderSystem	= std::move( p_plugin.m_pfnDestroyRenderSystem	);
		m_pfnGetRendererType		= std::move( p_plugin.m_pfnGetRendererType		);
		p_plugin.m_pfnCreateRenderSystem	= NULL;
		p_plugin.m_pfnDestroyRenderSystem	= NULL;
		p_plugin.m_pfnGetRendererType		= NULL;
	}

	return * this;
}

RenderSystem * RendererPlugin :: CreateRenderSystem( Engine * p_pEngine, Logger * p_pLogger )
{
	RenderSystem * l_pReturn = NULL;

	if( m_pfnCreateRenderSystem )
	{
		l_pReturn = m_pfnCreateRenderSystem( p_pEngine, p_pLogger );
	}

	return l_pReturn;
}

void RendererPlugin :: DestroyRenderSystem( RenderSystem * p_pRenderSystem )
{
	if( m_pfnDestroyRenderSystem )
	{
		m_pfnDestroyRenderSystem( p_pRenderSystem );
	}
}

eRENDERER_TYPE RendererPlugin :: GetRendererType()
{
	eRENDERER_TYPE l_eReturn = eRENDERER_TYPE_UNDEFINED;

	if( m_pfnGetRendererType )
	{
		l_eReturn = m_pfnGetRendererType();
	}

	return l_eReturn;
}

//*************************************************************************************************

ImporterPlugin :: ImporterPlugin( Engine * p_pEngine, DynamicLibrarySPtr p_pLibrary )throw( PluginException )
	:	PluginBase	( ePLUGIN_TYPE_IMPORTER, p_pLibrary	)
{
	if( !p_pLibrary->GetFunction( m_pfnCreateImporter, CreateImporterFunctionABIName ) )
	{
		String l_strError = cuT( "Error encountered while loading dll [" ) + p_pLibrary->GetPath().GetFileName() + cuT( "] CreateImporter plugin function : " );
		l_strError += str_utils::to_string( dlerror() );
		CASTOR_PLUGIN_EXCEPTION( str_utils::to_str( l_strError ) );
	}

	if( !p_pLibrary->GetFunction( m_pfnDestroyImporter, DestroyImporterFunctionABIName ) )
	{
		String l_strError = cuT( "Error encountered while loading dll [" ) + p_pLibrary->GetPath().GetFileName() + cuT( "] DestroyImporter plugin function : " );
		l_strError += str_utils::to_string( dlerror() );
		CASTOR_PLUGIN_EXCEPTION( str_utils::to_str( l_strError ) );
	}

	if( !p_pLibrary->GetFunction( m_pfnGetExtension, GetExtensionFunctionABIName ) )
	{
		String l_strError = cuT( "Error encountered while loading dll [" ) + p_pLibrary->GetPath().GetFileName() + cuT( "] GetExtension plugin function : " );
		l_strError += str_utils::to_string( dlerror() );
		CASTOR_PLUGIN_EXCEPTION( str_utils::to_str( l_strError ) );
	}
	
	m_pfnCreateImporter( p_pEngine, this );
}

ImporterPlugin :: ImporterPlugin( ImporterPlugin const & p_plugin )
	:	PluginBase				( p_plugin						)
	,	m_pfnCreateImporter		( p_plugin.m_pfnCreateImporter	)
	,	m_pfnDestroyImporter	( p_plugin.m_pfnDestroyImporter	)
	,	m_pfnGetExtension		( p_plugin.m_pfnGetExtension	)
	,	m_pImporter				( p_plugin.m_pImporter			)
{
}

ImporterPlugin :: ImporterPlugin( ImporterPlugin && p_plugin )
	:	PluginBase				( std::move( p_plugin						) )
	,	m_pfnCreateImporter		( std::move( p_plugin.m_pfnCreateImporter	) )
	,	m_pfnDestroyImporter	( std::move( p_plugin.m_pfnDestroyImporter	) )
	,	m_pfnGetExtension		( std::move( p_plugin.m_pfnGetExtension		) )
	,	m_pImporter				( std::move( p_plugin.m_pImporter			) )
{
	p_plugin.m_pfnCreateImporter	= NULL;
	p_plugin.m_pfnDestroyImporter	= NULL;
	p_plugin.m_pfnGetExtension		= NULL;
	p_plugin.m_pImporter			.reset();
}

ImporterPlugin & ImporterPlugin :: operator =( ImporterPlugin const & p_plugin )
{
	PluginBase::operator =( p_plugin );
	m_pfnCreateImporter		= p_plugin.m_pfnCreateImporter;
	m_pfnDestroyImporter	= p_plugin.m_pfnDestroyImporter;
	m_pfnGetExtension		= p_plugin.m_pfnGetExtension;
	m_pImporter				= p_plugin.m_pImporter;
	return * this;
}

ImporterPlugin & ImporterPlugin :: operator =( ImporterPlugin && p_plugin )
{
	PluginBase::operator =( std::move( p_plugin ) );

	if( this != & p_plugin )
	{
		m_pfnCreateImporter		= std::move( p_plugin.m_pfnCreateImporter	);
		m_pfnDestroyImporter	= std::move( p_plugin.m_pfnDestroyImporter	);
		m_pfnGetExtension		= std::move( p_plugin.m_pfnGetExtension		);
		m_pImporter				= std::move( p_plugin.m_pImporter			);
		p_plugin.m_pfnCreateImporter	= NULL;
		p_plugin.m_pfnDestroyImporter	= NULL;
		p_plugin.m_pfnGetExtension		= NULL;
		p_plugin.m_pImporter			.reset();
	}

	return * this;
}

ImporterPlugin :: ~ImporterPlugin()
{
	m_pfnDestroyImporter( this );
}

ImporterPlugin::ExtensionArray ImporterPlugin :: GetExtensions()
{
	ExtensionArray l_arrayReturn;

	if( m_pfnGetExtension )
	{
		l_arrayReturn = m_pfnGetExtension();
	}

	return l_arrayReturn;
}

//*************************************************************************************************

DividerPlugin :: DividerPlugin( DynamicLibrarySPtr p_pLibrary )throw( PluginException )
	:	PluginBase	( ePLUGIN_TYPE_DIVIDER, p_pLibrary )
{
	if( !p_pLibrary->GetFunction( m_pfnCreateDivider, CreateDividerFunctionABIName ) )
	{
		String l_strError = cuT( "Error encountered while loading dll [" ) + p_pLibrary->GetPath().GetFileName() + cuT( "] CreateDivider plugin function : " );
		l_strError += str_utils::to_string( dlerror() );
		CASTOR_PLUGIN_EXCEPTION( str_utils::to_str( l_strError ) );
	}

	if( !p_pLibrary->GetFunction( m_pfnDestroyDivider, DestroyDividerFunctionABIName ) )
	{
		String l_strError = cuT( "Error encountered while loading dll [" ) + p_pLibrary->GetPath().GetFileName() + cuT( "] DestroyDivider plugin function : " );
		l_strError += str_utils::to_string( dlerror() );
		CASTOR_PLUGIN_EXCEPTION( str_utils::to_str( l_strError ) );
	}

	if( !p_pLibrary->GetFunction( m_pfnGetDividerType, GetDividerTypeFunctionABIName ) )
	{
		String l_strError = cuT( "Error encountered while loading dll [" ) + p_pLibrary->GetPath().GetFileName() + cuT( "] GetDividerType plugin function : " );
		l_strError += str_utils::to_string( dlerror() );
		CASTOR_PLUGIN_EXCEPTION( str_utils::to_str( l_strError ) );
	}
}

DividerPlugin :: DividerPlugin( DividerPlugin const & p_plugin )
	:	PluginBase			( p_plugin						)
	,	m_pfnCreateDivider	( p_plugin.m_pfnCreateDivider	)
	,	m_pfnDestroyDivider	( p_plugin.m_pfnDestroyDivider	)
	,	m_pfnGetDividerType	( p_plugin.m_pfnGetDividerType	)
{
}

DividerPlugin :: DividerPlugin( DividerPlugin && p_plugin )
	:	PluginBase			( std::move( p_plugin						) )
	,	m_pfnCreateDivider	( std::move( p_plugin.m_pfnCreateDivider	) )
	,	m_pfnDestroyDivider	( std::move( p_plugin.m_pfnDestroyDivider	) )
	,	m_pfnGetDividerType	( std::move( p_plugin.m_pfnGetDividerType	) )
{
	p_plugin.m_pfnCreateDivider		= NULL;
	p_plugin.m_pfnDestroyDivider	= NULL;
	p_plugin.m_pfnGetDividerType	= NULL;
}

DividerPlugin :: ~DividerPlugin()
{
}

DividerPlugin & DividerPlugin :: operator =( DividerPlugin const & p_plugin )
{
	PluginBase::operator =( p_plugin );
	m_pfnCreateDivider	= p_plugin.m_pfnCreateDivider;
	m_pfnDestroyDivider	= p_plugin.m_pfnDestroyDivider;
	m_pfnGetDividerType	= p_plugin.m_pfnGetDividerType;
	return * this;
}

DividerPlugin & DividerPlugin :: operator =( DividerPlugin && p_plugin )
{
	PluginBase::operator =( std::move( p_plugin ) );

	if( this != & p_plugin )
	{
		m_pfnCreateDivider	= std::move( p_plugin.m_pfnCreateDivider	);
		m_pfnDestroyDivider	= std::move( p_plugin.m_pfnDestroyDivider	);
		m_pfnGetDividerType	= std::move( p_plugin.m_pfnGetDividerType	);
		p_plugin.m_pfnCreateDivider		= NULL;
		p_plugin.m_pfnDestroyDivider	= NULL;
		p_plugin.m_pfnGetDividerType	= NULL;
	}

	return * this;
}

Subdivider * DividerPlugin :: CreateDivider()
{
	Subdivider * l_pReturn = nullptr;

	if( m_pfnCreateDivider )
	{
		l_pReturn = m_pfnCreateDivider();
	}

	return l_pReturn;
}

void DividerPlugin :: DestroyDivider( Subdivider * p_pDivider )
{
	if( m_pfnDestroyDivider )
	{
		m_pfnDestroyDivider( p_pDivider );
	}
}

String DividerPlugin :: GetDividerType()
{
	String l_strReturn;

	if (m_pfnGetDividerType)
	{
		l_strReturn = m_pfnGetDividerType();
	}

	return l_strReturn;
}

//*************************************************************************************************

GeneratorPlugin :: GeneratorPlugin( DynamicLibrarySPtr p_pLibrary )throw( PluginException )
	:	PluginBase	( ePLUGIN_TYPE_DIVIDER, p_pLibrary	)
{
	if( !p_pLibrary->GetFunction( m_pfnCreateGenerator, CreateGeneratorFunctionABIName ) )
	{
		String l_strError = cuT( "Error encountered while loading dll [" ) + p_pLibrary->GetPath().GetFileName() + cuT( "] CreateGenerator plugin function : " );
		l_strError += str_utils::to_string( dlerror() );
		CASTOR_PLUGIN_EXCEPTION( str_utils::to_str( l_strError ) );
	}

	if( !p_pLibrary->GetFunction( m_pfnDestroyGenerator, DestroyGeneratorFunctionABIName ) )
	{
		String l_strError = cuT( "Error encountered while loading dll [" ) + p_pLibrary->GetPath().GetFileName() + cuT( "] DestroyGenerator plugin function : " );
		l_strError += str_utils::to_string( dlerror() );
		CASTOR_PLUGIN_EXCEPTION( str_utils::to_str( l_strError ) );
	}
}

GeneratorPlugin :: GeneratorPlugin( GeneratorPlugin const & p_plugin )
	:	PluginBase				( p_plugin							)
	,	m_pfnCreateGenerator	( p_plugin.m_pfnCreateGenerator		)
	,	m_pfnDestroyGenerator	( p_plugin.m_pfnDestroyGenerator	)
{
}

GeneratorPlugin :: GeneratorPlugin( GeneratorPlugin && p_plugin )
	:	PluginBase				( std::move( p_plugin						) )
	,	m_pfnCreateGenerator	( std::move( p_plugin.m_pfnCreateGenerator	) )
	,	m_pfnDestroyGenerator	( std::move( p_plugin.m_pfnDestroyGenerator	) )
{
	p_plugin.m_pfnCreateGenerator	= NULL;
	p_plugin.m_pfnDestroyGenerator	= NULL;
}

GeneratorPlugin :: ~GeneratorPlugin()
{
}

GeneratorPlugin & GeneratorPlugin :: operator =( GeneratorPlugin const & p_plugin )
{
	PluginBase::operator =( p_plugin );
	m_pfnCreateGenerator	= p_plugin.m_pfnCreateGenerator;
	m_pfnDestroyGenerator	= p_plugin.m_pfnDestroyGenerator;
	return * this;
}

GeneratorPlugin & GeneratorPlugin :: operator =( GeneratorPlugin && p_plugin )
{
	PluginBase::operator =( std::move( p_plugin ) );

	if( this != & p_plugin )
	{
		m_pfnCreateGenerator	= std::move( p_plugin.m_pfnCreateGenerator	);
		m_pfnDestroyGenerator	= std::move( p_plugin.m_pfnDestroyGenerator	);
		p_plugin.m_pfnCreateGenerator	= NULL;
		p_plugin.m_pfnDestroyGenerator	= NULL;
	}

	return * this;
}

Subdivider * GeneratorPlugin :: CreateGenerator( TextureUnit * p_pTexture)
{
	Subdivider * l_pReturn = nullptr;

	if( m_pfnCreateGenerator )
	{
		l_pReturn = m_pfnCreateGenerator( p_pTexture );
	}

	return l_pReturn;
}

void GeneratorPlugin :: DestroyGenerator( Subdivider * p_pGenerator )
{
	if( m_pfnDestroyGenerator )
	{
		m_pfnDestroyGenerator( p_pGenerator );
	}
}

//*************************************************************************************************

ShaderPlugin :: ShaderPlugin( DynamicLibrarySPtr p_pLibrary )throw( PluginException )
	:	PluginBase	( ePLUGIN_TYPE_PROGRAM, p_pLibrary	)
{
	if( !p_pLibrary->GetFunction( m_pfnCreateShader, CreateShaderFunctionABIName ) )
	{
		String l_strError = cuT( "Error encountered while loading dll [" ) + p_pLibrary->GetPath().GetFileName() + cuT( "] CreateShader plugin function : " );
		l_strError += str_utils::to_string( dlerror() );
		CASTOR_PLUGIN_EXCEPTION( str_utils::to_str( l_strError ) );
	}

	if( !p_pLibrary->GetFunction( m_pfnCreatePipeline, CreatePipelineFunctionABIName ) )
	{
		String l_strError = cuT( "Error encountered while loading dll [" ) + p_pLibrary->GetPath().GetFileName() + cuT( "] CreatePipeline plugin function : " );
		l_strError += str_utils::to_string( dlerror() );
		CASTOR_PLUGIN_EXCEPTION( str_utils::to_str( l_strError ) );
	}

	if( !p_pLibrary->GetFunction( m_pfnDestroyPipeline, DestroyPipelineFunctionABIName ) )
	{
		String l_strError = cuT( "Error encountered while loading dll [" ) + p_pLibrary->GetPath().GetFileName() + cuT( "] DestroyPipeline plugin function : " );
		l_strError += str_utils::to_string( dlerror() );
		CASTOR_PLUGIN_EXCEPTION( str_utils::to_str( l_strError ) );
	}

	if( !p_pLibrary->GetFunction( m_pfnGetShaderLanguage, GetShaderLanguageFunctionABIName ) )
	{
		String l_strError = cuT( "Error encountered while loading dll [" ) + p_pLibrary->GetPath().GetFileName() + cuT( "] GetShaderLanguage plugin function : " );
		l_strError += str_utils::to_string( dlerror() );
		CASTOR_PLUGIN_EXCEPTION( str_utils::to_str( l_strError ) );
	}
}

ShaderPlugin :: ShaderPlugin( ShaderPlugin const & p_plugin )
	:	PluginBase				( p_plugin							)
	,	m_pfnCreateShader		( p_plugin.m_pfnCreateShader		)
	,	m_pfnCreatePipeline		( p_plugin.m_pfnCreatePipeline		)
	,	m_pfnDestroyPipeline	( p_plugin.m_pfnDestroyPipeline		)
	,	m_pfnGetShaderLanguage	( p_plugin.m_pfnGetShaderLanguage	)
{
}

ShaderPlugin :: ShaderPlugin( ShaderPlugin && p_plugin )
	:	PluginBase				( std::move( p_plugin							) )
	,	m_pfnCreateShader		( std::move( p_plugin.m_pfnCreateShader			) )
	,	m_pfnCreatePipeline		( std::move( p_plugin.m_pfnCreatePipeline		) )
	,	m_pfnDestroyPipeline	( std::move( p_plugin.m_pfnDestroyPipeline		) )
	,	m_pfnGetShaderLanguage	( std::move( p_plugin.m_pfnGetShaderLanguage	) )
{
	p_plugin.m_pfnCreateShader		= NULL;
	p_plugin.m_pfnCreatePipeline	= NULL;
	p_plugin.m_pfnDestroyPipeline	= NULL;
	p_plugin.m_pfnGetShaderLanguage	= NULL;
}

ShaderPlugin :: ~ShaderPlugin()
{
}

ShaderPlugin & ShaderPlugin :: operator =( ShaderPlugin const & p_plugin )
{
	PluginBase::operator =( p_plugin );
	m_pfnCreateShader		= p_plugin.m_pfnCreateShader;
	m_pfnGetShaderLanguage	= p_plugin.m_pfnGetShaderLanguage;
	m_pfnCreatePipeline		= p_plugin.m_pfnCreatePipeline;
	m_pfnDestroyPipeline	= p_plugin.m_pfnDestroyPipeline;
	return * this;
}

ShaderPlugin & ShaderPlugin :: operator =( ShaderPlugin && p_plugin )
{
	PluginBase::operator =( std::move( p_plugin ) );

	if( this != & p_plugin )
	{
		m_pfnCreateShader		= std::move( p_plugin.m_pfnCreateShader			);
		m_pfnGetShaderLanguage	= std::move( p_plugin.m_pfnGetShaderLanguage	);
		m_pfnCreatePipeline		= std::move( p_plugin.m_pfnCreatePipeline		);
		m_pfnDestroyPipeline	= std::move( p_plugin.m_pfnDestroyPipeline		);
		p_plugin.m_pfnCreateShader		= NULL;
		p_plugin.m_pfnCreatePipeline	= NULL;
		p_plugin.m_pfnDestroyPipeline	= NULL;
		p_plugin.m_pfnGetShaderLanguage	= NULL;
	}

	return * this;
}

ShaderProgramBaseSPtr ShaderPlugin :: CreateShader( RenderSystem * p_pRenderSystem )
{
	ShaderProgramBaseSPtr l_pReturn;

	if( m_pfnCreateShader )
	{
		l_pReturn = m_pfnCreateShader( p_pRenderSystem );
	}

	return l_pReturn;
}

IPipelineImpl * ShaderPlugin :: CreatePipeline( Pipeline * p_pPipeline, RenderSystem * p_pRenderSystem )
{
	IPipelineImpl * l_pReturn = NULL;

	if( m_pfnCreateShader )
	{
		l_pReturn = m_pfnCreatePipeline( p_pPipeline, p_pRenderSystem );
	}

	return l_pReturn;
}

void ShaderPlugin :: DestroyPipeline( IPipelineImpl * p_pPipeline )
{
	if( m_pfnDestroyPipeline )
	{
		m_pfnDestroyPipeline( p_pPipeline );
	}
}

eSHADER_LANGUAGE ShaderPlugin :: GetShaderLanguage()
{
	eSHADER_LANGUAGE l_eReturn = eSHADER_LANGUAGE_COUNT;

	if( m_pfnGetShaderLanguage )
	{
		l_eReturn = m_pfnGetShaderLanguage();
	}

	return l_eReturn;
}

//*************************************************************************************************

PostFxPlugin :: PostFxPlugin( DynamicLibrarySPtr p_pLibrary )throw( PluginException )
	:	PluginBase	( ePLUGIN_TYPE_POSTFX, p_pLibrary	)
{
	if( !p_pLibrary->GetFunction( m_pfnCreateEffect, CreateEffectFunctionABIName ) )
	{
		String l_strError = cuT( "Error encountered while loading dll [" ) + p_pLibrary->GetPath().GetFileName() + cuT( "] CreateEffect plugin function : " );
		l_strError += str_utils::to_string( dlerror() );
		CASTOR_PLUGIN_EXCEPTION( str_utils::to_str( l_strError ) );
	}
}

PostFxPlugin :: PostFxPlugin( PostFxPlugin const & p_plugin )
	:	PluginBase				( p_plugin							)
	,	m_pfnCreateEffect		( p_plugin.m_pfnCreateEffect		)
{
}

PostFxPlugin :: PostFxPlugin( PostFxPlugin && p_plugin )
	:	PluginBase				( std::move( p_plugin					) )
	,	m_pfnCreateEffect		( std::move( p_plugin.m_pfnCreateEffect	) )
{
	p_plugin.m_pfnCreateEffect	= NULL;
}

PostFxPlugin :: ~PostFxPlugin()
{
}

PostFxPlugin & PostFxPlugin :: operator =( PostFxPlugin const & p_plugin )
{
	PluginBase::operator =( p_plugin );
	m_pfnCreateEffect		= p_plugin.m_pfnCreateEffect;
	return * this;
}

PostFxPlugin & PostFxPlugin :: operator =( PostFxPlugin && p_plugin )
{
	PluginBase::operator =( std::move( p_plugin ) );

	if( this != & p_plugin )
	{
		m_pfnCreateEffect			= std::move( p_plugin.m_pfnCreateEffect );
		p_plugin.m_pfnCreateEffect	= NULL;
	}

	return * this;
}

PostEffectSPtr PostFxPlugin :: CreateEffect( RenderSystem * p_pRenderSystem )
{
	PostEffectSPtr l_pReturn;

	if( m_pfnCreateEffect )
	{
		l_pReturn = m_pfnCreateEffect( p_pRenderSystem );
	}

	return l_pReturn;
}

//*************************************************************************************************

TechniquePlugin :: TechniquePlugin( DynamicLibrarySPtr p_pLibrary )throw( PluginException )
	:	PluginBase	( ePLUGIN_TYPE_TECHNIQUE, p_pLibrary	)
{
	if( !p_pLibrary->GetFunction( m_pfnCreateTechnique, CreateTechniqueFunctionABIName ) )
	{
		String l_strError = cuT( "Error encountered while loading dll [" ) + p_pLibrary->GetPath().GetFileName() + cuT( "] CreateTechnique plugin function : " );
		l_strError += str_utils::to_string( dlerror() );
		CASTOR_PLUGIN_EXCEPTION( str_utils::to_str( l_strError ) );
	}
}

TechniquePlugin :: TechniquePlugin( TechniquePlugin const & p_plugin )
	:	PluginBase				( p_plugin						)
	,	m_pfnCreateTechnique	( p_plugin.m_pfnCreateTechnique	)
{
}

TechniquePlugin :: TechniquePlugin( TechniquePlugin && p_plugin )
	:	PluginBase				( std::move( p_plugin						) )
	,	m_pfnCreateTechnique	( std::move( p_plugin.m_pfnCreateTechnique	) )
{
	p_plugin.m_pfnCreateTechnique	= NULL;
}

TechniquePlugin :: ~TechniquePlugin()
{
}

TechniquePlugin & TechniquePlugin :: operator =( TechniquePlugin const & p_plugin )
{
	PluginBase::operator =( p_plugin );
	m_pfnCreateTechnique		= p_plugin.m_pfnCreateTechnique;
	return * this;
}

TechniquePlugin & TechniquePlugin :: operator =( TechniquePlugin && p_plugin )
{
	PluginBase::operator =( std::move( p_plugin ) );

	if( this != & p_plugin )
	{
		m_pfnCreateTechnique			= std::move( p_plugin.m_pfnCreateTechnique );
		p_plugin.m_pfnCreateTechnique	= NULL;
	}

	return * this;
}

RenderTechniqueBaseSPtr TechniquePlugin :: CreateTechnique( RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & p_params )
{
	RenderTechniqueBaseSPtr l_pReturn;

	if( m_pfnCreateTechnique )
	{
		l_pReturn = m_pfnCreateTechnique( p_renderTarget, p_pRenderSystem, p_params );
	}

	return l_pReturn;
}

//*************************************************************************************************
