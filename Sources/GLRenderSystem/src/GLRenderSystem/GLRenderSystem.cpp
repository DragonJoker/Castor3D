//******************************************************************************
#include "PrecompiledHeader.h"

#include "GLRenderSystem.h"
#include "GLCameraRenderer.h"
#include "GLSubmeshRenderer.h"
#include "GLLightRenderer.h"
#include "GLMaterialRenderer.h"
#include "GLTexEnvironmentRenderer.h"
#include "GLTextureRenderer.h"
#include "GLWindowRenderer.h"
#include "GLSceneNodeRenderer.h"
#include "GLOverlayRenderer.h"
#include "GLContext.h"
#include "Module_GL.h"
#include "GLSLObject.h"
#include "GLSLProgram.h"
/*
#include <Castor3D/camera/Camera.h>
#include <Castor3D/camera/Viewport.h>
#include <Castor3D/light/Light.h>
#include <Castor3D/light/SpotLight.h>
#include <Castor3D/light/PointLight.h>
#include <Castor3D/light/DirectionalLight.h>
#include <Castor3D/material/MaterialManager.h>
#include <Castor3D/material/Pass.h>
#include <Castor3D/material/TextureEnvironment.h>
#include <Castor3D/material/TextureUnit.h>
#include <Castor3D/geometry/basic/Face.h>
#include <Castor3D/math/Vector3f.h>
#include <Castor3D/scene/SceneNode.h>
#include <Castor3D/scene/Scene.h>
#include <Castor3D/geometry/primitives/Geometry.h>
#include <Castor3D/main/Root.h>
#include <Castor3D/main/RenderWindow.h>
#include <CastorUtils/Log.h>
*/
//******************************************************************************
using namespace Castor3D;
//******************************************************************************
#define GL_LIGHT_MODEL_COLOR_CONTROL	0x81F8
#define GL_SEPARATE_SPECULAR_COLOR		0x81FA
//******************************************************************************
bool GLRenderSystem :: sm_useVBO = false;
int GLRenderSystem  :: sm_drawTypes[7]				= { GL_TRIANGLES, GL_LINES, GL_POINTS, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_QUADS, GL_QUAD_STRIP };
int GLRenderSystem  :: sm_environmentModes[5]		= { GL_REPLACE, GL_MODULATE, GL_BLEND, GL_ADD, GL_COMBINE };
int GLRenderSystem  :: sm_RGBCombinations[8]		= { 0, GL_REPLACE, GL_MODULATE, GL_ADD, GL_ADD_SIGNED, GL_DOT3_RGB, GL_DOT3_RGBA, GL_INTERPOLATE };
int GLRenderSystem  :: sm_RGBOperands[4]			= { GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA };
int GLRenderSystem  :: sm_alphaCombinations[6]		= { 0, GL_REPLACE, GL_MODULATE, GL_ADD, GL_ADD_SIGNED, GL_INTERPOLATE };
int GLRenderSystem  :: sm_alphaOperands[2]			= { GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA };
int GLRenderSystem  :: sm_combinationSources[5]		= { 1, GL_TEXTURE, GL_CONSTANT, GL_PRIMARY_COLOR, GL_PREVIOUS };
int GLRenderSystem  :: sm_textureDimensions[3]		= { GL_TEXTURE_1D, GL_TEXTURE_2D, GL_TEXTURE_3D };
int GLRenderSystem  :: sm_lightIndexes[8]			= { GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3, GL_LIGHT4, GL_LIGHT5, GL_LIGHT6, GL_LIGHT7 };

bool GLRenderSystem  :: sm_useGLSL = false;
bool GLRenderSystem  :: sm_extensionsInit = false;
bool GLRenderSystem  :: sm_geometryShader = false;
bool GLRenderSystem  :: sm_gpuShader4 = false;

char * GLRenderSystem  :: sm_GLSLStrings[] = {
	"[e00] GLSL is not available!",
	"[e01] Not a valid program object!",
	"[e02] Not a valid object!",
	"[e03] Out of memory!",
	"[e04] Unknown compiler error!",
	"[e05] Linker log is not available!",
	"[e06] Compiler log is not available!",
	"[Empty]"
};
//******************************************************************************

