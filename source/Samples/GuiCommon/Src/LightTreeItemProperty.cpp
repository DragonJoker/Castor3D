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

	LightTreeItemProperty::LightTreeItemProperty( bool p_editable, Light & p_light )
		: TreeItemProperty( p_light.getScene()->getEngine(), p_editable, ePROPERTY_DATA_TYPE_LIGHT )
		, m_light( p_light )
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

	void LightTreeItemProperty::doCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )
	{
		p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_LIGHT + wxString( m_light.getName() ) ) );
		p_grid->Append( new wxColourProperty( PROPERTY_LIGHT_COLOUR ) )->SetValue( wxVariant( wxColour( toBGRPacked( RgbColour::fromRGB( m_light.getColour() ) ) ) ) );
		p_grid->Append( new Point2fProperty( PROPERTY_LIGHT_INTENSITY ) )->SetValue( WXVARIANT( m_light.getIntensity() ) );

		switch ( m_light.getLightType() )
		{
		case LightType::eDirectional:
			doCreateDirectionalLightProperties( p_grid, m_light.getDirectionalLight() );
			break;

		case LightType::ePoint:
			doCreatePointLightProperties( p_grid, m_light.getPointLight() );
			break;

		case LightType::eSpot:
			doCreateSpotLightProperties( p_grid, m_light.getSpotLight() );
			break;
		}
	}

	void LightTreeItemProperty::doPropertyChange( wxPropertyGridEvent & p_event )
	{
		wxPGProperty * property = p_event.GetProperty();

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

	void LightTreeItemProperty::doCreateDirectionalLightProperties( wxPropertyGrid * p_grid, DirectionalLightSPtr p_light )
	{
	}

	void LightTreeItemProperty::doCreatePointLightProperties( wxPropertyGrid * p_grid, PointLightSPtr p_light )
	{
		p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_POINT_LIGHT ) );
		p_grid->Append( new Point3rProperty( GC_POINT_XYZ, PROPERTY_LIGHT_ATTENUATION ) )->SetValue( WXVARIANT( p_light->getAttenuation() ) );
	}

	void LightTreeItemProperty::doCreateSpotLightProperties( wxPropertyGrid * p_grid, SpotLightSPtr p_light )
	{
		p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_SPOT_LIGHT ) );
		p_grid->Append( new Point3rProperty( GC_POINT_XYZ, PROPERTY_LIGHT_ATTENUATION ) )->SetValue( WXVARIANT( p_light->getAttenuation() ) );
		p_grid->Append( new wxFloatProperty( PROPERTY_LIGHT_CUT_OFF ) )->SetValue( p_light->getCutOff().degrees() );
		p_grid->Append( new wxFloatProperty( PROPERTY_LIGHT_EXPONENT ) )->SetValue( p_light->getExponent() );
	}

	void LightTreeItemProperty::OnColourChange( RgbColour const & p_value )
	{
		doApplyChange( [p_value, this]()
		{
			m_light.setColour( p_value );
		} );
	}

	void LightTreeItemProperty::OnIntensityChange( Point2f const & p_value )
	{
		float d = p_value[0];
		float s = p_value[1];

		doApplyChange( [d, s, this]()
		{
			m_light.setIntensity( d, s );
		} );
	}

	void LightTreeItemProperty::OnAttenuationChange( Point3f const & p_value )
	{
		float x = p_value[0];
		float y = p_value[1];
		float z = p_value[2];

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

	void LightTreeItemProperty::OnCutOffChange( double p_value )
	{
		doApplyChange( [p_value, this]()
		{
			m_light.getSpotLight()->setCutOff( Angle::fromDegrees( p_value ) );
		} );
	}

	void LightTreeItemProperty::OnExponentChange( double p_value )
	{
		doApplyChange( [p_value, this]()
		{
			m_light.getSpotLight()->setExponent( float( p_value ) );
		} );
	}
}
