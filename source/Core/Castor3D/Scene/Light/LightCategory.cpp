#include "Castor3D/Scene/Light/LightCategory.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Scene/Light/Light.hpp"

#include <CastorUtils/Graphics/PixelBuffer.hpp>

#include <ShaderWriter/Source.hpp>

using namespace castor;

namespace castor3d
{
	LightCategory::LightCategory( LightType lightType, Light & light )
		: m_lightType{ lightType }
		, m_light{ light }
	{
	}

	void LightCategory::fillBuffer( LightBuffer::LightData & data )const
	{
		data.colourIndex = getColour();
		data.colourIndex.w = float( m_shadowMapIndex );

		data.intensityFarPlane = getIntensity();
		data.intensityFarPlane.z = getFarPlane();
		data.intensityFarPlane.w = float( getLight().getShadowType() );

		data.volumetric.x = float( getVolumetricSteps() );
		data.volumetric.y = getVolumetricScatteringFactor();
		data.volumetric.z = 0.0f;
		data.volumetric.w = 0.0f;

		data.shadowsOffsets.x = getShadowRawOffsets()->x;
		data.shadowsOffsets.y = getShadowRawOffsets()->y;
		data.shadowsOffsets.z = getShadowPcfOffsets()->y;
		data.shadowsOffsets.w = getShadowPcfOffsets()->y;

		data.shadowsVariances = getShadowVariance();

		doFillBuffer( data );
	}

	uint32_t LightCategory::getVolumetricSteps()const
	{
		return m_light.getVolumetricSteps();
	}

	float LightCategory::getVolumetricScatteringFactor()const
	{
		return m_light.getVolumetricScatteringFactor();
	}

	castor::Point2f const & LightCategory::getShadowRawOffsets()const
	{
		return m_light.getShadowRawOffsets();
	}

	castor::Point2f const & LightCategory::getShadowPcfOffsets()const
	{
		return m_light.getShadowPcfOffsets();
	}

	castor::Point2f const & LightCategory::getShadowVariance()const
	{
		return m_light.getShadowVariance();
	}

	ShadowConfig const & LightCategory::getShadowConfig()const
	{
		return m_light.getShadowConfig();
	}

	RsmConfig const & LightCategory::getRsmConfig()const
	{
		return m_light.getRsmConfig();
	}

	LpvConfig const & LightCategory::getLpvConfig()const
	{
		return m_light.getLpvConfig();
	}

	void LightCategory::setVolumetricSteps( uint32_t value )
	{
		m_light.setVolumetricSteps( value );
	}

	void LightCategory::setVolumetricScatteringFactor( float value )
	{
		m_light.setVolumetricScatteringFactor( value );
	}

	void LightCategory::setRawMinOffset( float value )
	{
		m_light.setRawMinOffset( value );
	}

	void LightCategory::setRawMaxSlopeOffset( float value )
	{
		m_light.setRawMaxSlopeOffset( value );
	}

	void LightCategory::setPcfMinOffset( float value )
	{
		m_light.setPcfMinOffset( value );
	}

	void LightCategory::setPcfMaxSlopeOffset( float value )
	{
		m_light.setPcfMaxSlopeOffset( value );
	}

	void LightCategory::setVsmMaxVariance( float value )
	{
		m_light.setVsmMaxVariance( value );
	}

	void LightCategory::setVsmVarianceBias( float value )
	{
		m_light.setVsmVarianceBias( value );
	}

	void LightCategory::setColour( castor::Point3f const & value )
	{
		m_colour = value;
		getLight().onGPUChanged( getLight() );
	}

	void LightCategory::setIntensity( castor::Point2f const & value )
	{
		m_intensity = value;
		getLight().onGPUChanged( getLight() );
	}

	void LightCategory::setDiffuseIntensity( float value )
	{
		m_intensity[0] = value;
		getLight().onGPUChanged( getLight() );
	}

	void LightCategory::setSpecularIntensity( float value )
	{
		m_intensity[1] = value;
		getLight().onGPUChanged( getLight() );
	}
}
