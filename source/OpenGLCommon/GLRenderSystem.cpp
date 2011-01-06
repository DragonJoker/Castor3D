#include "OpenGLCommon/PrecompiledHeader.h"

#include "OpenGLCommon/GLRenderSystem.h"
#include "OpenGLCommon/GLShaderProgram.h"
#include "OpenGLCommon/GLShaderObject.h"
#include "OpenGLCommon/CgGLShaderProgram.h"
#include "OpenGLCommon/CgGLShaderObject.h"

using namespace Castor3D;

#define GL_LIGHT_MODEL_COLOR_CONTROL	0x81F8
#define GL_SEPARATE_SPECULAR_COLOR		0x81FA

bool GLRenderSystem :: sm_useVBO = false;
bool GLRenderSystem  :: sm_extensionsInit = false;
bool GLRenderSystem  :: sm_gpuShader4 = false;

GLRenderSystem :: GLRenderSystem( SceneManager * p_pSceneManager)
	:	RenderSystem( p_pSceneManager)
{
	m_setAvailableIndexes.insert( GL_LIGHT0);
	m_setAvailableIndexes.insert( GL_LIGHT1);
	m_setAvailableIndexes.insert( GL_LIGHT2);
	m_setAvailableIndexes.insert( GL_LIGHT3);
	m_setAvailableIndexes.insert( GL_LIGHT4);
	m_setAvailableIndexes.insert( GL_LIGHT5);
	m_setAvailableIndexes.insert( GL_LIGHT6);
	m_setAvailableIndexes.insert( GL_LIGHT7);
}

GLRenderSystem :: ~GLRenderSystem()
{
	Delete();
}

void GLRenderSystem :: Initialise()
{
	Logger::LogMessage( CU_T( "GLRenderSystem :: Initialise"));

	if (sm_initialised)
	{
		return;
	}

	Logger::LogMessage( CU_T( "************************************************************************************************************************"));
	Logger::LogMessage( CU_T( "Initialising OpenGL"));

	InitOpenGLExtensions();

	sm_useMultiTexturing = false;

	if (glActiveTexture != NULL
		&& glClientActiveTexture != NULL
		&& glMultiTexCoord2 != NULL)
	{
		Logger::LogMessage( CU_T( "Using Multitexturing"));
		sm_useMultiTexturing = true;
	}

	sm_useVBO = false;
	if (glGenBuffers != NULL 
		&& glBindBuffer != NULL
		&& glDeleteBuffers != NULL
		&& glBufferData != NULL
		&& glBufferSubData != NULL
		&& glMultiDrawArrays != NULL)
	{
		Logger::LogMessage( CU_T( "Using Vertex Buffer Objects"));
		sm_useVBO = true;
	}

	Logger::LogMessage( CU_T( "OpenGL Initialisation Ended"));
	Logger::LogMessage( CU_T( "************************************************************************************************************************"));
	sm_initialised = true;

	GLPipeline::InitFunctions();
}

void GLRenderSystem :: Delete()
{
	Cleanup();
	map::deleteAll( m_contextMap);
}

void GLRenderSystem :: Cleanup()
{
	m_submeshesRenderers.clear();
	m_texEnvRenderers.clear();
	m_textureRenderers.clear();
	m_passRenderers.clear();
	m_lightRenderers.clear();
	m_windowRenderers.clear();
	m_cameraRenderers.clear();
}

bool GLRenderSystem :: InitOpenGLExtensions()
{
	if ( ! sm_extensionsInit)
	{
		GLenum l_err = glewInit();

		if (GLEW_OK != l_err)
		{
			Logger::LogError( String( (const char *)glewGetErrorString( l_err)));
			sm_extensionsInit = false;
		}
		else
		{
			Logger::LogMessage( CU_T( "Vendor : ") + String( (const char *)glGetString( GL_VENDOR)));
			Logger::LogMessage( CU_T( "Renderer : ") + String( (const char *)glGetString( GL_RENDERER)));
			Logger::LogMessage( CU_T( "OpenGL Version : ") + String( (const char *)glGetString( GL_VERSION)));

			sm_extensionsInit = true;

			HasGLSLSupport();
		}
	}

	return sm_extensionsInit;
}

