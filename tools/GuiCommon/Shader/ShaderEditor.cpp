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
#include <Castor3D/Render/RenderTechnique.hpp>
#include <Castor3D/Render/RenderTechniquePass.hpp>

#include <ShaderAST/Visitors/SelectEntryPoint.hpp>

#if GC_HasGLSL
#	include <CompilerGlsl/compileGlsl.hpp>
#endif
#if GC_HasHLSL
#	include <CompilerHlsl/compileHlsl.hpp>
#endif
#include <CompilerSpirV/compileSpirV.hpp>

namespace GuiCommon
{
	namespace shdedt
	{
#if GC_HasGLSL
		static glsl::GlslExtensionSet getGLSLExtensions( uint32_t glslVersion )
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
				result.insert( glsl::EXT_shader_explicit_arithmetic_types_int8 );
				result.insert( glsl::EXT_shader_explicit_arithmetic_types_int16 );
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

	ShaderEditor::ShaderEditor( castor3d::Engine * engine
		, bool canEdit
		, StcContext & stcContext
		, ShaderEntryPoint const & shader
		, castor::Vector< UniformBufferValues > & ubos
		, ShaderLanguage language
		, wxWindow * parent
		, wxPoint const & position
		, const wxSize size )
		: wxPanel( parent, wxID_ANY, position, size )
		, m_auiManager( this, wxAUI_MGR_ALLOW_FLOATING | wxAUI_MGR_TRANSPARENT_HINT | wxAUI_MGR_HINT_FADE | wxAUI_MGR_VENETIAN_BLINDS_HINT | wxAUI_MGR_LIVE_RESIZE )
		, m_stcContext( stcContext )
		, m_shader( shader )
		, m_ubos( ubos )
		, m_canEdit( canEdit )
	{
		doListAvailableLanguages();
		doInitialiseLayout( engine );
		loadLanguage( language );
		m_frameVariablesList->loadVariables( castor3d::getVkShaderStage( m_shader.entryPoint ), m_ubos );
	}

	ShaderEditor::~ShaderEditor()
	{
		doCleanup();
		m_auiManager.UnInit();
	}

