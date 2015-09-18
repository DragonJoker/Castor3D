#include "PassTreeItemProperty.hpp"

#include "ShaderDialog.hpp"

#include <Engine.hpp>
#include <FunctorEvent.hpp>
#include <Pass.hpp>

#include "AdditionalProperties.hpp"
#include <wx/propgrid/advprops.h>

using namespace Castor3D;
using namespace Castor;

namespace GuiCommon
{
	namespace
	{
		static wxString PROPERTY_CATEGORY_PASS = _( "Pass: " );
		static wxString PROPERTY_PASS_AMBIENT = _( "Ambient" );
		static wxString PROPERTY_PASS_DIFFUSE = _( "Diffuse" );
		static wxString PROPERTY_PASS_SPECULAR = _( "Specular" );
		static wxString PROPERTY_PASS_EMISSIVE = _( "Emissive" );
		static wxString PROPERTY_PASS_EXPONENT = _( "Exponent" );
		static wxString PROPERTY_PASS_TWO_SIDED = _( "Two sided" );
		static wxString PROPERTY_PASS_OPACITY = _( "Opacity" );
		static wxString PROPERTY_PASS_SHADER = _( "Shader" );
		static wxString PROPERTY_PASS_EDIT_SHADER = _( "Edit Shader..." );
	}

	PassTreeItemProperty::PassTreeItemProperty( bool p_editable, Castor3D::PassSPtr p_pass )
		: TreeItemProperty( p_pass->GetEngine(), p_editable, ePROPERTY_DATA_TYPE_PASS )
		, m_pass( p_pass )
	{
		PROPERTY_CATEGORY_PASS = _( "Pass: " );
		PROPERTY_PASS_AMBIENT = _( "Ambient" );
		PROPERTY_PASS_DIFFUSE = _( "Diffuse" );
		PROPERTY_PASS_SPECULAR = _( "Specular" );
		PROPERTY_PASS_EMISSIVE = _( "Emissive" );
		PROPERTY_PASS_EXPONENT = _( "Exponent" );
		PROPERTY_PASS_TWO_SIDED = _( "Two sided" );
		PROPERTY_PASS_OPACITY = _( "Opacity" );
		PROPERTY_PASS_SHADER = _( "Shader" );
		PROPERTY_PASS_EDIT_SHADER = _( "Edit Shader..." );

		CreateTreeItemMenu();
	}

	PassTreeItemProperty::~PassTreeItemProperty()
	{
	}

