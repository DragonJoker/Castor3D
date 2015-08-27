#ifndef ___C3D_OCEAN_LIGHTING_TECHNIQUE_H___
#define ___C3D_OCEAN_LIGHTING_TECHNIQUE_H___

#if defined( _MSC_VER )
#	pragma warning( push )
#	pragma warning( disable:4311 )
#	pragma warning( disable:4312 )
#endif

#include <Castor3DPrerequisites.hpp>
#include <Buffer.hpp>
#include <Context.hpp>
#include <Engine.hpp>
#include <FrameBuffer.hpp>
#include <Pipeline.hpp>
#include <RenderTarget.hpp>
#include <RenderTechnique.hpp>
#include <RenderWindow.hpp>
#include <Sampler.hpp>
#include <ShaderManager.hpp>
#include <ShaderProgram.hpp>
#include <Texture.hpp>
#include <Vertex.hpp>
#include <BlendState.hpp>

#if defined( _MSC_VER )
#	pragma warning( pop )
#endif

#define ENABLE_FFT	0

namespace OceanLighting
{
	class RenderTechnique : public Castor3D::RenderTechniqueBase
	{
	private:
		//!\~english The attach between colour buffer and frame buffer	\~french L'attache entre le buffer couleur et le frame buffer
		Castor3D::TextureAttachmentSPtr					m_pColorAttach;
		//!\~english The attach between depth buffer and frame buffer	\~french L'attache entre le buffer profondeur et le frame buffer
		Castor3D::RenderBufferAttachmentSPtr			m_pDepthAttach;

	private:
		int												m_width;
		int												m_height;

		Castor3D::ShaderProgramBaseSPtr					m_render;
		Castor3D::OneTextureFrameVariableSPtr			m_renderSkyIrradianceSampler;
		Castor3D::OneTextureFrameVariableSPtr			m_renderInscatterSampler;
		Castor3D::OneTextureFrameVariableSPtr			m_renderTransmittanceSampler;
		Castor3D::OneTextureFrameVariableSPtr			m_renderSkySampler;
		Castor3D::Matrix4x4fFrameVariableSPtr			m_renderScreenToCamera;
		Castor3D::Matrix4x4fFrameVariableSPtr			m_renderCameraToWorld;
		Castor3D::Matrix4x4fFrameVariableSPtr			m_renderWorldToScreen;
		Castor3D::Point3fFrameVariableSPtr				m_renderWorldCamera;
		Castor3D::Point3fFrameVariableSPtr				m_renderWorldSunDir;
		Castor3D::OneFloatFrameVariableSPtr				m_renderHdrExposure;
		Castor3D::Point3fFrameVariableSPtr				m_renderSeaColor;
#if ENABLE_FFT
		Castor3D::OneTextureFrameVariableSPtr			m_renderSpectrum_1_2_Sampler;
		Castor3D::OneTextureFrameVariableSPtr			m_renderSpectrum_3_4_Sampler;
		Castor3D::OneTextureFrameVariableSPtr			m_renderFftWavesSampler;
		Castor3D::OneTextureFrameVariableSPtr			m_renderSlopeVarianceSampler;
		Castor3D::Point4fFrameVariableSPtr				m_renderGridSizes;
		Castor3D::Point2fFrameVariableSPtr				m_renderGridSize;
		Castor3D::OneFloatFrameVariableSPtr				m_renderChoppy;
#else
		Castor3D::OneTextureFrameVariableSPtr			m_renderWavesSampler;
		Castor3D::Matrix2x2fFrameVariableSPtr			m_renderWorldToWind;
		Castor3D::Matrix2x2fFrameVariableSPtr			m_renderWindToWorld;
		Castor3D::OneFloatFrameVariableSPtr				m_renderNbWaves;
		Castor3D::OneFloatFrameVariableSPtr				m_renderHeightOffset;
		Castor3D::Point2fFrameVariableSPtr				m_renderSigmaSqTotal;
		Castor3D::OneFloatFrameVariableSPtr				m_renderTime;
		Castor3D::Point4fFrameVariableSPtr				m_renderLods;
		Castor3D::OneFloatFrameVariableSPtr				m_renderNyquistMin;
		Castor3D::OneFloatFrameVariableSPtr				m_renderNyquistMax;
#endif
		Castor3D::GeometryBuffersSPtr					m_renderGBuffers;
		Castor3D::BlendStateWPtr						m_renderBlendState;
		Castor3D::RasteriserStateWPtr					m_renderRasteriserState;

