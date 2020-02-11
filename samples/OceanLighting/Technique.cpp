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

#include <Cache/SamplerCache.hpp>
#include <FrameBuffer/DepthStencilRenderBuffer.hpp>
#include <FrameBuffer/TextureAttachment.hpp>
#include <FrameBuffer/RenderBufferAttachment.hpp>
#include <Render/RenderSystem.hpp>
#include <Shader/FrameVariableBuffer.hpp>
#include <Shader/OneFrameVariable.hpp>
#include <Shader/PointFrameVariable.hpp>
#include <Shader/MatrixFrameVariable.hpp>
#include <State/BlendState.hpp>
#include <State/DepthStencilState.hpp>
#include <State/RasteriserState.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureImage.hpp>
#include <Texture/TextureLayout.hpp>

#include <Exception/Assertion.hpp>
#include <Graphics/Image.hpp>
#include <Math/TransformationMatrix.hpp>

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
	RenderTechnique::RenderTechnique( RenderTarget & p_renderTarget, RenderSystem & p_renderSystem, Parameters const & p_params )
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
		, m_viewport( *p_renderSystem.GetEngine() )
	{
		m_vboBuffer[0] = 0.0f;
		m_vboBuffer[1] = 0.0f;
		m_vboBuffer[2] = 0.0f;
		m_vboBuffer[3] = 0.0f;
		BufferDeclaration l_skymapDeclaration
		{
			{
				BufferElementDeclaration( ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eVec2 )
			}
		};
		BufferDeclaration l_cloudsVertexDeclaration
		{
			{
				BufferElementDeclaration( ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eVec3 )
			}
		};

#if ENABLE_FFT

		BufferDeclaration l_quadVertexDeclaration
		{
			{
				BufferElementDeclaration( 0, uint32_t( ElementUsage::ePosition ), ElementType::eVec4 )
			}
		};
		m_variancesVtxBuffer = std::make_shared< VertexBuffer >( *GetEngine(), l_quadVertexDeclaration );
		m_initVtxBuffer = std::make_shared< VertexBuffer >( *GetEngine(), l_quadVertexDeclaration );
		m_fftxVtxBuffer = std::make_shared< VertexBuffer >( *GetEngine(), l_quadVertexDeclaration );
		m_fftyVtxBuffer = std::make_shared< VertexBuffer >( *GetEngine(), l_quadVertexDeclaration );
		m_variancesIdxBuffer = std::make_shared< IndexBuffer >( *GetEngine() );
		m_initIdxBuffer = std::make_shared< IndexBuffer >( *GetEngine() );
		m_fftxIdxBuffer = std::make_shared< IndexBuffer >( *GetEngine() );
		m_fftyIdxBuffer = std::make_shared< IndexBuffer >( *GetEngine() );
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

#endif

		m_skyVtxBuffer = std::make_shared< VertexBuffer >( *GetEngine(), l_skymapDeclaration );
		m_skymapVtxBuffer = std::make_shared< VertexBuffer >( *GetEngine(), l_skymapDeclaration );
		m_cloudsVtxBuffer = std::make_shared< VertexBuffer >( *GetEngine(), l_cloudsVertexDeclaration );
		m_skyIdxBuffer = std::make_shared< IndexBuffer >( *GetEngine() );
		m_skymapIdxBuffer = std::make_shared< IndexBuffer >( *GetEngine() );
		m_cloudsIdxBuffer = std::make_shared< IndexBuffer >( *GetEngine() );
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
	}

	RenderTechnique::~RenderTechnique()
	{
		m_renderPipeline.reset();
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

	RenderTechniqueSPtr RenderTechnique::CreateInstance( RenderTarget & p_renderTarget, RenderSystem & p_renderSystem, Parameters const & p_params )
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
		TextFile( l_pathShare / cuT( "atmosphere.frag" ), File::OpenMode::eRead ).CopyToString( l_strAtmF );
		TextFile( l_pathShare / cuT( "atmosphere.vert" ), File::OpenMode::eRead ).CopyToString( l_strAtmV );
#if ENABLE_FFT
		TextFile( l_pathShare / cuT( "ocean_fft.frag" ), File::OpenMode::eRead ).CopyToString( l_strOcnF );
		TextFile( l_pathShare / cuT( "ocean_fft.vert" ), File::OpenMode::eRead ).CopyToString( l_strOcnV );
#else
		TextFile( l_pathShare / cuT( "ocean.frag" ), File::OpenMode::eRead ).CopyToString( l_strOcnF );
		TextFile( l_pathShare / cuT( "ocean.vert" ), File::OpenMode::eRead ).CopyToString( l_strOcnV );
#endif
		TextFile( l_pathShare / cuT( "sky.frag" ), File::OpenMode::eRead ).CopyToString( l_strSkyF );
		TextFile( l_pathShare / cuT( "sky.vert" ), File::OpenMode::eRead ).CopyToString( l_strSkyV );
		TextFile( l_pathShare / cuT( "skymap.frag" ), File::OpenMode::eRead ).CopyToString( l_strMapF );
		TextFile( l_pathShare / cuT( "skymap.vert" ), File::OpenMode::eRead ).CopyToString( l_strMapV );
		l_strSrcF = l_strOpt + cuT( "\n" ) + l_strAtmF + cuT( "\n" ) + l_strOcnF;
		l_strSrcV = l_strOpt + cuT( "\n" ) + l_strAtmV + cuT( "\n" ) + l_strOcnV;
		Logger::LogDebug( "Loading 'render' shader program" );
		m_render = GetEngine()->GetRenderSystem()->GetEngine()->GetShaderProgramCache().GetNewProgram( false );
		m_render->CreateObject( ShaderType::eVertex );
		m_render->CreateObject( ShaderType::ePixel );
		m_render->SetSource( ShaderType::eVertex, ShaderModel::eCount, l_strVertex + l_strSrcV );
		m_render->SetSource( ShaderType::ePixel, ShaderModel::eCount, l_strPixel + l_strSrcF );
		m_renderSkyIrradianceSampler = m_render->CreateFrameVariable< OneIntFrameVariable >( cuT( "skyIrradianceSampler" ), ShaderType::ePixel );
		m_renderInscatterSampler = m_render->CreateFrameVariable< OneIntFrameVariable >( cuT( "inscatterSampler" ), ShaderType::ePixel );
		m_renderTransmittanceSampler = m_render->CreateFrameVariable< OneIntFrameVariable >( cuT( "transmittanceSampler" ), ShaderType::ePixel );
		m_renderSkySampler = m_render->CreateFrameVariable< OneIntFrameVariable >( cuT( "skySampler" ), ShaderType::ePixel );
		m_renderSkyIrradianceSampler->SetValue( IRRADIANCE_UNIT );
		m_renderInscatterSampler->SetValue( INSCATTER_UNIT );
		m_renderTransmittanceSampler->SetValue( TRANSMITTANCE_UNIT );
		m_renderSkySampler->SetValue( SKY_UNIT );

#if ENABLE_FFT

		m_renderSpectrum_1_2_Sampler = m_render->CreateFrameVariable< OneIntFrameVariable >( cuT( "spectrum_1_2_Sampler" ), ShaderType::ePixel );
		m_renderSpectrum_3_4_Sampler = m_render->CreateFrameVariable< OneIntFrameVariable >( cuT( "spectrum_3_4_Sampler" ), ShaderType::ePixel );
		m_renderFftWavesSampler = m_render->CreateFrameVariable< OneIntFrameVariable >( cuT( "fftWavesSampler" ), ShaderType::ePixel );
		m_renderSlopeVarianceSampler = m_render->CreateFrameVariable< OneIntFrameVariable >( cuT( "slopeVarianceSampler" ), ShaderType::ePixel );

#else

		m_renderWavesSampler = m_render->CreateFrameVariable< OneIntFrameVariable >( cuT( "wavesSampler" ), ShaderType::ePixel );
		m_renderWavesSampler->SetValue( m_pTexWave.get() );

#endif

		{
			auto & l_constants = m_render->CreateFrameVariableBuffer( cuT( "render" ), ShaderTypeFlag::eVertex | ShaderTypeFlag::ePixel );
			m_renderScreenToCamera = l_constants.CreateVariable< Matrix4x4fFrameVariable >( cuT( "screenToCamera" ) );
			m_renderCameraToWorld = l_constants.CreateVariable< Matrix4x4fFrameVariable >( cuT( "cameraToWorld" ) );
			m_renderWorldToScreen = l_constants.CreateVariable< Matrix4x4fFrameVariable >( cuT( "worldToScreen" ) );
			m_renderWorldCamera = l_constants.CreateVariable< Point3fFrameVariable >( cuT( "worldCamera" ) );
			m_renderWorldSunDir = l_constants.CreateVariable< Point3fFrameVariable >( cuT( "worldSunDir" ) );
			m_renderHdrExposure = l_constants.CreateVariable< OneFloatFrameVariable >( cuT( "hdrExposure" ) );
			m_renderSeaColor = l_constants.CreateVariable< Point3fFrameVariable >( cuT( "seaColor" ) );

#if ENABLE_FFT

			m_renderGridSizes = l_constants.CreateVariable< Point4fFrameVariable >( cuT( "GRID_SIZES" ) );
			m_renderGridSize = l_constants.CreateVariable< Point2fFrameVariable >( cuT( "gridSize" ) );
			m_renderChoppy = l_constants.CreateVariable< OneFloatFrameVariable >( cuT( "choppy" ) );

#else

			m_renderWorldToWind = l_constants.CreateVariable< Matrix2x2fFrameVariable >( cuT( "worldToWind" ) );
			m_renderWindToWorld = l_constants.CreateVariable< Matrix2x2fFrameVariable >( cuT( "windToWorld" ) );
			m_renderNbWaves = l_constants.CreateVariable< OneFloatFrameVariable >( cuT( "nbWaves" ) );
			m_renderHeightOffset = l_constants.CreateVariable< OneFloatFrameVariable >( cuT( "heightOffset" ) );
			m_renderSigmaSqTotal = l_constants.CreateVariable< Point2fFrameVariable >( cuT( "sigmaSqTotal" ) );
			m_renderTime = l_constants.CreateVariable< OneFloatFrameVariable >( cuT( "time" ) );
			m_renderLods = l_constants.CreateVariable< Point4fFrameVariable >( cuT( "lods" ) );
			m_renderNyquistMin = l_constants.CreateVariable< OneFloatFrameVariable >( cuT( "nyquistMin" ) );
			m_renderNyquistMax = l_constants.CreateVariable< OneFloatFrameVariable >( cuT( "nyquistMax" ) );

#endif

		}
		m_render->Initialise();

		if ( !all )
		{
			return;
		}

		l_strSrcV = l_strOpt + cuT( "\n" ) + l_strAtmV + cuT( "\n" ) + l_strSkyV;
		l_strSrcF = l_strOpt + cuT( "\n" ) + l_strAtmF + cuT( "\n" ) + l_strSkyF;
		Logger::LogDebug( "Loading 'sky' shader program" );
		m_sky = GetEngine()->GetRenderSystem()->GetEngine()->GetShaderProgramCache().GetNewProgram( false );
		m_sky->CreateObject( ShaderType::eVertex );
		m_sky->CreateObject( ShaderType::ePixel );
		m_sky->SetSource( ShaderType::eVertex, ShaderModel::eCount, l_strVertex + l_strSrcV );
		m_sky->SetSource( ShaderType::ePixel, ShaderModel::eCount, l_strPixel + l_strSrcF );
		m_skySkyIrradianceSampler = m_sky->CreateFrameVariable< OneIntFrameVariable >( cuT( "skyIrradianceSampler" ), ShaderType::ePixel );
		m_skyInscatterSampler = m_sky->CreateFrameVariable< OneIntFrameVariable >( cuT( "inscatterSampler" ), ShaderType::ePixel );
		m_skyTransmittanceSampler = m_sky->CreateFrameVariable< OneIntFrameVariable >( cuT( "transmittanceSampler" ), ShaderType::ePixel );
		m_skySkySampler = m_sky->CreateFrameVariable< OneIntFrameVariable >( cuT( "skySampler" ), ShaderType::ePixel );
		m_skySkyIrradianceSampler->SetValue( IRRADIANCE_UNIT );
		m_skyInscatterSampler->SetValue( INSCATTER_UNIT );
		m_skyTransmittanceSampler->SetValue( TRANSMITTANCE_UNIT );
		m_skySkySampler->SetValue( SKY_UNIT );
		{
			auto & l_constants = m_sky->CreateFrameVariableBuffer( cuT( "sky" ), ShaderTypeFlag::eVertex | ShaderTypeFlag::ePixel );
			m_skyScreenToCamera = l_constants.CreateVariable< Matrix4x4fFrameVariable >( cuT( "screenToCamera" ) );
			m_skyCameraToWorld = l_constants.CreateVariable< Matrix4x4fFrameVariable >( cuT( "cameraToWorld" ) );
			m_skyWorldCamera = l_constants.CreateVariable< Point3fFrameVariable >( cuT( "worldCamera" ) );
			m_skyWorldSunDir = l_constants.CreateVariable< Point3fFrameVariable >( cuT( "worldSunDir" ) );
			m_skyHdrExposure = l_constants.CreateVariable< OneFloatFrameVariable >( cuT( "hdrExposure" ) );
		}
		m_sky->Initialise();
		l_strSrcV = l_strOpt + cuT( "\n" ) + l_strAtmV + cuT( "\n" ) + l_strMapV;
		l_strSrcF = l_strOpt + cuT( "\n" ) + l_strAtmF + cuT( "\n" ) + l_strMapF;
		Logger::LogDebug( "Loading 'skymap' shader program" );
		m_skymap = GetEngine()->GetRenderSystem()->GetEngine()->GetShaderProgramCache().GetNewProgram( false );
		m_skymap->CreateObject( ShaderType::eVertex );
		m_skymap->CreateObject( ShaderType::ePixel );
		m_skymap->SetSource( ShaderType::eVertex, ShaderModel::eCount, l_strVertex + l_strSrcV );
		m_skymap->SetSource( ShaderType::ePixel, ShaderModel::eCount, l_strPixel + l_strSrcF );
		m_skymapSkyIrradianceSampler = m_skymap->CreateFrameVariable< OneIntFrameVariable >( cuT( "skyIrradianceSampler" ), ShaderType::ePixel );
		m_skymapInscatterSampler = m_skymap->CreateFrameVariable< OneIntFrameVariable >( cuT( "inscatterSampler" ), ShaderType::ePixel );
		m_skymapTransmittanceSampler = m_skymap->CreateFrameVariable< OneIntFrameVariable >( cuT( "transmittanceSampler" ), ShaderType::ePixel );
		m_skymapNoiseSampler = m_skymap->CreateFrameVariable< OneIntFrameVariable >( cuT( "noiseSampler" ), ShaderType::ePixel );
		m_skymapSkyIrradianceSampler->SetValue( IRRADIANCE_UNIT );
		m_skymapInscatterSampler->SetValue( INSCATTER_UNIT );
		m_skymapTransmittanceSampler->SetValue( TRANSMITTANCE_UNIT );
		m_skymapNoiseSampler->SetValue( NOISE_UNIT );
		{
			auto & l_constants = m_skymap->CreateFrameVariableBuffer( cuT( "skymap" ), ShaderTypeFlag::eVertex | ShaderTypeFlag::ePixel );
			m_skymapSunDir = l_constants.CreateVariable< Point3fFrameVariable >( cuT( "sunDir" ) );
			m_skymapOctaves = l_constants.CreateVariable< OneFloatFrameVariable >( cuT( "octaves" ) );
			m_skymapLacunarity = l_constants.CreateVariable< OneFloatFrameVariable >( cuT( "lacunarity" ) );
			m_skymapGain = l_constants.CreateVariable< OneFloatFrameVariable >( cuT( "gain" ) );
			m_skymapNorm = l_constants.CreateVariable< OneFloatFrameVariable >( cuT( "norm" ) );
			m_skymapClamp1 = l_constants.CreateVariable< OneFloatFrameVariable >( cuT( "clamp1" ) );
			m_skymapClamp2 = l_constants.CreateVariable< OneFloatFrameVariable >( cuT( "clamp2" ) );
			m_skymapCloudsColor = l_constants.CreateVariable< Point4fFrameVariable >( cuT( "cloudsColor" ) );
		}
		m_skymap->Initialise();

		if ( !m_clouds )
		{
			String l_strCloV;
			String l_strCloF;
			TextFile( l_pathShare / cuT( "clouds.vert" ), File::OpenMode::eRead ).CopyToString( l_strCloV );
			TextFile( l_pathShare / cuT( "clouds.frag" ), File::OpenMode::eRead ).CopyToString( l_strCloF );
			l_strSrcV = l_strOpt + cuT( "\n" ) + l_strAtmV + cuT( "\n" ) + l_strCloV;
			l_strSrcF = l_strOpt + cuT( "\n" ) + l_strAtmF + cuT( "\n" ) + l_strCloF;
			Logger::LogDebug( "Loading 'clouds' shader program" );
			m_clouds = GetEngine()->GetRenderSystem()->GetEngine()->GetShaderProgramCache().GetNewProgram( false );
			m_clouds->CreateObject( ShaderType::eVertex );
			m_clouds->CreateObject( ShaderType::ePixel );
			m_clouds->SetSource( ShaderType::eVertex, ShaderModel::eCount, l_strVertex + l_strSrcV );
			m_clouds->SetSource( ShaderType::ePixel, ShaderModel::eCount, l_strPixel + l_strSrcF );
			m_cloudsSkyIrradianceSampler = m_clouds->CreateFrameVariable< OneIntFrameVariable >( cuT( "skyIrradianceSampler" ), ShaderType::ePixel );
			m_cloudsInscatterSampler = m_clouds->CreateFrameVariable< OneIntFrameVariable >( cuT( "inscatterSampler" ), ShaderType::ePixel );
			m_cloudsTransmittanceSampler = m_clouds->CreateFrameVariable< OneIntFrameVariable >( cuT( "transmittanceSampler" ), ShaderType::ePixel );
			m_cloudsNoiseSampler = m_clouds->CreateFrameVariable< OneIntFrameVariable >( cuT( "noiseSampler" ), ShaderType::ePixel );
			m_cloudsSkyIrradianceSampler->SetValue( IRRADIANCE_UNIT );
			m_cloudsInscatterSampler->SetValue( INSCATTER_UNIT );
			m_cloudsTransmittanceSampler->SetValue( TRANSMITTANCE_UNIT );
			m_cloudsNoiseSampler->SetValue( NOISE_UNIT );
			{
				auto & l_constants = m_clouds->CreateFrameVariableBuffer( cuT( "clouds" ), ShaderTypeFlag::eVertex | ShaderTypeFlag::ePixel );
				m_cloudsWorldToScreen = l_constants.CreateVariable< Matrix4x4fFrameVariable >( cuT( "worldToScreen" ) );
				m_cloudsWorldCamera = l_constants.CreateVariable< Point3fFrameVariable >( cuT( "worldCamera" ) );
				m_cloudsWorldSunDir = l_constants.CreateVariable< Point3fFrameVariable >( cuT( "worldSunDir" ) );
				m_cloudsHdrExposure = l_constants.CreateVariable< OneFloatFrameVariable >( cuT( "hdrExposure" ) );
				m_cloudsOctaves = l_constants.CreateVariable< OneFloatFrameVariable >( cuT( "octaves" ) );
				m_cloudsLacunarity = l_constants.CreateVariable< OneFloatFrameVariable >( cuT( "lacunarity" ) );
				m_cloudsGain = l_constants.CreateVariable< OneFloatFrameVariable >( cuT( "gain" ) );
				m_cloudsNorm = l_constants.CreateVariable< OneFloatFrameVariable >( cuT( "norm" ) );
				m_cloudsClamp1 = l_constants.CreateVariable< OneFloatFrameVariable >( cuT( "clamp1" ) );
				m_cloudsClamp2 = l_constants.CreateVariable< OneFloatFrameVariable >( cuT( "clamp2" ) );
				m_cloudsCloudsColor = l_constants.CreateVariable< Point4fFrameVariable >( cuT( "cloudsColor" ) );
			}
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
		TextFile( l_pathShare / cuT( "init.frag" ), File::OpenMode::eRead ).CopyToString( l_strIniF );
		TextFile( l_pathShare / cuT( "init.vert" ), File::OpenMode::eRead ).CopyToString( l_strIniV );
		TextFile( l_pathShare / cuT( "variances.frag" ), File::OpenMode::eRead ).CopyToString( l_strVarF );
		TextFile( l_pathShare / cuT( "variances.vert" ), File::OpenMode::eRead ).CopyToString( l_strVarV );
		TextFile( l_pathShare / cuT( "fftx.frag" ), File::OpenMode::eRead ).CopyToString( l_strFtxF );
		TextFile( l_pathShare / cuT( "fftx.vert" ), File::OpenMode::eRead ).CopyToString( l_strFtxV );
		TextFile( l_pathShare / cuT( "ffty.frag" ), File::OpenMode::eRead ).CopyToString( l_strFtyF );
		TextFile( l_pathShare / cuT( "ffty.vert" ), File::OpenMode::eRead ).CopyToString( l_strFtyV );
		l_strSrcV = l_strIniV;
		l_strSrcF = l_strIniF;
		Logger::LogDebug( "Loading 'init' shader program" );
		m_init = GetEngine()->GetRenderSystem()->GetEngine()->GetShaderProgramCache().GetNewProgram( false );
		m_init->CreateObject( ShaderType::eVertex );
		m_init->CreateObject( ShaderType::ePixel );
		m_init->SetSource( ShaderType::eVertex, ShaderModel::eCount, l_strVertex + l_strSrcV );
		m_init->SetSource( ShaderType::ePixel, ShaderModel::eCount, l_strPixel + l_strSrcF );
		m_initSpectrum_1_2_Sampler = m_init->CreateFrameVariable< OneIntFrameVariable >( cuT( "spectrum_1_2_Sampler" ), ShaderType::ePixel );
		m_initSpectrum_3_4_Sampler = m_init->CreateFrameVariable< OneIntFrameVariable >( cuT( "spectrum_3_4_Sampler" ), ShaderType::ePixel );
		m_initSpectrum_1_2_Sampler->SetValue( SPECTRUM_1_2_UNIT );
		m_initSpectrum_3_4_Sampler->SetValue( SPECTRUM_3_4_UNIT );
		{
			auto & l_constants = m_init->CreateFrameVariableBuffer( cuT( "init" ), ShaderTypeFlag::eVertex | ShaderTypeFlag::ePixel );
			m_initFftSize = l_constants.CreateVariable< OneFloatFrameVariable >( cuT( "FFT_SIZE" ) );
			m_initInverseGridSizes = l_constants.CreateVariable< Point4fFrameVariable >( cuT( "INVERSE_GRID_SIZES" ) );
			m_initT = l_constants.CreateVariable< OneFloatFrameVariable >( cuT( "t" ) );
		}
		m_init->Initialise();
		l_strSrcV = l_strVarV;
		l_strSrcF = l_strVarF;
		Logger::LogDebug( "Loading 'variances' shader program" );
		m_variances = GetEngine()->GetRenderSystem()->GetEngine()->GetShaderProgramCache().GetNewProgram( false );
		m_variances->CreateObject( ShaderType::eVertex );
		m_variances->CreateObject( ShaderType::ePixel );
		m_variances->SetSource( ShaderType::eVertex, ShaderModel::eCount, l_strVertex + l_strSrcV );
		m_variances->SetSource( ShaderType::ePixel, ShaderModel::eCount, l_strPixel + l_strSrcF );
		m_variancesSpectrum_1_2_Sampler = m_variances->CreateFrameVariable< OneIntFrameVariable >( cuT( "spectrum_1_2_Sampler" ), ShaderType::ePixel );
		m_variancesSpectrum_3_4_Sampler = m_variances->CreateFrameVariable< OneIntFrameVariable >( cuT( "spectrum_3_4_Sampler" ), ShaderType::ePixel );
		m_variancesSpectrum_1_2_Sampler->SetValue( SPECTRUM_1_2_UNIT );
		m_variancesSpectrum_3_4_Sampler->SetValue( SPECTRUM_3_4_UNIT );
		{
			auto & l_constants = m_variances->CreateFrameVariableBuffer( cuT( "variances" ), ShaderTypeFlag::eVertex | ShaderTypeFlag::ePixel );
			m_variancesNSlopeVariance = l_constants.CreateVariable< OneFloatFrameVariable >( cuT( "N_SLOPE_VARIANCE" ) );
			m_variancesFFTSize = l_constants.CreateVariable< OneIntFrameVariable >( cuT( "FFT_SIZE" ) );
			m_variancesGridSizes = l_constants.CreateVariable< Point4fFrameVariable >( cuT( "GRID_SIZES" ) );
			m_variancesSlopeVarianceDelta = l_constants.CreateVariable< OneFloatFrameVariable >( cuT( "slopeVarianceDelta" ) );
			m_variancesC = l_constants.CreateVariable< OneFloatFrameVariable >( cuT( "c" ) );
			m_variancesNSlopeVariance->SetValue( float( m_N_SLOPE_VARIANCE ) );
			m_variancesFFTSize->SetValue( m_FFT_SIZE );
		}
		m_variances->Initialise();
		l_strSrcV = l_strFtxV;
		l_strSrcF = l_strFtxF;
		Logger::LogDebug( "Loading 'fftx' shader program" );
		m_fftx = GetEngine()->GetRenderSystem()->GetEngine()->GetShaderProgramCache().GetNewProgram( false );
		m_fftx->CreateObject( ShaderType::eVertex );
		m_fftx->CreateObject( ShaderType::ePixel );
		m_fftx->SetSource( ShaderType::eVertex, ShaderModel::eCount, l_strVertex + l_strSrcV );
		m_fftx->SetSource( ShaderType::ePixel, ShaderModel::eCount, l_strPixel + l_strSrcF );
		m_fftxButterflySampler = m_fftx->CreateFrameVariable< OneIntFrameVariable >( cuT( "butterflySampler" ), ShaderType::ePixel );
		m_fftxImgSampler = m_fftx->CreateFrameVariable< OneIntFrameVariable >( cuT( "imgSampler" ), ShaderType::ePixel );
		m_fftxButterflySampler->SetValue( BUTTERFLY_UNIT );
		{
			auto & l_constants = m_fftx->CreateFrameVariableBuffer( cuT( "fftx" ), ShaderTypeFlag::eVertex | ShaderTypeFlag::ePixel );
			m_fftxNLayers = l_constants.CreateVariable< OneIntFrameVariable >( cuT( "nLayers" ) );
			m_fftxPass = l_constants.CreateVariable< OneFloatFrameVariable >( cuT( "pass" ) );
		}
		m_fftx->Initialise();
		l_strSrcV = l_strFtyV;
		l_strSrcF = l_strFtyF;
		Logger::LogDebug( "Loading 'ffty' shader program" );
		m_ffty = GetEngine()->GetRenderSystem()->GetEngine()->GetShaderProgramCache().GetNewProgram( false );
		m_ffty->CreateObject( ShaderType::eVertex );
		m_ffty->CreateObject( ShaderType::ePixel );
		m_ffty->SetSource( ShaderType::eVertex, ShaderModel::eCount, l_strVertex + l_strSrcV );
		m_ffty->SetSource( ShaderType::ePixel, ShaderModel::eCount, l_strPixel + l_strSrcF );
		m_fftyButterflySampler = m_ffty->CreateFrameVariable< OneIntFrameVariable >( cuT( "butterflySampler" ), ShaderType::ePixel );
		m_fftyImgSampler = m_ffty->CreateFrameVariable< OneIntFrameVariable >( cuT( "imgSampler" ), ShaderType::ePixel );
		m_fftyButterflySampler->SetValue( BUTTERFLY_UNIT );
		{
			auto & l_constants = m_ffty->CreateFrameVariableBuffer( cuT( "ffty" ), ShaderTypeFlag::eVertex | ShaderTypeFlag::ePixel );
			m_fftyNLayers = l_constants.CreateVariable< OneIntFrameVariable >( cuT( "nLayers" ) );
			m_fftyPass = l_constants.CreateVariable< OneFloatFrameVariable >( cuT( "pass" ) );
		}
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
			m_renderVtxBuffer->Destroy();
			m_renderIdxBuffer->Destroy();
			m_renderGBuffers.reset();
		}

		BufferElementDeclaration l_meshDeclaration[] =
		{
			BufferElementDeclaration( ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eVec4 )
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

		m_renderVtxBuffer->Resize( n * sizeof( float ) );
		std::memcpy( m_renderVtxBuffer->data(), &data[0], n * sizeof( float ) );
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

		m_renderIdxBuffer->Resize( n );
		std::memcpy( m_renderIdxBuffer->data(), &indices[0], n * sizeof( uint32_t ) );
		m_renderVtxBuffer->Create();
		m_renderIdxBuffer->Create();
		m_renderVtxBuffer->Upload( BufferAccessType::eStatic, BufferAccessNature::eDraw );
		m_renderIdxBuffer->Upload( BufferAccessType::eStatic, BufferAccessNature::eDraw );
		m_renderGBuffers = GetEngine()->GetRenderSystem()->CreateGeometryBuffers( Topology::eTriangles, *m_render );
		m_renderGBuffers->Initialise( { *m_renderVtxBuffer }, m_renderIdxBuffer );
	}

	bool RenderTechnique::DoCreateSamplers()
	{
		bool l_return = true;
		auto & l_cache = GetEngine()->GetSamplerCache();

		m_pSamplerNearestClamp = l_cache.Add( cuT( "NearestClamp" ) );
		m_pSamplerNearestClamp->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
		m_pSamplerNearestClamp->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
		m_pSamplerNearestClamp->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
		m_pSamplerNearestClamp->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eNearest );
		m_pSamplerNearestClamp->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eNearest );
		l_return &= m_pSamplerNearestClamp->Initialise();

		m_pSamplerNearestRepeat = l_cache.Add( cuT( "NearestRepeat" ) );
		m_pSamplerNearestRepeat->SetWrappingMode( TextureUVW::eU, WrapMode::eRepeat );
		m_pSamplerNearestRepeat->SetWrappingMode( TextureUVW::eV, WrapMode::eRepeat );
		m_pSamplerNearestRepeat->SetWrappingMode( TextureUVW::eW, WrapMode::eRepeat );
		m_pSamplerNearestRepeat->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eNearest );
		m_pSamplerNearestRepeat->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eNearest );
		l_return &= m_pSamplerNearestRepeat->Initialise();

		m_pSamplerLinearClamp = l_cache.Add( cuT( "LinearClamp" ) );
		m_pSamplerLinearClamp->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
		m_pSamplerLinearClamp->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
		m_pSamplerLinearClamp->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
		m_pSamplerLinearClamp->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
		m_pSamplerLinearClamp->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
		l_return &= m_pSamplerLinearClamp->Initialise();

		m_pSamplerLinearRepeat = l_cache.Add( cuT( "LinearRepeat" ) );
		m_pSamplerLinearRepeat->SetWrappingMode( TextureUVW::eU, WrapMode::eRepeat );
		m_pSamplerLinearRepeat->SetWrappingMode( TextureUVW::eV, WrapMode::eRepeat );
		m_pSamplerLinearRepeat->SetWrappingMode( TextureUVW::eW, WrapMode::eRepeat );
		m_pSamplerLinearRepeat->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
		m_pSamplerLinearRepeat->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
		l_return &= m_pSamplerLinearRepeat->Initialise();

		m_pSamplerAnisotropicClamp = l_cache.Add( cuT( "AnisotropicClamp" ) );
		m_pSamplerAnisotropicClamp->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
		m_pSamplerAnisotropicClamp->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
		m_pSamplerAnisotropicClamp->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
		m_pSamplerAnisotropicClamp->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
		m_pSamplerAnisotropicClamp->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
		m_pSamplerAnisotropicClamp->SetMaxAnisotropy( real( 0.1 ) );
		l_return &= m_pSamplerAnisotropicClamp->Initialise();

		m_pSamplerAnisotropicRepeat = l_cache.Add( cuT( "AnisotropicRepeat" ) );
		m_pSamplerAnisotropicRepeat->SetWrappingMode( TextureUVW::eU, WrapMode::eRepeat );
		m_pSamplerAnisotropicRepeat->SetWrappingMode( TextureUVW::eV, WrapMode::eRepeat );
		m_pSamplerAnisotropicRepeat->SetWrappingMode( TextureUVW::eW, WrapMode::eRepeat );
		m_pSamplerAnisotropicRepeat->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
		m_pSamplerAnisotropicRepeat->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
		m_pSamplerAnisotropicRepeat->SetMaxAnisotropy( real( 0.1 ) );
		l_return &= m_pSamplerAnisotropicRepeat->Initialise();

		return l_return;
	}

	void RenderTechnique::DoDestroySamplers()
	{
		m_pSamplerAnisotropicRepeat->Cleanup();
		m_pSamplerAnisotropicRepeat.reset();
		m_pSamplerAnisotropicClamp->Cleanup();
		m_pSamplerAnisotropicClamp.reset();
		m_pSamplerLinearRepeat->Cleanup();
		m_pSamplerLinearRepeat.reset();
		m_pSamplerLinearClamp->Cleanup();
		m_pSamplerLinearClamp.reset();
		m_pSamplerNearestRepeat->Cleanup();
		m_pSamplerNearestRepeat.reset();
		m_pSamplerNearestClamp->Cleanup();
		m_pSamplerNearestClamp.reset();
	}

	bool RenderTechnique::DoCreateTextures()
	{
		bool l_return = true;
		FILE * f = nullptr;
		PxBufferBaseSPtr buffer = PxBufferBase::create( Size( 64, 16 ), PixelFormat::eRGB16F32F );

		if ( Castor::FOpen( f, string::string_cast< char >( Engine::GetDataDirectory() / cuT( "OceanLighting/data/irradiance.raw" ) ).c_str(), "rb" ) )
		{
			auto read = fread( buffer->ptr(), 1, 16 * 64 * 3 * sizeof( float ), f );
			ENSURE( read == 16 * 64 * 3 * sizeof( float ) );
			fclose( f );
		}

		m_pTexIrradiance = GetEngine()->GetRenderSystem()->CreateTexture( TextureType::eTwoDimensions, AccessType::eRead, AccessType::eRead );
		m_pTexIrradiance->SetSource( buffer );
		l_return &= m_pTexIrradiance->Initialise();
		int res = 64;
		int nr = res / 2;
		int nv = res * 2;
		int nb = res / 2;
		int na = 8;
		buffer = PxBufferBase::create( Size( na * nb, nv * nr ), PixelFormat::eRGB16F32F );

		if ( Castor::FOpen( f, string::string_cast< char >( Engine::GetDataDirectory() / cuT( "OceanLighting/data/inscatter.raw" ) ).c_str(), "rb" ) )
		{
			auto read = fread( buffer->ptr(), 1, nr * nv * nb * na * 4 * sizeof( float ), f );
			ENSURE( read == nr * nv * nb * na * 4 * sizeof( float ) );
			fclose( f );
		}

		m_pTexInscatter = GetEngine()->GetRenderSystem()->CreateTexture( TextureType::eThreeDimensions, AccessType::eRead, AccessType::eRead, buffer->format(), Point3ui( na * nb, nv, nr ) );
		l_return &= m_pTexInscatter->Initialise();
		buffer = PxBufferBase::create( Size( 256, 64 ), PixelFormat::eRGB16F32F );

		if ( Castor::FOpen( f, string::string_cast< char >( Engine::GetDataDirectory() / cuT( "OceanLighting/data/transmittance.raw" ) ).c_str(), "rb" ) )
		{
			auto read = fread( buffer->ptr(), 1, 256 * 64 * 3 * sizeof( float ), f );
			ENSURE( read == 256 * 64 * 3 * sizeof( float ) );
			fclose( f );
		}

		m_pTexTransmittance = GetEngine()->GetRenderSystem()->CreateTexture( TextureType::eTwoDimensions, AccessType::eRead, AccessType::eRead, PixelFormat::eR8_UNORM, Size( 512, 512 ) );
		m_pTexTransmittance->SetSource( buffer );
		l_return &= m_pTexTransmittance->Initialise();

		if ( Castor::FOpen( f, string::string_cast< char >( Engine::GetDataDirectory() / cuT( "OceanLighting/data/noise.pgm" ) ).c_str(), "rb" ) )
		{
			unsigned char * img = new unsigned char[512 * 512 + 38];
			auto read = fread( img, 1, 512 * 512 + 38, f );
			ENSURE( read == 512 * 512 + 38 );
			fclose( f );
			std::memcpy( m_pTexNoise->GetImage().GetBuffer()->ptr(), &img[38], 512 * 512 );
			delete[] img;
		}

		m_pTexNoise = m_renderSystem.CreateTexture( TextureType::eTwoDimensions, AccessType::eNone, AccessType::eRead | AccessType::eWrite );
		l_return &= m_pTexNoise->Initialise();
		m_pTexNoise->Bind( NOISE_UNIT );
		m_pTexNoise->GenerateMipmaps();
		m_pTexNoise->Unbind( NOISE_UNIT );

		m_pTexSky = m_renderSystem.CreateTexture( TextureType::eTwoDimensions, AccessType::eNone, AccessType::eRead | AccessType::eWrite, PixelFormat::eRGBA16F32F, Size( m_skyTexSize, m_skyTexSize ) );
		l_return &= m_pTexSky->Initialise();
		m_pTexSky->Bind( SKY_UNIT );
		m_pTexSky->GenerateMipmaps();
		m_pTexSky->Unbind( SKY_UNIT );

#if ENABLE_FFT

		m_pTexSpectrum_1_2 = m_renderSystem.CreateTexture( TextureType::eTwoDimensions, AccessType::eNone, AccessType::eRead | AccessType::eWrite, PixelFormat::eR32G32B32A32_SFLOAT, Size( m_FFT_SIZE, m_FFT_SIZE ) );
		l_return &= m_pTexSpectrum_1_2->Initialise();

		m_pTexSpectrum_3_4 = m_renderSystem.CreateTexture( TextureType::eTwoDimensions, AccessType::eNone, AccessType::eRead | AccessType::eWrite, PixelFormat::eR32G32B32A32_SFLOAT, Size( m_FFT_SIZE, m_FFT_SIZE ) );
		l_return &= m_pTexSpectrum_3_4->Initialise();

		m_pTexSlopeVariance = m_renderSystem.CreateTexture( TextureType::eThreeDimensions, AccessType::eNone, AccessType::eRead | AccessType::eWrite, PixelFormat::eAL16F32F, Point3ui( m_N_SLOPE_VARIANCE, m_N_SLOPE_VARIANCE, m_N_SLOPE_VARIANCE ) );
		l_return &= m_pTexSlopeVariance->Initialise();

		m_pTexFFTA = m_renderSystem.CreateTexture( TextureType::eTwoDimensionsArray, AccessType::eNone, AccessType::eRead | AccessType::eWrite, PixelFormat::eR32G32B32_SFLOAT, Point3ui( m_FFT_SIZE, m_FFT_SIZE, 5 ) );
		l_return &= m_pTexFFTA->Initialise();
		m_pTexFFTA->Bind( FFT_A_UNIT );
		m_pTexFFTA->GenerateMipmaps();
		m_pTexFFTA->Unbind( FFT_A_UNIT );

		m_pTexFFTB = m_renderSystem.CreateTexture( TextureType::eTwoDimensionsArray, AccessType::eNone, AccessType::eRead | AccessType::eWrite, PixelFormat::eR32G32B32_SFLOAT, Point3ui( m_FFT_SIZE, m_FFT_SIZE, 5 ) );
		l_return &= m_pTexFFTB->Initialise();
		m_pTexFFTB->Bind( FFT_B_UNIT );
		m_pTexFFTB->GenerateMipmaps();
		m_pTexFFTB->Unbind( FFT_B_UNIT );

		m_pTexButterfly = m_renderSystem.CreateTexture( TextureType::eTwoDimensions, AccessType::eNone, AccessType::eRead | AccessType::eWrite, PixelFormat::eR32G32B32A32_SFLOAT, Size( m_FFT_SIZE, m_PASSES ) );
		auto lookupTexData = computeButterflyLookupTexture();
		std::memcpy( m_pTexButterfly->GetImage().GetBuffer()->ptr(), computeButterflyLookupTexture(), m_FFT_SIZE * m_PASSES * 4 * sizeof( float ) );
		delete [] lookupTexData;
		l_return &= m_pTexButterfly->Initialise();

#else

		m_pTexWave = m_renderTarget.CreateTexture( TextureType::eOneDimension, AccessType::eNone, AccessType::Read | AccessType::Write, PixelFormat::eARGB32F, Size( m_nbWaves, 1 ) );
		m_pTexWave->Initialise( WAVE_UNIT );
		m_pTexWave->SetSampler( m_pSamplerNearestClamp );

#endif

		return l_return;
	}

	void RenderTechnique::DoDestroyTextures()
	{
		m_pTexIrradiance->Cleanup();
		m_pTexIrradiance.reset();
		m_pTexInscatter->Cleanup();
		m_pTexInscatter.reset();
		m_pTexTransmittance->Cleanup();
		m_pTexTransmittance.reset();
		m_pTexNoise->Cleanup();
		m_pTexNoise.reset();
		m_pTexSky->Cleanup();
		m_pTexSky.reset();

#if ENABLE_FFT

		m_pTexSpectrum_1_2->Cleanup();
		m_pTexSpectrum_1_2.reset();
		m_pTexSpectrum_3_4->Cleanup();
		m_pTexSpectrum_3_4.reset();
		m_pTexSlopeVariance->Cleanup();
		m_pTexSlopeVariance.reset();
		m_pTexFFTA->Cleanup();
		m_pTexFFTA.reset();
		m_pTexFFTB->Cleanup();
		m_pTexFFTB.reset();
		m_pTexButterfly->Cleanup();
		m_pTexButterfly.reset();

#else

		m_pTexWave->Cleanup();
		m_pTexWave.reset();

#endif
	}

	bool RenderTechnique::DoCreateFramebuffers()
	{
		bool l_return = true;
		m_frameBuffer = m_renderSystem.CreateFrameBuffer();
		m_pColorBuffer = m_renderSystem.CreateTexture( TextureType::eTwoDimensions, AccessType::eNone, AccessType::eRead | AccessType::eWrite, PixelFormat::eR8G8B8A8_UNORM, m_renderTarget.GetSize() );
		m_pDepthBuffer = m_frameBuffer->CreateDepthStencilRenderBuffer( PixelFormat::eD24_UNORM_S8_UINT );
		m_pColorAttach = m_frameBuffer->CreateAttachment( m_pColorBuffer );
		m_pDepthAttach = m_frameBuffer->CreateAttachment( m_pDepthBuffer );
		l_return &= m_frameBuffer->Create();
		l_return &= m_pDepthBuffer->Create();
		l_return &= m_pDepthBuffer->Initialise( m_renderTarget.GetSize() );
		l_return &= m_frameBuffer->Initialise( m_renderTarget.GetSize() );

		if ( l_return && m_frameBuffer->Bind( FrameBufferMode::eConfig ) )
		{
			l_return &= m_frameBuffer->Attach( AttachmentPoint::eColour, m_pColorAttach, m_pColorBuffer->GetType() );
			l_return &= m_frameBuffer->Attach( AttachmentPoint::eDepth, m_pDepthAttach );
			m_frameBuffer->Unbind();
		}


		m_fbo = m_renderSystem.CreateFrameBuffer();
		m_pAttachSky = m_fbo->CreateAttachment( m_pTexSky );
		l_return &= m_fbo->Create();
		l_return &= m_fbo->Initialise( m_pTexSky->GetDimensions() );

		if ( l_return && m_fbo->Bind( FrameBufferMode::eConfig ) )
		{
			//m_fbo->SetDrawBuffer( AttachmentPoint::eColour );
			m_fbo->Attach( AttachmentPoint::eColour, m_pAttachSky, m_pTexSky->GetType() );
			ENSURE( m_fbo->IsComplete() );
			m_fbo->Unbind();
		}

#if ENABLE_FFT

		m_variancesFbo = GetEngine()->GetRenderSystem()->CreateFrameBuffer();
		l_return &= m_variancesFbo->Create();
		l_return &= m_variancesFbo->Initialise( m_pTexSlopeVariance->GetDimensions() );

		if ( l_return && m_variancesFbo->Bind( FrameBufferMode::eConfig ) )
		{
			for ( int layer = 0; layer < m_N_SLOPE_VARIANCE; ++layer )
			{
				TextureAttachmentSPtr l_pAttach = m_variancesFbo->CreateAttachment( m_pTexSlopeVariance );
				m_arrayVarianceAttaches.push_back( l_pAttach );
				m_variancesFbo->Attach( AttachmentPoint::eColour, l_pAttach, m_pTexSlopeVariance->GetType(), layer );
			}

			m_variancesFbo->SetDrawBuffer( m_arrayVarianceAttaches[0] );
			ENSURE( m_variancesFbo->IsComplete() );
			m_variancesFbo->Unbind();
		}

		m_fftFbo1 = GetEngine()->GetRenderSystem()->CreateFrameBuffer();
		l_return &= m_fftFbo1->Create();
		l_return &= m_fftFbo1->Initialise( m_pTexFFTA->GetDimensions() );

		if ( l_return && m_fftFbo1->Bind( FrameBufferMode::eConfig ) )
		{
			for ( int i = 0; i < 5; ++i )
			{
				m_arrayFftAttaches.push_back( m_fftFbo1->CreateAttachment( m_pTexFFTA ) );
				m_fftFbo1->Attach( AttachmentPoint::eColour, i, m_arrayFftAttaches[i], m_pTexFFTA->GetType(), i );
			}

			m_fftFbo1->SetReadBuffer( AttachmentPoint::eColour, 0 );
			m_fftFbo1->SetDrawBuffers();
			ENSURE( m_fftFbo1->IsComplete() );
			m_fftFbo1->Unbind();
		}

		m_fftFbo2 = GetEngine()->GetRenderSystem()->CreateFrameBuffer();
		m_pAttachFftA = m_fftFbo2->CreateAttachment( m_pTexFFTA );
		m_pAttachFftB = m_fftFbo2->CreateAttachment( m_pTexFFTB );
		l_return &= m_fftFbo2->Create();
		l_return &= m_fftFbo2->Initialise( m_pTexFFTB->GetDimensions() );

		if ( l_return && m_fftFbo2->Bind( FrameBufferMode::eConfig ) )
		{
			m_fftFbo2->Attach( AttachmentPoint::eColour, 0, m_pAttachFftA, TextureType::eTwoDimensions );
			m_fftFbo2->Attach( AttachmentPoint::eColour, 1, m_pAttachFftB, TextureType::eTwoDimensions );
			ENSURE( m_fftFbo2->IsComplete() );
			m_fftFbo2->SetDrawBuffer( m_pAttachFftA );
			m_fftFbo2->SetReadBuffer( AttachmentPoint::eColour, 0 );
			m_fftFbo2->Unbind();
		}

#endif

		return l_return;
	}

	void RenderTechnique::DoDestroyFramebuffers()
	{
		m_fbo->Bind( FrameBufferMode::eConfig );
		m_fbo->DetachAll();
		m_fbo->Unbind();
		m_fbo->Cleanup();
		m_fbo->Destroy();
		m_fbo.reset();

#if ENABLE_FFT

		m_variancesFbo->Bind( FrameBufferMode::eConfig );
		m_variancesFbo->DetachAll();
		m_variancesFbo->Unbind();
		m_variancesFbo->Cleanup();
		m_variancesFbo->Destroy();
		m_variancesFbo.reset();

		m_fftFbo1->Bind( FrameBufferMode::eConfig );
		m_fftFbo1->DetachAll();
		m_fftFbo1->Unbind();
		m_fftFbo1->Cleanup();
		m_fftFbo1->Destroy();
		m_fftFbo1.reset();

		m_fftFbo2->Bind( FrameBufferMode::eConfig );
		m_fftFbo2->DetachAll();
		m_fftFbo2->Unbind();
		m_fftFbo2->Cleanup();
		m_fftFbo2->Destroy();
		m_fftFbo2.reset();

#endif

		m_frameBuffer->Bind( FrameBufferMode::eConfig );
		m_frameBuffer->DetachAll();
		m_frameBuffer->Unbind();
		m_frameBuffer->Cleanup();
		m_frameBuffer->Destroy();
		m_frameBuffer.reset();

		m_pColorBuffer->Cleanup();
		m_pColorBuffer.reset();
		m_pDepthBuffer->Cleanup();
		m_pDepthBuffer->Destroy();
		m_pDepthBuffer.reset();
	}

	bool RenderTechnique::DoCreate()
	{
		m_skyVtxBuffer->Create();
		m_skymapVtxBuffer->Create();
		m_cloudsVtxBuffer->Create();
		m_skyIdxBuffer->Create();
		m_skymapIdxBuffer->Create();
		m_cloudsIdxBuffer->Create();

#if ENABLE_FFT

		m_variancesVtxBuffer->Create();
		m_variancesIdxBuffer->Create();
		m_initVtxBuffer->Create();
		m_initIdxBuffer->Create();
		m_fftxVtxBuffer->Create();
		m_fftxIdxBuffer->Create();
		m_fftyVtxBuffer->Create();
		m_fftyIdxBuffer->Create();

#endif

		auto l_rsState = RasteriserState{};
		l_rsState.SetCulledFaces( Culling::eNone );
		m_renderPipeline = GetEngine()->GetRenderSystem()->CreatePipeline( DepthStencilState{}, std::move( l_rsState ), BlendState{}, MultisampleState{}, *m_render, PipelineFlags{} );
		
		BlendState l_blState;
		l_blState.EnableBlend( true );
		l_blState.SetAlphaSrcBlend( BlendOperand::eSrcAlpha );
		l_blState.SetAlphaDstBlend( BlendOperand::eInvSrcAlpha );
		m_cloudsPipeline = GetEngine()->GetRenderSystem()->CreatePipeline( DepthStencilState{}, RasteriserState{}, std::move( l_blState ), MultisampleState{}, *m_clouds, PipelineFlags{} );
		
		m_skyPipeline = GetEngine()->GetRenderSystem()->CreatePipeline( DepthStencilState{}, RasteriserState{}, BlendState{}, MultisampleState{}, *m_sky, PipelineFlags{} );
		
		auto l_dsState = DepthStencilState{};
		l_dsState.SetDepthTest( false );
		l_rsState = RasteriserState{};
		l_rsState.SetCulledFaces( Culling::eNone );
		m_skymapPipeline = GetEngine()->GetRenderSystem()->CreatePipeline( std::move( l_dsState ), std::move( l_rsState ), BlendState{}, MultisampleState{}, *m_skymap, PipelineFlags{} );

#if ENABLE_FFT

		m_initPipeline = GetEngine()->GetRenderSystem()->CreatePipeline( DepthStencilState{}, RasteriserState{}, BlendState{}, MultisampleState{}, *m_init, PipelineFlags{} );
		m_variancesPipeline = GetEngine()->GetRenderSystem()->CreatePipeline( DepthStencilState{}, RasteriserState{}, BlendState{}, MultisampleState{}, *m_variances, PipelineFlags{} );
		m_fftxPipeline = GetEngine()->GetRenderSystem()->CreatePipeline( DepthStencilState{}, RasteriserState{}, BlendState{}, MultisampleState{}, *m_fftx, PipelineFlags{} );
		m_fftyPipeline = GetEngine()->GetRenderSystem()->CreatePipeline( DepthStencilState{}, RasteriserState{}, BlendState{}, MultisampleState{}, *m_ffty, PipelineFlags{} );

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
	}

	bool RenderTechnique::DoInitialise( uint32_t & p_index )
	{
		bool l_return = DoCreateSamplers();
		l_return &= DoCreateTextures();
		l_return &= DoCreateFramebuffers();

#if ENABLE_FFT

		generateWavesSpectrum();

#endif

		generateMesh();
		loadPrograms( true );

#if ENABLE_FFT

		m_variancesVtxBuffer->Upload( BufferAccessType::eStatic, BufferAccessNature::eDraw );
		m_variancesIdxBuffer->Upload( BufferAccessType::eStatic, BufferAccessNature::eDraw );
		m_variancesGBuffers = GetEngine()->GetRenderSystem()->CreateGeometryBuffers( Topology::eTriangles, *m_variances );
		m_variancesGBuffers->Initialise( { *m_variancesVtxBuffer }, m_variancesIdxBuffer );
		m_initVtxBuffer->Upload( BufferAccessType::eStatic, BufferAccessNature::eDraw );
		m_initIdxBuffer->Upload( BufferAccessType::eStatic, BufferAccessNature::eDraw );
		m_initGBuffers = GetEngine()->GetRenderSystem()->CreateGeometryBuffers( Topology::eTriangles, *m_init );
		m_initGBuffers->Initialise( { *m_initVtxBuffer }, m_initIdxBuffer );
		m_fftxVtxBuffer->Upload( BufferAccessType::eStatic, BufferAccessNature::eDraw );
		m_fftxIdxBuffer->Upload( BufferAccessType::eStatic, BufferAccessNature::eDraw );
		m_fftxGBuffers = GetEngine()->GetRenderSystem()->CreateGeometryBuffers( Topology::eTriangles, *m_fftx );
		m_fftxGBuffers->Initialise( { *m_fftxVtxBuffer }, m_fftxIdxBuffer );
		m_fftyVtxBuffer->Upload( BufferAccessType::eStatic, BufferAccessNature::eDraw );
		m_fftyIdxBuffer->Upload( BufferAccessType::eStatic, BufferAccessNature::eDraw );
		m_fftyGBuffers = GetEngine()->GetRenderSystem()->CreateGeometryBuffers( Topology::eTriangles, *m_ffty );
		m_fftyGBuffers->Initialise( { *m_fftyVtxBuffer }, m_fftyIdxBuffer );

#endif

		m_skyVtxBuffer->Upload( BufferAccessType::eStatic, BufferAccessNature::eDraw );
		m_skymapVtxBuffer->Upload( BufferAccessType::eStatic, BufferAccessNature::eDraw );
		m_cloudsVtxBuffer->Upload( BufferAccessType::eStatic, BufferAccessNature::eDraw );
		m_skyIdxBuffer->Upload( BufferAccessType::eStatic, BufferAccessNature::eDraw );
		m_skymapIdxBuffer->Upload( BufferAccessType::eStatic, BufferAccessNature::eDraw );
		m_cloudsIdxBuffer->Upload( BufferAccessType::eStatic, BufferAccessNature::eDraw );
		m_skyGBuffers = GetEngine()->GetRenderSystem()->CreateGeometryBuffers( Topology::eTriangles, *m_sky );
		m_skyGBuffers->Initialise( { *m_skyVtxBuffer }, m_skyIdxBuffer );
		m_skymapGBuffers = GetEngine()->GetRenderSystem()->CreateGeometryBuffers( Topology::eTriangles, *m_skymap );
		m_skymapGBuffers->Initialise( { *m_skymapVtxBuffer }, m_skymapIdxBuffer );
		m_cloudsGBuffers = GetEngine()->GetRenderSystem()->CreateGeometryBuffers( Topology::eTriangles, *m_clouds );
		m_cloudsGBuffers->Initialise( { *m_cloudsVtxBuffer }, m_cloudsIdxBuffer );

#if !ENABLE_FFT

		generateWaves();

#endif
		return l_return;
	}

	void RenderTechnique::DoCleanup()
	{
		DoDestroyFramebuffers();
		DoDestroyTextures();
		DoDestroySamplers();

		m_renderGBuffers.reset();
		m_skyGBuffers.reset();
		m_skymapGBuffers.reset();
		m_cloudsGBuffers.reset();
#if ENABLE_FFT
		m_variancesGBuffers.reset();
		m_initGBuffers.reset();
		m_fftxGBuffers.reset();
		m_fftyGBuffers.reset();
		DoCleanupPrograms( true );
		m_variancesFbo->Bind( FrameBufferMode::eConfig );
		m_variancesFbo->Unbind();
		m_fftFbo1->Bind( FrameBufferMode::eConfig );
		m_fftFbo1->DetachAll();
		m_fftFbo1->Unbind();
		m_fftFbo2->Bind( FrameBufferMode::eConfig );
		m_fftFbo2->DetachAll();
		m_fftFbo2->Unbind();
#endif
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
		m_frameBuffer->Bind( FrameBufferMode::eConfig );
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

	void RenderTechnique::DoRender( Size const & p_size, SceneRenderNodes & p_nodes, Camera & p_camera, uint32_t p_frameTime )
	{
		Point3f sun( sin( m_sunTheta ) * cos( m_sunPhi ), sin( m_sunTheta ) * sin( m_sunPhi ), cos( m_sunTheta ) );
		m_fbo->Bind();
		m_viewport.Resize( Size( m_skyTexSize, m_skyTexSize ) );
		m_viewport.Update();
		m_skymapSunDir->SetValue( sun );
		m_skymapOctaves->SetValue( m_octaves );
		m_skymapLacunarity->SetValue( m_lacunarity );
		m_skymapGain->SetValue( m_gain );
		m_skymapNorm->SetValue( m_norm );
		m_skymapClamp1->SetValue( m_clamp1 );
		m_skymapClamp2->SetValue( m_clamp2 );
		m_skymapCloudsColor->SetValue( bgra_float( m_cloudColor ) );
		m_skymapPipeline->SetProjectionMatrix( m_viewport.GetProjection() );
		m_skymapPipeline->Apply();
		m_skymap->Bind();
		m_skymapGBuffers->Draw( m_skymapIdxBuffer->GetSize(), 0 );
		m_skymap->Unbind();
		m_fbo->Unbind();
		m_pTexSky->Bind( SKY_UNIT );
		m_pSamplerAnisotropicClamp->Bind( SKY_UNIT );
		m_pTexSky->GenerateMipmaps();
		m_pTexSky->Unbind( SKY_UNIT );
		Image l_image( cuT( "Skymap" ), *m_pAttachSky->DownloadBuffer() );
		Image::BinaryWriter()( const_cast< const Image & >( l_image ), Path{ cuT( "Skymap.bmp" ) } );
	}

	bool RenderTechnique::DoBeginOpaqueRendering()
	{
		return true;
	}

	void RenderTechnique::DoEndOpaqueRendering()
	{
	}

	bool RenderTechnique::DoBeginTransparentRendering()
	{
		return true;
	}

	void RenderTechnique::DoEndTransparentRendering()
	{
	}

	void RenderTechnique::DoEndRender()
	{
		Point3f sun( sin( m_sunTheta ) * cos( m_sunPhi ), sin( m_sunTheta ) * sin( m_sunPhi ), cos( m_sunTheta ) );
#if ENABLE_FFT
		static double m_lastTime = 0.0;
		double t = m_animate ? time() : m_lastTime;
		simulateFFTWaves( float( t ) );
		m_lastTime = t;
#endif
		m_frameBuffer->Bind( FrameBufferMode::eAutomatic, FrameBufferTarget::eDraw );
		m_viewport.Resize( Size( m_width, m_height ) );
		m_viewport.Update();
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
		m_skyPipeline->SetProjectionMatrix( m_viewport.GetProjection() );
		m_skyPipeline->Apply();
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

		//if ( m_grid )
		//{
		//	m_renderRasteriserState.lock()->SetFillMode( eFILL_MODE_LINE );
		//}
		//else
		//{
		//	m_renderRasteriserState.lock()->SetFillMode( eFILL_MODE_SOLID );
		//}

		m_renderPipeline->Apply();
		m_render->Bind();
		m_renderGBuffers->Draw( m_renderIdxBuffer->GetSize(), 0 );
		m_render->Unbind();

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
		m_cloudsPipeline->SetProjectionMatrix( m_viewport.GetProjection() );
		m_cloudsPipeline->Apply();
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
		uint8_t * l_pData = m_pTexSpectrum_1_2->Lock( AccessType::eWrite );
		std::memcpy( l_pData, m_spectrum12, sizeof( float ) * m_FFT_SIZE * m_FFT_SIZE * 4 );
		m_pTexSpectrum_1_2->Unlock( true );
		m_pTexSpectrum_1_2->Unbind( SPECTRUM_1_2_UNIT );
		m_pTexSpectrum_3_4->Bind( SPECTRUM_3_4_UNIT );
		m_pSamplerNearestRepeat->Bind( SPECTRUM_3_4_UNIT );
		l_pData = m_pTexSpectrum_3_4->Lock( AccessType::eWrite );
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

		m_variancesFbo->Bind( FrameBufferMode::eAutomatic );
		m_viewport.Resize( Size( m_N_SLOPE_VARIANCE, m_N_SLOPE_VARIANCE ) );
		m_viewport.Update();
		m_variancesGridSizes->SetValue( Point4f( m_GRID1_SIZE, m_GRID2_SIZE, m_GRID3_SIZE, m_GRID4_SIZE ) );
		m_variancesSlopeVarianceDelta->SetValue( float( 0.5 * ( theoreticSlopeVariance - totalSlopeVariance ) ) );
		m_variancesPipeline->SetProjectionMatrix( m_viewport.GetProjection() );
		m_variancesPipeline->Apply();

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
		// init
		m_fftFbo1->Bind( FrameBufferMode::eAutomatic );
		m_fftFbo1->SetReadBuffer( AttachmentPoint::eColour, 0 );
		m_viewport.Resize( Size( m_FFT_SIZE, m_FFT_SIZE ) );
		m_viewport.Update();
		m_initFftSize->SetValue( float( m_FFT_SIZE ) );
		m_initInverseGridSizes->SetValue( Point4f( 2.0 * M_PI * m_FFT_SIZE / m_GRID1_SIZE, 2.0 * M_PI * m_FFT_SIZE / m_GRID2_SIZE, 2.0 * M_PI * m_FFT_SIZE / m_GRID3_SIZE, 2.0 * M_PI * m_FFT_SIZE / m_GRID4_SIZE ) );
		m_initT->SetValue( t );
		m_initPipeline->SetProjectionMatrix( m_viewport.GetProjection() );
		m_initPipeline->Apply();
		m_init->Bind();
		drawQuadInit();
		m_init->Unbind();
		m_fftFbo1->Unbind();
		// fft passes
		m_fftFbo2->Bind( FrameBufferMode::eManual );
		m_fftxNLayers->SetValue( m_choppy ? 5 : 3 );
		m_fftxPipeline->SetProjectionMatrix( m_viewport.GetProjection() );
		m_fftxPipeline->Apply();

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

		m_fftyPipeline->SetProjectionMatrix( m_viewport.GetProjection() );
		m_fftyPipeline->Apply();

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
		uint8_t * l_pData = m_pTexWave->Lock( AccessType::Write );
		std::memcpy( l_pData, m_pWaves, m_nbWaves * 4 * sizeof( float ) );
		m_pTexWave->Unlock( true );
		m_pTexWave->Unbind();
	}
#endif
}

//*************************************************************************************************
