/*
See LICENSE file in root folder
*/
#ifndef ___GuiCommon_PropertiesHolder_H___
#define ___GuiCommon_PropertiesHolder_H___

#include "GuiCommonPrerequisites.hpp"

#include <wx/panel.h>
#include <wx/propgrid/propgrid.h>
#include <wx/aui/auibar.h>

namespace GuiCommon
{
	class PropertiesHolder
		: public wxPanel
	{
	public:
		PropertiesHolder( wxWindow * parent
			, wxPoint const & position = wxDefaultPosition
			, wxSize const & size = wxDefaultSize );
		~PropertiesHolder();

		void setGrid( wxPropertyGrid * grid );

	private:
		wxAuiManager m_auiManager;
		wxPropertyGrid * m_grid{ nullptr };
	};
}

#endif