		Castor3D::ShaderProgramBaseSPtr					m_sky;
		Castor3D::OneTextureFrameVariableSPtr			m_skySkyIrradianceSampler;
		Castor3D::OneTextureFrameVariableSPtr			m_skyInscatterSampler;
		Castor3D::OneTextureFrameVariableSPtr			m_skyTransmittanceSampler;
		Castor3D::OneTextureFrameVariableSPtr			m_skySkySampler;
		Castor3D::Matrix4x4fFrameVariableSPtr			m_skyScreenToCamera;
		Castor3D::Matrix4x4fFrameVariableSPtr			m_skyCameraToWorld;
		Castor3D::Point3fFrameVariableSPtr				m_skyWorldCamera;
		Castor3D::Point3fFrameVariableSPtr				m_skyWorldSunDir;
		Castor3D::OneFloatFrameVariableSPtr				m_skyHdrExposure;
		Castor3D::GeometryBuffersSPtr					m_skyGBuffers;
		Castor3D::BlendStateWPtr						m_skyBlendState;

		Castor3D::ShaderProgramBaseSPtr					m_skymap;
		Castor3D::OneTextureFrameVariableSPtr			m_skymapSkyIrradianceSampler;
		Castor3D::OneTextureFrameVariableSPtr			m_skymapInscatterSampler;
		Castor3D::OneTextureFrameVariableSPtr			m_skymapTransmittanceSampler;
		Castor3D::OneTextureFrameVariableSPtr			m_skymapNoiseSampler;
		Castor3D::Point3fFrameVariableSPtr				m_skymapSunDir;
		Castor3D::OneFloatFrameVariableSPtr				m_skymapOctaves;
		Castor3D::OneFloatFrameVariableSPtr				m_skymapLacunarity;
		Castor3D::OneFloatFrameVariableSPtr				m_skymapGain;
		Castor3D::OneFloatFrameVariableSPtr				m_skymapNorm;
		Castor3D::OneFloatFrameVariableSPtr				m_skymapClamp1;
		Castor3D::OneFloatFrameVariableSPtr				m_skymapClamp2;
		Castor3D::Point4fFrameVariableSPtr				m_skymapCloudsColor;
		Castor3D::GeometryBuffersSPtr					m_skymapGBuffers;
		Castor3D::BlendStateWPtr						m_skymapBlendState;

		Castor3D::ShaderProgramBaseSPtr					m_clouds;
		Castor3D::Matrix4x4fFrameVariableSPtr			m_cloudsWorldToScreen;
		Castor3D::Point3fFrameVariableSPtr				m_cloudsWorldCamera;
		Castor3D::Point3fFrameVariableSPtr				m_cloudsWorldSunDir;
		Castor3D::OneFloatFrameVariableSPtr				m_cloudsHdrExposure;
		Castor3D::OneFloatFrameVariableSPtr				m_cloudsOctaves;
		Castor3D::OneFloatFrameVariableSPtr				m_cloudsLacunarity;
		Castor3D::OneFloatFrameVariableSPtr				m_cloudsGain;
		Castor3D::OneFloatFrameVariableSPtr				m_cloudsNorm;
		Castor3D::OneFloatFrameVariableSPtr				m_cloudsClamp1;
		Castor3D::OneFloatFrameVariableSPtr				m_cloudsClamp2;
		Castor3D::Point4fFrameVariableSPtr				m_cloudsCloudsColor;
		Castor3D::OneTextureFrameVariableSPtr			m_cloudsSkyIrradianceSampler;
		Castor3D::OneTextureFrameVariableSPtr			m_cloudsInscatterSampler;
		Castor3D::OneTextureFrameVariableSPtr			m_cloudsTransmittanceSampler;
		Castor3D::OneTextureFrameVariableSPtr			m_cloudsNoiseSampler;
		Castor3D::GeometryBuffersSPtr					m_cloudsGBuffers;
		Castor3D::BlendStateWPtr						m_cloudsBlendState;

