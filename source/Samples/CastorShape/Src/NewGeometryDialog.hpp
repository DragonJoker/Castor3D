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
#ifndef ___CS_NEW_GEOMETRY_DIALOG_H___
#define ___CS_NEW_GEOMETRY_DIALOG_H___

#include "Castor3DPrerequisites.hpp"

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
		NewGeometryDialog( Castor3D::Scene & p_scene, wxWindow * p_parent, wxWindowID p_id, wxString const & p_name = cuT( "New Geometry" ), wxPoint const & p_pos = wxDefaultPosition, wxSize const & p_size = wxSize( 400, 200 ), long p_style = wxDEFAULT_DIALOG_STYLE );
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
		Castor3D::Scene & m_scene;
	};
}

#endif