void GLRenderSystem :: GL_CheckError( const String & p_text)
{
	GLenum l_errorCode = glGetError();

	if (l_errorCode != GL_NO_ERROR)
	{
		if (l_errorCode == GL_INVALID_ENUM)
		{
			Log::LogMessage( p_text + C3D_T( " - Erreur - Invalid Enum"));
		}
		else if (l_errorCode == GL_INVALID_VALUE)
		{
			Log::LogMessage( p_text + C3D_T( " - Erreur - Invalid Value"));
		}
		else if (l_errorCode == GL_INVALID_OPERATION)
		{
			Log::LogMessage( p_text + C3D_T( " - Erreur - Invalid Operation"));
		}
		else if (l_errorCode == GL_STACK_OVERFLOW)
		{
			Log::LogMessage( p_text + C3D_T( " - Erreur - Stack Overflow"));
		}
		else if (l_errorCode == GL_STACK_UNDERFLOW)
		{
			Log::LogMessage( p_text + C3D_T( " - Erreur - Stack Underflow"));
		}
		else if (l_errorCode == GL_OUT_OF_MEMORY)
		{
			Log::LogMessage( p_text + C3D_T( " - Erreur - Out of memory"));
		}
		else
		{
			Log::LogMessage( p_text + C3D_T( " - Erreur - Unknown error"));
		}
	}
}

//******************************************************************************

GLRenderSystem :: GLRenderSystem()
	:	RenderSystem(),
		m_mainContext( NULL),
		m_currentContext( NULL)
{
	Log::LogMessage( "GLRenderSystem :: GLRenderSystem");
	sm_initialised = false;
	sm_singleton = this;
	sm_useGLSL = false;
	sm_extensionsInit = false;
	sm_geometryShader = false;
	sm_gpuShader4 = false;

	Root::SetRenderSystem( this);
	m_setAvailableIndexes.insert( std::set<int>::value_type( GL_LIGHT0));
	m_setAvailableIndexes.insert( std::set<int>::value_type( GL_LIGHT1));
	m_setAvailableIndexes.insert( std::set<int>::value_type( GL_LIGHT2));
	m_setAvailableIndexes.insert( std::set<int>::value_type( GL_LIGHT3));
	m_setAvailableIndexes.insert( std::set<int>::value_type( GL_LIGHT4));
	m_setAvailableIndexes.insert( std::set<int>::value_type( GL_LIGHT5));
	m_setAvailableIndexes.insert( std::set<int>::value_type( GL_LIGHT6));
	m_setAvailableIndexes.insert( std::set<int>::value_type( GL_LIGHT7));
}

//******************************************************************************

GLRenderSystem :: ~GLRenderSystem()
{
	map::deleteAll( m_contextMap);
	vector::deleteAll( m_submeshesRenderers);
	vector::deleteAll( m_texEnvRenderers);
	vector::deleteAll( m_textureRenderers);
	vector::deleteAll( m_passRenderers);
	vector::deleteAll( m_lightRenderers);
	vector::deleteAll( m_windowRenderers);
	vector::deleteAll( m_cameraRenderers);
	vector::deleteAll( m_viewportRenderers);
	vector::deleteAll( m_nodeRenderers);
}

//******************************************************************************

void GLRenderSystem :: Initialise()
{
	Log::LogMessage( "GLRenderSystem :: Initialise");
	if (sm_initialised)
	{
		return;
	}

	Log::LogMessage( "****************************************\nInitialising OpenGL");

	InitOpenGLExtensions();

	sm_useMultiTexturing = false;

	if (glActiveTextureARB != NULL)
	{
		sm_useMultiTexturing = false;

		if (glClientActiveTextureARB != NULL)
		{
			if (glMultiTexCoord2fARB != NULL)
			{
				Log::LogMessage( "Using Multitexturing");
				sm_useMultiTexturing = true;
			}
		}
	}

	sm_useVBO = false;
	if (glGenBuffersARB != NULL)
	{
		if (glBindBufferARB != NULL)
		{
			if (glDeleteBuffersARB != NULL)
			{
				if (glBufferDataARB != NULL)
				{
					if (glBufferSubDataARB != NULL)
					{
						if (glMultiDrawArrays != NULL)
						{
							Log::LogMessage( "Using Vertex Buffer Objects");
							sm_useVBO = true;
						}
					}
				}
			}
		}
	}

	Log::LogMessage( "OpenGL Initialisation Ended\n****************************************");
	sm_initialised = true;
}

