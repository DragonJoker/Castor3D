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
		ShaderProgramPage( c3d::Engine * engine
			, ImagesLoader & imagesLoader
			, bool canEdit
			, StcContext & stcContext
			, ShaderSource & source
			, ShaderLanguage language
			, wxWindow * parent
			, wxPoint const & position = wxDefaultPosition
			, wxSize const & size = wxSize( 800, 600 ) );
		~ShaderProgramPage()override;

		void loadLanguage( ShaderLanguage language )const;

	private:
		void doInitialiseLayout();
		void doLoadPages( ShaderLanguage language );
		void doCleanup();

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-override"
		DECLARE_EVENT_TABLE()
#pragma clang diagnostic pop
		void onClose( wxCloseEvent & event );

	protected:
		c3d::Engine * m_engine;
		ImagesLoader & m_imagesLoader;
		ShaderSource & m_source;
		StcContext & m_stcContext;
		wxAuiManager m_auiManager;
		bool m_canEdit;
		wxAuiNotebook * m_editors{};
		c3d::Vector< ShaderEditor * > m_pages;
		ShaderLanguage m_language;
	};
}

#endif
