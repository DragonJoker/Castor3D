/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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
