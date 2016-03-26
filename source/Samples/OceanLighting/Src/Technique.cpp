#if CHECK_MEMORYLEAKS && defined( VLD_AVAILABLE ) && USE_VLD
#	include <vld.h>
#endif

#include <algorithm>
#include <limits>
#include <fstream>
#include <iostream>
#include <cmath>
#include <cstdint>
#include <ctime>

#if !defined( M_PI )
#	define M_PI 3.14159265
#endif

#include "Technique.hpp"

#include <BlendStateManager.hpp>
#include <RenderSystem.hpp>
#include <RasteriserState.hpp>
#include <DepthStencilStateManager.hpp>
#include <DepthStencilRenderBuffer.hpp>
#include <TextureAttachment.hpp>
#include <RenderBufferAttachment.hpp>
#include <FrameVariableBuffer.hpp>
#include <OneFrameVariable.hpp>
#include <PointFrameVariable.hpp>
#include <MatrixFrameVariable.hpp>
#include <RasteriserStateManager.hpp>
#include <SamplerManager.hpp>
#include <StaticTexture.hpp>
#include <DynamicTexture.hpp>

#include <Assertion.hpp>
#include <Image.hpp>
#include <TransformationMatrix.hpp>

#if defined( min )
#	undef min
#	undef max
#endif

//*************************************************************************************************

#define IRRADIANCE_UNIT 0
#define INSCATTER_UNIT 1
#define TRANSMITTANCE_UNIT 2
#define SKY_UNIT 3
#define NOISE_UNIT 4
#if ENABLE_FFT
#	define SPECTRUM_1_2_UNIT 5
#	define SPECTRUM_3_4_UNIT 6
#	define SLOPE_VARIANCE_UNIT 7
#	define FFT_A_UNIT 8
#	define FFT_B_UNIT 9
#	define BUTTERFLY_UNIT 10
#else
#	define WAVE_UNIT 5
#endif

using namespace Castor;
using namespace Castor3D;

//*************************************************************************************************

#if defined( _WIN32 )
#	include <windows.h>
#else
#	include <sys/time.h>
#endif

double time()
{
#ifdef _WIN32
	__int64 time;
	__int64 cpuFrequency;
	::QueryPerformanceCounter( ( LARGE_INTEGER * )&time );
	::QueryPerformanceFrequency( ( LARGE_INTEGER * )&cpuFrequency );
	return time / double( cpuFrequency );
#else
	static double t0 = 0;
	timeval tv;
	gettimeofday( &tv, NULL );

	if ( !t0 )
	{
		t0 = tv.tv_sec;
	}

	return double( tv.tv_sec - t0 ) + double( tv.tv_usec ) / 1e6;
#endif
}

//*************************************************************************************************

namespace OceanLighting
{
	RenderTechnique::RenderTechnique( RenderTarget & p_renderTarget, RenderSystem * p_renderSystem, Parameters const & p_params )
		: Castor3D::RenderTechnique( cuT( "ocean" ), p_renderTarget, p_renderSystem, p_params )
		, m_render( NULL )
		, m_sky( NULL )
		, m_skymap( NULL )
		, m_clouds( NULL )
		, m_skyTexSize( 512 )
		, m_cloudLayer( false )
		, m_octaves( 10.0f )
		, m_lacunarity( 2.2f )
		, m_gain( 0.7f )
		, m_norm( 0.5f )
		, m_clamp1( -0.15f )
		, m_clamp2( 0.2f )
		, m_vboParams( m_vboBuffer )
		, m_sunTheta( float( M_PI / 2.0f - 0.05f ) )
		, m_sunPhi( 0.0f )
		, m_cameraHeight( 2.0f )
		, m_cameraTheta( 0.0f )
		, m_cameraPhi( 0.0f )
		, m_gridSize( 8.0f )
		, m_hdrExposure( 0.4f )
		, m_grid( false )
		, m_animate( true )
		, m_seaContrib( true )
		, m_sunContrib( true )
		, m_skyContrib( true )
		, m_manualFilter( false )
#if ENABLE_FFT
		, m_choppy( true )
		, m_spectrum12( NULL )
		, m_spectrum34( NULL )
		, m_init( NULL )
		, m_variances( NULL )
		, m_fftx( NULL )
		, m_ffty( NULL )
		, m_GRID1_SIZE( 5488.0f )
		, m_GRID2_SIZE( 392.0f )
		, m_GRID3_SIZE( 28.0f )
		, m_GRID4_SIZE( 2.0f )
		, m_WIND( 5.0f )
		, m_OMEGA( 0.84f )
		, m_A( 1.0f )
		, m_N_SLOPE_VARIANCE( 10 )
		, m_cm( 0.23f )
		, m_km( 370.0f )
		, m_PASSES( 8 )
		, m_FFT_SIZE( 1 << m_PASSES )
#else
		, m_nyquistMin( 1.0f )
		, m_nyquistMax( 1.5f )
		, m_nbWaves( 60 )
		, m_waves( NULL )
		, m_lambdaMin( 0.02f )
		, m_lambdaMax( 30.0f )
		, m_heightMax( 0.32f )
		, m_waveDirection( 2.4f )
		, m_U0( 10.0f )
		, m_waveDispersion( 1.25f )
		, m_sigmaXsq( 0.0f )
		, m_sigmaYsq( 0.0f )
		, m_meanHeight( 0.0f )
		, m_heightVariance( 0.0f )
		, m_amplitudeMax( 0.0f )
#endif
		, m_cloudColor( Colour::from_components( 1.0, 1.0, 1.0, 1.0 ) )
		, m_seaColor( Colour::from_components( 10, 40, 120, 25 ) )
		, m_bReloadPrograms( false )
		, m_bComputeSlopeVarianceTex( false )
		, m_bGenerateWavesSpectrum( false )
		, m_bGenerateMesh( false )
		, m_bGenerateWaves( false )
	{
		m_vboBuffer[0] = 0.0f;
		m_vboBuffer[1] = 0.0f;
		m_vboBuffer[2] = 0.0f;
		m_vboBuffer[3] = 0.0f;
		m_frameBuffer = m_renderSystem->CreateFrameBuffer();
		m_pColorBuffer = m_renderSystem->CreateDynamicTexture( 0, eACCESS_TYPE_READ | eACCESS_TYPE_WRITE );
		m_pDepthBuffer = m_frameBuffer->CreateDepthStencilRenderBuffer( ePIXEL_FORMAT_DEPTH24S8 );
		m_pColorAttach = m_frameBuffer->CreateAttachment( m_pColorBuffer );
		m_pDepthAttach = m_frameBuffer->CreateAttachment( m_pDepthBuffer );
		BufferElementDeclaration l_skymapDeclaration[] =
		{
			BufferElementDeclaration( ShaderProgram::Position, eELEMENT_USAGE_POSITION, eELEMENT_TYPE_2FLOATS )
		};
		BufferElementDeclaration l_cloudsVertexDeclarationElements[] =
		{
			BufferElementDeclaration( ShaderProgram::Position, eELEMENT_USAGE_POSITION, eELEMENT_TYPE_3FLOATS )
		};
		Engine * l_pEngine = GetEngine();
		m_pSamplerNearestClamp = l_pEngine->GetSamplerManager().Create( cuT( "NearestClamp" ) );
		m_pSamplerNearestRepeat = l_pEngine->GetSamplerManager().Create( cuT( "NearestRepeat" ) );
		m_pSamplerLinearClamp = l_pEngine->GetSamplerManager().Create( cuT( "LinearClamp" ) );
		m_pSamplerLinearRepeat = l_pEngine->GetSamplerManager().Create( cuT( "LinearRepeat" ) );
		m_pSamplerAnisotropicClamp = l_pEngine->GetSamplerManager().Create( cuT( "AnisotropicClamp" ) );
		m_pSamplerAnisotropicRepeat = l_pEngine->GetSamplerManager().Create( cuT( "AnisotropicRepeat" ) );
		m_pTexIrradiance = GetEngine()->GetRenderSystem()->CreateStaticTexture();
		m_pTexInscatter = GetEngine()->GetRenderSystem()->CreateStaticTexture();
		m_pTexTransmittance = GetEngine()->GetRenderSystem()->CreateStaticTexture();
		m_pTexSky = m_renderTarget->CreateDynamicTexture( 0, eACCESS_TYPE_READ | eACCESS_TYPE_WRITE );
		m_pTexNoise = m_renderTarget->CreateDynamicTexture( 0, eACCESS_TYPE_READ | eACCESS_TYPE_WRITE );
#if ENABLE_FFT
		BufferElementDeclaration l_quadVertexDeclarationElements[] =
		{
			BufferElementDeclaration( 0, eELEMENT_USAGE_POSITION, eELEMENT_TYPE_4FLOATS )
		};
		m_pTexSpectrum_1_2 = m_renderTarget->CreateDynamicTexture( 0, eACCESS_TYPE_READ | eACCESS_TYPE_WRITE );
		m_pTexSpectrum_3_4 = m_renderTarget->CreateDynamicTexture( 0, eACCESS_TYPE_READ | eACCESS_TYPE_WRITE );
		m_pTexSlopeVariance = m_renderTarget->CreateDynamicTexture( 0, eACCESS_TYPE_READ | eACCESS_TYPE_WRITE );
		m_pTexFFTA = m_renderTarget->CreateDynamicTexture( 0, eACCESS_TYPE_READ | eACCESS_TYPE_WRITE );
		m_pTexFFTB = m_renderTarget->CreateDynamicTexture( 0, eACCESS_TYPE_READ | eACCESS_TYPE_WRITE );
		m_pTexButterfly = m_renderTarget->CreateDynamicTexture( 0, eACCESS_TYPE_READ | eACCESS_TYPE_WRITE );
		m_variancesFbo = GetEngine()->GetRenderSystem()->CreateFrameBuffer();
		m_fftFbo1 = GetEngine()->GetRenderSystem()->CreateFrameBuffer();
		m_fftFbo2 = GetEngine()->GetRenderSystem()->CreateFrameBuffer();
		m_pAttachFftA = m_fftFbo2->CreateAttachment( m_pTexFFTA );
		m_pAttachFftB = m_fftFbo2->CreateAttachment( m_pTexFFTB );
		m_variancesVtxBuffer = std::make_unique< VertexBuffer >( *GetEngine(), l_quadVertexDeclarationElements );
		m_initVtxBuffer = std::make_unique< VertexBuffer >( *GetEngine(), l_quadVertexDeclarationElements );
		m_fftxVtxBuffer = std::make_unique< VertexBuffer >( *GetEngine(), l_quadVertexDeclarationElements );
		m_fftyVtxBuffer = std::make_unique< VertexBuffer >( *GetEngine(), l_quadVertexDeclarationElements );
		m_variancesIdxBuffer = std::make_unique< IndexBuffer >( *GetEngine() );
		m_initIdxBuffer = std::make_unique< IndexBuffer >( *GetEngine() );
		m_fftxIdxBuffer = std::make_unique< IndexBuffer >( *GetEngine() );
		m_fftyIdxBuffer = std::make_unique< IndexBuffer >( *GetEngine() );
		real l_quadVertices[] =
		{
			-1.0, -1.0, 0.0, 0.0
			, +1.0, -1.0, 1.0, 0.0
			, -1.0, +1.0, 0.0, 1.0
			, +1.0, +1.0, 1.0, 1.0
		};
		uint32_t l_quadIndices[] =
		{
			0, 1, 2,
			1, 3, 2
		};
		m_variancesVtxBuffer->Resize( sizeof( l_quadVertices ) / sizeof( real ) );
		m_initVtxBuffer->Resize( sizeof( l_quadVertices ) / sizeof( real ) );
		m_fftxVtxBuffer->Resize( sizeof( l_quadVertices ) / sizeof( real ) );
		m_fftyVtxBuffer->Resize( sizeof( l_quadVertices ) / sizeof( real ) );
		m_variancesIdxBuffer->Resize( sizeof( l_quadIndices ) / sizeof( uint32_t ) );
		m_initIdxBuffer->Resize( sizeof( l_quadIndices ) / sizeof( uint32_t ) );
		m_fftxIdxBuffer->Resize( sizeof( l_quadIndices ) / sizeof( uint32_t ) );
		m_fftyIdxBuffer->Resize( sizeof( l_quadIndices ) / sizeof( uint32_t ) );
		std::memcpy( m_variancesVtxBuffer->data(), &l_quadVertices[0], sizeof( l_quadVertices ) );
		std::memcpy( m_initVtxBuffer->data(), &l_quadVertices[0], sizeof( l_quadVertices ) );
		std::memcpy( m_fftxVtxBuffer->data(), &l_quadVertices[0], sizeof( l_quadVertices ) );
		std::memcpy( m_fftyVtxBuffer->data(), &l_quadVertices[0], sizeof( l_quadVertices ) );
		std::memcpy( m_variancesIdxBuffer->data(), &l_quadIndices[0], sizeof( l_quadIndices ) );
		std::memcpy( m_initIdxBuffer->data(), &l_quadIndices[0], sizeof( l_quadIndices ) );
		std::memcpy( m_fftxIdxBuffer->data(), &l_quadIndices[0], sizeof( l_quadIndices ) );
		std::memcpy( m_fftyIdxBuffer->data(), &l_quadIndices[0], sizeof( l_quadIndices ) );
#else
		m_pTexWave = m_renderTarget->CreateDynamicTexture( 0, eACCESS_TYPE_READ | eACCESS_TYPE_WRITE );
#endif
		m_fbo = m_renderSystem->CreateFrameBuffer();
		m_pAttachSky = m_fbo->CreateAttachment( m_pTexSky );
		m_skyVtxBuffer = std::make_unique< VertexBuffer >( *GetEngine(), l_skymapDeclaration );
		m_skymapVtxBuffer = std::make_unique< VertexBuffer >( *GetEngine(), l_skymapDeclaration );
		m_cloudsVtxBuffer = std::make_unique< VertexBuffer >( *GetEngine(), l_cloudsVertexDeclarationElements );
		m_skyIdxBuffer = std::make_unique< IndexBuffer >( *GetEngine() );
		m_skymapIdxBuffer = std::make_unique< IndexBuffer >( *GetEngine() );
		m_cloudsIdxBuffer = std::make_unique< IndexBuffer >( *GetEngine() );
		real l_skyVertices[] =
		{
			-1, -1
			, +1, -1
			, -1, +1
			, +1, +1
		};
		real l_cloudsVertices[] =
		{
			-1e6, -1e6, 3000.0
			, +1e6, -1e6, 3000.0
			, -1e6, +1e6, 3000.0
			, +1e6, +1e6, 3000.0
		};
		uint32_t l_skyIndices[] =
		{
			0, 1, 2,
			1, 3, 2
		};
		uint32_t l_cloudsIndices[] =
		{
			0, 1, 2,
			1, 3, 2
		};
		m_skyVtxBuffer->Resize( sizeof( l_skyVertices ) / sizeof( real ) );
		m_skymapVtxBuffer->Resize( sizeof( l_skyVertices ) / sizeof( real ) );
		m_cloudsVtxBuffer->Resize( sizeof( l_cloudsVertices ) / sizeof( real ) );
		m_skyVtxBuffer->Resize( sizeof( l_skyIndices ) / sizeof( uint32_t ) );
		m_skymapIdxBuffer->Resize( sizeof( l_skyIndices ) / sizeof( uint32_t ) );
		m_cloudsIdxBuffer->Resize( sizeof( l_cloudsIndices ) / sizeof( uint32_t ) );
		std::memcpy( m_skyVtxBuffer->data(), &l_skyVertices[0], sizeof( l_skyVertices ) );
		std::memcpy( m_skymapVtxBuffer->data(), &l_skyVertices[0], sizeof( l_skyVertices ) );
		std::memcpy( m_cloudsVtxBuffer->data(), &l_cloudsVertices[0], sizeof( l_cloudsVertices ) );
		std::memcpy( m_skyVtxBuffer->data(), &l_skyIndices[0], sizeof( l_skyIndices ) );
		std::memcpy( m_skymapIdxBuffer->data(), &l_skyIndices[0], sizeof( l_skyIndices ) );
		std::memcpy( m_cloudsIdxBuffer->data(), &l_cloudsIndices[0], sizeof( l_cloudsIndices ) );
		RasteriserStateSPtr l_pRasteriser = GetEngine()->GetRenderSystem()->GetEngine()->GetRasteriserStateManager().Create( cuT( "OceanLighting" ) );
		l_pRasteriser->SetCulledFaces( eFACE_NONE );
		l_pRasteriser->SetFillMode( eFILL_MODE_SOLID );
		m_pRasteriserState = l_pRasteriser;
		DepthStencilStateSPtr l_pDepthStencil = GetEngine()->GetRenderSystem()->GetEngine()->GetDepthStencilStateManager().Create( cuT( "OceanLighting" ) );
		l_pDepthStencil->SetDepthTest( false );
		m_pDepthStencilState = l_pDepthStencil;
		l_pRasteriser = GetEngine()->GetRenderSystem()->GetEngine()->GetRasteriserStateManager().Create( cuT( "OceanLighting_Render" ) );
		l_pRasteriser->SetCulledFaces( eFACE_NONE );
		l_pRasteriser->SetFillMode( eFILL_MODE_SOLID );
		m_renderRasteriserState = l_pRasteriser;
	}

