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
		ShaderDialog( Castor3D::Scene & p_scene
			, bool p_bCanEdit
			, wxWindow * p_parent
			, Castor3D::Pass & p_pass
			, wxPoint const & p_position = wxDefaultPosition
			, const wxSize p_size = wxSize( 800, 600 ) );
		~ShaderDialog();

	private:
		void DoInitialiseShaderLanguage();
		void DoInitialiseLayout();
		void DoLoadPages();
		void DoPopulateMenu();
		void DoCleanup();
		void DoLoadShader();
		void DoOpenShaderFile();
		void DoFolder( Castor3D::ShaderType p_type );
		void DoSave( Castor3D::ShaderType p_type, bool p_createIfNone );

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
		ShaderEditorPage * m_pEditorPages[size_t( Castor3D::ShaderType::eCount )];
		Castor3D::ShaderProgramWPtr m_shaderProgram;
		Castor3D::Pass & m_pass;
		bool m_bCompiled;
		bool m_bOwnShader;
		bool m_bCanEdit;
		Castor3D::Scene & m_scene;
	};
}

#endif
