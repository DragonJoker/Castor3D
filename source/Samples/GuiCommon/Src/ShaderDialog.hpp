/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_SHADER_DIALOG_H___
#define ___GUICOMMON_SHADER_DIALOG_H___

#include "GuiCommonPrerequisites.hpp"

#include <wx/frame.h>
#include <wx/aui/auibook.h>

namespace GuiCommon
{
	//class ShaderDialog
	//	: public wxFrame
	//{
	//public:
	//	ShaderDialog( castor3d::Scene & p_scene
	//		, bool p_bCanEdit
	//		, wxWindow * p_parent
	//		, castor3d::Pass & p_pass
	//		, wxPoint const & p_position = wxDefaultPosition
	//		, const wxSize p_size = wxSize( 800, 600 ) );
	//	~ShaderDialog();

	//private:
	//	void doInitialiseShaderLanguage();
	//	void doInitialiseLayout();
	//	void doLoadPages();
	//	void doPopulateMenu();
	//	void doCleanup();
	//	void doLoadShader();
	//	void doOpenShaderFile();
	//	void doFolder( renderer::ShaderStageFlag p_type );
	//	void doSave( renderer::ShaderStageFlag p_type, bool p_createIfNone );

	//	DECLARE_EVENT_TABLE()
	//	void OnOpenFile( wxCommandEvent & p_event );
	//	void OnLoadShader( wxCommandEvent & p_event );
	//	void OnClose( wxCloseEvent & p_event );
	//	void OnOk( wxCommandEvent & p_event );
	//	void OnCancel( wxCommandEvent & p_event );
	//	void OnSaveFile( wxCommandEvent & p_event );
	//	void OnSaveAll( wxCommandEvent & p_event );
	//	void OnMenuClose( wxCommandEvent & p_event );

	//protected:
	//	wxAuiManager m_auiManager;
	//	wxAuiNotebook * m_pNotebookEditors;
	//	std::unique_ptr< StcContext > m_pStcContext;
	//	ShaderEditorPage * m_pEditorPages[6u];
	//	castor3d::ShaderProgramWPtr m_shaderProgram;
	//	castor3d::Pass & m_pass;
	//	bool m_bCompiled;
	//	bool m_bOwnShader;
	//	bool m_bCanEdit;
	//	castor3d::Scene & m_scene;
	//};
}

#endif