	void PassTreeItemProperty::DoCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )
	{
		PassSPtr l_pass = GetPass();

		if ( l_pass )
		{
			p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_PASS + wxString( l_pass->GetParent()->GetName() ) ) );
			p_grid->Append( new wxColourProperty( PROPERTY_PASS_DIFFUSE ) )->SetValue( wxVariant( wxColour( l_pass->GetAmbient().to_bgr() ) ) );
			p_grid->Append( new wxColourProperty( PROPERTY_PASS_AMBIENT ) )->SetValue( wxVariant( wxColour( l_pass->GetDiffuse().to_bgr() ) ) );
			p_grid->Append( new wxColourProperty( PROPERTY_PASS_SPECULAR ) )->SetValue( wxVariant( wxColour( l_pass->GetSpecular().to_bgr() ) ) );
			p_grid->Append( new wxColourProperty( PROPERTY_PASS_EMISSIVE ) )->SetValue( wxVariant( wxColour( l_pass->GetEmissive().to_bgr() ) ) );
			p_grid->Append( new wxFloatProperty( PROPERTY_PASS_EXPONENT ) )->SetValue( l_pass->GetShininess() );
			p_grid->Append( new wxBoolProperty( PROPERTY_PASS_TWO_SIDED, wxPG_BOOL_USE_CHECKBOX ) )->SetValue( l_pass->IsTwoSided() );
			p_grid->Append( new wxFloatProperty( PROPERTY_PASS_OPACITY ) )->SetValue( l_pass->GetAlpha() );
			p_grid->Append( CreateProperty( PROPERTY_PASS_SHADER, PROPERTY_PASS_EDIT_SHADER, static_cast< ButtonEventMethod >( &PassTreeItemProperty::OnEditShader ), this, p_editor ) );
		}
	}

	void PassTreeItemProperty::DoPropertyChange( wxPropertyGridEvent & p_event )
	{
		PassSPtr l_pass = GetPass();
		wxPGProperty * l_property = p_event.GetProperty();

		if ( l_property && l_pass )
		{
			wxColour l_colour;

			if ( l_property->GetName() == PROPERTY_PASS_DIFFUSE )
			{
				l_colour << l_property->GetValue();
				OnAmbientColourChange( Colour::from_bgr( l_colour.GetRGB() ) );
			}
			else if ( l_property->GetName() == PROPERTY_PASS_AMBIENT )
			{
				l_colour << l_property->GetValue();
				OnDiffuseColourChange( Colour::from_bgr( l_colour.GetRGB() ) );
			}
			else if ( l_property->GetName() == PROPERTY_PASS_SPECULAR )
			{
				l_colour << l_property->GetValue();
				OnSpecularColourChange( Colour::from_bgr( l_colour.GetRGB() ) );
			}
			else if ( l_property->GetName() == PROPERTY_PASS_EMISSIVE )
			{
				l_colour << l_property->GetValue();
				OnEmissiveColourChange( Colour::from_bgr( l_colour.GetRGB() ) );
			}
			else if ( l_property->GetName() == PROPERTY_PASS_EXPONENT )
			{
				OnExponentChange( l_property->GetValue() );
			}
			else if ( l_property->GetName() == PROPERTY_PASS_TWO_SIDED )
			{
				OnTwoSidedChange( l_property->GetValue() );
			}
			else if ( l_property->GetName() == PROPERTY_PASS_OPACITY )
			{
				OnOpacityChange( l_property->GetValue() );
			}
		}
	}

	void PassTreeItemProperty::OnAmbientColourChange( Colour const & p_value )
	{
		PassSPtr l_pass = GetPass();

		DoApplyChange( [p_value, l_pass]()
		{
			l_pass->SetAmbient( p_value );
		} );
	}

	void PassTreeItemProperty::OnDiffuseColourChange( Colour const & p_value )
	{
		PassSPtr l_pass = GetPass();

		DoApplyChange( [p_value, l_pass]()
		{
			l_pass->SetDiffuse( p_value );
		} );
	}

	void PassTreeItemProperty::OnSpecularColourChange( Colour const & p_value )
	{
		PassSPtr l_pass = GetPass();

		DoApplyChange( [p_value, l_pass]()
		{
			l_pass->SetSpecular( p_value );
		} );
	}

	void PassTreeItemProperty::OnEmissiveColourChange( Colour const & p_value )
	{
		PassSPtr l_pass = GetPass();

		DoApplyChange( [p_value, l_pass]()
		{
			l_pass->SetSpecular( p_value );
		} );
	}

	void PassTreeItemProperty::OnExponentChange( double p_value )
	{
		PassSPtr l_pass = GetPass();

		DoApplyChange( [p_value, l_pass]()
		{
			l_pass->SetShininess( p_value );
		} );
	}

	void PassTreeItemProperty::OnTwoSidedChange( bool p_value )
	{
		PassSPtr l_pass = GetPass();

		DoApplyChange( [p_value, l_pass]()
		{
			l_pass->SetTwoSided( p_value );
		} );
	}

	void PassTreeItemProperty::OnOpacityChange( double p_value )
	{
		PassSPtr l_pass = GetPass();

		DoApplyChange( [p_value, l_pass]()
		{
			l_pass->SetAlpha( p_value );
		} );
	}

	bool PassTreeItemProperty::OnEditShader( wxPGProperty * p_property )
	{
		PassSPtr l_pass = GetPass();
		ShaderDialog * l_editor = new ShaderDialog( l_pass->GetEngine(), IsEditable(), NULL, l_pass );
		l_editor->Show();
		return false;
	}
}
