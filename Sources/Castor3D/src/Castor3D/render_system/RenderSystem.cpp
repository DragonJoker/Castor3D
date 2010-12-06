#include "PrecompiledHeader.h"

#include "render_system/Module_Render.h"

#include "render_system/RenderSystem.h"
#include "material/TextureUnit.h"
#include "material/MaterialManager.h"
#include "main/Pipeline.h"

using namespace Castor3D;

Castor::MultiThreading::RecursiveMutex RenderSystem :: m_mutex;
RenderSystem * RenderSystem :: sm_singleton = NULL;
bool RenderSystem :: sm_useMultiTexturing = false;
bool RenderSystem :: sm_initialised = false;
bool RenderSystem :: sm_useShaders = false;
bool RenderSystem :: sm_forceShaders = false;
bool RenderSystem :: sm_useGeometryShaders = false;

RenderSystem :: RenderSystem()
	:	m_mainContext( NULL),
		m_currentContext( NULL),
		m_pCurrentProgram( NULL)
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
	Pipeline::MatrixMode( Pipeline::eProjection);
	Pipeline::PushMatrix();
	Pipeline::LoadIdentity();
	Pipeline::Ortho( 0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
}

void RenderSystem :: EndOverlaysRendering()
{
	Pipeline::PopMatrix();
	Pipeline::MatrixMode( Pipeline::eModelView);
}

void RenderSystem :: AddContext( Context * p_context, RenderWindow * p_window)
{
	m_contextMap[p_window] = p_context;
}