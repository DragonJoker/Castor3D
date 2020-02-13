/*
See LICENSE file in root folder
*/
#ifndef ___GuiCommon_TreeHolder_H___
#define ___GuiCommon_TreeHolder_H___

#include "GuiCommon/GuiCommonPrerequisites.hpp"

#include <wx/panel.h>
#include <wx/treectrl.h>
#include <wx/aui/auibar.h>
#include <wx/aui/framemanager.h>

namespace GuiCommon
{
	class TreeHolder
		: public wxPanel
	{
	public:
		TreeHolder( wxWindow * parent
			, wxPoint const & position = wxDefaultPosition
			, wxSize const & size = wxDefaultSize );
		~TreeHolder();

		void setTree( wxTreeCtrl * tree );

	protected:
		DECLARE_EVENT_TABLE()
		void onCollapseAll( wxCommandEvent & event );
		void onExpandAll( wxCommandEvent & event );

	private:
		wxAuiManager m_auiManager;
		wxTreeCtrl * m_tree{ nullptr };
		wxAuiToolBar * m_toolBar{ nullptr };
	};
}

#endif
