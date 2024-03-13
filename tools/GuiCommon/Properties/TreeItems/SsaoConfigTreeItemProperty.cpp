#include "GuiCommon/Properties/TreeItems/SsaoConfigTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"
#include "GuiCommon/Properties/TreeItems/TreeItemConfigurationBuilder.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/Ssao/SsaoConfig.hpp>
#include <Castor3D/Render/RenderTechnique.hpp>

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

	void SsaoConfigTreeItemProperty::doCreateProperties( wxPropertyGrid * grid )
	{
		TreeItemConfigurationBuilder::submit( grid, *this, m_config );
	}

	//*********************************************************************************************
}
