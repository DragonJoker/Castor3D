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
#ifndef ___GUICOMMON_SHADER_DIALOG_H___
#define ___GUICOMMON_SHADER_DIALOG_H___

#include "GuiCommonPrerequisites.hpp"

#include <wx/frame.h>
#include <wx/notebook.h>

namespace GuiCommon
{
	class wxShaderDialog
		:	public wxFrame
	{
	protected:
		wxNotebook * m_pNotebookEditors;
		StcContext * m_pStcContext;
		wxStcTextEditor * m_pStcEditors[Castor3D::eSHADER_TYPE_COUNT];
		wxListBox * m_pListFrameVariables[Castor3D::eSHADER_TYPE_COUNT];
		wxString m_strShaderFiles[Castor3D::eSHADER_TYPE_COUNT];
		wxString m_strShaderSources[Castor3D::eSHADER_TYPE_COUNT];
		Castor3D::ShaderProgramBaseWPtr m_pShaderProgram;
		Castor3D::ShaderObjectBaseWPtr m_pShaderObject;
		Castor3D::PassWPtr m_pPass;
		bool m_bCompiled;
		bool m_bOwnShader;
		FrameVariableMap m_mapFrameVariables[Castor3D::eSHADER_TYPE_COUNT];
		bool m_bCanEdit;
		Castor3D::Engine * m_pEngine;

	public:
		wxShaderDialog( Castor3D::Engine * p_pEngine, bool p_bCanEdit, wxWindow * p_pParent, Castor3D::PassSPtr p_pPass, wxPoint const & p_ptPosition = wxDefaultPosition, const wxSize p_ptSize = wxSize( 800, 600 ) );
		~wxShaderDialog();

	private:
		void DoPopulateMenu();
		void DoCleanup();
		void DoLoadShader();
		void DoOpenShaderFile();
		void DoFolder( Castor3D::eSHADER_TYPE p_eType );
		void DoSave( Castor3D::eSHADER_TYPE p_eType, bool p_bTell );
		void DoGridCellChange( Castor3D::eSHADER_TYPE p_eType, int p_iRow );

		DECLARE_EVENT_TABLE()
		void OnOpenFile( wxCommandEvent & p_event );
		void OnLoadShader( wxCommandEvent & p_event );
		void OnClose( wxCloseEvent & p_event );
		void OnOk( wxCommandEvent & p_event );
		void OnCancel( wxCommandEvent & p_event );
		void OnGridVertexCellChange( wxCommandEvent & p_event );
		void OnGridFragmentCellChange( wxCommandEvent & p_event );
		void OnGridGeometryCellChange( wxCommandEvent & p_event );
		void OnGridHullCellChange( wxCommandEvent & p_event );
		void OnGridDomainCellChange( wxCommandEvent & p_event );
		void OnSaveFile( wxCommandEvent & p_event );
		void OnSaveAll( wxCommandEvent & p_event );
		void OnMenuClose( wxCommandEvent & p_event );
	};
}

#endif
