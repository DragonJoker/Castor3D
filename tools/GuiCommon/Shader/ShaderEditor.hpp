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
		ShaderEditor( c3d::Engine * engine
			, ImagesLoader & imagesLoader
			, bool canEdit
			, StcContext & stcContext
			, ShaderEntryPoint const & shader
			, c3d::Vector< UniformBufferValues > & ubos
			, ShaderLanguage language
			, wxWindow * parent
			, wxPoint const & position = wxDefaultPosition
			, const wxSize size = wxSize( 800, 600 ) );
		~ShaderEditor()override;

		void loadLanguage( ShaderLanguage language );

	private:
		void doInitialiseLayout( c3d::Engine * engine
			, ImagesLoader & imagesLoader );
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
		c3d::Vector< UniformBufferValues > & m_ubos;
		c3d::Map< ShaderLanguage, wxString > m_sources;
		bool m_canEdit;
	};
}

#endif
