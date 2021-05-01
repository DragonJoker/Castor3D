#include "Aria/RendererPage.hpp"

#include "Aria/CategoryPanel.hpp"
#include "Aria/LayeredPanel.hpp"
#include "Aria/MainFrame.hpp"
#include "Aria/TestDatabase.hpp"
#include "Aria/TestPanel.hpp"
#include "Aria/TestsCounts.hpp"
#include "Aria/Aui/AuiDockArt.hpp"
#include "Aria/DatabaseTest.hpp"
#include "Aria/Editor/SceneFileDialog.hpp"
#include "Aria/Model/TreeModel.hpp"
#include "Aria/Model/TreeModelNode.hpp"

#include <CastorUtils/Design/BlockGuard.hpp>
#include <CastorUtils/Design/BlockGuard.hpp>
#include <CastorUtils/Miscellaneous/Utils.hpp>

#include <wx/clipbrd.h>
#include <wx/progdlg.h>
#include <wx/stattext.h>

namespace aria
{
	//*********************************************************************************************

	namespace
	{
		enum ID
		{
			eID_GRID,
		};

		struct TestView
		{
			enum Type : size_t
			{
				eNone,
				eTest,
				eCategory,
			};
		};

		struct GeneralView
		{
			enum Type : size_t
			{
				eNone,
				eAll,
			};
		};
	}

	//*********************************************************************************************

	RendererPage::RendererPage( Config const & config
		, Renderer renderer
		, TestRunCategoryMap & runs
		, RendererTestsCounts & counts
		, wxWindow * parent
		, wxWindow * frame
		, wxMenu * testMenu
		, wxMenu * categoryMenu
		, wxMenu * rendererMenu
		, wxMenu * allMenu
		, wxMenu * busyMenu )
		: wxPanel{ parent, wxID_ANY, wxDefaultPosition, wxDefaultSize }
		, m_config{ config }
		, m_renderer{ renderer }
		, m_testMenu{ testMenu }
		, m_categoryMenu{ categoryMenu }
		, m_rendererMenu{ rendererMenu }
		, m_allMenu{ allMenu }
		, m_busyMenu{ busyMenu }
		, m_auiManager{ this, wxAUI_MGR_ALLOW_FLOATING | wxAUI_MGR_TRANSPARENT_HINT | wxAUI_MGR_HINT_FADE | wxAUI_MGR_VENETIAN_BLINDS_HINT | wxAUI_MGR_LIVE_RESIZE }
		, m_runs{ &runs }
		, m_counts{ counts }
		, m_model{ new TreeModel{ m_config, renderer, counts } }
	{
		doInitLayout( frame );
	}

	RendererPage::~RendererPage()
	{
		m_auiManager.UnInit();
	}

	TreeModelNode * RendererPage::getTestNode( DatabaseTest const & test )const
	{
		auto it = m_modelNodes.find( test->test->id );
		return it->second;
	}

	void RendererPage::refreshView()const
	{
		m_view->Refresh();
	}

	void RendererPage::resizeModel( wxSize const & size )
	{
		auto genSize = m_generalViews->GetSize();
		m_model->resize( m_view
			, { size.GetWidth() - genSize.GetWidth(), size.GetHeight() } );
	}

	void RendererPage::listLatestRuns( TestDatabase & database
		, TestMap const & tests
		, AllTestsCounts & counts
		, wxProgressDialog & progress
		, int & index )
	{
		database.listLatestRuns( m_renderer
			, tests
			, *m_runs
			, progress
			, index );

		for ( auto & category : *m_runs )
		{
			auto testsIt = tests.find( category.first );
			auto & catCounts = counts.addCategory( m_renderer
				, category.first
				, testsIt->second
				, category.second );
			m_model->addCategory( category.first, catCounts );

			for ( auto & run : category.second )
			{
				catCounts.addTest( run );
				auto testNode = m_model->addTest( run );
				m_modelNodes[run->test->id] = testNode;
				progress.Update( index++
					, _( "Filling tests list" )
					+ wxT( "\n" ) + getProgressDetails( run ) );
				progress.Fit();
			}
		}

		auto & rendCounts = counts.getRenderer( m_renderer );
		m_categoryView->update( m_renderer->name
			, rendCounts );
		m_allView->update( _( "All" )
			, counts );
		m_model->expandRoots( m_view );
		m_auiManager.Update();
		m_allView->refresh();
	}