	RenderTechnique::~RenderTechnique()
	{
		m_renderGBuffers.reset();
		m_cloudsGBuffers.reset();
		m_skyGBuffers.reset();
		m_skymapGBuffers.reset();
		m_pTexIrradiance.reset();
		m_pTexInscatter.reset();
		m_pTexTransmittance.reset();
		m_pTexSky.reset();
		m_pTexNoise.reset();
#if ENABLE_FFT
		m_variancesGBuffers.reset();
		m_initGBuffers.reset();
		m_fftxGBuffers.reset();
		m_fftyGBuffers.reset();
		m_pTexSpectrum_1_2.reset();
		m_pTexSpectrum_3_4.reset();
		m_pTexSlopeVariance.reset();
		m_pTexFFTA.reset();
		m_pTexFFTB.reset();
		m_pTexButterfly.reset();
#else
		m_pTexWave.reset();
#endif
		DoDeletePrograms( true );
#if ENABLE_FFT
		m_variancesFbo.reset();
		m_fftFbo1.reset();
		m_fftFbo2.reset();
		m_pAttachFftA.reset();
		m_pAttachFftB.reset();
#endif
		m_fbo.reset();
		m_pAttachSky.reset();
		m_pSamplerNearestClamp.reset();
		m_pSamplerNearestRepeat.reset();
		m_pSamplerLinearClamp.reset();
		m_pSamplerLinearRepeat.reset();
		m_pSamplerAnisotropicClamp.reset();
		m_pSamplerAnisotropicRepeat.reset();
	}

	RenderTechniqueSPtr RenderTechnique::CreateInstance( RenderTarget & p_renderTarget, RenderSystem * p_renderSystem, Parameters const & p_params )
	{
		// No make_shared because ctor is protected;
		return RenderTechniqueSPtr( new RenderTechnique( p_renderTarget, p_renderSystem, p_params ) );
	}

	// ----------------------------------------------------------------------------
	// PROGRAM RELOAD
	// ----------------------------------------------------------------------------

