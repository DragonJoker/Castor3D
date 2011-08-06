#include "GlRenderSystem/PrecompiledHeader.hpp"

#include "GlRenderSystem/GlRenderSystem.hpp"
#include "GlRenderSystem/GlCameraRenderer.hpp"
#include "GlRenderSystem/GlLightRenderer.hpp"
#include "GlRenderSystem/GlMaterialRenderer.hpp"
#include "GlRenderSystem/GlOverlayRenderer.hpp"
#include "GlRenderSystem/GlSubmeshRenderer.hpp"
#include "GlRenderSystem/GlTextureRenderer.hpp"
#include "GlRenderSystem/GlWindowRenderer.hpp"
#include "GlRenderSystem/GlShaderProgram.hpp"
#include "GlRenderSystem/GlShaderObject.hpp"
#include "GlRenderSystem/CgGlShaderProgram.hpp"
#include "GlRenderSystem/CgGlShaderObject.hpp"
#include "GlRenderSystem/OpenGl.hpp"

#include <Cg/cgGL.h>

using namespace Castor3D;

#define GL_LIGHT_MODEL_COLOR_CONTROL	0x81F8
#define GL_SEPARATE_SPECULAR_COLOR		0x81FA

bool GlRenderSystem :: sm_useVertexBufferObjects = false;
bool GlRenderSystem :: sm_extensionsInit = false;

GlRenderSystem :: GlRenderSystem()
	:	RenderSystem()
	,	m_iOpenGlMajor( 0)
	,	m_iOpenGlMinor( 0)
{
	Logger::LogMessage( cuT( "GlRenderSystem :: GlRenderSystem"));
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
	Logger::LogMessage( cuT( "GlRenderSystem :: Initialise"));

	if (sm_initialised)
	{
		return;
	}

	Logger::LogMessage( cuT( "************************************************************************************************************************"));
	Logger::LogMessage( cuT( "Initialising OpenGL"));

	InitOpenGlExtensions();

	sm_useMultiTexturing = false;

	if (glActiveTexture && glClientActiveTexture && glMultiTexCoord2)
	{
		Logger::LogMessage( cuT( "Using Multitexturing"));
		sm_useMultiTexturing = true;
	}

	sm_useVertexBufferObjects = false;
	if (glGenBuffers && glBindBuffer && glDeleteBuffers && glBufferData && glBufferSubData && glMultiDrawArrays)
	{
		Logger::LogMessage( cuT( "Using Vertex Buffer Objects"));
		sm_useVertexBufferObjects = true;
	}
	sm_initialised = true;

	sm_forceShaders = m_iOpenGlMajor >= 3;

	m_cgContext = cgCreateContext();

	_checkCgAvailableProfiles();
	CheckShaderSupport();

	GlPipeline::InitFunctions();

	Logger::LogMessage( cuT( "OpenGL Initialisation Ended"));
	Logger::LogMessage( cuT( "************************************************************************************************************************"));
}

void GlRenderSystem :: _checkCgAvailableProfiles()
{
	for (int i = 0 ; i < eSHADER_TYPE_COUNT ; i++)
	{
		m_mapSupportedProfiles[i] = CG_PROFILE_UNKNOWN;
	}

	_isCgProfileSupported( eSHADER_TYPE_HULL,		"gp5tcp"	);
	_isCgProfileSupported( eSHADER_TYPE_DOMAIN,		"gp5tep"	);
	_isCgProfileSupported( eSHADER_TYPE_VERTEX,		"gp5vp"		);
	_isCgProfileSupported( eSHADER_TYPE_GEOMETRY,	"gp5gp"		);
	_isCgProfileSupported( eSHADER_TYPE_PIXEL,		"gp5fp"		);
	_isCgProfileSupported( eSHADER_TYPE_VERTEX,		"gp4vp"		);
	_isCgProfileSupported( eSHADER_TYPE_GEOMETRY,	"gp4gp"		);
	_isCgProfileSupported( eSHADER_TYPE_PIXEL,		"gp4fp"		);
	_isCgProfileSupported( eSHADER_TYPE_VERTEX,		"glslv"		);
	_isCgProfileSupported( eSHADER_TYPE_GEOMETRY,	"glslg"		);
	_isCgProfileSupported( eSHADER_TYPE_PIXEL,		"glslf"		);
	_isCgProfileSupported( eSHADER_TYPE_PIXEL,		"arbfp1"	);
	_isCgProfileSupported( eSHADER_TYPE_VERTEX,		"arbvp1"	);

#	undef IsCgProfileSupported
}

