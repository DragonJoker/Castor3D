#include "FrameVariablesList.hpp"

#include "ImagesLoader.hpp"
#include "FrameVariableBufferTreeItemProperty.hpp"
#include "FrameVariableTreeItemProperty.hpp"
#include "PropertiesContainer.hpp"

#include <Engine.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>

#include <wx/imaglist.h>

#include "xpms/frame_variable.xpm"
#include "xpms/frame_variable_sel.xpm"
#include "xpms/frame_variable_buffer.xpm"
#include "xpms/frame_variable_buffer_sel.xpm"

#ifdef LoadImage
#	undef LoadImage
#	define LoadImage wxBitmap::LoadImage
#endif

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	//typedef enum eID
	//{
	//	eID_FRAME_VARIABLE,
	//	eID_FRAME_VARIABLE_SEL,
	//	eID_FRAME_VARIABLE_BUFFER,
	//	eID_FRAME_VARIABLE_BUFFER_SEL,
	//}	eID;

	//FrameVariablesList::FrameVariablesList( PropertiesContainer * p_propertiesHolder
	//	, wxWindow * p_parent
	//	, wxPoint const & p_ptPos
	//	, wxSize const & p_size )
	//	: wxTreeCtrl( p_parent, wxID_ANY, p_ptPos, p_size, wxTR_DEFAULT_STYLE | wxTR_HIDE_ROOT | wxNO_BORDER )
	//	, m_propertiesHolder( p_propertiesHolder )
	//{
	//	wxBusyCursor wait;
	//	ImagesLoader::addBitmap( eBMP_FRAME_VARIABLE, frame_variable_xpm );
	//	ImagesLoader::addBitmap( eBMP_FRAME_VARIABLE_SEL, frame_variable_sel_xpm );
	//	ImagesLoader::addBitmap( eBMP_FRAME_VARIABLE_BUFFER, frame_variable_buffer_xpm );
	//	ImagesLoader::addBitmap( eBMP_FRAME_VARIABLE_BUFFER_SEL, frame_variable_buffer_sel_xpm );
	//	ImagesLoader::waitAsyncLoads();

	//	wxImage * icons[] =
	//	{
	//		ImagesLoader::getBitmap( eBMP_FRAME_VARIABLE ),
	//		ImagesLoader::getBitmap( eBMP_FRAME_VARIABLE_SEL ),
	//		ImagesLoader::getBitmap( eBMP_FRAME_VARIABLE_BUFFER ),
	//		ImagesLoader::getBitmap( eBMP_FRAME_VARIABLE_BUFFER_SEL ),
	//	};

	//	wxImageList * imageList = new wxImageList( GC_IMG_SIZE, GC_IMG_SIZE, true );

	//	for ( auto image : icons )
	//	{
	//		int sizeOrig = image->GetWidth();

	//		if ( sizeOrig != GC_IMG_SIZE )
	//		{
	//			image->Rescale( GC_IMG_SIZE, GC_IMG_SIZE, wxIMAGE_QUALITY_HIGHEST );
	//		}

	//		imageList->Add( wxImage( *image ) );
	//	}

	//	AssignImageList( imageList );
	//}

	//FrameVariablesList::~FrameVariablesList()
	//{
	//	UnloadVariables();
	//}

	//void FrameVariablesList::LoadVariables( ShaderType p_type
	//	, ShaderProgramSPtr p_program
	//	, RenderPipeline & p_pipeline )
	//{
	//	m_program = p_program;
	//	wxTreeItemId root = AddRoot( _( "Root" ) );

	//	for ( auto & binding : p_pipeline.getBindings() )
	//	{
	//		doAddBuffer( root, *binding.get().getOwner() );
	//	}

	//	if ( p_program->getObjectStatus( p_type ) != ShaderStatus::eDontExist )
	//	{
	//		for ( auto variable : p_program->getUniforms( p_type ) )
	//		{
	//			doAddVariable( root, variable, p_type );
	//		}
	//	}
	//}

	//void FrameVariablesList::UnloadVariables()
	//{
	//	DeleteAllItems();
	//}

	//void FrameVariablesList::doAddBuffer( wxTreeItemId p_id
	//	, UniformBuffer & p_buffer )
	//{
	//	wxTreeItemId id = AppendItem( p_id, p_buffer.getName()
	//		, eID_FRAME_VARIABLE_BUFFER
	//		, eID_FRAME_VARIABLE_BUFFER_SEL
	//		, new FrameVariableBufferTreeItemProperty( m_program.lock()->getRenderSystem()->getEngine()
	//			, m_propertiesHolder->IsEditable()
	//			, p_buffer ) );

	//	for ( auto variable : p_buffer )
	//	{
	//		doAddVariable( id, variable, p_buffer );
	//	}
	//}

	//void FrameVariablesList::doAddVariable( wxTreeItemId p_id
	//	, UniformSPtr p_variable
	//	, UniformBuffer & p_buffer )
	//{
	//	wxString displayName = p_variable->getName();

	//	if ( p_variable->getOccCount() > 1 )
	//	{
	//		displayName << wxT( "[" ) << p_variable->getOccCount() << wxT( "]" );
	//	}

	//	AppendItem( p_id
	//		, displayName
	//		, eID_FRAME_VARIABLE
	//		, eID_FRAME_VARIABLE_SEL
	//		, new FrameVariableTreeItemProperty( m_propertiesHolder->IsEditable()
	//			, p_variable
	//			, p_buffer ) );
	//}

	//void FrameVariablesList::doAddVariable( wxTreeItemId p_id
	//	, PushUniformSPtr p_variable
	//	, ShaderType p_type )
	//{
	//	wxString displayName = p_variable->getBaseUniform().getName();

	//	if ( p_variable->getBaseUniform().getOccCount() > 1 )
	//	{
	//		displayName << wxT( "[" ) << p_variable->getBaseUniform().getOccCount() << wxT( "]" );
	//	}

	//	AppendItem( p_id
	//		, displayName
	//		, eID_FRAME_VARIABLE
	//		, eID_FRAME_VARIABLE_SEL
	//		, new FrameVariableTreeItemProperty( m_propertiesHolder->IsEditable()
	//			, p_variable
	//			, p_type ) );
	//}

	//BEGIN_EVENT_TABLE( FrameVariablesList, wxTreeCtrl )
	//	EVT_CLOSE( FrameVariablesList::OnClose )
	//	EVT_TREE_SEL_CHANGED( wxID_ANY, FrameVariablesList::OnSelectItem )
	//	EVT_TREE_ITEM_RIGHT_CLICK( wxID_ANY, FrameVariablesList::OnMouseRButtonUp )
	//END_EVENT_TABLE()

	//void FrameVariablesList::OnClose( wxCloseEvent & p_event )
	//{
	//	UnloadVariables();
	//	p_event.Skip();
	//}

	//void FrameVariablesList::OnSelectItem( wxTreeEvent & p_event )
	//{
	//	TreeItemProperty * data = reinterpret_cast< TreeItemProperty * >( p_event.GetClientObject() );
	//	m_propertiesHolder->setPropertyData( data );
	//	p_event.Skip();
	//}

	//void FrameVariablesList::OnMouseRButtonUp( wxTreeEvent & p_event )
	//{
	//}
}
