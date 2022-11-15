#include "AtmosphereScattering/AtmosphereScatteringUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBufferPool.hpp>
#include <Castor3D/Scene/SceneNode.hpp>

#include <CastorUtils/Graphics/Size.hpp>

#include <ShaderWriter/Source.hpp>

namespace atmosphere_scattering
{
	//*********************************************************************************************

	castor::String const AtmosphereScatteringUbo::Buffer = cuT( "C3D_ATM_Atmosphere" );
	castor::String const AtmosphereScatteringUbo::Data = cuT( "d" );

	AtmosphereScatteringUbo::AtmosphereScatteringUbo( castor3d::RenderDevice const & device
		, bool & dirty )
		: m_device{ device }
		, m_ubo{ device.uboPool->getBuffer< Configuration >( 0u ) }
		, m_dirty{ dirty }
		, m_config{ m_dirty }
		, m_sunDirection{ m_dirty }
		, m_mieAbsorption{ m_dirty }
	{
	}

	AtmosphereScatteringUbo::~AtmosphereScatteringUbo()
	{
		m_device.uboPool->putBuffer( m_ubo );
	}

	castor::Point3f AtmosphereScatteringUbo::cpuUpdate( Configuration const & config
		, castor3d::SceneNode const & node )
	{
		auto direction = castor::Point3f{ 0, 0, 1 };
		node.getDerivedOrientation().transform( direction, direction );
		direction = -castor::point::getNormalised( direction );
		m_sunDirection = { direction };

		auto mieAbsorption = config.mieExtinction - config.mieScattering;
		m_mieAbsorption = castor::Point3f{ std::max( 0.0f, mieAbsorption->x )
			, std::max( 0.0f, mieAbsorption->y )
			, std::max( 0.0f, mieAbsorption->z ) };

		m_config = config;

		if ( m_dirty )
		{
			auto & data = m_ubo.getData();
			data = config;
			data.sunIlluminance *= data.sunIlluminanceScale;
			data.sunDirection->x = (* m_sunDirection )->x;
			data.sunDirection->y = (* m_sunDirection )->y;
			data.sunDirection->z = (* m_sunDirection )->z;

			data.mieAbsorption->x = std::max( 0.0f, ( *m_mieAbsorption )->x );
			data.mieAbsorption->y = std::max( 0.0f, ( *m_mieAbsorption )->y );
			data.mieAbsorption->z = std::max( 0.0f, ( *m_mieAbsorption )->z );
		}

		return direction;
	}

	//************************************************************************************************
}