//******************************************************************************

void GLRenderSystem :: DisplayArc( DrawType p_displayMode,
								   const Vector3fPtrList & p_vertex)
{
	//On selectionne le type d'affichage necessaire
	glBegin( sm_drawTypes[p_displayMode]);

	Vector3f * l_vertex;
	for (Vector3fPtrList::const_iterator l_it = p_vertex.begin() ; l_it != p_vertex.end() ; ++l_it)
	{
		l_vertex = *l_it;
		glVertex3f( l_vertex->x, l_vertex->y, l_vertex->z);
	}

	glEnd();
}

//******************************************************************************

bool GLRenderSystem :: InitOpenGLExtensions()
{
	if ( ! sm_extensionsInit)
	{
		GLenum l_err = glewInit();

		if (GLEW_OK != l_err)
		{
			Log::LogMessage( C3D_T( "Error:") + String( (const char *)glewGetErrorString( l_err)));
			sm_extensionsInit = false;
		}
		else
		{
			Log::LogMessage( C3D_T( "Vendor : ") + String( (const char *)glGetString( GL_VENDOR)));
			Log::LogMessage( C3D_T( "Renderer : ") + String( (const char *)glGetString( GL_RENDERER)));
			Log::LogMessage( C3D_T( "Version : ") + String( (const char *)glGetString( GL_VERSION)));

			sm_extensionsInit = true;

			HasGLSLSupport();
		}
	}

	return sm_extensionsInit;
}

//******************************************************************************

bool GLRenderSystem :: HasGLSLSupport()
{
	bool l_bReturn = false;

	sm_geometryShader = HasGeometryShaderSupport();
	sm_gpuShader4     = HasShaderModel4();

	if ( ! sm_useGLSL)
	{
		sm_useGLSL = true;

		if ( ! sm_extensionsInit)
		{
			InitOpenGLExtensions();
		}

		if (GLEW_VERSION_2_0)
		{
			Log::LogMessage( "Using version 2.0 (or higher)");
		}
		else if (GLEW_VERSION_1_5)
		{
			Log::LogMessage( "Using version 1.5 core functions");
		}
		else if (GLEW_VERSION_1_4)
		{
			Log::LogMessage( "Using version 1.4 core functions");
		}
		else if (GLEW_VERSION_1_3)
		{
			Log::LogMessage( "Using version 1.3 core functions");
		}
		else if (GLEW_VERSION_1_2)
		{
			Log::LogMessage( "Using version 1.2 core functions");
		}

		if (GL_TRUE != glewGetExtension("GL_ARB_fragment_shader"))
		{
			Log::LogMessage( "OpenGL : GL_ARB_fragment_shader extension is not available");
			sm_useGLSL = false;
		}

		if (GL_TRUE != glewGetExtension("GL_ARB_vertex_shader"))
		{
			Log::LogMessage( "OpenGL : GL_ARB_vertex_shader extension is not available!");
			sm_useGLSL = false;
		}

		if (GL_TRUE != glewGetExtension("GL_ARB_shader_objects"))
		{
			Log::LogMessage( "OpenGL : GL_ARB_shader_objects extension is not available!");
			sm_useGLSL = false;
		}

		if (sm_useGLSL)
		{
			Log::LogMessage( "Using OpenGL Shading Language");
		}
	}

	if (sm_useGLSL)
	{
		for (int i = 0 ; i < 8 ; i++)
		{
			GLLightRenderer::sm_pLightShaders.push_back( new GLShaderProgram());
			GLLightRenderer::sm_pLightShaders[i]->SetVertexFile( GLLightRenderer::sm_vertexShader);
			GLLightRenderer::sm_pLightShaders[i]->SetFragmentFile( GLLightRenderer::sm_fragmentShader);

			Point3DUniformVariable<float> * l_pPosition = new Point3DUniformVariable<float>();
			l_pPosition->SetName( "vPosition");
			GLLightRenderer::sm_pLightShaders[i]->AddUniformVariable( l_pPosition);

			Point4DUniformVariable<float> * l_pColor = new Point4DUniformVariable<float>();
			l_pColor->SetName( "vAmbient");
			GLLightRenderer::sm_pLightShaders[i]->AddUniformVariable( l_pColor);

			l_pColor = new Point4DUniformVariable<float>();
			l_pColor->SetName( "vDiffuse");
			GLLightRenderer::sm_pLightShaders[i]->AddUniformVariable( l_pColor);

			l_pColor = new Point4DUniformVariable<float>();
			l_pColor->SetName( "vSpecular");
			GLLightRenderer::sm_pLightShaders[i]->AddUniformVariable( l_pColor);

			OneUniformVariable<float> * l_pExponent = new OneUniformVariable<float>();
			l_pExponent->SetName( "vExponent");
			GLLightRenderer::sm_pLightShaders[i]->AddUniformVariable( l_pExponent);
		}
	}

	return sm_useGLSL;
}

