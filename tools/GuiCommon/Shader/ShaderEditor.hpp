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
			, ShaderEntryPoint const & shader
			, std::vector< UniformBufferValues > & ubos
			, ShaderLanguage language
			, wxWindow * parent
			, wxPoint const & position = wxDefaultPosition
			, const wxSize size = wxSize( 800, 600 ) );
		~ShaderEditor()override;

		void loadLanguage( ShaderLanguage language );

	private:
		void doInitialiseLayout( castor3d::Engine * engine );
		void doCleanup();
		void doListAvailableLanguages();

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-override"
		DECLARE_EVENT_TABLE()
#pragma clang diagnostic pop
		void onClose( wxCloseEvent & event );

	private:
		wxAuiManager m_auiManager;
		StcContext & m_stcContext;
		wxWindowPtr< StcTextEditor > m_editor;
		wxWindowPtr< FrameVariablesList > m_frameVariablesList;
		wxWindowPtr< PropertiesContainer > m_frameVariablesProperties;
		ShaderEntryPoint const & m_shader;
		std::vector< UniformBufferValues > & m_ubos;
		std::map< ShaderLanguage, wxString > m_sources;
		bool m_canEdit;
	};
}

#endif
