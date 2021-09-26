#include "GuiCommon/Properties/TreeItems/SsaoConfigTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/Ssao/SsaoConfig.hpp>
#include <Castor3D/Render/Technique/RenderTechnique.hpp>

#include <wx/propgrid/advprops.h>

namespace GuiCommon
{
	//******************************************************************************************   

	namespace
	{
		static wxString PROPERTY_SSAO = _( "SSAO Configuration" );
	}

	//******************************************************************************************   

	SsaoConfigTreeItemProperty::SsaoConfigTreeItemProperty( bool editable
		, castor3d::Engine * engine
		, castor3d::SsaoConfig & config )
		: TreeItemProperty{ engine, editable }
		, m_config{ config }
	{
		PROPERTY_SSAO = _( "SSAO Configuration" );
		CreateTreeItemMenu();
	}

	void SsaoConfigTreeItemProperty::doCreateProperties( wxPGEditor * editor
		, wxPropertyGrid * grid )
	{
		static wxString PROPERTY_SSAO_ENABLED = _( "Enable SSAO" );
		static wxString PROPERTY_SSAO_HIGH_QUALITY = _( "High Quality" );
		static wxString PROPERTY_SSAO_NORMALS_BUFFER = _( "Normals Buffer" );
		static wxString PROPERTY_SSAO_RADIUS = _( "Radius" );
		static wxString PROPERTY_SSAO_BIAS = _( "Bias" );
		static wxString PROPERTY_SSAO_INTENSITY = _( "Intensity" );
		static wxString PROPERTY_SSAO_SAMPLES = _( "Samples" );
		static wxString PROPERTY_SSAO_EDGE_SHARPNESS = _( "Edge Sharpness" );
		static wxString PROPERTY_SSAO_BLUR_HIGH_QUALITY = _( "High Quality Blur" );
		static wxString PROPERTY_SSAO_BLUR_STEP_SIZE = _( "Blur Step Size" );
		static wxString PROPERTY_SSAO_BLUR_RADIUS = _( "Blur Radius" );
		static wxString PROPERTY_SSAO_BEND_STEP_COUNT = _( "Bend Step Count" );
		static wxString PROPERTY_SSAO_BEND_STEP_SIZE = _( "Bend Step Size" );

		addProperty( grid, PROPERTY_SSAO );
		addPropertyT( grid, PROPERTY_SSAO_ENABLED, &m_config.enabled );
		addPropertyT( grid, PROPERTY_SSAO_HIGH_QUALITY, &m_config.highQuality );
		addPropertyT( grid, PROPERTY_SSAO_NORMALS_BUFFER, &m_config.useNormalsBuffer );
		addPropertyT( grid, PROPERTY_SSAO_RADIUS, &m_config.radius );
		addPropertyT( grid, PROPERTY_SSAO_BIAS, &m_config.bias );
		addPropertyT( grid, PROPERTY_SSAO_INTENSITY, &m_config.intensity );
		addPropertyT( grid, PROPERTY_SSAO_SAMPLES, &m_config.numSamples );
		addPropertyT( grid, PROPERTY_SSAO_EDGE_SHARPNESS, &m_config.edgeSharpness, 0.1f );
		addPropertyT( grid, PROPERTY_SSAO_BLUR_HIGH_QUALITY, &m_config.blurHighQuality );
		addPropertyT( grid, PROPERTY_SSAO_BLUR_STEP_SIZE, &m_config.blurStepSize );
		addPropertyT( grid, PROPERTY_SSAO_BLUR_RADIUS, &m_config.blurRadius );
		addPropertyT( grid, PROPERTY_SSAO_BEND_STEP_COUNT, &m_config.bendStepCount );
		addPropertyT( grid, PROPERTY_SSAO_BEND_STEP_SIZE, &m_config.bendStepSize );
	}

	//*********************************************************************************************
}
