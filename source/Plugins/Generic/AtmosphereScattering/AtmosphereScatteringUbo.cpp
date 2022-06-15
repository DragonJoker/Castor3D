#include "AtmosphereScattering/AtmosphereScatteringUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBufferPool.hpp>

#include <CastorUtils/Graphics/Size.hpp>

#include <ShaderWriter/Source.hpp>

namespace atmosphere_scattering
{
	//*********************************************************************************************

	AtmosphereData::AtmosphereData( sdw::ShaderWriter & writer
		, ast::expr::ExprPtr expr
		, bool enabled )
		: sdw::StructInstance{ writer, std::move( expr ), enabled }
		, sunDirection{ getMember< sdw::Vec3 >( "sunDirection" ) }
		, pad0{ getMember< sdw::Float >( "pad0" ) }
		, solarIrradiance{ getMember< sdw::Vec3 >( "solarIrradiance" ) }
		, sunAngularRadius{ getMember< sdw::Float >( "sunAngularRadius" ) }
		, sunIlluminance{ getMember< sdw::Vec3 >( "sunIlluminance" ) }
		, sunIlluminanceScale{ getMember< sdw::Vec2 >( "sunIlluminanceScale" ) }
		, rayMarchMinMaxSPP{ getMember< sdw::Vec2 >( "rayMarchMinMaxSPP" ) }
		, pad1{ getMember< sdw::Vec2 >( "pad1" ) }
		, absorptionExtinction{ getMember< sdw::Vec3 >( "absorptionExtinction" ) }
		, muSMin{ getMember< sdw::Float >( "muSMin" ) }
		, rayleighScattering{ getMember< sdw::Vec3 >( "rayleighScattering" ) }
		, miePhaseFunctionG{ getMember< sdw::Float >( "miePhaseFunctionG" ) }
		, mieScattering{ getMember< sdw::Vec3 >( "mieScattering" ) }
		, bottomRadius{ getMember< sdw::Float >( "bottomRadius" ) }
		, mieExtinction{ getMember< sdw::Vec3 >( "mieExtinction" ) }
		, topRadius{ getMember< sdw::Float >( "topRadius" ) }
		, mieAbsorption{ getMember< sdw::Vec3 >( "mieAbsorption" ) }
		, multipleScatteringFactor{ getMember< sdw::Float >( "multipleScatteringFactor" ) }
		, groundAlbedo{ getMember< sdw::Vec3 >( "groundAlbedo" ) }
		, multiScatteringLUTRes{ getMember< sdw::Float >( "multiScatteringLUTRes" ) }
		, rayleighDensity0LayerWidth{ getMember< sdw::Float >( "rayleighDensity0LayerWidth" ) }
		, rayleighDensity0ExpTerm{ getMember < sdw::Float >( "rayleighDensity0ExpTerm" ) }
		, rayleighDensity0ExpScale{ getMember < sdw::Float >( "rayleighDensity0ExpScale" ) }
		, rayleighDensity0LinearTerm{ getMember < sdw::Float >( "rayleighDensity0LinearTerm" ) }
		, rayleighDensity0ConstantTerm{ getMember < sdw::Float >( "rayleighDensity0ConstantTerm" ) }
		, rayleighDensity1LayerWidth{ getMember < sdw::Float >( "rayleighDensity1LayerWidth" ) }
		, rayleighDensity1ExpTerm{ getMember < sdw::Float >( "rayleighDensity1ExpTerm" ) }
		, rayleighDensity1ExpScale{ getMember < sdw::Float >( "rayleighDensity1ExpScale" ) }
		, rayleighDensity1LinearTerm{ getMember < sdw::Float >( "rayleighDensity1LinearTerm" ) }
		, rayleighDensity1ConstantTerm{ getMember < sdw::Float >( "rayleighDensity1ConstantTerm" ) }
		, mieDensity0LayerWidth{ getMember < sdw::Float >( "mieDensity0LayerWidth" ) }
		, mieDensity0ExpTerm{ getMember < sdw::Float >( "mieDensity0ExpTerm" ) }
		, mieDensity0ExpScale{ getMember < sdw::Float >( "mieDensity0ExpScale" ) }
		, mieDensity0LinearTerm{ getMember < sdw::Float >( "mieDensity0LinearTerm" ) }
		, mieDensity0ConstantTerm{ getMember < sdw::Float >( "mieDensity0ConstantTerm" ) }
		, mieDensity1LayerWidth{ getMember < sdw::Float >( "mieDensity1LayerWidth" ) }
		, mieDensity1ExpTerm{ getMember < sdw::Float >( "mieDensity1ExpTerm" ) }
		, mieDensity1ExpScale{ getMember < sdw::Float >( "mieDensity1ExpScale" ) }
		, mieDensity1LinearTerm{ getMember < sdw::Float >( "mieDensity1LinearTerm" ) }
		, mieDensity1ConstantTerm{ getMember < sdw::Float >( "mieDensity1ConstantTerm" ) }
		, absorptionDensity0LayerWidth{ getMember < sdw::Float >( "absorptionDensity0LayerWidth" ) }
		, absorptionDensity0ExpTerm{ getMember < sdw::Float >( "absorptionDensity0ExpTerm" ) }
		, absorptionDensity0ExpScale{ getMember < sdw::Float >( "absorptionDensity0ExpScale" ) }
		, absorptionDensity0LinearTerm{ getMember < sdw::Float >( "absorptionDensity0LinearTerm" ) }
		, absorptionDensity0ConstantTerm{ getMember < sdw::Float >( "absorptionDensity0ConstantTerm" ) }
		, absorptionDensity1LayerWidth{ getMember < sdw::Float >( "absorptionDensity1LayerWidth" ) }
		, absorptionDensity1ExpTerm{ getMember < sdw::Float >( "absorptionDensity1ExpTerm" ) }
		, absorptionDensity1ExpScale{ getMember < sdw::Float >( "absorptionDensity1ExpScale" ) }
		, absorptionDensity1LinearTerm{ getMember < sdw::Float >( "absorptionDensity1LinearTerm" ) }
		, absorptionDensity1ConstantTerm{ getMember < sdw::Float >( "absorptionDensity1ConstantTerm" ) }
	{
	}

