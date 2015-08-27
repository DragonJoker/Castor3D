#include "Dx9RenderSystem.hpp"
#include "Dx9SubmeshRenderer.hpp"
#include "Dx9CameraRenderer.hpp"
#include "Dx9LightRenderer.hpp"
#include "Dx9OverlayRenderer.hpp"
#include "Dx9MaterialRenderer.hpp"
#include "Dx9TextureRenderer.hpp"
#include "Dx9TargetRenderer.hpp"
#include "Dx9WindowRenderer.hpp"
#include "Dx9ShaderProgram.hpp"
#include "Dx9ShaderObject.hpp"
#include "Dx9OneFrameVariable.hpp"
#include "Dx9PointFrameVariable.hpp"
#include "Dx9MatrixFrameVariable.hpp"
#include "Dx9Context.hpp"
#include "Dx9BillboardList.hpp"
#include "Dx9VertexBuffer.hpp"
#include "Dx9IndexBuffer.hpp"
#include "Dx9GeometryBuffers.hpp"
#include "Dx9DepthStencilState.hpp"
#include "Dx9RasteriserState.hpp"
#include "Dx9BlendState.hpp"
#include "Dx9FrameVariableBuffer.hpp"
#include "Dx9SamplerRenderer.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;
using namespace Dx9Render;

DxRenderSystem::DxRenderSystem( Engine * p_pEngine, Logger * p_pLogger )
	:	RenderSystem( p_pEngine, eRENDERER_TYPE_DIRECT3D9 )
	,	m_pDirect3D( NULL )
	,	m_pDevice( NULL )
{
	m_pPipeline = new DxPipeline( this );

	if ( p_pLogger )
	{
		Logger::Initialise( p_pLogger );
	}
	else
	{
#if defined( NDEBUG )
		Logger::Initialise( eLOG_TYPE_MESSAGE );
#else
		Logger::Initialise( eLOG_TYPE_DEBUG );
#endif
		Logger::SetFileName( File::DirectoryGetCurrent() / cuT( "DxRenderSystem.log" ) );
	}

	Logger::LogMessage( cuT( "DxRenderSystem::DxRenderSystem" ) );
	m_setAvailableIndexes.insert( 0 );
	m_setAvailableIndexes.insert( 1 );
	m_setAvailableIndexes.insert( 2 );
	m_setAvailableIndexes.insert( 3 );
	m_setAvailableIndexes.insert( 4 );
	m_setAvailableIndexes.insert( 5 );
	m_setAvailableIndexes.insert( 6 );
	m_setAvailableIndexes.insert( 7 );
	m_bNonPowerOfTwoTextures = true;
}

DxRenderSystem::~DxRenderSystem()
{
	delete m_pPipeline;
	Delete();
}

void DxRenderSystem::Delete()
{
	Cleanup();
	Logger::Cleanup();
}

bool DxRenderSystem::CheckSupport( eSHADER_MODEL p_eProfile )
{
	bool l_bReturn = false;

	switch ( p_eProfile )
	{
	case eSHADER_MODEL_1:
	case eSHADER_MODEL_2:
		l_bReturn = true;
		break;

	default:
		l_bReturn = false;
		break;
	}

	return l_bReturn;
}

void DxRenderSystem::CheckShaderSupport()
{
	m_useShader[eSHADER_TYPE_VERTEX		] = true;
	m_useShader[eSHADER_TYPE_PIXEL		] = true;
	m_useShader[eSHADER_TYPE_GEOMETRY	] = false;
	m_useShader[eSHADER_TYPE_HULL		] = false;
	m_useShader[eSHADER_TYPE_DOMAIN		] = false;
	m_useShader[eSHADER_TYPE_COMPUTE	] = false;
}

bool DxRenderSystem::InitialiseDevice( HWND p_hWnd, D3DPRESENT_PARAMETERS * p_presentParameters )
{
	bool l_bReturn = true;

	if ( !m_pDevice )
	{
		if ( m_pDirect3D )
		{
			HRESULT l_hr = m_pDirect3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, p_hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, p_presentParameters, &m_pDevice );

			if ( l_hr == S_OK )
			{
				m_pDevice->SetRenderState(	D3DRS_SPECULARENABLE,	TRUE	);
				m_useShaders = true;
			}
			else
			{
				Logger::LogWarning( "DxContext ::InitialiseDevice - Error creating device : 0x%X", uint32_t( l_hr ) );
				l_bReturn = false;
			}
		}
	}

	return l_bReturn;
}

ContextSPtr DxRenderSystem::CreateContext()
{
	return std::make_shared< DxContext >();
}

GeometryBuffersSPtr DxRenderSystem::CreateGeometryBuffers( Castor3D::VertexBufferUPtr p_pVertexBuffer, Castor3D::IndexBufferUPtr p_pIndexBuffer, Castor3D::MatrixBufferUPtr p_pMatrixBuffer )
{
	return std::make_shared< DxGeometryBuffers >( this, std::move( p_pVertexBuffer ), std::move( p_pIndexBuffer ) );
}

DepthStencilStateSPtr DxRenderSystem::CreateDepthStencilState()
{
	return std::make_shared< DxDepthStencilState >( this );
}

RasteriserStateSPtr DxRenderSystem::CreateRasteriserState()
{
	return std::make_shared< DxRasteriserState >( this );
}

BlendStateSPtr DxRenderSystem::CreateBlendState()
{
	return std::make_shared< DxBlendState >( this );
}

FrameVariableBufferSPtr DxRenderSystem::CreateFrameVariableBuffer( Castor::String const & p_strName )
{
	return std::make_shared< DxFrameVariableBuffer >( p_strName, this );
}

