#include "AtmosphereScattering/AtmosphereBackgroundModel.hpp"

#include <Castor3D/Engine.hpp>

#include <ShaderWriter/Writer.hpp>
#include <ShaderWriter/BaseTypes/Float.hpp>
#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

namespace atmosphere_scattering
{
	namespace model
	{
		template< typename DataT >
		DataT getData( sdw::UniformBuffer & ubo, std::string const & name )
		{
			auto result = ubo.declMember< DataT >( name );
			ubo.end();
			return result;
		}
	}

	castor::String const AtmosphereBackgroundModel::Name = cuT( "c3d.atmosphere" );

	AtmosphereBackgroundModel::AtmosphereBackgroundModel( castor3d::Engine const & engine
		, sdw::ShaderWriter & writer
		, castor3d::shader::Utils & utils
		, VkExtent2D targetSize
		, bool needsForeground
		, uint32_t & binding
		, uint32_t set )
		: castor3d::shader::BackgroundModel{ writer, utils, std::move( targetSize ) }
		, cameraBuffer{ writer.declUniformBuffer<>( CameraUbo::Buffer
			, binding++
			, set ) }
		, cameraData{ model::getData< CameraData >( cameraBuffer, CameraUbo::Data ) }
		, atmosphereBuffer{ writer.declUniformBuffer<>( AtmosphereScatteringUbo::Buffer
			, binding++
			, set ) }
		, atmosphereData{ model::getData< AtmosphereData >( atmosphereBuffer, AtmosphereScatteringUbo::Data ) }
		, atmosphere{ m_writer
			, atmosphereData
			, AtmosphereModel::Settings{ castor::Length::fromUnit( 1.0f, engine.getLengthUnit() ) }
				.setCameraData( &cameraData )
				.setVariableSampleCount( true )
				.setMieRayPhase( true )
				.setMultiScatApprox( true )
			, { 256u, 64u } }
		, scattering{ m_writer
			, atmosphere
			, ScatteringModel::Settings{}
				.setNeedsMultiscatter( true )
				.setFastAerialPerspective( true )
			, binding
			, set }
	{
	}

	castor3d::shader::BackgroundModelPtr AtmosphereBackgroundModel::create( castor3d::Engine const & engine
		, sdw::ShaderWriter & writer
		, castor3d::shader::Utils & utils
		, VkExtent2D targetSize
		, bool needsForeground
		, uint32_t & binding
		, uint32_t set )
	{
		return std::make_unique< AtmosphereBackgroundModel >( engine
			, writer
			, utils
			, std::move( targetSize )
			, needsForeground
			, binding
			, set );
	}

	sdw::RetVec3 AtmosphereBackgroundModel::computeRefractions( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, sdw::Float const & refractionRatio
		, castor3d::shader::BlendComponents & components )
	{
		if ( !m_computeRefractions )
		{
			m_computeRefractions = m_writer.implementFunction< sdw::Vec3 >( "c3d_atmbg_computeRefractions"
				, [&]()
				{
					m_writer.returnStmt( vec3( 0.0_f ) );
				} );
		}

		return m_computeRefractions();
	}

	void AtmosphereBackgroundModel::applyForeground( sdw::Vec2 const pfragCoord
		, sdw::Float const plinearDepth
		, sdw::Vec2 const ptargetSize
		, sdw::Vec2 const pcameraPlanes
		, sdw::Vec4 & poutput )
	{
	}

	sdw::Vec3 AtmosphereBackgroundModel::getSunRadiance( sdw::Vec3 const & psunDir )
	{
		return scattering.getSunRadiance( psunDir );
	}
}
