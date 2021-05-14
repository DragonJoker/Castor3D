/*
See LICENSE file in root folder
*/
#ifndef ___GC_PropertiesDialog_H___
#define ___GC_PropertiesDialog_H___

#include "PropertiesContainer.hpp"
#include "TreeItems/TreeItemProperty.hpp"

#include <wx/dialog.h>

namespace GuiCommon
{
	class PropertiesDialog
		: public wxDialog
	{
	public:
		PropertiesDialog( wxWindow * parent
			, wxString const & title
			, TreeItemPropertyUPtr properties
			, wxPoint const & pos = wxDefaultPosition
			, wxSize const & size = wxDefaultSize );

	private:
		TreeItemPropertyUPtr m_properties;
		PropertiesContainer * m_holder{ nullptr };
	};
}

#endif