	uint32_t RendererPage::getSelectedRange( TestRunMap const & runs )const
	{
		uint32_t range = 0u;

		for ( auto & item : m_selected.items )
		{
			auto node = static_cast< TreeModelNode * >( item.GetID() );

			if ( isRendererNode( *node ) )
			{
				auto rendIt = runs.find( node->renderer );

				if ( rendIt != runs.end() )
				{
					for ( auto & category : rendIt->second )
					{
						range += category.second.size();
					}
				}
			}
		}

		return range;
	}

	uint32_t RendererPage::getSelectedCategoryRange( TestRunMap const & runs )const
	{
		uint32_t range = 0u;

		for ( auto & item : m_selected.items )
		{
			auto node = static_cast< TreeModelNode * >( item.GetID() );

			if ( isCategoryNode( *node ) )
			{
				auto rendIt = runs.find( node->renderer );

				if ( rendIt != runs.end() )
				{
					auto catIt = rendIt->second.find( node->category );

					if ( catIt != rendIt->second.end() )
					{
						range += catIt->second.size();
					}
				}
			}
		}

		return range;
	}

	void RendererPage::updateTest( TreeModelNode * node )
	{
		m_model->ItemChanged( wxDataViewItem{ node } );
		m_categoryView->refresh();
	}

	std::vector< wxDataViewItem > RendererPage::listRendererTests( TestRunMap const & runs
		, FilterFunc filter )const
	{
		std::vector< wxDataViewItem > result;

		for ( auto & item : m_selected.items )
		{
			auto node = static_cast< TreeModelNode * >( item.GetID() );

			if ( isRendererNode( *node ) )
			{
				auto rendIt = runs.find( node->renderer );

				if ( rendIt != runs.end() )
				{
					for ( auto & category : rendIt->second )
					{
						for ( auto & run : category.second )
						{
							if ( filter( run ) )
							{
								result.push_back( wxDataViewItem{ getTestNode( run ) } );
							}
						}
					}
				}
			}
		}

		return result;
	}

	std::vector< wxDataViewItem > RendererPage::listCategoryTests( TestRunMap const & runs
		, RendererPage::FilterFunc filter )const
	{
		std::vector< wxDataViewItem > result;

		for ( auto & item : m_selected.items )
		{
			auto node = static_cast< TreeModelNode * >( item.GetID() );

			if ( isCategoryNode( *node ) )
			{
				auto rendIt = runs.find( node->renderer );

				if ( rendIt != runs.end() )
				{
					auto catIt = rendIt->second.find( node->category );

					if ( catIt != rendIt->second.end() )
					{
						for ( auto & run : catIt->second )
						{
							if ( filter( run ) )
							{
								result.push_back( wxDataViewItem{ getTestNode( run ) } );
							}
						}
					}
				}
			}
		}

		return result;
	}

	std::vector< wxDataViewItem > RendererPage::listSelectedTests()const
	{
		std::vector< wxDataViewItem > result;

		for ( auto & item : m_selected.items )
		{
			TreeModelNode * node = static_cast< TreeModelNode * >( item.GetID() );

			if ( isTestNode( *node ) )
			{
				result.push_back( item );
			}
		}

		return result;
	}

	void RendererPage::copyTestFileName()const
	{
		if ( m_selected.items.size() == 1 )
		{
			auto & item = *m_selected.items.begin();
			auto node = static_cast< TreeModelNode * >( item.GetID() );

			if ( isTestNode( *node ) && wxTheClipboard->Open() )
			{
				auto guard = castor::makeBlockGuard( []()
					{
						wxTheClipboard->Close();
					} );
				wxTheClipboard->SetData( new wxTextDataObject( makeWxString( getTestFileName( m_config.test, *node->test ) ) ) );
			}
		}
	}

	void RendererPage::viewTestSceneFile()
	{
		if ( m_selected.items.size() == 1 )
		{
			auto & item = *m_selected.items.begin();
			auto node = static_cast< TreeModelNode * >( item.GetID() );

			if ( isTestNode( *node ) )
			{
				auto filePath = getTestFileName( m_config.test, *node->test );
				auto editor = new SceneFileDialog{ m_config
					, makeWxString( filePath )
					, makeWxString( filePath.getFileName( true ) )
					, this };
				editor->Show();
			}
		}
	}

