/* See LICENSE file in root folder */
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
