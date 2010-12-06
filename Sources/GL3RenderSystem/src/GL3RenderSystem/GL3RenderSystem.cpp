#include "PrecompiledHeader.h"

#include "GL3RenderSystem.h"
#include "GL3SubmeshRenderer.h"
#include "GL3LightRenderer.h"
#include "GL3MaterialRenderer.h"
#include "GL3TextureRenderer.h"
#include "GL3WindowRenderer.h"
#include "GL3Context.h"
#include "GL3ShaderProgram.h"

using namespace Castor3D;

GL3RenderSystem :: GL3RenderSystem()
	:	GLRenderSystem()
{
	Logger::LogMessage( CU_T( "GL3RenderSystem :: GL3RenderSystem"));
}

GL3RenderSystem :: ~GL3RenderSystem()
{
	Delete();
}

void GL3RenderSystem :: Initialise()
{
	Logger::LogMessage( CU_T( "GL3RenderSystem :: Initialise"));
	if (sm_initialised)
	{
		return;
	}

	Logger::LogMessage( CU_T( "****************************************"));
	Logger::LogMessage( CU_T( "Initialising OpenGL"));

	InitOpenGLExtensions();

	sm_useMultiTexturing = true;
	sm_useVBO = true;
	sm_forceShaders = true;

	Logger::LogMessage( CU_T( "OpenGL Initialisation Ended"));
	Logger::LogMessage( CU_T( "****************************************"));
	sm_initialised = true;

	GLPipeline::InitFunctions();
}

void GL3RenderSystem :: SetCurrentShaderProgram( ShaderProgram * p_pShader)
{
	if (m_pCurrentProgram != p_pShader)
	{
		RenderSystem::SetCurrentShaderProgram( p_pShader);
		GL3ShaderProgram * l_pCurrentProgram = (GL3ShaderProgram *)p_pShader;

		if (m_pCurrentProgram != NULL)
		{
			for (size_t i = 0 ; i < m_lightRenderers.size() ; i++)
			{
				m_lightRenderers[i]->Initialise();
			}
		}

		GLPipeline::InitFunctions();
	}
}

int GL3RenderSystem :: LockLight()
{
	int l_iReturn = -1;

	if (m_setAvailableIndexes.size() > 0)
	{
		l_iReturn = *m_setAvailableIndexes.begin();
		m_setAvailableIndexes.erase( m_setAvailableIndexes.begin());
	}

	return l_iReturn;
}

void GL3RenderSystem :: UnlockLight( int p_iIndex)
{
	if (p_iIndex >= 0 && m_setAvailableIndexes.find( p_iIndex) == m_setAvailableIndexes.end())
	{
		m_setAvailableIndexes.insert( std::set <int>::value_type( p_iIndex));
	}
}

void GL3RenderSystem :: DisplayArc( eDRAW_TYPE p_displayMode,
								  const Point3rPtrList & p_vertex)
{
	//On selectionne le type d'affichage necessaire
	glBegin( GetDrawType( p_displayMode));
	Point3rPtr l_vertex;

	for (Point3rPtrList::const_iterator l_it = p_vertex.begin() ; l_it != p_vertex.end() ; ++l_it)
	{
		l_vertex = *l_it;
		glVertex3( l_vertex->m_coords[0], l_vertex->m_coords[1], l_vertex->m_coords[2]);
	}

	glEnd();
}

ShaderProgram * GL3RenderSystem :: CreateShaderProgram( const String & p_vertexShaderFile, 
													   const String & p_fragmentShaderFile,
													   const String & p_geometryShaderFile)
{
	return new GL3ShaderProgram( p_vertexShaderFile, p_fragmentShaderFile, p_geometryShaderFile);
}

void GL3RenderSystem :: BeginOverlaysRendering()
{
	RenderSystem::BeginOverlaysRendering();
	glDisable( GL_DEPTH_TEST);
	CheckGLError( CU_T( "GL3RenderSystem :: BeginOverlaysRendering - glDisable( GL_DEPTH_TEST)"));
}

void GL3RenderSystem :: RenderAmbientLight( const Colour & p_clColour)
{
	if (m_pCurrentProgram != NULL)
	{
		static_cast <GL3ShaderProgram *>( m_pCurrentProgram)->SetAmbientLight( p_clColour);
	}
}

SubmeshRendererPtr GL3RenderSystem :: _createSubmeshRenderer()
{
	SubmeshRendererPtr l_renderer( new GL3SubmeshRenderer());
	m_submeshesRenderers.push_back( l_renderer);
	return l_renderer;
}

TextureEnvironmentRendererPtr GL3RenderSystem :: _createTextureEnvironmentRenderer()
{
	TextureEnvironmentRendererPtr l_renderer( new GLTextureEnvironmentRenderer());
	m_texEnvRenderers.push_back( l_renderer);
	return l_renderer;
}

TextureRendererPtr GL3RenderSystem :: _createTextureRenderer()
{
	TextureRendererPtr l_renderer( new GL3TextureRenderer());
	m_textureRenderers.push_back( l_renderer);
	return l_renderer;
}

PassRendererPtr GL3RenderSystem :: _createPassRenderer()
{
	PassRendererPtr l_renderer( new GL3PassRenderer());
	m_passRenderers.push_back( l_renderer);
	return l_renderer;
}

LightRendererPtr GL3RenderSystem :: _createLightRenderer()
{
	LightRendererPtr l_renderer( new GL3LightRenderer());
	m_lightRenderers.push_back( l_renderer);
	return l_renderer;
}

CameraRendererPtr GL3RenderSystem :: _createCameraRenderer()
{
	CameraRendererPtr l_renderer( new GLCameraRenderer());
	m_cameraRenderers.push_back( l_renderer);
	return l_renderer;
}

WindowRendererPtr GL3RenderSystem :: _createWindowRenderer()
{
	WindowRendererPtr l_renderer( new GL3WindowRenderer());
	m_windowRenderers.push_back( l_renderer);
	return l_renderer;
}

OverlayRendererPtr GL3RenderSystem :: _createOverlayRenderer()
{
	OverlayRendererPtr l_renderer( new GLOverlayRenderer());
	m_overlayRenderers.push_back( l_renderer);
	return l_renderer;
}