	void RendererPage::viewTest( wxProcess * process
		, wxStaticText * statusText )const
	{
		if ( m_selected.items.size() == 1 )
		{
			auto & item = *m_selected.items.begin();
			auto node = static_cast< TreeModelNode * >( item.GetID() );

			if ( isTestNode( *node ) )
			{
				wxString command = m_config.viewer;
				command << " " << makeWxString( getTestFileName( m_config.test, *node->test ) );
				command << " -" << node->test->getRenderer()->name;
				statusText->SetLabel( _( "Viewing: " ) + node->test->getName() );
				auto result = wxExecute( command
					, wxEXEC_SYNC
					, process );

				if ( result != 0 )
				{
					castor::Logger::logError( "doViewTest: " + castor::System::getLastErrorText() );
				}
			}
		}
	}

	void RendererPage::setTestsReferences( AllTestsCounts & counts )
	{
		if ( !m_selected.items.empty() )
		{
			int index = 0;
			wxProgressDialog progress{ wxT( "Updating tests reference" )
				, wxT( "Updating tests reference..." )
				, int( m_selected.items.size() )
				, this };

			for ( auto & item : m_selected.items )
			{
				auto node = static_cast< TreeModelNode * >( item.GetID() );

				if ( isTestNode( *node ) )
				{
					auto & run = *node->test;
					progress.Update( index++
						, _( "Setting reference\n" )
						+ wxT( "\n" ) + getProgressDetails( run ) );
					progress.Fit();
					doUpdateTestStatus( run, counts, TestStatus::eNegligible, true );
					m_model->ItemChanged( item );
				}
			}
		}
	}

	void RendererPage::ignoreTestsResult( bool ignore )
	{
		if ( !m_selected.items.empty() )
		{
			int index = 0;
			wxProgressDialog progress{ wxT( "Ignoring tests results" )
				, wxT( "Ignoring tests results..." )
				, int( m_selected.items.size() )
				, this };

			for ( auto & item : m_selected.items )
			{
				auto node = static_cast< TreeModelNode * >( item.GetID() );

				if ( isTestNode( *node ) )
				{
					auto & run = *node->test;
					progress.Update( index++
						, _( "Ignoring" )
						+ wxT( "\n" ) + getProgressDetails( run ) );
					progress.Fit();
					run.updateIgnoreResult( ignore
						, m_config.castorRefDate
						, true );
					m_model->ItemChanged( item );
				}
			}
		}
	}

	void RendererPage::updateTestsCastorDate()
	{
		if ( !m_selected.items.empty() )
		{
			int index = 0;
			wxProgressDialog progress{ wxT( "Setting Castor Date" )
				, wxT( "Setting Castor Date..." )
				, int( m_selected.items.size() )
				, this };

			for ( auto & item : m_selected.items )
			{
				auto node = static_cast< TreeModelNode * >( item.GetID() );

				if ( isTestNode( *node ) )
				{
					auto & run = *node->test;
					progress.Update( index++
						, _( "Setting reference" )
						+ wxT( "\n" ) + getProgressDetails( run ) );
					progress.Fit();
					run.updateCastorDate( m_config.castorRefDate );
				}
			}
		}
	}

	void RendererPage::updateTestsSceneDate()
	{
		if ( !m_selected.items.empty() )
		{
			int index = 0;
			wxProgressDialog progress{ wxT( "Setting Scene Date" )
				, wxT( "Setting Scene Date..." )
				, int( m_selected.items.size() )
				, this };

			for ( auto & item : m_selected.items )
			{
				auto node = static_cast< TreeModelNode * >( item.GetID() );

				if ( isTestNode( *node ) )
				{
					auto & run = *node->test;
					progress.Update( index++
						, _( "Setting reference" )
						+ wxT( "\n" ) + getProgressDetails( run ) );
					progress.Fit();
					run.updateSceneDate();
				}
			}
		}
	}

	void RendererPage::addCategory( Category category
		, CategoryTestsCounts & catCounts )
	{
		m_model->addCategory( category
			, catCounts
			, true );
	}

	void RendererPage::addTest( DatabaseTest & dbTest )
	{
		auto node = m_model->addTest( dbTest, true );
		m_modelNodes.emplace( dbTest->id, node );
	}

