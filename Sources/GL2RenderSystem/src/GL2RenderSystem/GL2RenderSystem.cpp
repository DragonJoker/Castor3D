#include "PrecompiledHeader.h"

#include "GL2RenderSystem.h"
#include "GL2SubmeshRenderer.h"
#include "GL2LightRenderer.h"
#include "GL2MaterialRenderer.h"
#include "GL2TextureRenderer.h"
#include "GL2WindowRenderer.h"
#include "GL2ShaderProgram.h"

using namespace Castor3D;

GL2RenderSystem :: GL2RenderSystem()
	:	GLRenderSystem()
{
	Logger::LogMessage( CU_T( "GL2RenderSystem :: GL2RenderSystem"));
}

GL2RenderSystem :: ~GL2RenderSystem()
{
}

ShaderProgram * GL2RenderSystem :: CreateShaderProgram( const String & p_vertexShaderFile, 
													   const String & p_fragmentShaderFile,
													   const String & p_geometryShaderFile)
{
	return new GL2ShaderProgram( p_vertexShaderFile, p_fragmentShaderFile, p_geometryShaderFile);
}

void GL2RenderSystem :: RenderAmbientLight( const Colour & p_clColour)
{
	if (RenderSystem::UseShaders())
	{
		if (m_pCurrentProgram != NULL)
		{
			static_cast <GL2ShaderProgram *>( m_pCurrentProgram)->SetAmbientLight( p_clColour);
		}
	}
	else
	{
		GLRenderSystem::RenderAmbientLight( p_clColour);
	}
}

SubmeshRendererPtr GL2RenderSystem :: _createSubmeshRenderer()
{
	SubmeshRendererPtr l_renderer( new GL2SubmeshRenderer());
	m_submeshesRenderers.push_back( l_renderer);
	return l_renderer;
}

TextureEnvironmentRendererPtr GL2RenderSystem :: _createTextureEnvironmentRenderer()
{
	TextureEnvironmentRendererPtr l_renderer( new GLTextureEnvironmentRenderer());
	m_texEnvRenderers.push_back( l_renderer);
	return l_renderer;
}

TextureRendererPtr GL2RenderSystem :: _createTextureRenderer()
{
	TextureRendererPtr l_renderer( new GL2TextureRenderer());
	m_textureRenderers.push_back( l_renderer);
	return l_renderer;
}

PassRendererPtr GL2RenderSystem :: _createPassRenderer()
{
	PassRendererPtr l_renderer( new GL2PassRenderer());
	m_passRenderers.push_back( l_renderer);
	return l_renderer;
}

LightRendererPtr GL2RenderSystem :: _createLightRenderer()
{
	LightRendererPtr l_renderer( new GL2LightRenderer());
	m_lightRenderers.push_back( l_renderer);
	return l_renderer;
}

CameraRendererPtr GL2RenderSystem :: _createCameraRenderer()
{
	CameraRendererPtr l_renderer( new GLCameraRenderer());
	m_cameraRenderers.push_back( l_renderer);
	return l_renderer;
}

WindowRendererPtr GL2RenderSystem :: _createWindowRenderer()
{
	WindowRendererPtr l_renderer( new GL2WindowRenderer());
	m_windowRenderers.push_back( l_renderer);
	return l_renderer;
}

OverlayRendererPtr GL2RenderSystem :: _createOverlayRenderer()
{
	OverlayRendererPtr l_renderer( new GLOverlayRenderer());
	m_overlayRenderers.push_back( l_renderer);
	return l_renderer;
}