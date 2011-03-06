#include "OpenGlCommon/PrecompiledHeader.h"

#include "OpenGlCommon/GlRenderSystem.h"
#include "OpenGlCommon/GlShaderProgram.h"
#include "OpenGlCommon/GlShaderObject.h"
#include "OpenGlCommon/CgGlShaderProgram.h"
#include "OpenGlCommon/CgGlShaderObject.h"

using namespace Castor3D;

#define GL_LIGHT_MODEL_COLOR_CONTROL	0x81F8
#define GL_SEPARATE_SPECULAR_COLOR		0x81FA

bool GlRenderSystem :: sm_useVertexBufferObjects = false;
bool GlRenderSystem  :: sm_extensionsInit = false;
bool GlRenderSystem  :: sm_gpuShader4 = false;

GlRenderSystem :: GlRenderSystem( SceneManager * p_pSceneManager)
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

GlRenderSystem :: ~GlRenderSystem()
{
	Delete();
}

void GlRenderSystem :: Initialise()
{
	Logger::LogMessage( CU_T( "GlRenderSystem :: Initialise"));

	if (sm_initialised)
	{
		return;
	}

	Logger::LogMessage( CU_T( "************************************************************************************************************************"));
	Logger::LogMessage( CU_T( "Initialising OpenGL"));

	InitOpenGlExtensions();

	sm_useMultiTexturing = false;

	if (glActiveTexture != NULL
		&& glClientActiveTexture != NULL
		&& glMultiTexCoord2 != NULL)
	{
		Logger::LogMessage( CU_T( "Using Multitexturing"));
		sm_useMultiTexturing = true;
	}

	sm_useVertexBufferObjects = false;
	if (glGenBuffers != NULL
		&& glBindBuffer != NULL
		&& glDeleteBuffers != NULL
		&& glBufferData != NULL
		&& glBufferSubData != NULL
		&& glMultiDrawArrays != NULL)
	{
		Logger::LogMessage( CU_T( "Using Vertex Buffer Objects"));
		sm_useVertexBufferObjects = true;
	}

	Logger::LogMessage( CU_T( "OpenGL Initialisation Ended"));
	Logger::LogMessage( CU_T( "************************************************************************************************************************"));
	sm_initialised = true;

	GlPipeline::InitFunctions();
}

void GlRenderSystem :: Delete()
{
	Cleanup();
	map::deleteAll( m_contextMap);
}

void GlRenderSystem :: Cleanup()
{
	m_submeshesRenderers.clear();
	m_texEnvRenderers.clear();
	m_textureRenderers.clear();
	m_passRenderers.clear();
	m_lightRenderers.clear();
	m_windowRenderers.clear();
	m_cameraRenderers.clear();
}

bool GlRenderSystem :: InitOpenGlExtensions()
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

			HasGlslSupport();
		}
	}

	return sm_extensionsInit;
}