	ast::type::BaseStructPtr AtmosphereData::makeType( ast::type::TypesCache & cache )
	{
		auto result = cache.getStruct( sdw::type::MemoryLayout::eStd430
			, "AtmosphereData" );

		if ( result->empty() )
		{
			result->declMember( "sunDirection", sdw::type::Kind::eVec3F, sdw::type::NotArray );
			result->declMember( "pad0", sdw::type::Kind::eFloat, sdw::type::NotArray );
			result->declMember( "solarIrradiance", sdw::type::Kind::eVec3F, sdw::type::NotArray );
			result->declMember( "sunAngularRadius", sdw::type::Kind::eFloat, sdw::type::NotArray );
			result->declMember( "sunIlluminance", sdw::type::Kind::eVec3F, sdw::type::NotArray );
			result->declMember( "sunIlluminanceScale", sdw::type::Kind::eFloat, sdw::type::NotArray );
			result->declMember( "rayMarchMinMaxSPP", sdw::type::Kind::eVec2F, sdw::type::NotArray );
			result->declMember( "pad1", sdw::type::Kind::eVec2F, sdw::type::NotArray );
			result->declMember( "absorptionExtinction", sdw::type::Kind::eVec3F, sdw::type::NotArray );
			result->declMember( "muSMin", sdw::type::Kind::eFloat, sdw::type::NotArray );
			result->declMember( "rayleighScattering", sdw::type::Kind::eVec3F, sdw::type::NotArray );
			result->declMember( "miePhaseFunctionG", sdw::type::Kind::eFloat, sdw::type::NotArray );
			result->declMember( "mieScattering", sdw::type::Kind::eVec3F, sdw::type::NotArray );
			result->declMember( "bottomRadius", sdw::type::Kind::eFloat, sdw::type::NotArray );
			result->declMember( "mieExtinction", sdw::type::Kind::eVec3F, sdw::type::NotArray );
			result->declMember( "topRadius", sdw::type::Kind::eFloat, sdw::type::NotArray );
			result->declMember( "mieAbsorption", sdw::type::Kind::eVec3F, sdw::type::NotArray );
			result->declMember( "multipleScatteringFactor", sdw::type::Kind::eFloat, sdw::type::NotArray );
			result->declMember( "groundAlbedo", sdw::type::Kind::eVec3F, sdw::type::NotArray );
			result->declMember( "multiScatteringLUTRes", sdw::type::Kind::eFloat, sdw::type::NotArray );
			result->declMember( "rayleighDensity0LayerWidth", sdw::type::Kind::eFloat, sdw::type::NotArray );
			result->declMember( "rayleighDensity0ExpTerm", sdw::type::Kind::eFloat, sdw::type::NotArray );
			result->declMember( "rayleighDensity0ExpScale", sdw::type::Kind::eFloat, sdw::type::NotArray );
			result->declMember( "rayleighDensity0LinearTerm", sdw::type::Kind::eFloat, sdw::type::NotArray );
			result->declMember( "rayleighDensity0ConstantTerm", sdw::type::Kind::eFloat, sdw::type::NotArray );
			result->declMember( "rayleighDensity1LayerWidth", sdw::type::Kind::eFloat, sdw::type::NotArray );
			result->declMember( "rayleighDensity1ExpTerm", sdw::type::Kind::eFloat, sdw::type::NotArray );
			result->declMember( "rayleighDensity1ExpScale", sdw::type::Kind::eFloat, sdw::type::NotArray );
			result->declMember( "rayleighDensity1LinearTerm", sdw::type::Kind::eFloat, sdw::type::NotArray );
			result->declMember( "rayleighDensity1ConstantTerm", sdw::type::Kind::eFloat, sdw::type::NotArray );
			result->declMember( "mieDensity0LayerWidth", sdw::type::Kind::eFloat, sdw::type::NotArray );
			result->declMember( "mieDensity0ExpTerm", sdw::type::Kind::eFloat, sdw::type::NotArray );
			result->declMember( "mieDensity0ExpScale", sdw::type::Kind::eFloat, sdw::type::NotArray );
			result->declMember( "mieDensity0LinearTerm", sdw::type::Kind::eFloat, sdw::type::NotArray );
			result->declMember( "mieDensity0ConstantTerm", sdw::type::Kind::eFloat, sdw::type::NotArray );
			result->declMember( "mieDensity1LayerWidth", sdw::type::Kind::eFloat, sdw::type::NotArray );
			result->declMember( "mieDensity1ExpTerm", sdw::type::Kind::eFloat, sdw::type::NotArray );
			result->declMember( "mieDensity1ExpScale", sdw::type::Kind::eFloat, sdw::type::NotArray );
			result->declMember( "mieDensity1LinearTerm", sdw::type::Kind::eFloat, sdw::type::NotArray );
			result->declMember( "mieDensity1ConstantTerm", sdw::type::Kind::eFloat, sdw::type::NotArray );
			result->declMember( "absorptionDensity0LayerWidth", sdw::type::Kind::eFloat, sdw::type::NotArray );
			result->declMember( "absorptionDensity0ExpTerm", sdw::type::Kind::eFloat, sdw::type::NotArray );
			result->declMember( "absorptionDensity0ExpScale", sdw::type::Kind::eFloat, sdw::type::NotArray );
			result->declMember( "absorptionDensity0LinearTerm", sdw::type::Kind::eFloat, sdw::type::NotArray );
			result->declMember( "absorptionDensity0ConstantTerm", sdw::type::Kind::eFloat, sdw::type::NotArray );
			result->declMember( "absorptionDensity1LayerWidth", sdw::type::Kind::eFloat, sdw::type::NotArray );
			result->declMember( "absorptionDensity1ExpTerm", sdw::type::Kind::eFloat, sdw::type::NotArray );
			result->declMember( "absorptionDensity1ExpScale", sdw::type::Kind::eFloat, sdw::type::NotArray );
			result->declMember( "absorptionDensity1LinearTerm", sdw::type::Kind::eFloat, sdw::type::NotArray );
			result->declMember( "absorptionDensity1ConstantTerm", sdw::type::Kind::eFloat, sdw::type::NotArray );
			result->end();
		}

		return result;
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

		auto sunOrientation = castor::Quaternion::fromAxisAngle( castor::Point3f{ data.sunDirection }
			, castor::Angle::fromRadians( data.sunDirection->w ) );
		auto front = castor::Point3f{ 0.0f, 0.0f, 1.0f };
		sunOrientation.transform( front, front );
		data.sunDirection->x = front->x;
		data.sunDirection->y = front->y;
		data.sunDirection->z = front->z;

		data.sunIlluminance *= data.sunIlluminanceScale;

		auto mieAbsorption = data.mieExtinction - data.mieScattering;
		data.mieAbsorption->x = std::max( 0.0f, mieAbsorption->x );
		data.mieAbsorption->y = std::max( 0.0f, mieAbsorption->y );
		data.mieAbsorption->z = std::max( 0.0f, mieAbsorption->z );
	}

	//************************************************************************************************
}
