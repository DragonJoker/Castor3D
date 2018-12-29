#include "LightTreeItemProperty.hpp"

#include <Engine.hpp>
#include <Event/Frame/FunctorEvent.hpp>
#include <Scene/Scene.hpp>
#include <Scene/Light/DirectionalLight.hpp>
#include <Scene/Light/SpotLight.hpp>
#include <Scene/Light/Light.hpp>
#include <Scene/Light/PointLight.hpp>

#include "PointProperties.hpp"
#include <wx/propgrid/advprops.h>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	namespace
	{
		static wxString PROPERTY_CATEGORY_LIGHT = _( "Light: " );
		static wxString PROPERTY_CATEGORY_POINT_LIGHT = _( "Point Light" );
		static wxString PROPERTY_CATEGORY_SPOT_LIGHT = _( "Spot Light" );
		static wxString PROPERTY_LIGHT_COLOUR = _( "Colour" );
		static wxString PROPERTY_LIGHT_INTENSITY = _( "Intensity" );
		static wxString PROPERTY_LIGHT_ATTENUATION = _( "Attenuation" );
		static wxString PROPERTY_LIGHT_CUT_OFF = _( "Cut off" );
		static wxString PROPERTY_LIGHT_EXPONENT = _( "Exponent" );
	}

	LightTreeItemProperty::LightTreeItemProperty( bool editable, Light & light )
		: TreeItemProperty( light.getScene()->getEngine(), editable, ePROPERTY_DATA_TYPE_LIGHT )
		, m_light( light )
	{
		PROPERTY_CATEGORY_LIGHT = _( "Light: " );
		PROPERTY_CATEGORY_POINT_LIGHT = _( "Point Light" );
		PROPERTY_CATEGORY_SPOT_LIGHT = _( "Spot Light" );
		PROPERTY_LIGHT_COLOUR = _( "Colour" );
		PROPERTY_LIGHT_INTENSITY = _( "Intensity" );
		PROPERTY_LIGHT_ATTENUATION = _( "Attenuation" );
		PROPERTY_LIGHT_CUT_OFF = _( "Cut off" );
		PROPERTY_LIGHT_EXPONENT = _( "Exponent" );

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
				OnColourChange( RgbColour::fromBGR( colour.GetRGB() ) );
			}
			else if ( property->GetName() == PROPERTY_LIGHT_INTENSITY )
			{
				OnIntensityChange( Point2fRefFromVariant( property->GetValue() ) );
			}
			else if ( m_light.getLightType() != LightType::eDirectional )
			{
				if ( property->GetName() == PROPERTY_LIGHT_ATTENUATION )
				{
					OnAttenuationChange( Point3fRefFromVariant( property->GetValue() ) );
				}
				else if ( m_light.getLightType() == LightType::eSpot )
				{
					if ( property->GetName() == PROPERTY_LIGHT_CUT_OFF )
					{
						OnCutOffChange( property->GetValue() );
					}
					else if ( property->GetName() == PROPERTY_LIGHT_EXPONENT )
					{
						OnExponentChange( property->GetValue() );
					}
				}
			}
		}
	}

	void LightTreeItemProperty::doCreateDirectionalLightProperties( wxPropertyGrid * grid, DirectionalLightSPtr light )
	{
	}

	void LightTreeItemProperty::doCreatePointLightProperties( wxPropertyGrid * grid, PointLightSPtr light )
	{
		grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_POINT_LIGHT ) );
		grid->Append( new Point3rProperty( GC_POINT_XYZ, PROPERTY_LIGHT_ATTENUATION ) )->SetValue( WXVARIANT( light->getAttenuation() ) );
	}

	void LightTreeItemProperty::doCreateSpotLightProperties( wxPropertyGrid * grid, SpotLightSPtr light )
	{
		grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_SPOT_LIGHT ) );
		grid->Append( new Point3rProperty( GC_POINT_XYZ, PROPERTY_LIGHT_ATTENUATION ) )->SetValue( WXVARIANT( light->getAttenuation() ) );
		grid->Append( new wxFloatProperty( PROPERTY_LIGHT_CUT_OFF ) )->SetValue( light->getCutOff().degrees() );
		grid->Append( new wxFloatProperty( PROPERTY_LIGHT_EXPONENT ) )->SetValue( light->getExponent() );
	}

	void LightTreeItemProperty::OnColourChange( RgbColour const & value )
	{
		doApplyChange( [value, this]()
		{
			m_light.setColour( value );
		} );
	}

	void LightTreeItemProperty::OnIntensityChange( Point2f const & value )
	{
		float d = value[0];
		float s = value[1];

		doApplyChange( [d, s, this]()
		{
			m_light.setIntensity( d, s );
		} );
	}

	void LightTreeItemProperty::OnAttenuationChange( Point3f const & value )
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

	void LightTreeItemProperty::OnCutOffChange( double value )
	{
		doApplyChange( [value, this]()
		{
			m_light.getSpotLight()->setCutOff( Angle::fromDegrees( value ) );
		} );
	}

	void LightTreeItemProperty::OnExponentChange( double value )
	{
		doApplyChange( [value, this]()
		{
			m_light.getSpotLight()->setExponent( float( value ) );
		} );
	}
}
