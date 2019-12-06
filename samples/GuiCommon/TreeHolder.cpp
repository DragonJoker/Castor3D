#include "GuiCommon/TreeHolder.hpp"

#include "GuiCommon/AuiDockArt.hpp"
#include "GuiCommon/AuiToolBarArt.hpp"
#include "GuiCommon/ImagesLoader.hpp"

#include <wx/treectrl.h>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	namespace
	{
		enum eIDS
		{
			eCollapseAll,
			eExpandAll,
		};
	}

	TreeHolder::TreeHolder( wxWindow * parent
		, wxPoint const & position
		, wxSize const & size )
		: wxPanel( parent, wxID_ANY, position, size )
		, m_auiManager( this, wxAUI_MGR_ALLOW_FLOATING | wxAUI_MGR_TRANSPARENT_HINT | wxAUI_MGR_HINT_FADE | wxAUI_MGR_VENETIAN_BLINDS_HINT | wxAUI_MGR_LIVE_RESIZE )
	{
		m_auiManager.SetArtProvider( new AuiDockArt );
		SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_toolBar = new wxAuiToolBar( this
			, wxID_ANY
			, wxDefaultPosition
			, wxDefaultSize
			, wxAUI_TB_PLAIN_BACKGROUND | wxAUI_TB_HORIZONTAL );
		m_toolBar->SetArtProvider( new AuiToolBarArt );
		m_toolBar->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_toolBar->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_toolBar->SetToolBitmapSize( wxSize( 16, 16 ) );
		m_toolBar->AddTool( eCollapseAll
			, _( "Collapse all" )
			, wxImage( *ImagesLoader::getBitmap( eBMP_COLLAPSE_ALL ) ).Rescale( 16, 16, wxIMAGE_QUALITY_HIGH )
			, _( "Collapse all the nodes of the tree" ) );
		m_toolBar->AddTool( eExpandAll
			, _( "Expand all" )
			, wxImage( *ImagesLoader::getBitmap( eBMP_EXPAND_ALL ) ).Rescale( 16, 16, wxIMAGE_QUALITY_HIGH )
			, _( "Expands all the nodes of the tree" ) );
		m_toolBar->Realize();

		m_auiManager.AddPane( m_toolBar
			, wxAuiPaneInfo().Name( wxT( "TreeToolBar" ) )
				.ToolbarPane()
				.Top()
				.Row( 1 )
				.Dockable( false )
				.Gripper( false ) );
		m_auiManager.Update();
	}

	TreeHolder::~TreeHolder()
	{
		m_auiManager.UnInit();
	}

	void TreeHolder::setTree( wxTreeCtrl * tree )
	{
		m_tree = tree;
		m_auiManager.AddPane( m_tree
			, wxAuiPaneInfo().Name( wxT( "Tree" ) )
				.CaptionVisible( false )
				.Center()
				.CloseButton( false )
				.Name( wxT( "Render" ) )
				.Layer( 0 )
				.Movable( false )
				.PaneBorder( false )
				.Dockable( false ) );
		m_auiManager.Update();
	}

	BEGIN_EVENT_TABLE( TreeHolder, wxPanel )
		EVT_TOOL( eCollapseAll, TreeHolder::onCollapseAll )
		EVT_TOOL( eExpandAll, TreeHolder::onExpandAll )
	END_EVENT_TABLE()

	void TreeHolder::onCollapseAll( wxCommandEvent & event )
	{
		m_tree->CollapseAll();
		//event.Skip();
	}

	void TreeHolder::onExpandAll( wxCommandEvent & event )
	{
		m_tree->ExpandAll();
		//event.Skip();
	}
}