	void RenderTechnique::loadPrograms( bool all )
	{
		DoCleanupPrograms( all );
		DoDestroyPrograms( all );
		DoDeletePrograms( all );
		String l_strVertex/* = cuT( "#version 130\n#define _VERTEX_\nin vec4 vertex;\n" )*/;
		String l_strPixel/* = cuT( "#version 130\n#define _FRAGMENT_\n" )*/;
		FrameVariableBufferSPtr l_pConstants;
		String l_strAtmF;
		String l_strAtmV;
		String l_strOcnF;
		String l_strOcnV;
		String l_strSkyF;
		String l_strSkyV;
		String l_strMapF;
		String l_strMapV;
		String l_strSrcF;
		String l_strSrcV;
		String l_strOpt;
		char options[512];
		sprintf( options, "#version 130\n#define %sSEA_CONTRIB\n#define %sSUN_CONTRIB\n#define %sSKY_CONTRIB\n#define %sCLOUDS\n#define %sHARDWARE_ANISTROPIC_FILTERING\n", m_seaContrib ? "" : "NO_", m_sunContrib ? "" : "NO_", m_skyContrib ? "" : "NO_", m_cloudLayer ? "" : "NO_", m_manualFilter ? "NO_" : "" );
		l_strOpt = string::string_cast< xchar >( options );
		Path l_pathShare = Engine::GetDataDirectory() / cuT( "OceanLighting" ) / cuT( "Glsl130_Shaders" );
		TextFile( l_pathShare / cuT( "atmosphere.frag" ), File::eOPEN_MODE_READ ).CopyToString( l_strAtmF );
		TextFile( l_pathShare / cuT( "atmosphere.vert" ), File::eOPEN_MODE_READ ).CopyToString( l_strAtmV );
#if ENABLE_FFT
		TextFile( l_pathShare / cuT( "ocean_fft.frag" ), File::eOPEN_MODE_READ ).CopyToString( l_strOcnF );
		TextFile( l_pathShare / cuT( "ocean_fft.vert" ), File::eOPEN_MODE_READ ).CopyToString( l_strOcnV );
#else
		TextFile( l_pathShare / cuT( "ocean.frag" ), File::eOPEN_MODE_READ ).CopyToString( l_strOcnF );
		TextFile( l_pathShare / cuT( "ocean.vert" ), File::eOPEN_MODE_READ ).CopyToString( l_strOcnV );
#endif
		TextFile( l_pathShare / cuT( "sky.frag" ), File::eOPEN_MODE_READ ).CopyToString( l_strSkyF );
		TextFile( l_pathShare / cuT( "sky.vert" ), File::eOPEN_MODE_READ ).CopyToString( l_strSkyV );
		TextFile( l_pathShare / cuT( "skymap.frag" ), File::eOPEN_MODE_READ ).CopyToString( l_strMapF );
		TextFile( l_pathShare / cuT( "skymap.vert" ), File::eOPEN_MODE_READ ).CopyToString( l_strMapV );
		l_strSrcF = l_strOpt + cuT( "\n" ) + l_strAtmF + cuT( "\n" ) + l_strOcnF;
		l_strSrcV = l_strOpt + cuT( "\n" ) + l_strAtmV + cuT( "\n" ) + l_strOcnV;
		Logger::LogDebug( "Loading 'render' shader program" );
		m_render = GetEngine()->GetRenderSystem()->GetEngine()->GetShaderManager().GetNewProgram();
		m_render->SetSource( eSHADER_TYPE_VERTEX, eSHADER_MODEL_COUNT, l_strVertex + l_strSrcV );
		m_render->SetSource( eSHADER_TYPE_PIXEL, eSHADER_MODEL_COUNT, l_strPixel + l_strSrcF );
		l_pConstants = GetEngine()->GetRenderSystem()->CreateFrameVariableBuffer( cuT( "render" ) );
		m_render->AddFrameVariableBuffer( l_pConstants, MASK_SHADER_TYPE_VERTEX | MASK_SHADER_TYPE_PIXEL );
		m_renderSkyIrradianceSampler = m_render->CreateFrameVariable( cuT( "skyIrradianceSampler" ), eSHADER_TYPE_PIXEL );
		m_renderInscatterSampler = m_render->CreateFrameVariable( cuT( "inscatterSampler" ), eSHADER_TYPE_PIXEL );
		m_renderTransmittanceSampler = m_render->CreateFrameVariable( cuT( "transmittanceSampler" ), eSHADER_TYPE_PIXEL );
		m_renderSkySampler = m_render->CreateFrameVariable( cuT( "skySampler" ), eSHADER_TYPE_PIXEL );
		m_renderScreenToCamera = std::static_pointer_cast< Matrix4x4fFrameVariable >( l_pConstants->CreateVariable( *m_render, eFRAME_VARIABLE_TYPE_MAT4X4F, cuT( "screenToCamera" ) ) );
		m_renderCameraToWorld = std::static_pointer_cast< Matrix4x4fFrameVariable >( l_pConstants->CreateVariable( *m_render, eFRAME_VARIABLE_TYPE_MAT4X4F, cuT( "cameraToWorld" ) ) );
		m_renderWorldToScreen = std::static_pointer_cast< Matrix4x4fFrameVariable >( l_pConstants->CreateVariable( *m_render, eFRAME_VARIABLE_TYPE_MAT4X4F, cuT( "worldToScreen" ) ) );
		m_renderWorldCamera = std::static_pointer_cast< Point3fFrameVariable >( l_pConstants->CreateVariable( *m_render, eFRAME_VARIABLE_TYPE_VEC3F, cuT( "worldCamera" ) ) );
		m_renderWorldSunDir = std::static_pointer_cast< Point3fFrameVariable >( l_pConstants->CreateVariable( *m_render, eFRAME_VARIABLE_TYPE_VEC3F, cuT( "worldSunDir" ) ) );
		m_renderHdrExposure = std::static_pointer_cast< OneFloatFrameVariable >( l_pConstants->CreateVariable( *m_render, eFRAME_VARIABLE_TYPE_FLOAT, cuT( "hdrExposure" ) ) );
		m_renderSeaColor = std::static_pointer_cast< Point3fFrameVariable >( l_pConstants->CreateVariable( *m_render, eFRAME_VARIABLE_TYPE_VEC3F, cuT( "seaColor" ) ) );
		m_renderSkyIrradianceSampler->SetValue( IRRADIANCE_UNIT );
		m_renderInscatterSampler->SetValue( INSCATTER_UNIT );
		m_renderTransmittanceSampler->SetValue( TRANSMITTANCE_UNIT );
		m_renderSkySampler->SetValue( SKY_UNIT );
#if ENABLE_FFT
		m_renderSpectrum_1_2_Sampler = m_render->CreateFrameVariable( cuT( "spectrum_1_2_Sampler" ), eSHADER_TYPE_PIXEL );
		m_renderSpectrum_3_4_Sampler = m_render->CreateFrameVariable( cuT( "spectrum_3_4_Sampler" ), eSHADER_TYPE_PIXEL );
		m_renderFftWavesSampler = m_render->CreateFrameVariable( cuT( "fftWavesSampler" ), eSHADER_TYPE_PIXEL );
		m_renderSlopeVarianceSampler = m_render->CreateFrameVariable( cuT( "slopeVarianceSampler" ), eSHADER_TYPE_PIXEL );
		m_renderGridSizes = std::static_pointer_cast< Point4fFrameVariable >( l_pConstants->CreateVariable( *m_render, eFRAME_VARIABLE_TYPE_VEC4F, cuT( "GRID_SIZES" ) ) );
		m_renderGridSize = std::static_pointer_cast< Point2fFrameVariable >( l_pConstants->CreateVariable( *m_render, eFRAME_VARIABLE_TYPE_VEC2F, cuT( "gridSize" ) ) );
		m_renderChoppy = std::static_pointer_cast< OneFloatFrameVariable >( l_pConstants->CreateVariable( *m_render, eFRAME_VARIABLE_TYPE_FLOAT, cuT( "choppy" ) ) );
#else
		m_renderWavesSampler = m_render->CreateFrameVariable( cuT( "wavesSampler" ), eSHADER_TYPE_PIXEL );
		m_renderWorldToWind = std::static_pointer_cast< Matrix2x2fFrameVariable >( l_pConstants->CreateVariable( *m_render, eFRAME_VARIABLE_TYPE_MAT2X2F, cuT( "worldToWind" ) ) );
		m_renderWindToWorld = std::static_pointer_cast< Matrix2x2fFrameVariable >( l_pConstants->CreateVariable( *m_render, eFRAME_VARIABLE_TYPE_MAT2X2F, cuT( "windToWorld" ) ) );
		m_renderNbWaves = std::static_pointer_cast< OneFloatFrameVariable >( l_pConstants->CreateVariable( *m_render, eFRAME_VARIABLE_TYPE_FLOAT, cuT( "nbWaves" ) ) );
		m_renderHeightOffset = std::static_pointer_cast< OneFloatFrameVariable >( l_pConstants->CreateVariable( *m_render, eFRAME_VARIABLE_TYPE_FLOAT, cuT( "heightOffset" ) ) );
		m_renderSigmaSqTotal = std::static_pointer_cast< Point2fFrameVariable >( l_pConstants->CreateVariable( *m_render, eFRAME_VARIABLE_TYPE_VEC2F, cuT( "sigmaSqTotal" ) ) );
		m_renderTime = std::static_pointer_cast< OneFloatFrameVariable >( l_pConstants->CreateVariable( *m_render, eFRAME_VARIABLE_TYPE_FLOAT, cuT( "time" ) ) );
		m_renderLods = std::static_pointer_cast< Point4fFrameVariable >( l_pConstants->CreateVariable( *m_render, eFRAME_VARIABLE_TYPE_VEC4F, cuT( "lods" ) ) );
		m_renderNyquistMin = std::static_pointer_cast< OneFloatFrameVariable >( l_pConstants->CreateVariable( *m_render, eFRAME_VARIABLE_TYPE_FLOAT, cuT( "nyquistMin" ) ) );
		m_renderNyquistMax = std::static_pointer_cast< OneFloatFrameVariable >( l_pConstants->CreateVariable( *m_render, eFRAME_VARIABLE_TYPE_FLOAT, cuT( "nyquistMax" ) ) );
		m_renderWavesSampler->SetValue( m_pTexWave.get() );
#endif
		m_render->Initialise();

		if ( !all )
		{
			return;
		}

		l_strSrcV = l_strOpt + cuT( "\n" ) + l_strAtmV + cuT( "\n" ) + l_strSkyV;
		l_strSrcF = l_strOpt + cuT( "\n" ) + l_strAtmF + cuT( "\n" ) + l_strSkyF;
		Logger::LogDebug( "Loading 'sky' shader program" );
		m_sky = GetEngine()->GetRenderSystem()->GetEngine()->GetShaderManager().GetNewProgram();
		m_sky->SetSource( eSHADER_TYPE_VERTEX, eSHADER_MODEL_COUNT, l_strVertex + l_strSrcV );
		m_sky->SetSource( eSHADER_TYPE_PIXEL, eSHADER_MODEL_COUNT, l_strPixel + l_strSrcF );
		l_pConstants = GetEngine()->GetRenderSystem()->CreateFrameVariableBuffer( cuT( "sky" ) );
		m_sky->AddFrameVariableBuffer( l_pConstants, MASK_SHADER_TYPE_VERTEX | MASK_SHADER_TYPE_PIXEL );
		m_skySkyIrradianceSampler = m_sky->CreateFrameVariable( cuT( "skyIrradianceSampler" ), eSHADER_TYPE_PIXEL );
		m_skyInscatterSampler = m_sky->CreateFrameVariable( cuT( "inscatterSampler" ), eSHADER_TYPE_PIXEL );
		m_skyTransmittanceSampler = m_sky->CreateFrameVariable( cuT( "transmittanceSampler" ), eSHADER_TYPE_PIXEL );
		m_skySkySampler = m_sky->CreateFrameVariable( cuT( "skySampler" ), eSHADER_TYPE_PIXEL );
		m_skyScreenToCamera = std::static_pointer_cast< Matrix4x4fFrameVariable >( l_pConstants->CreateVariable( *m_sky, eFRAME_VARIABLE_TYPE_MAT4X4F, cuT( "screenToCamera" ) ) );
		m_skyCameraToWorld = std::static_pointer_cast< Matrix4x4fFrameVariable >( l_pConstants->CreateVariable( *m_sky, eFRAME_VARIABLE_TYPE_MAT4X4F, cuT( "cameraToWorld" ) ) );
		m_skyWorldCamera = std::static_pointer_cast< Point3fFrameVariable >( l_pConstants->CreateVariable( *m_sky, eFRAME_VARIABLE_TYPE_VEC3F, cuT( "worldCamera" ) ) );
		m_skyWorldSunDir = std::static_pointer_cast< Point3fFrameVariable >( l_pConstants->CreateVariable( *m_sky, eFRAME_VARIABLE_TYPE_VEC3F, cuT( "worldSunDir" ) ) );
		m_skyHdrExposure = std::static_pointer_cast< OneFloatFrameVariable >( l_pConstants->CreateVariable( *m_sky, eFRAME_VARIABLE_TYPE_FLOAT, cuT( "hdrExposure" ) ) );
		m_skySkyIrradianceSampler->SetValue( IRRADIANCE_UNIT );
		m_skyInscatterSampler->SetValue( INSCATTER_UNIT );
		m_skyTransmittanceSampler->SetValue( TRANSMITTANCE_UNIT );
		m_skySkySampler->SetValue( SKY_UNIT );
		m_sky->Initialise();
		l_strSrcV = l_strOpt + cuT( "\n" ) + l_strAtmV + cuT( "\n" ) + l_strMapV;
		l_strSrcF = l_strOpt + cuT( "\n" ) + l_strAtmF + cuT( "\n" ) + l_strMapF;
		Logger::LogDebug( "Loading 'skymap' shader program" );
		m_skymap = GetEngine()->GetRenderSystem()->GetEngine()->GetShaderManager().GetNewProgram();
		m_skymap->SetSource( eSHADER_TYPE_VERTEX, eSHADER_MODEL_COUNT, l_strVertex + l_strSrcV );
		m_skymap->SetSource( eSHADER_TYPE_PIXEL, eSHADER_MODEL_COUNT, l_strPixel + l_strSrcF );
		l_pConstants = GetEngine()->GetRenderSystem()->CreateFrameVariableBuffer( cuT( "skymap" ) );
		m_skymap->AddFrameVariableBuffer( l_pConstants, MASK_SHADER_TYPE_VERTEX | MASK_SHADER_TYPE_PIXEL );
		m_skymapSkyIrradianceSampler = m_skymap->CreateFrameVariable( cuT( "skyIrradianceSampler" ), eSHADER_TYPE_PIXEL );
		m_skymapInscatterSampler = m_skymap->CreateFrameVariable( cuT( "inscatterSampler" ), eSHADER_TYPE_PIXEL );
		m_skymapTransmittanceSampler = m_skymap->CreateFrameVariable( cuT( "transmittanceSampler" ), eSHADER_TYPE_PIXEL );
		m_skymapNoiseSampler = m_skymap->CreateFrameVariable( cuT( "noiseSampler" ), eSHADER_TYPE_PIXEL );
		m_skymapSunDir = std::static_pointer_cast< Point3fFrameVariable >( l_pConstants->CreateVariable( *m_skymap, eFRAME_VARIABLE_TYPE_VEC3F, cuT( "sunDir" ) ) );
		m_skymapOctaves = std::static_pointer_cast< OneFloatFrameVariable >( l_pConstants->CreateVariable( *m_skymap, eFRAME_VARIABLE_TYPE_FLOAT, cuT( "octaves" ) ) );
		m_skymapLacunarity = std::static_pointer_cast< OneFloatFrameVariable >( l_pConstants->CreateVariable( *m_skymap, eFRAME_VARIABLE_TYPE_FLOAT, cuT( "lacunarity" ) ) );
		m_skymapGain = std::static_pointer_cast< OneFloatFrameVariable >( l_pConstants->CreateVariable( *m_skymap, eFRAME_VARIABLE_TYPE_FLOAT, cuT( "gain" ) ) );
		m_skymapNorm = std::static_pointer_cast< OneFloatFrameVariable >( l_pConstants->CreateVariable( *m_skymap, eFRAME_VARIABLE_TYPE_FLOAT, cuT( "norm" ) ) );
		m_skymapClamp1 = std::static_pointer_cast< OneFloatFrameVariable >( l_pConstants->CreateVariable( *m_skymap, eFRAME_VARIABLE_TYPE_FLOAT, cuT( "clamp1" ) ) );
		m_skymapClamp2 = std::static_pointer_cast< OneFloatFrameVariable >( l_pConstants->CreateVariable( *m_skymap, eFRAME_VARIABLE_TYPE_FLOAT, cuT( "clamp2" ) ) );
		m_skymapCloudsColor = std::static_pointer_cast< Point4fFrameVariable >( l_pConstants->CreateVariable( *m_skymap, eFRAME_VARIABLE_TYPE_VEC4F, cuT( "cloudsColor" ) ) );
		m_skymapSkyIrradianceSampler->SetValue( IRRADIANCE_UNIT );
		m_skymapInscatterSampler->SetValue( INSCATTER_UNIT );
		m_skymapTransmittanceSampler->SetValue( TRANSMITTANCE_UNIT );
		m_skymapNoiseSampler->SetValue( NOISE_UNIT );
		m_skymap->Initialise();

		if ( !m_clouds )
		{
			String l_strCloV;
			String l_strCloF;
			TextFile( l_pathShare / cuT( "clouds.vert" ), File::eOPEN_MODE_READ ).CopyToString( l_strCloV );
			TextFile( l_pathShare / cuT( "clouds.frag" ), File::eOPEN_MODE_READ ).CopyToString( l_strCloF );
			l_strSrcV = l_strOpt + cuT( "\n" ) + l_strAtmV + cuT( "\n" ) + l_strCloV;
			l_strSrcF = l_strOpt + cuT( "\n" ) + l_strAtmF + cuT( "\n" ) + l_strCloF;
			Logger::LogDebug( "Loading 'clouds' shader program" );
			m_clouds = GetEngine()->GetRenderSystem()->GetEngine()->GetShaderManager().GetNewProgram();
			m_clouds->SetSource( eSHADER_TYPE_VERTEX, eSHADER_MODEL_COUNT, l_strVertex + l_strSrcV );
			m_clouds->SetSource( eSHADER_TYPE_PIXEL, eSHADER_MODEL_COUNT, l_strPixel + l_strSrcF );
			l_pConstants = GetEngine()->GetRenderSystem()->CreateFrameVariableBuffer( cuT( "clouds" ) );
			m_clouds->AddFrameVariableBuffer( l_pConstants, MASK_SHADER_TYPE_VERTEX | MASK_SHADER_TYPE_PIXEL );
			m_cloudsSkyIrradianceSampler = m_clouds->CreateFrameVariable( cuT( "skyIrradianceSampler" ), eSHADER_TYPE_PIXEL );
			m_cloudsInscatterSampler = m_clouds->CreateFrameVariable( cuT( "inscatterSampler" ), eSHADER_TYPE_PIXEL );
			m_cloudsTransmittanceSampler = m_clouds->CreateFrameVariable( cuT( "transmittanceSampler" ), eSHADER_TYPE_PIXEL );
			m_cloudsNoiseSampler = m_clouds->CreateFrameVariable( cuT( "noiseSampler" ), eSHADER_TYPE_PIXEL );
			m_cloudsWorldToScreen = std::static_pointer_cast< Matrix4x4fFrameVariable >( l_pConstants->CreateVariable( *m_clouds, eFRAME_VARIABLE_TYPE_MAT4X4F, cuT( "worldToScreen" ) ) );
			m_cloudsWorldCamera = std::static_pointer_cast< Point3fFrameVariable >( l_pConstants->CreateVariable( *m_clouds, eFRAME_VARIABLE_TYPE_VEC3F, cuT( "worldCamera" ) ) );
			m_cloudsWorldSunDir = std::static_pointer_cast< Point3fFrameVariable >( l_pConstants->CreateVariable( *m_clouds, eFRAME_VARIABLE_TYPE_VEC3F, cuT( "worldSunDir" ) ) );
			m_cloudsHdrExposure = std::static_pointer_cast< OneFloatFrameVariable >( l_pConstants->CreateVariable( *m_clouds, eFRAME_VARIABLE_TYPE_FLOAT, cuT( "hdrExposure" ) ) );
			m_cloudsOctaves = std::static_pointer_cast< OneFloatFrameVariable >( l_pConstants->CreateVariable( *m_clouds, eFRAME_VARIABLE_TYPE_FLOAT, cuT( "octaves" ) ) );
			m_cloudsLacunarity = std::static_pointer_cast< OneFloatFrameVariable >( l_pConstants->CreateVariable( *m_clouds, eFRAME_VARIABLE_TYPE_FLOAT, cuT( "lacunarity" ) ) );
			m_cloudsGain = std::static_pointer_cast< OneFloatFrameVariable >( l_pConstants->CreateVariable( *m_clouds, eFRAME_VARIABLE_TYPE_FLOAT, cuT( "gain" ) ) );
			m_cloudsNorm = std::static_pointer_cast< OneFloatFrameVariable >( l_pConstants->CreateVariable( *m_clouds, eFRAME_VARIABLE_TYPE_FLOAT, cuT( "norm" ) ) );
			m_cloudsClamp1 = std::static_pointer_cast< OneFloatFrameVariable >( l_pConstants->CreateVariable( *m_clouds, eFRAME_VARIABLE_TYPE_FLOAT, cuT( "clamp1" ) ) );
			m_cloudsClamp2 = std::static_pointer_cast< OneFloatFrameVariable >( l_pConstants->CreateVariable( *m_clouds, eFRAME_VARIABLE_TYPE_FLOAT, cuT( "clamp2" ) ) );
			m_cloudsCloudsColor = std::static_pointer_cast< Point4fFrameVariable >( l_pConstants->CreateVariable( *m_clouds, eFRAME_VARIABLE_TYPE_VEC4F, cuT( "cloudsColor" ) ) );
			m_cloudsSkyIrradianceSampler->SetValue( IRRADIANCE_UNIT );
			m_cloudsInscatterSampler->SetValue( INSCATTER_UNIT );
			m_cloudsTransmittanceSampler->SetValue( TRANSMITTANCE_UNIT );
			m_cloudsNoiseSampler->SetValue( NOISE_UNIT );
			m_clouds->Initialise();
		}

#if ENABLE_FFT
		String l_strIniF;
		String l_strIniV;
		String l_strVarF;
		String l_strVarV;
		String l_strFtxF;
		String l_strFtxV;
		String l_strFtyF;
		String l_strFtyV;
		TextFile( l_pathShare / cuT( "init.frag" ), File::eOPEN_MODE_READ ).CopyToString( l_strIniF );
		TextFile( l_pathShare / cuT( "init.vert" ), File::eOPEN_MODE_READ ).CopyToString( l_strIniV );
		TextFile( l_pathShare / cuT( "variances.frag" ), File::eOPEN_MODE_READ ).CopyToString( l_strVarF );
		TextFile( l_pathShare / cuT( "variances.vert" ), File::eOPEN_MODE_READ ).CopyToString( l_strVarV );
		TextFile( l_pathShare / cuT( "fftx.frag" ), File::eOPEN_MODE_READ ).CopyToString( l_strFtxF );
		TextFile( l_pathShare / cuT( "fftx.vert" ), File::eOPEN_MODE_READ ).CopyToString( l_strFtxV );
		TextFile( l_pathShare / cuT( "ffty.frag" ), File::eOPEN_MODE_READ ).CopyToString( l_strFtyF );
		TextFile( l_pathShare / cuT( "ffty.vert" ), File::eOPEN_MODE_READ ).CopyToString( l_strFtyV );
		l_strSrcV = l_strIniV;
		l_strSrcF = l_strIniF;
		Logger::LogDebug( "Loading 'init' shader program" );
		m_init = GetEngine()->GetRenderSystem()->GetEngine()->GetShaderManager().GetNewProgram();
		m_init->SetSource( eSHADER_TYPE_VERTEX, eSHADER_MODEL_COUNT, l_strVertex + l_strSrcV );
		m_init->SetSource( eSHADER_TYPE_PIXEL, eSHADER_MODEL_COUNT, l_strPixel + l_strSrcF );
		l_pConstants = GetEngine()->GetRenderSystem()->CreateFrameVariableBuffer( cuT( "init" ) );
		m_init->AddFrameVariableBuffer( l_pConstants, MASK_SHADER_TYPE_VERTEX | MASK_SHADER_TYPE_PIXEL );
		m_initSpectrum_1_2_Sampler = m_init->CreateFrameVariable( cuT( "spectrum_1_2_Sampler" ), eSHADER_TYPE_PIXEL );
		m_initSpectrum_3_4_Sampler = m_init->CreateFrameVariable( cuT( "spectrum_3_4_Sampler" ), eSHADER_TYPE_PIXEL );
		m_initFftSize = std::static_pointer_cast< OneFloatFrameVariable >( l_pConstants->CreateVariable( *m_init, eFRAME_VARIABLE_TYPE_FLOAT, cuT( "FFT_SIZE" ) ) );
		m_initInverseGridSizes = std::static_pointer_cast< Point4fFrameVariable >( l_pConstants->CreateVariable( *m_init, eFRAME_VARIABLE_TYPE_VEC4F, cuT( "INVERSE_GRID_SIZES" ) ) );
		m_initT = std::static_pointer_cast< OneFloatFrameVariable >( l_pConstants->CreateVariable( *m_init, eFRAME_VARIABLE_TYPE_FLOAT, cuT( "t" ) ) );
		m_initSpectrum_1_2_Sampler->SetValue( SPECTRUM_1_2_UNIT );
		m_initSpectrum_3_4_Sampler->SetValue( SPECTRUM_3_4_UNIT );
		m_init->Initialise();
		l_strSrcV = l_strVarV;
		l_strSrcF = l_strVarF;
		Logger::LogDebug( "Loading 'variances' shader program" );
		m_variances = GetEngine()->GetRenderSystem()->GetEngine()->GetShaderManager().GetNewProgram();
		m_variances->SetSource( eSHADER_TYPE_VERTEX, eSHADER_MODEL_COUNT, l_strVertex + l_strSrcV );
		m_variances->SetSource( eSHADER_TYPE_PIXEL, eSHADER_MODEL_COUNT, l_strPixel + l_strSrcF );
		l_pConstants = GetEngine()->GetRenderSystem()->CreateFrameVariableBuffer( cuT( "variances" ) );
		m_variances->AddFrameVariableBuffer( l_pConstants, MASK_SHADER_TYPE_VERTEX | MASK_SHADER_TYPE_PIXEL );
		m_variancesSpectrum_1_2_Sampler = m_variances->CreateFrameVariable( cuT( "spectrum_1_2_Sampler" ), eSHADER_TYPE_PIXEL );
		m_variancesSpectrum_3_4_Sampler = m_variances->CreateFrameVariable( cuT( "spectrum_3_4_Sampler" ), eSHADER_TYPE_PIXEL );
		m_variancesNSlopeVariance = std::static_pointer_cast< OneFloatFrameVariable >( l_pConstants->CreateVariable( *m_variances, eFRAME_VARIABLE_TYPE_FLOAT, cuT( "N_SLOPE_VARIANCE" ) ) );
		m_variancesFFTSize = std::static_pointer_cast< OneIntFrameVariable >( l_pConstants->CreateVariable( *m_variances, eFRAME_VARIABLE_TYPE_INT, cuT( "FFT_SIZE" ) ) );
		m_variancesGridSizes = std::static_pointer_cast< Point4fFrameVariable >( l_pConstants->CreateVariable( *m_variances, eFRAME_VARIABLE_TYPE_VEC4F, cuT( "GRID_SIZES" ) ) );
		m_variancesSlopeVarianceDelta = std::static_pointer_cast< OneFloatFrameVariable >( l_pConstants->CreateVariable( *m_variances, eFRAME_VARIABLE_TYPE_FLOAT, cuT( "slopeVarianceDelta" ) ) );
		m_variancesC = std::static_pointer_cast< OneFloatFrameVariable >( l_pConstants->CreateVariable( *m_variances, eFRAME_VARIABLE_TYPE_FLOAT, cuT( "c" ) ) );
		m_variancesSpectrum_1_2_Sampler->SetValue( SPECTRUM_1_2_UNIT );
		m_variancesSpectrum_3_4_Sampler->SetValue( SPECTRUM_3_4_UNIT );
		m_variancesNSlopeVariance->SetValue( float( m_N_SLOPE_VARIANCE ) );
		m_variancesFFTSize->SetValue( m_FFT_SIZE );
		m_variances->Initialise();
		l_strSrcV = l_strFtxV;
		l_strSrcF = l_strFtxF;
		Logger::LogDebug( "Loading 'fftx' shader program" );
		m_fftx = GetEngine()->GetRenderSystem()->GetEngine()->GetShaderManager().GetNewProgram();
		m_fftx->SetSource( eSHADER_TYPE_VERTEX, eSHADER_MODEL_COUNT, l_strVertex + l_strSrcV );
		m_fftx->SetSource( eSHADER_TYPE_PIXEL, eSHADER_MODEL_COUNT, l_strPixel + l_strSrcF );
		l_pConstants = GetEngine()->GetRenderSystem()->CreateFrameVariableBuffer( cuT( "fftx" ) );
		m_fftx->AddFrameVariableBuffer( l_pConstants, MASK_SHADER_TYPE_VERTEX | MASK_SHADER_TYPE_PIXEL );
		m_fftxButterflySampler = m_fftx->CreateFrameVariable( cuT( "butterflySampler" ), eSHADER_TYPE_PIXEL );
		m_fftxImgSampler = m_fftx->CreateFrameVariable( cuT( "imgSampler" ), eSHADER_TYPE_PIXEL );
		m_fftxNLayers = std::static_pointer_cast< OneIntFrameVariable >( l_pConstants->CreateVariable( *m_fftx, eFRAME_VARIABLE_TYPE_INT, cuT( "nLayers" ) ) );
		m_fftxPass = std::static_pointer_cast< OneFloatFrameVariable >( l_pConstants->CreateVariable( *m_fftx, eFRAME_VARIABLE_TYPE_FLOAT, cuT( "pass" ) ) );
		m_fftxButterflySampler->SetValue( BUTTERFLY_UNIT );
		m_fftx->Initialise();
		l_strSrcV = l_strFtyV;
		l_strSrcF = l_strFtyF;
		Logger::LogDebug( "Loading 'ffty' shader program" );
		m_ffty = GetEngine()->GetRenderSystem()->GetEngine()->GetShaderManager().GetNewProgram();
		m_ffty->SetSource( eSHADER_TYPE_VERTEX, eSHADER_MODEL_COUNT, l_strVertex + l_strSrcV );
		m_ffty->SetSource( eSHADER_TYPE_PIXEL, eSHADER_MODEL_COUNT, l_strPixel + l_strSrcF );
		l_pConstants = GetEngine()->GetRenderSystem()->CreateFrameVariableBuffer( cuT( "ffty" ) );
		m_ffty->AddFrameVariableBuffer( l_pConstants, MASK_SHADER_TYPE_VERTEX | MASK_SHADER_TYPE_PIXEL );
		m_fftyButterflySampler = m_ffty->CreateFrameVariable( cuT( "butterflySampler" ), eSHADER_TYPE_PIXEL );
		m_fftyImgSampler = m_ffty->CreateFrameVariable( cuT( "imgSampler" ), eSHADER_TYPE_PIXEL );
		m_fftyNLayers = std::static_pointer_cast< OneIntFrameVariable >( l_pConstants->CreateVariable( *m_ffty, eFRAME_VARIABLE_TYPE_INT, cuT( "nLayers" ) ) );
		m_fftyPass = std::static_pointer_cast< OneFloatFrameVariable >( l_pConstants->CreateVariable( *m_ffty, eFRAME_VARIABLE_TYPE_FLOAT, cuT( "pass" ) ) );
		m_fftyButterflySampler->SetValue( BUTTERFLY_UNIT );
		m_ffty->Initialise();
#endif
	}

