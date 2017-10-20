/* See LICENSE file in root folder */
#ifndef ___CS_NEW_MATERIAL_DIALOG_H___
#define ___CS_NEW_MATERIAL_DIALOG_H___

#include "Castor3DPrerequisites.hpp"

#include <wx/propgrid/manager.h>

namespace CastorShape
{
	class NewMaterialDialog
		: public wxDialog
	{
	public:
		NewMaterialDialog( wxPGEditor * p_editor, Castor3D::Scene & p_scene, wxWindow * parent, wxWindowID p_id, wxString const & p_name = cuT( "New Material" ), wxPoint const & pos = wxDefaultPosition, wxSize const & size = wxSize( 380, 500 ), long style = wxDEFAULT_DIALOG_STYLE );
		~NewMaterialDialog();

		Castor3D::MaterialSPtr GetMaterial()const
		{
			return m_material;
		}

	private:
		void DoCreateProperties();
		void DoCreatePassProperties();
		void DoCreateTextureProperties();

		// Material properties
		void OnMaterialNameChange( Castor::String const & p_value );
		void OnPassChange( Castor::String const & p_value );
		// Pass properties
		void OnAmbientColourChange( Castor::Colour const & p_value );
		void OnDiffuseColourChange( Castor::Colour const & p_value );
		void OnSpecularColourChange( Castor::Colour const & p_value );
		void OnEmissiveColourChange( Castor::Colour const & p_value );
		void OnExponentChange( double p_value );
		void OnTwoSidedChange( bool p_value );
		void OnOpacityChange( double p_value );
		bool OnEditShader( wxPGProperty * p_property );
		void OnTextureChange( Castor::String const & p_value );
		// Texture properties
		void OnChannelChange( Castor3D::TextureChannel p_value );
		void OnImageChange( Castor::Path const & p_value );

		DECLARE_EVENT_TABLE()
		void OnClose( wxCloseEvent & event );
		void OnOk( wxCommandEvent & event );
		void OnCancel( wxCommandEvent & event );
		void OnPropertyChange( wxPropertyGridEvent & p_event );

	private:
		wxPGEditor * m_editor;
		wxPropertyGrid * m_properties;
		Castor3D::MaterialSPtr m_material;
		Castor3D::Scene & m_scene;
		Castor3D::PassSPtr m_pass;
		Castor3D::TextureUnitSPtr m_texture;
	};
}

#endif
