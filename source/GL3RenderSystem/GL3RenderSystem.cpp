#include "Gl3RenderSystem/PrecompiledHeader.h"

#include "Gl3RenderSystem/Gl3RenderSystem.h"
#include "Gl3RenderSystem/Gl3SubmeshRenderer.h"
#include "Gl3RenderSystem/Gl3LightRenderer.h"
#include "Gl3RenderSystem/Gl3MaterialRenderer.h"
#include "Gl3RenderSystem/Gl3TextureRenderer.h"
#include "Gl3RenderSystem/Gl3TextureEnvironmentRenderer.h"
#include "Gl3RenderSystem/Gl3WindowRenderer.h"
#include "Gl3RenderSystem/Gl3Context.h"
#include "Gl3RenderSystem/Gl3ShaderProgram.h"

using namespace Castor3D;

Gl3RenderSystem :: Gl3RenderSystem( SceneManager * p_pSceneManager)
	:	GlRenderSystem( p_pSceneManager)
{
	Logger::LogMessage( CU_T( "Gl3RenderSystem :: Gl3RenderSystem"));
}

Gl3RenderSystem :: ~Gl3RenderSystem()
{
	Delete();
}

void Gl3RenderSystem :: Initialise()
{
	Logger::LogMessage( CU_T( "Gl3RenderSystem :: Initialise"));
	if (sm_initialised)
	{
		return;
	}

	Logger::LogMessage( CU_T( "****************************************"));
	Logger::LogMessage( CU_T( "Initialising OpenGL"));

	InitOpenGlExtensions();

	sm_useMultiTexturing = true;
	sm_useVertexBufferObjects = true;
	sm_forceShaders = true;

	Logger::LogMessage( CU_T( "OpenGL Initialisation Ended"));
	Logger::LogMessage( CU_T( "****************************************"));

	if ( ! GLEW_VERSION_3_0)
	{
		Logger::LogError( String( "No support of OpenGL 3.x or higher"));
	}
	sm_initialised = true;

	GlPipeline::InitFunctions();
}

void Gl3RenderSystem :: SetCurrentShaderProgram( ShaderProgramBase * p_pShader)
{
	if (m_pCurrentProgram != p_pShader)
	{
		RenderSystem::SetCurrentShaderProgram( p_pShader);
		Gl3ShaderProgram * l_pCurrentProgram = (Gl3ShaderProgram *)p_pShader;

		if (m_pCurrentProgram != NULL)
		{
			for (size_t i = 0 ; i < m_lightRenderers.size() ; i++)
			{
				m_lightRenderers[i]->Initialise();
			}
		}

		GlPipeline::InitFunctions();
	}
}

int Gl3RenderSystem :: LockLight()
{
	int l_iReturn = -1;

	if (m_setAvailableIndexes.size() > 0)
	{
		l_iReturn = *m_setAvailableIndexes.begin();
		m_setAvailableIndexes.erase( m_setAvailableIndexes.begin());
	}

	return l_iReturn;
}

void Gl3RenderSystem :: UnlockLight( int p_iIndex)
{
	if (p_iIndex >= 0 && m_setAvailableIndexes.find( p_iIndex) == m_setAvailableIndexes.end())
	{
		m_setAvailableIndexes.insert( std::set <int>::value_type( p_iIndex));
	}
}

void Gl3RenderSystem :: BeginOverlaysRendering()
{
	RenderSystem::BeginOverlaysRendering();
	CheckGlError( glDisable( GL_DEPTH_TEST), CU_T( "Gl3RenderSystem :: BeginOverlaysRendering - glDisable( GL_DEPTH_TEST)"));
}

void Gl3RenderSystem :: RenderAmbientLight( const Colour & p_clColour)
{
	if (m_pCurrentProgram != NULL)
	{
		static_cast <Gl3ShaderProgram *>( m_pCurrentProgram)->SetAmbientLight( p_clColour);
	}
}

GlslShaderProgramPtr Gl3RenderSystem :: _createGlslShaderProgram( const String & p_vertexShaderFile, 
																 const String & p_fragmentShaderFile,
																 const String & p_geometryShaderFile)
{
	return GlslShaderProgramPtr( new Gl3ShaderProgram( p_vertexShaderFile, p_fragmentShaderFile, p_geometryShaderFile));
}

SubmeshRendererPtr Gl3RenderSystem :: _createSubmeshRenderer()
{
	return SubmeshRendererPtr( new Gl3SubmeshRenderer( m_pSceneManager));
}

TextureEnvironmentRendererPtr Gl3RenderSystem :: _createTexEnvRenderer()
{
	return TextureEnvironmentRendererPtr( new Gl3TextureEnvironmentRenderer( m_pSceneManager));
}

TextureRendererPtr Gl3RenderSystem :: _createTextureRenderer()
{
	return TextureRendererPtr( new Gl3TextureRenderer( m_pSceneManager));
}

PassRendererPtr Gl3RenderSystem :: _createPassRenderer()
{
	return PassRendererPtr( new Gl3PassRenderer( m_pSceneManager));
}

CameraRendererPtr Gl3RenderSystem :: _createCameraRenderer()
{
	return CameraRendererPtr( new GlCameraRenderer( m_pSceneManager));
}

LightRendererPtr Gl3RenderSystem :: _createLightRenderer()
{
	return LightRendererPtr( new Gl3LightRenderer( m_pSceneManager));
}

WindowRendererPtr Gl3RenderSystem :: _createWindowRenderer()
{
	return WindowRendererPtr( new Gl3WindowRenderer( m_pSceneManager));
}

OverlayRendererPtr Gl3RenderSystem :: _createOverlayRenderer()
{
	return OverlayRendererPtr( new GlOverlayRenderer( m_pSceneManager));
}

IndexBufferPtr Gl3RenderSystem :: _createIndexBuffer()
{
	return BufferManager::CreateBuffer<unsigned int, GlVboIndexBuffer>();
}

VertexBufferPtr Gl3RenderSystem :: _createVertexBuffer( const BufferElementDeclaration * p_pElements, size_t p_uiNbElements)
{
	return BufferManager::CreateVertexBuffer<Gl3VboVertexBuffer>( p_pElements, p_uiNbElements);
}

TextureBufferObjectPtr Gl3RenderSystem :: _createTextureBuffer()
{
	return BufferManager::CreateBuffer<unsigned char, GlTextureBufferObject>();
}
