#include "FrameVariableBufferTreeItemProperty.hpp"

#include <Shader/UniformBuffer.hpp>

#include "AdditionalProperties.hpp"
#include <wx/propgrid/advprops.h>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	namespace
	{
		static wxString PROPERTY_CATEGORY_FRAME_VARIABLE_BUFFER = _( "Frame Variable Buffer: " );
		static wxString PROPERTY_NAME = _( "Name" );
	}

	FrameVariableBufferTreeItemProperty::FrameVariableBufferTreeItemProperty( Engine * engine
		, bool p_editable
		, UniformBuffer & p_buffer )
		: TreeItemProperty( engine, p_editable, ePROPERTY_DATA_TYPE_CAMERA )
		, m_buffer( p_buffer )
	{
		PROPERTY_CATEGORY_FRAME_VARIABLE_BUFFER = _( "Frame Variable Buffer: " );
		PROPERTY_NAME = _( "Name" );

		CreateTreeItemMenu();
	}

	FrameVariableBufferTreeItemProperty::~FrameVariableBufferTreeItemProperty()
	{
	}

	void FrameVariableBufferTreeItemProperty::doCreateProperties( wxPGEditor * p_editor
		, wxPropertyGrid * p_grid )
	{
		p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_FRAME_VARIABLE_BUFFER + m_buffer.getName() ) );
		p_grid->Append( new wxStringProperty( PROPERTY_NAME ) )->SetValue( m_buffer.getName() );
	}

	void FrameVariableBufferTreeItemProperty::doPropertyChange( wxPropertyGridEvent & p_event )
	{
		wxPGProperty * property = p_event.GetProperty();

		if ( property )
		{
		}
	}
}
