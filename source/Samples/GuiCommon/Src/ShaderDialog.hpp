/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#include <wx/aui/auibook.h>

namespace GuiCommon
{
	class ShaderDialog
		: public wxFrame
	{
	public:
		ShaderDialog( Castor3D::Scene & p_scene, bool p_bCanEdit, wxWindow * p_parent, Castor3D::PassSPtr p_pass, wxPoint const & p_position = wxDefaultPosition, const wxSize p_size = wxSize( 800, 600 ) );
		~ShaderDialog();

	private:
		void DoInitialiseShaderLanguage();
		void DoInitialiseLayout();
		void DoLoadPages();
		void DoPopulateMenu();
		void DoCleanup();
		void DoLoadShader();
		void DoOpenShaderFile();
		void DoFolder( Castor3D::eSHADER_TYPE p_type );
		void DoSave( Castor3D::eSHADER_TYPE p_type, bool p_createIfNone );

		DECLARE_EVENT_TABLE()
		void OnOpenFile( wxCommandEvent & p_event );
		void OnLoadShader( wxCommandEvent & p_event );
		void OnClose( wxCloseEvent & p_event );
		void OnOk( wxCommandEvent & p_event );
		void OnCancel( wxCommandEvent & p_event );
		void OnSaveFile( wxCommandEvent & p_event );
		void OnSaveAll( wxCommandEvent & p_event );
		void OnMenuClose( wxCommandEvent & p_event );

	protected:
		wxAuiManager m_auiManager;
		wxAuiNotebook * m_pNotebookEditors;
		std::unique_ptr< StcContext > m_pStcContext;
		ShaderEditorPage * m_pEditorPages[Castor3D::eSHADER_TYPE_COUNT];
		Castor3D::ShaderProgramWPtr m_shaderProgram;
		Castor3D::PassWPtr m_pPass;
		bool m_bCompiled;
		bool m_bOwnShader;
		bool m_bCanEdit;
		Castor3D::Scene & m_scene;
	};
}

#endif
