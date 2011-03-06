#include "Gl2RenderSystem/PrecompiledHeader.h"

#include "Gl2RenderSystem/Gl2RenderSystem.h"
#include "Gl2RenderSystem/Gl2SubmeshRenderer.h"
#include "Gl2RenderSystem/Gl2LightRenderer.h"
#include "Gl2RenderSystem/Gl2MaterialRenderer.h"
#include "Gl2RenderSystem/Gl2TextureRenderer.h"
#include "Gl2RenderSystem/Gl2WindowRenderer.h"
#include "Gl2RenderSystem/Gl2ShaderProgram.h"

using namespace Castor3D;

Gl2RenderSystem :: Gl2RenderSystem( SceneManager * p_pSceneManager)
	:	GlRenderSystem( p_pSceneManager)
{
	Logger::LogMessage( CU_T( "Gl2RenderSystem :: Gl2RenderSystem"));
}

Gl2RenderSystem :: ~Gl2RenderSystem()
{
}

void Gl2RenderSystem :: RenderAmbientLight( const Colour & p_clColour)
{
	if (RenderSystem::UseShaders())
	{
		if (m_pCurrentProgram != NULL)
		{
			if (m_pCurrentProgram->GetType() == ShaderProgramBase::eGlslShader)
			{
				static_cast <Gl2ShaderProgram *>( m_pCurrentProgram)->SetAmbientLight( p_clColour);
			}
			else if (m_pCurrentProgram->GetType() == ShaderProgramBase::eCgShader)
			{
				static_cast <CgGlShaderProgram *>( m_pCurrentProgram)->SetAmbientLight( p_clColour);
			}
		}
	}
	else
	{
		GlRenderSystem::RenderAmbientLight( p_clColour);
	}
}

GlslShaderProgramPtr Gl2RenderSystem :: _createGlslShaderProgram( const String & p_vertexShaderFile, 
																  const String & p_fragmentShaderFile,
																  const String & p_geometryShaderFile)
{
	return GlslShaderProgramPtr( new Gl2ShaderProgram( p_vertexShaderFile, p_fragmentShaderFile, p_geometryShaderFile));
}

SubmeshRendererPtr Gl2RenderSystem :: _createSubmeshRenderer()
{
	return SubmeshRendererPtr( new Gl2SubmeshRenderer( m_pSceneManager));
}

TextureEnvironmentRendererPtr Gl2RenderSystem :: _createTexEnvRenderer()
{
	return TextureEnvironmentRendererPtr( new GlTextureEnvironmentRenderer( m_pSceneManager));
}

TextureRendererPtr Gl2RenderSystem :: _createTextureRenderer()
{
	return TextureRendererPtr( new Gl2TextureRenderer( m_pSceneManager));
}

PassRendererPtr Gl2RenderSystem :: _createPassRenderer()
{
	return PassRendererPtr( new Gl2PassRenderer( m_pSceneManager));
}

CameraRendererPtr Gl2RenderSystem :: _createCameraRenderer()
{
	return CameraRendererPtr( new GlCameraRenderer( m_pSceneManager));
}

LightRendererPtr Gl2RenderSystem :: _createLightRenderer()
{
	return LightRendererPtr( new Gl2LightRenderer( m_pSceneManager));
}

WindowRendererPtr Gl2RenderSystem :: _createWindowRenderer()
{
	return WindowRendererPtr( new Gl2WindowRenderer( m_pSceneManager));
}

OverlayRendererPtr Gl2RenderSystem :: _createOverlayRenderer()
{
	return OverlayRendererPtr( new GlOverlayRenderer( m_pSceneManager));
}

IndexBufferPtr Gl2RenderSystem :: _createIndexBuffer()
{
	IndexBufferPtr l_pReturn;

	if (UseVertexBufferObjects())
	{
		l_pReturn = BufferManager::CreateBuffer<unsigned int, GlVboIndexBuffer>();
	}
	else
	{
		l_pReturn = BufferManager::CreateBuffer<unsigned int, GlVbIndexBuffer>();
	}

	return l_pReturn;
}

VertexBufferPtr Gl2RenderSystem :: _createVertexBuffer( const BufferElementDeclaration * p_pElements, size_t p_uiCount)
{
	VertexBufferPtr l_pReturn;

	if (UseVertexBufferObjects())
	{
		l_pReturn = BufferManager::CreateVertexBuffer<GlVboVertexBuffer>( p_pElements, p_uiCount);
	}
	else
	{
		l_pReturn = BufferManager::CreateVertexBuffer<GlVbVertexBuffer>( p_pElements, p_uiCount);
	}

	return l_pReturn;
}

TextureBufferObjectPtr Gl2RenderSystem :: _createTextureBuffer()
{
	TextureBufferObjectPtr l_pReturn;
	return l_pReturn;
}