BillboardListSPtr DxRenderSystem::CreateBillboardsList( Castor3D::SceneRPtr p_pScene )
{
	return std::make_shared< DxBillboardList >( p_pScene, this );
}

int DxRenderSystem::LockLight()
{
	int l_iReturn = -1;

	if ( m_setAvailableIndexes.size() > 0 )
	{
		l_iReturn = *m_setAvailableIndexes.begin();
		m_setAvailableIndexes.erase( m_setAvailableIndexes.begin() );
	}

	return l_iReturn;
}

void DxRenderSystem::DoInitialise()
{
	Logger::LogMessage( cuT( "DxRenderSystem::Initialise" ) );

	if ( m_bInitialised )
	{
		return;
	}

	Logger::LogMessage( cuT( "************************************************************************************************************************" ) );
	Logger::LogMessage( cuT( "Initialising Direct3D" ) );

	if ( ( m_pDirect3D = Direct3DCreate9( D3D_SDK_VERSION ) ) == NULL )
	{
		CASTOR_EXCEPTION( "Can't create D3D9 object" );
	}

	m_useMultiTexturing = true;
	m_bInitialised = true;
	CheckShaderSupport();
	m_pPipeline->Initialise();
	Logger::LogMessage( cuT( "Direct3D Initialisation Ended" ) );
	Logger::LogMessage( cuT( "************************************************************************************************************************" ) );
}

void DxRenderSystem::DoCleanup()
{
	SafeRelease( m_pDevice );
	SafeRelease( m_pDirect3D );
	m_submeshRenderers.clear();
	m_textureRenderers.clear();
	m_passRenderers.clear();
	m_lightRenderers.clear();
	m_windowRenderers.clear();
	m_cameraRenderers.clear();
}

ShaderProgramBaseSPtr DxRenderSystem::DoCreateHlslShaderProgram()
{
	return std::make_shared< DxShaderProgram >( this );
}

ShaderProgramBaseSPtr DxRenderSystem::DoCreateShaderProgram()
{
	return std::make_shared< DxShaderProgram >( this );
}

void DxRenderSystem::UnlockLight( int p_iIndex )
{
	if ( p_iIndex >= 0 && m_setAvailableIndexes.find( p_iIndex ) == m_setAvailableIndexes.end() )
	{
		m_setAvailableIndexes.insert( std::set <int>::value_type( p_iIndex ) );
	}
}

void DxRenderSystem::BeginOverlaysRendering()
{
	m_pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
//	m_pDevice->BeginScene();
	RenderSystem::BeginOverlaysRendering();
}

void DxRenderSystem::EndOverlaysRendering()
{
	RenderSystem::EndOverlaysRendering();
//	m_pDevice->EndScene();
	m_pDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
}

void DxRenderSystem::DoRenderAmbientLight( Colour const & p_clColour )
{
	uint8_t l_r, l_g, l_b, l_a;
	m_pDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_RGBA( p_clColour.red().convert_to( l_r ), p_clColour.green().convert_to( l_g ), p_clColour.blue().convert_to( l_b ), p_clColour.alpha().convert_to( l_a ) ) );
}

SubmeshRendererSPtr DxRenderSystem::DoCreateSubmeshRenderer()
{
	return std::make_shared< DxSubmeshRenderer >( this );
}

TextureRendererSPtr DxRenderSystem::DoCreateTextureRenderer()
{
	return std::make_shared< DxTextureRenderer >( this );
}

PassRendererSPtr DxRenderSystem::DoCreatePassRenderer()
{
	return std::make_shared< DxPassRenderer >( this );
}

CameraRendererSPtr DxRenderSystem::DoCreateCameraRenderer()
{
	return std::make_shared< DxCameraRenderer >( this );
}

LightRendererSPtr DxRenderSystem::DoCreateLightRenderer()
{
	return std::make_shared< DxLightRenderer >( this );
}

WindowRendererSPtr DxRenderSystem::DoCreateWindowRenderer()
{
	return std::make_shared< DxWindowRenderer >( this );
}

OverlayRendererSPtr DxRenderSystem::DoCreateOverlayRenderer()
{
	return std::make_shared< DxOverlayRenderer >( this );
}

TargetRendererSPtr DxRenderSystem::DoCreateTargetRenderer()
{
	return std::make_shared< DxTargetRenderer >( this );
}

SamplerRendererSPtr DxRenderSystem::DoCreateSamplerRenderer()
{
	return std::make_shared< DxSamplerRenderer >( this );
}

std::shared_ptr< Castor3D::GpuBuffer< uint32_t > > DxRenderSystem::DoCreateIndexBuffer( CpuBuffer< uint32_t > * p_pBuffer )
{
	return std::make_shared< DxIndexBuffer >( p_pBuffer );
}

std::shared_ptr< Castor3D::GpuBuffer< uint8_t > > DxRenderSystem::DoCreateVertexBuffer( BufferDeclaration const & p_declaration, CpuBuffer< uint8_t > * p_pBuffer )
{
	return std::make_shared< DxVertexBuffer >( p_declaration, p_pBuffer );
}

std::shared_ptr< Castor3D::GpuBuffer< real > > DxRenderSystem::DoCreateMatrixBuffer( CpuBuffer< real > * p_pBuffer )
{
	return nullptr;
}

std::shared_ptr< Castor3D::GpuBuffer< uint8_t > > DxRenderSystem::DoCreateTextureBuffer( CpuBuffer< uint8_t > * p_pBuffer )
{
	return nullptr;
}

StaticTextureSPtr DxRenderSystem::DoCreateStaticTexture()
{
	return std::make_shared< DxStaticTexture >( this );
}

DynamicTextureSPtr DxRenderSystem::DoCreateDynamicTexture()
{
	return std::make_shared< DxDynamicTexture >( this );
}
