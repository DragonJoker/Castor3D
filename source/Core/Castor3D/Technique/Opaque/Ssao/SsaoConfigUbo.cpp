#include "Castor3D/Technique/Opaque/Ssao/SsaoConfigUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Viewport.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Technique/Opaque/Ssao/SsaoConfig.hpp"

#include <Ashes/Buffer/UniformBuffer.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		castor::Matrix4x4f getProjectUnitMatrix( ashes::Device const & device
			, Viewport const & viewport )
		{
			// Uses double precision because the division operations may otherwise 
			// significantly hurt prevision.
			double const screenWidth( viewport.getWidth() );
			double const screenHeight( viewport.getHeight() );

			double y = double( -viewport.getNear() ) * ( viewport.getFovY() / 2 ).tan();
			double x = y * ( screenWidth / screenHeight );

			float n = -viewport.getNear();
			float f = -viewport.getFar();

			// Scale the pixel offset relative to the (non-square!) pixels in the unit frustum
			auto r = float( x );
			auto l = float( -x );
			auto t = float( y );
			auto b = float( -y );

			return convert( device.frustum( l, r, b, t, n, f ) );
		}
	}

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
	String const SsaoConfigUbo::ViewMatrix = cuT( "c3d_viewMatrix" );

	SsaoConfigUbo::SsaoConfigUbo( Engine & engine )
		: m_engine{ engine }
	{
		if ( m_engine.getRenderSystem()->hasCurrentDevice() )
		{
			initialise();
		}
	}

	SsaoConfigUbo::~SsaoConfigUbo()
	{
		if ( m_engine.getRenderSystem()->hasCurrentDevice() )
		{
			cleanup();
		}
	}

	void SsaoConfigUbo::initialise()
	{
		auto & device = getCurrentDevice( m_engine );
		m_ubo = ashes::makeUniformBuffer< Configuration >( device
			, 1u
			, ashes::BufferTarget::eTransferDst
			, ashes::MemoryPropertyFlag::eHostVisible );
	}

	void SsaoConfigUbo::cleanup()
	{
		m_ubo.reset();
	}

	void SsaoConfigUbo::update( SsaoConfig const & config
		, Camera const & camera )
	{
		auto & device = getCurrentDevice( m_engine );
		auto & viewport = camera.getViewport();
		int numSpiralTurns = 0;

		static constexpr uint32_t numPrecomputed = 100u;

		static int const minDiscrepancyArray[numPrecomputed]
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

		if ( config.numSamples < numPrecomputed )
		{
			numSpiralTurns =  minDiscrepancyArray[config.numSamples];
		}
		else
		{
			numSpiralTurns = 5779; // Some large prime. Hope it does alright. It'll at least never degenerate into a perfect line until we have 5779 samples...
		}

		float const radius = config.radius;
		float const invRadius = 1 / radius;
		float const radius2 = radius * radius;
		float const invRadius2 = 1.0f / radius2;
		float const intersityDivR6 = config.intensity / std::pow( radius, 6.0f );
		float const projScale = viewport.getProjectionScale();
		float const MIN_AO_SS_RADIUS = 1.0f;
		// Second parameter of max is just solving for Z coordinate at which we hit MIN_AO_SS_RADIUS
		float farZ = std::max( viewport.getFar(), -projScale * radius / MIN_AO_SS_RADIUS );
		// Hack because setting farZ lower results in banding artefacts on some scenes, should tune later.
		farZ = std::min( farZ, -1000.0f );
		auto const proj = getProjectUnitMatrix( device, viewport );

		auto & configuration = m_ubo->getData( 0u );
		configuration.numSamples = config.numSamples;
		configuration.numSpiralTurns = numSpiralTurns;
		configuration.projScale = projScale;
		configuration.radius = radius;
		configuration.invRadius = invRadius;
		configuration.radius2 = radius2;
		configuration.invRadius2 = invRadius2;
		configuration.bias = config.bias;
		configuration.intensity = config.intensity;
		configuration.intensityDivR6 = intersityDivR6;
		configuration.farPlaneZ = farZ;
		configuration.edgeSharpness = config.edgeSharpness;
		configuration.blurStepSize = config.blurStepSize;
		configuration.blurRadius = config.blurRadius.value().value();
		configuration.projInfo = castor::Point4f
		{
			-2.0f / ( viewport.getWidth() * proj[0][0] ),
			-2.0f / ( viewport.getHeight() * proj[1][1] ),
			( 1.0f - proj[0][2] ) / proj[0][0],
			( 1.0f - proj[1][2] ) / proj[1][1]
		};
		configuration.viewMatrix = camera.getView();

		m_ubo->upload();
	}
}
