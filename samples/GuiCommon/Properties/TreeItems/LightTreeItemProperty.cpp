#include "GuiCommon/Properties/TreeItems/LightTreeItemProperty.hpp"

#include "GuiCommon/Properties/Math/PointProperties.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Event/Frame/FunctorEvent.hpp>
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
	namespace
	{
		static wxString PROPERTY_CATEGORY_LIGHT = _( "Light:" );
		static wxString PROPERTY_CATEGORY_POINT_LIGHT = _( "Point Light" );
		static wxString PROPERTY_CATEGORY_SPOT_LIGHT = _( "Spot Light" );
		static wxString PROPERTY_LIGHT_COLOUR = _( "Colour" );
		static wxString PROPERTY_LIGHT_INTENSITY = _( "Intensities" );
		static wxString PROPERTY_LIGHT_ATTENUATION = _( "Attenuation" );
		static wxString PROPERTY_LIGHT_CUT_OFF = _( "Cut off" );
		static wxString PROPERTY_LIGHT_EXPONENT = _( "Exponent" );
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
	}

	LightTreeItemProperty::LightTreeItemProperty( bool editable, Light & light )
		: TreeItemProperty( light.getScene()->getEngine(), editable, ePROPERTY_DATA_TYPE_LIGHT )
		, m_light( light )
	{
		PROPERTY_CATEGORY_LIGHT = _( "Light: " );
		PROPERTY_CATEGORY_POINT_LIGHT = _( "Point Light" );
		PROPERTY_CATEGORY_SPOT_LIGHT = _( "Spot Light" );
		PROPERTY_LIGHT_COLOUR = _( "Colour" );
		PROPERTY_LIGHT_INTENSITY = _( "Intensities" );
		PROPERTY_LIGHT_ATTENUATION = _( "Attenuation" );
		PROPERTY_LIGHT_CUT_OFF = _( "Cut off" );
		PROPERTY_LIGHT_EXPONENT = _( "Exponent" );
		PROPERTY_CATEGORY_SHADOW = _( "Shadow:" );
		PROPERTY_SHADOW_ENABLED = _( "Enabled" );
		PROPERTY_SHADOW_TYPE = _( "Type" );
		PROPERTY_SHADOW_TYPE_NONE = _( "None" );
		PROPERTY_SHADOW_TYPE_RAW = _( "Raw" );
		PROPERTY_SHADOW_TYPE_PCF = _( "PCF" );
		PROPERTY_SHADOW_TYPE_VSM = _( "VSM" );
		PROPERTY_SHADOW_MIN_OFFSET = _( "Min. Offset" );
		PROPERTY_SHADOW_MAX_SLOPE_OFFSET = _( "Max. Slope Offset" );
		PROPERTY_SHADOW_MAX_VARIANCE = _( "Max. Variance" );
		PROPERTY_SHADOW_VARIANCE_BIAS = _( "Variance Bias" );
		PROPERTY_SHADOW_VOLUMETRIC_STEPS = _( "Volumetric Steps" );
		PROPERTY_SHADOW_VOLUMETRIC_SCATTERING_FACTOR = _( "Volumetric Scattering Factor" );
		PROPERTY_CATEGORY_GLOBAL_ILLUM = _( "Global Illumination" );
		PROPERTY_SHADOW_GLOBAL_ILLUM_TYPE_NONE = _( "None" );
		PROPERTY_SHADOW_GLOBAL_ILLUM_TYPE_RSM = _( "RSM" );
		PROPERTY_SHADOW_GLOBAL_ILLUM_TYPE_LPV = _( "LPV" );
		PROPERTY_SHADOW_GLOBAL_ILLUM_TYPE_LAYERED_LPV = _( "Layered LPV" );
		PROPERTY_SHADOW_GLOBAL_ILLUM_TYPE = _( "GI Type" );
		PROPERTY_SHADOW_RSM_INTENSITY = _( "Intensity" );
		PROPERTY_SHADOW_RSM_MAX_RADIUS = _( "Max. Radius" );
		PROPERTY_SHADOW_RSM_SAMPLE_COUNT = _( "Sample Count" );

		CreateTreeItemMenu();
	}

	LightTreeItemProperty::~LightTreeItemProperty()
	{
	}

	void LightTreeItemProperty::doCreateProperties( wxPGEditor * editor, wxPropertyGrid * grid )
	{
		grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_LIGHT + wxString( m_light.getName() ) ) );
		grid->Append( new wxColourProperty( PROPERTY_LIGHT_COLOUR ) )->SetValue( wxVariant( wxColour( toBGRPacked( RgbColour::fromRGB( m_light.getColour() ) ) ) ) );
		grid->Append( new Point2fProperty( PROPERTY_LIGHT_INTENSITY ) )->SetValue( WXVARIANT( m_light.getIntensity() ) );

		switch ( m_light.getLightType() )
		{
		case LightType::eDirectional:
			doCreateDirectionalLightProperties( grid, m_light.getDirectionalLight() );
			break;

		case LightType::ePoint:
			doCreatePointLightProperties( grid, m_light.getPointLight() );
			break;

		case LightType::eSpot:
			doCreateSpotLightProperties( grid, m_light.getSpotLight() );
			break;
		}

		doCreateShadowProperties( grid );
	}

	void LightTreeItemProperty::doPropertyChange( wxPropertyGridEvent & event )
	{
		wxPGProperty * property = event.GetProperty();

		if ( property )
		{
			wxColour colour;

			if ( property->GetName() == PROPERTY_LIGHT_COLOUR )
			{
				colour << property->GetValue();
				onColourChange( RgbColour::fromBGR( colour.GetRGB() ) );
			}
			else if ( property->GetName() == PROPERTY_LIGHT_INTENSITY )
			{
				onIntensityChange( Point2fRefFromVariant( property->GetValue() ) );
			}
			else if ( property->GetName() == PROPERTY_LIGHT_ATTENUATION )
			{
				CU_Assert( m_light.getLightType() != LightType::eDirectional
					, "Only for non directional light types" );
				onAttenuationChange( Point3fRefFromVariant( property->GetValue() ) );
			}
			else if ( property->GetName() == PROPERTY_LIGHT_CUT_OFF )
			{
				CU_Assert( m_light.getLightType() == LightType::eSpot
					, "Only for spot light types" );
				onCutOffChange( property->GetValue() );
			}
			else if ( property->GetName() == PROPERTY_LIGHT_EXPONENT )
			{
				CU_Assert( m_light.getLightType() == LightType::eSpot
					, "Only for spot light types" );
				onExponentChange( property->GetValue() );
			}
			else if ( property->GetName() == PROPERTY_SHADOW_TYPE )
			{
				if ( property->GetValueAsString() == PROPERTY_SHADOW_TYPE_NONE )
				{
					onShadowTypeChange( ShadowType::eNone );
				}
				else if ( property->GetValueAsString() == PROPERTY_SHADOW_TYPE_RAW )
				{
					onShadowTypeChange( ShadowType::eRaw );
				}
				else if ( property->GetValueAsString() == PROPERTY_SHADOW_TYPE_PCF )
				{
					onShadowTypeChange( ShadowType::ePCF );
				}
				else if ( property->GetValueAsString() == PROPERTY_SHADOW_TYPE_VSM )
				{
					onShadowTypeChange( ShadowType::eVariance );
				}
			}
			else if ( property->GetName() == PROPERTY_SHADOW_MIN_OFFSET )
			{
				onShadowMinOffsetChange( property->GetValue() );
			}
			else if ( property->GetName() == PROPERTY_SHADOW_MAX_SLOPE_OFFSET )
			{
				onShadowMaxSlopeOffsetChange( property->GetValue() );
			}
			else if ( property->GetName() == PROPERTY_SHADOW_MAX_VARIANCE )
			{
				onShadowMaxVarianceChange( property->GetValue() );
			}
			else if ( property->GetName() == PROPERTY_SHADOW_VARIANCE_BIAS )
			{
				onShadowVarianceBiasChange( property->GetValue() );
			}
			else if ( property->GetName() == PROPERTY_SHADOW_VOLUMETRIC_STEPS )
			{
				CU_Assert( m_light.getLightType() == LightType::eDirectional
					, "Only for directional light types" );
				onShadowVolumetricStepsChange( property->GetValue() );
			}
			else if ( property->GetName() == PROPERTY_SHADOW_VOLUMETRIC_SCATTERING_FACTOR )
			{
				CU_Assert( m_light.getLightType() == LightType::eDirectional
					, "Only for directional light types" );
				onShadowVolumetricScatteringFactorChange( property->GetValue() );
			}
			else if ( property->GetName() == PROPERTY_SHADOW_GLOBAL_ILLUM_TYPE )
			{
				if ( property->GetValueAsString() == PROPERTY_SHADOW_GLOBAL_ILLUM_TYPE_NONE )
				{
					onGiTypeChange( GlobalIlluminationType::eNone );
				}
				else if ( property->GetValueAsString() == PROPERTY_SHADOW_GLOBAL_ILLUM_TYPE_RSM )
				{
					onGiTypeChange( GlobalIlluminationType::eRsm );
				}
				else if ( property->GetValueAsString() == PROPERTY_SHADOW_GLOBAL_ILLUM_TYPE_LPV )
				{
					onGiTypeChange( GlobalIlluminationType::eLpv );
				}
				else if ( property->GetValueAsString() == PROPERTY_SHADOW_GLOBAL_ILLUM_TYPE_LAYERED_LPV )
				{
					onGiTypeChange( GlobalIlluminationType::eLayeredLpv );
				}
			}
			else if ( property->GetName() == PROPERTY_SHADOW_RSM_INTENSITY )
			{
				onRsmIntensityChange( property->GetValue() );
			}
			else if ( property->GetName() == PROPERTY_SHADOW_RSM_MAX_RADIUS )
			{
				onRsmMaxRadiusChange( property->GetValue() );
			}
			else if ( property->GetName() == PROPERTY_SHADOW_RSM_SAMPLE_COUNT )
			{
				onRsmSampleCountChange( property->GetValue() );
			}
		}
	}

	void LightTreeItemProperty::doCreateDirectionalLightProperties( wxPropertyGrid * grid, DirectionalLightSPtr light )
	{
	}

	void LightTreeItemProperty::doCreatePointLightProperties( wxPropertyGrid * grid, PointLightSPtr light )
	{
		grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_POINT_LIGHT ) );
		grid->Append( new Point3fProperty( GC_POINT_XYZ, PROPERTY_LIGHT_ATTENUATION ) )->SetValue( WXVARIANT( light->getAttenuation() ) );
	}

	void LightTreeItemProperty::doCreateSpotLightProperties( wxPropertyGrid * grid, SpotLightSPtr light )
	{
		grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_SPOT_LIGHT ) );
		grid->Append( new Point3fProperty( GC_POINT_XYZ, PROPERTY_LIGHT_ATTENUATION ) )->SetValue( WXVARIANT( light->getAttenuation() ) );
		grid->Append( new wxFloatProperty( PROPERTY_LIGHT_CUT_OFF ) )->SetValue( light->getCutOff().degrees() );
		grid->Append( new wxFloatProperty( PROPERTY_LIGHT_EXPONENT ) )->SetValue( light->getExponent() );
	}

	void LightTreeItemProperty::doCreateShadowProperties( wxPropertyGrid * grid )
	{
		wxString selected;
		wxPGChoices shadowChoices;
		shadowChoices.Add( PROPERTY_SHADOW_TYPE_NONE );
		shadowChoices.Add( PROPERTY_SHADOW_TYPE_RAW );
		shadowChoices.Add( PROPERTY_SHADOW_TYPE_PCF );
		shadowChoices.Add( PROPERTY_SHADOW_TYPE_VSM );
		wxPGChoices giChoices;
		giChoices.Add( PROPERTY_SHADOW_GLOBAL_ILLUM_TYPE_NONE );
		giChoices.Add( PROPERTY_SHADOW_GLOBAL_ILLUM_TYPE_RSM );
		giChoices.Add( PROPERTY_SHADOW_GLOBAL_ILLUM_TYPE_LPV );
		selected = shadowChoices.GetLabels()[size_t( m_light.getShadowType() )];

		grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_SHADOW ) );
		grid->Append( new wxEnumProperty( PROPERTY_SHADOW_TYPE, PROPERTY_SHADOW_TYPE, shadowChoices ) )->SetValue( selected );
		grid->Append( new wxFloatProperty( PROPERTY_SHADOW_MIN_OFFSET ) )->SetValue( m_light.getCategory()->getShadowOffsets()[0] );
		grid->Append( new wxFloatProperty( PROPERTY_SHADOW_MAX_SLOPE_OFFSET ) )->SetValue( m_light.getCategory()->getShadowOffsets()[1] );
		grid->Append( new wxFloatProperty( PROPERTY_SHADOW_MAX_VARIANCE ) )->SetValue( m_light.getCategory()->getShadowVariance()[0] );
		grid->Append( new wxFloatProperty( PROPERTY_SHADOW_VARIANCE_BIAS ) )->SetValue( m_light.getCategory()->getShadowVariance()[1] );

		if ( m_light.getLightType() == LightType::eDirectional )
		{
			grid->Append( new wxIntProperty( PROPERTY_SHADOW_VOLUMETRIC_STEPS ) )->SetValue( long( m_light.getCategory()->getVolumetricSteps() ) );
			grid->Append( new wxFloatProperty( PROPERTY_SHADOW_VOLUMETRIC_SCATTERING_FACTOR ) )->SetValue( m_light.getCategory()->getVolumetricScatteringFactor() );

			giChoices.Add( PROPERTY_SHADOW_GLOBAL_ILLUM_TYPE_LAYERED_LPV );
		}

		auto & rsmConfig = m_light.getRsmConfig();
		selected = giChoices.GetLabels()[size_t( m_light.getGlobalIlluminationType() )];
		grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_GLOBAL_ILLUM ) );
		grid->Append( new wxEnumProperty( PROPERTY_SHADOW_GLOBAL_ILLUM_TYPE, PROPERTY_SHADOW_GLOBAL_ILLUM_TYPE, giChoices ) )->SetValue( selected );
		grid->Append( new wxFloatProperty( PROPERTY_SHADOW_RSM_INTENSITY, PROPERTY_SHADOW_RSM_INTENSITY ) )->SetValue( rsmConfig.intensity.value() );
		grid->Append( new wxFloatProperty( PROPERTY_SHADOW_RSM_MAX_RADIUS, PROPERTY_SHADOW_RSM_MAX_RADIUS ) )->SetValue( rsmConfig.maxRadius.value() );
		grid->Append( new wxIntProperty( PROPERTY_SHADOW_RSM_SAMPLE_COUNT, PROPERTY_SHADOW_RSM_SAMPLE_COUNT ) )->SetValue( int32_t( rsmConfig.sampleCount.value().value() ) );
	}

	void LightTreeItemProperty::onColourChange( RgbColour const & value )
	{
		doApplyChange( [value, this]()
		{
			m_light.setColour( value );
		} );
	}

	void LightTreeItemProperty::onIntensityChange( Point2f const & value )
	{
		float d = value[0];
		float s = value[1];

		doApplyChange( [d, s, this]()
		{
			m_light.setIntensity( d, s );
		} );
	}

	void LightTreeItemProperty::onAttenuationChange( Point3f const & value )
	{
		float x = value[0];
		float y = value[1];
		float z = value[2];

		doApplyChange( [x, y, z, this]()
		{
			Point3f value( x, y, z );

			if ( m_light.getLightType() == LightType::ePoint )
			{
				m_light.getPointLight()->setAttenuation( value );
			}
			else
			{
				m_light.getSpotLight()->setAttenuation( value );
			}
		} );
	}

	void LightTreeItemProperty::onCutOffChange( double value )
	{
		doApplyChange( [value, this]()
		{
			m_light.getSpotLight()->setCutOff( Angle::fromDegrees( value ) );
		} );
	}

	void LightTreeItemProperty::onExponentChange( double value )
	{
		doApplyChange( [value, this]()
		{
			m_light.getSpotLight()->setExponent( float( value ) );
		} );
	}

	void LightTreeItemProperty::onShadowTypeChange( ShadowType value )
	{
		doApplyChange( [value, this]()
			{
				m_light.setShadowProducer( value != ShadowType::eNone );
				m_light.setShadowType( value );
			} );
	}

	void LightTreeItemProperty::onShadowMinOffsetChange( double value )
	{
		doApplyChange( [value, this]()
			{
				m_light.setShadowMinOffset( float( value ) );
			} );
	}

	void LightTreeItemProperty::onShadowMaxSlopeOffsetChange( double value )
	{
		doApplyChange( [value, this]()
			{
				m_light.setShadowMaxSlopeOffset( float( value ) );
			} );
	}

	void LightTreeItemProperty::onShadowMaxVarianceChange( double value )
	{
		doApplyChange( [value, this]()
			{
				m_light.setShadowMaxVariance( float( value ) );
			} );
	}

	void LightTreeItemProperty::onShadowVarianceBiasChange( double value )
	{
		doApplyChange( [value, this]()
			{
				m_light.setShadowVarianceBias( float( value ) );
			} );
	}

	void LightTreeItemProperty::onShadowVolumetricStepsChange( long value )
	{
		doApplyChange( [value, this]()
			{
				m_light.setVolumetricSteps( uint32_t( value ) );
			} );
	}

	void LightTreeItemProperty::onShadowVolumetricScatteringFactorChange( double value )
	{
		doApplyChange( [value, this]()
			{
				m_light.setVolumetricScatteringFactor( float( value ) );
			} );
	}

	void LightTreeItemProperty::onGiTypeChange( castor3d::GlobalIlluminationType value )
	{
		doApplyChange( [value, this]()
			{
				m_light.setGlobalIlluminationType( value );
			} );
	}

	void LightTreeItemProperty::onRsmIntensityChange( double value )
	{
		doApplyChange( [value, this]()
			{
				m_light.getRsmConfig().intensity = float( value );
			} );
	}

	void LightTreeItemProperty::onRsmMaxRadiusChange( double value )
	{
		doApplyChange( [value, this]()
			{
				m_light.getRsmConfig().maxRadius = float( value );
			} );
	}

	void LightTreeItemProperty::onRsmSampleCountChange( long value )
	{
		doApplyChange( [value, this]()
			{
				auto range = m_light.getRsmConfig().sampleCount.value();
				range = value;
				m_light.getRsmConfig().sampleCount = range;
			} );
	}
}