void GlRenderSystem :: Delete()
{
	if (m_cgContext)
	{
		cgDestroyContext( m_cgContext);
		m_cgContext = nullptr;
	}

	Cleanup();
	map::deleteAll( m_contextMap);
}

void GlRenderSystem :: Cleanup()
{
	m_submeshesRenderers.clear();
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
			Logger::LogError( String( (char const *)glewGetErrorString( l_err)));
			sm_extensionsInit = false;
		}
		else
		{
			Logger::LogMessage( String( cuT( "Vendor : ")) + (char const *)glGetString( GL_VENDOR));
			Logger::LogMessage( String( cuT( "Renderer : ")) + (char const *)glGetString( GL_RENDERER));
			Logger::LogMessage( String( cuT( "OpenGL Version : ")) + (char const *)glGetString( GL_VERSION));

			sm_extensionsInit = true;

			if ( ! UseShaders())
			{
				sm_useShaders = true;

				m_iOpenGlMajor = 1;
				m_iOpenGlMinor = 0;

				if (GLEW_VERSION_4_1)
				{
					m_iOpenGlMajor = 4;
					m_iOpenGlMinor = 1;
					Logger::LogMessage( cuT( "Using version 4.1 (or higher) core functions"));
				}
				else if (GLEW_VERSION_4_0)
				{
					m_iOpenGlMajor = 4;
					m_iOpenGlMinor = 0;
					Logger::LogMessage( cuT( "Using version 4.0 core functions"));
				}
				else if (GLEW_VERSION_3_3)
				{
					m_iOpenGlMajor = 3;
					m_iOpenGlMinor = 3;
					Logger::LogMessage( cuT( "Using version 3.3 core functions"));
				}
				else if (GLEW_VERSION_3_2)
				{
					m_iOpenGlMajor = 3;
					m_iOpenGlMinor = 2;
					Logger::LogMessage( cuT( "Using version 3.2 core functions"));
				}
				else if (GLEW_VERSION_3_1)
				{
					m_iOpenGlMajor = 3;
					m_iOpenGlMinor = 1;
					Logger::LogMessage( cuT( "Using version 3.1 core functions"));
				}
				else if (GLEW_VERSION_3_0)
				{
					m_iOpenGlMajor = 3;
					m_iOpenGlMinor = 0;
					Logger::LogMessage( cuT( "Using version 3.0 core functions"));
				}
				else if (GLEW_VERSION_2_1)
				{
					m_iOpenGlMajor = 2;
					m_iOpenGlMinor = 1;
					Logger::LogMessage( cuT( "Using version 2.1 core functions"));
				}
				else if (GLEW_VERSION_2_0)
				{
					m_iOpenGlMajor = 2;
					m_iOpenGlMinor = 0;
					Logger::LogMessage( cuT( "Using version 2.0 core functions"));
				}
				else if (GLEW_VERSION_1_5)
				{
					m_iOpenGlMajor = 1;
					m_iOpenGlMinor = 5;
					Logger::LogMessage( cuT( "Using version 1.5 core functions"));
				}
				else if (GLEW_VERSION_1_4)
				{
					m_iOpenGlMajor = 1;
					m_iOpenGlMinor = 4;
					Logger::LogMessage( cuT( "Using version 1.4 core functions"));
				}
				else if (GLEW_VERSION_1_3)
				{
					m_iOpenGlMajor = 1;
					m_iOpenGlMinor = 3;
					Logger::LogMessage( cuT( "Using version 1.3 core functions"));
				}
				else if (GLEW_VERSION_1_2)
				{
					m_iOpenGlMajor = 1;
					m_iOpenGlMinor = 2;
					Logger::LogMessage( cuT( "Using version 1.2 core functions"));
				}

				if (GL_TRUE != glewGetExtension("GL_ARB_fragment_shader"))
				{
					Logger::LogWarning( cuT( "OpenGL : GL_ARB_fragment_shader extension is not available"));
					sm_useShaders = false;
				}

				if (GL_TRUE != glewGetExtension("GL_ARB_vertex_shader"))
				{
					Logger::LogWarning( cuT( "OpenGL : GL_ARB_vertex_shader extension is not available!"));
					sm_useShaders = false;
				}

				if (GL_TRUE != glewGetExtension("GL_ARB_shader_objects"))
				{
					Logger::LogWarning( cuT( "OpenGL : GL_ARB_shader_objects extension is not available!"));
					sm_useShaders = false;
				}

				if (UseShaders())
				{
					Logger::LogMessage( cuT( "Using OpenGL Shading Language"));
				}
			}

			OpenGl::Initialise();
		}
	}

	return sm_extensionsInit;
}

