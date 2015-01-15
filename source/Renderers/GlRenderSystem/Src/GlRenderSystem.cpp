#include "GlRenderSystem.hpp"
#include "GlCameraRenderer.hpp"
#include "GlLightRenderer.hpp"
#include "GlMaterialRenderer.hpp"
#include "GlOverlayRenderer.hpp"
#include "GlSamplerRenderer.hpp"
#include "GlSubmeshRenderer.hpp"
#include "GlTextureRenderer.hpp"
#include "GlWindowRenderer.hpp"
#include "GlTargetRenderer.hpp"
#include "GlShaderProgram.hpp"
#include "GlShaderObject.hpp"
#include "OpenGl.hpp"
#include "GlPipeline.hpp"
#include "GlContext.hpp"
#include "GlBillboardList.hpp"
#include "GlVertexArray.hpp"
#include "GlIndexArray.hpp"
#include "GlVertexBufferObject.hpp"
#include "GlIndexBufferObject.hpp"
#include "GlMatrixBufferObject.hpp"
#include "GlTextureBuffer.hpp"
#include "Gl3VertexBufferObject.hpp"
#include "GlGeometryBuffers.hpp"
#include "GlDynamicTexture.hpp"
#include "GlStaticTexture.hpp"
#include "GlBlendState.hpp"
#include "GlDepthStencilState.hpp"
#include "GlRasteriserState.hpp"
#include "GlFrameVariableBuffer.hpp"

#include <Logger.hpp>

using namespace GlRender;
using namespace Castor3D;
using namespace Castor;

GlRenderSystem::GlRenderSystem( Engine * p_pEngine )
	:	RenderSystem( p_pEngine, eRENDERER_TYPE_OPENGL )
	,	m_iOpenGlMajor( 0	)
	,	m_iOpenGlMinor( 0	)
	,	m_useVertexBufferObjects( false	)
	,	m_extensionsInit( false	)
{
	m_bAccumBuffer = true;
	m_pPipeline = new GlPipeline( m_gl, this );

	Logger::LogMessage( cuT( "GlRenderSystem::GlRenderSystem" ) );
	m_setAvailableIndexes.insert( GL_LIGHT0 );
	m_setAvailableIndexes.insert( GL_LIGHT1 );
	m_setAvailableIndexes.insert( GL_LIGHT2 );
	m_setAvailableIndexes.insert( GL_LIGHT3 );
	m_setAvailableIndexes.insert( GL_LIGHT4 );
	m_setAvailableIndexes.insert( GL_LIGHT5 );
	m_setAvailableIndexes.insert( GL_LIGHT6 );
	m_setAvailableIndexes.insert( GL_LIGHT7 );
}

GlRenderSystem::~GlRenderSystem()
{
	delete m_pPipeline;
	Delete();
}

void GlRenderSystem::Initialise( String const & p_strExtensions )
{
	m_strExtensions = p_strExtensions;
	RenderSystem::Initialise();
}

void GlRenderSystem::Delete()
{
	Cleanup();
	m_gl.Cleanup();
	Logger::Cleanup();
}

bool GlRenderSystem::CheckSupport( eSHADER_MODEL p_eProfile )
{
	bool l_bReturn = false;

	switch ( p_eProfile )
	{
	case eSHADER_MODEL_1:
		l_bReturn = m_gl.HasVSh();
		break;

	case eSHADER_MODEL_2:
		l_bReturn = m_gl.HasPSh();
		break;

	case eSHADER_MODEL_3:
		l_bReturn = m_gl.HasGSh();
		break;

	case eSHADER_MODEL_4:
		l_bReturn = m_gl.HasTSh();
		break;

	case eSHADER_MODEL_5:
		l_bReturn = m_gl.HasCSh();
		break;

	default:
		l_bReturn = false;
	}

	return l_bReturn;
}

