#include "Dx11RenderSystem.hpp"
#include "Dx11SubmeshRenderer.hpp"
#include "Dx11CameraRenderer.hpp"
#include "Dx11LightRenderer.hpp"
#include "Dx11OverlayRenderer.hpp"
#include "Dx11MaterialRenderer.hpp"
#include "Dx11SamplerRenderer.hpp"
#include "Dx11TextureRenderer.hpp"
#include "Dx11TargetRenderer.hpp"
#include "Dx11WindowRenderer.hpp"
#include "Dx11ShaderProgram.hpp"
#include "Dx11ShaderObject.hpp"
#include "Dx11Context.hpp"
#include "Dx11BillboardList.hpp"
#include "Dx11GeometryBuffers.hpp"
#include "Dx11DepthStencilState.hpp"
#include "Dx11BlendState.hpp"
#include "Dx11RasteriserState.hpp"
#include "Dx11FrameVariableBuffer.hpp"
#include "Dx11DynamicTexture.hpp"
#include "Dx11StaticTexture.hpp"
#include "Dx11IndexBuffer.hpp"
#include "Dx11MatrixBuffer.hpp"
#include "Dx11VertexBuffer.hpp"

#include <Logger.hpp>
#include <Material.hpp>
#include <MaterialManager.hpp>

using namespace Castor3D;
using namespace Castor;
using namespace Dx11Render;

DxRenderSystem::DxRenderSystem( Engine * p_pEngine )
	: RenderSystem( p_pEngine, eRENDERER_TYPE_DIRECT3D11 )
	, m_pDevice( NULL )
	, m_pFactory( NULL )
{
	DirectX11::Initialise();
	m_pPipeline = new DxPipeline( this );
	m_bInstancing = true;

	Logger::LogInfo( cuT( "Dx11RenderSystem::Dx11RenderSystem" ) );
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
	DirectX11::Cleanup();
}

void DxRenderSystem::Delete()
{
	Cleanup();
#if !defined( NDEBUG )
	m_pDebug->ReportLiveDeviceObjects( D3D11_RLDO_SUMMARY );
	m_pDebug->ReportLiveDeviceObjects( D3D11_RLDO_DETAIL );
	SafeRelease( m_pDebug );
#endif
	SafeRelease( m_pDevice );
	SafeRelease( m_pFactory );
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
	case eSHADER_MODEL_5:
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
	m_useShader[eSHADER_TYPE_VERTEX] = true;
	m_useShader[eSHADER_TYPE_PIXEL] = true;
	m_useShader[eSHADER_TYPE_GEOMETRY] = true;
	m_useShader[eSHADER_TYPE_HULL] = true;
	m_useShader[eSHADER_TYPE_DOMAIN] = true;
	m_useShader[eSHADER_TYPE_COMPUTE] = false;
}

