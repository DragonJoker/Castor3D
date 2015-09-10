#include "FrameVariableBufferTreeItemProperty.hpp"

#include <FrameVariableBuffer.hpp>
#include <FunctorEvent.hpp>

#include "AdditionalProperties.hpp"
#include <wx/propgrid/advprops.h>

using namespace Castor3D;
using namespace Castor;

namespace GuiCommon
{
	namespace
	{
		static const wxString PROPERTY_CATEGORY_FRAME_VARIABLE_BUFFER = _( "Frame Variable Buffer: " );
		static const wxString PROPERTY_NAME = _( "Name" );
	}

	FrameVariableBufferTreeItemProperty::FrameVariableBufferTreeItemProperty( bool p_editable, FrameVariableBufferSPtr p_buffer )
		: TreeItemProperty( p_editable, ePROPERTY_DATA_TYPE_CAMERA )
		, m_buffer( p_buffer )
	{
	}

	FrameVariableBufferTreeItemProperty::~FrameVariableBufferTreeItemProperty()
	{
	}

	void FrameVariableBufferTreeItemProperty::CreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )
	{
		FrameVariableBufferSPtr l_buffer = GetBuffer();

		if ( l_buffer )
		{
			p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_FRAME_VARIABLE_BUFFER + l_buffer->GetName() ) );
			p_grid->Append( new wxStringProperty( PROPERTY_NAME ) )->SetValue( l_buffer->GetName() );
		}
	}

	void FrameVariableBufferTreeItemProperty::OnPropertyChange( wxPropertyGridEvent & p_event )
	{
		wxPGProperty * l_property = p_event.GetProperty();
		FrameVariableBufferSPtr l_buffer = GetBuffer();

		if ( l_property && l_buffer )
		{
		}
	}
}