bool GlRenderSystem :: HasGlslSupport()
{
	bool l_bReturn = false;

	sm_useGeometryShaders = HasGeometryShaderSupport();
	sm_gpuShader4     = HasShaderModel4();

	if ( ! UseShaders())
	{
		sm_useShaders = true;

		if ( ! sm_extensionsInit)
		{
			InitOpenGlExtensions();
		}

		m_iOpenGlMajor = 1;
		m_iOpenGlMinor = 0;

		if (GLEW_VERSION_4_0)
		{
			m_iOpenGlMajor = 4;
			m_iOpenGlMinor = 0;
			Logger::LogMessage( CU_T( "Using version 4.0 (or higher) core functions"));
		}
		else if (GLEW_VERSION_3_3)
		{
			m_iOpenGlMajor = 3;
			m_iOpenGlMinor = 3;
			Logger::LogMessage( CU_T( "Using version 3.3 core functions"));
		}
		else if (GLEW_VERSION_3_2)
		{
			m_iOpenGlMajor = 3;
			m_iOpenGlMinor = 2;
			Logger::LogMessage( CU_T( "Using version 3.2 core functions"));
		}
		else if (GLEW_VERSION_3_1)
		{
			m_iOpenGlMajor = 3;
			m_iOpenGlMinor = 1;
			Logger::LogMessage( CU_T( "Using version 3.1 core functions"));
		}
		else if (GLEW_VERSION_3_0)
		{
			m_iOpenGlMajor = 3;
			m_iOpenGlMinor = 0;
			Logger::LogMessage( CU_T( "Using version 3.0 core functions"));
		}
		else if (GLEW_VERSION_2_1)
		{
			m_iOpenGlMajor = 2;
			m_iOpenGlMinor = 1;
			Logger::LogMessage( CU_T( "Using version 2.1 core functions"));
		}
		else if (GLEW_VERSION_2_0)
		{
			m_iOpenGlMajor = 2;
			m_iOpenGlMinor = 0;
			Logger::LogMessage( CU_T( "Using version 2.0 core functions"));
		}
		else if (GLEW_VERSION_1_5)
		{
			m_iOpenGlMajor = 1;
			m_iOpenGlMinor = 5;
			Logger::LogMessage( CU_T( "Using version 1.5 core functions"));
		}
		else if (GLEW_VERSION_1_4)
		{
			m_iOpenGlMajor = 1;
			m_iOpenGlMinor = 4;
			Logger::LogMessage( CU_T( "Using version 1.4 core functions"));
		}
		else if (GLEW_VERSION_1_3)
		{
			m_iOpenGlMajor = 1;
			m_iOpenGlMinor = 3;
			Logger::LogMessage( CU_T( "Using version 1.3 core functions"));
		}
		else if (GLEW_VERSION_1_2)
		{
			m_iOpenGlMajor = 1;
			m_iOpenGlMinor = 2;
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

bool GlRenderSystem :: HasOpenGl2Support()
{
	if ( ! sm_extensionsInit)
	{
		InitOpenGlExtensions();
	}

	return (GLEW_VERSION_2_0 == GL_TRUE);
}

bool GlRenderSystem :: HasGeometryShaderSupport()
{
	if (GL_TRUE != glewGetExtension( "GL_EXT_geometry_shader4"))
	{
		return false;
	}

	return true;
}

bool GlRenderSystem :: HasShaderModel4()
{
	if (GL_TRUE != glewGetExtension( "GL_EXT_gpu_shader4"))
	{
		return false;
	}

	return true;
}

void GlRenderSystem :: DrawIndexedPrimitives( ePRIMITIVE_TYPE p_eType, size_t p_uiMinVertex, size_t p_uiVertexCount, size_t p_uiFirstIndex, size_t p_uiCount)
{
	const void * l_pOffset = BUFFER_OFFSET( p_uiFirstIndex * 4);

	switch (p_eType)
	{
	case eTriangles:		glDrawElements( GL_TRIANGLES,      p_uiCount * 3,	GL_UNSIGNED_INT,	l_pOffset); break;
	case eTriangleStrips:	glDrawElements( GL_TRIANGLE_STRIP, p_uiCount + 2,	GL_UNSIGNED_INT,	l_pOffset); break;
	case eTriangleFan:		glDrawElements( GL_TRIANGLE_FAN,   p_uiCount + 1,	GL_UNSIGNED_INT,	l_pOffset); break;
	case eLines:			glDrawElements( GL_LINES,          p_uiCount * 2,	GL_UNSIGNED_INT,	l_pOffset); break; 
	case ePoints:			glDrawElements( GL_POINTS,         p_uiCount,		GL_UNSIGNED_INT,	l_pOffset); break;
	}
}

GlslShaderObjectPtr GlRenderSystem :: _createGlslVertexShader()
{
	return GlslShaderObjectPtr( new GlVertexShader());
}

GlslShaderObjectPtr GlRenderSystem :: _createGlslFragmentShader()
{
	return GlslShaderObjectPtr( new GlFragmentShader());
}

GlslShaderObjectPtr GlRenderSystem :: _createGlslGeometryShader()
{
	return GlslShaderObjectPtr( new GlGeometryShader());
}

CgShaderObjectPtr GlRenderSystem :: _createCgVertexShader()
{
	return CgShaderObjectPtr( new CgGlVertexShader());
}

CgShaderObjectPtr GlRenderSystem :: _createCgFragmentShader()
{
	return CgShaderObjectPtr( new CgGlFragmentShader());
}

CgShaderObjectPtr GlRenderSystem :: _createCgGeometryShader()
{
	return CgShaderObjectPtr( new CgGlGeometryShader());
}

CgShaderProgramPtr GlRenderSystem :: _createCgShaderProgram( const String & p_vertexShaderFile,
														    const String & p_fragmentShaderFile,
														    const String & p_geometryShaderFile)
{
	return CgShaderProgramPtr( new CgGlShaderProgram( p_vertexShaderFile, p_fragmentShaderFile, p_geometryShaderFile));
}

int GlRenderSystem :: LockLight()
{
	int l_iReturn = -1;

	if (m_setAvailableIndexes.size() > 0)
	{
		l_iReturn = *m_setAvailableIndexes.begin();
		m_setAvailableIndexes.erase( m_setAvailableIndexes.begin());
	}

	return l_iReturn;
}

void GlRenderSystem :: UnlockLight( int p_iIndex)
{
	if (p_iIndex >= 0 && m_setAvailableIndexes.find( p_iIndex) == m_setAvailableIndexes.end())
	{
		m_setAvailableIndexes.insert( std::set <int>::value_type( p_iIndex));
	}
}

void GlRenderSystem :: SetCurrentShaderProgram( ShaderProgramBase * p_pVal)
{
	if (m_pCurrentProgram != p_pVal)
	{
		RenderSystem::SetCurrentShaderProgram( p_pVal);

		for (size_t i = 0 ; i < m_lightRenderers.size() ; i++)
		{
			m_lightRenderers[i]->Initialise();
		}

		GlPipeline::InitFunctions();
	}
}

void GlRenderSystem :: BeginOverlaysRendering()
{
	RenderSystem::BeginOverlaysRendering();/*
	glDisable( GL_LIGHTING);
	CheckGlError( CU_T( "GlRenderSystem :: BeginOverlaysRendering - glDisable( GL_LIGHTING)"));
	glDisable( GL_DEPTH_TEST);
	CheckGlError( CU_T( "GlRenderSystem :: BeginOverlaysRendering - glDisable( GL_DEPTH_TEST)"));*/
}

void GlRenderSystem :: RenderAmbientLight( const Colour & p_clColour)
{
	CheckGlError( glLightModelfv( GL_LIGHT_MODEL_AMBIENT, p_clColour.const_ptr()), CU_T( "GlRenderSystem :: EndOverlaysRendering - Pipeline::LoadIdentity"));
}

VertexAttribsBufferBoolPtr GlRenderSystem :: _create1BoolVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<bool, 1>( p_strArg);
}

VertexAttribsBufferIntPtr GlRenderSystem :: _create1IntVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<int, 1>( p_strArg);
}

VertexAttribsBufferUIntPtr GlRenderSystem :: _create1UIntVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<unsigned int, 1>( p_strArg);
}

