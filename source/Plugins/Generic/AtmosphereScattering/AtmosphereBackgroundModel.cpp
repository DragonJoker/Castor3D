#include "AtmosphereScattering/AtmosphereBackgroundModel.hpp"

#include <ShaderWriter/Writer.hpp>
#include <ShaderWriter/BaseTypes/Float.hpp>
#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

namespace atmosphere_scattering
{
	namespace model
	{
		template< typename DataT >
		DataT getData( sdw::Ubo & ubo, std::string const & name )
		{
			auto result = ubo.declMember< DataT >( name );
			ubo.end();
			return result;
		}
	}

	castor::String const AtmosphereBackgroundModel::Name = cuT( "c3d.atmosphere" );

	AtmosphereBackgroundModel::AtmosphereBackgroundModel( sdw::ShaderWriter & writer
		, castor3d::shader::Utils & utils
		, VkExtent2D targetSize
		, uint32_t & binding
		, uint32_t set )
		: castor3d::shader::BackgroundModel{ writer, utils, std::move( targetSize ) }
		, m_cameraBuffer{ writer.declUniformBuffer<>( CameraUbo::Buffer
			, binding++
			, set ) }
		, m_cameraData{ model::getData< CameraData >( m_cameraBuffer, CameraUbo::Data ) }
		, m_atmosphereBuffer{ writer.declUniformBuffer<>( AtmosphereScatteringUbo::Buffer
			, binding++
			, set ) }
		, m_atmosphereData{ model::getData< AtmosphereData >( m_atmosphereBuffer, AtmosphereScatteringUbo::Data ) }
		, m_transmittanceMap{ m_writer.declCombinedImg< sdw::CombinedImage2DRgba32 >( "c3d_mapTransmittance"
			, binding++
			, set ) }
		, m_atmosphere{ m_writer
			, m_atmosphereData
			, { false, &m_cameraData, true, true }
			, { 256u, 64u }
			, &m_transmittanceMap }
		, m_scattering{ m_writer
			, m_atmosphere
			, m_cameraData
			, m_atmosphereData
			, false, true, true, false }
	{
		m_writer.declCombinedImg< sdw::CombinedImage2DRgba32 >( "c3d_mapSkyView"
			, binding++
			, set );
		m_writer.declCombinedImg< sdw::CombinedImage3DRgba32 >( "c3d_mapVolume"
			, binding++
			, set );
	}

	castor3d::shader::BackgroundModelPtr AtmosphereBackgroundModel::create( sdw::ShaderWriter & writer
		, castor3d::shader::Utils & utils
		, VkExtent2D targetSize
		, uint32_t & binding
		, uint32_t set )
	{
		return std::make_unique< AtmosphereBackgroundModel >( writer
			, utils
			, std::move( targetSize )
			, binding
			, set );
	}

	sdw::Vec3 AtmosphereBackgroundModel::computeReflections( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, castor3d::shader::LightMaterial const & material
		, sdw::CombinedImage2DRg32 const & brdf )
	{
		return vec3( 0.0_f );
	}

	sdw::Vec3 AtmosphereBackgroundModel::computeRefractions( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, sdw::Float const & refractionRatio
		, sdw::Vec3 const & transmission
		, castor3d::shader::LightMaterial const & material )
	{
		return vec3( 0.0_f );
	}

	sdw::Void AtmosphereBackgroundModel::mergeReflRefr( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, sdw::Float const & refractionRatio
		, sdw::Vec3 const & transmission
		, castor3d::shader::LightMaterial const & material
		, sdw::Vec3 & reflection
		, sdw::Vec3 & refraction )
	{
		return sdw::Void{};
	}

	sdw::Vec4 AtmosphereBackgroundModel::scatter( sdw::Vec2 const & fragPos
		, sdw::Vec2 const & fragSize
		, sdw::Float const & fragDepth )
	{
		auto luminance = m_writer.declLocale< sdw::Vec4 >( "luminance" );
		auto transmittance = m_writer.declLocale< sdw::Vec4 >( "transmittance" );
		m_scattering.getPixelTransLum( fragPos
			, fragSize
			, fragDepth
			, m_transmittanceMap
			, m_writer.getVariable< sdw::CombinedImage2DRgba32 >( "c3d_mapSkyView" )
			, m_writer.getVariable< sdw::CombinedImage3DRgba32 >( "c3d_mapVolume" )
			, transmittance
			, luminance );

		luminance.xyz() /= luminance.a();	// Normalise according to sample count when path tracing

		// Similar setup to the Bruneton demo
		auto whitePoint = m_writer.declLocale( "whitePoint"
			, vec3( 1.08241_f, 0.96756_f, 0.95003_f ) );
		auto exposure = m_writer.declLocale( "exposure"
			, 10.0_f );
		auto hdr = vec3( 1.0_f ) - exp( -luminance.rgb() / whitePoint * exposure );
		return vec4( hdr, luminance.a() );
	}
}
