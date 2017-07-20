#include "FrameVariablesList.hpp"

#include "ImagesLoader.hpp"
#include "FrameVariableBufferTreeItemProperty.hpp"
#include "FrameVariableTreeItemProperty.hpp"
#include "PropertiesHolder.hpp"

#include <Engine.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Shader/Uniform.hpp>
#include <Shader/UniformBuffer.hpp>
#include <Shader/UniformBufferBinding.hpp>
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

	FrameVariablesList::FrameVariablesList( PropertiesHolder * p_propertiesHolder
		, wxWindow * p_parent
		, wxPoint const & p_ptPos
		, wxSize const & p_size )
		: wxTreeCtrl( p_parent, wxID_ANY, p_ptPos, p_size, wxTR_DEFAULT_STYLE | wxTR_HIDE_ROOT | wxNO_BORDER )
		, m_propertiesHolder( p_propertiesHolder )
	{
		wxBusyCursor wait;
		ImagesLoader::AddBitmap( eBMP_FRAME_VARIABLE, frame_variable_xpm );
		ImagesLoader::AddBitmap( eBMP_FRAME_VARIABLE_SEL, frame_variable_sel_xpm );
		ImagesLoader::AddBitmap( eBMP_FRAME_VARIABLE_BUFFER, frame_variable_buffer_xpm );
		ImagesLoader::AddBitmap( eBMP_FRAME_VARIABLE_BUFFER_SEL, frame_variable_buffer_sel_xpm );
		ImagesLoader::WaitAsyncLoads();

		wxImage * icons[] =
		{
			ImagesLoader::GetBitmap( eBMP_FRAME_VARIABLE ),
			ImagesLoader::GetBitmap( eBMP_FRAME_VARIABLE_SEL ),
			ImagesLoader::GetBitmap( eBMP_FRAME_VARIABLE_BUFFER ),
			ImagesLoader::GetBitmap( eBMP_FRAME_VARIABLE_BUFFER_SEL ),
		};

		wxImageList * imageList = new wxImageList( GC_IMG_SIZE, GC_IMG_SIZE, true );

		for ( auto image : icons )
		{
			int sizeOrig = image->GetWidth();

			if ( sizeOrig != GC_IMG_SIZE )
			{
				image->Rescale( GC_IMG_SIZE, GC_IMG_SIZE, wxIMAGE_QUALITY_HIGHEST );
			}

			imageList->Add( wxImage( *image ) );
		}

		AssignImageList( imageList );
	}

	FrameVariablesList::~FrameVariablesList()
	{
		UnloadVariables();
	}

	void FrameVariablesList::LoadVariables( ShaderType p_type
		, ShaderProgramSPtr p_program
		, RenderPipeline & p_pipeline )
	{
		m_program = p_program;
		wxTreeItemId root = AddRoot( _( "Root" ) );

		for ( auto & binding : p_pipeline.GetBindings() )
		{
			DoAddBuffer( root, *binding.get().GetOwner() );
		}

		if ( p_program->GetObjectStatus( p_type ) != ShaderStatus::eDontExist )
		{
			for ( auto variable : p_program->GetUniforms( p_type ) )
			{
				DoAddVariable( root, variable, p_type );
			}
		}
	}

	void FrameVariablesList::UnloadVariables()
	{
		DeleteAllItems();
	}

	void FrameVariablesList::DoAddBuffer( wxTreeItemId p_id
		, UniformBuffer & p_buffer )
	{
		wxTreeItemId id = AppendItem( p_id, p_buffer.GetName()
			, eID_FRAME_VARIABLE_BUFFER
			, eID_FRAME_VARIABLE_BUFFER_SEL
			, new FrameVariableBufferTreeItemProperty( m_program.lock()->GetRenderSystem()->GetEngine()
				, m_propertiesHolder->IsEditable()
				, p_buffer ) );
		uint32_t index = 0;

		for ( auto variable : p_buffer )
		{
			DoAddVariable( id, variable, p_buffer );
		}
	}

	void FrameVariablesList::DoAddVariable( wxTreeItemId p_id
		, UniformSPtr p_variable
		, UniformBuffer & p_buffer )
	{
		wxString displayName = p_variable->GetName();

		if ( p_variable->GetOccCount() > 1 )
		{
			displayName << wxT( "[" ) << p_variable->GetOccCount() << wxT( "]" );
		}

		AppendItem( p_id
			, displayName
			, eID_FRAME_VARIABLE
			, eID_FRAME_VARIABLE_SEL
			, new FrameVariableTreeItemProperty( m_propertiesHolder->IsEditable()
				, p_variable
				, p_buffer ) );
	}

	void FrameVariablesList::DoAddVariable( wxTreeItemId p_id
		, PushUniformSPtr p_variable
		, ShaderType p_type )
	{
		wxString displayName = p_variable->GetBaseUniform().GetName();

		if ( p_variable->GetBaseUniform().GetOccCount() > 1 )
		{
			displayName << wxT( "[" ) << p_variable->GetBaseUniform().GetOccCount() << wxT( "]" );
		}

		AppendItem( p_id
			, displayName
			, eID_FRAME_VARIABLE
			, eID_FRAME_VARIABLE_SEL
			, new FrameVariableTreeItemProperty( m_propertiesHolder->IsEditable()
				, p_variable
				, p_type ) );
	}

	BEGIN_EVENT_TABLE( FrameVariablesList, wxTreeCtrl )
		EVT_CLOSE( FrameVariablesList::OnClose )
		EVT_TREE_SEL_CHANGED( wxID_ANY, FrameVariablesList::OnSelectItem )
		EVT_TREE_ITEM_RIGHT_CLICK( wxID_ANY, FrameVariablesList::OnMouseRButtonUp )
	END_EVENT_TABLE()

	void FrameVariablesList::OnClose( wxCloseEvent & p_event )
	{
		UnloadVariables();
		p_event.Skip();
	}

	void FrameVariablesList::OnSelectItem( wxTreeEvent & p_event )
	{
		TreeItemProperty * data = reinterpret_cast< TreeItemProperty * >( p_event.GetClientObject() );
		m_propertiesHolder->SetPropertyData( data );
		p_event.Skip();
	}

	void FrameVariablesList::OnMouseRButtonUp( wxTreeEvent & p_event )
	{
	}
}
