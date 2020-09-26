#include "GuiCommon/Properties/TreeItems/FrameVariableBufferTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <wx/propgrid/advprops.h>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	FrameVariableBufferTreeItemProperty::FrameVariableBufferTreeItemProperty( Engine * engine
		, bool editable
		, UniformBufferValues & buffer )
		: TreeItemProperty{ engine, editable, ePROPERTY_DATA_TYPE_CAMERA }
		, m_buffer{ buffer }
	{
		CreateTreeItemMenu();
	}

	FrameVariableBufferTreeItemProperty::~FrameVariableBufferTreeItemProperty()
	{
	}

	void FrameVariableBufferTreeItemProperty::doCreateProperties( wxPGEditor * p_editor
		, wxPropertyGrid * p_grid )
	{
	}
}
