#include "GuiCommon/Properties/TreeItems/FrameVariableBufferTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <wx/propgrid/advprops.h>

namespace GuiCommon
{
	FrameVariableBufferTreeItemProperty::FrameVariableBufferTreeItemProperty( castor3d::Engine * engine
		, bool editable
		, UniformBufferValues & /*buffer*/ )
		: TreeItemProperty{ engine, editable }
	{
		CreateTreeItemMenu();
	}

	void FrameVariableBufferTreeItemProperty::doCreateProperties( wxPGEditor * editor
		, wxPropertyGrid * grid )
	{
	}
}
