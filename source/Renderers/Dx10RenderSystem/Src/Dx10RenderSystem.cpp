#include "Dx10RenderSystem.hpp"
#include "Dx10SubmeshRenderer.hpp"
#include "Dx10CameraRenderer.hpp"
#include "Dx10LightRenderer.hpp"
#include "Dx10OverlayRenderer.hpp"
#include "Dx10MaterialRenderer.hpp"
#include "Dx10TextureRenderer.hpp"
#include "Dx10TargetRenderer.hpp"
#include "Dx10WindowRenderer.hpp"
#include "Dx10ShaderProgram.hpp"
#include "Dx10ShaderObject.hpp"
#include "Dx10Context.hpp"
#include "Dx10BillboardList.hpp"
#include "Dx10GeometryBuffers.hpp"
#include "Dx10DepthStencilState.hpp"
#include "Dx10RasteriserState.hpp"
#include "Dx10BlendState.hpp"
#include "Dx10FrameVariableBuffer.hpp"
#include "Dx10SamplerRenderer.hpp"
#include "Dx10IndexBuffer.hpp"
#include "Dx10VertexBuffer.hpp"
#include "Dx10MatrixBuffer.hpp"
#include "Dx10StaticTexture.hpp"
#include "Dx10DynamicTexture.hpp"

#include <Logger.hpp>
#include <Material.hpp>

using namespace Castor3D;
using namespace Castor;
using namespace Dx10Render;

DxRenderSystem::DxRenderSystem( Engine * p_pEngine, Logger * p_pLogger )
	:	RenderSystem( p_pEngine, eRENDERER_TYPE_DIRECT3D10	)
	,	m_pDevice( NULL	)
	,	m_pFactory( NULL	)
{
	DirectX10::Initialise();
	m_pPipeline = new DxPipeline( this );
	m_bInstancing = true;

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
		Logger::SetFileName( File::DirectoryGetCurrent() / cuT( "Dx10RenderSystem.log" ) );
	}

	Logger::LogMessage( cuT( "Dx10RenderSystem::Dx10RenderSystem" ) );
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
	DirectX10::Cleanup();
}

void DxRenderSystem::Delete()
{
	Cleanup();
#if !defined( NDEBUG )
	SafeRelease( m_pDebug );
#endif
	SafeRelease( m_pDevice );
	SafeRelease( m_pFactory );
	Logger::Cleanup();
}