bool DxRenderSystem::InitialiseDevice( HWND p_hWnd, DXGI_SWAP_CHAIN_DESC & p_swapChainDesc )
{
	bool l_bReturn = true;
	static std::map< D3D_FEATURE_LEVEL, String > StrFeatureLevel;

	if ( !m_pFactory )
	{
		HRESULT l_hr = CreateDXGIFactory( __uuidof( IDXGIFactory ) , reinterpret_cast< void ** >( &m_pFactory ) );

		if ( l_hr != S_OK )
		{
			CASTOR_EXCEPTION( "Can't create Factory object" );
		}
	}

	if ( StrFeatureLevel.empty() )
	{
//		StrFeatureLevel[D3D_FEATURE_LEVEL_11_1] = cuT( "Direct3D 11.1" );
		StrFeatureLevel[D3D_FEATURE_LEVEL_11_0] = cuT( "Direct3D 11.0" );
		StrFeatureLevel[D3D_FEATURE_LEVEL_10_1] = cuT( "Direct3D 10.1" );
		StrFeatureLevel[D3D_FEATURE_LEVEL_10_0] = cuT( "Direct3D 10.0" );
		StrFeatureLevel[D3D_FEATURE_LEVEL_9_3] = cuT( "Direct3D 9.3" );
		StrFeatureLevel[D3D_FEATURE_LEVEL_9_2] = cuT( "Direct3D 9.2" );
		StrFeatureLevel[D3D_FEATURE_LEVEL_9_1] = cuT( "Direct3D 9.1" );
	}

	if ( !m_pDevice )
	{
		std::vector< D3D_FEATURE_LEVEL > arrayD3dFeatureLevelsRequested;
//		arrayD3dFeatureLevelsRequested.push_back( D3D_FEATURE_LEVEL_11_1 );
		arrayD3dFeatureLevelsRequested.push_back( D3D_FEATURE_LEVEL_11_0 );
		arrayD3dFeatureLevelsRequested.push_back( D3D_FEATURE_LEVEL_10_1 );
		arrayD3dFeatureLevelsRequested.push_back( D3D_FEATURE_LEVEL_10_0 );
		arrayD3dFeatureLevelsRequested.push_back( D3D_FEATURE_LEVEL_9_3 );
		arrayD3dFeatureLevelsRequested.push_back( D3D_FEATURE_LEVEL_9_2 );
		arrayD3dFeatureLevelsRequested.push_back( D3D_FEATURE_LEVEL_9_1 );
		D3D_FEATURE_LEVEL d3dFeatureLevelsSupported;
		HRESULT l_hr;
		HWND l_hWnd;
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
		l_uiFlags = D3D11_CREATE_DEVICE_DEBUG;
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
						l_hr = D3D11CreateDevice( l_pAdapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, l_uiFlags, arrayD3dFeatureLevelsRequested.data(), UINT( arrayD3dFeatureLevelsRequested.size() ), D3D11_SDK_VERSION, NULL, &d3dFeatureLevelsSupported, NULL );

						if ( SUCCEEDED( l_hr ) )
						{
							l_hr = D3D11CreateDevice( l_pAdapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, l_uiFlags, &d3dFeatureLevelsSupported, 1, D3D11_SDK_VERSION, &m_pDevice, &m_featureLevel, NULL );

							if ( SUCCEEDED( l_hr ) )
							{
								// Get the adapter (video card) description.
								l_hr = l_pAdapter->GetDesc( &l_adapterDesc );
							}

							if ( SUCCEEDED( l_hr ) )
							{
								// Store the dedicated video card memory in megabytes.
								int l_videoCardMemory = int( l_adapterDesc.DedicatedVideoMemory / 1024 / 1024 );
								// Convert the name of the video card to a character array and store it.
								String l_strVideoCardDescription = str_utils::from_wstr( l_adapterDesc.Description );
								Logger::LogInfo( cuT( "Video card name : " ) + l_strVideoCardDescription );
								Logger::LogInfo( cuT( "Video card memory : %d" ), l_videoCardMemory );
							}
						}
						else
						{
							// First me check max supported feature level
							l_hr = D3D11CreateDevice( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, l_uiFlags, arrayD3dFeatureLevelsRequested.data(), UINT( arrayD3dFeatureLevelsRequested.size() ), D3D11_SDK_VERSION, NULL, &d3dFeatureLevelsSupported, NULL );

							if ( SUCCEEDED( l_hr ) )
							{
								l_hr = D3D11CreateDevice( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, l_uiFlags, &d3dFeatureLevelsSupported, 1, D3D11_SDK_VERSION, &m_pDevice, &m_featureLevel, NULL );

								if ( SUCCEEDED( l_hr ) )
								{
									// Get the adapter (video card) description.
									l_hr = l_pAdapter->GetDesc( &l_adapterDesc );
								}

								if ( SUCCEEDED( l_hr ) )
								{
									// Store the dedicated video card memory in megabytes.
									int l_videoCardMemory = int( l_adapterDesc.DedicatedVideoMemory / 1024 / 1024 );
									// Convert the name of the video card to a character array and store it.
									String l_strVideoCardDescription = str_utils::from_wstr( l_adapterDesc.Description );
									Logger::LogInfo( cuT( "Video card name : " ) + l_strVideoCardDescription );
									Logger::LogInfo( cuT( "Video card memory : %d" ), l_videoCardMemory );
								}
							}
						}
					}
				}

				// Release the adapter output.
				SafeRelease( l_pAdapterOutput );
			}

			// Release the adapter.
			SafeRelease( l_pAdapter );
			l_uiCount++;
		}

		dxDebugName( m_pDevice, MainDevice );
#if !defined( NDEBUG )

		if ( m_pDevice )
		{
			m_pDevice->QueryInterface( IID_ID3D11Debug, reinterpret_cast< void ** >( &m_pDebug ) );
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
			Logger::LogInfo( cuT( "Dx11Context::InitialiseDevice - Loaded " ) + StrFeatureLevel[m_featureLevel] + cuT( " compliant device" ) );
		}
		else
		{
			Logger::LogWarning( "Dx11Context ::InitialiseDevice - Error creating device : 0x%X", uint32_t( l_hr ) );
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

BillboardListSPtr DxRenderSystem::CreateBillboardsList( Castor3D::SceneSPtr p_pScene )
{
	return std::make_shared< DxBillboardList >( p_pScene, this );
}

void DxRenderSystem::DoInitialise()
{
	Logger::LogInfo( cuT( "Dx11RenderSystem::Initialise" ) );

	if ( m_bInitialised )
	{
		return;
	}

	Logger::LogInfo( cuT( "************************************************************************************************************************" ) );
	Logger::LogInfo( cuT( "Initialising Direct3D" ) );
	m_useMultiTexturing = true;
	m_bInitialised = true;
	CheckShaderSupport();
	m_pPipeline->Initialise();
	Logger::LogInfo( cuT( "Direct3D Initialisation Ended" ) );
	Logger::LogInfo( cuT( "************************************************************************************************************************" ) );
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

void DxRenderSystem::BeginOverlaysRendering( Size const & p_size )
{
	RenderSystem::BeginOverlaysRendering( p_size );
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