	void ShaderEditor::doInitialiseLayout( castor3d::Engine * engine )
	{
		static int constexpr ListWidth = 200;
		wxSize size = GetClientSize();
		// The editor
		m_editor = wxMakeWindowPtr< StcTextEditor >( m_stcContext, this, wxID_ANY, wxDefaultPosition, size );
		m_editor->Show();
#if wxMAJOR_VERSION >= 3 || ( wxMAJOR_VERSION == 2 && wxMINOR_VERSION >= 9 )
		m_editor->AlwaysShowScrollbars( true, true );
#endif

		// The frame variable properties holder
		m_frameVariablesProperties = wxMakeWindowPtr< PropertiesContainer >( m_canEdit, this, wxDefaultPosition, wxSize( ListWidth, 0 ) );
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
		m_frameVariablesList = wxMakeWindowPtr< FrameVariablesList >( engine, &*m_frameVariablesProperties, this, wxPoint( 0, 25 ), wxSize( ListWidth, 0 ) );
		m_frameVariablesList->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_frameVariablesList->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_frameVariablesList->Enable( m_canEdit );

		// Put all that in the AUI manager
		m_auiManager.SetArtProvider( new AuiDockArt );
		m_auiManager.AddPane( &*m_editor
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
		m_auiManager.AddPane( &*m_frameVariablesList
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
		m_auiManager.AddPane( &*m_frameVariablesProperties
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
		if ( m_sources.empty() )
		{
			return;
		}

		wxString extension;
		wxString source;
		auto it = m_sources.find( language );

		if ( it == m_sources.end() )
		{
			language = ShaderLanguage::SPIRV;
			it = m_sources.find( language );
		}

		if ( it != m_sources.end() )
		{
			source = it->second;

			switch ( language )
			{
			case ShaderLanguage::SPIRV:
				extension = wxT( ".spirv" );
				break;
#if GC_HasGLSL
			case ShaderLanguage::GLSL:
				extension = wxT( ".glsl" );
				break;
#endif
#if GC_HasHLSL
			case ShaderLanguage::HLSL:
				extension = wxT( ".hlsl" );
				break;
#endif
			}
		}
		else
		{
			source = make_wxString( m_shader.source.text );
			extension = wxT( ".spirv" );
		}

		m_editor->setText( source );
		m_editor->SetReadOnly( !m_canEdit );
		m_editor->initializePrefs( m_editor->determinePrefs( extension ) );
	}

	void ShaderEditor::doCleanup()
	{
		m_auiManager.DetachPane( &*m_editor );
	}

	void ShaderEditor::doListAvailableLanguages()
	{
		if ( m_shader.shader )
		{
			auto stage = getShaderStage( m_shader.entryPoint );
			auto entryPoints = ast::listEntryPoints( *m_shader.shader->getStatements() );
			auto it = std::find_if( entryPoints.begin()
				, entryPoints.end()
				, [stage]( ast::EntryPointConfig const & lookup )
				{
					return lookup.stage == stage;
				} );

			if ( it == entryPoints.end() )
			{
				return;
			}

			ast::ShaderAllocator shaderAllocator{ ast::AllocationMode::eIncremental };
			auto allocator = shaderAllocator.getBlock();
			ast::stmt::StmtCache compileStmtCache{ *allocator };
			ast::expr::ExprCache compileExprCache{ *allocator };
			auto statements = ast::selectEntryPoint( compileStmtCache
				, compileExprCache
				, *it
				, *m_shader.shader->getStatements() );

			spirv::SpirVConfig spvConfig{ spirv::v1_5 };
			m_sources.try_emplace( ShaderLanguage::SPIRV
				, make_wxString( spirv::writeSpirv( *allocator
					, *m_shader.shader
					, statements.get()
					, stage
					, spvConfig
					, true ) ) );
#if GC_HasGLSL
			glsl::GlslConfig glslConfig{ stage
				, glsl::v4_6
				, shdedt::getGLSLExtensions( glsl::v4_6 )
				, false
				, false
				, true
				, true
				, true
				, true };
			m_sources.try_emplace( ShaderLanguage::GLSL
				, make_wxString( glsl::compileGlsl( *allocator
					, *m_shader.shader
					, statements.get()
					, stage
					, {}
					, glslConfig ) ) );
#endif
#if GC_HasHLSL
			hlsl::HlslConfig hlslConfig{ hlsl::v6_6
				, stage
				, false };
			m_sources.try_emplace( ShaderLanguage::HLSL
				, make_wxString( hlsl::compileHlsl( *allocator
					, *m_shader.shader
					, statements.get()
					, stage
					, {}
					, hlslConfig ) ) );
#endif
			return;
		}

		auto spirvIndex = m_shader.source.text.find( "; Magic:" );

		if ( auto glslIndex = m_shader.source.text.find( '#' );
			glslIndex == 0u
				&& spirvIndex != castor::MbString::npos
				&& spirvIndex > glslIndex )
		{
#if GC_HasGLSL
			m_sources.try_emplace( ShaderLanguage::GLSL
				, make_wxString( m_shader.source.text.substr( 0u, spirvIndex ) ) );
#endif
			m_sources.try_emplace( ShaderLanguage::SPIRV
				, make_wxString( m_shader.source.text.substr( spirvIndex ) ) );
			return;
		}

		spirv::SpirVConfig spvConfig{ spirv::v1_5 };
		ast::ShaderAllocator shaderAllocator{ ast::AllocationMode::eIncremental };
		auto allocator = shaderAllocator.getBlock();
		m_sources.try_emplace( ShaderLanguage::SPIRV
			, make_wxString( spirv::displaySpirv( *allocator
				, m_shader.source.spirv ) ) );
	}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
	BEGIN_EVENT_TABLE( ShaderEditor, wxPanel )
		EVT_CLOSE( ShaderEditor::onClose )
	END_EVENT_TABLE()
#pragma GCC diagnostic pop

	void ShaderEditor::onClose( wxCloseEvent & event )
	{
		doCleanup();
		event.Skip();
	}
}