//******************************************************************************

bool GLRenderSystem :: HasOpenGL2Support(void)
{
	if ( ! sm_extensionsInit)
	{
		InitOpenGLExtensions();
	}

	return (GLEW_VERSION_2_0 == GL_TRUE);
} 

//******************************************************************************  

bool GLRenderSystem :: HasGeometryShaderSupport(void)
{
	if (GL_TRUE != glewGetExtension("GL_EXT_geometry_shader4"))
	{
		return false;
	}

	return true;
}

//******************************************************************************

bool GLRenderSystem :: HasShaderModel4(void)
{
	if (GL_TRUE != glewGetExtension("GL_EXT_gpu_shader4"))
	{
		return false;
	}

	return true;
}

//******************************************************************************

void GLRenderSystem :: AddContext( GLContext * p_context, RenderWindow * p_window)
{
	m_contextMap[p_window] = p_context;
}

//******************************************************************************

SubmeshRenderer * GLRenderSystem :: CreateSubmeshRenderer()
{
	GLSubmeshRenderer * l_renderer = new GLSubmeshRenderer( this);
	m_submeshesRenderers.push_back( l_renderer);
	return l_renderer;
}

//******************************************************************************

TextureEnvironmentRenderer * GLRenderSystem :: CreateTextureEnvironmentRenderer()
{
	GLTextureEnvironmentRenderer * l_renderer = new GLTextureEnvironmentRenderer( this);
	m_texEnvRenderers.push_back( l_renderer);
	return l_renderer;
}

//******************************************************************************

TextureRenderer * GLRenderSystem :: CreateTextureRenderer()
{
	GLTextureRenderer * l_renderer = new GLTextureRenderer( this);
	m_textureRenderers.push_back( l_renderer);
	return l_renderer;
}

//******************************************************************************

PassRenderer * GLRenderSystem :: CreatePassRenderer()
{
	GLPassRenderer * l_renderer = new GLPassRenderer( this);
	m_passRenderers.push_back( l_renderer);
	return l_renderer;
}

//******************************************************************************

LightRenderer * GLRenderSystem :: CreateLightRenderer()
{
	GLLightRenderer * l_renderer = new GLLightRenderer( this);
	m_lightRenderers.push_back( l_renderer);
	return l_renderer;
}

//******************************************************************************

CameraRenderer * GLRenderSystem :: CreateCameraRenderer()
{
	GLCameraRenderer * l_renderer = new GLCameraRenderer( this);
	m_cameraRenderers.push_back( l_renderer);
	return l_renderer;
}

//******************************************************************************

ViewportRenderer * GLRenderSystem :: CreateViewportRenderer()
{
	GLViewportRenderer * l_renderer = new GLViewportRenderer( this);
	m_viewportRenderers.push_back( l_renderer);
	return l_renderer;
}

