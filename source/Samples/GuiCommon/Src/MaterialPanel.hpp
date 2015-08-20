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
#ifndef ___GUICOMMON_MATERIAL_PANEL_H___
#define ___GUICOMMON_MATERIAL_PANEL_H___

#include "GuiCommonPrerequisites.hpp"

#include <wx/panel.h>

namespace GuiCommon
{
	class wxMaterialPanel
		: public wxPanel
	{
	public:
		wxMaterialPanel( bool p_small, bool p_bCanEdit, wxWindow * p_pParent, wxPoint const & p_ptPos = wxDefaultPosition, wxSize const & p_size = wxSize( 370, 470 ) );
		~wxMaterialPanel();

		void SelectPass( int p_iIndex );
		void SetMaterialName( Castor3D::Engine * p_pEngine, Castor::String const & p_strMaterialName );
		int GetPassIndex()const;

	private:
		void DoSetMaterialName( wxString const & p_strMaterialName );
		void DoCreatePassPanel();
		void DoShowMaterialFields( bool p_bShow );
		void DoShowPassFields( bool p_bShow );

		DECLARE_EVENT_TABLE()
		void OnDeletePass( wxCommandEvent & p_event );
		void OnMaterialName( wxCommandEvent & p_event );
		void OnPassSelect( wxCommandEvent & p_event );

	private:
		wxStaticText * m_pStaticName;
		wxTextCtrl * m_pEditMaterialName;
		wxStaticBox * m_pBoxPass;
		wxComboBox * m_pComboPass;
		wxButton * m_pButtonDeletePass;
		wxPanel * m_pPanelPasses;
		wxPassPanel * m_pPanelSelectedPass;
		Castor3D::MaterialWPtr m_wpMaterial;
		Castor3D::PassWPtr m_wpPassSelected;
		int m_iSelectedPassIndex;
		bool m_bCanEdit;
		Castor3D::Engine * m_pEngine;
	};
}

#endif
