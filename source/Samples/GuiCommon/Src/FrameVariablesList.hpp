/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_FRAME_VARIABLES_LIST_VIEW_H___
#define ___GUICOMMON_FRAME_VARIABLES_LIST_VIEW_H___

#include "GuiCommonPrerequisites.hpp"

#include <wx/treectrl.h>

namespace GuiCommon
{
	//class FrameVariablesList
	//	: public wxTreeCtrl
	//{
	//public:
	//	FrameVariablesList( PropertiesContainer * p_propertiesHolder
	//		, wxWindow * p_parent
	//		, wxPoint const & p_ptPos = wxDefaultPosition
	//		, wxSize const & p_size = wxDefaultSize );
	//	~FrameVariablesList();

	//	void LoadVariables( castor3d::ShaderType p_type
	//		, castor3d::ShaderProgramSPtr p_program
	//		, castor3d::RenderPipeline & p_pipeline );
	//	void UnloadVariables();

	//private:
	//	void doAddBuffer( wxTreeItemId p_id
	//		, castor3d::UniformBuffer & p_buffer );
	//	void doAddVariable( wxTreeItemId p_id
	//		, castor3d::UniformSPtr p_variable
	//		, castor3d::UniformBuffer & p_buffer );
	//	void doAddVariable( wxTreeItemId p_id
	//		, castor3d::PushUniformSPtr p_variable
	//		, castor3d::ShaderType p_type );

	//	DECLARE_EVENT_TABLE()
	//	void OnClose( wxCloseEvent & p_event );
	//	void OnSelectItem( wxTreeEvent & p_event );
	//	void OnMouseRButtonUp( wxTreeEvent & p_event );

	//private:
	//	castor3d::ShaderProgramWPtr m_program;
	//	PropertiesContainer * m_propertiesHolder;
	//};
}

#endif