//******************************************************************************

WindowRenderer * GLRenderSystem :: CreateWindowRenderer()
{
	GLWindowRenderer * l_renderer = new GLWindowRenderer( this);
	m_windowRenderers.push_back( l_renderer);
	return l_renderer;
}

//******************************************************************************

SceneNodeRenderer * GLRenderSystem :: CreateSceneNodeRenderer()
{
	GLSceneNodeRenderer * l_renderer = new GLSceneNodeRenderer( this);
	m_nodeRenderers.push_back( l_renderer);
	return l_renderer;
}

//******************************************************************************

OverlayRenderer * GLRenderSystem :: CreateOverlayRenderer()
{
	GLOverlayRenderer * l_renderer = new GLOverlayRenderer( this);
	m_overlayRenderers.push_back( l_renderer);
	return l_renderer;
}

//******************************************************************************

ShaderObject * GLRenderSystem :: CreateVertexShader()
{
	return new GLVertexShader();
}

//******************************************************************************

ShaderObject * GLRenderSystem :: CreateFragmentShader()
{
	return new GLFragmentShader();
}

//******************************************************************************

ShaderObject * GLRenderSystem :: CreateGeometryShader()
{
	return new GLGeometryShader();
}

//******************************************************************************

ShaderProgram * GLRenderSystem :: CreateShaderProgram( const String & p_vertexShaderFile, 
													   const String & p_fragmentShaderFile,
													   const String & p_geometryShaderFile)
{
	return new GLShaderProgram( p_vertexShaderFile, p_fragmentShaderFile, p_geometryShaderFile);
}

//******************************************************************************

void GLRenderSystem :: ApplyTransformations( const Vector3f & p_position, float * p_matrix)
{
	glPushMatrix();
	glTranslatef( p_position.x, p_position.y, p_position.z);
	glMultMatrixf( p_matrix);
}

//******************************************************************************

void GLRenderSystem :: RemoveTransformations()
{
	glPopMatrix();
}

//******************************************************************************

int GLRenderSystem :: LockLight()
{
	int l_iReturn = -1;

	if (m_setAvailableIndexes.size() > 0 && GLLightRenderer::sm_pLightShaders.size() > 0)
	{
		l_iReturn = *m_setAvailableIndexes.begin();
		m_setAvailableIndexes.erase( m_setAvailableIndexes.begin());
	}

	return l_iReturn;
}

//******************************************************************************

void GLRenderSystem :: UnlockLight( int p_iIndex)
{
	if (p_iIndex >= 0 && m_setAvailableIndexes.find( p_iIndex) == m_setAvailableIndexes.end())
	{
		m_setAvailableIndexes.insert( std::set <int>::value_type( p_iIndex));
	}
}

//******************************************************************************

void GLRenderSystem :: BeginOverlaysRendering()
{
	glMatrixMode( GL_PROJECTION);
	CheckGLError( "GLRenderSystem :: BeginOverlaysRendering - glMatrixMode");
	glLoadIdentity();
	CheckGLError( "GLRenderSystem :: BeginOverlaysRendering - glLoadIdentity");
	glOrtho( 0, 1.0, 0, 1.0, -1.0, 1.0);
	CheckGLError( "GLRenderSystem :: BeginOverlaysRendering - glOrtho");
	glDisable( GL_LIGHTING);
	CheckGLError( "GLRenderSystem :: BeginOverlaysRendering - glDisable( GL_LIGHTING)");
	glDisable( GL_DEPTH_TEST);
	CheckGLError( "GLRenderSystem :: BeginOverlaysRendering - glDisable( GL_DEPTH_TEST)");
}

//******************************************************************************

void GLRenderSystem :: EndOverlaysRendering()
{
	glMatrixMode( GL_MODELVIEW);
	CheckGLError( "GLRenderSystem :: EndOverlaysRendering - glMatrixMode");
	glLoadIdentity();
	CheckGLError( "GLRenderSystem :: EndOverlaysRendering - glLoadIdentity");
}

//******************************************************************************