	// ----------------------------------------------------------------------------
	// MESH GENERATION
	// ----------------------------------------------------------------------------

	void RenderTechnique::generateMesh()
	{
		if ( m_renderGBuffers )
		{
			m_renderVtxBuffer->Cleanup();
			m_renderVtxBuffer->Destroy();
			m_renderIdxBuffer->Cleanup();
			m_renderIdxBuffer->Destroy();
			m_renderGBuffers.reset();
		}

		BufferElementDeclaration l_meshDeclaration[] =
		{
			BufferElementDeclaration( ShaderProgram::Position, eELEMENT_USAGE_POSITION, eELEMENT_TYPE_4FLOATS )
		};
		float horizon = float( tan( m_cameraTheta / 180.0 * M_PI ) );
		float s = std::min< float >( 1.1f, 0.5f + horizon * 0.5f );
		float vmargin = 0.1f;
		float hmargin = 0.1f;
		m_vboParams = Point4f( float( m_width ), float( m_height ), m_gridSize, m_cameraTheta );
		std::vector< real > data( 4 * int( ceil( m_height * ( s + vmargin ) / m_gridSize ) + 5 ) * int( ceil( m_width * ( 1.0 + 2.0 * hmargin ) / m_gridSize ) + 5 ) );
		int n = 0;
		int nx = 0;

		for ( double j = m_height * s - 0.1; j > -m_height * vmargin - m_gridSize; j -= m_gridSize )
		{
			nx = 0;

			for ( double i = -m_width * hmargin; i < m_width * ( 1.0 + hmargin ) + m_gridSize; i += m_gridSize )
			{
				data[n++] = float( -1.0 + 2.0 * i / m_width );
				data[n++] = float( -1.0 + 2.0 * j / m_height );
				data[n++] = float( 0.0 );
				data[n++] = float( 1.0 );
				nx++;
			}
		}

		VertexBufferUPtr l_pVtxBuffer = std::make_unique< VertexBuffer >( *GetEngine(), l_meshDeclaration );
		l_pVtxBuffer->Resize( n * sizeof( float ) );
		std::memcpy( l_pVtxBuffer->data(), &data[0], n * sizeof( float ) );
		std::vector< uint32_t > indices( 6 * int( ceil( m_height * ( s + vmargin ) / m_gridSize ) + 4 ) * int( ceil( m_width * ( 1.0 + 2.0 * hmargin ) / m_gridSize ) + 4 ) );
		n = 0;
		int nj = 0;

		for ( float j = float( m_height * s - 0.1 ); j > -m_height * vmargin; j -= m_gridSize )
		{
			int ni = 0;

			for ( float i = -m_width * hmargin; i < m_width * ( 1.0 + hmargin ); i += m_gridSize )
			{
				indices[n++] = ni + ( nj + 1 ) * nx;
				indices[n++] = ( ni + 1 ) + ( nj + 1 ) * nx;
				indices[n++] = ( ni + 1 ) + nj * nx;
				indices[n++] = ( ni + 1 ) + nj * nx;
				indices[n++] = ni + ( nj + 1 ) * nx;
				indices[n++] = ni + nj * nx;
				ni++;
			}

			nj++;
		}

		IndexBufferUPtr l_pIdxBuffer = std::make_unique< IndexBuffer >( *GetEngine() );
		l_pIdxBuffer->Resize( n );
		std::memcpy( l_pIdxBuffer->data(), &indices[0], n * sizeof( uint32_t ) );
		m_renderVtxBuffer->Create();
		m_renderIdxBuffer->Create();
		m_renderVtxBuffer->Initialise( eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW );
		m_renderIdxBuffer->Initialise( eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW );
		m_renderGBuffers = GetEngine()->GetRenderSystem()->CreateGeometryBuffers( eTOPOLOGY_TRIANGLES, *m_render, m_renderVtxBuffer.get(), m_renderIdxBuffer.get(), nullptr, nullptr );
	}

	bool RenderTechnique::DoCreate()
	{
		m_pSamplerNearestClamp->SetWrappingMode( eTEXTURE_UVW_U, eWRAP_MODE_CLAMP_TO_EDGE );
		m_pSamplerNearestClamp->SetWrappingMode( eTEXTURE_UVW_V, eWRAP_MODE_CLAMP_TO_EDGE );
		m_pSamplerNearestClamp->SetWrappingMode( eTEXTURE_UVW_W, eWRAP_MODE_CLAMP_TO_EDGE );
		m_pSamplerNearestClamp->SetInterpolationMode( eINTERPOLATION_FILTER_MIN, eINTERPOLATION_MODE_NEAREST );
		m_pSamplerNearestClamp->SetInterpolationMode( eINTERPOLATION_FILTER_MAG, eINTERPOLATION_MODE_NEAREST );
		m_pSamplerNearestRepeat->SetWrappingMode( eTEXTURE_UVW_U, eWRAP_MODE_REPEAT );
		m_pSamplerNearestRepeat->SetWrappingMode( eTEXTURE_UVW_V, eWRAP_MODE_REPEAT );
		m_pSamplerNearestRepeat->SetWrappingMode( eTEXTURE_UVW_W, eWRAP_MODE_REPEAT );
		m_pSamplerNearestRepeat->SetInterpolationMode( eINTERPOLATION_FILTER_MIN, eINTERPOLATION_MODE_NEAREST );
		m_pSamplerNearestRepeat->SetInterpolationMode( eINTERPOLATION_FILTER_MAG, eINTERPOLATION_MODE_NEAREST );
		m_pSamplerLinearClamp->SetWrappingMode( eTEXTURE_UVW_U, eWRAP_MODE_CLAMP_TO_EDGE );
		m_pSamplerLinearClamp->SetWrappingMode( eTEXTURE_UVW_V, eWRAP_MODE_CLAMP_TO_EDGE );
		m_pSamplerLinearClamp->SetWrappingMode( eTEXTURE_UVW_W, eWRAP_MODE_CLAMP_TO_EDGE );
		m_pSamplerLinearClamp->SetInterpolationMode( eINTERPOLATION_FILTER_MIN, eINTERPOLATION_MODE_LINEAR );
		m_pSamplerLinearClamp->SetInterpolationMode( eINTERPOLATION_FILTER_MAG, eINTERPOLATION_MODE_LINEAR );
		m_pSamplerLinearRepeat->SetWrappingMode( eTEXTURE_UVW_U, eWRAP_MODE_REPEAT );
		m_pSamplerLinearRepeat->SetWrappingMode( eTEXTURE_UVW_V, eWRAP_MODE_REPEAT );
		m_pSamplerLinearRepeat->SetWrappingMode( eTEXTURE_UVW_W, eWRAP_MODE_REPEAT );
		m_pSamplerLinearRepeat->SetInterpolationMode( eINTERPOLATION_FILTER_MIN, eINTERPOLATION_MODE_LINEAR );
		m_pSamplerLinearRepeat->SetInterpolationMode( eINTERPOLATION_FILTER_MAG, eINTERPOLATION_MODE_LINEAR );
		m_pSamplerAnisotropicClamp->SetWrappingMode( eTEXTURE_UVW_U, eWRAP_MODE_CLAMP_TO_EDGE );
		m_pSamplerAnisotropicClamp->SetWrappingMode( eTEXTURE_UVW_V, eWRAP_MODE_CLAMP_TO_EDGE );
		m_pSamplerAnisotropicClamp->SetWrappingMode( eTEXTURE_UVW_W, eWRAP_MODE_CLAMP_TO_EDGE );
		m_pSamplerAnisotropicClamp->SetInterpolationMode( eINTERPOLATION_FILTER_MIN, eINTERPOLATION_MODE_LINEAR );
		m_pSamplerAnisotropicClamp->SetInterpolationMode( eINTERPOLATION_FILTER_MAG, eINTERPOLATION_MODE_LINEAR );
		m_pSamplerAnisotropicClamp->SetMaxAnisotropy( real( 0.1 ) );
		m_pSamplerAnisotropicRepeat->SetWrappingMode( eTEXTURE_UVW_U, eWRAP_MODE_REPEAT );
		m_pSamplerAnisotropicRepeat->SetWrappingMode( eTEXTURE_UVW_V, eWRAP_MODE_REPEAT );
		m_pSamplerAnisotropicRepeat->SetWrappingMode( eTEXTURE_UVW_W, eWRAP_MODE_REPEAT );
		m_pSamplerAnisotropicRepeat->SetInterpolationMode( eINTERPOLATION_FILTER_MIN, eINTERPOLATION_MODE_LINEAR );
		m_pSamplerAnisotropicRepeat->SetInterpolationMode( eINTERPOLATION_FILTER_MAG, eINTERPOLATION_MODE_LINEAR );
		m_pSamplerAnisotropicRepeat->SetMaxAnisotropy( real( 0.1 ) );
		m_frameBuffer->Create( 0 );
		m_pColorBuffer->Create();
		m_pDepthBuffer->Create();
		m_pTexIrradiance->Create();
		m_pTexInscatter->Create();
		m_pTexTransmittance->Create();
		m_pTexSky->Create();
		m_pTexNoise->Create();
		m_fbo->Create( 0 );
		m_skyVtxBuffer->Create();
		m_skymapVtxBuffer->Create();
		m_cloudsVtxBuffer->Create();
		m_skyIdxBuffer->Create();
		m_skymapIdxBuffer->Create();
		m_cloudsIdxBuffer->Create();
#if ENABLE_FFT
		m_pTexSpectrum_1_2->Create();
		m_pTexSpectrum_3_4->Create();
		m_pTexSlopeVariance->Create();
		m_pTexFFTA->Create();
		m_pTexFFTB->Create();
		m_pTexButterfly->Create();
		m_variancesFbo->Create( 0 );
		m_fftFbo1->Create( 0 );
		m_fftFbo2->Create( 0 );
		m_variancesVtxBuffer->Create();
		m_variancesIdxBuffer->Create();
		m_initVtxBuffer->Create();
		m_initIdxBuffer->Create();
		m_fftxVtxBuffer->Create();
		m_fftxIdxBuffer->Create();
		m_fftyVtxBuffer->Create();
		m_fftyIdxBuffer->Create();
#else
		m_pTexWave->Create();
#endif
		BlendStateSPtr l_pBlendState = GetEngine()->GetRenderSystem()->GetEngine()->GetBlendStateManager().Create( cuT( "OL_Clouds" ) );
		l_pBlendState->EnableBlend( true );
		l_pBlendState->SetAlphaSrcBlend( eBLEND_SRC_ALPHA );
		l_pBlendState->SetAlphaDstBlend( eBLEND_INV_SRC_ALPHA );
		m_cloudsBlendState = l_pBlendState;
		m_renderBlendState = GetEngine()->GetRenderSystem()->GetEngine()->GetBlendStateManager().Create( cuT( "OL_Render" ) );
		m_skyBlendState = GetEngine()->GetRenderSystem()->GetEngine()->GetBlendStateManager().Create( cuT( "OL_Sky" ) );
		m_skymapBlendState = GetEngine()->GetRenderSystem()->GetEngine()->GetBlendStateManager().Create( cuT( "OL_Skymap" ) );
#if ENABLE_FFT
		m_initBlendState = GetEngine()->GetRenderSystem()->GetEngine()->GetBlendStateManager().Create( cuT( "OL_Init" ) );
		m_variancesBlendState = GetEngine()->GetRenderSystem()->GetEngine()->GetBlendStateManager().Create( cuT( "OL_Variances" ) );
		m_fftxBlendState = GetEngine()->GetRenderSystem()->GetEngine()->GetBlendStateManager().Create( cuT( "OL_Fftx" ) );
		m_fftyBlendState = GetEngine()->GetRenderSystem()->GetEngine()->GetBlendStateManager().Create( cuT( "OL_Ffty" ) );
#endif
		return true;
	}

