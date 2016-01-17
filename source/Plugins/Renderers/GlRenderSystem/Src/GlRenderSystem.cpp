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
#include "GlVertexBufferObject.hpp"
#include "GlIndexBufferObject.hpp"
#include "GlMatrixBufferObject.hpp"
#include "GlGeometryBuffers.hpp"
#include "GlDynamicTexture.hpp"
#include "GlStaticTexture.hpp"
#include "GlBlendState.hpp"
#include "GlDepthStencilState.hpp"
#include "GlRasteriserState.hpp"
#include "GlFrameVariableBuffer.hpp"
#include "GlSampler.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlRenderSystem::GlRenderSystem( Engine & p_engine )
		: RenderSystem( p_engine, eRENDERER_TYPE_OPENGL )
		, m_openGlMajor( 0 )
		, m_openGlMinor( 0 )
		, m_useVertexBufferObjects( false )
		, m_extensionsInit( false )
		, m_openGl( *this )
	{
		m_instancing = true;
		m_accumBuffer = true;
		m_pipelineImpl = std::make_shared< GlPipelineImpl >( GetOpenGl(), *m_pipeline );
		m_pipeline->UpdateImpl();
	}

	GlRenderSystem::~GlRenderSystem()
	{
		m_pipelineImpl.reset();
	}

	void GlRenderSystem::CheckShaderSupport()
	{
		m_useShader[eSHADER_TYPE_COMPUTE] = GetOpenGl().HasCSh();
		m_useShader[eSHADER_TYPE_HULL] = GetOpenGl().HasTSh();
		m_useShader[eSHADER_TYPE_DOMAIN] = GetOpenGl().HasTSh();
		m_useShader[eSHADER_TYPE_GEOMETRY] = GetOpenGl().HasGSh();
		m_useShader[eSHADER_TYPE_PIXEL] = GetOpenGl().HasPSh();
		m_useShader[eSHADER_TYPE_VERTEX] = GetOpenGl().HasVSh();
	}

	bool GlRenderSystem::InitOpenGlExtensions()
	{
		if ( !m_extensionsInit )
		{
			if ( !GetOpenGl().Initialise() )
			{
				m_extensionsInit = false;
			}
			else
			{
				Logger::LogInfo( cuT( "Vendor : " ) + GetOpenGl().GetVendor()	);
				Logger::LogInfo( cuT( "Renderer : " ) + GetOpenGl().GetRenderer()	);
				Logger::LogInfo( cuT( "OpenGL Version : " ) + GetOpenGl().GetStrVersion()	);
				m_extensionsInit = true;
				m_instancing = GetOpenGl().HasInstancing();

				m_openGlMajor = GetOpenGl().GetVersion() / 10;
				m_openGlMinor = GetOpenGl().GetVersion() % 10;

				Logger::LogInfo( StringStream() << cuT( "Using version " ) << m_openGlMajor << cuT( "." ) << m_openGlMinor << cuT( " core functions" ) );
				m_useShader[eSHADER_TYPE_COMPUTE] = GetOpenGl().HasCSh();
				m_useShader[eSHADER_TYPE_HULL] = GetOpenGl().HasTSh();
				m_useShader[eSHADER_TYPE_DOMAIN] = GetOpenGl().HasTSh();
				m_useShader[eSHADER_TYPE_GEOMETRY] = GetOpenGl().HasGSh();
				m_useShader[eSHADER_TYPE_PIXEL] = GetOpenGl().HasPSh();
				m_useShader[eSHADER_TYPE_VERTEX] = GetOpenGl().HasVSh();

				if ( m_useShader[eSHADER_TYPE_COMPUTE] )
				{
					m_maxShaderModel = eSHADER_MODEL_5;
				}
				else if ( m_useShader[eSHADER_TYPE_HULL] )
				{
					m_maxShaderModel = eSHADER_MODEL_4;
				}
				else if ( m_useShader[eSHADER_TYPE_GEOMETRY] )
				{
					m_maxShaderModel = eSHADER_MODEL_3;
				}
				else if ( m_useShader[eSHADER_TYPE_PIXEL] )
				{
					m_maxShaderModel = eSHADER_MODEL_2;
				}
				else
				{
					m_maxShaderModel = eSHADER_MODEL_1;
				}

				m_nonPowerOfTwoTextures = GetOpenGl().HasNonPowerOfTwoTextures();
				REQUIRE( m_maxShaderModel >= eSHADER_MODEL_2 );
			}
		}

		return m_extensionsInit;
	}

	ContextSPtr GlRenderSystem::CreateContext()
	{
		return std::make_shared< GlContext >( *this, GetOpenGl() );
	}

	GeometryBuffersSPtr GlRenderSystem::CreateGeometryBuffers( VertexBufferUPtr p_pVertexBuffer, IndexBufferUPtr p_pIndexBuffer, MatrixBufferUPtr p_pMatrixBuffer, eTOPOLOGY p_topology )
	{
		return std::make_shared< GlGeometryBuffers >( GetOpenGl(), std::move( p_pVertexBuffer ), std::move( p_pIndexBuffer ), std::move( p_pMatrixBuffer ), p_topology );
	}

	DepthStencilStateSPtr GlRenderSystem::CreateDepthStencilState()
	{
		return std::make_shared< GlDepthStencilState >( this, GetOpenGl() );
	}

	RasteriserStateSPtr GlRenderSystem::CreateRasteriserState()
	{
		return std::make_shared< GlRasteriserState >( this, GetOpenGl() );
	}

	BlendStateSPtr GlRenderSystem::CreateBlendState()
	{
		return std::make_shared< GlBlendState >( this, GetOpenGl() );
	}

	FrameVariableBufferSPtr GlRenderSystem::CreateFrameVariableBuffer( Castor::String const & p_name )
	{
		return std::make_shared< GlFrameVariableBuffer >( GetOpenGl(), p_name, *this );
	}

	BillboardListSPtr GlRenderSystem::CreateBillboardsList( Castor3D::SceneSPtr p_scene )
	{
		return std::make_shared< GlBillboardList >( p_scene, *this, GetOpenGl() );
	}

	SamplerSPtr GlRenderSystem::CreateSampler( Castor::String const & p_name )
	{
		return std::make_shared< GlSampler >( GetOpenGl(), this, p_name );
	}

	RenderTargetSPtr GlRenderSystem::CreateRenderTarget( eTARGET_TYPE p_type )
	{
		return std::make_shared< GlRenderTarget >( GetOpenGl(), this, p_type );
	}

	RenderWindowSPtr GlRenderSystem::CreateRenderWindow()
	{
		return std::make_shared< GlRenderWindow >( GetOpenGl(), this );
	}

	ShaderProgramBaseSPtr GlRenderSystem::CreateGlslShaderProgram()
	{
		return std::make_shared< GlShaderProgram >( GetOpenGl(), *this );
	}

	ShaderProgramBaseSPtr GlRenderSystem::CreateShaderProgram()
	{
		return std::make_shared< GlShaderProgram >( GetOpenGl(), *this );
	}

	OverlayRendererSPtr GlRenderSystem::CreateOverlayRenderer()
	{
		return std::make_shared< GlOverlayRenderer >( GetOpenGl(), *this );
	}

	std::shared_ptr< Castor3D::GpuBuffer< uint32_t > > GlRenderSystem::CreateIndexBuffer( CpuBuffer< uint32_t > * p_buffer )
	{
		return std::make_shared< GlIndexBufferObject >( *this, GetOpenGl(), p_buffer );
	}

	std::shared_ptr< Castor3D::GpuBuffer< uint8_t > > GlRenderSystem::CreateVertexBuffer( BufferDeclaration const & p_declaration, CpuBuffer< uint8_t > * p_buffer )
	{
		return std::make_shared< GlVertexBufferObject >( *this, GetOpenGl(), p_declaration, p_buffer );
	}

	std::shared_ptr< Castor3D::GpuBuffer< real > > GlRenderSystem::CreateMatrixBuffer( CpuBuffer< real > * p_buffer )
	{
		return std::make_shared< GlMatrixBufferObject >( *this, GetOpenGl(), p_buffer );
	}

	StaticTextureSPtr GlRenderSystem::CreateStaticTexture()
	{
		return std::make_shared< GlStaticTexture >( GetOpenGl(), *this );
	}

	DynamicTextureSPtr GlRenderSystem::CreateDynamicTexture( uint8_t p_cpuAccess, uint8_t p_gpuAccess )
	{
		return std::make_shared< GlDynamicTexture >( GetOpenGl(), *this, p_cpuAccess, p_gpuAccess );
	}

	void GlRenderSystem::DoInitialise()
	{
		if ( !m_initialised )
		{
			InitOpenGlExtensions();

			m_useVertexBufferObjects = GetOpenGl().HasVbo();
			m_initialised = true;
			CheckShaderSupport();
			m_pipeline->Initialise();
		}
	}

	void GlRenderSystem::DoCleanup()
	{
		m_extensionsInit = false;
		m_initialised = false;
		GetOpenGl().Cleanup();
	}

#if C3D_TRACE_OBJECTS

	void GlRenderSystem::Track( void * p_object, std::string const & p_name, std::string const & p_file, int p_line )
	{
		std::string l_name;

		if ( DoTrack( p_object, p_name, p_file, p_line, l_name ) )
		{
		}
	}

	void GlRenderSystem::Untrack( void * p_object )
	{
		ObjectDeclaration l_decl;

		if ( DoUntrack( p_object, l_decl ) )
		{
		}
	}

#endif

}
