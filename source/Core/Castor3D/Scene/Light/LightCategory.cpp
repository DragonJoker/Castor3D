#include "Castor3D/Scene/Light/LightCategory.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Scene/Light/Light.hpp"

#include <CastorUtils/Graphics/PixelBuffer.hpp>

#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	LightCategory::LightCategory( LightType lightType, Light & light )
		: m_lightType{ lightType }
		, m_light{ light }
	{
	}

	void LightCategory::fillBuffer( castor::Point4f * data )const
	{
		auto & base = *reinterpret_cast< LightData * >( data->ptr() );
		base.colour = getColour();
		base.shadowMapIndex = float( m_light.getShadowMapIndex() );

		base.intensity = getIntensity();
		base.farPlane = getFarPlane();
		base.shadowType = float( getLight().isShadowProducer()
			? getLight().getShadowType()
			: ShadowType::eNone );

		base.volumetricSteps = float( getVolumetricSteps() );
		base.volumetricScattering = getVolumetricScatteringFactor();

		base.rawShadowsOffsets = getShadowRawOffsets();
		base.pcfShadowsOffsets = getShadowPcfOffsets();

		base.vsmShadowVariance = getShadowVariance();

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
		getLight().markDirty();
	}

	void LightCategory::setIntensity( castor::Point2f const & value )
	{
		m_intensity = value;
		getLight().markDirty();
	}

	void LightCategory::setDiffuseIntensity( float value )
	{
		m_intensity[0] = value;
		getLight().markDirty();
	}

	void LightCategory::setSpecularIntensity( float value )
	{
		m_intensity[1] = value;
		getLight().markDirty();
	}
}