	void RenderTechnique::DoDestroy()
	{
		m_skyVtxBuffer->Destroy();
		m_skymapVtxBuffer->Destroy();
		m_cloudsVtxBuffer->Destroy();
		m_skyIdxBuffer->Destroy();
		m_skymapIdxBuffer->Destroy();
		m_cloudsIdxBuffer->Destroy();
		m_renderVtxBuffer->Destroy();
		m_renderIdxBuffer->Destroy();
#if ENABLE_FFT
		m_variancesVtxBuffer->Destroy();
		m_variancesIdxBuffer->Destroy();
		m_initVtxBuffer->Destroy();
		m_initIdxBuffer->Destroy();
		m_fftxVtxBuffer->Destroy();
		m_fftxIdxBuffer->Destroy();
		m_fftyVtxBuffer->Destroy();
		m_fftyIdxBuffer->Destroy();
#endif
		DoDestroyPrograms( true );
		m_pTexIrradiance->Destroy();
		m_pTexInscatter->Destroy();
		m_pTexTransmittance->Destroy();
		m_pTexSky->Destroy();
		m_pTexNoise->Destroy();
		m_fbo->Destroy();
#if ENABLE_FFT
		m_pTexSpectrum_1_2->Destroy();
		m_pTexSpectrum_3_4->Destroy();
		m_pTexSlopeVariance->Destroy();
		m_pTexFFTA->Destroy();
		m_pTexFFTB->Destroy();
		m_pTexButterfly->Destroy();
		m_variancesFbo->Destroy();
		m_fftFbo1->Destroy();
		m_fftFbo2->Destroy();
#else
		m_pTexWave->Destroy();
#endif
		m_pColorBuffer->Destroy();
		m_pDepthBuffer->Destroy();
		m_frameBuffer->Destroy();
	}

	bool RenderTechnique::DoInitialise( uint32_t & p_index )
	{
		m_pSamplerNearestClamp->Initialise();
		m_pSamplerNearestRepeat->Initialise();
		m_pSamplerLinearClamp->Initialise();
		m_pSamplerLinearRepeat->Initialise();
		m_pSamplerAnisotropicClamp->Initialise();
		m_pSamplerAnisotropicRepeat->Initialise();

		m_pColorBuffer->SetType( eTEXTURE_TYPE_2D );
		m_pColorBuffer->SetImage( m_renderTarget->GetSize(), ePIXEL_FORMAT_A8R8G8B8 );
		m_pDepthBuffer->Initialise( m_renderTarget->GetSize() );
		m_frameBuffer->Initialise( m_renderTarget->GetSize() );
		bool l_bReturn = m_frameBuffer->Bind( eFRAMEBUFFER_MODE_CONFIG );

		if ( l_bReturn )
		{
			l_bReturn &= m_frameBuffer->Attach( eATTACHMENT_POINT_COLOUR, m_pColorAttach, eTEXTURE_TARGET_2D );
			l_bReturn &= m_frameBuffer->Attach( eATTACHMENT_POINT_DEPTH, m_pDepthAttach );
			m_frameBuffer->Unbind();
		}

		FILE * f = NULL;
		m_pTexIrradiance->SetType( eTEXTURE_TYPE_2D );
		PxBufferBaseSPtr buffer = PxBufferBase::create( Size( 64, 16 ), ePIXEL_FORMAT_RGB16F32F );

		if ( Castor::FOpen( f, string::string_cast< char >( Engine::GetDataDirectory() / cuT( "OceanLighting/data/irradiance.raw" ) ).c_str(), "rb" ) )
		{
			ENSURE( fread( buffer->ptr(), 1, 16 * 64 * 3 * sizeof( float ), f ) <= 16 * 64 * 3 * sizeof( float ) );
			fclose( f );
		}

		m_pTexIrradiance->SetImage( buffer );
		m_pTexIrradiance->Initialise();
		int res = 64;
		int nr = res / 2;
		int nv = res * 2;
		int nb = res / 2;
		int na = 8;
		m_pTexInscatter->SetType( eTEXTURE_TYPE_3D );
		buffer = PxBufferBase::create( Size( na * nb, nv * nr ), ePIXEL_FORMAT_RGB16F32F );

		if ( Castor::FOpen( f, string::string_cast< char >( Engine::GetDataDirectory() / cuT( "OceanLighting/data/inscatter.raw" ) ).c_str(), "rb" ) )
		{
			ENSURE( fread( buffer->ptr(), 1, nr * nv * nb * na * 4 * sizeof( float ), f ) <= nr * nv * nb * na * 4 * sizeof( float ) );
			fclose( f );
		}

		m_pTexInscatter->SetImage( Point3ui( na * nb, nv, nr ), buffer );
		m_pTexInscatter->Initialise();
		m_pTexTransmittance->SetType( eTEXTURE_TYPE_2D );
		buffer = PxBufferBase::create( Size( 256, 64 ), ePIXEL_FORMAT_RGB16F32F );

		if ( Castor::FOpen( f, string::string_cast< char >( Engine::GetDataDirectory() / cuT( "OceanLighting/data/transmittance.raw" ) ).c_str(), "rb" ) )
		{
			ENSURE( fread( buffer->ptr(), 1, 256 * 64 * 3 * sizeof( float ), f ) <= 256 * 64 * 3 * sizeof( float ) );
			fclose( f );
		}

		m_pTexTransmittance->SetImage( buffer );
		m_pTexTransmittance->Initialise();
		m_pTexNoise->SetType( eTEXTURE_TYPE_2D );
		m_pTexNoise->SetImage( Size( 512, 512 ), ePIXEL_FORMAT_L8 );

		if ( Castor::FOpen( f, string::string_cast< char >( Engine::GetDataDirectory() / cuT( "OceanLighting/data/noise.pgm" ) ).c_str(), "rb" ) )
		{
			unsigned char * img = new unsigned char[512 * 512 + 38];
			ENSURE( fread( img, 1, 512 * 512 + 38, f ) <= 512 * 512 + 38 );
			fclose( f );
			std::memcpy( m_pTexNoise->GetBuffer()->ptr(), &img[38], 512 * 512 );
			delete[] img;
		}

		m_pTexNoise->Initialise();
		m_pTexNoise->Bind( NOISE_UNIT );
		m_pTexNoise->GenerateMipmaps();
		m_pTexNoise->Unbind( NOISE_UNIT );
		m_pTexSky->SetType( eTEXTURE_TYPE_2D );
		m_pTexSky->SetImage( Size( m_skyTexSize, m_skyTexSize ), ePIXEL_FORMAT_ARGB16F32F );
		m_pTexSky->Initialise();
		m_pTexSky->Bind( SKY_UNIT );
		m_pTexSky->GenerateMipmaps();
		m_pTexSky->Unbind( SKY_UNIT );
#if ENABLE_FFT
		m_pTexSpectrum_1_2->SetType( eTEXTURE_TYPE_2D );
		m_pTexSpectrum_1_2->SetImage( Size( m_FFT_SIZE, m_FFT_SIZE ), ePIXEL_FORMAT_ARGB32F );
		m_pTexSpectrum_1_2->Initialise();
		m_pTexSpectrum_3_4->SetType( eTEXTURE_TYPE_2D );
		m_pTexSpectrum_3_4->SetImage( Size( m_FFT_SIZE, m_FFT_SIZE ), ePIXEL_FORMAT_ARGB32F );
		m_pTexSpectrum_3_4->Initialise();
		m_pTexSlopeVariance->SetType( eTEXTURE_TYPE_3D );
		m_pTexSlopeVariance->SetImage( Point3ui( m_N_SLOPE_VARIANCE, m_N_SLOPE_VARIANCE, m_N_SLOPE_VARIANCE ), ePIXEL_FORMAT_AL16F32F );
		m_pTexSlopeVariance->Initialise();
		m_pTexFFTA->SetType( eTEXTURE_TYPE_2DARRAY );
		m_pTexFFTA->SetImage( Point3ui( m_FFT_SIZE, m_FFT_SIZE, 5 ), ePIXEL_FORMAT_RGB32F );
		m_pTexFFTA->Initialise();
		m_pTexFFTA->Bind( FFT_A_UNIT );
		m_pTexFFTA->GenerateMipmaps();
		m_pTexFFTA->Unbind( FFT_A_UNIT );
		m_pTexFFTB->SetType( eTEXTURE_TYPE_2DARRAY );
		m_pTexFFTB->SetImage( Point3ui( m_FFT_SIZE, m_FFT_SIZE, 5 ), ePIXEL_FORMAT_RGB32F );
		m_pTexFFTB->Initialise();
		m_pTexFFTB->Bind( FFT_B_UNIT );
		m_pTexFFTB->GenerateMipmaps();
		m_pTexFFTB->Unbind( FFT_B_UNIT );
		m_pTexButterfly->SetType( eTEXTURE_TYPE_2D );
		m_pTexButterfly->SetImage( Size( m_FFT_SIZE, m_PASSES ), ePIXEL_FORMAT_ARGB32F );
		std::memcpy( m_pTexButterfly->GetBuffer()->ptr(), computeButterflyLookupTexture(), m_FFT_SIZE * m_PASSES * 4 * sizeof( float ) );
		m_pTexButterfly->Initialise();
		generateWavesSpectrum();
		m_fftFbo1->Bind( eFRAMEBUFFER_MODE_CONFIG );

		for ( int i = 0; i < 5; ++i )
		{
			m_arrayFftAttaches.push_back( m_fftFbo1->CreateAttachment( m_pTexFFTA ) );
			m_fftFbo1->Attach( eATTACHMENT_POINT_COLOUR, i, m_arrayFftAttaches[i], eTEXTURE_TARGET_LAYER, i );
		}

		m_fftFbo1->SetReadBuffer( eATTACHMENT_POINT_COLOUR, 0 );
		m_fftFbo1->SetDrawBuffers();
		ENSURE( m_fftFbo1->IsComplete() );
		m_fftFbo1->Unbind();
		m_fftFbo2->Bind( eFRAMEBUFFER_MODE_CONFIG );
		m_fftFbo2->Attach( eATTACHMENT_POINT_COLOUR, 0, m_pAttachFftA, eTEXTURE_TARGET_2D );
		m_fftFbo2->Attach( eATTACHMENT_POINT_COLOUR, 1, m_pAttachFftB, eTEXTURE_TARGET_2D );
		ENSURE( m_fftFbo2->IsComplete() );
		m_fftFbo2->SetDrawBuffer( m_pAttachFftA );
		m_fftFbo2->SetReadBuffer( eATTACHMENT_POINT_COLOUR, 0 );
		m_fftFbo2->Unbind();
#else
		m_pTexWave->SetType( eTEXTURE_TYPE_1D );
		m_pTexWave->SetImage( Size( m_nbWaves, 1 ), ePIXEL_FORMAT_ARGB32F );
		m_pTexWave->Initialise( WAVE_UNIT );
		m_pTexWave->SetSampler( m_pSamplerNearestClamp );
#endif
		m_fbo->Bind( eFRAMEBUFFER_MODE_CONFIG );
		//m_fbo->SetDrawBuffer( eATTACHMENT_POINT_COLOUR );
		m_fbo->Attach( eATTACHMENT_POINT_COLOUR, m_pAttachSky, eTEXTURE_TARGET_2D );
		ENSURE( m_fbo->IsComplete() );
		m_fbo->Unbind();
		generateMesh();
		loadPrograms( true );
#if ENABLE_FFT
		m_variancesVtxBuffer->Initialise( eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW );
		m_variancesIdxBuffer->Initialise( eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW );
		m_variancesGBuffers = GetEngine()->GetRenderSystem()->CreateGeometryBuffers( eTOPOLOGY_TRIANGLES, *m_variances, m_variancesVtxBuffer.get(), m_variancesIdxBuffer.get(), nullptr, nullptr );
		m_initVtxBuffer->Initialise( eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW );
		m_initIdxBuffer->Initialise( eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW );
		m_initGBuffers = GetEngine()->GetRenderSystem()->CreateGeometryBuffers( eTOPOLOGY_TRIANGLES, *m_init, m_initVtxBuffer.get(), m_initIdxBuffer.get(), nullptr, nullptr );
		m_fftxVtxBuffer->Initialise( eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW );
		m_fftxIdxBuffer->Initialise( eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW );
		m_fftxGBuffers = GetEngine()->GetRenderSystem()->CreateGeometryBuffers( eTOPOLOGY_TRIANGLES, *m_fftx, m_fftxVtxBuffer.get(), m_fftxIdxBuffer.get(), nullptr, nullptr );
		m_fftyVtxBuffer->Initialise( eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW );
		m_fftyIdxBuffer->Initialise( eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW );
		m_fftyGBuffers = GetEngine()->GetRenderSystem()->CreateGeometryBuffers( eTOPOLOGY_TRIANGLES, *m_ffty, m_fftyVtxBuffer.get(), m_fftyIdxBuffer.get(), nullptr, nullptr );
#endif
		m_skyVtxBuffer->Initialise( eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW );
		m_skymapVtxBuffer->Initialise( eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW );
		m_cloudsVtxBuffer->Initialise( eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW );
		m_skyIdxBuffer->Initialise( eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW );
		m_skymapIdxBuffer->Initialise( eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW );
		m_cloudsIdxBuffer->Initialise( eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW );
		m_skyGBuffers = GetEngine()->GetRenderSystem()->CreateGeometryBuffers( eTOPOLOGY_TRIANGLES, *m_sky, m_skyVtxBuffer.get(), m_skyIdxBuffer.get(), nullptr, nullptr );
		m_skymapGBuffers = GetEngine()->GetRenderSystem()->CreateGeometryBuffers( eTOPOLOGY_TRIANGLES, *m_skymap, m_skymapVtxBuffer.get(), m_skymapIdxBuffer.get(), nullptr, nullptr );
		m_cloudsGBuffers = GetEngine()->GetRenderSystem()->CreateGeometryBuffers( eTOPOLOGY_TRIANGLES, *m_clouds, m_cloudsVtxBuffer.get(), m_cloudsIdxBuffer.get(), nullptr, nullptr );
#if ENABLE_FFT
		m_variancesFbo->Bind( eFRAMEBUFFER_MODE_CONFIG );

		for ( int layer = 0; layer < m_N_SLOPE_VARIANCE; ++layer )
		{
			TextureAttachmentSPtr l_pAttach = m_variancesFbo->CreateAttachment( m_pTexSlopeVariance );
			m_arrayVarianceAttaches.push_back( l_pAttach );
			m_variancesFbo->Attach( eATTACHMENT_POINT_COLOUR, l_pAttach, eTEXTURE_TARGET_3D, layer );
		}

		m_variancesFbo->SetDrawBuffer( m_arrayVarianceAttaches[0] );
		ENSURE( m_variancesFbo->IsComplete() );
		m_variancesFbo->Unbind();
#else
		generateWaves();
#endif
		return l_bReturn;
	}

