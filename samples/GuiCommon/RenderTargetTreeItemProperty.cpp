#include "GuiCommon/RenderTargetTreeItemProperty.hpp"

#include "GuiCommon/AdditionalProperties.hpp"
#include "GuiCommon/ShaderDialog.hpp"

#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Technique/RenderTechnique.hpp>

#include <wx/propgrid/advprops.h>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	namespace
	{
		static wxString PROPERTY_CATEGORY_RENDER_TARGET = _( "Render Target: " );
		static wxString PROPERTY_RENDER_TARGET_SHADER = _( "Shader" );
		static wxString PROPERTY_RENDER_TARGET_EDIT_SHADER = _( "View Shaders..." );
		static wxString PROPERTY_RENDER_TARGET_SSAO = _( "SSAO" );
		static wxString PROPERTY_RENDER_TARGET_SSAO_ENABLED = _( "Enabled" );
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
	}

	RenderTargetTreeItemProperty::RenderTargetTreeItemProperty( bool editable
		, RenderTargetSPtr target )
		: TreeItemProperty( target->getEngine(), editable, ePROPERTY_DATA_TYPE_RENDER_TARGET )
		, m_target( target )
	{
		PROPERTY_CATEGORY_RENDER_TARGET = _( "Render Target: " );
		PROPERTY_RENDER_TARGET_SHADER = _( "Shader" );
		PROPERTY_RENDER_TARGET_EDIT_SHADER = _( "View Shaders..." );

		CreateTreeItemMenu();
	}

	RenderTargetTreeItemProperty::~RenderTargetTreeItemProperty()
	{
	}

	void RenderTargetTreeItemProperty::doCreateProperties( wxPGEditor * editor
		, wxPropertyGrid * grid )
	{
		RenderTargetSPtr target = getRenderTarget();

		if ( target )
		{
			wxString TARGETS[] =
			{
				_( "Window" ),
				_( "Texture" )
			};

			grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_RENDER_TARGET + TARGETS[size_t( target->getTargetType() )] ) );

			for ( auto & postEffect : target->getHDRPostEffects() )
			{
				grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_RENDER_TARGET + TARGETS[size_t( target->getTargetType() )] ) );
			}

			grid->Append( new wxPropertyCategory( PROPERTY_RENDER_TARGET_SSAO ) );
			auto & ssaoConfig = target->getTechnique()->getSsaoConfig();
			grid->Append( new wxBoolProperty( PROPERTY_RENDER_TARGET_SSAO_ENABLED, PROPERTY_RENDER_TARGET_SSAO_ENABLED, ssaoConfig.enabled ) );
			grid->Append( new wxBoolProperty( PROPERTY_RENDER_TARGET_SSAO_HIGH_QUALITY, PROPERTY_RENDER_TARGET_SSAO_HIGH_QUALITY, ssaoConfig.highQuality ) );
			grid->Append( new wxBoolProperty( PROPERTY_RENDER_TARGET_SSAO_NORMALS_BUFFER, PROPERTY_RENDER_TARGET_SSAO_NORMALS_BUFFER, ssaoConfig.useNormalsBuffer ) );
			grid->Append( new wxFloatProperty( PROPERTY_RENDER_TARGET_SSAO_RADIUS, PROPERTY_RENDER_TARGET_SSAO_RADIUS, ssaoConfig.radius ) );
			grid->Append( new wxFloatProperty( PROPERTY_RENDER_TARGET_SSAO_BIAS, PROPERTY_RENDER_TARGET_SSAO_BIAS, ssaoConfig.bias ) );
			grid->Append( new wxFloatProperty( PROPERTY_RENDER_TARGET_SSAO_INTENSITY, PROPERTY_RENDER_TARGET_SSAO_INTENSITY, ssaoConfig.intensity ) );
			grid->Append( new wxUIntProperty( PROPERTY_RENDER_TARGET_SSAO_SAMPLES, PROPERTY_RENDER_TARGET_SSAO_SAMPLES, ssaoConfig.numSamples ) );
			grid->Append( new wxFloatProperty( PROPERTY_RENDER_TARGET_SSAO_EDGE_SHARPNESS, PROPERTY_RENDER_TARGET_SSAO_EDGE_SHARPNESS, ssaoConfig.edgeSharpness ) );
			grid->Append( new wxBoolProperty( PROPERTY_RENDER_TARGET_SSAO_BLUR_HIGH_QUALITY, PROPERTY_RENDER_TARGET_SSAO_BLUR_HIGH_QUALITY, ssaoConfig.blurHighQuality ) );
			grid->Append( new wxUIntProperty( PROPERTY_RENDER_TARGET_SSAO_BLUR_STEP_SIZE, PROPERTY_RENDER_TARGET_SSAO_BLUR_STEP_SIZE, ssaoConfig.blurStepSize ) );
			grid->Append( new wxIntProperty( PROPERTY_RENDER_TARGET_SSAO_BLUR_RADIUS, PROPERTY_RENDER_TARGET_SSAO_BLUR_RADIUS, ssaoConfig.blurRadius->value() ) );
		}
	}

	void RenderTargetTreeItemProperty::doPropertyChange( wxPropertyGridEvent & event )
	{
		RenderTargetSPtr target = getRenderTarget();
		wxPGProperty * property = event.GetProperty();

		if ( property && target )
		{
			if ( property->GetName() == PROPERTY_RENDER_TARGET_SSAO_ENABLED )
			{
				OnSsaoEnable( property->GetValue() );
			}
			else if ( property->GetName() == PROPERTY_RENDER_TARGET_SSAO_HIGH_QUALITY )
			{
				OnSsaoHighQuality( property->GetValue() );
			}
			else if ( property->GetName() == PROPERTY_RENDER_TARGET_SSAO_NORMALS_BUFFER )
			{
				OnSsaoNormalsBuffer( property->GetValue() );
			}
			else if ( property->GetName() == PROPERTY_RENDER_TARGET_SSAO_RADIUS )
			{
				OnSsaoRadius( float( property->GetValue().GetDouble() ) );
			}
			else if ( property->GetName() == PROPERTY_RENDER_TARGET_SSAO_BIAS )
			{
				OnSsaoBias( float( property->GetValue().GetDouble() ) );
			}
			else if ( property->GetName() == PROPERTY_RENDER_TARGET_SSAO_INTENSITY )
			{
				OnSsaoIntensity( float( property->GetValue().GetDouble() ) );
			}
			else if ( property->GetName() == PROPERTY_RENDER_TARGET_SSAO_SAMPLES )
			{
				OnSsaoSamples( uint32_t( property->GetValue().GetLong() ) );
			}
			else if ( property->GetName() == PROPERTY_RENDER_TARGET_SSAO_EDGE_SHARPNESS )
			{
				OnSsaoEdgeSharpness( float( property->GetValue().GetDouble() ) );
			}
			else if ( property->GetName() == PROPERTY_RENDER_TARGET_SSAO_BLUR_HIGH_QUALITY )
			{
				OnSsaoBlurHighQuality( property->GetValue() );
			}
			else if ( property->GetName() == PROPERTY_RENDER_TARGET_SSAO_BLUR_STEP_SIZE )
			{
				OnSsaoBlurStepSize( uint32_t( property->GetValue().GetLong() ) );
			}
			else if ( property->GetName() == PROPERTY_RENDER_TARGET_SSAO_BLUR_RADIUS )
			{
				OnSsaoBlurRadius( int32_t( property->GetValue().GetLong() ) );
			}
		}
	}

	void RenderTargetTreeItemProperty::OnSsaoEnable( bool value )
	{
		RenderTarget & target = *getRenderTarget();

		doApplyChange( [value, &target]()
			{
				target.getTechnique()->getSsaoConfig().enabled = value;
			} );
	}

	void RenderTargetTreeItemProperty::OnSsaoHighQuality( bool value )
	{
		RenderTarget & target = *getRenderTarget();

		doApplyChange( [value, &target]()
			{
				target.getTechnique()->getSsaoConfig().highQuality = value;
			} );
	}

	void RenderTargetTreeItemProperty::OnSsaoNormalsBuffer( bool value )
	{
		RenderTarget & target = *getRenderTarget();

		doApplyChange( [value, &target]()
			{
				target.getTechnique()->getSsaoConfig().useNormalsBuffer = value;
			} );
	}

	void RenderTargetTreeItemProperty::OnSsaoRadius( float value )
	{
		RenderTarget & target = *getRenderTarget();

		doApplyChange( [value, &target]()
			{
				target.getTechnique()->getSsaoConfig().radius = value;
			} );
	}

	void RenderTargetTreeItemProperty::OnSsaoBias( float value )
	{
		RenderTarget & target = *getRenderTarget();

		doApplyChange( [value, &target]()
			{
				target.getTechnique()->getSsaoConfig().bias = value;
			} );
	}

	void RenderTargetTreeItemProperty::OnSsaoIntensity( float value )
	{
		RenderTarget & target = *getRenderTarget();

		doApplyChange( [value, &target]()
			{
				target.getTechnique()->getSsaoConfig().intensity = value;
			} );
	}

	void RenderTargetTreeItemProperty::OnSsaoSamples( uint32_t value )
	{
		RenderTarget & target = *getRenderTarget();

		doApplyChange( [value, &target]()
			{
				target.getTechnique()->getSsaoConfig().numSamples = value;
			} );
	}

	void RenderTargetTreeItemProperty::OnSsaoEdgeSharpness( float value )
	{
		RenderTarget & target = *getRenderTarget();

		doApplyChange( [value, &target]()
			{
				target.getTechnique()->getSsaoConfig().edgeSharpness = value;
			} );
	}

	void RenderTargetTreeItemProperty::OnSsaoBlurHighQuality( bool value )
	{
		RenderTarget & target = *getRenderTarget();

		doApplyChange( [value, &target]()
			{
				target.getTechnique()->getSsaoConfig().blurHighQuality = value;
			} );
	}

	void RenderTargetTreeItemProperty::OnSsaoBlurStepSize( uint32_t value )
	{
		RenderTarget & target = *getRenderTarget();

		doApplyChange( [value, &target]()
			{
				target.getTechnique()->getSsaoConfig().blurStepSize = value;
			} );
	}

	void RenderTargetTreeItemProperty::OnSsaoBlurRadius( int32_t value )
	{
		RenderTarget & target = *getRenderTarget();

		doApplyChange( [value, &target]()
			{
				auto radius = target.getTechnique()->getSsaoConfig().blurRadius.value();
				radius = value;
				target.getTechnique()->getSsaoConfig().blurRadius = radius;
			} );
	}
}