		uint32_t										m_skyTexSize;
		bool											m_cloudLayer;
		float											m_octaves;
		float											m_lacunarity;
		float											m_gain;
		float											m_norm;
		float											m_clamp1;
		float											m_clamp2;
		Castor::Point4f									m_cloudColor;
		Castor3D::FrameBufferSPtr						m_fbo;
		Castor3D::TextureAttachmentSPtr					m_pAttachSky;
		Castor::Point4f									m_vboParams;
		float											m_sunTheta;
		float											m_sunPhi;
		float											m_cameraHeight;
		float											m_cameraTheta;
		float											m_cameraPhi;

		// RENDERING OPTIONS
		float											m_gridSize;
		Castor::Point4f									m_seaColor;
		float											m_hdrExposure;
		bool											m_grid;
		bool											m_animate;
		bool											m_seaContrib;
		bool											m_sunContrib;
		bool											m_skyContrib;
		bool											m_manualFilter;
#if ENABLE_FFT
		bool											m_choppy;
		// WAVES SPECTRUM
		// using "A unified directional spectrum for long and short wind-driven waves"
		// T. Elfouhaily, B. Chapron, K. Katsaros, D. Vandemark
		// Journal of Geophysical Research vol 102, p781-796, 1997
		const int										m_N_SLOPE_VARIANCE;	// size of the 3d texture containing precomputed filtered slope m_variances
		const float										m_cm;				// Eq 59
		const float										m_km;				// Eq 59
		float											m_GRID1_SIZE;		// size in meters (i.e. in spatial domain) of the first m_grid
		float											m_GRID2_SIZE;		// size in meters (i.e. in spatial domain) of the second m_grid
		float											m_GRID3_SIZE;		// size in meters (i.e. in spatial domain) of the third m_grid
		float											m_GRID4_SIZE;		// size in meters (i.e. in spatial domain) of the fourth m_grid
		float											m_WIND;				// wind speed in meters per second (at 10m above surface)
		float											m_OMEGA;			// sea state (inverse wave age)
		float											m_A;				// wave amplitude factor (should be one)
		// FFT WAVES
		const int										m_PASSES;			// number of passes needed for the FFT 6 -> 64, 7 -> 128, 8 -> 256, etc
		const int										m_FFT_SIZE;			// size of the textures storing the waves in frequency and spatial domains
		float 					*						m_spectrum12;
		float 					*						m_spectrum34;

		Castor3D::FrameBufferSPtr						m_fftFbo1;
		std::vector< Castor3D::TextureAttachmentSPtr >	m_arrayFftAttaches;

		Castor3D::FrameBufferSPtr						m_fftFbo2;
		Castor3D::DynamicTextureSPtr					m_pTexFFTA;
		Castor3D::DynamicTextureSPtr					m_pTexFFTB;
		Castor3D::TextureAttachmentSPtr					m_pAttachFftA;
		Castor3D::TextureAttachmentSPtr					m_pAttachFftB;

		Castor3D::FrameBufferSPtr						m_variancesFbo;
		std::vector< Castor3D::TextureAttachmentSPtr >	m_arrayVarianceAttaches;

		Castor3D::ShaderProgramBaseSPtr					m_init;
		Castor3D::OneTextureFrameVariableSPtr			m_initSpectrum_1_2_Sampler;
		Castor3D::OneTextureFrameVariableSPtr			m_initSpectrum_3_4_Sampler;
		Castor3D::OneFloatFrameVariableSPtr				m_initFftSize;
		Castor3D::Point4fFrameVariableSPtr				m_initInverseGridSizes;
		Castor3D::OneFloatFrameVariableSPtr				m_initT;
		Castor3D::GeometryBuffersSPtr					m_initGBuffers;
		Castor3D::BlendStateWPtr						m_initBlendState;