	void RendererPage::updateTestView( DatabaseTest const & test
		, AllTestsCounts & counts )
	{
		wxDataViewItem testItem{ getTestNode( test ) };
		m_model->ItemChanged( testItem );
		wxDataViewItem categoryItem{ m_model->GetParent( testItem ) };
		m_model->ItemChanged( categoryItem );
		wxDataViewItem rendererItem{ m_model->GetParent( categoryItem ) };
		m_model->ItemChanged( rendererItem );

		if ( m_detailViews->isLayerShown( TestView::eTest )
			&& m_testView->getTest() == &test )
		{
			m_testView->refresh();
		}

		m_allView->update( _( "All" )
			, counts );
	}

	void RendererPage::doInitLayout( wxWindow * frame )
	{
		wxPanel * listPanel = new wxPanel{ this
			, wxID_ANY
			, wxDefaultPosition
			, wxDefaultSize };
		listPanel->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		listPanel->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		wxBoxSizer * sizerList = new wxBoxSizer{ wxVERTICAL };
		m_view = new wxDataViewCtrl{ listPanel
			, eID_GRID
			, wxDefaultPosition
			, wxDefaultSize
			, wxDV_MULTIPLE };
		// Intently inverted back and fore.
		m_view->SetBackgroundColour( PANEL_FOREGROUND_COLOUR );
		m_view->SetForegroundColour( PANEL_BACKGROUND_COLOUR );
		sizerList->Add( m_view, wxSizerFlags( 1 ).Expand() );
		listPanel->SetSizer( sizerList );
		sizerList->SetSizeHints( listPanel );

		m_view->AssociateModel( m_model.get() );
		m_view->Connect( wxEVT_DATAVIEW_SELECTION_CHANGED
			, wxDataViewEventHandler( RendererPage::onSelectionChange )
			, nullptr
			, this );
		m_view->Connect( wxEVT_DATAVIEW_ITEM_CONTEXT_MENU
			, wxDataViewEventHandler( RendererPage::onItemContextMenu )
			, nullptr
			, this );
		m_model->instantiate( m_view );

		auto size = frame->GetClientSize();
		m_generalViews = new LayeredPanel{ this
			, wxDefaultPosition
			, wxDefaultSize };
		auto generalViews = m_generalViews;
		generalViews->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		generalViews->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		auto layer = new wxPanel{ generalViews, wxID_ANY, wxDefaultPosition, size };
		layer->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		layer->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		generalViews->addLayer( layer );
		m_allView = new CategoryPanel{ m_config, generalViews, wxDefaultPosition, size };
		generalViews->addLayer( m_allView );
		generalViews->showLayer( GeneralView::eAll );

		m_detailViews = new LayeredPanel{ this
			, wxDefaultPosition
			, wxDefaultSize };
		auto detailViews = m_detailViews;
		detailViews->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		detailViews->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		layer = new wxPanel{ detailViews, wxID_ANY, wxDefaultPosition, size };
		layer->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		layer->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		detailViews->addLayer( layer );
		m_testView = new TestPanel{ detailViews, m_config };
		detailViews->addLayer( m_testView );
		m_categoryView = new CategoryPanel{ m_config, detailViews, wxDefaultPosition, size };
		detailViews->addLayer( m_categoryView );
		detailViews->showLayer( TestView::eCategory );

		m_auiManager.AddPane( listPanel
			, wxAuiPaneInfo()
			.Layer( 0 )
			.Caption( _( "Tests List" ) )
			.CaptionVisible( true )
			.CloseButton( false )
			.PaneBorder( false )
			.Center()
			.Movable( false )
			.Dockable( false )
			.Floatable( false ) );
		m_auiManager.AddPane( generalViews
			, wxAuiPaneInfo()
			.Layer( 0 )
			.MinSize( 200, 200 )
			.Caption( _( "General View" ) )
			.CaptionVisible( true )
			.CloseButton( false )
			.PaneBorder( false )
			.Right()
			.Movable( true )
			.Dockable( true )
			.Floatable( true )
			.RightDockable( true )
			.LeftDockable( true )
			.BottomDockable( true )
			.TopDockable( true ) );
		m_auiManager.AddPane( detailViews
			, wxAuiPaneInfo()
			.Layer( 0 )
			.MinSize( 200, 200 )
			.Caption( _( "Details View" ) )
			.CaptionVisible( true )
			.CloseButton( false )
			.PaneBorder( false )
			.Bottom()
			.Movable( true )
			.Dockable( true )
			.Floatable( true )
			.BottomDockable( true )
			.TopDockable( true ) );
		m_auiManager.SetArtProvider( new AuiDockArt );
		m_auiManager.Update();
	}