bool DxRenderSystem::CheckSupport( eSHADER_MODEL p_eProfile )
{
	bool l_bReturn = false;

	switch ( p_eProfile )
	{
	case eSHADER_MODEL_1:
	case eSHADER_MODEL_2:
	case eSHADER_MODEL_3:
	case eSHADER_MODEL_4:
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
	m_useShader[eSHADER_TYPE_GEOMETRY	] = true;
	m_useShader[eSHADER_TYPE_HULL		] = true;
	m_useShader[eSHADER_TYPE_DOMAIN		] = true;
	m_useShader[eSHADER_TYPE_COMPUTE	] = false;
}

bool DxRenderSystem::InitialiseDevice( HWND p_hWnd, DXGI_SWAP_CHAIN_DESC & p_swapChainDesc )
{
	bool l_bReturn = true;

	if ( !m_pFactory )
	{
		HRESULT l_hr = CreateDXGIFactory( __uuidof( IDXGIFactory ) , reinterpret_cast< void ** >( &m_pFactory ) );
		dxDebugName( m_pFactory, DXGIFactory );

		if ( l_hr != S_OK )
		{
			CASTOR_EXCEPTION( "Can't create Factory object" );
		}
	}

	if ( !m_pDevice )
	{
		HRESULT	l_hr;
		HWND	l_hWnd;
		l_hr = m_pFactory->MakeWindowAssociation( p_hWnd, 0 );

		if ( SUCCEEDED( l_hr ) )
		{
			l_hr = m_pFactory->GetWindowAssociation( &l_hWnd );
		}

		// Enumerate adapters.
		uint32_t l_uiCount = 0;
		IDXGIAdapter * l_pAdapter;
		IDXGIOutput * l_pAdapterOutput;
		DXGI_ADAPTER_DESC l_adapterDesc;
		UINT l_uiFlags = 0;
#if !defined( NDEBUG )
		l_uiFlags = D3D10_CREATE_DEVICE_DEBUG;
#endif

		while ( m_pFactory->EnumAdapters( l_uiCount, &l_pAdapter ) != DXGI_ERROR_NOT_FOUND && !m_pDevice )
		{
			UINT l_numModes = 0;
			// Enumerate the primary adapter output (monitor).
			l_hr = l_pAdapter->EnumOutputs( 0, &l_pAdapterOutput );

			if ( SUCCEEDED( l_hr ) )
			{
				// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
				l_hr = l_pAdapterOutput->GetDisplayModeList( p_swapChainDesc.BufferDesc.Format, DXGI_ENUM_MODES_INTERLACED, &l_numModes, NULL );

				if ( SUCCEEDED( l_hr ) )
				{
					// Create a list to hold all the possible display modes for this monitor/video card combination.
					std::vector< DXGI_MODE_DESC > l_displayModeList( l_numModes );
					// Now fill the display mode list structures.
					l_hr = l_pAdapterOutput->GetDisplayModeList( p_swapChainDesc.BufferDesc.Format, DXGI_ENUM_MODES_INTERLACED, &l_numModes, &l_displayModeList[0] );

					if ( SUCCEEDED( l_hr ) )
					{
						// Now go through all the display modes and find the one that matches the screen width and height.
						// When a match is found store the numerator and denominator of the refresh rate for that monitor.
						std::vector< DXGI_MODE_DESC > l_matchingDisplayModes;
						bool l_bFound = false;
						int l_iTolerance = 300;

						for ( UINT i = 0; i < l_numModes && !l_bFound; i++ )
						{
							int l_iDiffWidth = int( l_displayModeList[i].Width ) - int( p_swapChainDesc.BufferDesc.Width );
							int l_iDiffHeight = int( l_displayModeList[i].Height ) - int( p_swapChainDesc.BufferDesc.Height );

							if ( l_iDiffWidth >= 0 && l_iDiffWidth <= l_iTolerance )
							{
								if ( l_iDiffHeight >= 0 && l_iDiffHeight <= l_iTolerance )
								{
									l_matchingDisplayModes.push_back( l_displayModeList[i] );
								}
							}
						}

						int l_iWantedFPS = int( 1.0 / m_pEngine->GetFrameTime() );

						for ( std::vector< DXGI_MODE_DESC >::iterator l_it = l_matchingDisplayModes.begin(); l_it != l_matchingDisplayModes.end() && !l_bFound; ++l_it )
						{
							// We loop through adequate display modes to find one with adequate frame rate
							int l_iDiffFPS = int( l_it->RefreshRate.Numerator / l_it->RefreshRate.Denominator ) - l_iWantedFPS;

							if ( l_iDiffFPS > 0 )
							{
								p_swapChainDesc.BufferDesc.RefreshRate.Numerator = l_it->RefreshRate.Numerator;
								p_swapChainDesc.BufferDesc.RefreshRate.Denominator = l_it->RefreshRate.Denominator;
								p_swapChainDesc.BufferDesc.Width = l_it->Width;
								p_swapChainDesc.BufferDesc.Height = l_it->Height;
								l_bFound = true;
							}
						}

						// First me check max supported feature level
						l_hr = D3D10CreateDevice( l_pAdapter, D3D10_DRIVER_TYPE_HARDWARE, NULL, l_uiFlags, D3D10_SDK_VERSION, &m_pDevice );

						if ( SUCCEEDED( l_hr ) )
						{
							// Get the adapter (video card) description.
							l_hr = l_pAdapter->GetDesc( &l_adapterDesc );

							if ( SUCCEEDED( l_hr ) )
							{
								// Store the dedicated video card memory in megabytes.
								int l_videoCardMemory = int( l_adapterDesc.DedicatedVideoMemory / 1024 / 1024 );
								// Convert the name of the video card to a character array and store it.
								String l_strVideoCardDescription = str_utils::from_wstr( l_adapterDesc.Description );
								Logger::LogMessage( cuT( "Video card name : " ) + l_strVideoCardDescription );
								Logger::LogMessage( cuT( "Video card memory : %d" ), l_videoCardMemory );
							}
						}
						else
						{
							// First me check max supported feature level
							l_hr = D3D10CreateDevice( NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL, l_uiFlags, D3D10_SDK_VERSION, &m_pDevice );

							if ( SUCCEEDED( l_hr ) )
							{
								// Get the adapter (video card) description.
								l_hr = l_pAdapter->GetDesc( &l_adapterDesc );

								if ( SUCCEEDED( l_hr ) )
								{
									// Store the dedicated video card memory in megabytes.
									int l_videoCardMemory = int( l_adapterDesc.DedicatedVideoMemory / 1024 / 1024 );
									// Convert the name of the video card to a character array and store it.
									String l_strVideoCardDescription = str_utils::from_wstr( l_adapterDesc.Description );
									Logger::LogMessage( cuT( "Video card name : " ) + l_strVideoCardDescription );
									Logger::LogMessage( cuT( "Video card memory : %d" ), l_videoCardMemory );
								}
							}
						}
					}
				}
			}

			l_uiCount++;
		}

		dxDebugName( m_pDevice, MainDevice );
#if !defined( NDEBUG )

		if ( m_pDevice )
		{
			m_pDevice->QueryInterface( IID_ID3D10Debug, reinterpret_cast< void ** >( &m_pDebug ) );
		}

#endif

		if ( !IsInitialised() )
		{
			Initialise();
			m_pEngine->GetMaterialManager().Initialise();
		}

		if ( m_pDevice )
		{
			m_useShaders = true;
			Logger::LogMessage( cuT( "Dx10Context::InitialiseDevice - Loaded device" ) );
		}
		else
		{
			Logger::LogWarning( "Dx10Context ::InitialiseDevice - Error creating device : 0x%X", uint32_t( l_hr ) );
			l_bReturn = false;
		}
	}

	return l_bReturn;
}

ContextSPtr DxRenderSystem::CreateContext()
{
	return std::make_shared< DxContext >();
}

GeometryBuffersSPtr DxRenderSystem::CreateGeometryBuffers( VertexBufferUPtr p_pVertexBuffer, IndexBufferUPtr p_pIndexBuffer, MatrixBufferUPtr p_pMatrixBuffer )
{
	return std::make_shared< DxGeometryBuffers >( this, std::move( p_pVertexBuffer ), std::move( p_pIndexBuffer ), std::move( p_pMatrixBuffer ) );
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

void DxRenderSystem::DoInitialise()
{
	Logger::LogMessage( cuT( "Dx10RenderSystem::Initialise" ) );

	if ( m_bInitialised )
	{
		return;
	}

	Logger::LogMessage( cuT( "************************************************************************************************************************" ) );
	Logger::LogMessage( cuT( "Initialising Direct3D" ) );
	m_useMultiTexturing = true;
	m_bInitialised = true;
	CheckShaderSupport();
	m_pPipeline->Initialise();
	Logger::LogMessage( cuT( "Direct3D Initialisation Ended" ) );
	Logger::LogMessage( cuT( "************************************************************************************************************************" ) );
}

void DxRenderSystem::DoCleanup()
{
}

void DxRenderSystem::DoRenderAmbientLight( Colour const & p_clColour )
{
}

ShaderProgramBaseSPtr DxRenderSystem::DoCreateHlslShaderProgram()
{
	return std::make_shared< DxShaderProgram >( this );
}

int DxRenderSystem::LockLight()
{
	int l_iReturn = -1;

	if ( !m_setAvailableIndexes.empty() )
	{
		l_iReturn = *m_setAvailableIndexes.begin();
		m_setAvailableIndexes.erase( m_setAvailableIndexes.begin() );
	}

	return l_iReturn;
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
	RenderSystem::BeginOverlaysRendering();
}

void DxRenderSystem::EndOverlaysRendering()
{
	RenderSystem::EndOverlaysRendering();
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
	return std::make_shared< DxMatrixBuffer >( p_pBuffer );
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
