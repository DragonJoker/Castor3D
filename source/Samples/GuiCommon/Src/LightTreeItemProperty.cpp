#include "LightTreeItemProperty.hpp"

#include <Engine.hpp>
#include <FunctorEvent.hpp>
#include <Light.hpp>

#include "AdditionalProperties.hpp"
#include <wx/propgrid/advprops.h>

using namespace Castor3D;
using namespace Castor;

namespace GuiCommon
{
	namespace
	{
		static const wxString PROPERTY_CATEGORY_LIGHT = _( "Light: " );
		static const wxString PROPERTY_CATEGORY_POINT_LIGHT = _( "Point Light" );
		static const wxString PROPERTY_CATEGORY_SPOT_LIGHT = _( "Spot Light" );
		static const wxString PROPERTY_LIGHT_AMBIENT = _( "Ambient" );
		static const wxString PROPERTY_LIGHT_DIFFUSE = _( "Diffuse" );
		static const wxString PROPERTY_LIGHT_SPECULAR = _( "Specular" );
		static const wxString PROPERTY_LIGHT_ATTENUATION = _( "Attenuation" );
		static const wxString PROPERTY_LIGHT_CUT_OFF = _( "Cut off" );
		static const wxString PROPERTY_LIGHT_EXPONENT = _( "Exponent" );
	}

	LightTreeItemProperty::LightTreeItemProperty( bool p_editable, LightSPtr p_light )
		: TreeItemProperty( p_editable, ePROPERTY_DATA_TYPE_LIGHT )
		, m_light( p_light )
	{
	}

	LightTreeItemProperty::~LightTreeItemProperty()
	{
	}

	void LightTreeItemProperty::CreateProperties( wxPropertyGrid * p_grid )
	{
		LightSPtr l_light = GetLight();

		if ( l_light )
		{
			p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_LIGHT + wxString( l_light->GetName() ) ) );
			Colour l_colour = Colour::from_rgba( l_light->GetAmbient() );
			p_grid->Append( new wxColourProperty( PROPERTY_LIGHT_AMBIENT ) )->SetValue( wxVariant( wxColour( l_colour.to_bgr() ) ) );
			l_colour = Colour::from_rgba( l_light->GetDiffuse() );
			p_grid->Append( new wxColourProperty( PROPERTY_LIGHT_DIFFUSE ) )->SetValue( wxVariant( wxColour( l_colour.to_bgr() ) ) );
			l_colour = Colour::from_rgba( l_light->GetSpecular() );
			p_grid->Append( new wxColourProperty( PROPERTY_LIGHT_SPECULAR ) )->SetValue( wxVariant( wxColour( l_colour.to_bgr() ) ) );

			switch ( l_light->GetLightType() )
			{
			case eLIGHT_TYPE_DIRECTIONAL:
				DoCreateDirectionalLightProperties( p_grid, l_light->GetDirectionalLight() );
				break;

			case eLIGHT_TYPE_POINT:
				DoCreatePointLightProperties( p_grid, l_light->GetPointLight() );
				break;

			case eLIGHT_TYPE_SPOT:
				DoCreateSpotLightProperties( p_grid, l_light->GetSpotLight() );
				break;
			}
		}
	}

	void LightTreeItemProperty::OnPropertyChange( wxPropertyGridEvent & p_event )
	{
		LightSPtr l_light = GetLight();
		wxPGProperty * l_property = p_event.GetProperty();

		if ( l_property && l_light )
		{
			wxColour l_colour;

			if ( l_property->GetName() == PROPERTY_LIGHT_AMBIENT )
			{
				l_colour << l_property->GetValue();
				OnAmbientColourChange( Colour::from_bgr( l_colour.GetRGB() ) );
			}
			else if ( l_property->GetName() == PROPERTY_LIGHT_DIFFUSE )
			{
				l_colour << l_property->GetValue();
				OnDiffuseColourChange( Colour::from_bgr( l_colour.GetRGB() ) );
			}
			else if ( l_property->GetName() == PROPERTY_LIGHT_SPECULAR )
			{
				l_colour << l_property->GetValue();
				OnSpecularColourChange( Colour::from_bgr( l_colour.GetRGB() ) );
			}
			else if ( l_light->GetLightType() != eLIGHT_TYPE_DIRECTIONAL )
			{
				if ( l_property->GetName() == PROPERTY_LIGHT_ATTENUATION )
				{
					OnAttenuationChange( Point3fRefFromVariant( l_property->GetValue() ) );
				}
				else if ( l_light->GetLightType() == eLIGHT_TYPE_SPOT )
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
		p_grid->Append( new Point3rProperty( PROPERTY_LIGHT_ATTENUATION ) )->SetValue( wxVariant( p_light->GetAttenuation() ) );
	}

	void LightTreeItemProperty::DoCreateSpotLightProperties( wxPropertyGrid * p_grid, SpotLightSPtr p_light )
	{
		p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_SPOT_LIGHT ) );
		p_grid->Append( new Point3rProperty( PROPERTY_LIGHT_ATTENUATION ) )->SetValue( wxVariant( p_light->GetAttenuation() ) );
		p_grid->Append( new wxFloatProperty( PROPERTY_LIGHT_CUT_OFF ) )->SetValue( p_light->GetCutOff() );
		p_grid->Append( new wxFloatProperty( PROPERTY_LIGHT_EXPONENT ) )->SetValue( p_light->GetExponent() );
	}

	void LightTreeItemProperty::OnAmbientColourChange( Colour const & p_value )
	{
		LightSPtr l_light = GetLight();

		l_light->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_value, l_light]()
		{
			l_light->SetAmbient( p_value );
		} ) );
	}

	void LightTreeItemProperty::OnDiffuseColourChange( Colour const & p_value )
	{
		LightSPtr l_light = GetLight();

		l_light->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_value, l_light]()
		{
			l_light->SetDiffuse( p_value );
		} ) );
	}

	void LightTreeItemProperty::OnSpecularColourChange( Colour const & p_value )
	{
		LightSPtr l_light = GetLight();

		l_light->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_value, l_light]()
		{
			l_light->SetSpecular( p_value );
		} ) );
	}

	void LightTreeItemProperty::OnAttenuationChange( Point3f const & p_value )
	{
		LightSPtr l_light = GetLight();

		l_light->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_value, l_light]()
		{
			if ( l_light->GetLightType() == eLIGHT_TYPE_POINT )
			{
				l_light->GetPointLight()->SetAttenuation( p_value );
			}
			else
			{
				l_light->GetSpotLight()->SetAttenuation( p_value );
			}
		} ) );
	}

	void LightTreeItemProperty::OnCutOffChange( double p_value )
	{
		LightSPtr l_light = GetLight();

		l_light->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_value, l_light]()
		{
			l_light->GetSpotLight()->SetCutOff( p_value );
		} ) );
	}

	void LightTreeItemProperty::OnExponentChange( double p_value )
	{
		LightSPtr l_light = GetLight();

		l_light->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_value, l_light]()
		{
			l_light->GetSpotLight()->SetExponent( p_value );
		} ) );
	}
}