void GlRenderSystem :: CheckShaderSupport()
{
	sm_useShader[eSHADER_TYPE_VERTEX] = GLEW_VERSION_2_0 == GL_TRUE;
	sm_useShader[eSHADER_TYPE_PIXEL] = sm_useShader[eSHADER_TYPE_VERTEX];
	sm_useShader[eSHADER_TYPE_GEOMETRY] = glewGetExtension( "GL_EXT_geometry_shader4") == GL_TRUE;
	sm_useShader[eSHADER_TYPE_HULL] = glewGetExtension( "GL_ARB_tesselation_shader") == GL_TRUE;
	sm_useShader[eSHADER_TYPE_DOMAIN] = sm_useShader[eSHADER_TYPE_HULL];
}

void GlRenderSystem :: RenderAmbientLight( Colour const & p_clColour)
{
	if (RenderSystem::UseShaders())
	{
		if (m_pCurrentProgram)
		{
			if (m_pCurrentProgram->GetType() == ShaderProgramBase::eSHADER_LANGUAGE_GLSL)
			{
				static_cast <GlShaderProgram *>( m_pCurrentProgram)->SetAmbientLight( p_clColour);
			}
			else if (m_pCurrentProgram->GetType() == ShaderProgramBase::eSHADER_LANGUAGE_CG)
			{
				static_cast <CgGlShaderProgram *>( m_pCurrentProgram)->SetAmbientLight( p_clColour);
			}
		}
	}
	else
	{
		OpenGl::LightModelfv( GL_LIGHT_MODEL_AMBIENT, p_clColour.const_ptr());
	}
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
	CASTOR_TRACK;
	RenderSystem::BeginOverlaysRendering();

	if (m_iOpenGlMajor < 3)
	{
		OpenGl::Disable( GL_LIGHTING);
		OpenGl::Disable( GL_DEPTH_TEST);
	}
}

void GlRenderSystem :: DrawIndexedPrimitives( ePRIMITIVE_TYPE p_eType, size_t p_uiMinVertex, size_t p_uiVertexCount, size_t p_uiFirstIndex, size_t p_uiCount)
{
	CASTOR_TRACK;
	void const * l_pOffset = BUFFER_OFFSET( p_uiFirstIndex * 4);

	switch (p_eType)
	{
	case ePRIMITIVE_TYPE_TRIANGLES:			OpenGl::DrawElements( GL_TRIANGLES,      p_uiCount * 3,	GL_UNSIGNED_INT,	l_pOffset); break;
	case ePRIMITICE_TYPE_TRIANGLE_STRIPS:	OpenGl::DrawElements( GL_TRIANGLE_STRIP, p_uiCount + 2,	GL_UNSIGNED_INT,	l_pOffset); break;
	case ePRIMITIVE_TYPE_TRIANGLE_FAN:		OpenGl::DrawElements( GL_TRIANGLE_FAN,   p_uiCount + 1,	GL_UNSIGNED_INT,	l_pOffset); break;
	case ePRIMITIVE_TYPE_LINES:				OpenGl::DrawElements( GL_LINES,          p_uiCount * 2,	GL_UNSIGNED_INT,	l_pOffset); break;
	case ePRIMITIVE_TYPE_POINTS:			OpenGl::DrawElements( GL_POINTS,         p_uiCount,		GL_UNSIGNED_INT,	l_pOffset); break;
	}
}

void GlRenderSystem :: _isCgProfileSupported( eSHADER_TYPE p_eType, char const * p_szName)
{
	CGprofile l_cgProfile = cgGetProfile( p_szName);
	bool l_bSupported = cgGLIsProfileSupported( l_cgProfile) != 0;
	Logger::LogMessage( String( cuT( "RenderSystem :: CheckCgProfileSupport - Profile : ")) + p_szName + cuT( " - Support : ") + (l_bSupported ? cuT( "true") : cuT( "false")));

	if (l_bSupported)
	{
		if (m_mapSupportedProfiles[p_eType] == CG_PROFILE_UNKNOWN)
		{
			m_mapSupportedProfiles[p_eType] = l_cgProfile;
		}
	}
}

