/*
See LICENSE file in root folder
*/
#ifndef ___GC_ShaderDialog_HPP___
#define ___GC_ShaderDialog_HPP___

#include "GuiCommon/Shader/ShaderSource.hpp"

#include <wx/frame.h>
#include <wx/aui/auibook.h>

namespace GuiCommon
{
	class ShaderDialog
		: public wxFrame
	{
	public:
		ShaderDialog( castor3d::Engine * engine
			, ShaderSources sources
			, wxString const & title
			, wxWindow * parent
			, wxPoint const & position = wxDefaultPosition
			, const wxSize size = wxSize( 800, 600 ) );
		~ShaderDialog()override;

	private:
		void doLoadLanguage( ShaderLanguage language );
		void doInitialiseShaderLanguage();
		void doInitialiseLayout();
		void doLoadPages();
		void doPopulateMenu();
		void doCleanup();

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-override"
		DECLARE_EVENT_TABLE()
#pragma clang diagnostic pop
		void onClose( wxCloseEvent & event );
		void onMenuClose( wxCommandEvent & event );
#if GC_HasGLSL
		void onMenuLanguageGLSL( wxCommandEvent & event );
#endif
#if GC_HasHLSL
		void onMenuLanguageHLSL( wxCommandEvent & event );
#endif
		void onMenuLanguageSPIRV( wxCommandEvent & event );
		void onMenuPreferences( wxCommandEvent & event );

	private:
		castor3d::Engine * m_engine;
		wxAuiManager m_auiManager;
		wxAuiNotebook * m_programs;
		castor::RawUniquePtr< StcContext > m_stcContext;
		castor::Vector< ShaderProgramPage * > m_pages;
		ShaderSources m_sources;
		wxMenuItem * m_glslRadio;
		wxMenuItem * m_hlslRadio;
		wxMenuItem * m_spirvRadio;
	};
}

#endif