		Castor3D::ShaderProgramBaseSPtr					m_variances;
		Castor3D::OneTextureFrameVariableSPtr			m_variancesSpectrum_1_2_Sampler;
		Castor3D::OneTextureFrameVariableSPtr			m_variancesSpectrum_3_4_Sampler;
		Castor3D::OneFloatFrameVariableSPtr				m_variancesNSlopeVariance;
		Castor3D::OneIntFrameVariableSPtr				m_variancesFFTSize;
		Castor3D::Point4fFrameVariableSPtr				m_variancesGridSizes;
		Castor3D::OneFloatFrameVariableSPtr				m_variancesSlopeVarianceDelta;
		Castor3D::OneFloatFrameVariableSPtr				m_variancesC;
		Castor3D::GeometryBuffersSPtr					m_variancesGBuffers;
		Castor3D::BlendStateWPtr						m_variancesBlendState;

		Castor3D::ShaderProgramBaseSPtr					m_fftx;
		Castor3D::OneTextureFrameVariableSPtr			m_fftxButterflySampler;
		Castor3D::OneIntFrameVariableSPtr				m_fftxNLayers;
		Castor3D::OneFloatFrameVariableSPtr				m_fftxPass;
		Castor3D::OneTextureFrameVariableSPtr			m_fftxImgSampler;
		Castor3D::GeometryBuffersSPtr					m_fftxGBuffers;
		Castor3D::BlendStateWPtr						m_fftxBlendState;

		Castor3D::ShaderProgramBaseSPtr					m_ffty;
		Castor3D::OneTextureFrameVariableSPtr			m_fftyButterflySampler;
		Castor3D::OneIntFrameVariableSPtr				m_fftyNLayers;
		Castor3D::OneFloatFrameVariableSPtr				m_fftyPass;
		Castor3D::OneTextureFrameVariableSPtr			m_fftyImgSampler;
		Castor3D::GeometryBuffersSPtr					m_fftyGBuffers;
		Castor3D::BlendStateWPtr						m_fftyBlendState;

		Castor3D::DynamicTextureSPtr					m_pTexSpectrum_1_2;
		Castor3D::DynamicTextureSPtr					m_pTexSpectrum_3_4;
		Castor3D::DynamicTextureSPtr					m_pTexSlopeVariance;	// the 3d texture containing precomputed filtered slope m_variances
		Castor3D::DynamicTextureSPtr					m_pTexButterfly;
#else
		float											m_nyquistMin;
		float											m_nyquistMax;

		// WAVES PARAMETERS (INPUT)
		Castor3D::DynamicTextureSPtr					m_pTexWave;
		int												m_nbWaves;
		Castor::Coords4f 				*				m_waves;
		float 					*						m_pWaves;
		float											m_lambdaMin;
		float											m_lambdaMax;
		float											m_heightMax;
		float											m_waveDirection;
		float											m_U0;
		float											m_waveDispersion;

		// WAVE STATISTICS (OUTPUT)
		float											m_sigmaXsq;
		float											m_sigmaYsq;
		float											m_meanHeight;
		float											m_heightVariance;
		float											m_amplitudeMax;
#endif
		Castor3D::SamplerSPtr							m_pSamplerNearestClamp;
		Castor3D::SamplerSPtr							m_pSamplerNearestRepeat;
		Castor3D::SamplerSPtr							m_pSamplerLinearClamp;
		Castor3D::SamplerSPtr							m_pSamplerLinearRepeat;
		Castor3D::SamplerSPtr							m_pSamplerAnisotropicClamp;
		Castor3D::SamplerSPtr							m_pSamplerAnisotropicRepeat;

		Castor3D::StaticTextureSPtr						m_pTexIrradiance;
		Castor3D::StaticTextureSPtr						m_pTexInscatter;
		Castor3D::StaticTextureSPtr						m_pTexTransmittance;
		Castor3D::DynamicTextureSPtr					m_pTexSky;
		Castor3D::DynamicTextureSPtr					m_pTexNoise;

		bool											m_bReloadPrograms;
		bool											m_bLayer;
		bool											m_bComputeSlopeVarianceTex;
		bool											m_bGenerateWavesSpectrum;
		bool											m_bGenerateMesh;
		bool											m_bGenerateWaves;

		Castor3D::RasteriserStateWPtr					m_pRasteriserState;
		Castor3D::DepthStencilStateWPtr					m_pDepthStencilState;

	private:
		RenderTechnique & operator =( RenderTechnique const & )
		{
			return *this;
		}

	protected:
		friend class Castor3D::TechniqueFactory;

