/*
See LICENSE file in root folder
*/
#ifndef ___C3DGC_ShaderEditor_H___
#define ___C3DGC_ShaderEditor_H___

#include "GuiCommon/Shader/ShaderSource.hpp"

#include <wx/frame.h>
#include <wx/aui/auibook.h>

namespace GuiCommon
{
	class ShaderEditor
		: public wxPanel
	{
	public:
		ShaderEditor( castor3d::Engine * engine
			, bool canEdit
			, StcContext & stcContext
			, castor3d::ShaderModule const & module
			, std::vector< UniformBufferValues > & ubos
			, ShaderLanguage language
			, wxWindow * parent
			, wxPoint const & position = wxDefaultPosition
			, const wxSize size = wxSize( 800, 600 ) );
		~ShaderEditor();

		void loadLanguage( ShaderLanguage language );

	private:
		void doInitialiseLayout( castor3d::Engine * engine );
		void doCleanup();

		DECLARE_EVENT_TABLE()
		void onClose( wxCloseEvent & event );

	protected:
		wxAuiManager m_auiManager;
		StcContext & m_stcContext;
		StcTextEditor * m_editor;
		FrameVariablesList * m_frameVariablesList;
		PropertiesContainer * m_frameVariablesProperties;
		castor3d::ShaderModule const & m_module;
		std::vector< UniformBufferValues > & m_ubos;
		bool m_canEdit;
	};
}

#endif
