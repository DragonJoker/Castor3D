#include "SsaoConfigUbo.hpp"

#include "Engine.hpp"
#include "Technique/Opaque/Ssao/SsaoConfig.hpp"
#include "Render/RenderInfo.hpp"
#include "Render/Viewport.hpp"
#include "Scene/Camera.hpp"

using namespace castor;

namespace castor3d
{
	String const SsaoConfigUbo::BufferSsaoConfig = cuT( "SsaoConfig" );
	String const SsaoConfigUbo::NumSamples = cuT( "c3d_numSamples" );
	String const SsaoConfigUbo::NumSpiralTurns = cuT( "c3d_numSpiralTurns" );
	String const SsaoConfigUbo::ProjScale = cuT( "c3d_projScale" );
	String const SsaoConfigUbo::Radius = cuT( "c3d_radius" );
	String const SsaoConfigUbo::InvRadius = cuT( "c3d_invRadius" );
	String const SsaoConfigUbo::Radius2 = cuT( "c3d_radius2" );
	String const SsaoConfigUbo::InvRadius2 = cuT( "c3d_invRadius2" );
	String const SsaoConfigUbo::Bias = cuT( "c3d_bias" );
	String const SsaoConfigUbo::Intensity = cuT( "c3d_intensity" );
	String const SsaoConfigUbo::IntensityDivR6 = cuT( "c3d_intensityDivR6" );
	String const SsaoConfigUbo::FarPlaneZ = cuT( "c3d_farPlaneZ" );
	String const SsaoConfigUbo::EdgeSharpness = cuT( "c3d_edgeSharpness" );
	String const SsaoConfigUbo::BlurStepSize = cuT( "c3d_blurStepSize" );
	String const SsaoConfigUbo::BlurRadius = cuT( "c3d_blurRadius" );
	String const SsaoConfigUbo::ProjInfo = cuT( "c3d_projInfo" );
	String const SsaoConfigUbo::InvViewMatrix = cuT( "c3d_worldToCamera" );

	SsaoConfigUbo::SsaoConfigUbo( Engine & engine )
		: m_ubo{ SsaoConfigUbo::BufferSsaoConfig
			, *engine.getRenderSystem()
			, SsaoConfigUbo::BindingPoint }
		, m_numSamples{ *m_ubo.createUniform< UniformType::eInt >( SsaoConfigUbo::NumSamples ) }
		, m_numSpiralTurns{ *m_ubo.createUniform< UniformType::eInt >( SsaoConfigUbo::NumSpiralTurns ) }
		, m_projScale{ *m_ubo.createUniform< UniformType::eFloat >( SsaoConfigUbo::ProjScale ) }
		, m_radius{ *m_ubo.createUniform< UniformType::eFloat >( SsaoConfigUbo::Radius ) }
		, m_invRadius{ *m_ubo.createUniform< UniformType::eFloat >( SsaoConfigUbo::InvRadius ) }
		, m_radius2{ *m_ubo.createUniform< UniformType::eFloat >( SsaoConfigUbo::Radius2 ) }
		, m_invRadius2{ *m_ubo.createUniform< UniformType::eFloat >( SsaoConfigUbo::InvRadius2 ) }
		, m_bias{ *m_ubo.createUniform< UniformType::eFloat >( SsaoConfigUbo::Bias ) }
		, m_intensity{ *m_ubo.createUniform< UniformType::eFloat >( SsaoConfigUbo::Intensity ) }
		, m_intensityDivR6{ *m_ubo.createUniform< UniformType::eFloat >( SsaoConfigUbo::IntensityDivR6 ) }
		, m_farPlaneZ{ *m_ubo.createUniform< UniformType::eFloat >( SsaoConfigUbo::FarPlaneZ ) }
		, m_edgeSharpness{ *m_ubo.createUniform< UniformType::eFloat >( SsaoConfigUbo::EdgeSharpness ) }
		, m_blurStepSize{ *m_ubo.createUniform< UniformType::eInt >( SsaoConfigUbo::BlurStepSize ) }
		, m_blurRadius{ *m_ubo.createUniform< UniformType::eInt >( SsaoConfigUbo::BlurRadius ) }
		, m_projInfo{ *m_ubo.createUniform< UniformType::eVec4f >( SsaoConfigUbo::ProjInfo ) }
		, m_invViewMatrix{ *m_ubo.createUniform< UniformType::eMat4x4f >( SsaoConfigUbo::InvViewMatrix ) }
	{
	}

	SsaoConfigUbo::~SsaoConfigUbo()
	{
	}

