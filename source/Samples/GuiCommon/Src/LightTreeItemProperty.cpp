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

using namespace Castor3D;
using namespace Castor;

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
		: TreeItemProperty( p_light.GetScene()->GetEngine(), p_editable, ePROPERTY_DATA_TYPE_LIGHT )
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

	void LightTreeItemProperty::DoCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )
	{
		p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_LIGHT + wxString( m_light.GetName() ) ) );
		p_grid->Append( new wxColourProperty( PROPERTY_LIGHT_COLOUR ) )->SetValue( wxVariant( wxColour( bgr_packed( Colour::from_rgb( m_light.GetColour() ) ) ) ) );
		p_grid->Append( new Point2fProperty( PROPERTY_LIGHT_INTENSITY ) )->SetValue( WXVARIANT( m_light.GetIntensity() ) );

		switch ( m_light.GetLightType() )
		{
		case LightType::eDirectional:
			DoCreateDirectionalLightProperties( p_grid, m_light.GetDirectionalLight() );
			break;

		case LightType::ePoint:
			DoCreatePointLightProperties( p_grid, m_light.GetPointLight() );
			break;

		case LightType::eSpot:
			DoCreateSpotLightProperties( p_grid, m_light.GetSpotLight() );
			break;
		}
	}

	void LightTreeItemProperty::DoPropertyChange( wxPropertyGridEvent & p_event )
	{
		wxPGProperty * l_property = p_event.GetProperty();

		if ( l_property )
		{
			wxColour l_colour;

			if ( l_property->GetName() == PROPERTY_LIGHT_COLOUR )
			{
				l_colour << l_property->GetValue();
				OnColourChange( Colour::from_bgr( l_colour.GetRGB() ) );
			}
			else if ( l_property->GetName() == PROPERTY_LIGHT_INTENSITY )
			{
				OnIntensityChange( Point2fRefFromVariant( l_property->GetValue() ) );
			}
			else if ( m_light.GetLightType() != LightType::eDirectional )
			{
				if ( l_property->GetName() == PROPERTY_LIGHT_ATTENUATION )
				{
					OnAttenuationChange( Point3fRefFromVariant( l_property->GetValue() ) );
				}
				else if ( m_light.GetLightType() == LightType::eSpot )
				{
					if ( l_property->GetName() == PROPERTY_LIGHT_CUT_OFF )
					{
						OnCutOffChange( l_property->GetValue() );
					}
					else if ( l_property->GetName() == PROPERTY_LIGHT_EXPONENT )
					{
						OnExponentChange( l_property->GetValue() );
					}
				}
			}
		}
	}

	void LightTreeItemProperty::DoCreateDirectionalLightProperties( wxPropertyGrid * p_grid, DirectionalLightSPtr p_light )
	{
	}

	void LightTreeItemProperty::DoCreatePointLightProperties( wxPropertyGrid * p_grid, PointLightSPtr p_light )
	{
		p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_POINT_LIGHT ) );
		p_grid->Append( new Point3rProperty( GC_POINT_XYZ, PROPERTY_LIGHT_ATTENUATION ) )->SetValue( WXVARIANT( p_light->GetAttenuation() ) );
	}

	void LightTreeItemProperty::DoCreateSpotLightProperties( wxPropertyGrid * p_grid, SpotLightSPtr p_light )
	{
		p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_SPOT_LIGHT ) );
		p_grid->Append( new Point3rProperty( GC_POINT_XYZ, PROPERTY_LIGHT_ATTENUATION ) )->SetValue( WXVARIANT( p_light->GetAttenuation() ) );
		p_grid->Append( new wxFloatProperty( PROPERTY_LIGHT_CUT_OFF ) )->SetValue( p_light->GetCutOff().degrees() );
		p_grid->Append( new wxFloatProperty( PROPERTY_LIGHT_EXPONENT ) )->SetValue( p_light->GetExponent() );
	}

	void LightTreeItemProperty::OnColourChange( Colour const & p_value )
	{
		DoApplyChange( [p_value, this]()
		{
			m_light.SetColour( p_value );
		} );
	}

	void LightTreeItemProperty::OnIntensityChange( Point2f const & p_value )
	{
		float d = p_value[0];
		float s = p_value[1];

		DoApplyChange( [d, s, this]()
		{
			m_light.SetIntensity( d, s );
		} );
	}

	void LightTreeItemProperty::OnAttenuationChange( Point3f const & p_value )
	{
		float x = p_value[0];
		float y = p_value[1];
		float z = p_value[2];

		DoApplyChange( [x, y, z, this]()
		{
			Point3f l_value( x, y, z );

			if ( m_light.GetLightType() == LightType::ePoint )
			{
				m_light.GetPointLight()->SetAttenuation( l_value );
			}
			else
			{
				m_light.GetSpotLight()->SetAttenuation( l_value );
			}
		} );
	}

	void LightTreeItemProperty::OnCutOffChange( double p_value )
	{
		DoApplyChange( [p_value, this]()
		{
			m_light.GetSpotLight()->SetCutOff( Angle::from_degrees( p_value ) );
		} );
	}

	void LightTreeItemProperty::OnExponentChange( double p_value )
	{
		DoApplyChange( [p_value, this]()
		{
			m_light.GetSpotLight()->SetExponent( float( p_value ) );
		} );
	}
}
