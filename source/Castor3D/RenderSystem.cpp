#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/RenderSystem.hpp"
#include "Castor3D/TextureUnit.hpp"
#include "Castor3D/ShaderManager.hpp"
#include "Castor3D/Pipeline.hpp"

using namespace Castor3D;
using namespace Castor::MultiThreading;

RecursiveMutex		RenderSystem :: m_mutex;
RenderSystem	*	RenderSystem :: sm_singleton					= nullptr;
bool				RenderSystem :: sm_useMultiTexturing			= false;
bool				RenderSystem :: sm_initialised					= false;
bool				RenderSystem :: sm_useShaders					= false;
bool				RenderSystem :: sm_forceShaders					= false;
bool				RenderSystem :: sm_useShader[eSHADER_TYPE_COUNT]	= { false, false, false, false, false };

RenderSystem :: RenderSystem()
	:	m_mainContext( nullptr)
	,	m_currentContext( nullptr)
	,	m_pCurrentProgram( nullptr)
	,	m_cgContext( nullptr)
{
	sm_singleton = this;
}

RenderSystem :: ~RenderSystem()
{
	sm_singleton = nullptr;
}

void RenderSystem :: CleanupRenderers()
{
	for (size_t i = 0 ; i < m_textureRenderers.size() ; i++)
	{
		m_textureRenderersToCleanup.push_back( m_textureRenderers[i]);
	}
}

void RenderSystem :: CleanupRenderersToCleanup()
{
	for (size_t i = 0 ; i < m_textureRenderersToCleanup.size() ; i++)
	{
		m_textureRenderersToCleanup[i]->Cleanup();
	}
}

void RenderSystem :: BeginOverlaysRendering()
{
	CASTOR_TRACK;
	Pipeline::MatrixMode( Pipeline::eMODE_PROJECTION);
	Pipeline::PushMatrix();
	Pipeline::LoadIdentity();
	Pipeline::Ortho( 0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
}

void RenderSystem :: EndOverlaysRendering()
{
	CASTOR_TRACK;
	Pipeline::PopMatrix();
	Pipeline::MatrixMode( Pipeline::eMODE_MODELVIEW);
}

void RenderSystem :: AddContext( Context * p_context, RenderWindow * p_window)
{
	m_contextMap[p_window] = p_context;
}

IndexBufferPtr RenderSystem :: CreateIndexBuffer()
{
	IndexBufferPtr l_pReturn;

	if (sm_singleton)
	{
		l_pReturn = sm_singleton->_createIndexBuffer();
	}

	return l_pReturn;
}

VertexBufferPtr RenderSystem :: CreateVertexBuffer( const BufferElementDeclaration * p_pElements, size_t p_uiCount)
{
	VertexBufferPtr l_pReturn;

	if (sm_singleton)
	{
		l_pReturn = sm_singleton->_createVertexBuffer( p_pElements, p_uiCount);
	}

	return l_pReturn;
}

TextureBufferObjectPtr RenderSystem :: CreateTextureBuffer()
{
	TextureBufferObjectPtr l_pReturn;

	if (sm_singleton)
	{
		l_pReturn = sm_singleton->_createTextureBuffer();
	}

	return l_pReturn;
}

void RenderSystem :: _addShaderProgram( ShaderProgramPtr p_pToAdd)
{
	ShaderManager l_shaderManager;
	l_shaderManager.AddProgram( p_pToAdd);
}