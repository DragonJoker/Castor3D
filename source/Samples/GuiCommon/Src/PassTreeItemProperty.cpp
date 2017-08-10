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

using namespace castor3d;
using namespace castor;
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
		: TreeItemProperty( p_pass->getOwner()->getEngine(), p_editable, ePROPERTY_DATA_TYPE_PASS )
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

	void PassTreeItemProperty::doCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )
	{
		PassSPtr pass = getPass();

		if ( pass )
		{
			p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_PASS + wxString( pass->getOwner()->getName() ) ) );

			switch ( pass->getType() )
			{
			case MaterialType::eLegacy:
				{
					auto legacy = std::static_pointer_cast< LegacyPass >( pass );
					p_grid->Append( new wxColourProperty( PROPERTY_PASS_DIFFUSE ) )->SetValue( WXVARIANT( wxColour( toBGRPacked( legacy->getDiffuse() ) ) ) );
					p_grid->Append( new wxColourProperty( PROPERTY_PASS_SPECULAR ) )->SetValue( WXVARIANT( wxColour( toBGRPacked( legacy->getSpecular() ) ) ) );
					p_grid->Append( new wxFloatProperty( PROPERTY_PASS_AMBIENT ) )->SetValue( legacy->getAmbient() );
					p_grid->Append( new wxFloatProperty( PROPERTY_PASS_EXPONENT ) )->SetValue( legacy->getShininess() );
				}
				break;

			case MaterialType::ePbrMetallicRoughness:
				{
					auto pbr = std::static_pointer_cast< MetallicRoughnessPbrPass >( pass );
					p_grid->Append( new wxColourProperty( PROPERTY_PASS_ALBEDO ) )->SetValue( WXVARIANT( wxColour( toBGRPacked( pbr->getAlbedo() ) ) ) );
					p_grid->Append( new wxFloatProperty( PROPERTY_PASS_ROUGHNESS ) )->SetValue( pbr->getRoughness() );
					p_grid->Append( new wxFloatProperty( PROPERTY_PASS_METALLIC ) )->SetValue( pbr->getMetallic() );
				}
				break;

			case MaterialType::ePbrSpecularGlossiness:
				{
					auto pbr = std::static_pointer_cast< SpecularGlossinessPbrPass >( pass );
					p_grid->Append( new wxColourProperty( PROPERTY_PASS_DIFFUSE ) )->SetValue( WXVARIANT( wxColour( toBGRPacked( pbr->getDiffuse() ) ) ) );
					p_grid->Append( new wxColourProperty( PROPERTY_PASS_SPECULAR ) )->SetValue( WXVARIANT( wxColour( toBGRPacked( pbr->getSpecular() ) ) ) );
					p_grid->Append( new wxFloatProperty( PROPERTY_PASS_GLOSSINESS ) )->SetValue( pbr->getGlossiness() );
				}
				break;
			}

			p_grid->Append( new wxBoolProperty( PROPERTY_PASS_TWO_SIDED, wxPG_BOOL_USE_CHECKBOX ) )->SetValue( pass->IsTwoSided() );
			p_grid->Append( new wxFloatProperty( PROPERTY_PASS_EMISSIVE ) )->SetValue( pass->getEmissive() );
			p_grid->Append( new wxFloatProperty( PROPERTY_PASS_OPACITY ) )->SetValue( pass->getOpacity() );

			if ( checkFlag( pass->getTextureFlags(), TextureChannel::eRefraction ) )
			{
				p_grid->Append( new wxFloatProperty( PROPERTY_PASS_REFRACTION ) )->SetValue( pass->getRefractionRatio() );
			}

			p_grid->Append( CreateProperty( PROPERTY_PASS_SHADER, PROPERTY_PASS_EDIT_SHADER, static_cast< ButtonEventMethod >( &PassTreeItemProperty::OnEditShader ), this, p_editor ) );
		}
	}

	void PassTreeItemProperty::doPropertyChange( wxPropertyGridEvent & p_event )
	{
		PassSPtr pass = getPass();
		wxPGProperty * property = p_event.GetProperty();

		if ( property && pass )
		{
			wxColour colour;

			if ( property->GetName() == PROPERTY_PASS_DIFFUSE )
			{
				colour << property->GetValue();
				OnDiffuseColourChange( Colour::fromBGR( colour.GetRGB() ) );
			}
			else if ( property->GetName() == PROPERTY_PASS_SPECULAR )
			{
				colour << property->GetValue();
				OnSpecularColourChange( Colour::fromBGR( colour.GetRGB() ) );
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
				OnAlbedoChange( Colour::fromBGR( colour.GetRGB() ) );
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

		switch ( pass->getType() )
		{
		case MaterialType::eLegacy:
			{
				auto legacy = getTypedPass< MaterialType::eLegacy >();
				doApplyChange( [p_value, legacy]()
				{
					legacy->setDiffuse( p_value );
				} );
			}
			break;

		case MaterialType::ePbrSpecularGlossiness:
			{
				auto pbr = getTypedPass< MaterialType::ePbrSpecularGlossiness >();
				doApplyChange( [p_value, pbr]()
				{
					pbr->setDiffuse( p_value );
				} );
			}
			break;
		}
	}

	void PassTreeItemProperty::OnSpecularColourChange( Colour const & p_value )
	{
		auto pass = m_pass.lock();

		switch ( pass->getType() )
		{
		case MaterialType::eLegacy:
			{
				auto legacy = getTypedPass< MaterialType::eLegacy >();
				doApplyChange( [p_value, legacy]()
				{
					legacy->setSpecular( p_value );
				} );
			}
			break;

		case MaterialType::ePbrSpecularGlossiness:
			{
				auto pbr = getTypedPass< MaterialType::ePbrSpecularGlossiness >();
				doApplyChange( [p_value, pbr]()
				{
					pbr->setSpecular( p_value );
				} );
			}
			break;
		}
	}

	void PassTreeItemProperty::OnAmbientChange (double p_value)
	{
		auto pass = getTypedPass< MaterialType::eLegacy >();

		doApplyChange( [p_value, pass]()
		{
			pass->setAmbient( float( p_value ) );
		} );
	}

	void PassTreeItemProperty::OnEmissiveChange( double p_value )
	{
		auto pass = getTypedPass< MaterialType::eLegacy >();

		doApplyChange( [p_value, pass]()
		{
			pass->setEmissive( float( p_value ) );
		} );
	}

	void PassTreeItemProperty::OnExponentChange( double p_value )
	{
		auto pass = getTypedPass< MaterialType::eLegacy >();

		doApplyChange( [p_value, pass]()
		{
			pass->setShininess( float( p_value ) );
		} );
	}

	void PassTreeItemProperty::OnTwoSidedChange( bool p_value )
	{
		PassSPtr pass = getPass();

		doApplyChange( [p_value, pass]()
		{
			pass->setTwoSided( p_value );
		} );
	}

	void PassTreeItemProperty::OnOpacityChange( double p_value )
	{
		PassSPtr pass = getPass();

		doApplyChange( [p_value, pass]()
		{
			pass->setOpacity( float( p_value ) );
		} );
	}

	void PassTreeItemProperty::OnRefractionRatioChange( double p_value )
	{
		PassSPtr pass = getPass();

		doApplyChange( [p_value, pass]()
		{
			pass->setRefractionRatio( float( p_value ) );
		} );
	}

	void PassTreeItemProperty::OnAlbedoChange( castor::Colour const & p_value )
	{
		auto pass = getTypedPass< MaterialType::ePbrMetallicRoughness >();

		doApplyChange( [p_value, pass]()
		{
			pass->setAlbedo( p_value );
		} );
	}

	void PassTreeItemProperty::OnRoughnessChange( double p_value )
	{
		auto pass = getTypedPass< MaterialType::ePbrMetallicRoughness >();

		doApplyChange( [p_value, pass]()
		{
			pass->setRoughness( float( p_value ) );
		} );
	}

	void PassTreeItemProperty::OnMetallicChange( double p_value )
	{
		auto pass = getTypedPass< MaterialType::ePbrMetallicRoughness >();

		doApplyChange( [p_value, pass]()
		{
			pass->setMetallic( float( p_value ) );
		} );
	}

	void PassTreeItemProperty::OnGlossinessChange( double p_value )
	{
		auto pass = getTypedPass< MaterialType::ePbrSpecularGlossiness >();

		doApplyChange( [p_value, pass]()
		{
			pass->setGlossiness( float( p_value ) );
		} );
	}

	bool PassTreeItemProperty::OnEditShader( wxPGProperty * p_property )
	{
		PassSPtr pass = getPass();
		ShaderDialog * editor = new ShaderDialog( m_scene
			, IsEditable()
			, nullptr
			, *pass );
		editor->Show();
		return false;
	}
}