bool GLRenderSystem :: HasGLSLSupport()
{
	bool l_bReturn = false;

	sm_useGeometryShaders = HasGeometryShaderSupport();
	sm_gpuShader4     = HasShaderModel4();

	if ( ! UseShaders())
	{
		sm_useShaders = true;

		if ( ! sm_extensionsInit)
		{
			InitOpenGLExtensions();
		}

		m_iOpenGLMajor = 1;
		m_iOpenGLMinor = 0;

		if (GLEW_VERSION_4_0)
		{
			m_iOpenGLMajor = 4;
			m_iOpenGLMinor = 0;
			Logger::LogMessage( CU_T( "Using version 4.0 (or higher) core functions"));
		}
		else if (GLEW_VERSION_3_3)
		{
			m_iOpenGLMajor = 3;
			m_iOpenGLMinor = 3;
			Logger::LogMessage( CU_T( "Using version 3.3 core functions"));
		}
		else if (GLEW_VERSION_3_2)
		{
			m_iOpenGLMajor = 3;
			m_iOpenGLMinor = 2;
			Logger::LogMessage( CU_T( "Using version 3.2 core functions"));
		}
		else if (GLEW_VERSION_3_1)
		{
			m_iOpenGLMajor = 3;
			m_iOpenGLMinor = 1;
			Logger::LogMessage( CU_T( "Using version 3.1 core functions"));
		}
		else if (GLEW_VERSION_3_0)
		{
			m_iOpenGLMajor = 3;
			m_iOpenGLMinor = 0;
			Logger::LogMessage( CU_T( "Using version 3.0 core functions"));
		}
		else if (GLEW_VERSION_2_1)
		{
			m_iOpenGLMajor = 2;
			m_iOpenGLMinor = 1;
			Logger::LogMessage( CU_T( "Using version 2.1 core functions"));
		}
		else if (GLEW_VERSION_2_0)
		{
			m_iOpenGLMajor = 2;
			m_iOpenGLMinor = 0;
			Logger::LogMessage( CU_T( "Using version 2.0 core functions"));
		}
		else if (GLEW_VERSION_1_5)
		{
			m_iOpenGLMajor = 1;
			m_iOpenGLMinor = 5;
			Logger::LogMessage( CU_T( "Using version 1.5 core functions"));
		}
		else if (GLEW_VERSION_1_4)
		{
			m_iOpenGLMajor = 1;
			m_iOpenGLMinor = 4;
			Logger::LogMessage( CU_T( "Using version 1.4 core functions"));
		}
		else if (GLEW_VERSION_1_3)
		{
			m_iOpenGLMajor = 1;
			m_iOpenGLMinor = 3;
			Logger::LogMessage( CU_T( "Using version 1.3 core functions"));
		}
		else if (GLEW_VERSION_1_2)
		{
			m_iOpenGLMajor = 1;
			m_iOpenGLMinor = 2;
			Logger::LogMessage( CU_T( "Using version 1.2 core functions"));
		}

		if (GL_TRUE != glewGetExtension("GL_ARB_fragment_shader"))
		{
			Logger::LogWarning( CU_T( "OpenGL : GL_ARB_fragment_shader extension is not available"));
			sm_useShaders = false;
		}

		if (GL_TRUE != glewGetExtension("GL_ARB_vertex_shader"))
		{
			Logger::LogWarning( CU_T( "OpenGL : GL_ARB_vertex_shader extension is not available!"));
			sm_useShaders = false;
		}

		if (GL_TRUE != glewGetExtension("GL_ARB_shader_objects"))
		{
			Logger::LogWarning( CU_T( "OpenGL : GL_ARB_shader_objects extension is not available!"));
			sm_useShaders = false;
		}

		if (UseShaders())
		{
			Logger::LogMessage( CU_T( "Using OpenGL Shading Language"));
		}
	}

	return UseShaders();
}

bool GLRenderSystem :: HasOpenGL2Support()
{
	if ( ! sm_extensionsInit)
	{
		InitOpenGLExtensions();
	}

	return (GLEW_VERSION_2_0 == GL_TRUE);
} 

bool GLRenderSystem :: HasGeometryShaderSupport()
{
	if (GL_TRUE != glewGetExtension( "GL_EXT_geometry_shader4"))
	{
		return false;
	}

	return true;
}

bool GLRenderSystem :: HasShaderModel4()
{
	if (GL_TRUE != glewGetExtension( "GL_EXT_gpu_shader4"))
	{
		return false;
	}

	return true;
}

ShaderObject * GLRenderSystem :: CreateVertexShader()
{
	return new GLVertexShader();
}

ShaderObject * GLRenderSystem :: CreateFragmentShader()
{
	return new GLFragmentShader();
}

ShaderObject * GLRenderSystem :: CreateGeometryShader()
{
	return new GLGeometryShader();
}

CgShaderObject * GLRenderSystem :: CreateCgVertexShader()
{
	return new CgGLVertexShader();
}

CgShaderObject * GLRenderSystem :: CreateCgFragmentShader()
{
	return new CgGLFragmentShader();
}

CgShaderObject * GLRenderSystem :: CreateCgGeometryShader()
{
	return new CgGLGeometryShader();
}

