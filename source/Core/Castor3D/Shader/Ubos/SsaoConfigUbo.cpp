#include "Castor3D/Shader/Ubos/SsaoConfigUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Render/Ssao/SsaoConfig.hpp"

#include <ShaderWriter/Source.hpp>

namespace c3d
{
	//*********************************************************************************************

	namespace shader::ssao
	{
		static uint32_t constexpr numPrecomputed = 100u;

		static Array< int, numPrecomputed > constexpr minDiscrepancyArray
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
	}

	//*********************************************************************************************

	SsaoConfigUbo::SsaoConfigUbo( RenderDevice const & device )
		: m_device{ device }
		, m_ubo{ m_device.uboPool->getBuffer< Configuration >( MemoryPropertyFlags::eNone ) }
	{
	}

	SsaoConfigUbo::~SsaoConfigUbo()noexcept
	{
		m_device.uboPool->putBuffer( m_ubo );
	}

	void SsaoConfigUbo::cpuUpdate( SsaoConfig const & config
		, Camera const & camera
		, Size const & renderSize )
	{
		int numSpiralTurns = 0;

		if ( config.numSamples < shader::ssao::numPrecomputed )
		{
			numSpiralTurns = shader::ssao::minDiscrepancyArray[config.numSamples];
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

		auto & configuration = m_ubo.getData();
		float const projScale = camera.getProjectionScale( renderSize );
		float const MIN_AO_SS_RADIUS = 1.0f;
		// Second parameter of max is just solving for Z coordinate at which we hit MIN_AO_SS_RADIUS
		float farZ = std::max( camera.getFar(), -projScale * radius / MIN_AO_SS_RADIUS );
		// Hack because setting farZ lower results in banding artefacts on some scenes, should tune later.
		farZ = std::min( farZ, -1000.0f );
		auto const & proj = camera.getProjection( renderSize, true );
		configuration.projInfo = Point4f
		{
			-2.0f / ( float( renderSize.getWidth() ) * proj[0][0] ),
			-2.0f / ( float( renderSize.getHeight() ) * proj[1][1] ),
			( 1.0f - proj[0][2] ) / proj[0][0],
			( 1.0f - proj[1][2] ) / proj[1][1]
		};
		configuration.numSamples = int32_t( config.numSamples );
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
		configuration.blurStepSize = config.blurStepSize.value().value();
		configuration.blurRadius = config.blurRadius.value().value();
		configuration.blurHighQuality = int32_t( config.blurHighQuality ? 1u : 0u );
		configuration.highQuality = int32_t( config.highQuality ? 1u : 0u );
		configuration.logMaxOffset = config.logMaxOffset.value();
		configuration.maxMipLevel = config.maxMipLevel;
		configuration.minRadius = config.minRadius;
		configuration.variation = config.variation;
		configuration.bendStepCount = config.bendStepCount.value();
		configuration.bendStepSize = config.bendStepSize;
	}

	//*********************************************************************************************
}