	void RenderTechnique::DoCleanup()
	{
		m_renderVtxBuffer->Cleanup();
		m_renderIdxBuffer->Cleanup();
		m_renderGBuffers.reset();
		m_skyVtxBuffer->Cleanup();
		m_skyIdxBuffer->Cleanup();
		m_skyGBuffers.reset();
		m_skymapVtxBuffer->Cleanup();
		m_skymapIdxBuffer->Cleanup();
		m_skymapGBuffers.reset();
		m_cloudsVtxBuffer->Cleanup();
		m_cloudsIdxBuffer->Cleanup();
		m_cloudsGBuffers.reset();
#if ENABLE_FFT
		m_variancesVtxBuffer->Cleanup();
		m_variancesIdxBuffer->Cleanup();
		m_variancesGBuffers.reset();
		m_initVtxBuffer->Cleanup();
		m_initIdxBuffer->Cleanup();
		m_initGBuffers.reset();
		m_fftxVtxBuffer->Cleanup();
		m_fftxIdxBuffer->Cleanup();
		m_fftxGBuffers.reset();
		m_fftyVtxBuffer->Cleanup();
		m_fftyIdxBuffer->Cleanup();
		m_fftyGBuffers.reset();
		DoCleanupPrograms( true );
		m_variancesFbo->Bind( eFRAMEBUFFER_MODE_CONFIG );
		m_variancesFbo->Unbind();
		m_fftFbo1->Bind( eFRAMEBUFFER_MODE_CONFIG );
		m_fftFbo1->DetachAll();
		m_fftFbo1->Unbind();
		m_fftFbo2->Bind( eFRAMEBUFFER_MODE_CONFIG );
		m_fftFbo2->DetachAll();
		m_fftFbo2->Unbind();
#endif
		m_fbo->Bind( eFRAMEBUFFER_MODE_CONFIG );
		m_fbo->DetachAll();
		m_fbo->Unbind();
		m_pTexIrradiance->Cleanup();
		m_pTexInscatter->Cleanup();
		m_pTexTransmittance->Cleanup();
		m_pTexSky->Cleanup();
		m_pTexNoise->Cleanup();
#if ENABLE_FFT
		m_pTexSpectrum_1_2->Cleanup();
		m_pTexSpectrum_3_4->Cleanup();
		m_pTexSlopeVariance->Cleanup();
		m_pTexFFTA->Cleanup();
		m_pTexFFTB->Cleanup();
		m_pTexButterfly->Cleanup();
#else
		m_pTexWave->Cleanup();
#endif
		m_pSamplerNearestClamp->Cleanup();
		m_pSamplerNearestRepeat->Cleanup();
		m_pSamplerLinearClamp->Cleanup();
		m_pSamplerLinearRepeat->Cleanup();
		m_pSamplerAnisotropicClamp->Cleanup();
		m_pSamplerAnisotropicRepeat->Cleanup();
		m_frameBuffer->Bind( eFRAMEBUFFER_MODE_CONFIG );
		m_frameBuffer->DetachAll();
		m_frameBuffer->Unbind();
		m_frameBuffer->Cleanup();
		m_pColorBuffer->Cleanup();
		m_pDepthBuffer->Cleanup();
	}

	bool RenderTechnique::DoBeginRender()
	{
		if ( m_bReloadPrograms )
		{
			loadPrograms( m_bLayer == m_cloudLayer );
			m_bReloadPrograms = false;
		}

		if ( m_bGenerateMesh || m_vboParams[0] != m_width || m_vboParams[1] != m_height || m_vboParams[2] != m_gridSize || m_vboParams[3] != m_cameraTheta )
		{
			generateMesh();
			m_bGenerateMesh = false;
		}

#if ENABLE_FFT

		if ( m_bComputeSlopeVarianceTex )
		{
			computeSlopeVarianceTex( NULL );
			m_bComputeSlopeVarianceTex = false;
		}

		if ( m_bGenerateWavesSpectrum )
		{
			generateWavesSpectrum();
			m_bGenerateWavesSpectrum = false;
		}

#else

		if ( m_bGenerateWaves )
		{
			generateWaves();
			m_bGenerateWaves = false;
		}

#endif
		return true;
	}

	void RenderTechnique::DoRender( RenderTechnique::stSCENE_RENDER_NODES & CU_PARAM_UNUSED( p_nodes ), Camera & CU_PARAM_UNUSED( p_camera ), double CU_PARAM_UNUSED( p_dFrameTime ) )
	{
		Pipeline & l_pPipeline = GetEngine()->GetRenderSystem()->GetPipeline();
		Point3f sun( sin( m_sunTheta ) * cos( m_sunPhi ), sin( m_sunTheta ) * sin( m_sunPhi ), cos( m_sunTheta ) );
		m_fbo->Bind();
		m_pDepthStencilState.lock()->Apply();
		m_pRasteriserState.lock()->Apply();
		l_pPipeline.ApplyViewport( m_skyTexSize, m_skyTexSize );
		m_skymapSunDir->SetValue( sun );
		m_skymapOctaves->SetValue( m_octaves );
		m_skymapLacunarity->SetValue( m_lacunarity );
		m_skymapGain->SetValue( m_gain );
		m_skymapNorm->SetValue( m_norm );
		m_skymapClamp1->SetValue( m_clamp1 );
		m_skymapClamp2->SetValue( m_clamp2 );
		m_skymapCloudsColor->SetValue( bgra_float( m_cloudColor ) );
		m_skymapBlendState.lock()->Apply();
		m_skymap->Bind();
		m_skymapGBuffers->Draw( m_skymapIdxBuffer->GetSize(), 0 );
		m_skymap->Unbind();
		m_fbo->Unbind();
		m_pTexSky->Bind( SKY_UNIT );
		m_pSamplerAnisotropicClamp->Bind( SKY_UNIT );
		m_pTexSky->GenerateMipmaps();
		m_pTexSky->Unbind( SKY_UNIT );
		Image l_image( cuT( "Skymap" ), *m_pAttachSky->DownloadBuffer() );
		Image::BinaryLoader()( const_cast< const Image & >( l_image ), cuT( "Skymap.bmp" ) );
	}

	void RenderTechnique::DoEndRender()
	{
		Pipeline & l_pPipeline = GetEngine()->GetRenderSystem()->GetPipeline();
		Point3f sun( sin( m_sunTheta ) * cos( m_sunPhi ), sin( m_sunTheta ) * sin( m_sunPhi ), cos( m_sunTheta ) );
#if ENABLE_FFT
		static double m_lastTime = 0.0;
		double t = m_animate ? time() : m_lastTime;
		simulateFFTWaves( float( t ) );
		m_lastTime = t;
#endif
		m_frameBuffer->Bind( eFRAMEBUFFER_MODE_AUTOMATIC, eFRAMEBUFFER_TARGET_DRAW );
		m_renderTarget->GetDepthStencilState()->Apply();
		m_renderTarget->GetRasteriserState()->Apply();
		l_pPipeline.ApplyViewport( m_width, m_height );
#if ENABLE_FFT
		float ch = m_cameraHeight;
#else
		float ch = m_cameraHeight - m_meanHeight;
#endif
		real fView[16] =
		{
			+0.0, -1.0, 0.0, 0.0
			, +0.0, +0.0, 1.0, -ch
			, -1.0, +0.0, 0.0, 0.0
			, +0.0, +0.0, 0.0, 1.0
		};
		Matrix4x4r view( fView );
		Matrix4x4r proj;
		Matrix4x4r projview;
		Matrix4x4r invProj;
		Matrix4x4r invView;
		Matrix4x4r yaw, roll;
		matrix::perspective( proj, Angle::from_degrees( 90.0 ), double( m_width ) / double( m_height ), 0.1 * ch, 1000000.0 * ch );
		matrix::roll( roll, Angle::from_radians( m_cameraTheta ) );
#if ENABLE_FFT
		matrix::yaw( yaw, Angle::from_degrees( m_cameraPhi ) );
		view = yaw * view;
#endif
		view = roll * view;
		invView = view.get_inverse();
		invProj = proj.get_inverse();
		projview = proj * view;
		m_skyScreenToCamera->SetValue( Matrix4x4r{ invProj.const_ptr() } );
		m_skyCameraToWorld->SetValue( Matrix4x4r{ invView.const_ptr() } );
		m_skyWorldCamera->SetValue( Point3f( 0.0f, 0.0f, ch ) );
		m_skyWorldSunDir->SetValue( sun );
		m_skyHdrExposure->SetValue( m_hdrExposure );
		m_skyBlendState.lock()->Apply();
		m_sky->Bind();
		m_skyGBuffers->Draw( m_skymapIdxBuffer->GetSize(), 0 );
		m_sky->Unbind();

		if ( m_cloudLayer && ch < 3000.0 )
		{
			drawClouds( sun, Matrix4x4r{ projview.const_ptr() } );
		}

		m_renderScreenToCamera->SetValue( Matrix4x4r{ invProj.const_ptr() } );
		m_renderCameraToWorld->SetValue( Matrix4x4r{ invView.const_ptr() } );
		m_renderWorldToScreen->SetValue( Matrix4x4r{ projview.const_ptr() } );
		m_renderWorldCamera->SetValue( Point3f( 0.0f, 0.0f, ch ) );
		m_renderWorldSunDir->SetValue( sun );
		m_renderHdrExposure->SetValue( m_hdrExposure );
		m_renderSeaColor->SetValue( Point3f( m_seaColor.red() * m_seaColor.alpha(), m_seaColor.green() * m_seaColor.alpha(), m_seaColor.blue() * m_seaColor.alpha() ) );
#if ENABLE_FFT
		m_renderSpectrum_1_2_Sampler->SetValue( SPECTRUM_1_2_UNIT );
		m_renderSpectrum_3_4_Sampler->SetValue( SPECTRUM_3_4_UNIT );
		m_renderFftWavesSampler->SetValue( FFT_A_UNIT );
		m_renderSlopeVarianceSampler->SetValue( SLOPE_VARIANCE_UNIT );
		m_renderGridSizes->SetValue( Point4f( m_GRID1_SIZE, m_GRID2_SIZE, m_GRID3_SIZE, m_GRID4_SIZE ) );
		m_renderGridSize->SetValue( Point2f( m_gridSize / float( m_width ), m_gridSize / float( m_height ) ) );
		m_renderChoppy->SetValue( m_choppy );
#else
		float worldToWind[4];
		worldToWind[0] = cos( m_waveDirection );
		worldToWind[1] = sin( m_waveDirection );
		worldToWind[2] = -sin( m_waveDirection );
		worldToWind[3] = cos( m_waveDirection );
		float windToWorld[4];
		windToWorld[0] = cos( m_waveDirection );
		windToWorld[1] = -sin( m_waveDirection );
		windToWorld[2] = sin( m_waveDirection );
		windToWorld[3] = cos( m_waveDirection );
		m_renderWorldToWind->SetValue( worldToWind );
		m_renderWindToWorld->SetValue( windToWorld );
		m_renderNbWaves->SetValue( float( m_nbWaves ) );
		m_renderHeightOffset->SetValue( -m_meanHeight );
		m_renderSigmaSqTotal->SetValue( Point2f( m_sigmaXsq, m_sigmaYsq ) );
		m_renderLods->SetValue( Point4f( m_gridSize,
										 float( atan( 2.0 / m_height ) * m_gridSize ), // angle under which a screen pixel is viewed from the camera * gridSize
										 float( log( m_lambdaMin ) / log( 2.0f ) ),
										 float( ( m_nbWaves - 1.0f ) / ( log( m_lambdaMax ) / log( 2.0f ) - log( m_lambdaMin ) / log( 2.0f ) ) ) ) );
		m_renderNyquistMin->SetValue( m_nyquistMin );
		m_renderNyquistMax->SetValue( m_nyquistMax );

		if ( m_animate )
		{
			m_renderTime->SetValue( float( time() ) );
		}

#endif

		if ( m_grid )
		{
			m_renderRasteriserState.lock()->SetFillMode( eFILL_MODE_LINE );
		}
		else
		{
			m_renderRasteriserState.lock()->SetFillMode( eFILL_MODE_SOLID );
		}

		m_renderRasteriserState.lock()->Apply();
		m_renderBlendState.lock()->Apply();
		m_render->Bind();
		m_renderGBuffers->Draw( m_renderIdxBuffer->GetSize(), 0 );
		m_render->Unbind();
		m_renderTarget->GetRasteriserState()->Apply();

		if ( m_cloudLayer && ch > 3000.0 )
		{
			drawClouds( sun, Matrix4x4r{ projview.const_ptr() } );
		}

		m_frameBuffer->Unbind();
	}

