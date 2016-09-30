#include "FrameVariablesList.hpp"

#include "ImagesLoader.hpp"
#include "FrameVariableBufferTreeItemProperty.hpp"
#include "FrameVariableTreeItemProperty.hpp"
#include "PropertiesHolder.hpp"

#include <Engine.hpp>
#include <Render/RenderSystem.hpp>
#include <Shader/FrameVariable.hpp>
#include <Shader/FrameVariableBuffer.hpp>
#include <Shader/ShaderProgram.hpp>

#include <wx/imaglist.h>

#include "xpms/frame_variable.xpm"
#include "xpms/frame_variable_sel.xpm"
#include "xpms/frame_variable_buffer.xpm"
#include "xpms/frame_variable_buffer_sel.xpm"

#ifdef LoadImage
#	undef LoadImage
#	define LoadImage wxBitmap::LoadImage
#endif

using namespace Castor3D;
using namespace Castor;

namespace GuiCommon
{
	typedef enum eID
	{
		eID_FRAME_VARIABLE,
		eID_FRAME_VARIABLE_SEL,
		eID_FRAME_VARIABLE_BUFFER,
		eID_FRAME_VARIABLE_BUFFER_SEL,
	}	eID;

	FrameVariablesList::FrameVariablesList( PropertiesHolder * p_propertiesHolder, wxWindow * p_parent, wxPoint const & p_ptPos, wxSize const & p_size )
		: wxTreeCtrl( p_parent, wxID_ANY, p_ptPos, p_size, wxTR_DEFAULT_STYLE | wxTR_HIDE_ROOT | wxNO_BORDER )
		, m_propertiesHolder( p_propertiesHolder )
	{
		wxBusyCursor l_wait;
		ImagesLoader::AddBitmap( eBMP_FRAME_VARIABLE, frame_variable_xpm );
		ImagesLoader::AddBitmap( eBMP_FRAME_VARIABLE_SEL, frame_variable_sel_xpm );
		ImagesLoader::AddBitmap( eBMP_FRAME_VARIABLE_BUFFER, frame_variable_buffer_xpm );
		ImagesLoader::AddBitmap( eBMP_FRAME_VARIABLE_BUFFER_SEL, frame_variable_buffer_sel_xpm );
		ImagesLoader::WaitAsyncLoads();

		wxImage * l_icons[] =
		{
			ImagesLoader::GetBitmap( eBMP_FRAME_VARIABLE ),
			ImagesLoader::GetBitmap( eBMP_FRAME_VARIABLE_SEL ),
			ImagesLoader::GetBitmap( eBMP_FRAME_VARIABLE_BUFFER ),
			ImagesLoader::GetBitmap( eBMP_FRAME_VARIABLE_BUFFER_SEL ),
		};

		wxImageList * l_imageList = new wxImageList( GC_IMG_SIZE, GC_IMG_SIZE, true );

		for ( auto l_image : l_icons )
		{
			int l_sizeOrig = l_image->GetWidth();

			if ( l_sizeOrig != GC_IMG_SIZE )
			{
				l_image->Rescale( GC_IMG_SIZE, GC_IMG_SIZE, wxIMAGE_QUALITY_HIGHEST );
			}

			l_imageList->Add( wxImage( *l_image ) );
		}

		AssignImageList( l_imageList );
	}

	FrameVariablesList::~FrameVariablesList()
	{
		UnloadVariables();
	}

	void FrameVariablesList::LoadVariables( ShaderType p_type, ShaderProgramSPtr p_program )
	{
		m_program = p_program;
		wxTreeItemId l_root = AddRoot( _( "Root" ) );

		if ( p_program->GetObjectStatus( p_type ) != ShaderStatus::DontExist )
		{
			for ( auto l_buffer : p_program->GetFrameVariableBuffers( p_type ) )
			{
				DoAddBuffer( l_root, l_buffer );
			}

			for ( auto l_variable : p_program->GetFrameVariables( p_type ) )
			{
				DoAddVariable( l_root, l_variable, p_type );
			}
		}
	}

	void FrameVariablesList::UnloadVariables()
	{
		DeleteAllItems();
	}

	void FrameVariablesList::DoAddBuffer( wxTreeItemId p_id, FrameVariableBufferSPtr p_buffer )
	{
		wxTreeItemId l_id = AppendItem( p_id, p_buffer->GetName(), eID_FRAME_VARIABLE_BUFFER, eID_FRAME_VARIABLE_BUFFER_SEL, new FrameVariableBufferTreeItemProperty( m_program.lock()->GetRenderSystem()->GetEngine(), m_propertiesHolder->IsEditable(), p_buffer ) );
		uint32_t l_index = 0;

		for ( auto l_variable : *p_buffer )
		{
			DoAddVariable( l_id, l_variable, p_buffer );
		}
	}

	void FrameVariablesList::DoAddVariable( wxTreeItemId p_id, FrameVariableSPtr p_variable, FrameVariableBufferSPtr p_buffer )
	{
		wxString l_displayName = p_variable->GetName();

		if ( p_variable->GetOccCount() > 1 )
		{
			l_displayName << wxT( "[" ) << p_variable->GetOccCount() << wxT( "]" );
		}

		AppendItem( p_id, l_displayName, eID_FRAME_VARIABLE, eID_FRAME_VARIABLE_SEL, new FrameVariableTreeItemProperty( m_propertiesHolder->IsEditable(), p_variable, p_buffer ) );
	}

	void FrameVariablesList::DoAddVariable( wxTreeItemId p_id, Castor3D::FrameVariableSPtr p_variable, Castor3D::ShaderType p_type )
	{
		wxString l_displayName = p_variable->GetName();

		if ( p_variable->GetOccCount() > 1 )
		{
			l_displayName << wxT( "[" ) << p_variable->GetOccCount() << wxT( "]" );
		}

		AppendItem( p_id, l_displayName, eID_FRAME_VARIABLE, eID_FRAME_VARIABLE_SEL, new FrameVariableTreeItemProperty( m_propertiesHolder->IsEditable(), p_variable, p_type ) );
	}

	BEGIN_EVENT_TABLE( FrameVariablesList, wxTreeCtrl )
		EVT_CLOSE( FrameVariablesList::OnClose )
		EVT_TREE_SEL_CHANGED( wxID_ANY, FrameVariablesList::OnSelectItem )
		EVT_TREE_ITEM_RIGHT_CLICK( wxID_ANY, FrameVariablesList::OnMouseRButtonUp )
	END_EVENT_TABLE()

	void FrameVariablesList::OnClose( wxCloseEvent & p_event )
	{
		DeleteAllItems();
		p_event.Skip();
	}

	void FrameVariablesList::OnSelectItem( wxTreeEvent & p_event )
	{
		TreeItemProperty * l_data = reinterpret_cast< TreeItemProperty * >( p_event.GetClientObject() );
		m_propertiesHolder->SetPropertyData( l_data );
		p_event.Skip();
	}

	void FrameVariablesList::OnMouseRButtonUp( wxTreeEvent & p_event )
	{
	}
}
