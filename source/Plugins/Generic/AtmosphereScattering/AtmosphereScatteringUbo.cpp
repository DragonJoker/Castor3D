#include "AtmosphereScattering/AtmosphereScatteringUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBufferPool.hpp>

#include <CastorUtils/Graphics/Size.hpp>

#include <ShaderWriter/Source.hpp>

namespace atmosphere_scattering
{
	//*********************************************************************************************

#if AtmosphereScattering_Debug
	castor::StringArray const & getDisplayDataNames()
	{
		static castor::StringArray const result{ "Result"
			, "SunContribution"
			, "SkyContribution" };
		return result;
	}
#endif

	//*********************************************************************************************

	AtmosphereData::AtmosphereData( sdw::ShaderWriter & writer
		, ast::expr::ExprPtr expr
		, bool enabled )
		: sdw::StructInstance{ writer, std::move( expr ), enabled }
		, m_sunEarth{ getMember< sdw::Vec4 >( "sunEarth" ) }
		, m_atmos0{ getMember< sdw::Vec4 >( "atmos0" ) }
		, m_atmos1{ getMember< sdw::Vec4 >( "atmos1" ) }
		, m_atmos2{ getMember< sdw::Vec4 >( "atmos2" ) }
		, m_atmos3{ getMember< sdw::Vec4 >( "atmos3" ) }
		, m_atmos4{ getMember< sdw::IVec4 >( "atmos4" ) }
		, m_atmos5{ getMember< sdw::IVec4 >( "atmos5" ) }
		, m_atmos6{ getMember< sdw::IVec4 >( "atmos6" ) }
		, m_sd{ getMember< sdw::UVec4 >( "sd" ) }
		, earthPos{ m_sunEarth.xyz() }
		, sunIntensity{ m_sunEarth.w() }
		, scale{ m_atmos0.x() }
		, Rg{ m_atmos0.y() }
		, Rt{ m_atmos0.z() }
		, RL{ m_atmos0.w() }
		, averageGroundReflectance{ m_atmos1.x() }
		, HR{ m_atmos1.y() }
		, HM{ m_atmos1.z() }
		, betaMSca{ m_atmos1.w() }
		, betaR{ m_atmos2.xyz() }
		, betaMDiv{ m_atmos2.w() }
		, mieG{ m_atmos3.x() }
		, g{ m_atmos3.y() }
		, transmittanceIntegralSamples{ m_atmos4.x() }
		, inscatterIntegralSamples{ m_atmos4.y() }
		, irradianceIntegralSamples{ m_atmos4.z() }
		, inscatterSphericalIntegralSamples{ m_atmos4.w() }
		, transmittanceW{ m_atmos5.x() }
		, transmittanceH{ m_atmos5.y() }
		, skyW{ m_atmos5.z() }
		, skyH{ m_atmos5.w() }
		, resR{ m_atmos6.x() }
		, resMu{ m_atmos6.y() }
		, resMuS{ m_atmos6.z() }
		, resNu{ m_atmos6.w() }
		, size{ m_sd.xy() }
		, vertexCount{ m_sd.z() }
#if AtmosphereScattering_Debug
		, debug{ m_sd.w() }
#endif
	{
	}

	ast::type::BaseStructPtr AtmosphereData::makeType( ast::type::TypesCache & cache )
	{
		auto result = cache.getStruct( sdw::type::MemoryLayout::eStd430
			, "AtmosphereData" );

		if ( result->empty() )
		{
			result->declMember( "sunEarth", sdw::type::Kind::eVec4F, sdw::type::NotArray );
			result->declMember( "atmos0", sdw::type::Kind::eVec4F, sdw::type::NotArray );
			result->declMember( "atmos1", sdw::type::Kind::eVec4F, sdw::type::NotArray );
			result->declMember( "atmos2", sdw::type::Kind::eVec4F, sdw::type::NotArray );
			result->declMember( "atmos3", sdw::type::Kind::eVec4F, sdw::type::NotArray );
			result->declMember( "atmos4", sdw::type::Kind::eVec4I, sdw::type::NotArray );
			result->declMember( "atmos5", sdw::type::Kind::eVec4I, sdw::type::NotArray );
			result->declMember( "atmos6", sdw::type::Kind::eVec4I, sdw::type::NotArray );
			result->declMember( "sd", sdw::type::Kind::eVec4U, sdw::type::NotArray );
			result->end();
		}

		return result;
	}

	sdw::Vec3 AtmosphereData::getSunDir()
	{
		//auto sunTheta = ( time / 10.0_f ) * castor::Pi< float > / 2.0f - 0.05f;
		//auto sunPhi = 0.0_f;
		//return vec3( sin( sdw::Float{ sunTheta } ) * cos( sdw::Float{ sunPhi } )
		//	, sin( sdw::Float{ sunTheta } ) * sin( sdw::Float{ sunPhi } )
		//	, cos( sdw::Float{ sunTheta } ) );
		return vec3( 0.99875_f, 0.0_f, 0.0499792_f );
	}

	//************************************************************************************************

	castor::String const AtmosphereScatteringUbo::Buffer = cuT( "Atmosphere" );
	castor::String const AtmosphereScatteringUbo::Data = cuT( "c3d_atmosphereData" );

	AtmosphereScatteringUbo::AtmosphereScatteringUbo( castor3d::RenderDevice const & device )
		: m_device{ device }
		, m_ubo{ device.uboPool->getBuffer< Configuration >( 0u ) }
	{
	}

	AtmosphereScatteringUbo::~AtmosphereScatteringUbo()
	{
		m_device.uboPool->putBuffer( m_ubo );
	}

	void AtmosphereScatteringUbo::cpuUpdate( Configuration const & config )
	{
		auto & data = m_ubo.getData();
		data = config;
	}

	//************************************************************************************************
}
