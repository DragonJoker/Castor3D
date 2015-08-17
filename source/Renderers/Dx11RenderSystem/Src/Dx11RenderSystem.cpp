#include "Dx11RenderSystem.hpp"

#include "Dx11OverlayRenderer.hpp"
#include "Dx11RenderTarget.hpp"
#include "Dx11RenderWindow.hpp"
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
#include "Dx11Sampler.hpp"

#include <Logger.hpp>
#include <Material.hpp>
#include <MaterialManager.hpp>

using namespace Castor3D;
using namespace Castor;
using namespace Dx11Render;

DxRenderSystem::DxRenderSystem( Engine * p_pEngine )
	: RenderSystem( p_pEngine, eRENDERER_TYPE_DIRECT3D11 )
	, m_pDevice( NULL )
{
	Logger::LogInfo( cuT( "Dx11RenderSystem::Dx11RenderSystem" ) );
	DirectX11::Initialise();
	m_pPipeline = new DxPipeline( this );
	m_bInstancing = true;
	m_bNonPowerOfTwoTextures = true;
}

DxRenderSystem::~DxRenderSystem()
{
	delete m_pPipeline;
	Delete();
	DirectX11::Cleanup();
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

	IDXGIFactory * l_factory = NULL;
	HRESULT l_hr = CreateDXGIFactory( __uuidof( IDXGIFactory ) , reinterpret_cast< void ** >( &l_factory ) );

	if ( l_hr != S_OK )
	{
		CASTOR_EXCEPTION( "Can't create Factory object" );
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
		l_hr = l_factory->MakeWindowAssociation( p_hWnd, 0 );

		if ( SUCCEEDED( l_hr ) )
		{
			l_hr = l_factory->GetWindowAssociation( &l_hWnd );
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

		while ( l_factory->EnumAdapters( l_uiCount, &l_pAdapter ) != DXGI_ERROR_NOT_FOUND && !m_pDevice )
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
					}
				}

				// Release the adapter output.
				SafeRelease( l_pAdapterOutput );
			}

			// Release the adapter.
			SafeRelease( l_pAdapter );
			l_uiCount++;
		}

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

		l_factory->Release();
	}

	return l_bReturn;
}

