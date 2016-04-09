/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
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
		NewMaterialDialog( wxPGEditor * p_editor, Castor3D::Engine * p_engine, wxWindow * parent, wxWindowID p_id, wxString const & p_name = cuT( "New Material" ), wxPoint const & pos = wxDefaultPosition, wxSize const & size = wxSize( 380, 500 ), long style = wxDEFAULT_DIALOG_STYLE );
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
		void OnChannelChange( Castor3D::eTEXTURE_CHANNEL p_value );
		void OnImageChange( Castor::String const & p_value );

		DECLARE_EVENT_TABLE()
		void OnClose( wxCloseEvent & event );
		void OnOk( wxCommandEvent & event );
		void OnCancel( wxCommandEvent & event );
		void OnPropertyChange( wxPropertyGridEvent & p_event );

	private:
		wxPGEditor * m_editor;
		wxPropertyGrid * m_properties;
		Castor3D::MaterialSPtr m_material;
		Castor3D::Engine * m_engine;
		Castor3D::PassSPtr m_pass;
		Castor3D::TextureUnitSPtr m_texture;
	};
}

#endif