		RenderTechnique();
		RenderTechnique( Castor3D::RenderTarget & p_renderTarget, Castor3D::RenderSystem * p_pRenderSystem, Castor3D::Parameters const & p_params );

		virtual bool Render( Castor3D::Scene & p_scene, Castor3D::Camera & p_camera, Castor3D::eTOPOLOGY p_ePrimitives, double p_dFrameTime );

	public:
		virtual ~RenderTechnique();

		virtual Castor3D::RenderTechniqueBaseSPtr Clone( Castor3D::RenderTarget & p_renderTarget, Castor3D::RenderSystem * p_pRenderSystem, Castor3D::Parameters const & p_params );

		inline void SetReloadPrograms( bool p_bLayer )
		{
			m_bReloadPrograms = true;
			m_bLayer = p_bLayer;
		}
		inline void SetWidth( int p_width )
		{
			m_width = p_width;
		}
		inline void SetHeight( int p_height )
		{
			m_height = p_height;
		}
		inline void SetComputeSlopeVarianceTex()
		{
			m_bComputeSlopeVarianceTex = true;
		}
		inline void SetGenerateWavesSpectrum()
		{
			m_bGenerateWavesSpectrum = true;
		}
		inline void Resize( int p_width, int p_height )
		{
			m_bGenerateMesh = true;
			m_width = p_width;
			m_height = p_height;
		}
		inline void CameraThetaPlus( float p_fVal )
		{
			m_cameraTheta = std::min( m_cameraTheta + p_fVal, 90.0f - 0.001f );
		}
		inline void CameraThetaMinus( float p_fVal )
		{
			m_cameraTheta = m_cameraTheta - p_fVal;
		}
		inline void UpdateCameraPhi( float p_fVal )
		{
			m_cameraPhi += p_fVal;
		}
		inline void CameraHeightPlus( float p_fVal )
		{
			std::min( 8000.0f, m_cameraHeight * p_fVal );
		}
		inline void CameraHeightMinus( float p_fVal )
		{
			std::max( 0.5f, m_cameraHeight / p_fVal );
		}
		inline void UpdateSunPhi( int p_offset )
		{
			m_sunPhi += p_offset / 400.0f;
		}
		inline void UpdateSunTheta( int p_offset )
		{
			m_sunTheta += p_offset / 400.0f;
		}

#if ENABLE_FFT
		inline float & GetWindSpeed()
		{
			return m_WIND;
		}
		inline float const & GetWindSpeed()const
		{
			return m_WIND;
		}
		inline float & GetOmega()
		{
			return m_OMEGA;
		}
		inline float const & GetOmega()const
		{
			return m_OMEGA;
		}
		inline float & GetA()
		{
			return m_A;
		}
		inline float const & GetA()const
		{
			return m_A;
		}
		inline bool & GetChoppy()
		{
			return m_choppy;
		}
		inline bool const & GetChoppy()const
		{
			return m_choppy;
		}
#else
#endif
		inline float & GetCameraHeight()
		{
			return m_cameraHeight;
		}
		inline float const & GetCameraHeight()const
		{
			return m_cameraHeight;
		}
		inline float & GetCameraPhi()
		{
			return m_cameraPhi;
		}
		inline float const & GetCameraPhi()const
		{
			return m_cameraPhi;
		}
		inline float & GetCameraTheta()
		{
			return m_cameraTheta;
		}
		inline float const & GetCameraTheta()const
		{
			return m_cameraTheta;
		}
		inline float & GetGridSize()
		{
			return m_gridSize;
		}
		inline float const & GetGridSize()const
		{
			return m_gridSize;
		}
		inline Castor::Point4f & GetSeaColour()
		{
			return m_seaColor;
		}
		inline Castor::Point4f const & GetSeaColour()const
		{
			return m_seaColor;
		}
		inline float & GetHdrExposure()
		{
			return m_hdrExposure;
		}
		inline float const & GetHdrExposure()const
		{
			return m_hdrExposure;
		}
		inline bool & GetAnimate()
		{
			return m_animate;
		}
		inline bool const & GetAnimate()const
		{
			return m_animate;
		}
		inline bool & GetGrid()
		{
			return m_grid;
		}
		inline bool const & GetGrid()const
		{
			return m_grid;
		}
		inline bool & GetSea()
		{
			return m_seaContrib;
		}
		inline bool const & GetSea()const
		{
			return m_seaContrib;
		}
		inline bool & GetSun()
		{
			return m_sunContrib;
		}
		inline bool const & GetSun()const
		{
			return m_sunContrib;
		}
		inline bool & GetSky()
		{
			return m_skyContrib;
		}
		inline bool const & GetSky()const
		{
			return m_skyContrib;
		}
		inline bool & GetManualFilter()
		{
			return m_manualFilter;
		}
		inline bool const & GetManualFilter()const
		{
			return m_manualFilter;
		}
		inline bool & GetClouds()
		{
			return m_cloudLayer;
		}
		inline bool const & GetClouds()const
		{
			return m_cloudLayer;
		}
		inline Castor::Point4f & GetColour()
		{
			return m_cloudColor;
		}
		inline Castor::Point4f const & GetColour()const
		{
			return m_cloudColor;
		}
		inline float & GetOctaves()
		{
			return m_octaves;
		}
		inline float const & GetOctaves()const
		{
			return m_octaves;
		}
		inline float & GetLacunarity()
		{
			return m_lacunarity;
		}
		inline float const & GetLacunarity()const
		{
			return m_lacunarity;
		}
		inline float & GetGain()
		{
			return m_gain;
		}
		inline float const & GetGain()const
		{
			return m_gain;
		}
		inline float & GetNorm()
		{
			return m_norm;
		}
		inline float const & GetNorm()const
		{
			return m_norm;
		}
		inline float & GetClamp1()
		{
			return m_clamp1;
		}
		inline float const & GetClamp1()const
		{
			return m_clamp1;
		}
		inline float & GetClamp2()
		{
			return m_clamp2;
		}
		inline float const & GetClamp2()const
		{
			return m_clamp2;
		}

