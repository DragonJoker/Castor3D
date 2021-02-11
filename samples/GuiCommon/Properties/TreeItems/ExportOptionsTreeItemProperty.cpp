#include "GuiCommon/Properties/TreeItems/ExportOptionsTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <wx/propgrid/advprops.h>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	ExportOptionsTreeItemProperty::ExportOptionsTreeItemProperty( bool editable
		, ExportOptions & options )
		: TreeItemProperty{ nullptr, editable }
		, m_options{ options }
	{
		CreateTreeItemMenu();
	}

	ExportOptionsTreeItemProperty::~ExportOptionsTreeItemProperty()
	{
	}

	void ExportOptionsTreeItemProperty::doCreateProperties( wxPGEditor * editor
		, wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_EXPORT_OPTIONS = _( "Export Options" );
		static wxString PROPERTY_OPTION_SCALE = _( "Scale" );
		static wxString PROPERTY_OPTION_SUBFOLDER = _( "Data in subfolder" );

		addProperty( grid, PROPERTY_CATEGORY_EXPORT_OPTIONS );
		addPropertyT( grid, PROPERTY_OPTION_SCALE, &m_options.scale );
		addPropertyT( grid, PROPERTY_OPTION_SUBFOLDER, &m_options.dataSubfolders );
	}
}