GlslShaderProgramPtr GlRenderSystem :: _createGlslShaderProgram()
{
	GlslShaderProgramPtr l_pReturn;
	l_pReturn = GlslShaderProgramPtr( new GlShaderProgram);
	return l_pReturn;
}

CgShaderProgramPtr GlRenderSystem :: _createCgShaderProgram()
{
	CgShaderProgramPtr l_pReturn;
	l_pReturn = CgGlShaderProgramPtr( new CgGlShaderProgram);
	return l_pReturn;
}

SubmeshRendererPtr GlRenderSystem :: _createSubmeshRenderer()
{
	SubmeshRendererPtr l_pReturn;
	l_pReturn = SubmeshRendererPtr( new GlSubmeshRenderer());
	return l_pReturn;
}

TextureRendererPtr GlRenderSystem :: _createTextureRenderer()
{
	TextureRendererPtr l_pReturn;
	l_pReturn = TextureRendererPtr( new GlTextureRenderer());
	return l_pReturn;
}

PassRendererPtr GlRenderSystem :: _createPassRenderer()
{
	PassRendererPtr l_pReturn;
	l_pReturn = PassRendererPtr( new GlPassRenderer());
	return l_pReturn;
}

CameraRendererPtr GlRenderSystem :: _createCameraRenderer()
{
	CameraRendererPtr l_pReturn;
	l_pReturn = CameraRendererPtr( new GlCameraRenderer());
	return l_pReturn;
}

LightRendererPtr GlRenderSystem :: _createLightRenderer()
{
	LightRendererPtr l_pReturn;
	l_pReturn = LightRendererPtr( new GlLightRenderer());
	l_pReturn->Initialise();
	return l_pReturn;
}

WindowRendererPtr GlRenderSystem :: _createWindowRenderer()
{
	WindowRendererPtr l_pReturn;
	l_pReturn = WindowRendererPtr( new GlWindowRenderer());
	return l_pReturn;
}

OverlayRendererPtr GlRenderSystem :: _createOverlayRenderer()
{
	OverlayRendererPtr l_pReturn;
	l_pReturn = OverlayRendererPtr( new GlOverlayRenderer());
	return l_pReturn;
}

IndexBufferPtr GlRenderSystem :: _createIndexBuffer()
{
	IndexBufferPtr l_pReturn;

	if (m_iOpenGlMajor < 3)
	{
		if (UseVertexBufferObjects())
		{
			l_pReturn = Root::GetSingleton()->GetBufferManager()->CreateBuffer<unsigned int, GlIndexBufferObject>();
		}
		else
		{
			l_pReturn = Root::GetSingleton()->GetBufferManager()->CreateBuffer<unsigned int, GlIndexArray>();
		}
	}
	else
	{
		l_pReturn = Root::GetSingleton()->GetBufferManager()->CreateBuffer<unsigned int, GlIndexBufferObject>();
	}

	return l_pReturn;
}

VertexBufferPtr GlRenderSystem :: _createVertexBuffer( const BufferElementDeclaration * p_pElements, size_t p_uiCount)
{
	VertexBufferPtr l_pReturn;

	if (m_iOpenGlMajor < 3)
	{
		if (UseVertexBufferObjects())
		{
			l_pReturn = Root::GetSingleton()->GetBufferManager()->CreateVertexBuffer<GlVertexBufferObject>( p_pElements, p_uiCount);
		}
		else
		{
			l_pReturn = Root::GetSingleton()->GetBufferManager()->CreateVertexBuffer<GlVertexArray>( p_pElements, p_uiCount);
		}
	}
	else
	{
		l_pReturn = Root::GetSingleton()->GetBufferManager()->CreateVertexBuffer<Gl3VertexBufferObject>( p_pElements, p_uiCount);
	}

	return l_pReturn;
}

TextureBufferObjectPtr GlRenderSystem :: _createTextureBuffer()
{
	TextureBufferObjectPtr l_pReturn;

	if (m_iOpenGlMajor >= 3)
	{
		l_pReturn = Root::GetSingleton()->GetBufferManager()->CreateBuffer<unsigned char, GlTextureBufferObject>();
	}

	return l_pReturn;
}