bool GlRenderSystem::InitOpenGlExtensions()
{
	if ( !m_extensionsInit )
	{
		if ( !m_gl.Initialise() )
		{
			m_extensionsInit = false;
		}
		else
		{
			Logger::LogMessage( cuT( "Vendor : "	) + m_gl.GetVendor()	);
			Logger::LogMessage( cuT( "Renderer : "	) + m_gl.GetRenderer()	);
			Logger::LogMessage( cuT( "OpenGL Version : "	) + m_gl.GetStrVersion()	);
			m_extensionsInit = true;
			m_bInstancing = m_gl.HasInstancing();

			if ( !UseShaders() )
			{
				m_useShaders = true;
				m_iOpenGlMajor = m_gl.GetVersion() / 10;
				m_iOpenGlMinor = m_gl.GetVersion() % 10;
#if C3DGL_LIMIT_TO_2_1
				m_iOpenGlMajor = 2;
				m_iOpenGlMinor = 1;
#endif
				Logger::LogMessage( cuT( "Using version %d.%d core functions" ), m_iOpenGlMajor, m_iOpenGlMinor );
				m_useShader[eSHADER_TYPE_COMPUTE]	= m_gl.HasCSh();
				m_useShader[eSHADER_TYPE_HULL]		= m_gl.HasTSh();
				m_useShader[eSHADER_TYPE_DOMAIN]	= m_gl.HasTSh();
				m_useShader[eSHADER_TYPE_GEOMETRY]	= m_gl.HasGSh();
				m_useShader[eSHADER_TYPE_PIXEL]		= m_gl.HasPSh();
				m_useShader[eSHADER_TYPE_VERTEX]	= m_gl.HasVSh();
				m_bNonPowerOfTwoTextures = m_gl.HasNonPowerOfTwoTextures();

				if ( !m_useShader[eSHADER_TYPE_VERTEX] || !m_useShader[eSHADER_TYPE_PIXEL] )
				{
					m_useShaders = false;
				}

				if ( !UseShaders() )
				{
					Logger::LogMessage( cuT( "Not using OpenGL Shading Language" ) );
				}
			}
		}
	}

	return m_extensionsInit;
}

ContextSPtr GlRenderSystem::CreateContext()
{
	return std::make_shared< GlContext >( m_gl );
}

GeometryBuffersSPtr GlRenderSystem::CreateGeometryBuffers( VertexBufferUPtr p_pVertexBuffer, IndexBufferUPtr p_pIndexBuffer, MatrixBufferUPtr p_pMatrixBuffer )
{
	return std::make_shared< GlGeometryBuffers >( m_gl, std::move( p_pVertexBuffer ), std::move( p_pIndexBuffer ), std::move( p_pMatrixBuffer ) );
}

DepthStencilStateSPtr GlRenderSystem::CreateDepthStencilState()
{
	return std::make_shared< GlDepthStencilState >( m_gl );
}

RasteriserStateSPtr GlRenderSystem::CreateRasteriserState()
{
	return std::make_shared< GlRasteriserState >( m_gl );
}

BlendStateSPtr GlRenderSystem::CreateBlendState()
{
	return std::make_shared< GlBlendState >( m_gl );
}

void GlRenderSystem::CheckShaderSupport()
{
	m_useShader[eSHADER_TYPE_COMPUTE]	= m_gl.HasCSh();
	m_useShader[eSHADER_TYPE_HULL]		= m_gl.HasTSh();
	m_useShader[eSHADER_TYPE_DOMAIN]	= m_gl.HasTSh();
	m_useShader[eSHADER_TYPE_GEOMETRY]	= m_gl.HasGSh();
	m_useShader[eSHADER_TYPE_PIXEL]		= m_gl.HasPSh();
	m_useShader[eSHADER_TYPE_VERTEX]	= m_gl.HasVSh();
}

FrameVariableBufferSPtr GlRenderSystem::CreateFrameVariableBuffer( Castor::String const & p_strName )
{
	return std::make_shared< GlFrameVariableBuffer >( m_gl, p_strName, this );
}

