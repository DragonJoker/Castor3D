#include "GuiCommon/Properties/TreeItems/RenderTargetTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"
#include "GuiCommon/Properties/TreeItems/PostEffectTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/ToneMappingTreeItemProperty.hpp"
#include "GuiCommon/Shader/ShaderDialog.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Plugin/ToneMappingPlugin.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/PostEffect/PostEffect.hpp>
#include <Castor3D/Render/Technique/RenderTechnique.hpp>
#include <Castor3D/Render/Technique/Voxelize/VoxelSceneData.hpp>

#include <wx/propgrid/advprops.h>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	void AppendRenderTarget( wxTreeCtrlBase * list
		, bool editable
		, wxTreeItemId id
		, RenderTarget & target )
	{
		auto targetId = list->AppendItem( id
			, make_wxString( target.getName() )
			, eBMP_RENDER_TARGET
			, eBMP_RENDER_TARGET_SEL
			, new RenderTargetTreeItemProperty( editable, target ) );

		for ( auto postEffect : target.getHDRPostEffects() )
		{
			wxTreeItemId idPostEffect = list->AppendItem( targetId
				, _( "HDR - " ) + make_wxString( postEffect->getFullName() )
				, eBMP_POST_EFFECT
				, eBMP_POST_EFFECT_SEL
				, new PostEffectTreeItemProperty( editable, *postEffect, list ) );
		}

		for ( auto postEffect : target.getSRGBPostEffects() )
		{
			wxTreeItemId idPostEffect = list->AppendItem( targetId
				, _( "SRGB - " ) + make_wxString( postEffect->getFullName() )
				, eBMP_POST_EFFECT
				, eBMP_POST_EFFECT_SEL
				, new PostEffectTreeItemProperty( editable, *postEffect, list ) );
		}

		wxTreeItemId idToneMapping = list->AppendItem( targetId
			, _( "Tone Mapping" )
			, eBMP_TONE_MAPPING
			, eBMP_TONE_MAPPING_SEL
			, new ToneMappingTreeItemProperty( editable, target, list ) );
	}

	RenderTargetTreeItemProperty::RenderTargetTreeItemProperty( bool editable
		, RenderTarget & target )
		: TreeItemProperty( target.getEngine(), editable, ePROPERTY_DATA_TYPE_RENDER_TARGET )
		, m_target( target )
	{
		CreateTreeItemMenu();
	}

	RenderTargetTreeItemProperty::~RenderTargetTreeItemProperty()
	{
	}

	void RenderTargetTreeItemProperty::doCreateProperties( wxPGEditor * editor
		, wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_RENDER_TARGET = _( "Render Target: " );
		static wxString PROPERTY_RENDER_TARGET_SHADER = _( "Shader" );
		static wxString PROPERTY_RENDER_TARGET_EDIT_SHADER = _( "View Shaders..." );
#if C3D_DebugQuads
		static wxString PROPERTY_RENDER_TARGET_DEBUG_VIEW = _( "Debug View" );
#endif

		auto & target = getRenderTarget();
		wxString TARGETS[] =
		{
			_( "Window" ),
			_( "Texture" )
		};

		addProperty( grid, PROPERTY_CATEGORY_RENDER_TARGET + TARGETS[size_t( target.getTargetType() )] );

#if C3D_DebugQuads
		auto & debugConfig = target.getTechnique()->getDebugConfig();
		wxArrayString debugChoices;

		for ( auto & intermediate : target.getIntermediateViews() )
		{
			debugChoices.Add( make_wxString( intermediate.name ) );
		}

		addPropertyET( grid, PROPERTY_RENDER_TARGET_DEBUG_VIEW, debugChoices, &debugConfig.debugIndex );
#endif
		doCreateSsaoProperties( editor, grid );
	}

	void RenderTargetTreeItemProperty::doCreateSsaoProperties( wxPGEditor * editor
		, wxPropertyGrid * grid )
	{
		static wxString PROPERTY_RENDER_TARGET_SSAO = _( "SSAO" );
		static wxString PROPERTY_RENDER_TARGET_SSAO_ENABLED = _( "Enable SSAO" );
		static wxString PROPERTY_RENDER_TARGET_SSAO_HIGH_QUALITY = _( "High Quality" );
		static wxString PROPERTY_RENDER_TARGET_SSAO_NORMALS_BUFFER = _( "Normals Buffer" );
		static wxString PROPERTY_RENDER_TARGET_SSAO_RADIUS = _( "Radius" );
		static wxString PROPERTY_RENDER_TARGET_SSAO_BIAS = _( "Bias" );
		static wxString PROPERTY_RENDER_TARGET_SSAO_INTENSITY = _( "Intensity" );
		static wxString PROPERTY_RENDER_TARGET_SSAO_SAMPLES = _( "Samples" );
		static wxString PROPERTY_RENDER_TARGET_SSAO_EDGE_SHARPNESS = _( "Edge Sharpness" );
		static wxString PROPERTY_RENDER_TARGET_SSAO_BLUR_HIGH_QUALITY = _( "High Quality Blur" );
		static wxString PROPERTY_RENDER_TARGET_SSAO_BLUR_STEP_SIZE = _( "Blur Step Size" );
		static wxString PROPERTY_RENDER_TARGET_SSAO_BLUR_RADIUS = _( "Blur Radius" );
		static wxString PROPERTY_RENDER_TARGET_SSAO_BEND_STEP_COUNT = _( "Bend Step Count" );
		static wxString PROPERTY_RENDER_TARGET_SSAO_BEND_STEP_SIZE = _( "Bend Step Size" );

		auto & target = getRenderTarget();
		auto & ssaoConfig = target.getTechnique()->getSsaoConfig();
		addProperty( grid, PROPERTY_RENDER_TARGET_SSAO );
		addPropertyT( grid, PROPERTY_RENDER_TARGET_SSAO_ENABLED, &ssaoConfig.enabled );
		addPropertyT( grid, PROPERTY_RENDER_TARGET_SSAO_HIGH_QUALITY, &ssaoConfig.highQuality );
		addPropertyT( grid, PROPERTY_RENDER_TARGET_SSAO_NORMALS_BUFFER, &ssaoConfig.useNormalsBuffer );
		addPropertyT( grid, PROPERTY_RENDER_TARGET_SSAO_RADIUS, &ssaoConfig.radius );
		addPropertyT( grid, PROPERTY_RENDER_TARGET_SSAO_BIAS, &ssaoConfig.bias );
		addPropertyT( grid, PROPERTY_RENDER_TARGET_SSAO_INTENSITY, &ssaoConfig.intensity );
		addPropertyT( grid, PROPERTY_RENDER_TARGET_SSAO_SAMPLES, &ssaoConfig.numSamples );
		addPropertyT( grid, PROPERTY_RENDER_TARGET_SSAO_EDGE_SHARPNESS, &ssaoConfig.edgeSharpness, 0.1f );
		addPropertyT( grid, PROPERTY_RENDER_TARGET_SSAO_BLUR_HIGH_QUALITY, &ssaoConfig.blurHighQuality );
		addPropertyT( grid, PROPERTY_RENDER_TARGET_SSAO_BLUR_STEP_SIZE, &ssaoConfig.blurStepSize );
		addPropertyT( grid, PROPERTY_RENDER_TARGET_SSAO_BLUR_RADIUS, &ssaoConfig.blurRadius );
		addPropertyT( grid, PROPERTY_RENDER_TARGET_SSAO_BEND_STEP_COUNT, &ssaoConfig.bendStepCount );
		addPropertyT( grid, PROPERTY_RENDER_TARGET_SSAO_BEND_STEP_SIZE, &ssaoConfig.bendStepSize );
	}
}
