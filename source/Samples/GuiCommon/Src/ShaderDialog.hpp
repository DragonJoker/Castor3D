/*
See LICENSE file in root folder
*/
#ifndef ___GC_ShaderDialog_HPP___
#define ___GC_ShaderDialog_HPP___

#include "ShaderSource.hpp"

#include <wx/frame.h>
#include <wx/aui/auibook.h>

namespace GuiCommon
{
	class ShaderDialog
		: public wxFrame
	{
	public:
		ShaderDialog( castor3d::Engine * engine
			, ShaderSources && sources
			, wxString const & title
			, wxWindow * parent
			, wxPoint const & position = wxDefaultPosition
			, const wxSize size = wxSize( 800, 600 ) );
		~ShaderDialog();

	private:
		void doInitialiseShaderLanguage();
		void doInitialiseLayout();
		void doLoadPages();
		void doPopulateMenu();
		void doCleanup();

		DECLARE_EVENT_TABLE()
		void onClose( wxCloseEvent & event );
		void onOk( wxCommandEvent & event );
		void onCancel( wxCommandEvent & event );
		void onMenuClose( wxCommandEvent & event );

	private:
		castor3d::Engine * m_engine;
		wxAuiManager m_auiManager;
		wxAuiNotebook * m_editors;
		std::unique_ptr< StcContext > m_stcContext;
		std::vector< ShaderEditorPage * > m_pages;
		ShaderSources m_sources;
	};
}

#endif
