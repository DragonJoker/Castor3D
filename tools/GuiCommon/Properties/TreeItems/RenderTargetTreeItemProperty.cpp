#include "GuiCommon/Properties/TreeItems/RenderTargetTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"
#include "GuiCommon/Properties/TreeItems/ClustersConfigTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/ColourGradingConfigTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/PostEffectTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/RenderPassTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/SsaoConfigTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/ToneMappingTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/VCTConfigTreeItemProperty.hpp"
#include "GuiCommon/Shader/ShaderDialog.hpp"

#include <Castor3D/Config.hpp>
#include <Castor3D/Engine.hpp>
#include <Castor3D/Plugin/ToneMappingPlugin.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/PostEffect/PostEffect.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/RenderTechnique.hpp>
#include <Castor3D/Render/RenderTechniquePass.hpp>
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
		list->AppendItem( targetId
			, _( "Colour Grading Configuration" )
			, eBMP_COLOURGRADING_CONFIG
			, eBMP_COLOURGRADING_CONFIG_SEL
			, new ColourGradingConfigTreeItemProperty{ editable
				, target.getEngine()
				, target.getColourGradingConfig() } );
		list->AppendItem( targetId
			, _( "Clusters Configuration" )
			, eBMP_CLUSTERS_CONFIG
			, eBMP_CLUSTERS_CONFIG_SEL
			, new ClustersConfigTreeItemProperty{ editable
			, target.getEngine()
			, target } );

		if ( target.isFullLoadingEnabled()
			|| target.getScene()->getVoxelConeTracingConfig().enabled )
		{
			list->AppendItem( targetId
				, _( "VCT Configuration" )
				, eBMP_VCT_CONFIG
				, eBMP_VCT_CONFIG_SEL
				, new VCTConfigTreeItemProperty{ editable
				, target.getEngine()
				, target.getScene()->getVoxelConeTracingConfig() } );
		}

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

		if ( target.areDebugTargetsEnabled() )
		{
			static wxString PROPERTY_RENDER_WINDOW_DEBUG_VIEW = _( "Debug View" );
			static wxString PROPERTY_RENDER_WINDOW_DEBUG_SHADER_VALUE = _( "Debug Value" );

			auto & targetDebugConfig = target.getDebugConfig();
			auto & debugConfig = target.getScene()->getDebugConfig();
			addPropertyET( grid, PROPERTY_RENDER_WINDOW_DEBUG_VIEW, make_wxArrayString( targetDebugConfig.getIntermediateImages() ), &targetDebugConfig.intermediateImageIndex );
			addPropertyET( grid, PROPERTY_RENDER_WINDOW_DEBUG_SHADER_VALUE, make_wxArrayString( debugConfig.getIntermediateValues() ), &debugConfig.intermediateShaderValueIndex );
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
}
