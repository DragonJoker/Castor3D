#include "OceanRendering/OceanUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBufferPool.hpp>

#include <CastorUtils/Graphics/Size.hpp>

namespace ocean
{
	//*********************************************************************************************

#if Ocean_Debug
	castor::StringArray const & getOceanDisplayDataNames()
	{
		static castor::StringArray const result{ "Result"
			, "FinalNormal"
			, "MatSpecular"
			, "LightDiffuse"
			, "LightSpecular"
			, "LightScattering"
			, "NoisedSpecular"
			, "SpecularNoise"
			, "IndirectOcclusion"
			, "LightIndirectDiffuse"
			, "LightIndirectSpecular"
			, "IndirectAmbient"
			, "IndirectDiffuse"
			, "BackgroundReflection"
			, "SSRResult"
			, "SSRFactor"
			, "SSRResultFactor"
			, "Reflection"
			, "Refraction"
			, "DepthSoftenedAlpha"
			, "HeightMixedRefraction"
			, "DistanceMixedRefraction"
			, "FresnelFactor"
			, "FinalReflection"
			, "FinalRefraction"
			, "WaterBaseColour" };
		return result;
	}
#endif

	//************************************************************************************************

	castor::String const OceanUbo::Buffer = cuT( "Ocean" );
	castor::String const OceanUbo::Data = cuT( "c3d_oceanData" );

	OceanUbo::OceanUbo( castor3d::RenderDevice const & device )
		: m_device{ device }
		, m_ubo{ device.uboPool->getBuffer< Configuration >( 0u ) }
	{
	}

	OceanUbo::~OceanUbo()
	{
		m_device.uboPool->putBuffer( m_ubo );
	}

	void OceanUbo::cpuUpdate( OceanUboConfiguration const & config )
	{
		auto & data = m_ubo.getData();
		data = config;
	}

	//************************************************************************************************
}
