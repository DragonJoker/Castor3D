/*
See LICENSE file in root folder
*/
#ifndef ___C3DGC_ShaderProgramPage_H___
#define ___C3DGC_ShaderProgramPage_H___

#include "GuiCommon/Shader/ShaderSource.hpp"

#include <wx/frame.h>
#include <wx/aui/auibook.h>

namespace GuiCommon
{
	class ShaderProgramPage
		: public wxPanel
	{
	public:
		ShaderProgramPage( castor3d::Engine * engine
			, bool canEdit
			, StcContext & stcContext
			, ShaderSource & source
			, ShaderLanguage language
			, wxWindow * parent
			, wxPoint const & position = wxDefaultPosition
			, wxSize const & size = wxSize( 800, 600 ) );
		~ShaderProgramPage()override;

		void loadLanguage( ShaderLanguage language );

	private:
		void doInitialiseLayout( castor3d::Engine * engine );
		void doLoadPages( ShaderLanguage language );
		void doCleanup();

		DECLARE_EVENT_TABLE()
		void onClose( wxCloseEvent & event );

	protected:
		castor3d::Engine * m_engine;
		ShaderSource & m_source;
		StcContext & m_stcContext;
		wxAuiManager m_auiManager;
		bool m_canEdit;
		wxAuiNotebook * m_editors{};
		std::vector< ShaderEditor * > m_pages;
	};
}

#endif
