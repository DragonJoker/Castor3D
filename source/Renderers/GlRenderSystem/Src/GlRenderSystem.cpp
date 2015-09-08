#include "GlRenderSystem.hpp"

#include "GlOverlayRenderer.hpp"
#include "GlRenderWindow.hpp"
#include "GlRenderTarget.hpp"
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
#include "GlSampler.hpp"

#include <Logger.hpp>

using namespace GlRender;
using namespace Castor3D;
using namespace Castor;

GlRenderSystem::GlRenderSystem( Engine * p_pEngine )
	: RenderSystem( p_pEngine, eRENDERER_TYPE_OPENGL )
	, m_iOpenGlMajor( 0 )
	, m_iOpenGlMinor( 0 )
	, m_useVertexBufferObjects( false )
	, m_extensionsInit( false )
	, m_gl( *this )
{
	Logger::LogInfo( cuT( "GlRenderSystem::GlRenderSystem" ) );
	m_bAccumBuffer = true;
	m_pPipeline = new GlPipeline( m_gl, this );
}

GlRenderSystem::~GlRenderSystem()
{
	delete m_pPipeline;
}

void GlRenderSystem::CheckShaderSupport()
{
	m_useShader[eSHADER_TYPE_COMPUTE] = m_gl.HasCSh();
	m_useShader[eSHADER_TYPE_HULL] = m_gl.HasTSh();
	m_useShader[eSHADER_TYPE_DOMAIN] = m_gl.HasTSh();
	m_useShader[eSHADER_TYPE_GEOMETRY] = m_gl.HasGSh();
	m_useShader[eSHADER_TYPE_PIXEL] = m_gl.HasPSh();
	m_useShader[eSHADER_TYPE_VERTEX] = m_gl.HasVSh();
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
			Logger::LogInfo( cuT( "Vendor : " ) + m_gl.GetVendor()	);
			Logger::LogInfo( cuT( "Renderer : " ) + m_gl.GetRenderer()	);
			Logger::LogInfo( cuT( "OpenGL Version : " ) + m_gl.GetStrVersion()	);
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
				Logger::LogInfo( StringStream() << cuT( "Using version " ) << m_iOpenGlMajor << cuT( "." ) << m_iOpenGlMinor << cuT( " core functions" ) );
				m_useShader[eSHADER_TYPE_COMPUTE] = m_gl.HasCSh();
				m_useShader[eSHADER_TYPE_HULL] = m_gl.HasTSh();
				m_useShader[eSHADER_TYPE_DOMAIN] = m_gl.HasTSh();
				m_useShader[eSHADER_TYPE_GEOMETRY] = m_gl.HasGSh();
				m_useShader[eSHADER_TYPE_PIXEL] = m_gl.HasPSh();
				m_useShader[eSHADER_TYPE_VERTEX] = m_gl.HasVSh();
				m_bNonPowerOfTwoTextures = m_gl.HasNonPowerOfTwoTextures();

				if ( !m_useShader[eSHADER_TYPE_VERTEX] || !m_useShader[eSHADER_TYPE_PIXEL] )
				{
					m_useShaders = false;
				}

				if ( !UseShaders() )
				{
					Logger::LogInfo( cuT( "Not using OpenGL Shading Language" ) );
				}
			}
		}
	}

	return m_extensionsInit;
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

FrameVariableBufferSPtr GlRenderSystem::CreateFrameVariableBuffer( Castor::String const & p_strName )
{
	return std::make_shared< GlFrameVariableBuffer >( m_gl, p_strName, this );
}

BillboardListSPtr GlRenderSystem::CreateBillboardsList( Castor3D::SceneSPtr p_pScene )
{
	return std::make_shared< GlBillboardList >( p_pScene, this, m_gl );
}

SamplerSPtr GlRenderSystem::CreateSampler( Castor::String const & p_name )
{
	return std::make_shared< GlSampler >( m_gl, this, p_name );
}

