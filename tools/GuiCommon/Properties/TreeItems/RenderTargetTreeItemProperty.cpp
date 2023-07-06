#include "GuiCommon/Properties/TreeItems/RenderTargetTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"
#include "GuiCommon/Properties/TreeItems/PostEffectTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/RenderPassTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/SsaoConfigTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/ToneMappingTreeItemProperty.hpp"
#include "GuiCommon/Shader/ShaderDialog.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Plugin/ToneMappingPlugin.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/PostEffect/PostEffect.hpp>
#include <Castor3D/Render/RenderTechnique.hpp>
#include <Castor3D/Render/RenderTechniquePass.hpp>
#include <Castor3D/Render/GlobalIllumination/VoxelConeTracing/VoxelSceneData.hpp>
#include <Castor3D/Scene/Scene.hpp>

#include <wx/propgrid/advprops.h>

namespace GuiCommon
{
	void appendRenderTarget( wxTreeCtrlBase * list
		, bool editable
		, wxTreeItemId id
		, castor3d::RenderTarget & target )
	{
		auto targetId = list->AppendItem( id
			, make_wxString( target.getName() )
			, eBMP_RENDER_TARGET
			, eBMP_RENDER_TARGET_SEL
			, new RenderTargetTreeItemProperty( editable, target ) );

		list->AppendItem( targetId
			, _( "SSAO Configuration" )
			, eBMP_SSAO_CONFIG
			, eBMP_SSAO_CONFIG_SEL
			, new SsaoConfigTreeItemProperty{ editable
				, target.getEngine()
				, target.getSsaoConfig() } );

		for ( auto & postEffect : target.getHDRPostEffects() )
		{
			if ( target.isFullLoadingEnabled() || postEffect->isEnabled() )
			{
				list->AppendItem( targetId
					, _( "HDR - " ) + make_wxString( postEffect->getFullName() )
					, eBMP_POST_EFFECT
					, eBMP_POST_EFFECT_SEL
					, new PostEffectTreeItemProperty( editable, *postEffect, list ) );
			}
		}

		for ( auto & postEffect : target.getSRGBPostEffects() )
		{
			if ( target.isFullLoadingEnabled() || postEffect->isEnabled() )
			{
				list->AppendItem( targetId
					, _( "SRGB - " ) + make_wxString( postEffect->getFullName() )
					, eBMP_POST_EFFECT
					, eBMP_POST_EFFECT_SEL
					, new PostEffectTreeItemProperty( editable, *postEffect, list ) );
			}
		}

		list->AppendItem( targetId
			, _( "Tone Mapping" )
			, eBMP_TONE_MAPPING
			, eBMP_TONE_MAPPING_SEL
			, new ToneMappingTreeItemProperty( editable, target, list ) );
	}

	RenderTargetTreeItemProperty::RenderTargetTreeItemProperty( bool editable
		, castor3d::RenderTarget & target )
		: TreeItemProperty( target.getEngine(), editable )
		, m_target( target )
	{
		CreateTreeItemMenu();
	}

	void RenderTargetTreeItemProperty::doCreateProperties( wxPGEditor * editor
		, wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_RENDER_TARGET = _( "Render Target: " );
		static wxString PROPERTY_RENDER_TARGET_SHADER = _( "Shader" );
		static wxString PROPERTY_RENDER_TARGET_EDIT_SHADER = _( "View Shaders..." );

		auto & target = getRenderTarget();
		wxString TARGETS[] =
		{
			_( "Window" ),
			_( "Texture" )
		};

		addProperty( grid, PROPERTY_CATEGORY_RENDER_TARGET + TARGETS[size_t( target.getTargetType() )] );

#if C3D_DebugQuads
		static wxString PROPERTY_RENDER_WINDOW_DEBUG_VIEW = _( "Debug View" );
		static wxString PROPERTY_RENDER_WINDOW_DEBUG_SHADER_VALUE = _( "Debug Value" );

		auto & targetDebugConfig = target.getDebugConfig();
		auto & debugConfig = target.getScene()->getDebugConfig();
		addPropertyET( grid, PROPERTY_RENDER_WINDOW_DEBUG_VIEW, make_wxArrayString( targetDebugConfig.getIntermediateImages() ), &targetDebugConfig.intermediateImageIndex );
		addPropertyET( grid, PROPERTY_RENDER_WINDOW_DEBUG_SHADER_VALUE, make_wxArrayString( debugConfig.getIntermediateValues() ), &debugConfig.intermediateShaderValueIndex );
#endif

		if ( target.isFullLoadingEnabled()
			|| target.getScene()->getVoxelConeTracingConfig().enabled )
		{
			doCreateVctProperties( editor, grid );
		}

		for ( auto & renderPass : target.getCustomRenderPasses() )
		{
			if ( renderPass->isPassEnabled() )
			{
				addProperty( grid, wxT( "Render pass " ) + renderPass->getName() );
				setPrefix( renderPass->getName() );
				fillRenderPassConfiguration( grid, *this, *renderPass );
			}
		}

		setPrefix( {} );
	}

	void RenderTargetTreeItemProperty::doCreateVctProperties( wxPGEditor * editor
		, wxPropertyGrid * grid )
	{
		static wxString PROPERTY_VCT = _( "Voxel Cone Tracing" );
		static wxString PROPERTY_VCT_ENABLED = _( "Enable VCT" );
		static wxString PROPERTY_VCT_CONSERVATIVE_RASTERIZATION = _( "Conservative Rasterization" );
		static wxString PROPERTY_VCT_OCCLUSION = _( "Occlusion" );
		static wxString PROPERTY_VCT_TEMPORAL_SMOOTHING = _( "Temporal Smoothing" );
		static wxString PROPERTY_VCT_SECONDARY_BOUNCE = _( "Secondary Bounce" );
		static wxString PROPERTY_VCT_NUM_CONES = _( "Num. Cones" );
		static wxString PROPERTY_VCT_MAX_DISTANCE = _( "Max. Distance" );
		static wxString PROPERTY_VCT_RAY_STEP_SIZE = _( "Ray Step Size" );
		static wxString PROPERTY_VCT_VOXEL_SIZE = _( "Voxel Size" );

		auto & vctConfig = getRenderTarget().getScene()->getVoxelConeTracingConfig();
		addProperty( grid, PROPERTY_VCT );
		addPropertyT( grid, PROPERTY_VCT_ENABLED, &vctConfig.enabled );
		addPropertyT( grid, PROPERTY_VCT_CONSERVATIVE_RASTERIZATION, &vctConfig.enableConservativeRasterization );
		addPropertyT( grid, PROPERTY_VCT_OCCLUSION, &vctConfig.enableOcclusion );
		addPropertyT( grid, PROPERTY_VCT_SECONDARY_BOUNCE, &vctConfig.enableSecondaryBounce );
		addPropertyT( grid, PROPERTY_VCT_TEMPORAL_SMOOTHING, &vctConfig.enableTemporalSmoothing );
		addPropertyT( grid, PROPERTY_VCT_NUM_CONES, &vctConfig.numCones );
		addPropertyT( grid, PROPERTY_VCT_MAX_DISTANCE, &vctConfig.maxDistance );
		addPropertyT( grid, PROPERTY_VCT_RAY_STEP_SIZE, &vctConfig.rayStepSize );
		addPropertyT( grid, PROPERTY_VCT_VOXEL_SIZE, &vctConfig.voxelSizeFactor );
	}
}
