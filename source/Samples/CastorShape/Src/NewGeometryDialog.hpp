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
#ifndef ___CS_NEW_GEOMETRY_DIALOG_H___
#define ___CS_NEW_GEOMETRY_DIALOG_H___

#include "CastorUtilsPrerequisites.hpp"

#include <wx/dialog.h>
#include <wx/propgrid/propgrid.h>

namespace CastorShape
{
	class NewGeometryDialog
		: public wxDialog
	{
	public:
		enum IDs
		{
			eID_MATERIAL_NAME,
		};

	public:
		NewGeometryDialog( Castor3D::Engine * p_engine, wxWindow * p_parent, wxWindowID p_id, wxString const & p_name = cuT( "New Geometry" ), wxPoint const & p_pos = wxDefaultPosition, wxSize const & p_size = wxSize( 400, 200 ), long p_style = wxDEFAULT_DIALOG_STYLE );
		~NewGeometryDialog();

		Castor::String GetGeometryName()const;
		Castor::String GetMaterialName()const;

	protected:
		void DoInitialise();
		virtual void DoCreateProperties() = 0;

	private:
		DECLARE_EVENT_TABLE()
		void OnOk( wxCommandEvent & event );

	protected:
		wxPropertyGrid * m_properties;
		Castor3D::Engine * m_engine;
	};
}

#endif
