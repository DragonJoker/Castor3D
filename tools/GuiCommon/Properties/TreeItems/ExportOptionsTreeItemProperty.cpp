#include "GuiCommon/Properties/TreeItems/ExportOptionsTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <wx/propgrid/advprops.h>

namespace GuiCommon
{
	ExportOptionsTreeItemProperty::ExportOptionsTreeItemProperty( bool editable
		, c3d::exporter::ExportOptions & options )
		: TreeItemProperty{ nullptr, editable }
		, m_options{ options }
	{
		CreateTreeItemMenu();
	}

	void ExportOptionsTreeItemProperty::doCreateProperties( wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_EXPORT_OPTIONS = _( "Export Options" );
		static wxString PROPERTY_OPTION_SCALE = _( "Scale" );
		static wxString PROPERTY_OPTION_SUBFOLDER = _( "Data in subfolder" );
		static wxString PROPERTY_OPTION_SPLIT_SUBMESHES = _( "Split submeshes into meshes" );
		static wxString PROPERTY_OPTION_RECENTER_SUBMESHES = _( "Recenter created meshes" );
		static wxString PROPERTY_OPTION_IGNORE_FAILURES = _( "Ignore failures" );
		static wxString PROPERTY_OPTION_FORCE_TEXT = _( "Force export as text files" );

		addProperty( grid, PROPERTY_CATEGORY_EXPORT_OPTIONS );
		addPropertyT( grid, PROPERTY_OPTION_SCALE, &m_options.scale );
		addPropertyT( grid, PROPERTY_OPTION_SUBFOLDER, &m_options.dataSubfolders );
		addPropertyT( grid, PROPERTY_OPTION_SPLIT_SUBMESHES, &m_options.splitPerMaterial );
		addPropertyT( grid, PROPERTY_OPTION_RECENTER_SUBMESHES, &m_options.recenter );
		addPropertyT( grid, PROPERTY_OPTION_IGNORE_FAILURES, &m_options.ignoreFailures );
		addPropertyT( grid, PROPERTY_OPTION_FORCE_TEXT, &m_options.forceText );
	}
}
