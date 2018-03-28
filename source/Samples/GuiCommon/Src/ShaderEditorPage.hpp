/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_SHADER_EDITOR_PAGE_H___
#define ___GUICOMMON_SHADER_EDITOR_PAGE_H___

#include "GuiCommonPrerequisites.hpp"

#include <wx/frame.h>
#include <wx/aui/auibook.h>

namespace GuiCommon
{
	//class ShaderEditorPage
	//	: public wxPanel
	//{
	//public:
	//	ShaderEditorPage( bool p_bCanEdit
	//		, StcContext & p_stcContext
	//		, castor3d::ShaderProgramSPtr p_shader
	//		, castor3d::ShaderType p_type
	//		, castor3d::Pass const & p_pass
	//		, castor3d::Scene const & p_scene
	//		, wxWindow * p_parent
	//		, wxPoint const & p_position = wxDefaultPosition
	//		, const wxSize p_size = wxSize( 800, 600 ) );
	//	~ShaderEditorPage();

	//	bool LoadFile( wxString const & p_file );
	//	void SaveFile( bool p_createIfNone );

	//	wxString const & getShaderFile()const
	//	{
	//		return m_shaderFile;
	//	}

	//	wxString const & getShaderSource()const
	//	{
	//		return m_shaderSource;
	//	}

	//private:
	//	void doInitialiseShaderLanguage();
	//	void doInitialiseLayout();
	//	void doLoadPage( castor3d::RenderPipeline & p_pipeline );
	//	void doCleanup();

	//	DECLARE_EVENT_TABLE()
	//	void OnClose( wxCloseEvent & p_event );

	//protected:
	//	wxAuiManager m_auiManager;
	//	StcContext & m_stcContext;
	//	StcTextEditor * m_editor;
	//	FrameVariablesList * m_frameVariablesList;
	//	PropertiesContainer * m_frameVariablesProperties;
	//	wxString m_shaderFile;
	//	wxString m_shaderSource;
	//	castor3d::ShaderProgramWPtr m_shaderProgram;
	//	castor3d::ShaderType m_shaderType;
	//	bool m_canEdit;
	//};
}

#endif
