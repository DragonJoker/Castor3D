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

	//*********************************************************************************************

	Wave::Wave( sdw::ShaderWriter & writer
		, sdw::expr::ExprPtr expr
		, bool enabled )
		: sdw::StructInstance{ writer, std::move( expr ), enabled }
		, m_direction{ getMember< sdw::Vec4 >( "direction" ) }
		, m_other{ getMember< sdw::Vec4 >( "other" ) }
		, direction{ m_direction.xyz() }
		, steepness{ m_other.x() }
		, length{ m_other.y() }
		, amplitude{ m_other.z() }
		, speed{ m_other.w() }
	{
	}

	sdw::type::BaseStructPtr Wave::makeType( sdw::type::TypesCache & cache )
	{
		auto result = cache.getStruct( sdw::type::MemoryLayout::eStd430
			, "Wave" );

		if ( result->empty() )
		{
			result->declMember( "direction", sdw::type::Kind::eVec4F, sdw::type::NotArray );
			result->declMember( "other", sdw::type::Kind::eVec4F, sdw::type::NotArray );
			result->end();
		}

		return result;
	}

	//*********************************************************************************************

	OceanData::OceanData( sdw::ShaderWriter & writer
		, ast::expr::ExprPtr expr
		, bool enabled )
		: sdw::StructInstance{ writer, std::move( expr ), enabled }
		, m_base{ getMember< sdw::Vec4 >( "base" ) }
		, m_refraction{ getMember< sdw::Vec4 >( "refraction" ) }
		, m_foam{ getMember< sdw::Vec4 >( "foam" ) }
		, m_other{ getMember< sdw::Vec4 >( "other" ) }
		, normalMapScroll{ getMember< sdw::Vec4 >( "normalMapScroll" ) }
		, ssrSettings{ getMember< sdw::Vec4 >( "ssrSettings" ) }
#if Ocean_Debug
		, debug{ getMember< sdw::UVec4 >( "debug" ) }
#endif
		, waves{ getMemberArray< Wave >( "waves" ) }
		, tessellationFactor{ m_base.x() }
		, numWaves{ m_base.y() }
		, time{ m_base.z() }
		, dampeningFactor{ m_base.w() }
		, refractionRatio{ m_refraction.x() }
		, refractionDistortionFactor{ m_refraction.y() }
		, refractionHeightFactor{ m_refraction.z() }
		, refractionDistanceFactor{ m_refraction.w() }
		, foamHeightStart{ m_foam.x() }
		, foamFadeDistance{ m_foam.y() }
		, foamTiling{ m_foam.z() }
		, foamAngleExponent{ m_foam.w() }
		, foamBrightness{ m_other.x() }
		, depthSofteningDistance{ m_other.y() }
		, normalMapScrollSpeed{ m_other.zw() }
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
			result->declMember( "foam", sdw::type::Kind::eVec4F, sdw::type::NotArray );
			result->declMember( "other", sdw::type::Kind::eVec4F, sdw::type::NotArray );
			result->declMember( "normalMapScroll", sdw::type::Kind::eVec4F, sdw::type::NotArray );
			result->declMember( "ssrSettings", sdw::type::Kind::eVec4F, sdw::type::NotArray );
#if Ocean_Debug
			result->declMember( "debug", sdw::type::Kind::eVec4U, sdw::type::NotArray );
#endif
			result->declMember( "waves", Wave::makeType( cache ), MaxWaves );
			result->end();
		}

		return result;
	}

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
