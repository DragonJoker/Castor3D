#include "Castor3D/Scene/Light/LightCategory.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Scene/Light/Light.hpp"

#include <CastorUtils/Graphics/PixelBuffer.hpp>

#include <ShaderWriter/Source.hpp>

CU_ImplementSmartPtr( castor3d, LightCategory )

namespace castor3d
{
	LightCategory::LightCategory( LightType lightType
		, Light & light
		, uint32_t lightComponentCount
		, uint32_t shadowComponentCount )
		: m_lightType{ lightType }
		, m_light{ light }
		, m_lightComponentCount{ lightComponentCount }
		, m_shadowComponentCount{ shadowComponentCount }
	{
	}

	void LightCategory::fillLightBuffer( castor::Point4f * data )const
	{
		auto & base = *reinterpret_cast< LightData * >( data->ptr() );
		base.colour = getColour();

		base.intensity = getIntensity();
		base.shadowMapIndex = float( m_light.getShadowMapIndex() );

		doFillLightBuffer( data );
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

	castor::RangedValue< uint32_t > LightCategory::getShadowPcfFilterSize()const
	{
		return m_light.getShadowPcfFilterSize();
	}

	castor::RangedValue< uint32_t > LightCategory::getShadowPcfSampleCount()const
	{
		return m_light.getShadowPcfSampleCount();
	}

	float LightCategory::getVsmMinVariance()const
	{
		return m_light.getVsmMinVariance();
	}

	float LightCategory::getVsmLightBleedingReduction()const
	{
		return m_light.getVsmLightBleedingReduction();
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

	void LightCategory::setPcfFilterSize( uint32_t value )
	{
		m_light.setPcfFilterSize( value );
	}

	void LightCategory::setPcfSampleCount( uint32_t value )
	{
		m_light.setPcfSampleCount( value );
	}

	void LightCategory::setVsmMinVariance( float value )
	{
		m_light.setVsmMinVariance( value );
	}

	void LightCategory::setVsmLightBleedingReduction( float value )
	{
		m_light.setVsmLightBleedingReduction( value );
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

	void LightCategory::doFillBaseShadowData( BaseShadowData & data )const
	{
		data.shadowType = uint32_t( getLight().isShadowProducer()
			? getLight().getShadowType()
			: ShadowType::eNone );
		data.pcfFilterSize = float( getShadowPcfFilterSize().value() );
		data.pcfSampleCount = getShadowPcfSampleCount().value();

		data.rawShadowsOffsets = getShadowRawOffsets();
		data.pcfShadowsOffsets = getShadowPcfOffsets();

		data.vsmMinVariance = getVsmMinVariance();
		data.vsmLightBleedingReduction = getVsmLightBleedingReduction();
		data.volumetricSteps = getVolumetricSteps();
		data.volumetricScattering = getVolumetricScatteringFactor();
	}
}