	void RenderTechnique::DoCleanupPrograms( bool all )
	{
		if ( m_render )
		{
			m_renderSkyIrradianceSampler.reset();
			m_renderInscatterSampler.reset();
			m_renderTransmittanceSampler.reset();
			m_renderSkySampler.reset();
			m_renderScreenToCamera.reset();
			m_renderCameraToWorld.reset();
			m_renderWorldToScreen.reset();
			m_renderWorldCamera.reset();
			m_renderWorldSunDir.reset();
			m_renderHdrExposure.reset();
			m_renderSeaColor.reset();
#if ENABLE_FFT
			m_renderSpectrum_1_2_Sampler.reset();
			m_renderSpectrum_3_4_Sampler.reset();
			m_renderFftWavesSampler.reset();
			m_renderSlopeVarianceSampler.reset();
			m_renderGridSizes.reset();
			m_renderGridSize.reset();
			m_renderChoppy.reset();
#else
			m_renderWorldToWind.reset();
			m_renderWindToWorld.reset();
			m_renderNbWaves.reset();
			m_renderHeightOffset.reset();
			m_renderSigmaSqTotal.reset();
			m_renderTime.reset();
			m_renderLods.reset();
			m_renderNyquistMin.reset();
			m_renderNyquistMax.reset();
#endif
			m_render->Cleanup();
		}

		if ( !all )
		{
			return;
		}

		if ( m_sky )
		{
			m_skySkyIrradianceSampler.reset();
			m_skyInscatterSampler.reset();
			m_skyTransmittanceSampler.reset();
			m_skySkySampler.reset();
			m_skyScreenToCamera.reset();
			m_skyCameraToWorld.reset();
			m_skyWorldCamera.reset();
			m_skyWorldSunDir.reset();
			m_skyHdrExposure.reset();
			m_sky->Cleanup();
		}

		if ( m_skymap )
		{
			m_skymapSkyIrradianceSampler.reset();
			m_skymapInscatterSampler.reset();
			m_skymapTransmittanceSampler.reset();
			m_skymapNoiseSampler.reset();
			m_skymapSunDir.reset();
			m_skymapOctaves.reset();
			m_skymapLacunarity.reset();
			m_skymapGain.reset();
			m_skymapNorm.reset();
			m_skymapClamp1.reset();
			m_skymapClamp2.reset();
			m_skymapCloudsColor.reset();
			m_skymap->Cleanup();
		}

		if ( m_clouds )
		{
			m_cloudsSkyIrradianceSampler.reset();
			m_cloudsInscatterSampler.reset();
			m_cloudsTransmittanceSampler.reset();
			m_cloudsNoiseSampler.reset();
			m_cloudsWorldToScreen.reset();
			m_cloudsWorldCamera.reset();
			m_cloudsWorldSunDir.reset();
			m_cloudsHdrExposure.reset();
			m_cloudsOctaves.reset();
			m_cloudsLacunarity.reset();
			m_cloudsGain.reset();
			m_cloudsNorm.reset();
			m_cloudsClamp1.reset();
			m_cloudsClamp2.reset();
			m_cloudsCloudsColor.reset();
			m_clouds->Cleanup();
		}

#if ENABLE_FFT

		if ( m_init )
		{
			m_initSpectrum_1_2_Sampler.reset();
			m_initSpectrum_3_4_Sampler.reset();
			m_initFftSize.reset();
			m_initInverseGridSizes.reset();
			m_initT.reset();
			m_init->Cleanup();
		}

		if ( m_variances )
		{
			m_variancesSpectrum_1_2_Sampler.reset();
			m_variancesSpectrum_3_4_Sampler.reset();
			m_variancesNSlopeVariance.reset();
			m_variancesFFTSize.reset();
			m_variancesGridSizes.reset();
			m_variancesSlopeVarianceDelta.reset();
			m_variancesC.reset();
			m_variances->Cleanup();
		}

		if ( m_fftx )
		{
			m_fftxButterflySampler.reset();
			m_fftxNLayers.reset();
			m_fftxPass.reset();
			m_fftxImgSampler.reset();
			m_fftx->Cleanup();
		}

		if ( m_ffty )
		{
			m_fftyButterflySampler.reset();
			m_fftyNLayers.reset();
			m_fftyPass.reset();
			m_fftyImgSampler.reset();
			m_ffty->Cleanup();
		}

#endif
	}

	void RenderTechnique::DoDestroyPrograms( bool CU_PARAM_UNUSED( all ) )
	{
	}

	void RenderTechnique::DoDeletePrograms( bool all )
	{
		m_render.reset();

		if ( !all )
		{
			return;
		}

		m_sky.reset();
		m_skymap.reset();
		m_clouds.reset();
#if ENABLE_FFT
		m_init.reset();
		m_variances.reset();
		m_fftx.reset();
		m_ffty.reset();
#endif
	}

#if ENABLE_FFT
	float RenderTechnique::sqr( float x )
	{
		return x * x;
	}

	double RenderTechnique::sqr( double x )
	{
		return x * x;
	}

	float RenderTechnique::omega( float k )
	{
		return float( sqrt( 9.81 * k * ( 1.0 + sqr( k / m_km ) ) ) ); // Eq 24
	}

	float RenderTechnique::spectrum( float kx, float ky, bool omnispectrum )
	{
		float U10 = m_WIND;
		float Omega = m_OMEGA;
		// phase speed
		float k = sqrt( kx * kx + ky * ky );
		float c = omega( k ) / k;
		// spectral peak
		float kp = float( 9.81 * sqr( Omega / U10 ) ); // after Eq 3
		float cp = omega( kp ) / kp;
		// friction velocity
		float z0 = float( 3.7e-5 * sqr( U10 ) / 9.81 * pow( U10 / cp, 0.9f ) ); // Eq 66
		float u_star = float( 0.41 * U10 / log( 10.0 / z0 ) ); // Eq 60
		float Lpm = float( std::exp( -5.0 / 4.0 * sqr( kp / k ) ) ); // after Eq 3
		float gamma = float( Omega < 1.0 ? 1.7 : 1.7 + 6.0 * log( Omega ) ); // after Eq 3 // log10 or log??
		float sigma = float( 0.08 * ( 1.0 + 4.0 / pow( Omega, 3.0f ) ) ); // after Eq 3
		float Gamma = float( std::exp( -1.0 / ( 2.0 * sqr( sigma ) ) * sqr( sqrt( k / kp ) - 1.0 ) ) );
		float Jp = pow( gamma, Gamma ); // Eq 3
		float Fp = float( Lpm * Jp * std::exp( -Omega / sqrt( 10.0 ) * ( sqrt( k / kp ) - 1.0 ) ) ); // Eq 32
		float alphap = float( 0.006 * sqrt( Omega ) ); // Eq 34
		float Bl = float( 0.5 * alphap * cp / c * Fp ); // Eq 31
		float alpham = float( 0.01 * ( u_star < m_cm ? 1.0 + log( u_star / m_cm ) : 1.0 + 3.0 * log( u_star / m_cm ) ) ); // Eq 44
		float Fm = float( std::exp( -0.25 * sqr( k / m_km - 1.0 ) ) ); // Eq 41
		float Bh = float( 0.5 * alpham * m_cm / c * Fm * Lpm ); // Eq 40 (fixed)

		if ( omnispectrum )
		{
			return m_A * ( Bl + Bh ) / ( k * sqr( k ) ); // Eq 30
		}

		float a0 = float( log( 2.0 ) / 4.0 );
		float ap = float( 4.0 );
		float am = float( 0.13 * u_star / m_cm ); // Eq 59
		float Delta = float( tanh( a0 + ap * pow( c / cp, 2.5f ) + am * pow( m_cm / c, 2.5f ) ) ); // Eq 57
		float phi = atan2( ky, kx );

		if ( kx < 0.0 )
		{
			return 0.0;
		}
		else
		{
			Bl *= 2.0;
			Bh *= 2.0;
		}

		return float( m_A * ( Bl + Bh ) * ( 1.0 + Delta * cos( 2.0 * phi ) ) / ( 2.0 * M_PI * sqr( sqr( k ) ) ) ); // Eq 67
	}

	void RenderTechnique::drawQuadVariances()
	{
		m_variancesGBuffers->Draw( m_variancesIdxBuffer->GetSize(), 0 );
	}

	void RenderTechnique::drawQuadInit()
	{
		m_initGBuffers->Draw( m_initIdxBuffer->GetSize(), 0 );
	}

	void RenderTechnique::drawQuadFFTx()
	{
		m_fftxGBuffers->Draw( m_fftxIdxBuffer->GetSize(), 0 );
	}

	void RenderTechnique::drawQuadFFTy()
	{
		m_fftyGBuffers->Draw( m_fftyIdxBuffer->GetSize(), 0 );
	}
#endif

	// ----------------------------------------------------------------------------
	// CLOUDS
	// ----------------------------------------------------------------------------

	void RenderTechnique::drawClouds( const Point3f & sun, const Matrix4x4f & mat )
	{
		m_cloudsWorldToScreen->SetValue( mat );
		m_cloudsWorldCamera->SetValue( Point3f( 0.0f, 0.0f, m_cameraHeight ) );
		m_cloudsWorldSunDir->SetValue( sun );
		m_cloudsHdrExposure->SetValue( m_hdrExposure );
		m_cloudsOctaves->SetValue( m_octaves );
		m_cloudsLacunarity->SetValue( m_lacunarity );
		m_cloudsGain->SetValue( m_gain );
		m_cloudsNorm->SetValue( m_norm );
		m_cloudsClamp1->SetValue( m_clamp1 );
		m_cloudsClamp2->SetValue( m_clamp2 );
		m_cloudsCloudsColor->SetValue( bgra_float( m_cloudColor ) );
		m_cloudsBlendState.lock()->Apply();
		m_clouds->Bind();
		m_cloudsGBuffers->Draw( m_cloudsIdxBuffer->GetSize(), 0 );
		m_clouds->Unbind();
	}

	// ----------------------------------------------------------------------------
	// WAVES SPECTRUM GENERATION
	// ----------------------------------------------------------------------------

	long RenderTechnique::lrandom( long * seed )
	{
		*seed = ( *seed * 1103515245 + 12345 ) & 0x7FFFFFFF;
		return *seed;
	}

	float RenderTechnique::frandom( long * seed )
	{
		long r = lrandom( seed ) >> ( 31 - 24 );
		return r / ( float )( 1 << 24 );
	}

	inline float RenderTechnique::grandom( float mean, float stdDeviation, long * seed )
	{
		float y1;
		static float y2;
		static int use_last = 0;

		if ( use_last )
		{
			y1 = y2;
			use_last = 0;
		}
		else
		{
			float x1, x2, w;

			do
			{
				x1 = 2.0f * frandom( seed ) - 1.0f;
				x2 = 2.0f * frandom( seed ) - 1.0f;
				w = x1 * x1 + x2 * x2;
			}
			while ( w >= 1.0f );

			w = sqrt( ( -2.0f * log( w ) ) / w );
			y1 = x1 * w;
			y2 = x2 * w;
			use_last = 1;
		}

		return mean + y1 * stdDeviation;
	}

#if ENABLE_FFT
	void RenderTechnique::getSpectrumSample( int i, int j, float lengthScale, float kMin, float * result )
	{
		static long seed = 1234;
		float dk = float( 2.0 * M_PI / lengthScale );
		float kx = i * dk;
		float ky = j * dk;

		if ( abs( kx ) < kMin && abs( ky ) < kMin )
		{
			result[0] = 0.0;
			result[1] = 0.0;
		}
		else
		{
			float S = spectrum( kx, ky );
			float h = float( sqrt( S / 2.0 ) * dk );
			float phi = float( frandom( &seed ) * 2.0 * M_PI );
			result[0] = h * cos( phi );
			result[1] = h * sin( phi );
		}
	}

	// generates the waves spectrum
	void RenderTechnique::generateWavesSpectrum()
	{
		if ( m_spectrum12 != NULL )
		{
			delete[] m_spectrum12;
			delete[] m_spectrum34;
		}

		m_spectrum12 = new float[m_FFT_SIZE * m_FFT_SIZE * 4];
		m_spectrum34 = new float[m_FFT_SIZE * m_FFT_SIZE * 4];

		for ( int y = 0; y < m_FFT_SIZE; ++y )
		{
			for ( int x = 0; x < m_FFT_SIZE; ++x )
			{
				int offset = 4 * ( x + y * m_FFT_SIZE );
				int i = x >= m_FFT_SIZE / 2 ? x - m_FFT_SIZE : x;
				int j = y >= m_FFT_SIZE / 2 ? y - m_FFT_SIZE : y;
				getSpectrumSample( i, j, m_GRID1_SIZE, float( M_PI / m_GRID1_SIZE ), m_spectrum12 + offset );
				getSpectrumSample( i, j, m_GRID2_SIZE, float( M_PI * m_FFT_SIZE / m_GRID1_SIZE ), m_spectrum12 + offset + 2 );
				getSpectrumSample( i, j, m_GRID3_SIZE, float( M_PI * m_FFT_SIZE / m_GRID2_SIZE ), m_spectrum34 + offset );
				getSpectrumSample( i, j, m_GRID4_SIZE, float( M_PI * m_FFT_SIZE / m_GRID3_SIZE ), m_spectrum34 + offset + 2 );
			}
		}

		m_pTexSpectrum_1_2->Bind( SPECTRUM_1_2_UNIT );
		m_pSamplerNearestRepeat->Bind( SPECTRUM_1_2_UNIT );
		uint8_t * l_pData = m_pTexSpectrum_1_2->Lock( eACCESS_TYPE_WRITE );
		std::memcpy( l_pData, m_spectrum12, sizeof( float ) * m_FFT_SIZE * m_FFT_SIZE * 4 );
		m_pTexSpectrum_1_2->Unlock( true );
		m_pTexSpectrum_1_2->Unbind( SPECTRUM_1_2_UNIT );
		m_pTexSpectrum_3_4->Bind( SPECTRUM_3_4_UNIT );
		m_pSamplerNearestRepeat->Bind( SPECTRUM_3_4_UNIT );
		l_pData = m_pTexSpectrum_3_4->Lock( eACCESS_TYPE_WRITE );
		std::memcpy( l_pData, m_spectrum34, sizeof( float ) * m_FFT_SIZE * m_FFT_SIZE * 4 );
		m_pTexSpectrum_3_4->Unlock( true );
		m_pTexSpectrum_3_4->Unbind( SPECTRUM_3_4_UNIT );
		//TwDefine("Parameters color='255 0 0'");
	}

