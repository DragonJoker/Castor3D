#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/render_system/RenderSystem.h"
#include "Castor3D/material/TextureUnit.h"
#include "Castor3D/material/MaterialManager.h"
#include "Castor3D/scene/SceneManager.h"
#include "Castor3D/shader/ShaderManager.h"
#include "Castor3D/main/Pipeline.h"

using namespace Castor3D;

Castor::MultiThreading::RecursiveMutex RenderSystem :: m_mutex;
RenderSystem * RenderSystem :: sm_singleton = NULL;
bool RenderSystem :: sm_useMultiTexturing = false;
bool RenderSystem :: sm_initialised = false;
bool RenderSystem :: sm_useShaders = false;
bool RenderSystem :: sm_forceShaders = false;
bool RenderSystem :: sm_useGeometryShaders = false;

RenderSystem :: RenderSystem( SceneManager * p_pSceneManager)
	:	m_mainContext( NULL)
	,	m_currentContext( NULL)
	,	m_pCurrentProgram( NULL)
	,	m_pSceneManager( p_pSceneManager)
{
	sm_singleton = this;
}

RenderSystem :: ~RenderSystem()
{
	sm_singleton = NULL;
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

void RenderSystem :: ApplyTransformations( const Point3r & p_position, real * p_matrix)
{
	Pipeline::PushMatrix();
	Pipeline::Translate( p_position);
	Pipeline::MultMatrix( p_matrix);
}

void RenderSystem :: ApplyTransformations( const Point3r & p_position, const Matrix4x4r & p_matrix)
{
	Pipeline::PushMatrix();
	Pipeline::Translate( p_position);
	Pipeline::MultMatrix( p_matrix);
}

void RenderSystem :: RemoveTransformations()
{
	Pipeline::PopMatrix();
}

void RenderSystem :: BeginOverlaysRendering()
{
	Pipeline::MatrixMode( Pipeline::eMatrixProjection);
	Pipeline::PushMatrix();
	Pipeline::LoadIdentity();
	Pipeline::Ortho( 0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
}

void RenderSystem :: EndOverlaysRendering()
{
	Pipeline::PopMatrix();
	Pipeline::MatrixMode( Pipeline::eMatrixModelView);
}

void RenderSystem :: AddContext( Context * p_context, RenderWindow * p_window)
{
	m_contextMap[p_window] = p_context;
}

IndexBufferPtr RenderSystem :: CreateIndexBuffer()
{
	IndexBufferPtr l_pReturn;

	if (sm_singleton != NULL)
	{
		l_pReturn = sm_singleton->_createIndexBuffer();
	}

	return l_pReturn;
}

VertexBufferPtr RenderSystem :: CreateVertexBuffer( const BufferElementDeclaration * p_pElements, size_t p_uiCount)
{
	VertexBufferPtr l_pReturn;

	if (sm_singleton != NULL)
	{
		l_pReturn = sm_singleton->_createVertexBuffer( p_pElements, p_uiCount);
	}

	return l_pReturn;
}

TextureBufferObjectPtr RenderSystem :: CreateTextureBuffer()
{
	TextureBufferObjectPtr l_pReturn;

	if (sm_singleton != NULL)
	{
		l_pReturn = sm_singleton->_createTextureBuffer();
	}

	return l_pReturn;
}

void RenderSystem :: _addGlslShaderProgram( GlslShaderProgramPtr p_pToAdd)
{
	m_pSceneManager->GetMaterialManager()->GetShaderManager()->AddGLSLProgram( p_pToAdd);
}

void RenderSystem :: _addHlslShaderProgram( HlslShaderProgramPtr p_pToAdd)
{
	m_pSceneManager->GetMaterialManager()->GetShaderManager()->AddHLSLProgram( p_pToAdd);
}

void RenderSystem :: _addCgShaderProgram( CgShaderProgramPtr p_pToAdd)
{
	m_pSceneManager->GetMaterialManager()->GetShaderManager()->AddCgProgram( p_pToAdd);
}

VertexAttribsBufferBoolPtr RenderSystem :: _create1BoolVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<bool>( p_strArg);
}

VertexAttribsBufferIntPtr RenderSystem :: _create1IntVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<int>( p_strArg);
}

VertexAttribsBufferUIntPtr RenderSystem :: _create1UIntVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<unsigned int>( p_strArg);
}

VertexAttribsBufferFloatPtr RenderSystem :: _create1FloatVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<float>( p_strArg);
}

VertexAttribsBufferDoublePtr RenderSystem :: _create1DoubleVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<double>( p_strArg);
}

VertexAttribsBufferBoolPtr RenderSystem :: _create2BoolVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<bool>( p_strArg);
}

VertexAttribsBufferIntPtr RenderSystem :: _create2IntVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<int>( p_strArg);
}

VertexAttribsBufferUIntPtr RenderSystem :: _create2UIntVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<unsigned int>( p_strArg);
}

VertexAttribsBufferFloatPtr RenderSystem :: _create2FloatVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<float>( p_strArg);
}

VertexAttribsBufferDoublePtr RenderSystem :: _create2DoubleVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<double>( p_strArg);
}

VertexAttribsBufferBoolPtr RenderSystem :: _create3BoolVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<bool>( p_strArg);
}

VertexAttribsBufferIntPtr RenderSystem :: _create3IntVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<int>( p_strArg);
}

VertexAttribsBufferUIntPtr RenderSystem :: _create3UIntVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<unsigned int>( p_strArg);
}

VertexAttribsBufferFloatPtr RenderSystem :: _create3FloatVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<float>( p_strArg);
}

VertexAttribsBufferDoublePtr RenderSystem :: _create3DoubleVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<double>( p_strArg);
}

VertexAttribsBufferBoolPtr RenderSystem :: _create4BoolVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<bool>( p_strArg);
}

VertexAttribsBufferIntPtr RenderSystem :: _create4IntVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<int>( p_strArg);
}

VertexAttribsBufferUIntPtr RenderSystem :: _create4UIntVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<unsigned int>( p_strArg);
}

VertexAttribsBufferFloatPtr RenderSystem :: _create4FloatVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<float>( p_strArg);
}

VertexAttribsBufferDoublePtr RenderSystem :: _create4DoubleVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<double>( p_strArg);
}
