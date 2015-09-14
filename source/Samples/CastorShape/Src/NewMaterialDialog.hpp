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
		NewMaterialDialog( Castor3D::Engine * p_pEngine, wxWindow * parent, wxWindowID p_id,
						   wxString const & p_name = cuT( "New Material" ),
						   wxPoint const & pos = wxDefaultPosition,
						   wxSize const & size = wxSize( 380, 500 ),
						   long style = wxDEFAULT_DIALOG_STYLE );
		~NewMaterialDialog();

		Castor3D::MaterialSPtr GetMaterial()const
		{
			return m_material.lock();
		}

	private:
		DECLARE_EVENT_TABLE()
		void OnClose( wxCloseEvent & event );
		void OnOk( wxCommandEvent & event );
		void OnCancel( wxCommandEvent & event );

	private:
		wxButton * m_okButton;
		wxButton * m_cancelButton;
		wxPropertyGrid * m_properties;
		Castor3D::MaterialWPtr m_material;
		Castor3D::Engine * m_pEngine;
	};
}

#endif