	float RenderTechnique::getSlopeVariance( float kx, float ky, float * spectrumSample )
	{
		float kSquare = kx * kx + ky * ky;
		float real = spectrumSample[0];
		float img = spectrumSample[1];
		float hSquare = real * real + img * img;
		return float( kSquare * hSquare * 2.0 );
	}

	void RenderTechnique::computeSlopeVarianceTex( void * CU_PARAM_UNUSED( unused ) )
	{
		// slope variance due to all waves, by integrating over the full spectrum
		float theoreticSlopeVariance = 0.0;
		float k = float( 5e-3 );

		while ( k < 1e3 )
		{
			float nextK = float( k * 1.001 );
			theoreticSlopeVariance += k * k * spectrum( k, 0, true ) * ( nextK - k );
			k = nextK;
		}

		// slope variance due to waves, by integrating over the spectrum part
		// that is covered by the four nested grids. This can give a smaller result
		// than the theoretic total slope variance, because the higher frequencies
		// may not be covered by the four nested m_grid. Hence the difference between
		// the two is added as a "delta" slope variance in the "m_variances" shader,
		// to be sure not to lose the variance due to missing wave frequencies in
		// the four nested grids
		float totalSlopeVariance = 0.0;

		for ( int y = 0; y < m_FFT_SIZE; ++y )
		{
			for ( int x = 0; x < m_FFT_SIZE; ++x )
			{
				int offset = 4 * ( x + y * m_FFT_SIZE );
				float i = float( 2.0 * M_PI * ( x >= m_FFT_SIZE / 2 ? x - m_FFT_SIZE : x ) );
				float j = float( 2.0 * M_PI * ( y >= m_FFT_SIZE / 2 ? y - m_FFT_SIZE : y ) );
				totalSlopeVariance += getSlopeVariance( i / m_GRID1_SIZE, j / m_GRID1_SIZE, m_spectrum12 + offset );
				totalSlopeVariance += getSlopeVariance( i / m_GRID2_SIZE, j / m_GRID2_SIZE, m_spectrum12 + offset + 2 );
				totalSlopeVariance += getSlopeVariance( i / m_GRID3_SIZE, j / m_GRID3_SIZE, m_spectrum34 + offset );
				totalSlopeVariance += getSlopeVariance( i / m_GRID4_SIZE, j / m_GRID4_SIZE, m_spectrum34 + offset + 2 );
			}
		}

		m_variancesFbo->Bind( eFRAMEBUFFER_MODE_AUTOMATIC );
		GetEngine()->GetRenderSystem()->GetPipeline().ApplyViewport( m_N_SLOPE_VARIANCE, m_N_SLOPE_VARIANCE );
		m_variancesGridSizes->SetValue( Point4f( m_GRID1_SIZE, m_GRID2_SIZE, m_GRID3_SIZE, m_GRID4_SIZE ) );
		m_variancesSlopeVarianceDelta->SetValue( float( 0.5 * ( theoreticSlopeVariance - totalSlopeVariance ) ) );
		m_variancesBlendState.lock()->Apply();

		for ( int layer = 0; layer < m_N_SLOPE_VARIANCE; ++layer )
		{
			m_variancesC->SetValue( float( layer ) );
			m_variances->Bind();
			drawQuadVariances();
			m_variances->Unbind();
		}

		//    TwDefine("Parameters color='17 109 143'");
		m_variancesFbo->Unbind();
	}

	// ----------------------------------------------------------------------------
	// WAVES GENERATION AND ANIMATION (using FFT on GPU)
	// ----------------------------------------------------------------------------

	int RenderTechnique::bitReverse( int i, int N )
	{
		int j = i;
		int M = N;
		int Sum = 0;
		int W = 1;
		M = M / 2;

		while ( M != 0 )
		{
			j = ( i & M ) > M - 1;
			Sum += j * W;
			W *= 2;
			M = M / 2;
		}

		return Sum;
	}

	void RenderTechnique::computeWeight( int N, int k, float & Wr, float & Wi )
	{
		Wr = float( cosl( 2.0 * M_PI * k / float( N ) ) );
		Wi = float( sinl( 2.0 * M_PI * k / float( N ) ) );
	}

	float * RenderTechnique::computeButterflyLookupTexture()
	{
		float * data = new float[m_FFT_SIZE * m_PASSES * 4];

		for ( int i = 0; i < m_PASSES; i++ )
		{
			int nBlocks = ( int )powf( 2.0, float( m_PASSES - 1 - i ) );
			int nHInputs = ( int )powf( 2.0, float( i ) );

			for ( int j = 0; j < nBlocks; j++ )
			{
				for ( int k = 0; k < nHInputs; k++ )
				{
					int i1, i2, j1, j2;

					if ( i == 0 )
					{
						i1 = j * nHInputs * 2 + k;
						i2 = j * nHInputs * 2 + nHInputs + k;
						j1 = bitReverse( i1, m_FFT_SIZE );
						j2 = bitReverse( i2, m_FFT_SIZE );
					}
					else
					{
						i1 = j * nHInputs * 2 + k;
						i2 = j * nHInputs * 2 + nHInputs + k;
						j1 = i1;
						j2 = i2;
					}

					float wr, wi;
					computeWeight( m_FFT_SIZE, k * nBlocks, wr, wi );
					int offset1 = 4 * ( i1 + i * m_FFT_SIZE );
					data[offset1 + 0] = float( ( j1 + 0.5 ) / m_FFT_SIZE );
					data[offset1 + 1] = float( ( j2 + 0.5 ) / m_FFT_SIZE );
					data[offset1 + 2] = wr;
					data[offset1 + 3] = wi;
					int offset2 = 4 * ( i2 + i * m_FFT_SIZE );
					data[offset2 + 0] = float( ( j1 + 0.5 ) / m_FFT_SIZE );
					data[offset2 + 1] = float( ( j2 + 0.5 ) / m_FFT_SIZE );
					data[offset2 + 2] = -wr;
					data[offset2 + 3] = -wi;
				}
			}
		}

		return data;
	}

	void RenderTechnique::simulateFFTWaves( float t )
	{
		RenderSystem * l_pRS = GetEngine()->GetRenderSystem();
		Pipeline & l_pPipeline = l_pRS->GetPipeline();
		TextureSPtr l_pTex;
		// init
		m_fftFbo1->Bind( eFRAMEBUFFER_MODE_AUTOMATIC );
		m_fftFbo1->SetReadBuffer( eATTACHMENT_POINT_COLOUR, 0 );
		l_pPipeline.ApplyViewport( m_FFT_SIZE, m_FFT_SIZE );
		m_initFftSize->SetValue( float( m_FFT_SIZE ) );
		m_initInverseGridSizes->SetValue( Point4f( 2.0 * M_PI * m_FFT_SIZE / m_GRID1_SIZE, 2.0 * M_PI * m_FFT_SIZE / m_GRID2_SIZE, 2.0 * M_PI * m_FFT_SIZE / m_GRID3_SIZE, 2.0 * M_PI * m_FFT_SIZE / m_GRID4_SIZE ) );
		m_initT->SetValue( t );
		m_initBlendState.lock()->Apply();
		m_init->Bind();
		drawQuadInit();
		m_init->Unbind();
		m_fftFbo1->Unbind();
		// fft passes
		m_fftFbo2->Bind( eFRAMEBUFFER_MODE_MANUAL );
		m_fftxNLayers->SetValue( m_choppy ? 5 : 3 );
		m_fftxBlendState.lock()->Apply();

		for ( int i = 0; i < m_PASSES; ++i )
		{
			m_fftxPass->SetValue( float( i + 0.5 ) / m_PASSES );

			if ( i % 2 == 0 )
			{
				m_fftxImgSampler->SetValue( FFT_A_UNIT );
				m_fftx->Bind();
				m_fftFbo2->SetDrawBuffer( m_pAttachFftB );
			}
			else
			{
				m_fftxImgSampler->SetValue( FFT_B_UNIT );
				m_fftx->Bind();
				m_fftFbo2->SetDrawBuffer( m_pAttachFftA );
			}

			drawQuadFFTx();
			m_fftx->Unbind();
		}

		m_fftyNLayers->SetValue( m_choppy ? 5 : 3 );
		m_fftyBlendState.lock()->Apply();

		for ( int i = m_PASSES; i < 2 * m_PASSES; ++i )
		{
			m_fftyPass->SetValue( float( i - m_PASSES + 0.5 ) / m_PASSES );

			if ( i % 2 == 0 )
			{
				m_fftyImgSampler->SetValue( FFT_A_UNIT );
				m_ffty->Bind();
				m_fftFbo2->SetDrawBuffer( m_pAttachFftB );
			}
			else
			{
				m_fftyImgSampler->SetValue( FFT_B_UNIT );
				m_ffty->Bind();
				m_fftFbo2->SetDrawBuffer( m_pAttachFftA );
			}

			drawQuadFFTy();
			m_ffty->Unbind();
		}

		m_fftFbo2->Unbind();
		m_pTexFFTA->Bind( FFT_A_UNIT );
		m_pSamplerLinearRepeat->Bind( FFT_A_UNIT );
		m_pTexFFTA->GenerateMipmaps();
		m_pTexFFTA->Unbind( FFT_A_UNIT );
	}
#else
#	define srnd() (2*frandom(&seed) - 1)

	void RenderTechnique::generateWaves()
	{
		long seed = 1234567;
		float min = log( m_lambdaMin ) / log( 2.0f );
		float max = log( m_lambdaMax ) / log( 2.0f );
		m_sigmaXsq = 0.0;
		m_sigmaYsq = 0.0;
		m_meanHeight = 0.0;
		m_heightVariance = 0.0;
		m_amplitudeMax = 0.0;

		if ( m_waves != NULL )
		{
			delete[] m_waves;
			delete[] m_pWaves;
		}

		m_pWaves = new float[4 * m_nbWaves];
		m_waves = new Coords4f[m_nbWaves];

		for ( int i = 0; i < m_nbWaves; i++ )
		{
			m_waves[i] = Coords4f( &m_pWaves[i * 4] );
		}

#define nbAngles 5 // even
#define angle(i) (1.5*(((i)%nbAngles)/(float)(nbAngles/2)-1))
#define dangle() (1.5/(float)(nbAngles/2))
		float Wa[nbAngles]; // normalised gaussian samples
		int index[nbAngles]; // to hash angle order
		float s = 0;

		for ( int i = 0; i < nbAngles; i++ )
		{
			index[i] = i;
			float a = float( angle( i ) );
			s += Wa[i] = float( std::exp( -0.5 * a * a ) );
		}

		for ( int i = 0; i < nbAngles; i++ )
		{
			Wa[i] /= s;
		}

		for ( int i = 0; i < m_nbWaves; ++i )
		{
			float x = i / ( m_nbWaves - 1.0f );
			float lambda = pow( 2.0f, ( 1.0f - x ) * min + x * max );
			float ktheta = grandom( 0.0f, 1.0f, &seed ) * m_waveDispersion;
			float knorm = float( 2.0f * M_PI / lambda );
			float omega = sqrt( 9.81f * knorm );
			float amplitude;
			float step = ( max - min ) / ( m_nbWaves - 1 ); // dlambda/di
			float omega0 = float( 9.81 / m_U0 );

			if ( ( i % ( nbAngles ) ) == 0 )
			{
				// scramble angle ordre
				for ( int k = 0; k < nbAngles; k++ )   // do N swap in indices
				{
					int n1 = lrandom( &seed ) % nbAngles, n2 = lrandom( &seed ) % nbAngles, n;
					n = index[n1];
					index[n1] = index[n2];
					index[n2] = n;
				}
			}

			ktheta = float( m_waveDispersion * ( angle( index[( i ) % nbAngles] ) + 0.4 * srnd() * dangle() ) );
			ktheta *= float( 1.0 / ( 1.0 + 40.0 * pow( omega0 / omega, 4 ) ) );
			amplitude = float( ( 8.1e-3 * 9.81 * 9.81 ) / pow( omega, 5 ) * std::exp( -0.74 * pow( omega0 / omega, 4 ) ) );
			amplitude *= float( 0.5 * sqrt( 2 * M_PI * 9.81 / lambda ) * nbAngles * step );
			amplitude = 3 * m_heightMax * sqrt( amplitude );

			if ( amplitude > 1.0f / knorm )
			{
				amplitude = 1.0f / knorm;
			}
			else if ( amplitude < -1.0f / knorm )
			{
				amplitude = -1.0f / knorm;
			}

			m_waves[i][0] = amplitude;
			m_waves[i][1] = omega;
			m_waves[i][2] = knorm * cos( ktheta );
			m_waves[i][3] = knorm * sin( ktheta );
			m_sigmaXsq += float( pow( cos( ktheta ), 2.0f ) * ( 1.0 - sqrt( 1.0 - knorm * knorm * amplitude * amplitude ) ) );
			m_sigmaYsq += float( pow( sin( ktheta ), 2.0f ) * ( 1.0 - sqrt( 1.0 - knorm * knorm * amplitude * amplitude ) ) );
			m_meanHeight -= knorm * amplitude * amplitude * 0.5f;
			m_heightVariance += amplitude * amplitude * ( 2.0f - knorm * knorm * amplitude * amplitude ) * 0.25f;
			m_amplitudeMax += fabs( amplitude );
		}

		float var = 4.0f;
		m_amplitudeMax = 2.0f * var * sqrt( m_heightVariance );
		m_pTexWave->Bind();
		uint8_t * l_pData = m_pTexWave->Lock( eACCESS_TYPE_WRITE );
		std::memcpy( l_pData, m_pWaves, m_nbWaves * 4 * sizeof( float ) );
		m_pTexWave->Unlock( true );
		m_pTexWave->Unbind();
	}
#endif
}

//*************************************************************************************************
