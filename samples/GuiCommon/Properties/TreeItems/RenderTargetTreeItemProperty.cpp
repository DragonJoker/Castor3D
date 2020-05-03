#include "GuiCommon/Properties/TreeItems/RenderTargetTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"
#include "GuiCommon/Shader/ShaderDialog.hpp"

#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/Technique/RenderTechnique.hpp>

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
		static wxString PROPERTY_RENDER_TARGET_DEBUG = _( "Debug Views" );
		static wxString PROPERTY_RENDER_TARGET_DEBUG_LIGHT_EYE = _( "Light Eye" );
		static wxString PROPERTY_RENDER_TARGET_DEBUG_LIGHT_VSPOSITION = _( "Light VS Position" );
		static wxString PROPERTY_RENDER_TARGET_DEBUG_LIGHT_WSPOSITION = _( "Light WS Position" );
		static wxString PROPERTY_RENDER_TARGET_DEBUG_LIGHT_WSNORMAL = _( "Light WS Normal" );
		static wxString PROPERTY_RENDER_TARGET_DEBUG_LIGHT_TEXCOORD = _( "Light Tex Coord" );
		static wxString PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_DIFFUSELIGHTING = _( "Deferred Diffuse Lighting" );
		static wxString PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_SPECULARLIGHTING = _( "Deferred Specular Lighting" );
		static wxString PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_OCCLUSION = _( "Deferred Occlusion" );
		static wxString PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_SSSTRANSMITTANCE = _( "Deferred SSS Transmittance" );
		static wxString PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_IBL = _( "Deferred IBL" );
		static wxString PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_NORMALS = _( "Deferred Normals" );
		static wxString PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_WORLDPOS = _( "Deferred World Pos" );
		static wxString PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_VIEWPOS = _( "Deferred View Pos" );
		static wxString PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_DEPTH = _( "Deferred Depth" );
		static wxString PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_DATA1 = _( "Deferred Data 1" );
		static wxString PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_DATA2 = _( "Deferred Data 2" );
		static wxString PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_DATA3 = _( "Deferred Data 3" );
		static wxString PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_DATA4 = _( "Deferred Data 4" );
		static wxString PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_DATA5 = _( "Deferred Data 5" );
	}

	RenderTargetTreeItemProperty::RenderTargetTreeItemProperty( bool editable
		, RenderTargetSPtr target )
		: TreeItemProperty( target->getEngine(), editable, ePROPERTY_DATA_TYPE_RENDER_TARGET )
		, m_target( target )
	{
		PROPERTY_CATEGORY_RENDER_TARGET = _( "Render Target: " );
		PROPERTY_RENDER_TARGET_SHADER = _( "Shader" );
		PROPERTY_RENDER_TARGET_EDIT_SHADER = _( "View Shaders..." );
		PROPERTY_RENDER_TARGET_SSAO = _( "SSAO" );
		PROPERTY_RENDER_TARGET_SSAO_ENABLED = _( "Enabled" );
		PROPERTY_RENDER_TARGET_SSAO_HIGH_QUALITY = _( "High Quality" );
		PROPERTY_RENDER_TARGET_SSAO_NORMALS_BUFFER = _( "Normals Buffer" );
		PROPERTY_RENDER_TARGET_SSAO_RADIUS = _( "Radius" );
		PROPERTY_RENDER_TARGET_SSAO_BIAS = _( "Bias" );
		PROPERTY_RENDER_TARGET_SSAO_INTENSITY = _( "Intensity" );
		PROPERTY_RENDER_TARGET_SSAO_SAMPLES = _( "Samples" );
		PROPERTY_RENDER_TARGET_SSAO_EDGE_SHARPNESS = _( "Edge Sharpness" );
		PROPERTY_RENDER_TARGET_SSAO_BLUR_HIGH_QUALITY = _( "High Quality Blur" );
		PROPERTY_RENDER_TARGET_SSAO_BLUR_STEP_SIZE = _( "Blur Step Size" );
		PROPERTY_RENDER_TARGET_SSAO_BLUR_RADIUS = _( "Blur Radius" );
		PROPERTY_RENDER_TARGET_DEBUG = _( "Debug Views" );
		PROPERTY_RENDER_TARGET_DEBUG_LIGHT_EYE = _( "Light Eye" );
		PROPERTY_RENDER_TARGET_DEBUG_LIGHT_VSPOSITION = _( "Light VS Position" );
		PROPERTY_RENDER_TARGET_DEBUG_LIGHT_WSPOSITION = _( "Light WS Position" );
		PROPERTY_RENDER_TARGET_DEBUG_LIGHT_WSNORMAL = _( "Light WS Normal" );
		PROPERTY_RENDER_TARGET_DEBUG_LIGHT_TEXCOORD = _( "Light Tex Coord" );
		PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_DIFFUSELIGHTING = _( "Deferred Diffuse Lighting" );
		PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_SPECULARLIGHTING = _( "Deferred Specular Lighting" );
		PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_OCCLUSION = _( "Deferred Occlusion" );
		PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_SSSTRANSMITTANCE = _( "Deferred SSS Transmittance" );
		PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_IBL = _( "Deferred IBL" );
		PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_NORMALS = _( "Deferred Normals" );
		PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_WORLDPOS = _( "Deferred World Pos" );
		PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_VIEWPOS = _( "Deferred View Pos" );
		PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_DEPTH = _( "Deferred Depth" );
		PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_DATA1 = _( "Deferred Data 1" );
		PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_DATA2 = _( "Deferred Data 2" );
		PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_DATA3 = _( "Deferred Data 3" );
		PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_DATA4 = _( "Deferred Data 4" );
		PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_DATA5 = _( "Deferred Data 5" );

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

			grid->Append( new wxPropertyCategory( PROPERTY_RENDER_TARGET_DEBUG ) );
			auto & debugConfig = target->getTechnique()->getDebugConfig();
			grid->Append( new wxBoolProperty( PROPERTY_RENDER_TARGET_DEBUG_LIGHT_EYE, PROPERTY_RENDER_TARGET_DEBUG_LIGHT_EYE, debugConfig.debugLightEye ) );
			grid->Append( new wxBoolProperty( PROPERTY_RENDER_TARGET_DEBUG_LIGHT_VSPOSITION, PROPERTY_RENDER_TARGET_DEBUG_LIGHT_VSPOSITION, debugConfig.debugLightVSPosition ) );
			grid->Append( new wxBoolProperty( PROPERTY_RENDER_TARGET_DEBUG_LIGHT_WSPOSITION, PROPERTY_RENDER_TARGET_DEBUG_LIGHT_WSPOSITION, debugConfig.debugLightWSPosition ) );
			grid->Append( new wxBoolProperty( PROPERTY_RENDER_TARGET_DEBUG_LIGHT_WSNORMAL, PROPERTY_RENDER_TARGET_DEBUG_LIGHT_WSNORMAL, debugConfig.debugLightWSNormal ) );
			grid->Append( new wxBoolProperty( PROPERTY_RENDER_TARGET_DEBUG_LIGHT_TEXCOORD, PROPERTY_RENDER_TARGET_DEBUG_LIGHT_TEXCOORD, debugConfig.debugLightTexCoord ) );
			grid->Append( new wxBoolProperty( PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_DIFFUSELIGHTING, PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_DIFFUSELIGHTING, debugConfig.debugDeferredDiffuseLighting ) );
			grid->Append( new wxBoolProperty( PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_SPECULARLIGHTING, PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_SPECULARLIGHTING, debugConfig.debugDeferredSpecularLighting ) );
			grid->Append( new wxBoolProperty( PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_OCCLUSION, PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_OCCLUSION, debugConfig.debugDeferredOcclusion ) );
			grid->Append( new wxBoolProperty( PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_SSSTRANSMITTANCE, PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_SSSTRANSMITTANCE, debugConfig.debugDeferredSSSTransmittance ) );
			grid->Append( new wxBoolProperty( PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_IBL, PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_IBL, debugConfig.debugDeferredIBL ) );
			grid->Append( new wxBoolProperty( PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_NORMALS, PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_NORMALS, debugConfig.debugDeferredNormals ) );
			grid->Append( new wxBoolProperty( PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_WORLDPOS, PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_WORLDPOS, debugConfig.debugDeferredWorldPos ) );
			grid->Append( new wxBoolProperty( PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_VIEWPOS, PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_VIEWPOS, debugConfig.debugDeferredViewPos ) );
			grid->Append( new wxBoolProperty( PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_DEPTH, PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_DEPTH, debugConfig.debugDeferredDepth ) );
			grid->Append( new wxBoolProperty( PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_DATA1, PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_DATA1, debugConfig.debugDeferredData1 ) );
			grid->Append( new wxBoolProperty( PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_DATA2, PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_DATA2, debugConfig.debugDeferredData2 ) );
			grid->Append( new wxBoolProperty( PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_DATA3, PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_DATA3, debugConfig.debugDeferredData3 ) );
			grid->Append( new wxBoolProperty( PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_DATA4, PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_DATA4, debugConfig.debugDeferredData4 ) );
			grid->Append( new wxBoolProperty( PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_DATA5, PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_DATA5, debugConfig.debugDeferredData5 ) );
		}
	}

	void RenderTargetTreeItemProperty::doPropertyChange( wxPropertyGridEvent & event )
	{
		RenderTargetSPtr target = getRenderTarget();
		wxPGProperty * property = event.GetProperty();

		if ( property && target )
		{
			auto & ssaoConfig = target->getTechnique()->getSsaoConfig();
			auto & debugConfig = target->getTechnique()->getDebugConfig();

			if ( property->GetName() == PROPERTY_RENDER_TARGET_SSAO_ENABLED )
			{
				onValueChange( bool( property->GetValue() ), &ssaoConfig.enabled );
			}
			else if ( property->GetName() == PROPERTY_RENDER_TARGET_SSAO_HIGH_QUALITY )
			{
				onValueChange( bool( property->GetValue() ), &ssaoConfig.highQuality );
			}
			else if ( property->GetName() == PROPERTY_RENDER_TARGET_SSAO_NORMALS_BUFFER )
			{
				onValueChange( bool( property->GetValue() ), &ssaoConfig.useNormalsBuffer );
			}
			else if ( property->GetName() == PROPERTY_RENDER_TARGET_SSAO_RADIUS )
			{
				onValueChange( float( property->GetValue().GetDouble() ), &ssaoConfig.radius );
			}
			else if ( property->GetName() == PROPERTY_RENDER_TARGET_SSAO_BIAS )
			{
				onValueChange( float( property->GetValue().GetDouble() ), &ssaoConfig.bias );
			}
			else if ( property->GetName() == PROPERTY_RENDER_TARGET_SSAO_INTENSITY )
			{
				onValueChange( float( property->GetValue().GetDouble() ), &ssaoConfig.intensity );
			}
			else if ( property->GetName() == PROPERTY_RENDER_TARGET_SSAO_SAMPLES )
			{
				onValueChange( uint32_t( property->GetValue().GetLong() ), &ssaoConfig.numSamples );
			}
			else if ( property->GetName() == PROPERTY_RENDER_TARGET_SSAO_EDGE_SHARPNESS )
			{
				onValueChange( float( property->GetValue().GetDouble() ), &ssaoConfig.edgeSharpness );
			}
			else if ( property->GetName() == PROPERTY_RENDER_TARGET_SSAO_BLUR_HIGH_QUALITY )
			{
				onValueChange( bool( property->GetValue() ), &ssaoConfig.blurHighQuality );
			}
			else if ( property->GetName() == PROPERTY_RENDER_TARGET_SSAO_BLUR_STEP_SIZE )
			{
				onValueChange( uint32_t( property->GetValue().GetLong() ), &ssaoConfig.blurStepSize );
			}
			else if ( property->GetName() == PROPERTY_RENDER_TARGET_SSAO_BLUR_RADIUS )
			{
				onValueChange( int32_t( property->GetValue().GetLong() ), &ssaoConfig.blurRadius );
			}
			else if ( property->GetName() == PROPERTY_RENDER_TARGET_DEBUG_LIGHT_EYE )
			{
				onValueChange( property->GetValue(), &debugConfig.debugLightEye );
			}
			else if ( property->GetName() == PROPERTY_RENDER_TARGET_DEBUG_LIGHT_VSPOSITION )
			{
				onValueChange( property->GetValue(), &debugConfig.debugLightVSPosition );
			}
			else if ( property->GetName() == PROPERTY_RENDER_TARGET_DEBUG_LIGHT_WSPOSITION )
			{
				onValueChange( property->GetValue(), &debugConfig.debugLightWSPosition );
			}
			else if ( property->GetName() == PROPERTY_RENDER_TARGET_DEBUG_LIGHT_WSNORMAL )
			{
				onValueChange( property->GetValue(), &debugConfig.debugLightWSNormal );
			}
			else if ( property->GetName() == PROPERTY_RENDER_TARGET_DEBUG_LIGHT_TEXCOORD )
			{
				onValueChange( property->GetValue(), &debugConfig.debugLightTexCoord );
			}
			else if ( property->GetName() == PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_DIFFUSELIGHTING )
			{
				onValueChange( property->GetValue(), &debugConfig.debugDeferredDiffuseLighting );
			}
			else if ( property->GetName() == PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_SPECULARLIGHTING )
			{
				onValueChange( property->GetValue(), &debugConfig.debugDeferredSpecularLighting );
			}
			else if ( property->GetName() == PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_OCCLUSION )
			{
				onValueChange( property->GetValue(), &debugConfig.debugDeferredOcclusion );
			}
			else if ( property->GetName() == PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_SSSTRANSMITTANCE )
			{
				onValueChange( property->GetValue(), &debugConfig.debugDeferredSSSTransmittance );
			}
			else if ( property->GetName() == PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_IBL )
			{
				onValueChange( property->GetValue(), &debugConfig.debugDeferredIBL );
			}
			else if ( property->GetName() == PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_NORMALS )
			{
				onValueChange( property->GetValue(), &debugConfig.debugDeferredNormals );
			}
			else if ( property->GetName() == PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_WORLDPOS )
			{
				onValueChange( property->GetValue(), &debugConfig.debugDeferredWorldPos );
			}
			else if ( property->GetName() == PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_VIEWPOS )
			{
				onValueChange( property->GetValue(), &debugConfig.debugDeferredViewPos );
			}
			else if ( property->GetName() == PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_DEPTH )
			{
				onValueChange( property->GetValue(), &debugConfig.debugDeferredDepth );
			}
			else if ( property->GetName() == PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_DATA1 )
			{
				onValueChange( property->GetValue(), &debugConfig.debugDeferredData1 );
			}
			else if ( property->GetName() == PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_DATA2 )
			{
				onValueChange( property->GetValue(), &debugConfig.debugDeferredData2 );
			}
			else if ( property->GetName() == PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_DATA3 )
			{
				onValueChange( property->GetValue(), &debugConfig.debugDeferredData3 );
			}
			else if ( property->GetName() == PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_DATA4 )
			{
				onValueChange( property->GetValue(), &debugConfig.debugDeferredData4 );
			}
			else if ( property->GetName() == PROPERTY_RENDER_TARGET_DEBUG_DEFERRED_DATA5 )
			{
				onValueChange( property->GetValue(), &debugConfig.debugDeferredData5 );
			}
		}
	}
}
