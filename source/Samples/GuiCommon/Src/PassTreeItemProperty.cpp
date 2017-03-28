#include "PassTreeItemProperty.hpp"

#include "ShaderDialog.hpp"

#include <Engine.hpp>
#include <Event/Frame/FunctorEvent.hpp>
#include <Material/Material.hpp>
#include <Material/LegacyPass.hpp>

#include "AdditionalProperties.hpp"
#include "PointProperties.hpp"
#include <wx/propgrid/advprops.h>

using namespace Castor3D;
using namespace Castor;
using namespace GuiCommon;

namespace GuiCommon
{
	namespace
	{
		static wxString PROPERTY_CATEGORY_PASS = _( "Pass: " );
		static wxString PROPERTY_PASS_DIFFUSE = _( "Diffuse" );
		static wxString PROPERTY_PASS_SPECULAR = _( "Specular" );
		static wxString PROPERTY_PASS_EMISSIVE = _( "Emissive" );
		static wxString PROPERTY_PASS_EXPONENT = _( "Exponent" );
		static wxString PROPERTY_PASS_TWO_SIDED = _( "Two sided" );
		static wxString PROPERTY_PASS_OPACITY = _( "Opacity" );
		static wxString PROPERTY_PASS_SHADER = _( "Shader" );
		static wxString PROPERTY_PASS_EDIT_SHADER = _( "Edit Shader..." );
	}

	PassTreeItemProperty::PassTreeItemProperty( bool p_editable, PassSPtr p_pass, Scene & p_scene )
		: TreeItemProperty( p_pass->GetOwner()->GetEngine(), p_editable, ePROPERTY_DATA_TYPE_PASS )
		, m_pass( p_pass )
		, m_scene( p_scene )
	{
		PROPERTY_CATEGORY_PASS = _( "Pass: " );
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
			p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_PASS + wxString( l_pass->GetOwner()->GetName() ) ) );

			if ( l_pass->GetType() == MaterialType::eLegacy )
			{
				auto l_legacy = std::static_pointer_cast< LegacyPass >( l_pass );
				p_grid->Append( new wxColourProperty( PROPERTY_PASS_DIFFUSE ) )->SetValue( WXVARIANT( wxColour( bgr_packed( l_legacy->GetDiffuse() ) ) ) );
				p_grid->Append( new wxColourProperty( PROPERTY_PASS_SPECULAR ) )->SetValue( WXVARIANT( wxColour( bgr_packed( l_legacy->GetSpecular() ) ) ) );
				p_grid->Append( new Point4fProperty( GC_HDR_COLOUR, PROPERTY_PASS_EMISSIVE ) )->SetValue( WXVARIANT( rgba_float( l_legacy->GetEmissive() ) ) );
				p_grid->Append( new wxFloatProperty( PROPERTY_PASS_EXPONENT ) )->SetValue( l_legacy->GetShininess() );
			}

			p_grid->Append( new wxBoolProperty( PROPERTY_PASS_TWO_SIDED, wxPG_BOOL_USE_CHECKBOX ) )->SetValue( l_pass->IsTwoSided() );
			p_grid->Append( new wxFloatProperty( PROPERTY_PASS_OPACITY ) )->SetValue( l_pass->GetOpacity() );
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
				OnDiffuseColourChange( Colour::from_bgr( l_colour.GetRGB() ) );
			}
			else if ( l_property->GetName() == PROPERTY_PASS_SPECULAR )
			{
				l_colour << l_property->GetValue();
				OnSpecularColourChange( Colour::from_bgr( l_colour.GetRGB() ) );
			}
			else if ( l_property->GetName() == PROPERTY_PASS_EMISSIVE )
			{
				Point4f l_value = PointRefFromVariant< float, 4 >( l_property->GetValue() );
				OnEmissiveColourChange( HdrColour::from_rgba( l_value ) );
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

	void PassTreeItemProperty::OnDiffuseColourChange( Colour const & p_value )
	{
		auto l_pass = GetTypedPass< MaterialType::eLegacy >();

		DoApplyChange( [p_value, l_pass]()
		{
			l_pass->SetDiffuse( p_value );
		} );
	}

	void PassTreeItemProperty::OnSpecularColourChange( Colour const & p_value )
	{
		auto l_pass = GetTypedPass< MaterialType::eLegacy >();

		DoApplyChange( [p_value, l_pass]()
		{
			l_pass->SetSpecular( p_value );
		} );
	}

	void PassTreeItemProperty::OnEmissiveColourChange( HdrColour const & p_value )
	{
		auto l_pass = GetTypedPass< MaterialType::eLegacy >();

		DoApplyChange( [p_value, l_pass]()
		{
			l_pass->SetEmissive( p_value );
		} );
	}

	void PassTreeItemProperty::OnExponentChange( double p_value )
	{
		auto l_pass = GetTypedPass< MaterialType::eLegacy >();

		DoApplyChange( [p_value, l_pass]()
		{
			l_pass->SetShininess( float( p_value ) );
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
			l_pass->SetOpacity( float( p_value ) );
		} );
	}

	bool PassTreeItemProperty::OnEditShader( wxPGProperty * p_property )
	{
		PassSPtr l_pass = GetPass();
		ShaderDialog * l_editor = new ShaderDialog( m_scene, IsEditable(), nullptr, l_pass );
		l_editor->Show();
		return false;
	}
}
