/*
See LICENSE file in root folder
*/
#ifndef ___GuiCommon_PropertiesHolder_H___
#define ___GuiCommon_PropertiesHolder_H___

#include "GuiCommon/GuiCommonPrerequisites.hpp"

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <wx/panel.h>
#include <wx/propgrid/propgrid.h>
#include <wx/aui/framemanager.h>
#include <wx/aui/auibar.h>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

namespace GuiCommon
{
	class PropertiesHolder
		: public wxPanel
	{
	public:
		explicit PropertiesHolder( wxWindow * parent
			, wxPoint const & position = wxDefaultPosition
			, wxSize const & size = wxDefaultSize );
		~PropertiesHolder()override;

		void setGrid( wxPropertyGrid * grid );

	private:
		wxAuiManager m_auiManager;
		wxPropertyGrid * m_grid{ nullptr };
	};
}

#endif