RenderTargetSPtr GlRenderSystem::CreateRenderTarget( eTARGET_TYPE p_type )
{
	return std::make_shared< GlRenderTarget >( m_gl, this, p_type );
}

RenderWindowSPtr GlRenderSystem::CreateRenderWindow()
{
	return std::make_shared< GlRenderWindow >( m_gl, this );
}

ShaderProgramBaseSPtr GlRenderSystem::CreateGlslShaderProgram()
{
	return std::make_shared< GlShaderProgram >( m_gl, this );
}

ShaderProgramBaseSPtr GlRenderSystem::CreateShaderProgram()
{
	return std::make_shared< GlShaderProgram >( m_gl, this );
}

OverlayRendererSPtr GlRenderSystem::CreateOverlayRenderer()
{
	return std::make_shared< GlOverlayRenderer >( m_gl, this );
}

std::shared_ptr< Castor3D::GpuBuffer< uint32_t > > GlRenderSystem::CreateIndexBuffer( CpuBuffer< uint32_t > * p_pBuffer )
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

std::shared_ptr< Castor3D::GpuBuffer< uint8_t > > GlRenderSystem::CreateVertexBuffer( BufferDeclaration const & p_declaration, CpuBuffer< uint8_t > * p_pBuffer )
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

std::shared_ptr< Castor3D::GpuBuffer< real > > GlRenderSystem::CreateMatrixBuffer( CpuBuffer< real > * p_pBuffer )
{
	return std::make_shared< GlMatrixBufferObject >( m_gl, p_pBuffer );
}

std::shared_ptr< Castor3D::GpuBuffer< uint8_t > > GlRenderSystem::CreateTextureBuffer( CpuBuffer< uint8_t > * p_pBuffer )
{
	std::shared_ptr< Castor3D::GpuBuffer< uint8_t > > l_pReturn;

	if ( m_iOpenGlMajor >= 3 )
	{
		l_pReturn = std::make_shared< GlTextureBufferObject >( m_gl, p_pBuffer );
	}

	return l_pReturn;
}

StaticTextureSPtr GlRenderSystem::CreateStaticTexture()
{
	return std::make_shared< GlStaticTexture >( m_gl, this );
}

DynamicTextureSPtr GlRenderSystem::CreateDynamicTexture()
{
	return std::make_shared< GlDynamicTexture >( m_gl, this );
}

void GlRenderSystem::DoInitialise()
{
	if ( m_bInitialised )
	{
		return;
	}

	Logger::LogInfo( cuT( "***********************************************************************************************************************" ) );
	Logger::LogInfo( cuT( "Initialising OpenGL" ) );
	InitOpenGlExtensions();
	m_useMultiTexturing = false;

	if ( m_gl.HasMultiTexturing() )
	{
		Logger::LogInfo( cuT( "Using Multitexturing" ) );
		m_useMultiTexturing = true;
	}

	m_useVertexBufferObjects = false;

	if ( m_gl.HasVbo() )
	{
		Logger::LogInfo( cuT( "Using Vertex Buffer Objects" ) );
		m_useVertexBufferObjects = true;
	}

	m_bInitialised = true;
	m_forceShaders = m_iOpenGlMajor >= 3;
	CheckShaderSupport();
	m_pPipeline->Initialise();
	Logger::LogInfo( cuT( "OpenGL Initialisation Ended" ) );
	Logger::LogInfo( cuT( "***********************************************************************************************************************" ) );
}

void GlRenderSystem::DoCleanup()
{
	m_gl.Cleanup();
}

#if !defined( NDEBUG )

void GlRenderSystem::Track( void * p_object, std::string const & p_name, std::string const & p_file, int p_line )
{
	std::string l_name;

	if ( DoTrack( p_object, p_name, p_file, p_line, l_name ) )
	{
	}
}

void GlRenderSystem::UnTrack( void * p_object )
{
	ObjectDeclaration l_decl;

	if ( DoUntrack( p_object, l_decl ) )
	{
	}
}

#endif
