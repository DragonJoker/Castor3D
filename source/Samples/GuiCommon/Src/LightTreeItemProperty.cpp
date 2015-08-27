#include "LightTreeItemProperty.hpp"

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

	wxLightTreeItemProperty::wxLightTreeItemProperty( LightSPtr p_light )
		: wxTreeItemProperty( ePROPERTY_DATA_TYPE_LIGHT )
		, m_light( p_light )
	{
	}

	wxLightTreeItemProperty::~wxLightTreeItemProperty()
	{
	}

	void wxLightTreeItemProperty::CreateProperties( wxPropertyGrid * p_grid )
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
				DoCreateDirectionalLightProperties( p_grid, std::static_pointer_cast< DirectionalLight >( l_light->GetLightCategory() ) );
				break;

			case eLIGHT_TYPE_POINT:
				DoCreatePointLightProperties( p_grid, std::static_pointer_cast< PointLight >( l_light->GetLightCategory() ) );
				break;

			case eLIGHT_TYPE_SPOT:
				DoCreateSpotLightProperties( p_grid, std::static_pointer_cast< SpotLight >( l_light->GetLightCategory() ) );
				break;
			}
		}
	}

	void wxLightTreeItemProperty::OnPropertyChange( wxPropertyGridEvent & p_event )
	{
		LightSPtr l_light = GetLight();
		wxPGProperty * l_property = p_event.GetProperty();

		if ( l_property && l_light )
		{
		}
	}

	void wxLightTreeItemProperty::DoCreateDirectionalLightProperties( wxPropertyGrid * p_grid, DirectionalLightSPtr p_light )
	{
	}

	void wxLightTreeItemProperty::DoCreatePointLightProperties( wxPropertyGrid * p_grid, PointLightSPtr p_light )
	{
		p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_POINT_LIGHT ) );
		p_grid->Append( new Point3rProperty( PROPERTY_LIGHT_ATTENUATION ) )->SetValue( wxVariant( p_light->GetAttenuation() ) );
	}

	void wxLightTreeItemProperty::DoCreateSpotLightProperties( wxPropertyGrid * p_grid, SpotLightSPtr p_light )
	{
		p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_SPOT_LIGHT ) );
		p_grid->Append( new Point3rProperty( PROPERTY_LIGHT_ATTENUATION ) )->SetValue( wxVariant( p_light->GetAttenuation() ) );
		p_grid->Append( new wxFloatProperty( PROPERTY_LIGHT_CUT_OFF ) )->SetValue( p_light->GetCutOff() );
		p_grid->Append( new wxFloatProperty( PROPERTY_LIGHT_EXPONENT ) )->SetValue( p_light->GetExponent() );
	}

	void wxLightTreeItemProperty::OnDirectionalLightPropertyChanged( wxPropertyGridEvent & p_event )
	{
	}

	void wxLightTreeItemProperty::OnPointLightPropertyChanged( wxPropertyGridEvent & p_event )
	{
	}

	void wxLightTreeItemProperty::OnSpotLightPropertyChanged( wxPropertyGridEvent & p_event )
	{
	}
}
