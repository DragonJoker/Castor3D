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
		static const wxString PROPERTY_CATEGORY_PASS = _( "Pass: " );
		static const wxString PROPERTY_PASS_AMBIENT = _( "Ambient" );
		static const wxString PROPERTY_PASS_DIFFUSE = _( "Diffuse" );
		static const wxString PROPERTY_PASS_SPECULAR = _( "Specular" );
		static const wxString PROPERTY_PASS_EMISSIVE = _( "Emissive" );
		static const wxString PROPERTY_PASS_EXPONENT = _( "Exponent" );
		static const wxString PROPERTY_PASS_TWO_SIDED = _( "Two sided" );
		static const wxString PROPERTY_PASS_OPACITY = _( "Opacity" );
		static const wxString PROPERTY_PASS_SHADER = _( "Shader" );
		static const wxString PROPERTY_PASS_EDIT_SHADER = _( "Edit Shader..." );
	}

	PassTreeItemProperty::PassTreeItemProperty( bool p_editable, Castor3D::PassSPtr p_pass )
		: TreeItemProperty( p_editable, ePROPERTY_DATA_TYPE_PASS )
		, m_pass( p_pass )
	{
	}

	PassTreeItemProperty::~PassTreeItemProperty()
	{
	}

	void PassTreeItemProperty::CreateProperties( wxPropertyGrid * p_grid )
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

			wxPGEditor * l_editor = p_grid->RegisterEditorClass( new ButtonEventEditor() );
			wxPGProperty * l_prop = p_grid->Append( new wxStringProperty( PROPERTY_PASS_SHADER ) );
			l_prop->SetValue( PROPERTY_PASS_EDIT_SHADER );
			l_prop->SetEditor( l_editor );
			l_prop->SetClientObject( new ButtonData( static_cast< ButtonEventMethod >( &PassTreeItemProperty::OnEditShader ), this ) );
		}
	}

	void PassTreeItemProperty::OnPropertyChange( wxPropertyGridEvent & p_event )
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

		l_pass->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_value, l_pass]()
		{
			l_pass->SetAmbient( p_value );
		} ) );
	}

	void PassTreeItemProperty::OnDiffuseColourChange( Colour const & p_value )
	{
		PassSPtr l_pass = GetPass();

		l_pass->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_value, l_pass]()
		{
			l_pass->SetDiffuse( p_value );
		} ) );
	}

	void PassTreeItemProperty::OnSpecularColourChange( Colour const & p_value )
	{
		PassSPtr l_pass = GetPass();

		l_pass->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_value, l_pass]()
		{
			l_pass->SetSpecular( p_value );
		} ) );
	}

	void PassTreeItemProperty::OnEmissiveColourChange( Colour const & p_value )
	{
		PassSPtr l_pass = GetPass();

		l_pass->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_value, l_pass]()
		{
			l_pass->SetSpecular( p_value );
		} ) );
	}

	void PassTreeItemProperty::OnExponentChange( double p_value )
	{
		PassSPtr l_pass = GetPass();

		l_pass->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_value, l_pass]()
		{
			l_pass->SetShininess( p_value );
		} ) );
	}

	void PassTreeItemProperty::OnTwoSidedChange( bool p_value )
	{
		PassSPtr l_pass = GetPass();

		l_pass->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_value, l_pass]()
		{
			l_pass->SetTwoSided( p_value );
		} ) );
	}

	void PassTreeItemProperty::OnOpacityChange( double p_value )
	{
		PassSPtr l_pass = GetPass();

		l_pass->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_value, l_pass]()
		{
			l_pass->SetAlpha( p_value );
		} ) );
	}

	bool PassTreeItemProperty::OnEditShader( wxPGProperty * p_property )
	{
		PassSPtr l_pass = GetPass();
		ShaderDialog * l_editor = new ShaderDialog( l_pass->GetEngine(), IsEditable(), NULL, l_pass );
		l_editor->Show();
		return false;
	}
}