VertexAttribsBufferFloatPtr GlRenderSystem :: _create1FloatVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<float, 1>( p_strArg);
}

VertexAttribsBufferDoublePtr GlRenderSystem :: _create1DoubleVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<double, 1>( p_strArg);
}

VertexAttribsBufferBoolPtr GlRenderSystem :: _create2BoolVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<bool, 2>( p_strArg);
}

VertexAttribsBufferIntPtr GlRenderSystem :: _create2IntVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<int, 2>( p_strArg);
}

VertexAttribsBufferUIntPtr GlRenderSystem :: _create2UIntVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<unsigned int, 2>( p_strArg);
}

VertexAttribsBufferFloatPtr GlRenderSystem :: _create2FloatVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<float, 2>( p_strArg);
}

VertexAttribsBufferDoublePtr GlRenderSystem :: _create2DoubleVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<double, 2>( p_strArg);
}

VertexAttribsBufferBoolPtr GlRenderSystem :: _create3BoolVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<bool, 3>( p_strArg);
}

VertexAttribsBufferIntPtr GlRenderSystem :: _create3IntVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<int, 3>( p_strArg);
}

VertexAttribsBufferUIntPtr GlRenderSystem :: _create3UIntVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<unsigned int, 3>( p_strArg);
}

VertexAttribsBufferFloatPtr GlRenderSystem :: _create3FloatVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<float, 3>( p_strArg);
}

VertexAttribsBufferDoublePtr GlRenderSystem :: _create3DoubleVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<double, 3>( p_strArg);
}

VertexAttribsBufferBoolPtr GlRenderSystem :: _create4BoolVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<bool, 4>( p_strArg);
}

VertexAttribsBufferIntPtr GlRenderSystem :: _create4IntVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<int, 4>( p_strArg);
}

VertexAttribsBufferUIntPtr GlRenderSystem :: _create4UIntVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<unsigned int, 4>( p_strArg);
}

VertexAttribsBufferFloatPtr GlRenderSystem :: _create4FloatVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<float, 4>( p_strArg);
}

VertexAttribsBufferDoublePtr GlRenderSystem :: _create4DoubleVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<double, 4>( p_strArg);
}
