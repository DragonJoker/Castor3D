#include "PassTreeItemProperty.hpp"

#include "ShaderDialog.hpp"

#include <Engine.hpp>
#include <Event/Frame/FunctorEvent.hpp>
#include <Material/Material.hpp>
#include <Material/LegacyPass.hpp>
#include <Material/MetallicRoughnessPbrPass.hpp>
#include <Material/SpecularGlossinessPbrPass.hpp>

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
		static wxString PROPERTY_PASS_AMBIENT = _( "Ambient" );
		static wxString PROPERTY_PASS_EMISSIVE = _( "Emissive" );
		static wxString PROPERTY_PASS_EXPONENT = _( "Exponent" );
		static wxString PROPERTY_PASS_TWO_SIDED = _( "Two sided" );
		static wxString PROPERTY_PASS_OPACITY = _( "Opacity" );
		static wxString PROPERTY_PASS_ALBEDO = _( "Albedo" );
		static wxString PROPERTY_PASS_ROUGHNESS = _( "Roughness" );
		static wxString PROPERTY_PASS_METALLIC = _( "Metallic" );
		static wxString PROPERTY_PASS_SHADER = _( "Shader" );
		static wxString PROPERTY_PASS_REFRACTION = _( "Refraction" );
		static wxString PROPERTY_PASS_GLOSSINESS = _( "Glossiness" );
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
		PROPERTY_PASS_AMBIENT = _( "Ambient" );
		PROPERTY_PASS_EMISSIVE = _( "Emissive" );
		PROPERTY_PASS_EXPONENT = _( "Exponent" );
		PROPERTY_PASS_TWO_SIDED = _( "Two sided" );
		PROPERTY_PASS_OPACITY = _( "Opacity" );
		PROPERTY_PASS_ALBEDO = _( "Albedo" );
		PROPERTY_PASS_ROUGHNESS = _( "Roughness" );
		PROPERTY_PASS_METALLIC = _( "Metallic" );
		PROPERTY_PASS_SHADER = _( "Shader" );
		PROPERTY_PASS_GLOSSINESS = _( "Glossiness" );
		PROPERTY_PASS_EDIT_SHADER = _( "Edit Shader..." );

		CreateTreeItemMenu();
	}

	PassTreeItemProperty::~PassTreeItemProperty()
	{
	}

	void PassTreeItemProperty::DoCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )
	{
		PassSPtr pass = GetPass();

		if ( pass )
		{
			p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_PASS + wxString( pass->GetOwner()->GetName() ) ) );

			switch ( pass->GetType() )
			{
			case MaterialType::eLegacy:
				{
					auto legacy = std::static_pointer_cast< LegacyPass >( pass );
					p_grid->Append( new wxColourProperty( PROPERTY_PASS_DIFFUSE ) )->SetValue( WXVARIANT( wxColour( bgr_packed( legacy->GetDiffuse() ) ) ) );
					p_grid->Append( new wxColourProperty( PROPERTY_PASS_SPECULAR ) )->SetValue( WXVARIANT( wxColour( bgr_packed( legacy->GetSpecular() ) ) ) );
					p_grid->Append( new wxFloatProperty( PROPERTY_PASS_AMBIENT ) )->SetValue( legacy->GetAmbient() );
					p_grid->Append( new wxFloatProperty( PROPERTY_PASS_EXPONENT ) )->SetValue( legacy->GetShininess() );
				}
				break;

			case MaterialType::ePbrMetallicRoughness:
				{
					auto pbr = std::static_pointer_cast< MetallicRoughnessPbrPass >( pass );
					p_grid->Append( new wxColourProperty( PROPERTY_PASS_ALBEDO ) )->SetValue( WXVARIANT( wxColour( bgr_packed( pbr->GetAlbedo() ) ) ) );
					p_grid->Append( new wxFloatProperty( PROPERTY_PASS_ROUGHNESS ) )->SetValue( pbr->GetRoughness() );
					p_grid->Append( new wxFloatProperty( PROPERTY_PASS_METALLIC ) )->SetValue( pbr->GetMetallic() );
				}
				break;

			case MaterialType::ePbrSpecularGlossiness:
				{
					auto pbr = std::static_pointer_cast< SpecularGlossinessPbrPass >( pass );
					p_grid->Append( new wxColourProperty( PROPERTY_PASS_DIFFUSE ) )->SetValue( WXVARIANT( wxColour( bgr_packed( pbr->GetDiffuse() ) ) ) );
					p_grid->Append( new wxColourProperty( PROPERTY_PASS_SPECULAR ) )->SetValue( WXVARIANT( wxColour( bgr_packed( pbr->GetSpecular() ) ) ) );
					p_grid->Append( new wxFloatProperty( PROPERTY_PASS_GLOSSINESS ) )->SetValue( pbr->GetGlossiness() );
				}
				break;
			}

			p_grid->Append( new wxBoolProperty( PROPERTY_PASS_TWO_SIDED, wxPG_BOOL_USE_CHECKBOX ) )->SetValue( pass->IsTwoSided() );
			p_grid->Append( new wxFloatProperty( PROPERTY_PASS_EMISSIVE ) )->SetValue( pass->GetEmissive() );
			p_grid->Append( new wxFloatProperty( PROPERTY_PASS_OPACITY ) )->SetValue( pass->GetOpacity() );

			if ( CheckFlag( pass->GetTextureFlags(), TextureChannel::eRefraction ) )
			{
				p_grid->Append( new wxFloatProperty( PROPERTY_PASS_REFRACTION ) )->SetValue( pass->GetRefractionRatio() );
			}

			p_grid->Append( CreateProperty( PROPERTY_PASS_SHADER, PROPERTY_PASS_EDIT_SHADER, static_cast< ButtonEventMethod >( &PassTreeItemProperty::OnEditShader ), this, p_editor ) );
		}
	}

	void PassTreeItemProperty::DoPropertyChange( wxPropertyGridEvent & p_event )
	{
		PassSPtr pass = GetPass();
		wxPGProperty * property = p_event.GetProperty();

		if ( property && pass )
		{
			wxColour colour;

			if ( property->GetName() == PROPERTY_PASS_DIFFUSE )
			{
				colour << property->GetValue();
				OnDiffuseColourChange( Colour::from_bgr( colour.GetRGB() ) );
			}
			else if ( property->GetName() == PROPERTY_PASS_SPECULAR )
			{
				colour << property->GetValue();
				OnSpecularColourChange( Colour::from_bgr( colour.GetRGB() ) );
			}
			else if ( property->GetName() == PROPERTY_PASS_AMBIENT )
			{
				OnAmbientChange( property->GetValue() );
			}
			else if ( property->GetName() == PROPERTY_PASS_EMISSIVE )
			{
				OnEmissiveChange( property->GetValue() );
			}
			else if ( property->GetName() == PROPERTY_PASS_EXPONENT )
			{
				OnExponentChange( property->GetValue() );
			}
			else if ( property->GetName() == PROPERTY_PASS_TWO_SIDED )
			{
				OnTwoSidedChange( property->GetValue() );
			}
			else if ( property->GetName() == PROPERTY_PASS_OPACITY )
			{
				OnOpacityChange( property->GetValue() );
			}
			else if ( property->GetName() == PROPERTY_PASS_REFRACTION )
			{
				OnRefractionRatioChange( property->GetValue() );
			}
			else if ( property->GetName() == PROPERTY_PASS_ALBEDO )
			{
				OnAlbedoChange( Colour::from_bgr( colour.GetRGB() ) );
			}
			else if ( property->GetName() == PROPERTY_PASS_ROUGHNESS )
			{
				OnRoughnessChange( property->GetValue() );
			}
			else if ( property->GetName() == PROPERTY_PASS_GLOSSINESS )
			{
				OnGlossinessChange( property->GetValue() );
			}
			else if ( property->GetName() == PROPERTY_PASS_METALLIC )
			{
				OnMetallicChange( property->GetValue() );
			}
		}
	}

	void PassTreeItemProperty::OnDiffuseColourChange( Colour const & p_value )
	{
		auto pass = m_pass.lock();

		switch ( pass->GetType() )
		{
		case MaterialType::eLegacy:
			{
				auto legacy = GetTypedPass< MaterialType::eLegacy >();
				DoApplyChange( [p_value, legacy]()
				{
					legacy->SetDiffuse( p_value );
				} );
			}
			break;

		case MaterialType::ePbrSpecularGlossiness:
			{
				auto pbr = GetTypedPass< MaterialType::ePbrSpecularGlossiness >();
				DoApplyChange( [p_value, pbr]()
				{
					pbr->SetDiffuse( p_value );
				} );
			}
			break;
		}
	}

	void PassTreeItemProperty::OnSpecularColourChange( Colour const & p_value )
	{
		auto pass = m_pass.lock();

		switch ( pass->GetType() )
		{
		case MaterialType::eLegacy:
			{
				auto legacy = GetTypedPass< MaterialType::eLegacy >();
				DoApplyChange( [p_value, legacy]()
				{
					legacy->SetSpecular( p_value );
				} );
			}
			break;

		case MaterialType::ePbrSpecularGlossiness:
			{
				auto pbr = GetTypedPass< MaterialType::ePbrSpecularGlossiness >();
				DoApplyChange( [p_value, pbr]()
				{
					pbr->SetSpecular( p_value );
				} );
			}
			break;
		}
	}

	void PassTreeItemProperty::OnAmbientChange (double p_value)
	{
		auto pass = GetTypedPass< MaterialType::eLegacy >();

		DoApplyChange( [p_value, pass]()
		{
			pass->SetAmbient( float( p_value ) );
		} );
	}

	void PassTreeItemProperty::OnEmissiveChange( double p_value )
	{
		auto pass = GetTypedPass< MaterialType::eLegacy >();

		DoApplyChange( [p_value, pass]()
		{
			pass->SetEmissive( float( p_value ) );
		} );
	}

	void PassTreeItemProperty::OnExponentChange( double p_value )
	{
		auto pass = GetTypedPass< MaterialType::eLegacy >();

		DoApplyChange( [p_value, pass]()
		{
			pass->SetShininess( float( p_value ) );
		} );
	}

	void PassTreeItemProperty::OnTwoSidedChange( bool p_value )
	{
		PassSPtr pass = GetPass();

		DoApplyChange( [p_value, pass]()
		{
			pass->SetTwoSided( p_value );
		} );
	}

	void PassTreeItemProperty::OnOpacityChange( double p_value )
	{
		PassSPtr pass = GetPass();

		DoApplyChange( [p_value, pass]()
		{
			pass->SetOpacity( float( p_value ) );
		} );
	}

	void PassTreeItemProperty::OnRefractionRatioChange( double p_value )
	{
		PassSPtr pass = GetPass();

		DoApplyChange( [p_value, pass]()
		{
			pass->SetRefractionRatio( float( p_value ) );
		} );
	}

	void PassTreeItemProperty::OnAlbedoChange( Castor::Colour const & p_value )
	{
		auto pass = GetTypedPass< MaterialType::ePbrMetallicRoughness >();

		DoApplyChange( [p_value, pass]()
		{
			pass->SetAlbedo( p_value );
		} );
	}

	void PassTreeItemProperty::OnRoughnessChange( double p_value )
	{
		auto pass = GetTypedPass< MaterialType::ePbrMetallicRoughness >();

		DoApplyChange( [p_value, pass]()
		{
			pass->SetRoughness( float( p_value ) );
		} );
	}

	void PassTreeItemProperty::OnMetallicChange( double p_value )
	{
		auto pass = GetTypedPass< MaterialType::ePbrMetallicRoughness >();

		DoApplyChange( [p_value, pass]()
		{
			pass->SetMetallic( float( p_value ) );
		} );
	}

	void PassTreeItemProperty::OnGlossinessChange( double p_value )
	{
		auto pass = GetTypedPass< MaterialType::ePbrSpecularGlossiness >();

		DoApplyChange( [p_value, pass]()
		{
			pass->SetGlossiness( float( p_value ) );
		} );
	}

	bool PassTreeItemProperty::OnEditShader( wxPGProperty * p_property )
	{
		PassSPtr pass = GetPass();
		ShaderDialog * editor = new ShaderDialog( m_scene
			, IsEditable()
			, nullptr
			, *pass );
		editor->Show();
		return false;
	}
}
