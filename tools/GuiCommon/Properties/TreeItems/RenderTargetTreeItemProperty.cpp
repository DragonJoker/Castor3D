#include "GuiCommon/Properties/TreeItems/RenderTargetTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"
#include "GuiCommon/Properties/TreeItems/PostEffectTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/SsaoConfigTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/ToneMappingTreeItemProperty.hpp"
#include "GuiCommon/Shader/ShaderDialog.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Plugin/ToneMappingPlugin.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/PostEffect/PostEffect.hpp>
#include <Castor3D/Render/Technique/RenderTechnique.hpp>
#include <Castor3D/Render/GlobalIllumination/VoxelConeTracing/VoxelSceneData.hpp>

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

		list->AppendItem( targetId
			, _( "SSAO Configuration" )
			, eBMP_SSAO_CONFIG
			, eBMP_SSAO_CONFIG_SEL
			, new SsaoConfigTreeItemProperty{ editable
				, target.getEngine()
				, target.getSsaoConfig() } );

		for ( auto postEffect : target.getHDRPostEffects() )
		{
			list->AppendItem( targetId
				, _( "HDR - " ) + make_wxString( postEffect->getFullName() )
				, eBMP_POST_EFFECT
				, eBMP_POST_EFFECT_SEL
				, new PostEffectTreeItemProperty( editable, *postEffect, list ) );
		}

		for ( auto postEffect : target.getSRGBPostEffects() )
		{
			list->AppendItem( targetId
				, _( "SRGB - " ) + make_wxString( postEffect->getFullName() )
				, eBMP_POST_EFFECT
				, eBMP_POST_EFFECT_SEL
				, new PostEffectTreeItemProperty( editable, *postEffect, list ) );
		}

		list->AppendItem( targetId
			, _( "Tone Mapping" )
			, eBMP_TONE_MAPPING
			, eBMP_TONE_MAPPING_SEL
			, new ToneMappingTreeItemProperty( editable, target, list ) );
	}

	RenderTargetTreeItemProperty::RenderTargetTreeItemProperty( bool editable
		, RenderTarget & target )
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
	}
}
