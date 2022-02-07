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
	namespace
	{
#if C3D_HasGLSL
		glsl::GlslExtensionSet getGLSLExtensions( uint32_t glslVersion )
		{
			glsl::GlslExtensionSet result;

			if ( glslVersion >= glsl::v4_6 )
			{
				result.insert( glsl::EXT_shader_atomic_float );
				result.insert( glsl::EXT_ray_tracing );
				result.insert( glsl::EXT_ray_query );
				result.insert( glsl::EXT_scalar_block_layout );
			}

			if ( glslVersion >= glsl::v4_5 )
			{
				result.insert( glsl::ARB_shader_ballot );
				result.insert( glsl::ARB_shader_viewport_layer_array );
				result.insert( glsl::NV_stereo_view_rendering );
				result.insert( glsl::NVX_multiview_per_view_attributes );
				result.insert( glsl::EXT_nonuniform_qualifier );
				result.insert( glsl::NV_mesh_shader );
				result.insert( glsl::EXT_buffer_reference2 );
			}

			if ( glslVersion >= glsl::v4_3 )
			{
				result.insert( glsl::NV_viewport_array2 );
				result.insert( glsl::NV_shader_atomic_fp16_vector );
			}

			if ( glslVersion >= glsl::v4_2 )
			{
				result.insert( glsl::ARB_compute_shader );
				result.insert( glsl::ARB_explicit_uniform_location );
				result.insert( glsl::ARB_shading_language_420pack );
				result.insert( glsl::NV_shader_atomic_float );
			}

			if ( glslVersion >= glsl::v4_1 )
			{
				result.insert( glsl::ARB_shading_language_packing );
			}

			if ( glslVersion >= glsl::v4_0 )
			{
				result.insert( glsl::ARB_separate_shader_objects );
				result.insert( glsl::ARB_texture_cube_map_array );
				result.insert( glsl::ARB_texture_gather );
			}

			if ( glslVersion >= glsl::v3_3 )
			{
				result.insert( glsl::ARB_shader_stencil_export );
				result.insert( glsl::KHR_vulkan_glsl );
				result.insert( glsl::EXT_shader_explicit_arithmetic_types_int64 );
				result.insert( glsl::EXT_multiview );
				result.insert( glsl::ARB_explicit_attrib_location );
				result.insert( glsl::ARB_shader_image_load_store );
				result.insert( glsl::EXT_gpu_shader4 );
				result.insert( glsl::ARB_gpu_shader5 );
				result.insert( glsl::EXT_gpu_shader4_1 );
				result.insert( glsl::ARB_texture_query_lod );
				result.insert( glsl::ARB_texture_query_levels );
				result.insert( glsl::ARB_shader_draw_parameters );
				result.insert( glsl::ARB_fragment_layer_viewport );
				result.insert( glsl::ARB_tessellation_shader );
			}

			return result;
		}
#endif
	}

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
		spirv::SpirVConfig spvConfig{ spirv::v1_5 };

		switch ( language )
		{
		case GuiCommon::ShaderLanguage::SPIRV:
			extension = wxT( ".spirv" );
			source = make_wxString( spirv::writeSpirv( *m_module.shader
				, spvConfig
				, true ) );
			break;
#if C3D_HasGLSL
		case GuiCommon::ShaderLanguage::GLSL:
			{
				extension = wxT( ".glsl" );
				glsl::GlslConfig config{ m_module.shader->getType()
					, glsl::v4_6
					, getGLSLExtensions( glsl::v4_6 )
					, false
					, false
					, true
					, true
					, true
					, true };
				source = make_wxString( glsl::compileGlsl( *m_module.shader
					, {}
					, config ) );
			}
			break;
#endif
#if GC_HasHLSL
		case GuiCommon::ShaderLanguage::HLSL:
			{
				extension = wxT( ".hlsl" );
				hlsl::HlslConfig config{ hlsl::v6_6
					, m_module.shader->getType()
					, false };
				source = make_wxString( hlsl::compileHlsl( *m_module.shader
					, {}
					, config ) );
			}
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