void DxRenderSystem::Delete()
{
	Cleanup();
#if !defined( NDEBUG )

	for ( auto && l_decl: m_allocated )
	{
		std::stringstream l_stream;
		l_stream << "Leaked 0x" << std::hex << l_decl.m_object << std::dec << " (" << l_decl.m_name << "), from file " << l_decl.m_file << ", line " << l_decl.m_line << std::endl;
		Castor::Logger::LogError( l_stream.str() );
	}

	//m_pDebug->ReportLiveDeviceObjects( D3D11_RLDO_SUMMARY );
	//m_pDebug->ReportLiveDeviceObjects( D3D11_RLDO_DETAIL );
	SafeRelease( m_pDebug );
#endif
	SafeRelease( m_pDevice );
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

SamplerSPtr DxRenderSystem::CreateSampler( Castor::String const & p_name )
{
	return std::make_shared< DxSampler >( this, p_name );
}

RenderTargetSPtr DxRenderSystem::CreateRenderTarget( Castor3D::eTARGET_TYPE p_eType )
{
	return std::make_shared< DxRenderTarget >( this, p_eType );
}

RenderWindowSPtr DxRenderSystem::CreateRenderWindow()
{
	return std::make_shared< DxRenderWindow >( this );
}

ShaderProgramBaseSPtr DxRenderSystem::CreateHlslShaderProgram()
{
	return std::make_shared< DxShaderProgram >( this );
}

ShaderProgramBaseSPtr DxRenderSystem::CreateShaderProgram()
{
	return std::make_shared< DxShaderProgram >( this );
}

OverlayRendererSPtr DxRenderSystem::CreateOverlayRenderer()
{
	return std::make_shared< DxOverlayRenderer >( this );
}

std::shared_ptr< Castor3D::GpuBuffer< uint32_t > > DxRenderSystem::CreateIndexBuffer( CpuBuffer< uint32_t > * p_pBuffer )
{
	return std::make_shared< DxIndexBuffer >( p_pBuffer );
}

std::shared_ptr< Castor3D::GpuBuffer< uint8_t > > DxRenderSystem::CreateVertexBuffer( BufferDeclaration const & p_declaration, CpuBuffer< uint8_t > * p_pBuffer )
{
	return std::make_shared< DxVertexBuffer >( p_declaration, p_pBuffer );
}

std::shared_ptr< Castor3D::GpuBuffer< real > > DxRenderSystem::CreateMatrixBuffer( CpuBuffer< real > * p_pBuffer )
{
	return std::make_shared< DxMatrixBuffer >( p_pBuffer );
}

std::shared_ptr< Castor3D::GpuBuffer< uint8_t > > DxRenderSystem::CreateTextureBuffer( CpuBuffer< uint8_t > * p_pBuffer )
{
	return nullptr;
}

StaticTextureSPtr DxRenderSystem::CreateStaticTexture()
{
	return std::make_shared< DxStaticTexture >( this );
}

DynamicTextureSPtr DxRenderSystem::CreateDynamicTexture()
{
	return std::make_shared< DxDynamicTexture >( this );
}

#if !defined( NDEBUG )

namespace
{
	template< typename T >
	void DoSetDxDebugName( uint32_t p_id, T * p_object, std::string const & p_type )
	{
		if( p_object )
		{
			char l_szName[100] = { 0 };
			uint64_t l_ui64Address = (uint64_t)p_object;
			std::stringstream l_type;
			l_type.width( 20 );
			l_type<< std::left << p_type;
			sprintf_s( l_szName, "(%d) %s [0x%016X]", p_id, l_type.str().c_str(), l_ui64Address );
			p_object->SetPrivateData( WKPDID_D3DDebugObjectName, UINT( strlen( l_szName ) - 1 ), l_szName );
			Castor::Logger::LogDebug( l_szName );
		}
	}

	template< typename T >
	void DoUnsetDxDebugName( T * p_object, std::string const & p_name )
	{
		std::stringstream l_type;
		l_type.width( 20 );
		ULONG l_count = p_object->AddRef() - 1;
		p_object->Release();
		l_type<< std::left << p_name;
		Castor::Logger::LogWarning( "Released %s [0x%016X] => %d", l_type.str().c_str(), uint64_t( p_object ), l_count );
	}
}

void DxRenderSystem::SetDxDebugName( ID3D11Device * p_object, std::string const & p_type, std::string const & p_file, int p_line )
{
	auto && l_it = std::find_if( m_allocated.begin(), m_allocated.end(), [p_object]( ObjectDeclaration const & l_object )
	{
		return p_object == l_object.m_object;
	} );

	if ( l_it == m_allocated.end() )
	{
		m_allocated.push_back( { ++m_id, p_type, p_object, p_file, p_line, 1 } );
		DoSetDxDebugName( m_id, p_object, p_type );
	}
	else
	{
		++l_it->m_ref;
	}
}

void DxRenderSystem::SetDxDebugName( ID3D11DeviceChild * p_object, std::string const & p_type, std::string const & p_file, int p_line )
{
	auto && l_it = std::find_if( m_allocated.begin(), m_allocated.end(), [p_object]( ObjectDeclaration const & l_object )
	{
		return p_object == l_object.m_object;
	} );

	if ( l_it == m_allocated.end() )
	{
		m_allocated.push_back( { ++m_id, p_type, p_object, p_file, p_line, 1 } );
		DoSetDxDebugName( m_id, p_object, p_type );
	}
	else
	{
		++l_it->m_ref;
	}
}

void DxRenderSystem::SetDxDebugName( IDXGIDeviceSubObject * p_object, std::string const & p_type, std::string const & p_file, int p_line )
{
	auto && l_it = std::find_if( m_allocated.begin(), m_allocated.end(), [p_object]( ObjectDeclaration const & l_object )
	{
		return p_object == l_object.m_object;
	} );

	if ( l_it == m_allocated.end() )
	{
		m_allocated.push_back( { ++m_id, p_type, p_object, p_file, p_line, 1 } );
		DoSetDxDebugName( m_id, p_object, p_type );
	}
	else
	{
		++l_it->m_ref;
	}
}

void DxRenderSystem::UnsetDxDebugName( ID3D11Device * p_object )
{
	auto l_it = std::find_if( m_allocated.begin(), m_allocated.end(), [&p_object]( ObjectDeclaration p_decl ){ return p_object == p_decl.m_object; } );
	char l_szName[1024] = { 0 };

	if ( l_it != m_allocated.end() )
	{
		if ( !--l_it->m_ref )
		{
			DoUnsetDxDebugName( p_object, l_it->m_name );
			m_allocated.erase( l_it );
		}
	}
	else
	{
		Castor::Logger::LogWarning( "Untracked [0x%016X]", uint64_t( p_object ) );
	}
}

void DxRenderSystem::UnsetDxDebugName( ID3D11DeviceChild * p_object )
{
	auto l_it = std::find_if( m_allocated.begin(), m_allocated.end(), [&p_object]( ObjectDeclaration p_decl ){ return p_object == p_decl.m_object; } );
	char l_szName[1024] = { 0 };

	if ( l_it != m_allocated.end() )
	{
		if ( !--l_it->m_ref )
		{
			DoUnsetDxDebugName( p_object, l_it->m_name );
			m_allocated.erase( l_it );
		}
	}
	else
	{
		Castor::Logger::LogWarning( "Untracked [0x%016X]", uint64_t( p_object ) );
	}
}

void DxRenderSystem::UnsetDxDebugName( IDXGIDeviceSubObject * p_object )
{
	auto l_it = std::find_if( m_allocated.begin(), m_allocated.end(), [&p_object]( ObjectDeclaration p_decl ){ return p_object == p_decl.m_object; } );
	char l_szName[1024] = { 0 };

	if ( l_it != m_allocated.end() )
	{
		if ( !--l_it->m_ref )
		{
			DoUnsetDxDebugName( p_object, l_it->m_name );
			m_allocated.erase( l_it );
		}
	}
	else
	{
		Castor::Logger::LogWarning( "Untracked [0x%016X]", uint64_t( p_object ) );
	}
}

#endif

void DxRenderSystem::DoInitialise()
{
	if ( !m_bInitialised )
	{
		Logger::LogInfo( cuT( "************************************************************************************************************************" ) );
		Logger::LogInfo( cuT( "Initialising Direct3D" ) );
		m_useMultiTexturing = true;
		m_bInitialised = true;
		CheckShaderSupport();
		m_pPipeline->Initialise();
		Logger::LogInfo( cuT( "Direct3D Initialisation Ended" ) );
		Logger::LogInfo( cuT( "************************************************************************************************************************" ) );
	}
}

void DxRenderSystem::DoCleanup()
{
}