BillboardListSPtr GlRenderSystem::CreateBillboardsList( Castor3D::SceneSPtr p_pScene )
{
	return std::make_shared< GlBillboardList >( p_pScene, this, m_gl );
}

int GlRenderSystem::LockLight()
{
	int l_iReturn = -1;

	if ( m_setAvailableIndexes.size() > 0 )
	{
		l_iReturn = *m_setAvailableIndexes.begin();
		m_setAvailableIndexes.erase( m_setAvailableIndexes.begin() );
	}

	return l_iReturn;
}

void GlRenderSystem::UnlockLight( int p_iIndex )
{
	if ( p_iIndex >= 0 && m_setAvailableIndexes.find( p_iIndex ) == m_setAvailableIndexes.end() )
	{
		m_setAvailableIndexes.insert( std::set <int>::value_type( p_iIndex ) );
	}
}

void GlRenderSystem::BeginOverlaysRendering( Castor::Size const & p_size )
{
	RenderSystem::BeginOverlaysRendering( p_size );

	if ( m_iOpenGlMajor < 3 )
	{
//		m_gl.Disable( eGL_TWEAK_LIGHTING		);
//		m_gl.Disable( eGL_TWEAK_DEPTH_TEST	);
	}
}

void GlRenderSystem::EndOverlaysRendering()
{
	if ( m_iOpenGlMajor < 3 )
	{
//		m_gl.Enable( eGL_TWEAK_LIGHTING		);
//		m_gl.Enable( eGL_TWEAK_DEPTH_TEST	);
	}

	RenderSystem::EndOverlaysRendering();
}

void GlRenderSystem::DoInitialise()
{
	if ( m_bInitialised )
	{
		return;
	}

	Logger::LogMessage( cuT( "***********************************************************************************************************************" ) );
	Logger::LogMessage( cuT( "Initialising OpenGL" ) );
	InitOpenGlExtensions();
	m_useMultiTexturing = false;

	if ( m_gl.HasMultiTexturing() )
	{
		Logger::LogMessage( cuT( "Using Multitexturing" ) );
		m_useMultiTexturing = true;
	}

	m_useVertexBufferObjects = false;

	if ( m_gl.HasVbo() )
	{
		Logger::LogMessage( cuT( "Using Vertex Buffer Objects" ) );
		m_useVertexBufferObjects = true;
	}

	m_bInitialised = true;
	m_forceShaders = m_iOpenGlMajor >= 3;
	CheckShaderSupport();
	m_pPipeline->Initialise();
	Logger::LogMessage( cuT( "OpenGL Initialisation Ended" ) );
	Logger::LogMessage( cuT( "***********************************************************************************************************************" ) );
}

void GlRenderSystem::DoCleanup()
{
}

void GlRenderSystem::DoRenderAmbientLight( Colour const & p_clColour )
{
	if ( !RenderSystem::ForceShaders() )
	{
		m_gl.LightModel( GL_LIGHT_MODEL_AMBIENT, p_clColour.const_ptr() );
	}
}

ShaderProgramBaseSPtr GlRenderSystem::DoCreateGlslShaderProgram()
{
	return std::make_shared< GlShaderProgram >( m_gl, this );
}

ShaderProgramBaseSPtr GlRenderSystem::DoCreateHlslShaderProgram()
{
	return nullptr;
}

ShaderProgramBaseSPtr GlRenderSystem::DoCreateShaderProgram()
{
	return std::make_shared< GlShaderProgram >( m_gl, this );
}

SubmeshRendererSPtr GlRenderSystem::DoCreateSubmeshRenderer()
{
	return std::make_shared< GlSubmeshRenderer >( m_gl, this );
}

TextureRendererSPtr GlRenderSystem::DoCreateTextureRenderer()
{
	return std::make_shared< GlTextureRenderer >( m_gl, this );
}

