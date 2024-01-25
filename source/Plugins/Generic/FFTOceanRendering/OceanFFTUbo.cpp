#include "FFTOceanRendering/OceanFFTUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBufferPool.hpp>

#include <CastorUtils/Graphics/Size.hpp>

#include <ShaderWriter/Source.hpp>

namespace ocean_fft
{
	//************************************************************************************************

	castor::MbString const OceanUbo::Buffer = "Ocean";
	castor::MbString const OceanUbo::Data = "c3d_oceanData";

	OceanUbo::OceanUbo( castor3d::RenderDevice const & device )
		: m_device{ device }
		, m_ubo{ device.uboPool->getBuffer< Configuration >( 0u ) }
	{
	}

	OceanUbo::~OceanUbo()
	{
		m_device.uboPool->putBuffer( m_ubo );
	}

	void OceanUbo::cpuUpdate( OceanUboConfiguration const & config
		, OceanFFTConfig const & fftConfig
		, castor::Point3f const & cameraPosition )
	{
		static constexpr float G = 9.81f;
		auto tileExtent = castor::Point2f{ float( fftConfig.heightMapSamples ), float( fftConfig.heightMapSamples ) };
		auto patchSizeMod = fftConfig.patchSize * tileExtent / fftConfig.size;
		auto sizeNormal = fftConfig.size / fftConfig.normalFreqMod;
		auto blockOffset = castor::Point2i{ castor::point::getRounded( castor::Point2f{ cameraPosition->x, cameraPosition->z } / patchSizeMod ) };
		blockOffset -= castor::Point2i{ fftConfig.blocksCount->x >> 1, fftConfig.blocksCount->y >> 1 };
		auto amplitude = float( fftConfig.amplitude * 0.3f / sqrt( fftConfig.heightMapSamples * fftConfig.heightMapSamples ) );

		auto & data = m_ubo.getData();
		data = config;
		data.patchSize = fftConfig.patchSize;
		data.heightMapSamples->x = fftConfig.heightMapSamples;
		data.heightMapSamples->y = fftConfig.heightMapSamples;
		data.displacementDownsample = int32_t( fftConfig.displacementDownsample );
		data.otherMod = castor::Point2f{ castor::PiMult2< float >, castor::PiMult2< float > } / fftConfig.size;
		data.normalMod = castor::Point2f{ castor::PiMult2< float >, castor::PiMult2< float > } / sizeNormal;
		data.tileScale = tileExtent / fftConfig.size;
		data.normalScale = fftConfig.normalFreqMod;
		data.blockOffset = castor::Point2f{ blockOffset };
		data.fftScale = fftConfig.size / tileExtent;
		data.maxTessLevel->x = log2( data.patchSize->x );
		data.maxTessLevel->y = data.patchSize->y;
		data.invHeightmapSize = castor::Point2f{ 1.0f, 1.0f } / fftConfig.size;
		data.distanceMod = 1.0f / fftConfig.lod0Distance;
		data.amplitude = amplitude;
		data.maxWaveLength = 2.0f / float( fftConfig.blocksCount->x );
		data.L = ( fftConfig.windVelocity * fftConfig.windVelocity ) / G;
		data.windDirection = castor::point::getNormalised( fftConfig.windDirection );
	}

	//************************************************************************************************
}
