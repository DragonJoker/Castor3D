#include "RenderSystem.hpp"
#include "CleanupEvent.hpp"
#include "FrameVariableBuffer.hpp"
#include "PointFrameVariable.hpp"
#include "Pipeline.hpp"
#include "Engine.hpp"
#include "Plugin.hpp"
#include "ShaderManager.hpp"
#include "ShaderProgram.hpp"
#include "FrameVariable.hpp"
#include "Submesh.hpp"
#include "Sampler.hpp"
#include "Overlay.hpp"
#include "OverlayRenderer.hpp"
#include "ShaderPlugin.hpp"
#include "Viewport.hpp"

using namespace Castor3D;
using namespace Castor;

//*************************************************************************************************

RenderSystem::RenderSystem( Engine * p_pEngine, eRENDERER_TYPE p_eRendererType )
	: m_pEngine( p_pEngine )
	, m_useMultiTexturing( false )
	, m_bInitialised( false )
	, m_useShaders( false )
	, m_forceShaders( false )
	, m_pPipeline( nullptr )
	, m_eRendererType( p_eRendererType )
	, m_bStereoAvailable( false )
	, m_pCurrentContext( NULL )
	, m_pCurrentCamera( NULL )
	, m_bInstancing( false )
	, m_bAccumBuffer( false )
	, m_bNonPowerOfTwoTextures( false )
{
	m_useShader[eSHADER_TYPE_VERTEX] = false;
	m_useShader[eSHADER_TYPE_PIXEL] = false;
	m_useShader[eSHADER_TYPE_GEOMETRY] = false;
	m_useShader[eSHADER_TYPE_HULL] = false;
	m_useShader[eSHADER_TYPE_DOMAIN] = false;
}

RenderSystem::~RenderSystem()
{
}

void RenderSystem::Initialise()
{
	DoInitialise();
}

void RenderSystem::Cleanup()
{
	DoCleanup();
}

void RenderSystem::RenderAmbientLight( Castor::Colour const & p_clColour, FrameVariableBuffer & p_variableBuffer )
{
	Point4fFrameVariableSPtr l_pVariable;
	p_variableBuffer.GetVariable( ShaderProgramBase::AmbientLight, l_pVariable );
	Point4f l_ptColour;
	p_clColour.to_rgba( l_ptColour );
	l_pVariable->SetValue( l_ptColour );
}

void RenderSystem::PushScene( Scene * p_pScene )
{
	m_stackScenes.push( p_pScene );
}

void RenderSystem::PopScene()
{
	m_stackScenes.pop();
}

Scene * RenderSystem::GetTopScene()
{
	Scene * l_pReturn = NULL;

	if ( m_stackScenes.size() )
	{
		l_pReturn = m_stackScenes.top();
	}

	return l_pReturn;
}

ShaderProgramBaseSPtr RenderSystem::CreateShaderProgram( eSHADER_LANGUAGE p_eLanguage )
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	ShaderProgramBaseSPtr l_pReturn;

	if ( p_eLanguage == eSHADER_LANGUAGE_GLSL || p_eLanguage == eSHADER_LANGUAGE_HLSL )
	{
		l_pReturn = CreateShaderProgram();
	}
	else
	{
		ShaderPluginSPtr l_pPlugin = m_pEngine->GetShaderPlugin( p_eLanguage );

		if ( l_pPlugin )
		{
			l_pReturn = l_pPlugin->CreateShader( this );
		}
	}

	return l_pReturn;
}

IPipelineImpl * RenderSystem::CreatePipeline( Pipeline * p_pPipeline, eSHADER_LANGUAGE p_eLanguage )
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	IPipelineImpl * l_pReturn = NULL;

	if ( p_eLanguage != eSHADER_LANGUAGE_GLSL && p_eLanguage != eSHADER_LANGUAGE_HLSL )
	{
		ShaderPluginSPtr l_pPlugin = m_pEngine->GetShaderPlugin( p_eLanguage );

		if ( l_pPlugin )
		{
			l_pReturn = l_pPlugin->CreatePipeline( p_pPipeline, this );
		}
	}

	return l_pReturn;
}

void RenderSystem::DestroyPipeline( eSHADER_LANGUAGE p_eLanguage, IPipelineImpl * p_pPipeline )
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();

	if ( p_eLanguage != eSHADER_LANGUAGE_GLSL && p_eLanguage != eSHADER_LANGUAGE_HLSL )
	{
		ShaderPluginSPtr l_pPlugin = m_pEngine->GetShaderPlugin( p_eLanguage );

		if ( l_pPlugin )
		{
			l_pPlugin->DestroyPipeline( p_pPipeline );
		}
	}
}

Camera * RenderSystem::GetCurrentCamera()const
{
	return m_pCurrentCamera;
}

void RenderSystem::SetCurrentCamera( Camera * p_pCamera )
{
	m_pCurrentCamera = p_pCamera;
}
