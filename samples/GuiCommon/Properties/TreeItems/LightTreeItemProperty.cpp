#include "GuiCommon/Properties/TreeItems/LightTreeItemProperty.hpp"

#include "GuiCommon/Properties/Math/PointProperties.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/Light/DirectionalLight.hpp>
#include <Castor3D/Scene/Light/SpotLight.hpp>
#include <Castor3D/Scene/Light/Light.hpp>
#include <Castor3D/Scene/Light/PointLight.hpp>

#include <wx/propgrid/advprops.h>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	LightTreeItemProperty::LightTreeItemProperty( bool editable, Light & light )
		: TreeItemProperty( light.getScene()->getEngine(), editable, ePROPERTY_DATA_TYPE_LIGHT )
		, m_light( light )
	{
		CreateTreeItemMenu();
	}

	LightTreeItemProperty::~LightTreeItemProperty()
	{
	}

	void LightTreeItemProperty::doCreateProperties( wxPGEditor * editor, wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_LIGHT = _( "Light:" );
		static wxString PROPERTY_LIGHT_COLOUR = _( "Colour" );
		static wxString PROPERTY_LIGHT_INTENSITY = _( "Intensities" );

		addProperty( grid, PROPERTY_CATEGORY_LIGHT + wxString( m_light.getName() ) );
		addPropertyT( grid, PROPERTY_LIGHT_COLOUR, m_light.getColour(), &m_light, ( void ( Light:: * )( castor::Point3f const & ) ) & Light::setColour );
		addPropertyT( grid, PROPERTY_LIGHT_INTENSITY, m_light.getIntensity(), &m_light, &Light::setIntensity );

		switch ( m_light.getLightType() )
		{
		case LightType::eDirectional:
			doCreateDirectionalLightProperties( grid, *m_light.getDirectionalLight() );
			break;

		case LightType::ePoint:
			doCreatePointLightProperties( grid, *m_light.getPointLight() );
			break;

		case LightType::eSpot:
			doCreateSpotLightProperties( grid, *m_light.getSpotLight() );
			break;
		}

		doCreateShadowProperties( grid );
	}

	void LightTreeItemProperty::doCreateDirectionalLightProperties( wxPropertyGrid * grid, DirectionalLight & light )
	{
	}

	void LightTreeItemProperty::doCreatePointLightProperties( wxPropertyGrid * grid, PointLight & light )
	{
		static wxString PROPERTY_CATEGORY_POINT_LIGHT = _( "Point Light" );
		static wxString PROPERTY_LIGHT_ATTENUATION = _( "Attenuation" );

		addProperty( grid, PROPERTY_CATEGORY_POINT_LIGHT );
		addPropertyT( grid, PROPERTY_LIGHT_ATTENUATION, light.getAttenuation(), &light, &PointLight::setAttenuation );
	}

	void LightTreeItemProperty::doCreateSpotLightProperties( wxPropertyGrid * grid, SpotLight & light )
	{
		static wxString PROPERTY_CATEGORY_SPOT_LIGHT = _( "Spot Light" );
		static wxString PROPERTY_LIGHT_ATTENUATION = _( "Attenuation" );
		static wxString PROPERTY_LIGHT_CUT_OFF = _( "Cut off" );
		static wxString PROPERTY_LIGHT_EXPONENT = _( "Exponent" );

		addProperty( grid, PROPERTY_CATEGORY_SPOT_LIGHT );
		addPropertyT( grid, PROPERTY_LIGHT_ATTENUATION, light.getAttenuation(), &light, &SpotLight::setAttenuation );
		addPropertyT( grid, PROPERTY_LIGHT_CUT_OFF, light.getCutOff(), &light, &SpotLight::setCutOff );
		addPropertyT( grid, PROPERTY_LIGHT_EXPONENT, light.getExponent(), &light, &SpotLight::setExponent );
	}

	void LightTreeItemProperty::doCreateShadowProperties( wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_SHADOW = _( "Shadow:" );
		static wxString PROPERTY_SHADOW_ENABLED = _( "Enable Shadows" );
		static wxString PROPERTY_SHADOW_TYPE = _( "Type" );
		static wxString PROPERTY_SHADOW_TYPE_NONE = _( "None" );
		static wxString PROPERTY_SHADOW_TYPE_RAW = _( "Raw" );
		static wxString PROPERTY_SHADOW_TYPE_PCF = _( "PCF" );
		static wxString PROPERTY_SHADOW_TYPE_VSM = _( "VSM" );
		static wxString PROPERTY_SHADOW_MIN_OFFSET = _( "Min. Offset" );
		static wxString PROPERTY_SHADOW_MAX_SLOPE_OFFSET = _( "Max. Slope Offset" );
		static wxString PROPERTY_SHADOW_MAX_VARIANCE = _( "Max. Variance" );
		static wxString PROPERTY_SHADOW_VARIANCE_BIAS = _( "Variance Bias" );
		static wxString PROPERTY_SHADOW_VOLUMETRIC_STEPS = _( "Volumetric Steps" );
		static wxString PROPERTY_SHADOW_VOLUMETRIC_SCATTERING_FACTOR = _( "Volumetric Scattering Factor" );
		static wxString PROPERTY_CATEGORY_GLOBAL_ILLUM = _( "Global Illumination" );
		static wxString PROPERTY_SHADOW_GLOBAL_ILLUM_TYPE = _( "GI Type" );
		static wxString PROPERTY_SHADOW_GLOBAL_ILLUM_TYPE_NONE = _( "None" );
		static wxString PROPERTY_SHADOW_GLOBAL_ILLUM_TYPE_RSM = _( "RSM" );
		static wxString PROPERTY_SHADOW_GLOBAL_ILLUM_TYPE_LPV = _( "LPV" );
		static wxString PROPERTY_SHADOW_GLOBAL_ILLUM_TYPE_LAYERED_LPV = _( "Layered LPV" );
		static wxString PROPERTY_SHADOW_RSM_INTENSITY = _( "Intensity" );
		static wxString PROPERTY_SHADOW_RSM_MAX_RADIUS = _( "Max. Radius" );
		static wxString PROPERTY_SHADOW_RSM_SAMPLE_COUNT = _( "Sample Count" );

		wxArrayString shadowChoices;
		shadowChoices.Add( PROPERTY_SHADOW_TYPE_NONE );
		shadowChoices.Add( PROPERTY_SHADOW_TYPE_RAW );
		shadowChoices.Add( PROPERTY_SHADOW_TYPE_PCF );
		shadowChoices.Add( PROPERTY_SHADOW_TYPE_VSM );
		wxArrayString giChoices;
		giChoices.Add( PROPERTY_SHADOW_GLOBAL_ILLUM_TYPE_NONE );
		giChoices.Add( PROPERTY_SHADOW_GLOBAL_ILLUM_TYPE_RSM );

		if ( m_light.getLightType() != LightType::ePoint )
		{
			giChoices.Add( PROPERTY_SHADOW_GLOBAL_ILLUM_TYPE_LPV );

			if ( m_light.getLightType() == LightType::eDirectional )
			{
				giChoices.Add( PROPERTY_SHADOW_GLOBAL_ILLUM_TYPE_LAYERED_LPV );
			}
		}

		addProperty( grid, PROPERTY_CATEGORY_SHADOW );
		addPropertyE( grid, PROPERTY_SHADOW_TYPE, shadowChoices, m_light.getShadowType()
			, [this]( ShadowType value ){ m_light.setShadowProducer( value != ShadowType::eNone );m_light.setShadowType( value ); } );
		addPropertyT( grid, PROPERTY_SHADOW_MIN_OFFSET, m_light.getCategory()->getShadowOffsets()[0], m_light.getCategory().get(), &LightCategory::setShadowMinOffset );
		addPropertyT( grid, PROPERTY_SHADOW_MAX_SLOPE_OFFSET, m_light.getCategory()->getShadowOffsets()[1], m_light.getCategory().get(), &LightCategory::setShadowMaxSlopeOffset );
		addPropertyT( grid, PROPERTY_SHADOW_MAX_VARIANCE, m_light.getCategory()->getShadowVariance()[0], m_light.getCategory().get(), &LightCategory::setShadowMaxVariance );
		addPropertyT( grid, PROPERTY_SHADOW_VARIANCE_BIAS, m_light.getCategory()->getShadowVariance()[1], m_light.getCategory().get(), &LightCategory::setShadowVarianceBias );

		if ( m_light.getLightType() == LightType::eDirectional )
		{
			addPropertyT( grid, PROPERTY_SHADOW_VOLUMETRIC_STEPS, m_light.getCategory()->getVolumetricSteps(), m_light.getCategory().get(), &LightCategory::setVolumetricSteps );
			addPropertyT( grid, PROPERTY_SHADOW_VOLUMETRIC_SCATTERING_FACTOR, m_light.getCategory()->getVolumetricScatteringFactor(), m_light.getCategory().get(), &LightCategory::setVolumetricScatteringFactor );
		}

		auto & rsmConfig = m_light.getRsmConfig();
		addProperty( grid, PROPERTY_CATEGORY_GLOBAL_ILLUM );
		addPropertyET( grid, PROPERTY_SHADOW_GLOBAL_ILLUM_TYPE, giChoices, m_light.getGlobalIlluminationType(), &m_light, &Light::setGlobalIlluminationType );
		addPropertyT( grid, PROPERTY_SHADOW_RSM_INTENSITY, &rsmConfig.intensity );
		addPropertyT( grid, PROPERTY_SHADOW_RSM_MAX_RADIUS, &rsmConfig.maxRadius );
		addPropertyT( grid, PROPERTY_SHADOW_RSM_SAMPLE_COUNT, &rsmConfig.sampleCount );
	}
}
