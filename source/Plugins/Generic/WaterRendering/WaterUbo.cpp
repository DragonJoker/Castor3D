#include "WaterRendering/WaterUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBufferPools.hpp>

#include <CastorUtils/Graphics/Size.hpp>

namespace water
{
	//*********************************************************************************************

#if Water_Debug
	castor::StringArray const & getWaterDisplayDataNames()
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

	WaterData::WaterData( sdw::ShaderWriter & writer
		, ast::expr::ExprPtr expr
		, bool enabled )
		: sdw::StructInstance{ writer, std::move( expr ), enabled }
		, m_base{ getMember< sdw::Vec4 >( "base" ) }
		, m_refraction{ getMember< sdw::Vec4 >( "refraction" ) }
		, m_scrSpdOth{ getMember< sdw::Vec4 >( "scrSpdOth" ) }
		, normalMapScroll{ getMember< sdw::Vec4 >( "normalMapScroll" ) }
		, ssrSettings{ getMember< sdw::Vec4 >( "ssrSettings" ) }
#if Water_Debug
		, debug{ getMember< sdw::UVec4 >( "debug") }
#endif
		, time{ m_base.x() }
		, dampeningFactor{ m_base.y() }
		, depthSofteningDistance{ m_base.z() }
		, refractionRatio{ m_refraction.x() }
		, refractionDistortionFactor{ m_refraction.y() }
		, refractionHeightFactor{ m_refraction.z() }
		, refractionDistanceFactor{ m_refraction.w() }
		, normalMapScrollSpeed{ m_scrSpdOth.xy() }
	{
	}

	ast::type::BaseStructPtr WaterData::makeType( ast::type::TypesCache & cache )
	{
		auto result = cache.getStruct( sdw::type::MemoryLayout::eStd430
			, "WaterData" );

		if ( result->empty() )
		{
			result->declMember( "base", sdw::type::Kind::eVec4F, sdw::type::NotArray );
			result->declMember( "refraction", sdw::type::Kind::eVec4F, sdw::type::NotArray );
			result->declMember( "scrSpdOth", sdw::type::Kind::eVec4F, sdw::type::NotArray );
			result->declMember( "normalMapScroll", sdw::type::Kind::eVec4F, sdw::type::NotArray );
			result->declMember( "ssrSettings", sdw::type::Kind::eVec4F, sdw::type::NotArray );
#if Water_Debug
			result->declMember( "debug", sdw::type::Kind::eVec4U, sdw::type::NotArray );
#endif
			result->end();
		}

		return result;
	}

	//************************************************************************************************

	castor::String const WaterUbo::Buffer = cuT( "Water" );
	castor::String const WaterUbo::Data = cuT( "c3d_waterData" );

	WaterUbo::WaterUbo( castor3d::RenderDevice const & device )
		: m_device{ device }
		, m_ubo{ device.uboPools->getBuffer< Configuration >( 0u ) }
	{
	}

	WaterUbo::~WaterUbo()
	{
		m_device.uboPools->putBuffer( m_ubo );
	}

	void WaterUbo::cpuUpdate( WaterUboConfiguration const & config )
	{
		auto & data = m_ubo.getData();
		data = config;
	}

	//************************************************************************************************
}