CgShaderProgram * GLRenderSystem :: CreateCgShaderProgram( const String & p_vertexShaderFile, 
														   const String & p_fragmentShaderFile,
														   const String & p_geometryShaderFile)
{
	return new CgGLShaderProgram( p_vertexShaderFile, p_fragmentShaderFile, p_geometryShaderFile);
}

int GLRenderSystem :: LockLight()
{
	int l_iReturn = -1;

	if (m_setAvailableIndexes.size() > 0)
	{
		l_iReturn = *m_setAvailableIndexes.begin();
		m_setAvailableIndexes.erase( m_setAvailableIndexes.begin());
	}

	return l_iReturn;
}

void GLRenderSystem :: UnlockLight( int p_iIndex)
{
	if (p_iIndex >= 0 && m_setAvailableIndexes.find( p_iIndex) == m_setAvailableIndexes.end())
	{
		m_setAvailableIndexes.insert( std::set <int>::value_type( p_iIndex));
	}
}

void GLRenderSystem :: SetCurrentShaderProgram( ShaderProgram * p_pVal)
{
	if (m_pCurrentProgram != p_pVal)
	{
		RenderSystem::SetCurrentShaderProgram( p_pVal);

		for (size_t i = 0 ; i < m_lightRenderers.size() ; i++)
		{
			m_lightRenderers[i]->Initialise();
		}

		GLPipeline::InitFunctions();
	}
}

void GLRenderSystem :: BeginOverlaysRendering()
{
	RenderSystem::BeginOverlaysRendering();/*
	glDisable( GL_LIGHTING);
	CheckGLError( CU_T( "GLRenderSystem :: BeginOverlaysRendering - glDisable( GL_LIGHTING)"));
	glDisable( GL_DEPTH_TEST);
	CheckGLError( CU_T( "GLRenderSystem :: BeginOverlaysRendering - glDisable( GL_DEPTH_TEST)"));*/
}

void GLRenderSystem :: RenderAmbientLight( const Colour & p_clColour)
{
	glLightModelfv( GL_LIGHT_MODEL_AMBIENT, p_clColour.const_ptr());
	CheckGLError( CU_T( "GLRenderSystem :: EndOverlaysRendering - Pipeline::LoadIdentity"));
}

VertexAttribsBufferBoolPtr GLRenderSystem :: _create1BoolVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<bool, 1>( p_strArg);
}

VertexAttribsBufferIntPtr GLRenderSystem :: _create1IntVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<int, 1>( p_strArg);
}

VertexAttribsBufferUIntPtr GLRenderSystem :: _create1UIntVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<unsigned int, 1>( p_strArg);
}

VertexAttribsBufferFloatPtr GLRenderSystem :: _create1FloatVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<float, 1>( p_strArg);
}

VertexAttribsBufferDoublePtr GLRenderSystem :: _create1DoubleVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<double, 1>( p_strArg);
}

VertexAttribsBufferBoolPtr GLRenderSystem :: _create2BoolVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<bool, 2>( p_strArg);
}

VertexAttribsBufferIntPtr GLRenderSystem :: _create2IntVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<int, 2>( p_strArg);
}

VertexAttribsBufferUIntPtr GLRenderSystem :: _create2UIntVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<unsigned int, 2>( p_strArg);
}

VertexAttribsBufferFloatPtr GLRenderSystem :: _create2FloatVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<float, 2>( p_strArg);
}

VertexAttribsBufferDoublePtr GLRenderSystem :: _create2DoubleVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<double, 2>( p_strArg);
}

VertexAttribsBufferBoolPtr GLRenderSystem :: _create3BoolVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<bool, 3>( p_strArg);
}

VertexAttribsBufferIntPtr GLRenderSystem :: _create3IntVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<int, 3>( p_strArg);
}

VertexAttribsBufferUIntPtr GLRenderSystem :: _create3UIntVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<unsigned int, 3>( p_strArg);
}

VertexAttribsBufferFloatPtr GLRenderSystem :: _create3FloatVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<float, 3>( p_strArg);
}

VertexAttribsBufferDoublePtr GLRenderSystem :: _create3DoubleVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<double, 3>( p_strArg);
}

VertexAttribsBufferBoolPtr GLRenderSystem :: _create4BoolVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<bool, 4>( p_strArg);
}

VertexAttribsBufferIntPtr GLRenderSystem :: _create4IntVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<int, 4>( p_strArg);
}

VertexAttribsBufferUIntPtr GLRenderSystem :: _create4UIntVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<unsigned int, 4>( p_strArg);
}

VertexAttribsBufferFloatPtr GLRenderSystem :: _create4FloatVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<float, 4>( p_strArg);
}

VertexAttribsBufferDoublePtr GLRenderSystem :: _create4DoubleVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<double, 4>( p_strArg);
}
