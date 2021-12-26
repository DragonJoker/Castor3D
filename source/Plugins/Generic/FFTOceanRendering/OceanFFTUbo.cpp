#include "FFTOceanRendering/OceanFFTUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBufferPools.hpp>

#include <CastorUtils/Graphics/Size.hpp>

#include <ShaderWriter/Source.hpp>

namespace ocean_fft
{
	//*********************************************************************************************

#if Ocean_Debug
	castor::StringArray const & getOceanDisplayDataNames()
	{
		static castor::StringArray const result{ "Result"
			, "GradJacobian"
			, "NoiseGradient"
			, "Jacobian"
			, "Turbulence"
			, "Normal"
			, "ColorMod"
			, "MatSpecular"
			, "LightDiffuse"
			, "LightSpecular"
			, "IndirectOcclusion"
			, "LightIndirectDiffuse"
			, "LightIndirectSpecular"
			, "IndirectAmbient"
			, "IndirectDiffuse"
			, "RawBackgroundReflection"
			, "FresnelBackgroundReflection"
			, "SSRResult"
			, "SSRFactor"
			, "SSRResultFactor"
			, "CombinedReflection"
			, "RawRefraction"
			, "LightAbsorbtion"
			, "WaterTransmission"
			, "RefractionResult"
			, "DepthSoftenedAlpha"
			, "HeightMixedRefraction"
			, "DistanceMixedRefraction"
			, "FresnelFactor"
			, "FinalReflection"
			, "FinalRefraction" };
		return result;
	}
#endif

	//*********************************************************************************************

	OceanData::OceanData( sdw::ShaderWriter & writer
		, ast::expr::ExprPtr expr
		, bool enabled )
		: sdw::StructInstance{ writer, std::move( expr ), enabled }
		, m_base{ getMember< sdw::Vec4 >( "base" ) }
		, m_refraction{ getMember< sdw::Vec4 >( "refraction" ) }
		, ssrSettings{ getMember< sdw::Vec4 >( "ssrSettings" ) }
		, m_mods{ getMember< sdw::Vec4 >( "mods" ) }
		, m_tileNormalScales{ getMember< sdw::Vec4 >( "tileNormalScales" ) }
		, m_blockOffFftScale{ getMember< sdw::Vec4 >( "blockOffFftScale" ) }
		, m_patchSizes{ getMember< sdw::Vec4 >( "patchSizes" ) }
		, m_fftSizes{ getMember< sdw::Vec4 >( "fftSizes" ) }
		, m_fftDistrib{ getMember< sdw::Vec4 >( "fftDistrib" ) }
		, m_size{ getMember< sdw::UVec4 >( "size" ) }
		, time{ m_base.x() }
		, depthSofteningDistance{ m_base.y() }
		, distanceMod{ m_base.z() }
		, density{ m_base.w() }
		, refractionRatio{ m_refraction.x() }
		, refractionDistortionFactor{ m_refraction.y() }
		, refractionHeightFactor{ m_refraction.z() }
		, refractionDistanceFactor{ m_refraction.w() }
		, otherMod{ m_mods.xy() }
		, normalMod{ m_mods.zw() }
		, tileScale{ m_tileNormalScales.xy() }
		, normalScale{ m_tileNormalScales.zw() }
		, blockOffset{ m_blockOffFftScale.xy() }
		, fftScale{ m_blockOffFftScale.zw() }
		, patchSize{ m_patchSizes.xy() }
		, maxTessLevel{ m_patchSizes.zw() }
		, invHeightmapSize{ m_fftSizes.xy() }
		, windDirection{ m_fftSizes.zw() }
		, amplitude{ m_fftDistrib.x() }
		, maxWaveLength{ m_fftDistrib.y() }
		, L{ m_fftDistrib.z() }
		, size{ m_size.xy() }
		, displacementDownsample{ writer.cast< sdw::Int >( m_size.z() ) }
#if Ocean_Debug
		, debug{ m_size.w() }
#endif
	{
	}

	ast::type::BaseStructPtr OceanData::makeType( ast::type::TypesCache & cache )
	{
		auto result = cache.getStruct( sdw::type::MemoryLayout::eStd430
			, "OceanData" );

		if ( result->empty() )
		{
			result->declMember( "base", sdw::type::Kind::eVec4F, sdw::type::NotArray );
			result->declMember( "refraction", sdw::type::Kind::eVec4F, sdw::type::NotArray );
			result->declMember( "ssrSettings", sdw::type::Kind::eVec4F, sdw::type::NotArray );
			result->declMember( "mods", sdw::type::Kind::eVec4F, sdw::type::NotArray );
			result->declMember( "tileNormalScales", sdw::type::Kind::eVec4F, sdw::type::NotArray );
			result->declMember( "blockOffFftScale", sdw::type::Kind::eVec4F, sdw::type::NotArray );
			result->declMember( "patchSizes", sdw::type::Kind::eVec4F, sdw::type::NotArray );
			result->declMember( "fftSizes", sdw::type::Kind::eVec4F, sdw::type::NotArray );
			result->declMember( "fftDistrib", sdw::type::Kind::eVec4F, sdw::type::NotArray );
			result->declMember( "size", sdw::type::Kind::eVec4U, sdw::type::NotArray );
			result->end();
		}

		return result;
	}

	//************************************************************************************************

	castor::String const OceanUbo::Buffer = cuT( "Ocean" );
	castor::String const OceanUbo::Data = cuT( "c3d_oceanData" );

	OceanUbo::OceanUbo( castor3d::RenderDevice const & device )
		: m_device{ device }
		, m_ubo{ device.uboPools->getBuffer< Configuration >( 0u ) }
	{
	}

	OceanUbo::~OceanUbo()
	{
		m_device.uboPools->putBuffer( m_ubo );
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
		data.size->x = fftConfig.heightMapSamples;
		data.size->y = fftConfig.heightMapSamples;
		data.displacementDownsample = fftConfig.displacementDownsample;
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
