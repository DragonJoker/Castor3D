/*
See LICENSE file in root folder
*/
#ifndef ___ARIA_SceneFileEditor_H___
#define ___ARIA_SceneFileEditor_H___

#include "EditorModule.hpp"

#include <wx/aui/framemanager.h>
#include <wx/panel.h>

namespace aria
{
	class SceneFileEditor
		: public wxPanel
	{
	public:
		SceneFileEditor( StcContext & stcContext
			, wxString const & filename
			, wxWindow * parent
			, wxPoint const & position = wxDefaultPosition
			, const wxSize size = wxSize( 800, 600 ) );
		~SceneFileEditor();

		bool saveFile();

	private:
		void doInitialiseLayout( wxString const & filename );
		void doCleanup();

	protected:
		wxAuiManager m_auiManager;
		StcContext & m_stcContext;
		StcTextEditor * m_editor{};
		wxString m_source;
	};
}

#endif
