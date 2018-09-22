/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_SHADER_EDITOR_PAGE_H___
#define ___GUICOMMON_SHADER_EDITOR_PAGE_H___

#include "ShaderSource.hpp"

#include <wx/frame.h>
#include <wx/aui/auibook.h>

namespace GuiCommon
{
	class ShaderEditorPage
		: public wxPanel
	{
	public:
		ShaderEditorPage( castor3d::Engine * engine
			, bool canEdit
			, StcContext & stcContext
			, ashes::ShaderStageFlag type
			, castor::String const & source
			, std::vector< UniformBufferValues > & ubos
			, wxWindow * parent
			, wxPoint const & position = wxDefaultPosition
			, const wxSize size = wxSize( 800, 600 ) );
		~ShaderEditorPage();

	private:
		void doInitialiseShaderLanguage();
		void doInitialiseLayout( castor3d::Engine * engine );
		void doLoadPage();
		void doCleanup();

		DECLARE_EVENT_TABLE()
		void onClose( wxCloseEvent & event );

	protected:
		wxAuiManager m_auiManager;
		StcContext & m_stcContext;
		StcTextEditor * m_editor;
		FrameVariablesList * m_frameVariablesList;
		PropertiesContainer * m_frameVariablesProperties;
		ashes::ShaderStageFlag m_stage;
		wxString m_source;
		std::vector< UniformBufferValues > & m_ubos;
		bool m_canEdit;
	};
}

#endif
