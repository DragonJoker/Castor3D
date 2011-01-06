#include "GL2RenderSystem/PrecompiledHeader.h"

#include "GL2RenderSystem/GL2RenderSystem.h"
#include "GL2RenderSystem/GL2SubmeshRenderer.h"
#include "GL2RenderSystem/GL2LightRenderer.h"
#include "GL2RenderSystem/GL2MaterialRenderer.h"
#include "GL2RenderSystem/GL2TextureRenderer.h"
#include "GL2RenderSystem/GL2WindowRenderer.h"
#include "GL2RenderSystem/GL2ShaderProgram.h"

using namespace Castor3D;

GL2RenderSystem :: GL2RenderSystem( SceneManager * p_pSceneManager)
	:	GLRenderSystem( p_pSceneManager)
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
			if (m_pCurrentProgram->GetType() == ShaderProgram::eGlShader)
			{
				static_cast <GL2ShaderProgram *>( m_pCurrentProgram)->SetAmbientLight( p_clColour);
			}
			else if (m_pCurrentProgram->GetType() == ShaderProgram::eCgShader)
			{
				static_cast <CgGLShaderProgram *>( m_pCurrentProgram)->SetAmbientLight( p_clColour);
			}
		}
	}
	else
	{
		GLRenderSystem::RenderAmbientLight( p_clColour);
	}
}

SubmeshRendererPtr GL2RenderSystem :: _createSubmeshRenderer()
{
	return SubmeshRendererPtr( new GL2SubmeshRenderer( m_pSceneManager));
}

TextureEnvironmentRendererPtr GL2RenderSystem :: _createTexEnvRenderer()
{
	return TextureEnvironmentRendererPtr( new GLTextureEnvironmentRenderer( m_pSceneManager));
}

TextureRendererPtr GL2RenderSystem :: _createTextureRenderer()
{
	return TextureRendererPtr( new GL2TextureRenderer( m_pSceneManager));
}

PassRendererPtr GL2RenderSystem :: _createPassRenderer()
{
	return PassRendererPtr( new GL2PassRenderer( m_pSceneManager));
}

CameraRendererPtr GL2RenderSystem :: _createCameraRenderer()
{
	return CameraRendererPtr( new GLCameraRenderer( m_pSceneManager));
}

LightRendererPtr GL2RenderSystem :: _createLightRenderer()
{
	return LightRendererPtr( new GL2LightRenderer( m_pSceneManager));
}

WindowRendererPtr GL2RenderSystem :: _createWindowRenderer()
{
	return WindowRendererPtr( new GL2WindowRenderer( m_pSceneManager));
}

OverlayRendererPtr GL2RenderSystem :: _createOverlayRenderer()
{
	return OverlayRendererPtr( new GLOverlayRenderer( m_pSceneManager));
}

IndicesBufferPtr GL2RenderSystem :: _createIndicesBuffer()
{
	IndicesBufferPtr l_pReturn;

	if (UseVBO())
	{
		l_pReturn = BufferManager::CreateBuffer<unsigned int, GLVBOIndicesBuffer>();
	}
	else
	{
		l_pReturn = BufferManager::CreateBuffer<unsigned int, GLVBIndicesBuffer>();
	}

	return l_pReturn;
}

VertexBufferPtr GL2RenderSystem :: _createVertexBuffer()
{
	VertexBufferPtr l_pReturn;

	if (UseVBO())
	{
		l_pReturn = BufferManager::CreateBuffer<real, GLVBOVertexBuffer>();
	}
	else
	{
		l_pReturn = BufferManager::CreateBuffer<real, GLVBVertexBuffer>();
	}

	return l_pReturn;
}

NormalsBufferPtr GL2RenderSystem :: _createNormalsBuffer()
{
	NormalsBufferPtr l_pReturn;

	if (UseVBO())
	{
		l_pReturn = BufferManager::CreateBuffer<real, GLVBONormalsBuffer>();
	}
	else
	{
		l_pReturn = BufferManager::CreateBuffer<real, GLVBNormalsBuffer>();
	}

	return l_pReturn;
}

TextureBufferPtr GL2RenderSystem :: _createTextureBuffer()
{
	TextureBufferPtr l_pReturn;

	if (UseVBO())
	{
		l_pReturn = BufferManager::CreateBuffer<real, GLVBOTextureBuffer>();
	}
	else
	{
		l_pReturn = BufferManager::CreateBuffer<real, GLVBTextureBuffer>();
	}

	return l_pReturn;
}

VertexInfosBufferPtr GL2RenderSystem :: _createVertexInfosBuffer()
{
	VertexInfosBufferPtr l_pReturn;

	if (UseVBO())
	{
		l_pReturn = BufferManager::CreateBuffer<real, GLVertexInfosBufferObject>();
	}

	return l_pReturn;
}

TextureBufferObjectPtr GL2RenderSystem :: _createTBOBuffer()
{
	TextureBufferObjectPtr l_pReturn;
	return l_pReturn;
}