	protected:
		virtual bool DoCreate();
		virtual void DoDestroy();
		virtual bool DoInitialise();
		virtual void DoCleanup();
		virtual bool DoBeginRender();
		virtual void DoEndRender();

	private:
		void DoCleanupPrograms( bool all );
		void DoDestroyPrograms( bool all );
		void DoDeletePrograms( bool all );
#if ENABLE_FFT
		float sqr( float x );
		double sqr( double x );
		float omega( float k );
		// 1/kx and 1/ky in meters
		float spectrum( float kx, float ky, bool omnispectrum = false );
		void drawQuadInit();
		void drawQuadVariances();
		void drawQuadFFTx();
		void drawQuadFFTy();
#endif
		// ----------------------------------------------------------------------------
		// MESH GENERATION
		// ----------------------------------------------------------------------------
		void generateMesh();
		// ----------------------------------------------------------------------------
		// PROGRAMS RELOAD
		// ----------------------------------------------------------------------------
		void loadPrograms( bool all );

		// ----------------------------------------------------------------------------
		// CLOUDS
		// ---------------------------------------------------------------------------
		void drawClouds( const Castor::Point3f & sun, const Castor::Matrix4x4f & mat );

		// ----------------------------------------------------------------------------
		// WAVES SPECTRUM GENERATION
		// ----------------------------------------------------------------------------
		long lrandom( long * seed );
		float frandom( long * seed );
		float grandom( float mean, float stdDeviation, long * seed );
#if ENABLE_FFT
		void getSpectrumSample( int i, int j, float lengthScale, float kMin, float * result );
		// generates the waves spectrum
		void generateWavesSpectrum();
		float getSlopeVariance( float kx, float ky, float * spectrumSample );
		// precomputes filtered slope variances in a 3d texture, based on the wave spectrum
		void computeSlopeVarianceTex( void * unused );
		// ----------------------------------------------------------------------------
		// WAVES GENERATION AND ANIMATION (using FFT on GPU)
		// ----------------------------------------------------------------------------
		int bitReverse( int i, int N );
		void computeWeight( int N, int k, float & Wr, float & Wi );
		float * computeButterflyLookupTexture();
		void simulateFFTWaves( float t );
#else
		void generateWaves();
#endif
	};
}

#endif
