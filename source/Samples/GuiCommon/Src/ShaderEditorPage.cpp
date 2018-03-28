#include "ShaderEditorPage.hpp"

#include "AuiDockArt.hpp"
#include "StcTextEditor.hpp"
#include "FrameVariablesList.hpp"
#include "PropertiesContainer.hpp"

#include <Engine.hpp>
#include <Cache/WindowCache.hpp>
#include <Cache/ShaderCache.hpp>
#include <Material/Pass.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>
#include <Render/RenderWindow.hpp>
#include <Scene/Scene.hpp>
#include <Shader/ShaderProgram.hpp>
#include <Technique/RenderTechnique.hpp>
#include <Technique/RenderTechniquePass.hpp>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
//	ShaderEditorPage::ShaderEditorPage( bool p_bCanEdit
//		, StcContext & p_stcContext
//		, ShaderProgramSPtr p_shader
//		, ShaderType p_type
//		, Pass const & p_pass
//		, Scene const & p_scene
//		, wxWindow * p_parent
//		, wxPoint const & p_position
//		, const wxSize p_size )
//		: wxPanel( p_parent, wxID_ANY, p_position, p_size )
//		, m_shaderProgram( p_shader )
//		, m_stcContext( p_stcContext )
//		, m_auiManager( this, wxAUI_MGR_ALLOW_FLOATING | wxAUI_MGR_TRANSPARENT_HINT | wxAUI_MGR_HINT_FADE | wxAUI_MGR_VENETIAN_BLINDS_HINT | wxAUI_MGR_LIVE_RESIZE )
//		, m_shaderType( p_type )
//#if defined( NDEBUG )
//		, m_canEdit( p_bCanEdit )
//#else
//		, m_canEdit( p_bCanEdit || true )
//#endif
//	{
//		doInitialiseShaderLanguage();
//		doInitialiseLayout();
//
//		auto & engine = *p_shader->getRenderSystem()->getEngine();
//		auto lock = castor::makeUniqueLock( engine.getRenderWindowCache() );
//		auto it = engine.getRenderWindowCache().begin();
//
//		if ( it != engine.getRenderWindowCache().end() )
//		{
//			auto & technique = *it->second->getRenderTarget()->getTechnique();
//			auto textureFlags = p_pass.getTextureFlags();
//			auto passFlags = p_pass.getPassFlags();
//			auto sceneFlags = p_scene.getFlags();
//			ProgramFlags programFlags;
//			RenderPipelineRPtr pipeline;
//
//			if ( p_pass.hasAlphaBlending())
//			{
//				technique.getTransparentPass().updateFlags( passFlags
//					, textureFlags
//					, programFlags
//					, sceneFlags );
//				pipeline = technique.getTransparentPass().getPipelineBack( p_pass.getColourBlendMode()
//					, p_pass.getAlphaBlendMode()
//					, p_pass.getAlphaFunc()
//					, passFlags
//					, textureFlags
//					, programFlags
//					, sceneFlags );
//			}
//			else
//			{
//				technique.getOpaquePass().updateFlags( passFlags
//					, textureFlags
//					, programFlags
//					, sceneFlags );
//				pipeline = technique.getOpaquePass().getPipelineBack( p_pass.getColourBlendMode()
//					, p_pass.getAlphaBlendMode()
//					, p_pass.getAlphaFunc()
//					, passFlags
//					, textureFlags
//					, programFlags
//					, sceneFlags );
//			}
//
//			if ( pipeline )
//			{
//				doLoadPage( *pipeline );
//			}
//			else
//			{
//				Logger::logWarning( cuT( "Pipeline not found." ) );
//			}
//		}
//	}
//
//	ShaderEditorPage::~ShaderEditorPage()
//	{
//		doCleanup();
//		m_auiManager.UnInit();
//	}
//
//	bool ShaderEditorPage::LoadFile( wxString const & p_file )
//	{
//		m_shaderFile = p_file;
//		return m_editor->LoadFile( m_shaderFile );
//	}
//
//	void ShaderEditorPage::SaveFile( bool p_createIfNone )
//	{
//		ShaderProgramSPtr program = m_shaderProgram.lock();
//
//		if ( m_shaderFile.empty() && p_createIfNone )
//		{
//			wxString wildcard;
//			wildcard = _( "GLSL Files" );
//			wildcard += wxT( " (*.glsl;*.frag;*.vert;*.geom;*.ctrl;*.eval)|*.glsl;*.frag;*.vert;*.geom;*.ctrl;*.eval" );
//
//			wxFileDialog dialog( this, _( "Save Shader file " ), wxEmptyString, wxEmptyString, wildcard, wxFD_SAVE | wxFD_OVERWRITE_PROMPT );
//
//			if ( dialog.ShowModal() == wxID_OK )
//			{
//				m_shaderFile = dialog.GetPath();
//			}
//		}
//
//		if ( !m_shaderFile.empty() )
//		{
//			m_editor->SaveFile( m_shaderFile );
//			m_editor->SetReadOnly( !m_canEdit );
//		}
//	}
//
//	void ShaderEditorPage::doInitialiseShaderLanguage()
//	{
//	}
//
//	void ShaderEditorPage::doInitialiseLayout()
//	{
//		const int iListWidth = 200;
//		wxSize size = GetClientSize();
//		// The editor
//		m_editor = new StcTextEditor( m_stcContext, this, wxID_ANY, wxPoint( iListWidth, 0 ), size - wxSize( iListWidth, 0 ) );
//		m_editor->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
//		m_editor->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
//		m_editor->Show();
//#if wxMAJOR_VERSION >= 3 || ( wxMAJOR_VERSION == 2 && wxMINOR_VERSION >= 9 )
//		m_editor->AlwaysShowScrollbars( true, true );
//#endif
//
//		// The frame variable properties holder
//		m_frameVariablesProperties = new PropertiesContainer( m_canEdit, this, wxDefaultPosition, wxSize( iListWidth, 0 ) );
//		m_frameVariablesProperties->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
//		m_frameVariablesProperties->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
//		m_frameVariablesProperties->SetCaptionBackgroundColour( PANEL_BACKGROUND_COLOUR );
//		m_frameVariablesProperties->SetCaptionTextColour( PANEL_FOREGROUND_COLOUR );
//		m_frameVariablesProperties->SetSelectionBackgroundColour( ACTIVE_TAB_COLOUR );
//		m_frameVariablesProperties->SetSelectionTextColour( ACTIVE_TEXT_COLOUR );
//		m_frameVariablesProperties->SetCellBackgroundColour( INACTIVE_TAB_COLOUR );
//		m_frameVariablesProperties->SetCellTextColour( INACTIVE_TEXT_COLOUR );
//		m_frameVariablesProperties->SetLineColour( BORDER_COLOUR );
//		m_frameVariablesProperties->SetMarginColour( BORDER_COLOUR );
//
//		// The frame variables list
//		m_frameVariablesList = new FrameVariablesList( m_frameVariablesProperties, this, wxPoint( 0, 25 ), wxSize( iListWidth, 0 ) );
//		m_frameVariablesList->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
//		m_frameVariablesList->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
//		m_frameVariablesList->Enable( m_canEdit );
//
//		// Put all that in the AUI manager
//		m_auiManager.SetArtProvider( new AuiDockArt );
//		m_auiManager.AddPane( m_editor
//			, wxAuiPaneInfo()
//				.CaptionVisible( false )
//				.CloseButton( false )
//				.Name( wxT( "Shaders" ) )
//				.Caption( _( "Shaders" ) )
//				.Center()
//				.Layer( 0 )
//				.Movable( false )
//				.PaneBorder( false )
//				.Dockable( false ) );
//		m_auiManager.AddPane( m_frameVariablesList
//			, wxAuiPaneInfo()
//				.CaptionVisible( false )
//				.CloseButton( false )
//				.Name( wxT( "FrameVariablesList" ) )
//				.Caption( _( "Frame variables" ) )
//				.Right()
//				.Dock()
//				.LeftDockable()
//				.RightDockable()
//				.Movable()
//				.PinButton()
//				.Layer( 2 )
//				.PaneBorder( false )
//				.MinSize( iListWidth, 0 ) );
//		m_auiManager.AddPane( m_frameVariablesProperties
//			, wxAuiPaneInfo()
//				.CaptionVisible( false )
//				.CloseButton( false )
//				.Name( wxT( "Properties" ) )
//				.Caption( _( "Properties" ) )
//				.Right()
//				.Dock()
//				.LeftDockable()
//				.RightDockable()
//				.Movable()
//				.PinButton()
//				.Layer( 2 )
//				.PaneBorder( false )
//				.MinSize( iListWidth, 0 ) );
//		m_auiManager.Update();
//	}
//
//	void ShaderEditorPage::doLoadPage( RenderPipeline & p_pipeline )
//	{
//		ShaderProgramSPtr program = m_shaderProgram.lock();
//		wxString extension = wxT( ".glsl" );
//
//		wxArrayString arrayChoices;
//		arrayChoices.push_back( wxCOMBO_NEW );
//
//		if ( program->getObjectStatus( m_shaderType ) != ShaderStatus::eDontExist )
//		{
//			// Load the shader source file/text
//			m_shaderSource = program->getSource( m_shaderType );
//			m_shaderFile = program->getFile( m_shaderType );
//		}
//
//		// Load the shader in the editor
//		if ( !m_shaderFile.empty() )
//		{
//			// file
//			m_editor->LoadFile( m_shaderFile );
//			m_editor->SetReadOnly( !m_canEdit );
//		}
//		else if ( !m_shaderSource.empty() )
//		{
//			// or source (read only, then)
//			m_editor->setText( m_shaderSource );
//			m_editor->SetReadOnly( true );
//		}
//
//		// Initialise the editor
//		m_editor->InitializePrefs( m_editor->DeterminePrefs( extension ) );
//
//		// Load frame variables list
//		m_frameVariablesList->LoadVariables( m_shaderType
//			, program
//			, p_pipeline );
//	}
//
//	void ShaderEditorPage::doCleanup()
//	{
//		m_auiManager.DetachPane( m_editor );
//		m_auiManager.DetachPane( m_frameVariablesList );
//		m_auiManager.DetachPane( m_frameVariablesProperties );
//		m_frameVariablesList->DeleteAllItems();
//		m_frameVariablesProperties->DestroyChildren();
//	}
//
//	BEGIN_EVENT_TABLE( ShaderEditorPage, wxPanel )
//		EVT_CLOSE( ShaderEditorPage::OnClose )
//	END_EVENT_TABLE()
//
//	void ShaderEditorPage::OnClose( wxCloseEvent & p_event )
//	{
//		doCleanup();
//		p_event.Skip();
//	}
}
