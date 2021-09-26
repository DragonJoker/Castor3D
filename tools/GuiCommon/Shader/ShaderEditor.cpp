#include "GuiCommon/Shader/ShaderEditor.hpp"

#include "GuiCommon/Aui/AuiDockArt.hpp"
#include "GuiCommon/Shader/StcTextEditor.hpp"
#include "GuiCommon/Shader/FrameVariablesList.hpp"
#include "GuiCommon/Properties/PropertiesContainer.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/ShaderCache.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/RenderWindow.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Render/Technique/RenderTechnique.hpp>
#include <Castor3D/Render/Technique/RenderTechniquePass.hpp>

#if C3D_HasGLSL
#	include <CompilerGlsl/compileGlsl.hpp>
#endif
#if GC_HasHLSL
#	include <CompilerHlsl/compileHlsl.hpp>
#endif
#include <CompilerSpirV/compileSpirV.hpp>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	ShaderEditor::ShaderEditor( Engine * engine
		, bool canEdit
		, StcContext & stcContext
		, castor3d::ShaderModule const & module
		, std::vector< UniformBufferValues > & ubos
		, ShaderLanguage language
		, wxWindow * parent
		, wxPoint const & position
		, const wxSize size )
		: wxPanel( parent, wxID_ANY, position, size )
		, m_auiManager( this, wxAUI_MGR_ALLOW_FLOATING | wxAUI_MGR_TRANSPARENT_HINT | wxAUI_MGR_HINT_FADE | wxAUI_MGR_VENETIAN_BLINDS_HINT | wxAUI_MGR_LIVE_RESIZE )
		, m_stcContext( stcContext )
		, m_module( module )
		, m_ubos( ubos )
		, m_canEdit( canEdit )
	{
		doInitialiseLayout( engine );
		loadLanguage( language );
		m_frameVariablesList->loadVariables( m_module.stage, m_ubos );
	}

	ShaderEditor::~ShaderEditor()
	{
		doCleanup();
		m_auiManager.UnInit();
	}

	void ShaderEditor::doInitialiseLayout( Engine * engine )
	{
		static int constexpr ListWidth = 200;
		wxSize size = GetClientSize();
		// The editor
		m_editor = new StcTextEditor( m_stcContext, this, wxID_ANY, wxDefaultPosition, size );
		m_editor->Show();
#if wxMAJOR_VERSION >= 3 || ( wxMAJOR_VERSION == 2 && wxMINOR_VERSION >= 9 )
		m_editor->AlwaysShowScrollbars( true, true );
#endif

		// The frame variable properties holder
		m_frameVariablesProperties = new PropertiesContainer( m_canEdit, this, wxDefaultPosition, wxSize( ListWidth, 0 ) );
		m_frameVariablesProperties->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_frameVariablesProperties->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_frameVariablesProperties->SetCaptionBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_frameVariablesProperties->SetCaptionTextColour( PANEL_FOREGROUND_COLOUR );
		m_frameVariablesProperties->SetSelectionBackgroundColour( ACTIVE_TAB_COLOUR );
		m_frameVariablesProperties->SetSelectionTextColour( ACTIVE_TEXT_COLOUR );
		m_frameVariablesProperties->SetCellBackgroundColour( INACTIVE_TAB_COLOUR );
		m_frameVariablesProperties->SetCellTextColour( INACTIVE_TEXT_COLOUR );
		m_frameVariablesProperties->SetLineColour( BORDER_COLOUR );
		m_frameVariablesProperties->SetMarginColour( BORDER_COLOUR );
//
		// The frame variables list
		m_frameVariablesList = new FrameVariablesList( engine, m_frameVariablesProperties, this, wxPoint( 0, 25 ), wxSize( ListWidth, 0 ) );
		m_frameVariablesList->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_frameVariablesList->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_frameVariablesList->Enable( m_canEdit );

		// Put all that in the AUI manager
		m_auiManager.SetArtProvider( new AuiDockArt );
		m_auiManager.AddPane( m_editor
			, wxAuiPaneInfo()
				.CaptionVisible( false )
				.CloseButton( false )
				.Name( wxT( "Shaders" ) )
				.Caption( _( "Shaders" ) )
				.Center()
				.Layer( 0 )
				.Movable( false )
				.PaneBorder( false )
				.Dockable( false ) );
		m_auiManager.AddPane( m_frameVariablesList
			, wxAuiPaneInfo()
				.CaptionVisible( false )
				.CloseButton( false )
				.Name( wxT( "UniformBuffers" ) )
				.Caption( _( "Uniform Buffers" ) )
				.Right()
				.Dock()
				.LeftDockable()
				.RightDockable()
				.Movable()
				.PinButton()
				.Layer( 2 )
				.PaneBorder( false )
				.MinSize( ListWidth, 0 ) );
		m_auiManager.AddPane( m_frameVariablesProperties
			, wxAuiPaneInfo()
				.CaptionVisible( false )
				.CloseButton( false )
				.Name( wxT( "Properties" ) )
				.Caption( _( "Properties" ) )
				.Right()
				.Dock()
				.LeftDockable()
				.RightDockable()
				.Movable()
				.PinButton()
				.Layer( 2 )
				.PaneBorder( false )
				.MinSize( ListWidth, 0 ) );
		m_auiManager.Update();
	}

	void ShaderEditor::loadLanguage( ShaderLanguage language )
	{
		wxString extension;
		wxString source;

		switch ( language )
		{
		case GuiCommon::ShaderLanguage::SPIRV:
			extension = wxT( ".spirv" );
			source = make_wxString( spirv::writeSpirv( *m_module.shader
				, spirv::SpirVConfig{ spirv::SpirVConfig::v1_3 }
				, true ) );
			break;
#if C3D_HasGLSL
		case GuiCommon::ShaderLanguage::GLSL:
			extension = wxT( ".glsl" );
			source = make_wxString( glsl::compileGlsl( *m_module.shader
				, {}
				, {
					m_module.shader->getType(),
					460,
					true,
					false,
					false,
					true,
					true,
					true,
					true,
				} ) );
			break;
#endif
#if GC_HasHLSL
		case GuiCommon::ShaderLanguage::HLSL:
			extension = wxT( ".hlsl" );
			source = make_wxString( hlsl::compileHlsl( *m_module.shader
				, {}
				, {
					m_module.shader->getType(),
					false,
				} ) );
			break;
#endif
		}

		m_editor->setText( source );
		m_editor->SetReadOnly( !m_canEdit );
		m_editor->initializePrefs( m_editor->determinePrefs( extension ) );
	}

	void ShaderEditor::doCleanup()
	{
		m_auiManager.DetachPane( m_editor );
	}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
	BEGIN_EVENT_TABLE( ShaderEditor, wxPanel )
		EVT_CLOSE( ShaderEditor::onClose )
	END_EVENT_TABLE()
#pragma GCC diagnostic pop

	void ShaderEditor::onClose( wxCloseEvent & p_event )
	{
		doCleanup();
		p_event.Skip();
	}
}