PassRendererSPtr GlRenderSystem::DoCreatePassRenderer()
{
	return std::make_shared< GlPassRenderer >( m_gl, this );
}

CameraRendererSPtr GlRenderSystem::DoCreateCameraRenderer()
{
	return std::make_shared< GlCameraRenderer >( m_gl, this );
}

LightRendererSPtr GlRenderSystem::DoCreateLightRenderer()
{
	LightRendererSPtr l_pReturn = std::make_shared< GlLightRenderer >( m_gl, this );
	l_pReturn->Initialise();
	return l_pReturn;
}

WindowRendererSPtr GlRenderSystem::DoCreateWindowRenderer()
{
	return std::make_shared< GlWindowRenderer >( m_gl, this );
}

OverlayRendererSPtr GlRenderSystem::DoCreateOverlayRenderer()
{
	return std::make_shared< GlOverlayRenderer >( m_gl, this );
}

TargetRendererSPtr GlRenderSystem::DoCreateTargetRenderer()
{
	return std::make_shared< GlTargetRenderer >( m_gl, this );
}

SamplerRendererSPtr GlRenderSystem::DoCreateSamplerRenderer()
{
	return std::make_shared< GlSamplerRenderer >( m_gl, this );
}

std::shared_ptr< Castor3D::GpuBuffer< uint32_t > > GlRenderSystem::DoCreateIndexBuffer( CpuBuffer< uint32_t > * p_pBuffer )
{
	std::shared_ptr< Castor3D::GpuBuffer< uint32_t > > l_pReturn;

	if ( m_iOpenGlMajor < 3 && !UseVertexBufferObjects() )
	{
		l_pReturn = std::make_shared< GlIndexArray >( m_gl, p_pBuffer );
	}
	else
	{
		l_pReturn = std::make_shared< GlIndexBufferObject >( m_gl, p_pBuffer );
	}

	return l_pReturn;
}

std::shared_ptr< Castor3D::GpuBuffer< uint8_t > > GlRenderSystem::DoCreateVertexBuffer( BufferDeclaration const & p_declaration, CpuBuffer< uint8_t > * p_pBuffer )
{
	std::shared_ptr< Castor3D::GpuBuffer< uint8_t > > l_pReturn;

	if ( m_iOpenGlMajor < 3 )
	{
		if ( UseVertexBufferObjects() )
		{
			l_pReturn = std::make_shared< GlVertexBufferObject >( m_gl, p_declaration, p_pBuffer );
		}
		else
		{
			l_pReturn = std::make_shared< GlVertexArray >( m_gl, p_declaration, p_pBuffer );
		}
	}
	else
	{
		l_pReturn = std::make_shared< Gl3VertexBufferObject >( m_gl, p_declaration, p_pBuffer );
	}

	return l_pReturn;
}

std::shared_ptr< Castor3D::GpuBuffer< real > > GlRenderSystem::DoCreateMatrixBuffer( CpuBuffer< real > * p_pBuffer )
{
	return std::make_shared< GlMatrixBufferObject >( m_gl, p_pBuffer );
}

std::shared_ptr< Castor3D::GpuBuffer< uint8_t > > GlRenderSystem::DoCreateTextureBuffer( CpuBuffer< uint8_t > * p_pBuffer )
{
	std::shared_ptr< Castor3D::GpuBuffer< uint8_t > > l_pReturn;

	if ( m_iOpenGlMajor >= 3 )
	{
		l_pReturn = std::make_shared< GlTextureBufferObject >( m_gl, p_pBuffer );
	}

	return l_pReturn;
}

StaticTextureSPtr GlRenderSystem::DoCreateStaticTexture()
{
	return std::make_shared< GlStaticTexture >( m_gl, this );
}

DynamicTextureSPtr GlRenderSystem::DoCreateDynamicTexture()
{
	return std::make_shared< GlDynamicTexture >( m_gl, this );
}
