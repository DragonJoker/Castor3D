#include "GuiCommon/Shader/ShaderProgramPage.hpp"

#include "GuiCommon/Aui/AuiDockArt.hpp"
#include "GuiCommon/Aui/AuiTabArt.hpp"
#include "GuiCommon/Aui/AuiToolBarArt.hpp"
#include "GuiCommon/Shader/ShaderEditor.hpp"

namespace GuiCommon
{
	namespace shader
	{
		typedef enum eID
		{
			eID_PAGES,
		}	eID;
	}

	ShaderProgramPage::ShaderProgramPage( castor3d::Engine * engine
		, bool canEdit
		, StcContext & stcContext
		, ShaderSource & source
		, ShaderLanguage language
		, wxWindow * parent
		, wxPoint const & position
		, wxSize const & size )
		: wxPanel( parent, wxID_ANY, position, size )
		, m_engine( engine )
		, m_source( source )
		, m_stcContext( stcContext )
		, m_auiManager( this, wxAUI_MGR_ALLOW_FLOATING | wxAUI_MGR_TRANSPARENT_HINT | wxAUI_MGR_HINT_FADE | wxAUI_MGR_VENETIAN_BLINDS_HINT | wxAUI_MGR_LIVE_RESIZE )
		, m_canEdit( canEdit )
	{
		doInitialiseLayout( engine );
		doLoadPages( language );
	}

	ShaderProgramPage::~ShaderProgramPage()
	{
		doCleanup();
		m_auiManager.UnInit();
	}

	void ShaderProgramPage::loadLanguage( ShaderLanguage language )
	{
		for ( auto & page : m_pages )
		{
			page->loadLanguage( language );
		}
	}

	void ShaderProgramPage::doInitialiseLayout( castor3d::Engine * engine )
	{
		wxSize size = GetClientSize();
		m_editors = new wxAuiNotebook( this
			, shader::eID_PAGES
			, wxDefaultPosition
			, wxDefaultSize
			, wxAUI_NB_TOP | wxAUI_NB_TAB_MOVE | wxAUI_NB_TAB_FIXED_WIDTH | wxAUI_NB_SCROLL_BUTTONS );
		m_editors->SetArtProvider( new AuiTabArt );

		m_auiManager.SetArtProvider( new AuiDockArt );
		m_auiManager.AddPane( m_editors
			, wxAuiPaneInfo()
			.CaptionVisible( false )
			.Name( wxT( "Shaders" ) )
			.Caption( _( "Shaders" ) )
			.CenterPane()
			.Dock()
			.MinSize( size )
			.Layer( 1 )
			.PaneBorder( false ) );
		m_auiManager.Update();
	}

	void ShaderProgramPage::doLoadPages( ShaderLanguage language )
	{
		castor::Map< ast::EntryPoint, wxString > const texts
		{
			{ ast::EntryPoint::eVertex, _( "Vertex" ) },
			{ ast::EntryPoint::eTessellationControl, _( "Tessellation Control" ) },
			{ ast::EntryPoint::eTessellationEvaluation, _( "Tessellation Evaluation" ) },
			{ ast::EntryPoint::eGeometry, _( "Geometry" ) },
			{ ast::EntryPoint::eFragment, _( "Fragment" ) },
			{ ast::EntryPoint::eCompute, _( "Compute" ) },
			{ ast::EntryPoint::eMesh, _( "Mesh" ) },
			{ ast::EntryPoint::eTask, _( "Task" ) },
			{ ast::EntryPoint::eMeshNV, _( "Mesh" ) },
			{ ast::EntryPoint::eTaskNV, _( "Task" ) },
			{ ast::EntryPoint::eCallable, _( "Callable" ) },
			{ ast::EntryPoint::eRayAnyHit, _( "Ray Any Hit" ) },
			{ ast::EntryPoint::eRayClosestHit, _( "Ray Closest Hit" ) },
			{ ast::EntryPoint::eRayGeneration, _( "Ray Generation" ) },
			{ ast::EntryPoint::eRayIntersection, _( "Ray Intersection" ) },
			{ ast::EntryPoint::eRayMiss, _( "Ray Miss" ) },
		};

		for ( auto & source : m_source.sources )
		{
			// The editor page
			m_pages.push_back( new ShaderEditor{ m_engine
				, m_canEdit
				, m_stcContext
				, source
				, m_source.ubos
				, language
				, m_editors } );
			auto & page = *m_pages.back();
			page.SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
			page.SetForegroundColour( PANEL_FOREGROUND_COLOUR );
			m_editors->AddPage( &page, texts.at( source.entryPoint ), true );
			page.SetSize( 0, 22, m_editors->GetClientSize().x, m_editors->GetClientSize().y - 22 );
		}
	}

	void ShaderProgramPage::doCleanup()
	{
		m_auiManager.DetachPane( m_editors );
		m_editors->DeleteAllPages();
	}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
	BEGIN_EVENT_TABLE( ShaderProgramPage, wxPanel )
		EVT_CLOSE( ShaderProgramPage::onClose )
	END_EVENT_TABLE()
#pragma GCC diagnostic pop

	void ShaderProgramPage::onClose( wxCloseEvent & event )
	{
		doCleanup();
		event.Skip();
	}
}