	Matrix4x4d getProjectUnitMatrix( Viewport const & viewport )
	{
		// Uses double precision because the division operations may otherwise 
		// significantly hurt prevision.
		double const screenWidth( viewport.getWidth() );
		double const screenHeight( viewport.getHeight() );

		double r, l, t, b, n, f, x, y;

		y = -viewport.getNear() * ( viewport.getFovY() / 2 ).tan();
		x = y * ( screenWidth / screenHeight );

		n = -viewport.getNear();
		f = -viewport.getFar();

		// Scale the pixel offset relative to the (non-square!) pixels in the unit frustum
		r = x;
		l = -x;
		t = y;
		b = -y;

		return matrix::frustum( l, r, b, t, n, f );
	}

	void SsaoConfigUbo::update( SsaoConfig const & config
		, Camera const & camera )
	{
		auto & viewport = camera.getViewport();
		int numSpiralTurns = 0;

#define NUM_PRECOMPUTED 100

		static int const minDiscrepancyArray[NUM_PRECOMPUTED]
		{
		//   0   1   2   3   4   5   6   7   8   9
			 1,  1,  1,  2,  3,  2,  5,  2,  3,  2,  // 0
			 3,  3,  5,  5,  3,  4,  7,  5,  5,  7,  // 1
			 9,  8,  5,  5,  7,  7,  7,  8,  5,  8,  // 2
			11, 12,  7, 10, 13,  8, 11,  8,  7, 14,  // 3
			11, 11, 13, 12, 13, 19, 17, 13, 11, 18,  // 4
			19, 11, 11, 14, 17, 21, 15, 16, 17, 18,  // 5
			13, 17, 11, 17, 19, 18, 25, 18, 19, 19,  // 6
			29, 21, 19, 27, 31, 29, 21, 18, 17, 29,  // 7
			31, 31, 23, 18, 25, 26, 25, 23, 19, 34,  // 8
			19, 27, 21, 25, 39, 29, 17, 21, 27, 29   // 9
		};

		if ( config.m_numSamples < NUM_PRECOMPUTED )
		{
			numSpiralTurns =  minDiscrepancyArray[config.m_numSamples];
		}
		else
		{
			numSpiralTurns = 5779; // Some large prime. Hope it does alright. It'll at least never degenerate into a perfect line until we have 5779 samples...
		}

#undef NUM_PRECOMPUTED

		float const scale = std::abs( 2.0f * ( viewport.getFovY() * 0.5f ).tan() );
		float const radius = config.m_radius * 100.0f;
		float const invRadius = 1 / radius;
		float const radius2 = radius * radius;
		float const invRadius2 = 1.0f / radius2;
		float const intersityDivR6 = config.m_intensity / std::pow( radius, 6.0f );
		float const projScale = std::abs( viewport.getHeight() / scale );
		float const MIN_AO_SS_RADIUS = 100.0f;
		// Second parameter of max is just solving for Z coordinate at which we hit MIN_AO_SS_RADIUS
		float farZ = std::max( viewport.getFar(), -projScale * radius / MIN_AO_SS_RADIUS );
		// Hack because setting farZ lower results in banding artefacts on some scenes, should tune later.
		farZ = std::min( farZ, -1000.0f );
		auto const proj = getProjectUnitMatrix( viewport );

		m_numSamples.setValue( config.m_numSamples );
		m_numSpiralTurns.setValue( numSpiralTurns );
		m_projScale.setValue( projScale );
		m_radius.setValue( radius );
		m_invRadius.setValue( invRadius );
		m_radius2.setValue( radius2 );
		m_invRadius2.setValue( invRadius2 );
		m_bias.setValue( config.m_bias );
		m_intensity.setValue( config.m_intensity );
		m_intensityDivR6.setValue( intersityDivR6 );
		m_farPlaneZ.setValue( farZ );
		m_edgeSharpness.setValue( config.m_edgeSharpness );
		m_blurStepSize.setValue( config.m_blurStepSize );
		m_blurRadius.setValue( config.m_blurRadius );
		m_projInfo.setValue( Point4f
		{
			-2.0f / ( viewport.getWidth() * proj[0][0] ),
			-2.0f / ( viewport.getHeight() * proj[1][1] ),
			( 1.0f - proj[0][2] ) / proj[0][0],
			( 1.0f - proj[1][2] ) / proj[1][1]
		} );
		m_invViewMatrix.setValue( camera.getView() );

		m_ubo.update();
		m_ubo.bindTo( SsaoConfigUbo::BindingPoint );
	}
}
