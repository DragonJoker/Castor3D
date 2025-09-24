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
#include "GuiCommon/System/SceneObjectsTree.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/RenderTechnique.hpp>
#include <Castor3D/Render/RenderTechniquePass.hpp>
#include <Castor3D/Render/PostEffect/PostEffect.hpp>
#include <Castor3D/Scene/Scene.hpp>

#include <wx/propgrid/advprops.h>
#include <wx/treelist.h>

namespace GuiCommon
{
	void appendRenderTarget( SceneObjectsTree * list
		, bool editable
		, wxTreeItemId id
		, c3d::RenderTarget & target )
	{
		auto targetId = list->AppendItem( id
			, make_wxString( target.getName() )
			, int( eBMP::eRenderTarget )
			, int( eBMP::eRenderTargetSelected )
			, new SceneObjectsTree::DataType{ std::make_unique< RenderTargetTreeItemProperty >( list->getImagesLoader(), editable, target ) } );

		list->AppendItem( targetId
			, _( "SSAO Configuration" )
			, int( eBMP::eSSAOConfig )
			, int( eBMP::eSSAOConfigSelected )
			, new SceneObjectsTree::DataType{ std::make_unique< SsaoConfigTreeItemProperty >( list->getImagesLoader()
				, editable
				, target.getEngine()
				, target.getSsaoConfig() ) } );
		list->AppendItem( targetId
			, _( "Colour Grading Configuration" )
			, int( eBMP::eColourGradingConfig )
			, int( eBMP::eColourGradingConfigSelected )
			, new SceneObjectsTree::DataType{ std::make_unique< ColourGradingConfigTreeItemProperty >( list->getImagesLoader()
				, editable
				, target.getEngine()
				, target.getColourGradingConfig() ) } );
		list->AppendItem( targetId
			, _( "Clusters Configuration" )
			, int( eBMP::eClustersConfig )
			, int( eBMP::eClustersConfigSelected )
			, new SceneObjectsTree::DataType{ std::make_unique< ClustersConfigTreeItemProperty >( list->getImagesLoader()
				, editable
				, target.getEngine()
				, target.getClustersConfig() ) } );

		if ( target.isFullLoadingEnabled()
			|| target.getScene()->getVoxelConeTracingConfig().enabled )
		{
			list->AppendItem( targetId
				, _( "VCT Configuration" )
				, int( eBMP::eVCTConfig )
				, int( eBMP::eVCTConfigSelected )
				, new SceneObjectsTree::DataType{ std::make_unique< VCTConfigTreeItemProperty >( list->getImagesLoader()
					, editable
					, target.getEngine()
					, target.getScene()->getVoxelConeTracingConfig() ) } );
		}

		for ( auto & postEffect : target.getHDRPostEffects() )
		{
			if ( target.isFullLoadingEnabled() || postEffect->isEnabled() )
			{
				list->AppendItem( targetId
					, _( "HDR - " ) + make_wxString( postEffect->getFullName() )
					, int( eBMP::ePostEffect )
					, int( eBMP::ePostEffectSelected )
					, new SceneObjectsTree::DataType{ std::make_unique< PostEffectTreeItemProperty >( list->getImagesLoader()
						, editable, *postEffect, list ) } );
			}
		}

		for ( auto & postEffect : target.getSRGBPostEffects() )
		{
			if ( target.isFullLoadingEnabled() || postEffect->isEnabled() )
			{
				list->AppendItem( targetId
					, _( "SRGB - " ) + make_wxString( postEffect->getFullName() )
					, int( eBMP::ePostEffect )
					, int( eBMP::ePostEffectSelected )
					, new SceneObjectsTree::DataType{ std::make_unique< PostEffectTreeItemProperty >( list->getImagesLoader()
						, editable, *postEffect, list ) } );
			}
		}

		list->AppendItem( targetId
			, _( "Tone Mapping" )
			, int( eBMP::eToneMapping )
			, int( eBMP::eToneMappingSelected )
			, new SceneObjectsTree::DataType{ std::make_unique< ToneMappingTreeItemProperty >( list->getImagesLoader()
				, editable, target, list ) } );
	}

	RenderTargetTreeItemProperty::RenderTargetTreeItemProperty( ImagesLoader & imagesLoader
		, bool editable
		, c3d::RenderTarget & target )
		: TreeItemProperty( target.getEngine(), imagesLoader, editable )
		, m_target( target )
	{
		CreateTreeItemMenu();
	}

	void RenderTargetTreeItemProperty::doCreateProperties( wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_RENDER_TARGET = _( "Render Target: " );
		static wxString PROPERTY_RENDER_TARGET_SHADER = _( "Shader" );
		static wxString PROPERTY_RENDER_TARGET_EDIT_SHADER = _( "View Shaders..." );

		auto & target = getRenderTarget();
		c3d::Array< wxString, 2u > TARGETS{ _( "Window" )
			, _( "Texture" ) };

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