	void RendererPage::doUpdateTestStatus( DatabaseTest & test
		, AllTestsCounts & counts
		, TestStatus newStatus
		, bool reference )
	{
		test.updateStatus( newStatus, reference );
		updateTestView( test, counts );
	}

	void RendererPage::onSelectionChange( wxDataViewEvent & evt )
	{
		auto wasDisplayingTest = m_detailViews->isLayerShown( TestView::eTest );
		auto wasDisplayingCategory = m_detailViews->isLayerShown( TestView::eCategory );
		m_selected.allTests = true;
		m_selected.allCategories = true;
		m_selected.allRenderers = true;
		m_view->GetSelections( m_selected.items );
		bool displayTest = false;
		bool displayCategory = false;

		if ( m_selected.items.size() == 1 )
		{
			TreeModelNode * node = static_cast< TreeModelNode * >( m_selected.items[0].GetID() );

			if ( node )
			{
				Category category{};
				Renderer renderer{};
				DatabaseTest * test{};

				if ( node->test )
				{
					test = node->test;
				}
				else if ( node->isRootNode() )
				{
					renderer = node->renderer;
				}
				else if ( node->GetParent() )
				{
					renderer = node->renderer;
					category = node->category;
				}

				if ( test )
				{
					if ( !isPending( test->getStatus() )
						&& !isRunning( test->getStatus() ) )
					{
						m_testView->setTest( *test );
						m_detailViews->showLayer( TestView::eTest );
						displayTest = true;
						m_selected.allTests = true;
					}
				}
				else if ( category )
				{
					auto & catCounts = m_counts.getCounts( category );
					m_categoryView->update( category->name
						, catCounts );
					m_detailViews->showLayer( TestView::eCategory );
					displayCategory = true;
				}
				else if ( renderer )
				{
					m_categoryView->update( renderer->name
						, m_counts );
					m_detailViews->showLayer( TestView::eCategory );
					displayCategory = true;
				}
			}
		}

		for ( auto & item : m_selected.items )
		{
			auto node = static_cast< TreeModelNode * >( item.GetID() );
			m_selected.allRenderers = isRendererNode( *node )
				&& m_selected.allRenderers;
			m_selected.allCategories = isCategoryNode( *node )
				&& m_selected.allCategories;
			m_selected.allTests = node->test
				&& m_selected.allTests;
		}

		if ( !displayTest && !displayCategory )
		{
			m_detailViews->hideLayers();
		}

		if ( wasDisplayingCategory != displayCategory
			|| wasDisplayingTest != displayTest )
		{
			// Page change
			m_auiManager.Update();
		}
	}

	void RendererPage::onItemContextMenu( wxDataViewEvent & evt )
	{
		if ( !m_selected.items.empty() )
		{
			if ( m_selected.allTests )
			{
				if ( m_selected.items.size() <= 1 )
				{
					m_testMenu->Enable( MainFrame::eID_TEST_IGNORE_RESULT, true );
					m_testMenu->Check( MainFrame::eID_TEST_IGNORE_RESULT, static_cast< TreeModelNode * >( m_selected.items.front().GetID() )->test->getIgnoreResult() );
				}
				else
				{
					m_testMenu->Enable( MainFrame::eID_TEST_IGNORE_RESULT, false );
					m_testMenu->Check( MainFrame::eID_TEST_IGNORE_RESULT, false );
				}

				PopupMenu( m_testMenu );
			}
			else if ( m_selected.allCategories )
			{
				PopupMenu( m_categoryMenu );
			}
			else if ( m_selected.allRenderers )
			{
				PopupMenu( m_rendererMenu );
			}
			else
			{
				PopupMenu( m_allMenu );
			}
		}
		else
		{
			PopupMenu( m_busyMenu );
		}
	}

	//*********************************************************************************************
}
